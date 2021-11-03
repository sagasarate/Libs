#include "stdafx.h"



CLuaLibSmartStruct::CLuaLibSmartStruct()
{
	RegisterFunction<CLuaLibSmartStruct>("New", &CLuaLibSmartStruct::LuaNew);
	RegisterFunction<CLuaLibSmartStruct>("GetEmptyStructSize", &CLuaLibSmartStruct::LuaGetEmptyStructSize);
	RegisterFunction<CLuaLibSmartStruct>("GetFixMemberSize", &CLuaLibSmartStruct::LuaGetFixMemberSize);
	RegisterFunction<CLuaLibSmartStruct>("GetStructMemberSize", &CLuaLibSmartStruct::LuaGetStructMemberSize);
	RegisterFunction<CLuaLibSmartStruct>("GetStringMemberSize", &CLuaLibSmartStruct::LuaGetStringMemberSize);
}


CLuaLibSmartStruct::~CLuaLibSmartStruct()
{
}


LUA_EMPTY_VALUE CLuaLibSmartStruct::LuaNew(CLuaThread * pLuaThread, UINT Size)
{
	UINT ClassSize = sizeof(CLuaSmartStruct);
	if (Size)
	{
		BYTE * pBuff = (BYTE *)lua_newuserdata(pLuaThread->GetLuaState(), ClassSize + Size);
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#undef new
#endif
		::new(pBuff) CLuaSmartStruct();
#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new NEWNEW
#endif
		((CLuaSmartStruct *)pBuff)->GetValue().Attach(pBuff + ClassSize, Size, true);
		((CLuaSmartStruct *)pBuff)->SetMetaClass(pLuaThread->GetLuaState());
	}
	else
	{
		lua_pushnil(pLuaThread->GetLuaState());
	}
	return LUA_EMPTY_VALUE();
}
UINT CLuaLibSmartStruct::LuaGetEmptyStructSize(CLuaThread * pLuaThread)
{
	return CSmartStruct::GetEmptyStructSize();
}
UINT CLuaLibSmartStruct::LuaGetFixMemberSize(CLuaThread * pLuaThread, UINT TypeLen)
{
	return CSmartStruct::GetFixMemberSize(TypeLen);
}
UINT CLuaLibSmartStruct::LuaGetStructMemberSize(CLuaThread * pLuaThread, UINT StructSize)
{
	return CSmartStruct::GetStructMemberSize(StructSize);
}
UINT CLuaLibSmartStruct::LuaGetStringMemberSize(CLuaThread * pLuaThread, LUA_EMPTY_VALUE)
{
	int Type = GetLuaObjectType(pLuaThread->GetLuaState(), 1);
	switch (Type)
	{
	case LUA_TSTRING:
		return CSmartStruct::GetStringMemberSize(lua_tostring(pLuaThread->GetLuaState(), 1));
	
	}
	return 0;
}
