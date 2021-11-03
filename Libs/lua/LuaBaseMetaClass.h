#pragma once

class CLuaBaseMetaClass :
	public CBaseScriptHost
{

public:
	static const int CLASS_ID = 20000;
	CLuaBaseMetaClass();	
	virtual ~CLuaBaseMetaClass();

	void SetMetaClass(lua_State * pLuaState) const;
	virtual const char * GetMetaClassName() const = 0;
	virtual int GetMetaClassID() const = 0;
	virtual size_t GetMetaClassSize() const = 0;
	void CheckAPI(lua_State * pLuaState, CEasyArray<CEasyStringA>& Functions) const;
protected:
	
	void RegisterMetaClass(lua_State * pLuaState) const;
	virtual void RegisterMemberFunctions(lua_State * pLuaState) const;
	virtual void OnGarbageCollect();
	static int DoGarbageCollect(lua_State* L);

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const char* funcName, Func func) const
	{
		RegisterMetaCFun(pLuaState, funcName, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	template <typename Callee, typename Func>
	inline void RegisterMetaFunction(lua_State * pLuaState, const wchar_t* funcName, Func func) const
	{
		CEasyStringA StringA;
		StringA = funcName;
		RegisterMetaCFun(pLuaState, StringA, LuaWrap::DirectCallMetaClassMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	template <typename Func>
	inline void RegisterMetaStaticFunction(lua_State * pLuaState, const char* funcName, Func func) const
	{
		RegisterMetaCFun(pLuaState, funcName, LuaWrap::DirectCallFunctionDispatchHelper<Func, 1>::DirectCallFunctionDispatcher, &func, sizeof(func));
	}

	template <typename Func>
	inline void RegisterMetaStaticFunction(lua_State * pLuaState, const wchar_t* funcName, Func func) const
	{
		CEasyStringA StringA;
		StringA = funcName;
		RegisterMetaCFun(pLuaState, StringA, LuaWrap::DirectCallFunctionDispatchHelper<Func, 1>::DirectCallFunctionDispatcher, &func, sizeof(func));
	}

	void RegisterMetaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc) const;

protected:

	const char * LuaGetClassName(CLuaThread * pThreadInfo);
	
};

typedef CLuaBaseMetaClass * LPLUABASEMETACLASS;

namespace LuaWrap
{
	inline void Push(lua_State* L, CLuaBaseMetaClass * pObject)
	{
		if (pObject)
		{
			void *buf = lua_newuserdata(L, sizeof(CLuaBaseMetaClass *));
			memcpy(buf, &pObject, sizeof(CLuaBaseMetaClass *));
			if (pObject)
				pObject->SetMetaClass(L);
		}
		else
		{
			lua_pushnil(L);
		}
	}
	inline bool	Match(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx)
	{
		int Type = GetLuaObjectType(L, idx);
		if (Type == LUA_TNIL)
		{
			return true;
		}
		else if (Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			//LogLua(_T("Match %d as CLuaBaseMetaClass"), Type);
			return true;
		}
		return false;
	}
	inline CLuaBaseMetaClass * Get(TypeWrapper<CLuaBaseMetaClass *>, lua_State* L, int idx)
	{
		if (GetLuaObjectType(L, idx) >= CLuaBaseMetaClass::CLASS_ID)
		{
			CLuaBaseMetaClass * pObject = dynamic_cast<CLuaBaseMetaClass *>(*((CLuaBaseMetaClass **)lua_touserdata(L, idx)));

			return pObject;
		}
		return NULL;
	}
}

#undef LUA_WRAP_CALL_RETURN_TYPE
#undef LUA_WRAP_RETURN_PUSH_OPERATION



#define LUA_WRAP_CALL_RETURN_TYPE CLuaBaseMetaClass *
#define LUA_WRAP_RETURN_PUSH_OPERATION(Ret) Push(L, Ret); if (pThreadInfo->IsNeedYield()) {return lua_yield(pThreadInfo->GetLuaState(),pThreadInfo->GetYieldReturnCount());} else return 1;
#include "LuaCallWrapTemplate.h"