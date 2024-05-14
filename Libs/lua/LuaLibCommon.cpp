#include "stdafx.h"



CLuaLibCommon::CLuaLibCommon()
{
	RegisterFunction(_T("Log"), &CLuaLibCommon::LuaLog);
	RegisterFunction(_T("LogDebug"), &CLuaLibCommon::LuaLogDebug);
	RegisterFunction(_T("LogWithStack"), &CLuaLibCommon::LuaLogWithStack);
	RegisterFunction(_T("GetCurTime"), &CLuaLibCommon::LuaGetCurTime);
	RegisterFunction(_T("TableToJsonStr"), &CLuaLibCommon::LuaTableToJsonStr);
	RegisterFunction(_T("JsonStrToTable"), &CLuaLibCommon::LuaJsonStrToTable);
	RegisterFunction(_T("Dump"), &CLuaLibCommon::LuaDump);
	RegisterFunction(_T("RegisterNewThreadCallback"), &CLuaLibCommon::LuaRegisterNewThreadCallback);
}


CLuaLibCommon::~CLuaLibCommon()
{
	
}



void CLuaLibCommon::LuaLog(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());

	if (lua_getglobal(pLuaThread->GetLuaState(), "string") == LUA_TTABLE)
	{
		pLuaThread->PushString("format");
		if (lua_gettable(pLuaThread->GetLuaState(), -2) == LUA_TFUNCTION)
		{
			lua_insert(pLuaThread->GetLuaState(), 1);
			pLuaThread->Pop(1);
			lua_pcall(pLuaThread->GetLuaState(), ParamCount, 1, NULL);
			LogLuaStr(pLuaThread->GetLuaState(), -1, NULL, false);
		}
		else
		{
			LogLuaStr(pLuaThread->GetLuaState(), 1, NULL, false);
		}
	}
	else
	{
		LogLuaStr(pLuaThread->GetLuaState(), 1, NULL, false);
	}	
}
void CLuaLibCommon::LuaLogDebug(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());

	if (lua_getglobal(pLuaThread->GetLuaState(), "string") == LUA_TTABLE)
	{
		pLuaThread->PushString("format");
		if (lua_gettable(pLuaThread->GetLuaState(), -2) == LUA_TFUNCTION)
		{
			lua_insert(pLuaThread->GetLuaState(), 1);
			pLuaThread->Pop(1);
			lua_pcall(pLuaThread->GetLuaState(), ParamCount, 1, NULL);
			LogLuaStrDebug(pLuaThread->GetLuaState(), -1, NULL, false);
		}
		else
		{
			LogLuaStrDebug(pLuaThread->GetLuaState(), 1, NULL, false);
		}
	}
	else
	{
		LogLuaStrDebug(pLuaThread->GetLuaState(), 1, NULL, false);
	}
}
void CLuaLibCommon::LuaLogWithStack(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{	
	int ParamCount = lua_gettop(pLuaThread->GetLuaState());

	if (lua_getglobal(pLuaThread->GetLuaState(), "string") == LUA_TTABLE)
	{
		pLuaThread->PushString("format");
		if (lua_gettable(pLuaThread->GetLuaState(), -2) == LUA_TFUNCTION)
		{
			lua_insert(pLuaThread->GetLuaState(), 1);
			pLuaThread->Pop(1);
			lua_pcall(pLuaThread->GetLuaState(), ParamCount, 1, NULL);
			LogLuaStr(pLuaThread->GetLuaState(), -1, NULL, true);
		}
		else
		{
			LogLuaStr(pLuaThread->GetLuaState(), 1, NULL, true);
		}
	}
	else
	{
		LogLuaStr(pLuaThread->GetLuaState(), 1, NULL, true);
	}
}
UINT CLuaLibCommon::LuaGetCurTime(CLuaThread* pLuaThread)
{
	return (UINT)time(NULL);
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaTableToJsonStr(CLuaThread * pLuaThread, LUA_EMPTY_VALUE, bool IsPretty)
{
	CEasyString JsonStr;
	if (pLuaThread->GetJson(JsonStr, 1, IsPretty))
		pLuaThread->PushString(JsonStr);
	else
		pLuaThread->PushNil();
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaJsonStrToTable(CLuaThread * pLuaThread, LPCTSTR JsonStr)
{
	pLuaThread->PushJson(JsonStr);
	return LUA_EMPTY_VALUE();
}

LUA_EMPTY_VALUE CLuaLibCommon::LuaDump(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	CStringBuilder Str;
	CBaseLuaVM::DumpLuaValue(pLuaThread->GetLuaState(), 1, Str, _T(""), false, 0);
	pLuaThread->PushString(Str.GetStr());
	return LUA_EMPTY_VALUE();
}

bool CLuaLibCommon::LuaRegisterNewThreadCallback(CLuaThread* pLuaThread, LUA_EMPTY_VALUE)
{
	return pLuaThread->GetLuaVM()->AddNewThreadCallback(pLuaThread->GetLuaState(), 1);
}