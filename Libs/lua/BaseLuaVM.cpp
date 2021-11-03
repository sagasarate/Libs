#include "stdafx.h"

UINT g_LuaLogChannel = 0;

CBaseLuaVM::CBaseLuaVM()
	:m_LuaThreadPool(_T("CLuaScript"))
{
	m_pLuaState = NULL;
	m_LuaStackSize = 0;
	m_ThreadCount = 0;
	m_GrowSize = 0;
	m_GrowLimit = 0;
}


CBaseLuaVM::~CBaseLuaVM()
{
}

bool CBaseLuaVM::InitThreadPool()
{
	int Type = lua_getglobal(m_pLuaState, "THREAD_POOL");
	lua_pop(m_pLuaState, 1);
	if (Type != LUA_TTABLE)
	{
		lua_newtable(m_pLuaState);
		lua_setglobal(m_pLuaState, "THREAD_POOL");
	}

	m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit);	
	LogLuaDebug(_T("创建了%u个lua线程"),
		m_LuaThreadPool.GetBufferSize());
	return true;
}
void CBaseLuaVM::DestoryThreadPool()
{
	lua_pushnil(m_pLuaState);
	lua_setglobal(m_pLuaState, "THREAD_POOL");
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
CLuaThread * CBaseLuaVM::AllocScriptThread(CBaseScriptHost * pObject, LPCTSTR szFunctionName)
{
	if (m_pLuaState == NULL)
		return NULL;
	if (szFunctionName == NULL)
		return NULL;

	if (m_LuaThreadPool.GetObjectCount() == m_LuaThreadPool.GetBufferSize())
	{
		//线程池已满，尝试回收无用线程
		RecycleThread();
	}
	CLuaThread * pLuaThread = m_LuaThreadPool.NewObject();
	if (pLuaThread)
	{
		lua_checkstack(m_pLuaState, m_LuaThreadPool.GetBufferSize());
		pLuaThread->Init(this, m_LuaStackSize);
		if (!pLuaThread->Prepare(pObject, szFunctionName))
		{
			pLuaThread->Destory();
			pLuaThread = NULL;
		}
	}
	else
	{
		LogLua(_T("Thread Pool Full (%u/%u)"), m_LuaThreadPool.GetObjectCount(), m_LuaThreadPool.GetBufferSize());
	}
	return pLuaThread;
}

bool CBaseLuaVM::AddCFunction(LPCTSTR LibName, LPCTSTR FuncName, lua_CFunction Function, LPCVOID FuncAddr, UINT FuncSize)
{
	if (m_pLuaState&&FuncName&&FuncAddr&&FuncSize)
	{
		LPCSTR szFuncName;
#ifdef UNICODE
		CEasyStringA StrFuncName;
		StrFuncName = FuncName;
		szFuncName = StrFuncName;
#else
		szFuncName = FuncName;
#endif

		if (LibName)
		{
			LPCSTR szLibName;
#ifdef UNICODE
			CEasyStringA StrLibName;
			StrLibName = LibName;
			szLibName = StrLibName;
#else
			szLibName = LibName;
#endif

			int Type = lua_getglobal(m_pLuaState, szLibName);
			if (Type != LUA_TTABLE)
			{
				lua_pop(m_pLuaState, 1);
				lua_newtable(m_pLuaState);
				lua_setglobal(m_pLuaState, szLibName);
				lua_getglobal(m_pLuaState, szLibName);
			}
			
			lua_pushstring(m_pLuaState, szFuncName);
			unsigned char* buffer = (unsigned char*)lua_newuserdata(m_pLuaState, FuncSize);

			if (buffer)
			{
				memcpy(buffer, FuncAddr, FuncSize);
				lua_pushcclosure(m_pLuaState, Function, 1);
				lua_settable(m_pLuaState, -3);
				lua_pop(m_pLuaState, 1);
				return true;
			}
			else
			{
				lua_pop(m_pLuaState, 2);
			}
		}
		else
		{
			unsigned char* buffer = (unsigned char*)lua_newuserdata(m_pLuaState, FuncSize);

			if (buffer)
			{
				memcpy(buffer, FuncAddr, FuncSize);

				lua_pushcclosure(m_pLuaState, Function, 1);

				lua_setglobal(m_pLuaState, szFuncName);

				return true;
			}
		}
		
	}
	return false;
}

UINT CBaseLuaVM::AddCFunctions(const char * LibName, const CEasyArray<LUA_CFUN_INFO>& FunctionList)
{
	UINT RegidterCount = 0;
	if (m_pLuaState)
	{
		if (LibName)
		{
			LPCSTR szLibName;
#ifdef UNICODE
			CEasyStringA StrLibName;
			StrLibName = LibName;
			szLibName = StrLibName;
#else
			szLibName = LibName;
#endif
			int Type = lua_getglobal(m_pLuaState, szLibName);
			if (Type != LUA_TTABLE)
			{
				lua_pop(m_pLuaState, 1);
				lua_newtable(m_pLuaState);
				lua_setglobal(m_pLuaState, szLibName);
				lua_getglobal(m_pLuaState, szLibName);
			}

			for (const LUA_CFUN_INFO& FuncInfo : FunctionList)
			{
				if (FuncInfo.FunName&&FuncInfo.FunAddr&&FuncInfo.FunSize)
				{
					LPCSTR szFuncName;
#ifdef UNICODE
					CEasyStringA StrFuncName;
					StrFuncName = (LPCTSTR)FuncInfo.FunName;
					szFuncName = StrFuncName;
#else
					szFuncName = FuncInfo.FunName;
#endif

					lua_pushstring(m_pLuaState, szFuncName);
					unsigned char* buffer = (unsigned char*)lua_newuserdata(m_pLuaState, FuncInfo.FunSize);

					if (buffer)
					{
						memcpy(buffer, FuncInfo.FunAddr, FuncInfo.FunSize);
						lua_pushcclosure(m_pLuaState, FuncInfo.pLuaCFun, 1);
						lua_settable(m_pLuaState, -3);
						RegidterCount++;
					}
					else
					{
						lua_pop(m_pLuaState, 1);
					}
				}
			}
			lua_pop(m_pLuaState, 1);
		}
		else
		{
			for (const LUA_CFUN_INFO& FuncInfo : FunctionList)
			{
				if (FuncInfo.FunName&&FuncInfo.FunAddr&&FuncInfo.FunSize)
				{

					LPCSTR szFuncName;
#ifdef UNICODE
					CEasyStringA StrFuncName;
					StrFuncName = (LPCTSTR)FuncInfo.FunName;
					szFuncName = StrFuncName;
#else
					szFuncName = FuncInfo.FunName;
#endif

					unsigned char* buffer = (unsigned char*)lua_newuserdata(m_pLuaState, FuncInfo.FunSize);

					if (buffer)
					{
						memcpy(buffer, FuncInfo.FunAddr, FuncInfo.FunSize);
						lua_pushcclosure(m_pLuaState, FuncInfo.pLuaCFun, 1);
						lua_setglobal(m_pLuaState, szFuncName);
						RegidterCount++;
					}
				}
			}
		}
	}
	return RegidterCount;
}

void CBaseLuaVM::RecycleThread()
{
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
		if (pLuaThread->GetLastLuaStatus() != LUA_YIELD)
		{
			pLuaThread->Destory();
		}
	}
	if (m_pLuaState)
		lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);
}

