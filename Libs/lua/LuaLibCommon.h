#pragma once


class CLuaLibCommon :
	public CBaseLuaLib,
	public CStaticObject<CLuaLibCommon>
{
public:
	CLuaLibCommon();
	~CLuaLibCommon();

	bool RegisterToVM(CBaseLuaVM * pLuaVM)
	{
		return CBaseLuaLib::RegisterToVM(pLuaVM, _T("Common"));
	}
protected:
	static void LuaLog(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
	static void LuaLogDebug(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
	static void LuaLogWithStack(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
	static UINT LuaGetCurTime(CLuaThread* pLuaThread);
	static LUA_EMPTY_VALUE LuaTableToJsonStr(CLuaThread* pLuaThread, LUA_EMPTY_VALUE, bool IsPretty);
	static LUA_EMPTY_VALUE LuaJsonStrToTable(CLuaThread * pLuaThread, LPCTSTR JsonStr);
	static LUA_EMPTY_VALUE LuaDump(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
	static bool LuaRegisterNewThreadCallback(CLuaThread* pLuaThread, LUA_EMPTY_VALUE);
};

