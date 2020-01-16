#pragma once

class CLuaThread;
class CLuaBaseStaticMetaClass;

typedef void (CLuaBaseStaticMetaClass::*FN_SET_META_CLASS)(lua_State * pLuaState);

class CSetMetaClassFNRegister
{
protected:
	UINT m_ClassID;
	static CStaticMapLite<UINT, FN_SET_META_CLASS> m_SetMetaClassFNMap;
public:
	CSetMetaClassFNRegister(UINT ClassID, FN_SET_META_CLASS pFN);
	~CSetMetaClassFNRegister();

	static FN_SET_META_CLASS * FindSetMetaClassFN(UINT ClassID)
	{
		return m_SetMetaClassFNMap.Find(ClassID);
	}
};

#define DECLARE_STATIC_META_CLASS(ClassName) \
protected:\
	static CSetMetaClassFNRegister m_##ClassName##SetMetaClassFNRegister;

#define IMPLEMENT_STATIC_META_CLASS(ClassName) \
	CSetMetaClassFNRegister	ClassName::m_##ClassName##SetMetaClassFNRegister(ClassName::CLASS_ID,(FN_SET_META_CLASS)&ClassName::SetMetaClass);
	

class CLuaBaseStaticMetaClass
{
protected:
	int				m_ClassID;
	const char *	m_ClassName;

	DECLARE_STATIC_META_CLASS(CLuaBaseStaticMetaClass)
public:
	static const int CLASS_ID = 10000;
	CLuaBaseStaticMetaClass()
	{
		m_ClassID = CLASS_ID;
		m_ClassName = "CLuaBaseStaticMetaClass";
	}
	~CLuaBaseStaticMetaClass()
	{

	}

	bool Attach(void * pData, UINT DataLen)
	{
		return false;
	}


	static bool SetMetaClassByObject(lua_State * pLuaState, CLuaBaseStaticMetaClass * pObject);
	void SetMetaClass(lua_State * pLuaState) const;
protected:
	void StartRegisterMetaClass(lua_State * pLuaState) const;
	void EndRegisterMetaClass(lua_State * pLuaState) const;

	void RegisterMemberFunctions(lua_State * pLuaState) const;	

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const char* funcName, Func func) const
	{
		RegisterMetaCFun(pLuaState, funcName, LuaWrap::DirectCallStaticMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const wchar_t* funcName, Func func) const
	{
		CEasyStringA StringA;
		StringA = funcName;
		RegisterMetaCFun(pLuaState, StringA, LuaWrap::DirectCallStaticMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	void RegisterMetaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc) const;

protected:

	const char * LuaGetClassName(CLuaThread * pThreadInfo);
};

