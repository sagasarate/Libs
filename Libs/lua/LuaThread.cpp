#include "stdafx.h"




bool CLuaThread::Init(CLuaScript * pLuaScript, UINT ScriptID, int StackSize)
{
	m_pLuaScript = pLuaScript;
	m_ScriptID = ScriptID;
	m_pLuaThread = lua_newthread(m_pLuaScript->GetLuaState());
	lua_checkstack(m_pLuaThread, StackSize);
	return true;
}
//void CLuaThread::Destory()
//{
//	if (m_IsInExec)
//	{
//		LogLua(_T("Release In Exec"));
//	}	
//	m_pLuaScript = NULL;
//	m_ScriptID = 0;
//	m_Param1 = NULL;
//	m_Param2 = NULL;
//	m_pHostObject = NULL;
//	m_pLuaThread = NULL;
//	m_ThreadType = 0;
//	m_IsInExec = false;
//	m_IsNeedYield = false;
//	m_LastLuaStatus = LUA_OK;
//	m_YieldType = LUA_YIELD_TYPE_NONE;
//	m_YeildReturnCount = 0;
//	m_pTimeManager = NULL;
//	m_SleepTime = 0;
//	m_SleepPastTime = 0;
//}
void CLuaThread::Release()
{
	if (m_IsInExec)
	{
		LogLua(_T("Release In Exec"));
		ClearYeild();
	}
	else if (m_pLuaScript)
	{
		if (m_pLuaThread)
		{
			lua_reset_state(m_pLuaThread);
		}
		m_pLuaScript->DeleteScriptThread(this);
	}
}
bool CLuaThread::PushPacketValue(const CSmartValue& Value)
{
	if (m_pLuaThread == NULL)
		return false;
	switch (Value.GetType())
	{
	case CSmartValue::VT_CHAR:
		lua_pushnil(m_pLuaThread);
		return true;
	case CSmartValue::VT_UCHAR:
	case CSmartValue::VT_SHORT:
	case CSmartValue::VT_USHORT:
	case CSmartValue::VT_INT:
	case CSmartValue::VT_UINT:
	case CSmartValue::VT_FLOAT:
	case CSmartValue::VT_DOUBLE:
		PushValue((double)Value);
		return true;
	case CSmartValue::VT_UBIGINT:
	case CSmartValue::VT_BIGINT:
		PushValue((INT64)Value);
		return true;
	case CSmartValue::VT_STRING:
		PushValue((LPCSTR)Value);
		return true;
	case CSmartValue::VT_USTRING:
		PushValue((LPCWSTR)Value);
		return true;
	case CSmartValue::VT_STRUCT:
		return PushTablePacket(Value);
	case CSmartValue::VT_BOOL:
		PushValue((bool)Value);
		return true;
	}
	return false;
}
int CLuaThread::PushPacket(const CSmartStruct& Packet)
{
	int Count = 0;
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		if (PushPacketValue(Value))
			Count++;
	}
	return Count;
}
bool CLuaThread::PushTablePacket(const CSmartStruct& Packet)
{
	if (m_pLuaThread == NULL)
		return false;

	lua_newtable(m_pLuaThread);

	CSmartValue TableKey;
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		if (MemberID == SSID_LUA_VALUE_TABLE_KEY)
		{
			TableKey = Value;
		}
		else if (MemberID == SSID_LUA_VALUE_TABLE_VALUE)
		{
			if (!PushPacketValue(TableKey))
				break;
			if (!PushPacketValue(Value))
			{
				lua_pop(m_pLuaThread, 1);
				break;
			}
			lua_settable(m_pLuaThread, -3);
		}		
	}
	return true;
}
bool CLuaThread::Prepare(CBaseScriptHost * pObject, LPCSTR szFunctionName)
{
	if (m_pLuaThread == NULL)
		return false;

	m_pHostObject = pObject;
	lua_pushlightuserdata(m_pLuaThread, (void*)pObject);
	lua_pushlightuserdata(m_pLuaThread, (void*)this);
	lua_getglobal(m_pLuaThread, szFunctionName);

	if (!lua_isfunction(m_pLuaThread, -1))
	{
		lua_pop(m_pLuaThread, 3);
		LogLua(_T("CLuaScriptPool::InitLuaThread:Lua函数%s未找到"),szFunctionName);
		return false;
	}

	return true;
}


int CLuaThread::DoCall(int ParamCount)
{
	if (m_pLuaThread)
	{
		if (!m_IsInExec)
		{
			if (m_IsNeedYield&&m_YeildReturnCount != ParamCount)
			{
				LogLua(_T("CLuaThread::DoCall:YeildReturnCount Error Need %d Have %d"), m_YeildReturnCount, ParamCount);
			}
			m_IsNeedYield = false;
			m_IsInExec = true;
			m_LastLuaStatus = lua_resume(m_pLuaThread, NULL, ParamCount);
			m_IsInExec = false;
			if (m_LastLuaStatus == LUA_OK)
			{

			}
			else if (m_LastLuaStatus == LUA_YIELD)
			{
				return LUA_YIELD;
			}
			else
			{
				if (lua_type(m_pLuaThread, -1) == LUA_TSTRING)
					LogLua(_T("脚本执行出错:%s"), lua_tolstring(m_pLuaThread, -1, NULL));
				else
					LogLua(_T("脚本执行出错"));

			}
		}
		else
		{
			LogLua(_T("DoCall in another call"));
		}
	}
	else
	{
		LogLua(_T("CLuaThread::DoCall:Thread not init"));
		m_LastLuaStatus = LUA_ERRERR;
	}	

	m_YieldType = LUA_YIELD_TYPE_NONE;
	
	return m_LastLuaStatus;
}

LPCSTR CLuaThread::GetErrorStringA()
{
	if (m_pLuaThread == NULL)
		return "";
	if (lua_type(m_pLuaThread, -1) == LUA_TSTRING)
	{
		return lua_tolstring(m_pLuaThread, -1, NULL);
	}
	return "";
}

bool CLuaThread::PackResult(CSmartStruct& Packet)
{
	if (m_pLuaThread == NULL)
		return false;
	if (m_LastLuaStatus == LUA_OK)
	{
		int Count = lua_gettop(m_pLuaThread) - 2;
		if (Count>0)
		{
			for (WORD Index = 3; Index < Count + 3; Index++)
			{
				switch (GetLuaObjectType(m_pLuaThread, Index))
				{
				case LUA_TNIL:
					Packet.AddMember(Index, (char)0);
					break;
				case LUA_TBOOLEAN:
					Packet.AddMember(Index, lua_toboolean(m_pLuaThread, Index));
					break;
				case LUA_TNUMBER:
					Packet.AddMember(Index, lua_tonumber(m_pLuaThread, Index));
					break;
				case LUA_TSTRING:
					Packet.AddMember(Index, lua_tostring(m_pLuaThread, Index));
					break;
				case LUA_TTABLE:
				{
					UINT BufferSize;
					void * pBuffer = Packet.PrepareMember(BufferSize);
					CSmartStruct SubPacket(pBuffer, BufferSize, true);
					if (!PackTable(SubPacket, Index))
						return false;
					Packet.FinishMember(Index, SubPacket.GetDataLen());
				}
				break;
				case LUA_TINTEGER:
					Packet.AddMember(Index, lua_tointeger(m_pLuaThread, Index));
					break;
				}
			}
			lua_pop(m_pLuaThread, Count);
		}
		return true;
	}
	return false;
}
bool CLuaThread::PackTable(CSmartStruct& Packet, int Index)
{
	if (m_pLuaThread == NULL)
		return false;

	if (!lua_istable(m_pLuaThread, Index))
	{		
		return false;
	}
	lua_pushnil(m_pLuaThread);
	if (Index < 0)
		Index--;
	while (lua_next(m_pLuaThread, Index) != 0)
	{	
		//打包Key
		switch (GetLuaObjectType(m_pLuaThread, -2))
		{
		case LUA_TNIL:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, (char)0);
			break;
		case LUA_TBOOLEAN:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_toboolean(m_pLuaThread, -2));
			break;
		case LUA_TNUMBER:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tonumber(m_pLuaThread, -2));
			break;
		case LUA_TSTRING:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tostring(m_pLuaThread, -2));
			break;		
		case LUA_TINTEGER:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tointeger(m_pLuaThread, -2));
			break;
		default:
			//对于不支持的类型，一律当作null
			Packet.AddMember(SSID_LUA_VALUE_TABLE_KEY, (char)0);
			break;
		}
		//打包Value
		switch (GetLuaObjectType(m_pLuaThread, -1))
		{
		case LUA_TNIL:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, (char)0);
			break;
		case LUA_TBOOLEAN:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_toboolean(m_pLuaThread, -1));
			break;
		case LUA_TNUMBER:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tonumber(m_pLuaThread, -1));
			break;
		case LUA_TSTRING:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tostring(m_pLuaThread, -1));
			break;
		case LUA_TTABLE:
			{
				UINT BufferSize;
				void * pBuffer = Packet.PrepareMember(BufferSize);
				CSmartStruct SubPacket(pBuffer, BufferSize, true);
				if (!PackTable(SubPacket, -1))
				{
					lua_pop(m_pLuaThread, 2);
					return false;
				}
				Packet.FinishMember(SSID_LUA_VALUE_TABLE_VALUE, SubPacket.GetDataLen());
			}			
			break;
		case LUA_TINTEGER:
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tointeger(m_pLuaThread, -1));
			break;
		default:
			//对于不支持的类型，一律当作null
			Packet.AddMember(SSID_LUA_VALUE_TABLE_VALUE, (char)0);
			break;
		}		
		lua_pop(m_pLuaThread, 1);
	}	
	return true;
}
int CLuaThread::MoveResultToLThread(CLuaThread * pLThread, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN)
{
	if (m_pLuaThread == NULL)
		return 0;
	int ParamCount = 0;
	while (GetResultCount())
	{
		int Type = GetLuaObjectType(m_pLuaThread, -1);
		switch (Type)
		{
		case LUA_TNIL:
			pLThread->PushNil();
			ParamCount++;
			break;
		case LUA_TBOOLEAN:
			pLThread->PushValue(GetValue<bool>(-1));
			ParamCount++;
			break;
		case LUA_TNUMBER:
			pLThread->PushValue(GetValue<double>(-1));
			ParamCount++;
			break;
		case LUA_TSTRING:
			pLThread->PushValue(GetValue<LPCTSTR>(-1));
			ParamCount++;
			break;
		case LUA_TTABLE:
			pLThread->PushValue(GetValue<CLuaTable>(-1));
			ParamCount++;
			break;
		case LUA_TINTEGER:
			pLThread->PushValue(GetValue<__int64>(-1));
			ParamCount++;
			break;
		default:
			if (pExtendCopyFN)
			{
				if (pExtendCopyFN(Type, this, -1, pLThread))
				{
					ParamCount++;
				}
			}
		}
		lua_pop(m_pLuaThread, 1);
	}
	return ParamCount;
}
//int CLuaThread::CopyStack(CLuaThread * pTargetLThread, int StartIndex, int Count, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN)
//{
//	if (m_pLuaThread == NULL)
//		return 0;
//	if (StartIndex <= 0 || Count<=0)
//		return 0;
//	if (lua_gettop(m_pLuaThread) < Count - StartIndex + 1)
//		return 0;
//	int CopyCount = 0;
//	for (int i = 0; i < Count; i++)
//	{
//		int Type = LuaWrap::GetLuaObjectType(m_pLuaThread, StartIndex + i);
//		switch (Type)
//		{
//		case LUA_TNIL:
//			pTargetLThread->PushNil();
//			CopyCount++;
//			break;
//		case LUA_TBOOLEAN:
//			pTargetLThread->PushValue(GetValue<bool>(StartIndex + i));
//			CopyCount++;
//			break;
//		case LUA_TNUMBER:
//			pTargetLThread->PushValue(GetValue<double>(StartIndex + i));
//			CopyCount++;
//			break;
//		case LUA_TSTRING:
//			pTargetLThread->PushValue(GetValue<LPCTSTR>(StartIndex + i));
//			CopyCount++;
//			break;
//		case LUA_TTABLE:
//			pTargetLThread->PushValue(GetValue<CLuaTable>(StartIndex + i));
//			CopyCount++;
//			break;
//		case LUA_TINTEGER:
//			pTargetLThread->PushValue(GetValue<__int64>(StartIndex + i));
//			CopyCount++;
//			break;
//		default:
//			if (pExtendCopyFN)
//			{
//				if (Type >= CLuaBaseMetaClass::CLASS_ID)
//				{
//					if (pExtendCopyFN(Type, this, StartIndex + i, pTargetLThread))
//					{
//						CopyCount++;
//					}
//				}
//				else if (Type >= CLuaBaseStaticMetaClass::CLASS_ID)
//				{
//					
//				}				
//				
//			}
//		}
//	}
//	return CopyCount;
//}

void CLuaThread::Update()
{
	if (m_LastLuaStatus == LUA_YIELD)
	{
		if (m_YieldType == LUA_YIELD_TYPE_SLEEP)
		{
			if (m_pTimeManager)
			{
				m_SleepPastTime += m_pTimeManager->GetDeltaTime()*m_pTimeManager->GetTimeRate();
				if (m_SleepPastTime >= m_SleepTime)
				{
					m_pTimeManager = NULL;
					m_SleepTime = 0;
					m_SleepPastTime = 0;
					DoCall(0);
				}
			}
			else
			{
				if (m_SleepTimer.IsTimeOut())
				{
					DoCall(0);
				}
			}
		}
		else if (m_YieldTimer.IsTimeOut(LUA_YIELD_TIME_OUT))
		{
			LogLua(_T("CLuaThread::Update:脚本中断超时"));
			m_pTimeManager = NULL;
			m_SleepTime = 0;
			m_SleepPastTime = 0;
			DoCall(0);			
		}
	}
}

bool CLuaThread::IsSiblingThread(CLuaThread * pThread1, CLuaThread * pThread2)
{
	if (pThread1&&pThread2)
	{
		lua_rawgeti(pThread1->GetLuaState(), LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua_rawgeti(pThread2->GetLuaState(), LUA_REGISTRYINDEX, LUA_RIDX_MAINTHREAD);
		lua_State * pMainThread1 = lua_tothread(pThread1->GetLuaState(), -1);
		lua_State * pMainThread2 = lua_tothread(pThread2->GetLuaState(), -1);
		lua_pop(pThread1->GetLuaState(), 1);
		lua_pop(pThread2->GetLuaState(), 1);
		return (pThread1 != NULL) && (pThread1 == pThread2);
	}
	return false;
}