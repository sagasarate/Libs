#pragma once
class CLuaScript:
	public CBaseScriptHost
{
public:
	struct LUA_CFUN_INFO
	{
		CEasyString		FunName;
		lua_CFunction	pLuaCFun;
		BYTE			FunAddr[16];
		UINT			FunSize;
	};
	static UINT			m_LogChannel;
protected:
	bool							m_NeedReload;
	bool							m_IsLoadFromFile;
	bool							m_IsCloneScript;
	UINT							m_ScriptID;
	CEasyString						m_ScriptName;
	CEasyString						m_ScriptContent;
	int								m_LuaStackSize;
	UINT							m_ThreadCount;
	UINT							m_GrowSize;
	UINT							m_GrowLimit;
	CEasyString						m_ReloadScriptContent;
	lua_State *						m_pLuaState;
	CIDStorage<CLuaThread>			m_LuaThreadPool;
	CEasyArray<LUA_CFUN_INFO> *		m_pFunctionList;
	CEasyTimer						m_ThreadRecyleTimer;
public:
	CLuaScript();
	CLuaScript(const CLuaScript& LuaScript, bool ShareLuaState);
	~CLuaScript();

	virtual void Destory();

	bool Init(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO>* pFunctionList, int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit);
	CLuaThread * AllocScriptThread(CBaseScriptHost * pObject, LPCTSTR szFunctionName);
	bool DeleteScriptThread(CLuaThread * pLuaThread);
	void ReloadScript(LPCTSTR ScriptContent);

	virtual void Update();

	void SetID(UINT ID)
	{
		m_ScriptID = ID;
	}
	UINT GetID()
	{
		return m_ScriptID;
	}
	LPCTSTR GetScriptName()
	{
		return m_ScriptName;
	}

	void SetLoadFromFile()
	{
		m_IsLoadFromFile = true;
	}
	bool IsLoadFromFile()
	{
		return m_IsLoadFromFile;
	}
protected:
	bool LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO>* pFunctionList);

	void RegisterLuaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc);

};

