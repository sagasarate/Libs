#pragma once

class CBaseLuaLib :
	public CBaseScriptHost
{
protected:
	CEasyArray<LUA_CFUN_INFO>		m_LuaCFunList;
public:
	CBaseLuaLib()
	{

	}
	~CBaseLuaLib()
	{

	}

	template <typename Func>
	inline void RegisterFunction(LPCTSTR FuncName, Func func)
	{
		AddCFunction(FuncName, LuaWrap::DirectCallFunctionDispatchHelper<Func, 1>::DirectCallFunctionDispatcher, &func, sizeof(func));
	}

	void AddCFunction(LPCTSTR FuncName, lua_CFunction Function, LPCVOID FunAddr, UINT FuncSize)
	{
		LUA_CFUN_INFO * pInfo = m_LuaCFunList.AddEmpty();
		pInfo->FunName = FuncName;
		pInfo->pLuaCFun = Function;
		memcpy(pInfo->FunAddr, FunAddr, FuncSize);
		pInfo->FunSize = FuncSize;
	}
	bool RegisterToVM(CBaseLuaVM * pLuaVM, LPCTSTR LibName)
	{
		return pLuaVM->AddCFunctions(LibName, m_LuaCFunList) == m_LuaCFunList.GetCount();
	}
};

