#include "stdafx.h"




bool CLuaThread::Init(CLuaScript * pLuaScript, lua_State * pLuaState, UINT ScriptID)
{
	m_pLuaScript = pLuaScript;
	m_ScriptID = ScriptID;
	//m_pLuaStatus = pLuaState;
	m_pLuaThread = lua_newthread(pLuaState);	
	return true;
}
void CLuaThread::Release()
{
	if (m_pLuaScript)
	{
		m_pLuaScript->DeleteScriptThread(this);
	}
}
int CLuaThread::PushPacket(const CSmartStruct& Packet)
{
	int Count = 0;
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		switch (Value.GetType())
		{
		case CSmartValue::VT_CHAR:
			lua_pushnil(m_pLuaThread);
			Count++;
			break;
		case CSmartValue::VT_UCHAR:
		case CSmartValue::VT_SHORT:
		case CSmartValue::VT_USHORT:
		case CSmartValue::VT_INT:
		case CSmartValue::VT_UINT:
		case CSmartValue::VT_FLOAT:
		case CSmartValue::VT_DOUBLE:
			PushValue((double)Value);
			Count++;
			break;
		case CSmartValue::VT_UBIGINT:
		case CSmartValue::VT_BIGINT:
			PushValue((INT64)Value);
			Count++;
			break;		
		case CSmartValue::VT_STRING:
			PushValue((LPCTSTR)Value);
			Count++;
			break;
		case CSmartValue::VT_STRUCT:
			if (PushTablePacket(Value))
				Count++;
			break;
		case CSmartValue::VT_BOOL:
			PushValue((bool)Value);
			Count++;
			break;
		}
	}
	return Count;
}
bool CLuaThread::PushTablePacket(const CSmartStruct& Packet)
{
	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID;
		CSmartValue Value = Packet.GetNextMember(Pos, MemberID);
		switch (Value.GetType())
		{
		case CSmartValue::VT_CHAR:
			lua_pushnil(m_pLuaThread);
			break;
		case CSmartValue::VT_UCHAR:
		case CSmartValue::VT_SHORT:
		case CSmartValue::VT_USHORT:
		case CSmartValue::VT_INT:
		case CSmartValue::VT_UINT:
		case CSmartValue::VT_FLOAT:
		case CSmartValue::VT_DOUBLE:
			PushValue((double)Value);
			break;
		case CSmartValue::VT_BIGINT:
		case CSmartValue::VT_UBIGINT:
			PushValue((INT64)Value);
			break;		
		case CSmartValue::VT_STRING:
			PushValue((LPCTSTR)Value);
			break;
		case CSmartValue::VT_STRUCT:
			if (!PushTablePacket(Value))
			{
				Log(_T("CLuaThread::PushTablePacket:Push Table Value Failed"));
			}
			break;
		case CSmartValue::VT_BOOL:
			PushValue((bool)Value);
			break;
		}
	}
	return true;
}
bool CLuaThread::Prepare(CBaseScriptHost * pObject, LPCSTR szFunctionName)
{
	Reset();	
	m_pHostObject = pObject;
	lua_pushlightuserdata(m_pLuaThread, (void*)pObject);
	lua_pushlightuserdata(m_pLuaThread, (void*)this);
	lua_getglobal(m_pLuaThread, szFunctionName);

	if (!lua_isfunction(m_pLuaThread, -1))
	{
		lua_pop(m_pLuaThread, 1);
		Log(_T("CLuaScriptPool::InitLuaThread:Lua函数%s未找到"),szFunctionName);
		return false;
	}

	m_StackInitSize = lua_gettop(m_pLuaThread);	
	return true;
}


int CLuaThread::DoCall(int ParamCount)
{
	if (m_IsNeedYield&&m_YeildReturnCount != ParamCount)
	{
		Log(_T("CLuaThread::DoCall:YeildReturnCount Error Need %d Have %d"), m_YeildReturnCount, ParamCount);
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
			Log(_T("脚本执行出错:%s"), lua_tolstring(m_pLuaThread, -1, NULL));
		else
			Log(_T("脚本执行出错"));

	}

	m_YieldType = LUA_YIELD_TYPE_NONE;
	
	return m_LastLuaStatus;
}

LPCSTR CLuaThread::GetErrorStringA()
{
	if (lua_type(m_pLuaThread, -1) == LUA_TSTRING)
	{
		return lua_tolstring(m_pLuaThread, -1, NULL);
	}
	return "";
}

bool CLuaThread::PackResult(CSmartStruct& Packet)
{
	if (m_LastLuaStatus == LUA_OK)
	{
		WORD Index = 1;
		while (lua_gettop(m_pLuaThread)>m_StackInitSize-1)
		{
			switch (LuaWrap::GetLuaObjectType(m_pLuaThread, -1))
			{
			case LUA_TNIL:
				Packet.AddMember(Index, (char)0);
				break;
			case LUA_TBOOLEAN:
				Packet.AddMember(Index, lua_toboolean(m_pLuaThread, -1));
				break;
			case LUA_TNUMBER:				
				Packet.AddMember(Index, lua_tonumber(m_pLuaThread, -1));
				break;
			case LUA_TSTRING:
				Packet.AddMember(Index, lua_tostring(m_pLuaThread, -1));
				break;
			case LUA_TTABLE:
				{
					UINT BufferSize;
					void * pBuffer = Packet.PrepareMember(BufferSize);
					CSmartStruct SubPacket(pBuffer, BufferSize, true);
					if (!PackTable(SubPacket, -1))
						return false;
					Packet.FinishMember(Index, SubPacket.GetDataLen());
				}
				break;
			case LUA_TINTEGER:
				Packet.AddMember(Index, lua_tointeger(m_pLuaThread, -1));
				break;
			}
			Index++;
			lua_pop(m_pLuaThread, 1);
		}
		return true;
	}
	return false;
}
bool CLuaThread::PackTable(CSmartStruct& Packet, int Index)
{
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
		switch (LuaWrap::GetLuaObjectType(m_pLuaThread, -2))
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
		switch (LuaWrap::GetLuaObjectType(m_pLuaThread, -1))
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
	int ParamCount = 0;
	while (GetResultCount())
	{
		int Type = LuaWrap::GetLuaObjectType(m_pLuaThread, -1);
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
int CLuaThread::CopyStack(CLuaThread * pTargetLThread, int StartIndex, int Count, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN)
{
	if (StartIndex <= 0 || Count<=0)
		return 0;
	if (lua_gettop(m_pLuaThread) < Count - StartIndex + 1)
		return 0;
	int CopyCount = 0;
	for (int i = 0; i < Count; i++)
	{
		int Type = LuaWrap::GetLuaObjectType(m_pLuaThread, StartIndex + i);
		switch (Type)
		{
		case LUA_TNIL:
			pTargetLThread->PushNil();
			CopyCount++;
			break;
		case LUA_TBOOLEAN:
			pTargetLThread->PushValue(GetValue<bool>(StartIndex + i));
			CopyCount++;
			break;
		case LUA_TNUMBER:
			pTargetLThread->PushValue(GetValue<double>(StartIndex + i));
			CopyCount++;
			break;
		case LUA_TSTRING:
			pTargetLThread->PushValue(GetValue<LPCTSTR>(StartIndex + i));
			CopyCount++;
			break;
		case LUA_TTABLE:
			pTargetLThread->PushValue(GetValue<CLuaTable>(StartIndex + i));
			CopyCount++;
			break;
		case LUA_TINTEGER:
			pTargetLThread->PushValue(GetValue<__int64>(StartIndex + i));
			CopyCount++;
			break;
		default:
			if (pExtendCopyFN)
			{
				if (pExtendCopyFN(Type, this, StartIndex + i,pTargetLThread))
				{
					CopyCount++;
				}
			}
		}
	}
	return CopyCount;
}

void CLuaThread::Update()
{
	if (m_LastLuaStatus == LUA_YIELD)
	{
		if (m_YieldType == LUA_YIELD_TYPE_SLEEP)
		{
			if (m_SleepTimer.IsTimeOut())
			{
				lua_pushnumber(m_pLuaThread, 0);
				DoCall(1);				
			}
		}
		else if (m_YieldTimer.IsTimeOut(LUA_YIELD_TIME_OUT))
		{
			Log(_T("CLuaThread::Update:脚本中断超时"));
			DoCall(0);
		}
	}
}