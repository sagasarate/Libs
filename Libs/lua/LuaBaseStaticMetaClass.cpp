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
	FN_SET_META_CLASS * ppFN = CSetMetaClassFNRegister::FindSetMetaClassFN(pObject->m_ClassID);
	if (ppFN)
	{
		(pObject->*(*ppFN))(pLuaState);
	}
	return false;
}
void CLuaBaseStaticMetaClass::SetMetaClass(lua_State * pLuaState)  const
{
	luaL_getmetatable(pLuaState, m_ClassName);
	if (lua_isnil(pLuaState, -1))
	{
		lua_pop(pLuaState, 1);
		StartRegisterMetaClass(pLuaState);
		RegisterMemberFunctions(pLuaState);
		EndRegisterMetaClass(pLuaState);
		luaL_getmetatable(pLuaState, m_ClassName);
	}
	lua_setmetatable(pLuaState, -2);
}

void CLuaBaseStaticMetaClass::StartRegisterMetaClass(lua_State * pLuaState) const
{
	luaL_newmetatable(pLuaState, m_ClassName);
	lua_pushstring(pLuaState, "__index");
	lua_newtable(pLuaState);	
	
}

void CLuaBaseStaticMetaClass::EndRegisterMetaClass(lua_State * pLuaState) const
{
	lua_settable(pLuaState, -3);
	lua_pop(pLuaState, 1);
}


void CLuaBaseStaticMetaClass::RegisterMemberFunctions(lua_State * pLuaState) const
{
	lua_pushstring(pLuaState, "_ClassID");
	lua_pushnumber(pLuaState, m_ClassID);
	lua_settable(pLuaState, -3);

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
	return m_ClassName;
}