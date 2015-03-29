#pragma once

class CLuaBaseMetaClass :
	public CLuaScript
{

public:
	static const int CLASS_ID = 101;
	CLuaBaseMetaClass();
	CLuaBaseMetaClass(const CLuaBaseMetaClass& MetaClass, bool ShareLuaState)
		:CLuaScript(MetaClass, ShareLuaState)
	{

	}
	virtual ~CLuaBaseMetaClass();

	void SetMetaClass(lua_State * pLuaState);
protected:
	
	void RegisterMetaClass(lua_State * pLuaState);
	
	virtual void RegisterMemberFunctions(lua_State * pLuaState);
	virtual const char * GetMetaClassName();
	virtual int GetMetaClassID();

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const char* funcName, Func func)
	{
		RegisterMetaCFun(pLuaState, funcName, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const wchar_t* funcName, Func func)
	{
		CEasyStringA StringA;
		StringA = funcName;
		RegisterMetaCFun(pLuaState, StringA, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	void RegisterMetaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc);

protected:

	const char * LuaGetClassName(CLuaThread * pThreadInfo);
};

