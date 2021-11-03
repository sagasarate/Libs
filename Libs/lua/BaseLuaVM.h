#pragma once
class CBaseLuaVM :
	public CBaseScriptHost
{
protected:
	lua_State *						m_pLuaState;
	CIDStorage<CLuaThread>			m_LuaThreadPool;	
	CEasyTimer						m_ThreadRecyleTimer;
	int								m_LuaStackSize;
	UINT							m_ThreadCount;
	UINT							m_GrowSize;
	UINT							m_GrowLimit;
public:
	CBaseLuaVM();
	~CBaseLuaVM();

	bool InitThreadPool();
	void DestoryThreadPool();
	bool DeleteScriptThread(CLuaThread * pLuaThread);
	void DeleteAllScriptThread();
	LPVOID GetFirstThreadPos()
	{
		return m_LuaThreadPool.GetFirstObjectPos();
	}
	CLuaThread * GetNextThread(LPVOID& Pos)
	{
		return m_LuaThreadPool.GetNextObject(Pos);
	}
	CLuaThread * GetThreadByID(UINT ID)
	{
		return m_LuaThreadPool.GetObject(ID);
	}
	CLuaThread * GetThreadByYeildType(int YeildType);

	lua_State *	GetLuaState()
	{
		return m_pLuaState;
	}	
	CLuaThread * AllocScriptThread(CBaseScriptHost * pObject, LPCTSTR szFunctionName);
	bool AddCFunction(LPCTSTR LibName, LPCTSTR FuncName, lua_CFunction Function, LPCVOID FuncAddr, UINT FuncSize);
	bool AddCFunction(LPCTSTR LibName, const LUA_CFUN_INFO& FunctionInfo)
	{
		return AddCFunction(LibName, FunctionInfo.FunName, FunctionInfo.pLuaCFun, FunctionInfo.FunAddr, FunctionInfo.FunSize);
	}
	UINT AddCFunctions(const char * LibName, const CEasyArray<LUA_CFUN_INFO>& FunctionList);
	void RecycleThread();

};

