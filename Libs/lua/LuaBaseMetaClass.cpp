#include "stdafx.h"


CLuaBaseMetaClass::CLuaBaseMetaClass()
{
	
}


CLuaBaseMetaClass::~CLuaBaseMetaClass()
{
}

void CLuaBaseMetaClass::SetMetaClass(lua_State * pLuaState)
{
	luaL_getmetatable(pLuaState, GetMetaClassName());
	if (lua_isnil(pLuaState, -1))
	{
		lua_pop(pLuaState, 1);
		RegisterMetaClass(pLuaState);
		luaL_getmetatable(pLuaState, GetMetaClassName());
	}
	lua_setmetatable(pLuaState, -2);
}

void CLuaBaseMetaClass::RegisterMetaClass(lua_State * pLuaState)
{
	luaL_newmetatable(pLuaState, GetMetaClassName());
	lua_pushstring(pLuaState, "__index");
	lua_newtable(pLuaState);
	RegisterMemberFunctions(pLuaState);
	lua_settable(pLuaState, -3);
	lua_pop(pLuaState, 1);
}



void CLuaBaseMetaClass::RegisterMemberFunctions(lua_State * pLuaState)
{
	lua_pushstring(pLuaState, "_ClassID");
	lua_pushnumber(pLuaState, GetMetaClassID());
	lua_settable(pLuaState, -3);

	RegisterMetaFunction<CLuaBaseMetaClass>(pLuaState, "GetClassName", &CLuaBaseMetaClass::LuaGetClassName);
}

const char * CLuaBaseMetaClass::GetMetaClassName()
{
	return "CLuaBaseMetaClass";
}
int CLuaBaseMetaClass::GetMetaClassID()
{
	return CLuaBaseMetaClass::CLASS_ID;
}

void CLuaBaseMetaClass::RegisterMetaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc)
{
	lua_pushstring(pLuaState, funcName);

	unsigned char* buffer = (unsigned char*)lua_newuserdata(pLuaState, sizeofFunc);

	memcpy(buffer, func, sizeofFunc), buffer += sizeofFunc;

	lua_pushcclosure(pLuaState, function, 1);
	lua_settable(pLuaState, -3);
}

const char * CLuaBaseMetaClass::LuaGetClassName(CLuaThread * pThreadInfo)
{
	return GetMetaClassName();
}