#include "stdafx.h"

CStaticMapLite<UINT, FN_SET_META_CLASS> CSetMetaClassFNRegister::m_SetMetaClassFNMap;

CSetMetaClassFNRegister::CSetMetaClassFNRegister(UINT ClassID, FN_SET_META_CLASS pFN)
{	
	m_ClassID = ClassID;
	if (m_SetMetaClassFNMap.GetBufferSize() == 0)
	{
		m_SetMetaClassFNMap.Create(64, 64, 64);
	}
	m_SetMetaClassFNMap.Insert(ClassID, pFN);
}
CSetMetaClassFNRegister::~CSetMetaClassFNRegister()
{
	m_SetMetaClassFNMap.Delete(m_ClassID);
}

IMPLEMENT_STATIC_META_CLASS(CLuaBaseStaticMetaClass)


bool CLuaBaseStaticMetaClass::SetMetaClassByObject(lua_State * pLuaState, CLuaBaseStaticMetaClass * pObject)
{
	FN_SET_META_CLASS * ppFN = CSetMetaClassFNRegister::FindSetMetaClassFN(pObject->GetMetaClassID());
	if (ppFN)
	{
		(pObject->*(*ppFN))(pLuaState);
	}
	return false;
}
void CLuaBaseStaticMetaClass::SetMetaClass(lua_State * pLuaState) const
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

void CLuaBaseStaticMetaClass::RegisterMetaClass(lua_State * pLuaState) const
{
	luaL_newmetatable(pLuaState, GetMetaClassName());
	lua_pushstring(pLuaState, "__gc");
	lua_pushcclosure(pLuaState, &CLuaBaseStaticMetaClass::DoGarbageCollect, 0);
	lua_settable(pLuaState, -3);
	lua_pushstring(pLuaState, "__index");
	lua_newtable(pLuaState);
	lua_pushstring(pLuaState, "_ClassID");
	lua_pushnumber(pLuaState, GetMetaClassID());
	lua_settable(pLuaState, -3);
	RegisterMemberFunctions(pLuaState);
	lua_settable(pLuaState, -3);
	lua_pop(pLuaState, 1);
}

void CLuaBaseStaticMetaClass::RegisterMemberFunctions(lua_State * pLuaState) const
{
	RegisterMetaFunction<CLuaBaseStaticMetaClass>(pLuaState, "GetClassName", &CLuaBaseStaticMetaClass::LuaGetClassName);	
}



void CLuaBaseStaticMetaClass::RegisterMetaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc) const
{
	lua_pushstring(pLuaState, funcName);

	unsigned char* buffer = (unsigned char*)lua_newuserdata(pLuaState, sizeofFunc);

	memcpy(buffer, func, sizeofFunc), buffer += sizeofFunc;

	lua_pushcclosure(pLuaState, function, 1);
	lua_settable(pLuaState, -3);
}

const char * CLuaBaseStaticMetaClass::LuaGetClassName(CLuaThread * pThreadInfo)
{
	return GetMetaClassName();
}

int CLuaBaseStaticMetaClass::DoGarbageCollect(lua_State* L)
{
	if (lua_type(L, 1) == LUA_TUSERDATA)
	{
		UINT Len = (UINT)lua_rawlen(L, 1);
		BYTE * pBuff = (BYTE *)lua_touserdata(L, 1);
		if (GetLuaObjectType(L, 1) >= CLuaBaseStaticMetaClass::CLASS_ID)
		{
			if (pBuff&&Len >= sizeof(CLuaBaseStaticMetaClass))
			{
				CLuaBaseStaticMetaClass * pObject = dynamic_cast<CLuaBaseStaticMetaClass *>((CLuaBaseStaticMetaClass *)pBuff);

				if (pObject)
				{
					pObject->~CLuaBaseStaticMetaClass();
				}
				else
				{
					LogLua("对象不是CLuaBaseStaticMetaClass");
				}
			}
			else
			{
				LogLua("userdata大小不符");
			}
		}
		else
		{
			LogLua("对象类型不符");
		}
	}
	else
	{
		LogLua("对象不是userdata");
	}
	return 0;
}