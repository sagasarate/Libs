#pragma once


typedef void (*LUA_META_DATA_REGISTER_FUNCTION) (lua_State *L);

class CLuaScriptPool
{
protected:
	

	CStaticMap<CEasyString,CLuaScript>			m_LuaScriptPool;
	CEasyArray<CLuaScript::LUA_CFUN_INFO>		m_LuaCFunList;
	CEasyTimer									m_UpdateTimer;
	UINT										m_LuaStackSize;
	UINT										m_ThreadStartSize;
	UINT										m_ThreadGrowSize;
	UINT										m_ThreadGrowLimit;
public:
	CLuaScriptPool(void);
	~CLuaScriptPool(void);
	

	bool Create(UINT StartSize, UINT GrowSize, UINT GrowLimit, UINT LuaStackSize, UINT ThreadStartSize, UINT ThreadGrowSize, UINT ThreadGrowLimit);

	virtual void Destory();

	int Update();

	CLuaScript * GetScript(LPCTSTR szLuaFileName);
	CLuaScript * GetScript(UINT ScriptID);
	CLuaScript * LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent);

	CLuaThread * AllocScriptThread(LPCTSTR szLuaFileName,CBaseScriptHost * pObject,LPCTSTR szFunctionName);
	CLuaThread * AllocScriptThread(UINT ScriptID,CBaseScriptHost * pObject,LPCTSTR szFunctionName);

	void ReloadAllScript();
	void ReloadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent);

	template <typename Callee, typename Func>
	inline void RegisterFunction(const char* funcName, Func func)
	{
		AddCFunction(funcName, LuaWrap::DirectCallObjectMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

protected:
	
	void AddCFunction(const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc);
	
};
