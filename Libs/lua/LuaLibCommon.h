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
	void LuaLog(CLuaThread * pLuaThread, LPCTSTR Text);
	void LuaLogDebug(CLuaThread * pLuaThread, LPCTSTR Text);
	void LuaLogWithStack(CLuaThread * pLuaThread, int Deepth, LPCTSTR Text);
	LUA_EMPTY_VALUE LuaNewByteArray(CLuaThread * pLuaThread, UINT Size);
	LUA_EMPTY_VALUE LuaNewGrid(CLuaThread * pLuaThread, UINT Col, UINT Row);
	LUA_EMPTY_VALUE LuaTableToJsonStr(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
	LUA_EMPTY_VALUE LuaTableToJsonStrPretty(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
	LUA_EMPTY_VALUE LuaJsonStrToTable(CLuaThread * pLuaThread, LPCTSTR JsonStr);
};

