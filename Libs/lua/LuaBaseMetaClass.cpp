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
	//RegisterMetaStaticFunction(pLuaState, "__gc", &CLuaBaseMetaClass::DoGarbageCollect);
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

void CLuaBaseMetaClass::CheckAPI(lua_State * pLuaState, CEasyArray<CEasyStringA>& Functions)
{
	
	luaL_getmetatable(pLuaState, GetMetaClassName());
	if (lua_isnil(pLuaState, -1))
	{
		lua_pop(pLuaState, 1);
		RegisterMetaClass(pLuaState);
		luaL_getmetatable(pLuaState, GetMetaClassName());
	}
	lua_pushstring(pLuaState, "__index");
	lua_gettable(pLuaState, -2);
	if (lua_istable(pLuaState, -1))
	{
		for (UINT i = 0; i < Functions.GetCount(); i++)
		{
			CEasyStringA& FuncName = Functions[i];
			bool IsExist = false;
			lua_pushstring(pLuaState, FuncName);
			lua_gettable(pLuaState, -2);
			if (lua_isnil(pLuaState, -1))
			{
				LogLua(_T("Lua %s API %s Not Exist"), GetMetaClassName(), (LPCSTR)FuncName);
			}
			lua_pop(pLuaState, 1);
		}
	}
	else
	{
		LogLua(_T("Invalid MetaTable %s "), GetMetaClassName());
	}

	lua_pop(pLuaState, 1);

	lua_pushnil(pLuaState);
	while (lua_next(pLuaState, -2) != 0)
	{
		if (lua_iscfunction(pLuaState, -1))
		{
			CEasyStringA FuncName = lua_tostring(pLuaState, -2);
			bool IsExist = false;
			for (UINT i = 0; i < Functions.GetCount(); i++)
			{
				if (Functions[i].CompareNoCase(FuncName) == 0)
				{
					IsExist = true;
					break;
				}
			}
			if (!IsExist)
			{
				LogLua(_T("Lua %s API %s Is Obsolete"), GetMetaClassName(),(LPCSTR)FuncName);
			}
		}		
		lua_pop(pLuaState, 1);
	}
	lua_pop(pLuaState, 1);
}

void CLuaBaseMetaClass::OnGarbageCollect(CLuaThread * pThreadInfo)
{

}

void CLuaBaseMetaClass::DoGarbageCollect(CLuaThread * pThreadInfo, CLuaBaseMetaClass * pObject)
{
	if (pObject)
		pObject->OnGarbageCollect(pThreadInfo);
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