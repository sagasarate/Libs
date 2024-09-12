#include "stdafx.h"

UINT g_LuaLogChannel = 0;

#ifdef WIN32
#ifdef UNICODE
CEasyString::STRING_CODE_PAGE LUA_SCRIPT_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UTF8;
#else
CEasyString::STRING_CODE_PAGE LUA_SCRIPT_CODE_PAGE = CEasyString::STRING_CODE_PAGE_ANSI;
#endif
#else
CEasyString::STRING_CODE_PAGE LUA_SCRIPT_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UTF8;
#endif

void SystemStrToLuaStr(LPCSTR SystemStr, CEasyStringA& LuaStr)
{
	if(SystemStr)
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			LuaStr = SystemStr;
		}
		else
		{
			//内码转换
			size_t SrcLen = CEasyStringA::GetStrLen(SystemStr);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				//转成UTF8
				size_t DestLen = AnsiToUTF8(SystemStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					LuaStr.Resize(DestLen, false);
					AnsiToUTF8(SystemStr, SrcLen, LuaStr.GetBuffer(), DestLen);
					LuaStr.TrimBuffer(DestLen);
				}
			}
			else
			{
				//转成ANSI
				size_t DestLen = UTF8ToAnsi(SystemStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					LuaStr.Resize(DestLen, false);
					UTF8ToAnsi(SystemStr, SrcLen, LuaStr.GetBuffer(), DestLen);
					LuaStr.TrimBuffer(DestLen);
				}
			}
		}
	}
}

void SystemStrToLuaStr(LPCWSTR SystemStr, CEasyStringA& LuaStr)
{
	if(SystemStr)
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			LuaStr = SystemStr;
		}
		else
		{
			//内码转换
			size_t SrcLen = CEasyStringW::GetStrLen(SystemStr);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				//转成UTF8
				size_t DestLen = UnicodeToUTF8(SystemStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					LuaStr.Resize(DestLen, false);
					UnicodeToUTF8(SystemStr, SrcLen, LuaStr.GetBuffer(), DestLen);
					LuaStr.TrimBuffer(DestLen);
				}
			}
			else
			{
				//转成ANSI
				size_t DestLen = UnicodeToAnsi(SystemStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					LuaStr.Resize(DestLen, false);
					UnicodeToAnsi(SystemStr, SrcLen, LuaStr.GetBuffer(), DestLen);
					LuaStr.TrimBuffer(DestLen);
				}
			}
		}
	}
}

void LuaStrToSystemStr(const char* LuaStr, CEasyStringA& SystemStr)
{
	if(LuaStr)
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			SystemStr = LuaStr;
		}
		else
		{
			//内码转换
			size_t SrcLen = CEasyStringA::GetStrLen(LuaStr);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				//UTF8

				size_t DestLen = UTF8ToAnsi(LuaStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					SystemStr.Resize(DestLen, false);
					UTF8ToAnsi(LuaStr, SrcLen, SystemStr.GetBuffer(), DestLen);
					SystemStr.TrimBuffer(DestLen);
				}
			}
			else
			{
				//ANSI
				size_t DestLen = AnsiToUTF8(SystemStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					SystemStr.Resize(DestLen, false);
					AnsiToUTF8(LuaStr, SrcLen, SystemStr.GetBuffer(), DestLen);
					SystemStr.TrimBuffer(DestLen);
				}
			}
		}
	}
}

void LuaStrToSystemStr(const char* LuaStr, CEasyStringW& SystemStr)
{
	if(LuaStr)
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			SystemStr = LuaStr;
		}
		else
		{
			//内码转换
			size_t SrcLen = CEasyStringA::GetStrLen(LuaStr);
			if (LUA_SCRIPT_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				//UTF8
				size_t DestLen = UTF8ToUnicode(LuaStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					SystemStr.Resize(DestLen, false);
					UTF8ToUnicode(LuaStr, SrcLen, SystemStr.GetBuffer(), DestLen);
					SystemStr.TrimBuffer(DestLen);
				}
			}
			else
			{
				//ANSI
				size_t DestLen = AnsiToUnicode(LuaStr, SrcLen, NULL, 0);
				if (DestLen)
				{
					SystemStr.Resize(DestLen, false);
					AnsiToUnicode(LuaStr, SrcLen, SystemStr.GetBuffer(), DestLen);
					SystemStr.TrimBuffer(DestLen);
				}
			}
		}
	}
}

//int CompareStrOnLuaStack(lua_State* L, int idx, LPCTSTR szStr)
//{
//	LPCTSTR szLuaStr = lua_tostring(L, idx);
//	if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
//	{
//		return CompareString(szLuaStr, szStr);
//	}
//	else
//	{
//		CEasyStringA LuaStr;
//		SystemStrToLuaStr(szLuaStr, LuaStr);
//		return CompareString(LuaStr, szStr);
//	}
//}

CBaseLuaVM::CBaseLuaVM()
	:m_LuaThreadPool(_T("CLuaScript"))
{
	m_LuaStackSize = 0;
	m_ThreadCount = 0;
	m_GrowSize = 0;
	m_GrowLimit = 0;
}


CBaseLuaVM::~CBaseLuaVM()
{
	DestoryThreadPool();
	m_MainThread.Destory();
}

void CBaseLuaVM::Destory()
{

	DestoryThreadPool();
	m_MainThread.Destory();
}

bool CBaseLuaVM::InitThreadPool()
{
	int Type = lua_getglobal(GetLuaState(), "THREAD_POOL");
	lua_pop(GetLuaState(), 1);
	if (Type != LUA_TTABLE)
	{
		lua_newtable(GetLuaState());
		lua_setglobal(GetLuaState(), "THREAD_POOL");
	}

	m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit);	
	LogLuaDebug(_T("创建了%u个lua线程"),
		m_LuaThreadPool.GetBufferSize());
	return true;
}
void CBaseLuaVM::DestoryThreadPool()
{
	if(GetLuaState())
	{
		lua_pushnil(GetLuaState());
		lua_setglobal(GetLuaState(), "THREAD_POOL");
	}
	m_LuaThreadPool.Destory();
}
bool CBaseLuaVM::DeleteScriptThread(CLuaThread * pLuaThread)
{
	return m_LuaThreadPool.DeleteObject(pLuaThread->GetID()) ? true : false;
}

void CBaseLuaVM::DeleteAllScriptThread()
{
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		pLuaThread->Destory();
	}
	m_LuaThreadPool.Clear();
}

CLuaThread * CBaseLuaVM::GetThreadByYeildType(int YeildType)
{
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		if (pLuaThread->GetLastLuaStatus() == LUA_YIELD && pLuaThread->GetYeildType() == YeildType)
		{
			return pLuaThread;
		}
	}
	return NULL;
}
void CBaseLuaVM::ClearAllYeildThread()
{
	void* Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread* pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		if (pLuaThread->GetLastLuaStatus() == LUA_YIELD)
		{
			pLuaThread->Destory();
		}
	}
}
CLuaThread* CBaseLuaVM::AllocLuaThread()
{
	if (GetLuaState() == NULL)
		return NULL;
	if (m_LuaThreadPool.GetObjectCount() == m_LuaThreadPool.GetBufferSize())
	{
		//线程池已满，尝试回收无用线程
		DoGarbageCollect(false);
	}
	CLuaThread* pLuaThread = m_LuaThreadPool.NewObject();
	if (pLuaThread)
	{
		//lua_checkstack(GetLuaState(), m_LuaThreadPool.GetBufferSize());
		if (pLuaThread->Init(this, m_LuaStackSize))
		{
			//调用回调，如果有的话
			lua_pushcclosure(pLuaThread->GetLuaState(), CLuaThread::LuaErrParser, 0);
			int nMsgFn = lua_gettop(pLuaThread->GetLuaState());
			if (lua_getglobal(pLuaThread->GetLuaState(), "__NEW_THREAD_CALLBACKS") == LUA_TTABLE)
			{
				lua_pushnil(pLuaThread->GetLuaState());				
				while (lua_next(pLuaThread->GetLuaState(), -2))
				{
					if (lua_isfunction(pLuaThread->GetLuaState(), -1))
					{
						lua_pushthread(pLuaThread->GetLuaState());
						int Result = lua_pcall(pLuaThread->GetLuaState(), 1, 0, nMsgFn);
						if (Result != LUA_OK)
						{
							if (lua_type(pLuaThread->GetLuaState(), -1) == LUA_TSTRING)
							{
								LogLuaStr(pLuaThread->GetLuaState(), -1, NULL, false);
							}
							else
							{
								LogLuaDirect(_T("回调函数调用失败"));
							}
						}
					}
					else
					{
						pLuaThread->Pop(1);
					}					
				}
			}
			else
			{
				LogLua(_T("表__NEW_THREAD_CALLBACKS不存在"));
			}
			pLuaThread->Pop(2);
			return pLuaThread;
		}
		else
		{
			pLuaThread->Destory();
		}
	}
	else
	{
		LogLua(_T("Thread Pool Full (%u/%u)"), m_LuaThreadPool.GetObjectCount(), m_LuaThreadPool.GetBufferSize());
	}
	return NULL;
}
CLuaThread * CBaseLuaVM::PrepareCall(CBaseScriptHost * pObject, LPCTSTR szObjectName, LPCTSTR szFunctionName)
{
	CLuaThread* pLuaThread = AllocLuaThread();
	if (pLuaThread)
	{		
		if (!pLuaThread->Prepare(pObject, szObjectName, szFunctionName))
		{
			pLuaThread->Destory();
			pLuaThread = NULL;
		}
	}	
	return pLuaThread;
}

CLuaThread* CBaseLuaVM::PrepareCallMember(CLuaBaseMetaClass* pObject, LPCTSTR szFunctionName)
{
	if (szFunctionName == NULL)
		return NULL;
	CLuaThread* pLuaThread = AllocLuaThread();
	if (pLuaThread)
	{
		if (!pObject->PrepareCall(pLuaThread, szFunctionName))
		{
			pLuaThread->Destory();
			pLuaThread = NULL;
		}
	}	
	return pLuaThread;
}

bool CBaseLuaVM::AddCFunction(LPCTSTR LibName, LPCTSTR FuncName, lua_CFunction Function, LPCVOID FuncAddr, UINT FuncSize)
{
	if (GetLuaState() && FuncName && FuncAddr && FuncSize)
	{
		LPCSTR szFuncName;
		CEasyStringA LuaStr;
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
		{
			szFuncName = (LPCSTR)FuncName;
		}
		else
		{
			//内码转换
			SystemStrToLuaStr(FuncName, LuaStr);
			szFuncName = LuaStr;
		}
		if(szFuncName)
		{
			if (LibName)
			{
				LPCSTR szLibName;
				if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
				{
					szLibName = (LPCSTR)LibName;
				}
				else
				{
					//内码转换
					SystemStrToLuaStr(LibName, LuaStr);
					szLibName = LuaStr;
				}

				if (szLibName)
				{
					int Type = lua_getglobal(GetLuaState(), szLibName);
					if (Type != LUA_TTABLE)
					{
						lua_pop(GetLuaState(), 1);
						lua_newtable(GetLuaState());
						lua_setglobal(GetLuaState(), szLibName);
						lua_getglobal(GetLuaState(), szLibName);
					}

					lua_pushstring(GetLuaState(), szFuncName);
					unsigned char* buffer = (unsigned char*)lua_newuserdatauv(GetLuaState(), FuncSize, 0);

					if (buffer)
					{
						memcpy(buffer, FuncAddr, FuncSize);
						lua_pushcclosure(GetLuaState(), Function, 1);
						lua_settable(GetLuaState(), -3);
						lua_pop(GetLuaState(), 1);
						return true;
					}
					else
					{
						lua_pop(GetLuaState(), 2);
					}
				}
			}
			else
			{
				unsigned char* buffer = (unsigned char*)lua_newuserdatauv(GetLuaState(), FuncSize, 0);

				if (buffer)
				{
					memcpy(buffer, FuncAddr, FuncSize);

					lua_pushcclosure(GetLuaState(), Function, 1);

					lua_setglobal(GetLuaState(), szFuncName);

					return true;
				}
			}
		}
	}
	return false;
}

UINT CBaseLuaVM::AddCFunctions(LPCTSTR LibName, const CEasyArray<LUA_CFUN_INFO>& FunctionList)
{
	UINT RegidterCount = 0;
	if (GetLuaState())
	{
		CEasyStringA LuaStr;
		if (LibName)
		{
			LPCSTR szLibName;

			if (LUA_SCRIPT_CODE_PAGE = CEasyString::SYSTEM_STRING_CODE_PAGE)
			{
				szLibName = (LPCSTR)LibName;
			}
			else
			{
				//内码转换
				SystemStrToLuaStr(LibName, LuaStr);
				szLibName = LuaStr;
			}
			if(szLibName)
			{
				int Type = lua_getglobal(GetLuaState(), szLibName);
				if (Type != LUA_TTABLE)
				{
					lua_pop(GetLuaState(), 1);
					lua_newtable(GetLuaState());
					lua_setglobal(GetLuaState(), szLibName);
					lua_getglobal(GetLuaState(), szLibName);
				}

				for (const LUA_CFUN_INFO& FuncInfo : FunctionList)
				{
					if (FuncInfo.FunName && FuncInfo.FunAddr && FuncInfo.FunSize)
					{
						LPCSTR szFuncName;

						if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
						{
							szFuncName = (LPCSTR)((LPCTSTR)FuncInfo.FunName);
						}
						else
						{
							//内码转换
							SystemStrToLuaStr(FuncInfo.FunName, LuaStr);
							szFuncName = LuaStr;
						}

						if(szFuncName)
						{
							lua_pushstring(GetLuaState(), szFuncName);
							unsigned char* buffer = (unsigned char*)lua_newuserdatauv(GetLuaState(), FuncInfo.FunSize, 0);

							if (buffer)
							{
								memcpy(buffer, FuncInfo.FunAddr, FuncInfo.FunSize);
								lua_pushcclosure(GetLuaState(), FuncInfo.pLuaCFun, 1);
								lua_settable(GetLuaState(), -3);
								RegidterCount++;
							}
							else
							{
								lua_pop(GetLuaState(), 1);
							}
						}
					}
				}
				lua_pop(GetLuaState(), 1);
			}
		}
		else
		{
			for (const LUA_CFUN_INFO& FuncInfo : FunctionList)
			{
				if (FuncInfo.FunName && FuncInfo.FunAddr && FuncInfo.FunSize)
				{

					LPCSTR szFuncName;
					if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
					{
						szFuncName = (LPCSTR)((LPCTSTR)FuncInfo.FunName);
					}
					else
					{
						//内码转换
						SystemStrToLuaStr(FuncInfo.FunName, LuaStr);
						szFuncName = LuaStr;
					}
					if (szFuncName)
					{
						unsigned char* buffer = (unsigned char*)lua_newuserdatauv(GetLuaState(), FuncInfo.FunSize, 0);

						if (buffer)
						{
							memcpy(buffer, FuncInfo.FunAddr, FuncInfo.FunSize);
							lua_pushcclosure(GetLuaState(), FuncInfo.pLuaCFun, 1);
							lua_setglobal(GetLuaState(), szFuncName);
							RegidterCount++;
						}
					}
				}
			}
		}
	}
	return RegidterCount;
}

int CBaseLuaVM::DoGarbageCollect(bool PrintStatus)
{
	int Count = 0;
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		if ((pLuaThread->GetLastLuaStatus() != LUA_YIELD) && (pLuaThread->GetLastLuaStatus() != LUA_EXECUTING))
		{
			pLuaThread->Destory();
			Count++;
		}
	}
	if (GetLuaState())
	{
		lua_gc(GetLuaState(), LUA_GCCOLLECT);
		if (PrintStatus)
		{
			int MemUsedK = lua_gc(GetLuaState(), LUA_GCCOUNT);
			int MemUsed= lua_gc(GetLuaState(), LUA_GCCOUNTB);
			//LogLua(_T("mem used=%dK%d"), MemUsedK, MemUsed);
		}
	}		
	return Count;
}

void CBaseLuaVM::AddLoadedModule(const char* szModuleName)
{
	AddUnique(m_LoadedModules, CEasyStringA(szModuleName));
}

void CBaseLuaVM::ClearLoadedModule()
{
	if (GetLuaState())
	{
		lua_getglobal(GetLuaState(), LUA_LOADLIBNAME);
		if (lua_istable(GetLuaState(), -1))
		{
			lua_getfield(GetLuaState(), -1, "loaded");
			if (lua_istable(GetLuaState(), -1))
			{
				for (CEasyStringA& ModuleName : m_LoadedModules)
				{
					lua_pushstring(GetLuaState(), ModuleName);
					lua_pushnil(GetLuaState());
					lua_rawset(GetLuaState(), -3);
					CEasyString Temp;
					LuaStrToSystemStr(ModuleName, Temp);
					//LogLuaDebug(_T("已移除模块%s的缓冲"), (LPCTSTR)Temp);
				}
				m_LoadedModules.Clear();
			}
			lua_pop(GetLuaState(), 1);
		}
		lua_pop(GetLuaState(), 1);
	}
}



int CBaseLuaVM::Update(int ProcessLimit)
{
	int ProcessCount = 0;
	void* Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread* pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		ProcessCount += pLuaThread->Update(ProcessLimit);
	}	
	return ProcessCount;
}

void CBaseLuaVM::InitEnv()
{
	lua_atpanic(GetLuaState(), &CBaseLuaVM::LuaPanic);
	luaL_openlibs(GetLuaState());	
	RegisterCustomLoader(CBaseLuaVM::ExtendModuleSearcher, CBaseLuaVM::ExtendModuleLoader);
	CLuaLibCommon::GetInstance()->RegisterToVM(this);
	//重定向print
	if (GetLuaState())
	{
		lua_pushcfunction(GetLuaState(), CBaseLuaVM::LuaPrint);
		lua_setglobal(GetLuaState(), "print");
	}
	//创建弱表__ALL_OBJECTS用来存放所有对象
	lua_newtable(GetLuaState());
	//设置为弱表
	lua_newtable(GetLuaState());
	lua_pushstring(GetLuaState(), "__mode");
	lua_pushstring(GetLuaState(), "v");
	lua_settable(GetLuaState(), -3);
	lua_setmetatable(GetLuaState(), -2);
	lua_setglobal(GetLuaState(), "__ALL_OBJECTS");

	//创建弱表__INSIDE_OBJECTS用来存放内部对象
	lua_newtable(GetLuaState());
	//设置为弱表
	lua_newtable(GetLuaState());
	lua_pushstring(GetLuaState(), "__mode");
	lua_pushstring(GetLuaState(), "v");
	lua_settable(GetLuaState(), -3);
	lua_setmetatable(GetLuaState(), -2);
	lua_setglobal(GetLuaState(), "__INSIDE_OBJECTS");

	//创建表__OUTSIDE_OBJECTS用来存放外部对象
	lua_newtable(GetLuaState());	
	lua_setglobal(GetLuaState(), "__OUTSIDE_OBJECTS");

	//创建表__NEW_THREAD_CALLBACKS用来存放新建线程时的回调函数
	lua_newtable(GetLuaState());
	lua_setglobal(GetLuaState(), "__NEW_THREAD_CALLBACKS");

	//创建表__PERSISTENT_OBJECTS用来存放持久对象
	lua_newtable(GetLuaState());
	lua_setglobal(GetLuaState(), "__PERSISTENT_OBJECTS");

	CLuaByteArray::ResgisterStaticFunctions(this);
	CLuaGrid::ResgisterStaticFunctions(this);
	CLuaSmartValue::ResgisterStaticFunctions(this);
	CLuaSmartStruct::ResgisterStaticFunctions(this);
	CLuaSmartArray::ResgisterStaticFunctions(this);
}


int CBaseLuaVM::LuaPrint(lua_State* L)
{
	int ParamCount = lua_gettop(L);
	if (ParamCount)
	{
		CStringBuilder Buffer;
		for (int i = 1; i <= ParamCount; i++)
		{
			DumpLuaValue(L, i, Buffer, _T(""), false, 0);
		}
		CEasyString Temp;
		UINT StartIndex = 0;
		while (StartIndex < Buffer.GetLength())
		{
			StartIndex += Buffer.GetStr(Temp, StartIndex, 3000);
			LogLuaDirect((LPCTSTR)Temp);
		}
	}
	return 0;
}

void CBaseLuaVM::DumpLuaValue(lua_State* L, int Index, CStringBuilder& OutBugger, LPCTSTR PrevStr, bool ForceSameLine, int Deep)
{
	LPCTSTR pPrevStr = _T("");
	if ((!ForceSameLine) && OutBugger.GetLength())
	{
		OutBugger.Append(_T("\r\n"));
		pPrevStr = PrevStr;
	}		
		
	Deep++;
	if (Deep > 8)
	{
		OutBugger.Append(_T("reach max deep"));
		return;
	}	
	int Type = GetLuaObjectType(L, Index);
	CEasyString Temp, Space;
	switch (Type)
	{
	case LUA_TNIL:
		Temp.Format(_T("%snil"), pPrevStr);
		OutBugger.Append(Temp);
		break;
	case LUA_TBOOLEAN:
		if (lua_toboolean(L, Index))
			Temp.Format(_T("%strue"), pPrevStr);
		else
			Temp.Format(_T("%sfalse"), pPrevStr);
		OutBugger.Append(Temp);
		break;
	case LUA_TLIGHTUSERDATA:
		Temp.Format(_T("%sLightUserData=0x%p"), pPrevStr, lua_touserdata(L, Index));
		OutBugger.Append(Temp);
		break;
	case LUA_TNUMBER:
		Temp.Format(_T("%s%f"), pPrevStr, lua_tonumber(L, Index));
		OutBugger.Append(Temp);
		break;
	case LUA_TSTRING:
		{
			CEasyString StrValue;
			LuaStrToSystemStr(lua_tostring(L, Index), StrValue);
			Temp.Format(_T("%s%s"), pPrevStr, (LPCTSTR)StrValue);
			OutBugger.Append(Temp);
		}
		break;
	case LUA_TTABLE:
		{
			lua_checkstack(L, 2);
			Temp.Format(_T("%s["), pPrevStr);
			OutBugger.Append(Temp);
			lua_pushnil(L);
			Space.Format(_T("%s    "), PrevStr);
			bool HaveChild = false;
			while (lua_next(L, Index) != 0)
			{
				DumpLuaValue(L, lua_gettop(L) - 1, OutBugger, Space, false, Deep);
				OutBugger.Append(_T(" : "));
				DumpLuaValue(L, lua_gettop(L), OutBugger, Space, true, Deep);
				lua_pop(L, 1);
				HaveChild = true;
			}
			if (HaveChild)
				Temp.Format(_T("\r\n%s    ]"), PrevStr);
			else
				Temp.Format(_T("]"));
			OutBugger.Append(Temp);
		}
		break;
	case LUA_TFUNCTION:
		{
			lua_pushvalue(L, Index);
			lua_Debug Info;
			lua_getinfo(L, ">Sn", &Info);
			Temp.Format(_T("%sFunction=%s %s @%s(%d-%d)"), pPrevStr, Info.namewhat, Info.name, Info.short_src, Info.linedefined, Info.lastlinedefined);
			OutBugger.Append(Temp);
		}
		break;
	case LUA_TUSERDATA:
		Temp.Format(_T("%sUserData"), pPrevStr);
		OutBugger.Append(Temp);
		break;
	case LUA_TTHREAD:
		Temp.Format(_T("%sThread"), pPrevStr);
		OutBugger.Append(Temp);
		break;
	case LUA_TINTEGER:
		Temp.Format(_T("%s%lld"), pPrevStr, lua_tointeger(L, Index));
		OutBugger.Append(Temp);
		break;
	default:
		if (Type >= CLuaBaseMetaClass::CLASS_ID)
		{
			CLuaBaseMetaClass* pObject = *((CLuaBaseMetaClass**)lua_touserdata(L, Index));
			if(pObject)
			{
				CEasyString ObjDump;
				pObject->Dump(ObjDump);
				Temp.Format(_T("%s%s"), pPrevStr, (LPCTSTR)ObjDump);
				OutBugger.Append(Temp);
			}
		}		
		else
		{
			Temp.Format(_T("%sUnknow"), pPrevStr);
			OutBugger.Append(Temp);
		}
	}	
}


bool CBaseLuaVM::IsObjectExist(CLuaBaseMetaClass* pObject)
{
	if (lua_getglobal(m_MainThread.GetLuaState(), "__ALL_OBJECTS") == LUA_TTABLE)
	{
		lua_pushlightuserdata(m_MainThread.GetLuaState(), pObject);
		int Type = lua_gettable(m_MainThread.GetLuaState(), -2);
		m_MainThread.Pop(2);
		return Type == LUA_TUSERDATA;
	}
	return false;
}

bool CBaseLuaVM::AddNewThreadCallback(lua_State* L, int Index)
{
	if (lua_isfunction(L, Index))
	{
		if (lua_getglobal(L, "__NEW_THREAD_CALLBACKS") == LUA_TTABLE)
		{
			lua_pushvalue(L, Index);
			lua_pushvalue(L, Index);
			lua_settable(L, -3);
			LogLuaDebug(_T("已添加线程创建回调"));
		}
		else
		{
			LogLua(_T("表__NEW_THREAD_CALLBACKS不存在"));
		}
		lua_pop(L, 1);
	}
	else
	{
		LogLua(_T("callback not function"));
	}
	return false;
}

int CBaseLuaVM::AddPersistentObject(lua_State* L, int Index)
{
	Index = lua_absindex(L, Index);
	if (lua_getglobal(L, "__PERSISTENT_OBJECTS") == LUA_TTABLE)
	{
		lua_pushvalue(L, Index);
		int ID = luaL_ref(L, -2);
		lua_pop(L, 1);
		return ID;
	}
	else
	{
		LogLua(_T("表__PERSISTENT_OBJECTS不存在"));
		lua_pop(L, 1);
		return LUA_NOREF;
	}	
}

int CBaseLuaVM::PushPersistentObject(lua_State* L, int ID)
{
	if (lua_getglobal(L, "__PERSISTENT_OBJECTS") == LUA_TTABLE)
	{
		lua_rawgeti(L, -1, ID);
		lua_remove(L, -2);
		return lua_type(L,-1);
	}
	else
	{
		LogLua(_T("表__PERSISTENT_OBJECTS不存在"));
		lua_pop(L, 1);
		lua_pushnil(L);
		return LUA_TNIL;
	}
}

bool CBaseLuaVM::RemovePersistentObject(lua_State* L, int ID)
{
	if (lua_getglobal(L, "__PERSISTENT_OBJECTS") == LUA_TTABLE)
	{
		luaL_unref(L, -1, ID);
		lua_pop(L, 1);
		return true;
	}
	else
	{
		LogLua(_T("表__PERSISTENT_OBJECTS不存在"));
		lua_pop(L, 1);
		return false;
	}
}

int CBaseLuaVM::LuaPanic(lua_State* L)
{
	if (lua_isstring(L, -1))
	{
		if (LUA_SCRIPT_CODE_PAGE == CEasyString::SYSTEM_STRING_CODE_PAGE)
			LogLua(_T("lua抛出异常:%s"), lua_tostring(L, -1));
		else
			LogLua(_T("lua抛出异常:%s"), (LPCTSTR)LuaGetString(L, -1));
	}
	else
	{
		LogLua(_T("lua抛出异常"));
	}
	LogLuaStack(L, _T("Stack:"));
	throw LUA_EXCEPTION();
	return 0;
}



int CBaseLuaVM::ExtendModuleSearcher(LPCTSTR ModelName)
{
	if (CompareString(ModelName, _T("socket.core")) == 0)
	{
		return true;
	}
	else if (CompareString(ModelName, _T("mime.core")) == 0)
	{
		return true;
	}
	return false;
}
int CBaseLuaVM::ExtendModuleLoader(lua_State* L, LPCTSTR ModelName)
{
	if (CompareString(ModelName, _T("socket.core")) == 0)
	{
		luaopen_socket_core(L);
	}
	else if (CompareString(ModelName, _T("mime.core")) == 0)
	{
		luaopen_mime_core(L);
	}
	else
	{
		lua_pushnil(L);
	}
	return 1;
}