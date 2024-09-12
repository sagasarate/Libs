#pragma once
class CLuaScript:
	public CBaseLuaVM
{
protected:
	bool							m_NeedReload;
	bool							m_IsLoadFromFile;
	UINT							m_ScriptID;
	CEasyString						m_ScriptName;
	CEasyString						m_ScriptContent;	
	CEasyString						m_ReloadScriptContent;
	CEasyArray<LUA_CFUN_INFO> 		m_CFunctionList;
	
public:
	CLuaScript();
	~CLuaScript();

	virtual void Destory();

	bool Init(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO> * pFunctionList, int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit);
	

	void ReloadScript(LPCTSTR ScriptContent);

	virtual int Update(int ProcessLimit = 32) override;

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
	//bool PopLuaThread(lua_State * pThreadState);
protected:
	bool LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent);

};

