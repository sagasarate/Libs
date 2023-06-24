#include "stdafx.h"


#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif

#include "../../Libs/rapidjson/document.h"
#include "../../Libs/rapidjson/stringbuffer.h"
#include "../../Libs/rapidjson/prettywriter.h"
#include "../../Libs/rapidjson/writer.h"

#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif

bool CLuaThread::Init(CBaseLuaVM * pLuaVM, int StackSize)
{
	m_pLuaVM = pLuaVM;
	m_ThreadType = LUA_THREAD_TYPE_NORMAL;
	lua_getglobal(m_pLuaVM->GetLuaState(), "THREAD_POOL");
	lua_pushinteger(m_pLuaVM->GetLuaState(), GetID());
	m_pLuaState = lua_newthread(m_pLuaVM->GetLuaState());
	lua_settable(m_pLuaVM->GetLuaState(), -3);
	lua_pop(m_pLuaVM->GetLuaState(), 1);
	lua_checkstack(m_pLuaState, StackSize);
	lua_pushlightuserdata(m_pLuaState, (void*)this);
	m_StartStackLen = lua_gettop(m_pLuaState);
	return true;
}
bool CLuaThread::CreateMainThread(CBaseLuaVM* pLuaVM, int StackSize)
{
	m_pLuaVM = pLuaVM;
	m_ThreadType = LUA_THREAD_TYPE_MAIN;
	m_pLuaState = luaL_newstate();
	lua_checkstack(m_pLuaState, StackSize);
	lua_pushlightuserdata(m_pLuaState, (void*)this);
	m_StartStackLen = lua_gettop(m_pLuaState);
	return true;
}
void CLuaThread::Destory()
{
	if (m_LastLuaStatus == LUA_EXECUTING)
	{
		LogLua(_T("Release In Exec"));
	}
	if (m_ThreadType == LUA_THREAD_TYPE_MAIN)
	{
		if (m_pLuaState)
		{
			lua_close(m_pLuaState);
		}
	}
	else
	{
		if (m_pLuaVM)
		{
			//从线程表移除线程
			lua_getglobal(m_pLuaVM->GetLuaState(), "THREAD_POOL");
			lua_pushinteger(m_pLuaVM->GetLuaState(), GetID());
			lua_pushnil(m_pLuaVM->GetLuaState());
			lua_settable(m_pLuaVM->GetLuaState(), -3);
			lua_pop(m_pLuaVM->GetLuaState(), 1);
			m_pLuaVM->DeleteScriptThread(this);
		}
	}
	m_pLuaVM = NULL;
	m_Param1 = NULL;
	m_Param2 = NULL;
	m_pHostObject = NULL;
	m_pLuaState = NULL;
	m_StartStackLen = 0;
	m_ThreadType = LUA_THREAD_TYPE_NORMAL;
	m_IsNeedYield = false;
	m_LastLuaStatus = LUA_OK;
	m_YieldType = LUA_RESERVE_YIELD_TYPE_NONE;
	m_YieldReturnCount = 0;
	m_pTimeManager = NULL;
	m_SleepTime = 0;
	m_SleepPastTime = 0;
}
//void CLuaThread::Release()
//{
//	if (m_IsInExec)
//	{
//		LogLua(_T("Release In Exec"));
//		ClearYield();
//	}
//	else if (m_pLuaVM)
//	{
//		ClearYield();
//		if (m_pLuaThread)
//		{
//			lua_reset_state(m_pLuaThread);			
//		}
//		m_pLuaVM->DeleteScriptThread(this);
//	}
//}
bool CLuaThread::PushPacketValue(const CSmartValue& Value)
{
	if (m_pLuaState == NULL)
	{
		PushNil();
		return false;
	}
	switch (Value.GetType())
	{
	case CSmartValue::VT_NULL:
		lua_pushnil(m_pLuaState);
		break;
	case CSmartValue::VT_CHAR:
	case CSmartValue::VT_UCHAR:
	case CSmartValue::VT_SHORT:
	case CSmartValue::VT_USHORT:
	case CSmartValue::VT_INT:
	case CSmartValue::VT_UINT:	
	case CSmartValue::VT_UBIGINT:
	case CSmartValue::VT_BIGINT:
		PushInteger(Value);
		break;
	case CSmartValue::VT_FLOAT:
	case CSmartValue::VT_DOUBLE:
		PushNumber(Value);
		break;
	case CSmartValue::VT_STRING_UTF8:
	case CSmartValue::VT_STRING_ANSI:
		PushString((LPCSTR)Value);
		break;
	case CSmartValue::VT_STRING_UCS16:
		PushString((LPCWSTR)Value);
		break;
	case CSmartValue::VT_ARRAY:
		return PushTablePacket(Value);
	case CSmartValue::VT_BOOL:
		PushBoolean(Value);
		break;
	default:
		PushNil();
		break;
	}
	return true;
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
bool CLuaThread::PushTablePacket(const CSmartArray& Packet)
{
	if (m_pLuaState == NULL)
	{
		PushNil();
		return false;
	}

	lua_newtable(m_pLuaState);

	void * Pos = Packet.GetFirstMemberPosition();
	while (Pos)
	{		
		CSmartStruct Value = Packet.GetNextMember(Pos);
		CSmartValue TableKey = Value.GetMember(CLuaThread::SSID_LUA_VALUE_TABLE_KEY);
		if (!PushPacketValue(TableKey))
			break;
		CSmartValue TableValue = Value.GetMember(CLuaThread::SSID_LUA_VALUE_TABLE_VALUE);
		if (!PushPacketValue(TableValue))
		{
			lua_pop(m_pLuaState, 1);
			break;
		}
		lua_settable(m_pLuaState, -3);
	}
	return true;
}

bool TableToJson(lua_State* pLuaState, int Index, rapidjson::Value& JsonObject, rapidjson::Document::AllocatorType& Alloc)
{
	if (!lua_istable(pLuaState, Index))
	{
		return false;
	}
	lua_pushnil(pLuaState);  /* 第一个 key */
	if (Index < 0)
		Index--;
	int Next = lua_next(pLuaState, Index);
	if (lua_isinteger(pLuaState, -2))
	{
		JsonObject.SetArray();
		while (Next != 0)
		{
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			int Type = GetLuaObjectType(pLuaState, -1);
			switch (Type)
			{
			case LUA_TNIL:
				JsonObject.PushBack(rapidjson::Value(rapidjson::kNullType), Alloc);
				break;
			case LUA_TBOOLEAN:
				JsonObject.PushBack((lua_toboolean(pLuaState, -1) != 0), Alloc);
				break;
			case LUA_TNUMBER:
				JsonObject.PushBack(lua_tonumber(pLuaState, -1), Alloc);
				break;
			case LUA_TSTRING:
			{
				CEasyString Str;
				LuaStrToSystemStr(lua_tostring(pLuaState, -1), Str);
				JsonObject.PushBack(rapidjson::Value(Str, Alloc), Alloc);
			}
			break;
			case LUA_TTABLE:
			{
				rapidjson::Value SubObject;
				TableToJson(pLuaState, -1, SubObject, Alloc);
				JsonObject.PushBack(SubObject, Alloc);
			}
			break;
			case LUA_TINTEGER:
				JsonObject.PushBack(rapidjson::Value((int64_t)lua_tointeger(pLuaState, -1)), Alloc);
				break;
			}
			lua_pop(pLuaState, 1);

			Next = lua_next(pLuaState, Index);
		}
	}
	else
	{
		JsonObject.SetObject();
		while (Next != 0)
		{
			/* 用一下 'key' （在索引 -2 处） 和 'value' （在索引 -1 处） */
			CEasyString Key;
			LuaStrToSystemStr(lua_tostring(pLuaState, -2), Key);
			int Type = GetLuaObjectType(pLuaState, -1);
			switch (Type)
			{
			case LUA_TNIL:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value(rapidjson::kNullType), Alloc);
				break;
			case LUA_TBOOLEAN:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value((lua_toboolean(pLuaState, -1) != 0)), Alloc);
				break;
			case LUA_TNUMBER:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value(lua_tonumber(pLuaState, -1)), Alloc);
				break;
			case LUA_TSTRING:
			{
				CEasyString Str;
				LuaStrToSystemStr(lua_tostring(pLuaState, -1), Str);
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value(Str, Alloc), Alloc);
			}
			break;
			case LUA_TTABLE:
			{
				rapidjson::Value SubObject;
				TableToJson(pLuaState, -1, SubObject, Alloc);
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), SubObject, Alloc);
			}
			break;
			case LUA_TINTEGER:
				JsonObject.AddMember(rapidjson::Value((LPCTSTR)Key, Alloc), rapidjson::Value((int64_t)lua_tointeger(pLuaState, -1)), Alloc);
				break;
			}
			lua_pop(pLuaState, 1);

			Next = lua_next(pLuaState, Index);
		}
	}

	return true;
}

void JsonToTable(CLuaThread* pLuaThread, rapidjson::Value& JsonObject)
{
	lua_newtable(pLuaThread->GetLuaState());
	if (JsonObject.IsArray())
	{
		for (size_t i = 0; i < JsonObject.Size(); i++)
		{
			rapidjson::Value& Element = JsonObject[(rapidjson::SizeType)i];

			pLuaThread->PushValue((UINT64)(i + 1));

			if (Element.IsBool())
			{
				pLuaThread->PushValue(Element.GetBool());
			}
			else if (Element.IsInt())
			{
				pLuaThread->PushValue(Element.GetInt());
			}
			else if (Element.IsUint())
			{
				pLuaThread->PushValue(Element.GetUint());
			}
			else if (Element.IsInt64())
			{
				pLuaThread->PushValue((INT64)Element.GetInt64());
			}
			else if (Element.IsUint64())
			{
				pLuaThread->PushValue((UINT64)Element.GetUint64());
			}
			else if (Element.IsDouble())
			{
				pLuaThread->PushValue(Element.GetDouble());
			}
			else if (Element.IsString())
			{
				pLuaThread->PushValue(Element.GetString());
			}
			else
			{
				JsonToTable(pLuaThread, Element);
			}
			lua_settable(pLuaThread->GetLuaState(), -3);
		}
	}
	else
	{
		for (rapidjson::Value::MemberIterator Itr = JsonObject.MemberBegin(); Itr != JsonObject.MemberEnd(); Itr++)
		{
			pLuaThread->PushString(Itr->name.GetString());
			if (Itr->value.IsBool())
			{
				pLuaThread->PushValue(Itr->value.GetBool());
			}
			else if (Itr->value.IsInt())
			{
				pLuaThread->PushValue(Itr->value.GetInt());
			}
			else if (Itr->value.IsUint())
			{
				pLuaThread->PushValue(Itr->value.GetUint());
			}
			else if (Itr->value.IsInt64())
			{
				pLuaThread->PushValue((INT64)Itr->value.GetInt64());
			}
			else if (Itr->value.IsUint64())
			{
				pLuaThread->PushValue((UINT64)Itr->value.GetUint64());
			}
			else if (Itr->value.IsDouble())
			{
				pLuaThread->PushValue(Itr->value.GetDouble());
			}
			else if (Itr->value.IsString())
			{
				pLuaThread->PushValue(Itr->value.GetString());
			}
			else
			{
				JsonToTable(pLuaThread, Itr->value);
			}
			lua_settable(pLuaThread->GetLuaState(), -3);
		}
	}
}
bool CLuaThread::PushJson(LPCTSTR JsonStr)
{
	rapidjson::Document Root;
	Root.Parse(JsonStr);
	if (!Root.HasParseError())
	{
		JsonToTable(this, Root);
		return true;
	}
	else
	{
		PushNil();
		return false;
	}
}
bool CLuaThread::GetJson(CEasyString& JsonStr, int Index, bool IsPretty)
{
	if (m_pLuaState == NULL)
		return false;
	if (lua_istable(m_pLuaState, Index))
	{
		if (IsPretty)
		{
			rapidjson::Document Root;
			rapidjson::Document::AllocatorType& Alloc = Root.GetAllocator();

			TableToJson(m_pLuaState, Index, Root, Alloc);

#ifdef UNICODE
			rapidjson::GenericStringBuffer<rapidjson::UTF16<> > buffer;
			rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF16<> >, rapidjson::UTF16<>, rapidjson::UTF16<> > writer(buffer);
#else
			rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
			rapidjson::PrettyWriter<rapidjson::GenericStringBuffer<rapidjson::UTF8<> >, rapidjson::UTF8<>, rapidjson::UTF8<> > writer(buffer);
#endif

			Root.Accept(writer);
			JsonStr = buffer.GetString();
		}
		else
		{
			rapidjson::Document Root;
			rapidjson::Document::AllocatorType& Alloc = Root.GetAllocator();

			TableToJson(m_pLuaState, Index, Root, Alloc);

#ifdef UNICODE
			rapidjson::GenericStringBuffer<rapidjson::UTF16<> > buffer;
			rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF16<> >, rapidjson::UTF16<>, rapidjson::UTF16<> > writer(buffer);
#else
			rapidjson::GenericStringBuffer<rapidjson::UTF8<> > buffer;
			rapidjson::Writer<rapidjson::GenericStringBuffer<rapidjson::UTF8<> >, rapidjson::UTF8<>, rapidjson::UTF8<> > writer(buffer);
#endif

			Root.Accept(writer);
			JsonStr = buffer.GetString();
		}
	}
	return false;
}
bool CLuaThread::Prepare(CBaseScriptHost * pObject, LPCSTR szFunctionName)
{
	if (m_pLuaState == NULL)
	{
		LogLua(_T("线程未初始化"));
		return false;
	}		

	m_pHostObject = pObject;	
	lua_pushlightuserdata(m_pLuaState, (void*)pObject);
	StartCall();
	if(szFunctionName)
	{
		lua_getglobal(m_pLuaState, szFunctionName);

		if (!lua_isfunction(m_pLuaState, -1))
		{
			LogLua(_T("Lua函数%s未找到"), szFunctionName);
			return false;
		}
	}
	return true;
}


int CLuaThread::DoCall(int ParamCount)
{
	if (m_pLuaState)
	{
		if (m_LastLuaStatus != LUA_EXECUTING)
		{
			if (m_IsNeedYield && m_YieldReturnCount != ParamCount)
			{
				LogLua(_T("YeildReturnCount Error Need %d Have %d"), m_YieldReturnCount, ParamCount);
			}			
			m_IsNeedYield = false;
			m_LastLuaStatus = LUA_EXECUTING;
			int nResult = 0;
			m_LastLuaStatus = lua_resume(m_pLuaState, NULL, ParamCount, &nResult);
			if (m_LastLuaStatus == LUA_OK)
			{

			}
			else if (m_LastLuaStatus == LUA_YIELD)
			{
				return LUA_YIELD;
			}
			else
			{
				if (lua_type(m_pLuaState, -1) == LUA_TSTRING)
					LogLuaStr(m_pLuaState, -1, _T("脚本执行出错"), true);
				else
					LogLuaStack(m_pLuaState, _T("脚本执行出错"));
			}
		}
		else
		{
			LogLua(_T("DoCall in another call"));
		}
	}
	else
	{
		LogLua(_T("DoCall:Thread not init"));
		m_LastLuaStatus = LUA_ERRERR;
	}

	m_YieldType = LUA_RESERVE_YIELD_TYPE_NONE;

	return m_LastLuaStatus;
}

int CLuaThread::DoCall(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{
	if (m_pLuaState)
	{
		const char* szScriptName = NULL;
		const char* szScriptContent = NULL;
		size_t ContentLen = 0;
		CEasyStringA LuaStr;
		if (LUA_SCRIPT_CODE_PAGE != CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			//内码转换
			SystemStrToLuaStr(ScriptName, LuaStr);
			szScriptName = LuaStr;
			SystemStrToLuaStr(ScriptContent, LuaStr);
			szScriptContent = LuaStr;
			ContentLen = LuaStr.GetLength();
		}
		else
		{
#ifndef UNICODE
			szScriptName = ScriptName;
			szScriptContent = ScriptContent;
			ContentLen = CEasyString::GetStrLen(szScriptContent);
#endif // !UNICODE
		}
		if (szScriptName && szScriptContent)
		{
			lua_pushcclosure(GetLuaState(), CLuaThread::LuaErrParser, 0);
			int Ret = luaL_loadbuffer(GetLuaState(), szScriptContent, ContentLen, szScriptName);
			if (Ret == LUA_OK)
			{
				Ret = lua_pcall(GetLuaState(), 0, LUA_MULTRET, lua_gettop(GetLuaState()) - 1);
			}
			if (Ret != LUA_OK)			
			{
				if (lua_type(GetLuaState(), -1) == LUA_TSTRING)
				{
					LogLuaStr(GetLuaState(), -1, NULL, false);
				}
				else
				{
					LogLuaDirect(_T("lua load failed"));
				}
			}
			return Ret;
		}
	}
	return LUA_ERRRUN;
}


bool CLuaThread::PackResult(CSmartStruct& Packet, int SkillCount)
{
	if (m_pLuaState == NULL)
		return false;
	if (m_LastLuaStatus == LUA_OK)
	{
		int Count = lua_gettop(m_pLuaState) - SkillCount;
		if (Count>0)
		{
			for (WORD Index = 3; Index < Count + 3; Index++)
			{
				switch (GetLuaObjectType(m_pLuaState, Index))
				{
				case LUA_TNIL:
					Packet.AddMemberNull(Index);
					break;
				case LUA_TBOOLEAN:
					Packet.AddMember(Index, lua_toboolean(m_pLuaState, Index));
					break;
				case LUA_TNUMBER:
					Packet.AddMember(Index, lua_tonumber(m_pLuaState, Index));
					break;
				case LUA_TSTRING:
					Packet.AddMember(Index, lua_tostring(m_pLuaState, Index));
					break;
				case LUA_TTABLE:
				{
					CSmartArray SubPacket = Packet.PrepareSubArray();
					if (!PackTable(SubPacket, Index))
						return false;
					Packet.FinishMember(Index, SubPacket.GetDataLen());
				}
				break;
				case LUA_TINTEGER:
					Packet.AddMember(Index, lua_tointeger(m_pLuaState, Index));
					break;
				}
			}
			lua_pop(m_pLuaState, Count);
		}
		return true;
	}
	return false;
}
bool CLuaThread::PackTable(CSmartArray& Packet, int Index)
{
	if (m_pLuaState == NULL)
		return false;

	if (!lua_istable(m_pLuaState, Index))
	{		
		return false;
	}
	lua_pushnil(m_pLuaState);
	if (Index < 0)
		Index--;
	while (lua_next(m_pLuaState, Index) != 0)
	{	
		bool IsValid = false;
		CSmartStruct Pair = Packet.PrepareSubStruct();
		//打包Key
		switch (GetLuaObjectType(m_pLuaState, -2))
		{
		case LUA_TBOOLEAN:
			Pair.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_toboolean(m_pLuaState, -2) != 0);
			IsValid = true;
			break;
		case LUA_TNUMBER:
			Pair.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tonumber(m_pLuaState, -2));
			IsValid = true;
			break;
		case LUA_TSTRING:
			Pair.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tostring(m_pLuaState, -2));
			IsValid = true;
			break;		
		case LUA_TINTEGER:
			Pair.AddMember(SSID_LUA_VALUE_TABLE_KEY, lua_tointeger(m_pLuaState, -2));
			IsValid = true;
			break;		
		}
		if(IsValid)
		{
			IsValid = false;
			//打包Value
			switch (GetLuaObjectType(m_pLuaState, -1))
			{			
			case LUA_TBOOLEAN:
				Pair.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_toboolean(m_pLuaState, -1) != 0);
				IsValid = true;
				break;
			case LUA_TNUMBER:
				Pair.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tonumber(m_pLuaState, -1));
				IsValid = true;
				break;
			case LUA_TSTRING:
				Pair.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tostring(m_pLuaState, -1));
				IsValid = true;
				break;
			case LUA_TTABLE:
				{
					CSmartArray SubPacket = Pair.PrepareSubArray();
					if (PackTable(SubPacket, -1))
					{
						IsValid = true;
						Pair.FinishMember(SSID_LUA_VALUE_TABLE_VALUE, SubPacket.GetDataLen());
					}				
				}
				break;
			case LUA_TINTEGER:
				Pair.AddMember(SSID_LUA_VALUE_TABLE_VALUE, lua_tointeger(m_pLuaState, -1));
				IsValid = true;
				break;			
			}
			if (IsValid)
				Packet.FinishMember(Pair.GetDataLen());
		}
		lua_pop(m_pLuaState, 1);
	}	
	return true;
}
//int CLuaThread::MoveResultToLThread(CLuaThread * pLThread, LUA_META_CLASS_COPY_FUNCTION pExtendCopyFN)
//{
//	if (m_pLuaState == NULL)
//		return 0;
//	int ParamCount = 0;
//	while (GetResultCount())
//	{
//		int Type = GetLuaObjectType(m_pLuaState, -1);
//		switch (Type)
//		{
//		case LUA_TNIL:
//			pLThread->PushNil();
//			ParamCount++;
//			break;
//		case LUA_TBOOLEAN:
//			pLThread->PushValue(GetValue<bool>(-1));
//			ParamCount++;
//			break;
//		case LUA_TNUMBER:
//			pLThread->PushValue(GetValue<double>(-1));
//			ParamCount++;
//			break;
//		case LUA_TSTRING:
//			pLThread->PushValue(GetValue<LPCTSTR>(-1));
//			ParamCount++;
//			break;
//		case LUA_TTABLE:
//			pLThread->PushValue(GetValue<CLuaTable>(-1));
//			ParamCount++;
//			break;
//		case LUA_TINTEGER:
//			pLThread->PushValue(GetValue<__int64>(-1));
//			ParamCount++;
//			break;
//		default:
//			if (pExtendCopyFN)
//			{
//				if (pExtendCopyFN(Type, this, -1, pLThread))
//				{
//					ParamCount++;
//				}
//			}
//		}
//		lua_pop(m_pLuaState, 1);
//	}
//	return ParamCount;
//}
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

int CLuaThread::Update(int ProcessLimit)
{
	int ProcessCount = 0;
	if (m_LastLuaStatus == LUA_YIELD)
	{
		ProcessCount++;
		if (m_YieldType == LUA_RESERVE_YIELD_TYPE_SLEEP)
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
		else if (m_YieldTimer.IsTimeOut())
		{
			LogLua(_T("脚本中断超时"));
			m_pTimeManager = NULL;
			m_SleepTime = 0;
			m_SleepPastTime = 0;
			DoCall(0);			
		}
	}
	return ProcessCount;
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
int CLuaThread::LuaErrParser(lua_State* L)
{
	if (lua_gettop(L))
	{
		if (lua_checkstack(L, 2))
		{
			lua_pushstring(L, "\r\n");
			luaL_traceback(L, L, NULL, 0);
			if (lua_isstring(L, -2))
				lua_concat(L, 3);
		}
		else
		{
			LogLua(_T("not have enough stack"));
		}
		return 1;
	}
	return 0;
}