#pragma once


class CLuaLibSmartStruct :
	public CBaseLuaLib,
	public CStaticObject<CLuaLibSmartStruct>
{
public:
	CLuaLibSmartStruct();
	~CLuaLibSmartStruct();

	bool RegisterToVM(CBaseLuaVM * pLuaVM)
	{
		return CBaseLuaLib::RegisterToVM(pLuaVM, _T("SmartStruct"));
	}

	LUA_EMPTY_VALUE LuaNew(CLuaThread * pLuaThread, UINT Size);
	UINT LuaGetEmptyStructSize(CLuaThread * pLuaThread);
	UINT LuaGetFixMemberSize(CLuaThread * pLuaThread, UINT TypeLen);
	UINT LuaGetStructMemberSize(CLuaThread * pLuaThread, UINT StructSize);
	UINT LuaGetStringMemberSize(CLuaThread * pLuaThread, LUA_EMPTY_VALUE);
};

