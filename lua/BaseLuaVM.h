#pragma once



class CBaseLuaVM
{
protected:
	CLuaThread						m_MainThread;
	CIDStorage<CLuaThread>			m_LuaThreadPool;
	int								m_LuaStackSize;
	UINT							m_ThreadCount;
	UINT							m_GrowSize;
	UINT							m_GrowLimit;
	CEasyArray<CEasyStringA>		m_LoadedModules;
public:

	

	CBaseLuaVM();
	virtual ~CBaseLuaVM();

	virtual void Destory();

	bool InitThreadPool();
	void DestoryThreadPool();
	bool DeleteScriptThread(CLuaThread* pLuaThread);
	void DeleteAllScriptThread();
	LPVOID GetFirstThreadPos()
	{
		return m_LuaThreadPool.GetFirstObjectPos();
	}
	CLuaThread* GetNextThread(LPVOID& Pos)
	{
		return m_LuaThreadPool.GetNextObject(Pos);
	}
	CLuaThread* GetThreadByID(UINT ID)
	{
		return m_LuaThreadPool.GetObject(ID);
	}
	CLuaThread* GetThreadByYeildType(int YeildType);
	void ClearAllYeildThread();
	CLuaThread* GetMainThread()
	{
		return &m_MainThread;
	}
	lua_State* GetLuaState()
	{
		return m_MainThread.GetLuaState();
	}
	CLuaThread* AllocLuaThread();
	CLuaThread* PrepareCall(CBaseScriptHost* pObject, LPCTSTR szFunctionName);
	CLuaThread* PrepareCall(CBaseScriptHost* pObject, LPCTSTR szObjectName, LPCTSTR szFunctionName);
	CLuaThread* PrepareCallMember(CLuaBaseMetaClass* pObject, LPCTSTR szFunctionName);
	bool AddCFunction(LPCTSTR LibName, LPCTSTR FuncName, lua_CFunction Function, LPCVOID FuncAddr, UINT FuncSize);
	bool AddCFunction(LPCTSTR LibName, const LUA_CFUN_INFO& FunctionInfo)
	{
		return AddCFunction(LibName, FunctionInfo.FunName, FunctionInfo.pLuaCFun, FunctionInfo.FunAddr, FunctionInfo.FunSize);
	}
	UINT AddCFunctions(LPCTSTR LibName, const CEasyArray<LUA_CFUN_INFO>& FunctionList);
	template <typename Callee, typename Func>
	inline void RegisterMemberFunction(LPCTSTR LibName, LPCTSTR FuncName, Func func)
	{
		AddCFunction(LibName, FuncName, LuaWrap::DirectCallObjectMemberDispatcherHelper<Callee, Func, 1>::DirectCallMemberDispatcher, &func, sizeof(func));
	}

	template <typename Func>
	inline void RegisterStaticFunction(LPCTSTR LibName, LPCTSTR FuncName, Func func)
	{
		AddCFunction(LibName, FuncName, LuaWrap::DirectCallFunctionDispatchHelper<Func, 1>::DirectCallFunctionDispatcher, &func, sizeof(func));
	}


	int DoGarbageCollect(bool PrintStatus);

	template <typename Callee, typename SearchFunc, typename LoadFunc>
	inline bool RegisterLoader(Callee* pCaller, SearchFunc SearchFn, LoadFunc LoadFn)
	{
		if (GetLuaState())
		{
			lua_getglobal(GetLuaState(), LUA_LOADLIBNAME);
			if (lua_istable(GetLuaState(), -1))
			{
				lua_getfield(GetLuaState(), -1, "searchers");
				if (lua_istable(GetLuaState(), -1))
				{
					size_t LoaderCount = lua_rawlen(GetLuaState(), -1);
					LogLuaDebug(_T("exist %u lua loaders"), LoaderCount);
					UINT DataSize = sizeof(CBaseLuaVM*) + sizeof(Callee*) + sizeof(SearchFunc) + sizeof(LoadFunc);
					BYTE* pBuffer = (BYTE*)lua_newuserdatauv(GetLuaState(), DataSize, 0);
					if (pBuffer)
					{
						CBaseLuaVM* pThis = this;
						memcpy(pBuffer, &pThis, sizeof(CBaseLuaVM*));
						memcpy(pBuffer + sizeof(CBaseLuaVM*), &pCaller, sizeof(Callee*));
						memcpy(pBuffer + sizeof(CBaseLuaVM*) + sizeof(Callee*), &SearchFn, sizeof(SearchFunc));
						memcpy(pBuffer + sizeof(CBaseLuaVM*) + sizeof(Callee*) + sizeof(SearchFunc), &LoadFn, sizeof(LoadFunc));
						lua_pushcclosure(GetLuaState(), (LuaLoaderDispatcherHelper<Callee, SearchFunc, LoadFunc>::LuaSearcher), 1);
						lua_rawseti(GetLuaState(), -2, LoaderCount + 1);
						LoaderCount = lua_rawlen(GetLuaState(), -1);
						LogLuaDebug(_T("exist %u lua loaders after register"), LoaderCount);
						lua_pop(GetLuaState(), 2);
						return true;
					}
				}
				lua_pop(GetLuaState(), 1);
			}
			lua_pop(GetLuaState(), 1);
		}
		return false;
	}
	template <typename SearchFunc, typename LoadFunc>
	inline bool RegisterLoader(SearchFunc SearchFn, LoadFunc LoadFn)
	{
		if (GetLuaState())
		{
			lua_getglobal(GetLuaState(), LUA_LOADLIBNAME);
			if (lua_istable(GetLuaState(), -1))
			{
				lua_getfield(GetLuaState(), -1, "searchers");
				if (lua_istable(GetLuaState(), -1))
				{
					size_t LoaderCount = lua_rawlen(GetLuaState(), -1);
					LogLuaDebug(_T("exist %u lua loaders"), LoaderCount);
					UINT DataSize = sizeof(CBaseLuaVM*) + sizeof(SearchFunc) + sizeof(LoadFunc);
					BYTE* pBuffer = (BYTE*)lua_newuserdatauv(GetLuaState(), DataSize, 0);
					if (pBuffer)
					{
						CBaseLuaVM* pThis = this;
						memcpy(pBuffer, &pThis, sizeof(CBaseLuaVM*));
						memcpy(pBuffer + sizeof(CBaseLuaVM*), &SearchFn, sizeof(SearchFunc));
						memcpy(pBuffer + sizeof(CBaseLuaVM*) + sizeof(SearchFunc), &LoadFn, sizeof(LoadFunc));
						lua_pushcclosure(GetLuaState(), (LuaLoaderDispatcherHelper2<SearchFunc, LoadFunc>::LuaSearcher), 1);
						lua_rawseti(GetLuaState(), -2, LoaderCount + 1);
						LoaderCount = lua_rawlen(GetLuaState(), -1);
						LogLuaDebug(_T("exist %u lua loaders after register"), LoaderCount);
						lua_pop(GetLuaState(), 2);
						return true;
					}
				}
				lua_pop(GetLuaState(), 1);
			}
			lua_pop(GetLuaState(), 1);
		}
		return false;
	}
	template <typename SearchFunc, typename LoadFunc>
	inline bool RegisterCustomLoader(SearchFunc SearchFn, LoadFunc LoadFn)
	{
		if (GetLuaState())
		{
			lua_getglobal(GetLuaState(), LUA_LOADLIBNAME);
			if (lua_istable(GetLuaState(), -1))
			{
				lua_getfield(GetLuaState(), -1, "searchers");
				if (lua_istable(GetLuaState(), -1))
				{
					size_t LoaderCount = (size_t)lua_rawlen(GetLuaState(), -1);
					LogLuaDebug(_T("exist %u lua loaders"), LoaderCount);
					UINT DataSize = sizeof(CBaseLuaVM*) + sizeof(SearchFunc) + sizeof(LoadFunc);
					BYTE* pBuffer = (BYTE*)lua_newuserdatauv(GetLuaState(), DataSize, 0);
					if (pBuffer)
					{
						CBaseLuaVM* pThis = this;
						memcpy(pBuffer, &pThis, sizeof(CBaseLuaVM*));
						memcpy(pBuffer + sizeof(CBaseLuaVM*), &SearchFn, sizeof(SearchFunc));
						memcpy(pBuffer + sizeof(CBaseLuaVM*) + sizeof(SearchFunc), &LoadFn, sizeof(LoadFunc));
						lua_pushcclosure(GetLuaState(), (LuaLoaderDispatcherHelper3<SearchFunc, LoadFunc>::LuaSearcher), 1);
						lua_rawseti(GetLuaState(), -2, LoaderCount + 1);
						LoaderCount = (size_t)lua_rawlen(GetLuaState(), -1);
						LogLuaDebug(_T("exist %u lua loaders after register"), LoaderCount);
						lua_pop(GetLuaState(), 2);
						return true;
					}
				}
				lua_pop(GetLuaState(), 1);
			}
			lua_pop(GetLuaState(), 1);
		}
		return false;
	}
	void AddLoadedModule(const char* szModuleName);
	void ClearLoadedModule();	

	virtual int Update(int ProcessLimit = 32);
	static void DumpLuaValue(lua_State* L, int Index, CStringBuilder& OutBugger, LPCTSTR PrevStr, bool ForceSameLine, int Deep);
	bool IsObjectExist(CLuaBaseMetaClass* pObject);
	bool AddNewThreadCallback(lua_State* L, int Index);
	int AddPersistentObject(lua_State* L, int Index);
	int PushPersistentObject(lua_State* L, int ID);
	bool RemovePersistentObject(lua_State* L, int ID);
protected:
	void InitEnv();
	static int LuaPrint(lua_State* L);	
	static int LuaPanic(lua_State* L);
	
	static int ExtendModuleSearcher(LPCTSTR ModelName);
	static int ExtendModuleLoader(lua_State* L, LPCTSTR ModelName);
};

inline CLuaThread* CBaseLuaVM::PrepareCall(CBaseScriptHost* pObject, LPCTSTR szFunctionName)
{
	return PrepareCall(pObject, NULL, szFunctionName);
}