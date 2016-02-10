#include "stdafx.h"

UINT CLuaScript::m_LogChannel = 0;
CLuaScript::CLuaScript()
{
	m_NeedReload = false;
	m_IsLoadFromFile = false;
	m_IsSharedLuaState = false;
	m_ScriptID = 0;
	m_pLuaState = NULL;
	m_LuaStackSize = 0;
	m_ThreadCount = 0;
	m_GrowSize = 0;
	m_GrowLimit = 0;
	m_pFunctionList = NULL;
}
CLuaScript::CLuaScript(const CLuaScript& LuaScript, int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit)
{
	CloneFrom(LuaScript, LuaStackSize, ThreadCount, GrowSize, GrowLimit);
}

CLuaScript::~CLuaScript()
{
	Destory();
}

bool CLuaScript::Init(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO> * pFunctionList, int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit)
{
	Destory();

	m_ScriptName = ScriptName;
	m_ScriptContent = ScriptContent;
	m_LuaStackSize = LuaStackSize;
	m_ThreadCount = ThreadCount;
	m_GrowSize = GrowSize;
	m_GrowLimit = GrowLimit;
	m_pFunctionList = pFunctionList;

	return LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
}

CLuaThread * CLuaScript::AllocScriptThread(CBaseScriptHost * pObject, LPCTSTR szFunctionName)
{
	if (m_pLuaState == NULL)
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
		if (!pLuaThread->IsInited())
		{
			pLuaThread->Init(this, m_ScriptID, m_LuaStackSize);
		}
		if (!pLuaThread->Prepare(pObject, szFunctionName))		
		{
			m_LuaThreadPool.DeleteObject(pLuaThread->GetID());
			pLuaThread = NULL;
		}
	}
	else
	{
		LogLua(_T("CLuaScript::AllocScriptThread:Thread Pool Full (%u/%u)"), m_LuaThreadPool.GetObjectCount(), m_LuaThreadPool.GetBufferSize());
	}
	return pLuaThread;
}

bool CLuaScript::DeleteScriptThread(CLuaThread * pLuaThread)
{	
	return m_LuaThreadPool.DeleteObject(pLuaThread->GetID()) ? true : false;
}
void CLuaScript::DeleteAllScriptThread()
{
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNext(Pos);
		pLuaThread->Release();
	}
	m_LuaThreadPool.Clear();
}
CLuaThread * CLuaScript::GetThreadByYeildType(int YeildType)
{
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNext(Pos);
		if (pLuaThread->GetLastLuaStatus() == LUA_YIELD&&pLuaThread->GetYeildType() == YeildType)
		{
			return pLuaThread;
		}
	}
	return NULL;
}
void CLuaScript::ReloadScript(LPCTSTR ScriptContent)
{
	m_NeedReload = true;
	m_ReloadScriptContent = ScriptContent;	
}

void CLuaScript::Destory()
{
	m_NeedReload = false;
	m_ScriptContent.Clear();
	m_ReloadScriptContent.Clear();
	CloseLuaState();		
}

void CLuaScript::CloneFrom(const CLuaScript& LuaScript, int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit)
{
	Destory();
	m_NeedReload = LuaScript.m_NeedReload;
	m_IsLoadFromFile = LuaScript.m_IsLoadFromFile;
	m_ScriptID = LuaScript.m_ScriptID;
	m_ScriptName = LuaScript.m_ScriptName;
	m_ScriptContent = LuaScript.m_ScriptContent;
	if (LuaStackSize)
		m_LuaStackSize = LuaStackSize;
	else
		m_LuaStackSize = LuaScript.m_LuaStackSize;
	if (ThreadCount)
	{
		m_ThreadCount = ThreadCount;
		m_GrowSize = GrowSize;
		m_GrowLimit = GrowLimit;
	}
	else
	{
		m_ThreadCount = LuaScript.m_ThreadCount;
		m_GrowSize = LuaScript.m_GrowSize;
		m_GrowLimit = LuaScript.m_GrowLimit;
	}	
	m_ReloadScriptContent = LuaScript.m_ReloadScriptContent;
	m_pFunctionList = LuaScript.m_pFunctionList;

	
	LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
}

void CLuaScript::Update()
{
	if (m_ThreadRecyleTimer.IsTimeOut(LUA_THREAD_RECYLE_CHECK_TIME))
	{
		m_ThreadRecyleTimer.SaveTime();
		RecycleThread();
	}
	if (m_NeedReload)
	{
		bool CanReload = true;
		void * Pos = m_LuaThreadPool.GetFirstObjectPos();
		while (Pos)
		{
			CLuaThread * pLuaThread = m_LuaThreadPool.GetNext(Pos);
			if (pLuaThread->GetLastLuaStatus() == LUA_YIELD)
			{
				CanReload = false;
				break;
			}			
		}
		if (CanReload)
		{
			m_NeedReload = false;
			if (!m_IsSharedLuaState)
			{
				if (LoadScript(m_ScriptName, m_ReloadScriptContent, m_pFunctionList))
				{
					LogLuaDebug(_T("CLuaScript::Update:已重新加载脚本[%s]"), (LPCTSTR)m_ScriptName);
				}
				else
				{
					LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
					LogLua(_T("CLuaScript::Update:重新加载脚本[%s]失败，返回加载源脚本"), (LPCTSTR)m_ScriptName);
				}
			}
		}
	}
}
//bool CLuaScript::PopLuaThread(lua_State * pThreadState)
//{
//	if (m_pLuaState == NULL)
//		return false;
//	for (int i = 1; i <= lua_gettop(m_pLuaState); i++)
//	{
//		if (lua_isthread(m_pLuaState, i))
//		{
//			if (lua_tothread(m_pLuaState, i) == pThreadState)
//			{
//				lua_remove(m_pLuaState, i);
//				return true;
//			}
//		}
//	}
//	LogLua(_T("CLuaScript::Update:无法弹出线程"));
//	return false;
//}
void CLuaScript::CloseLuaState()
{
	m_LuaThreadPool.Destory();
	if (m_pLuaState)
	{
		if (!m_IsSharedLuaState)
			lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	m_IsSharedLuaState = false;
}
bool CLuaScript::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO>* pFunctionList)
{	
	CloseLuaState();
	m_pLuaState = luaL_newstate();
	if (m_pLuaState)
	{
		luaL_openlibs(m_pLuaState);
		//luaopen_bit32(m_pLuaState);
		//luaopen_base(m_pLuaState);
		//luaopen_string(m_pLuaState);
		//luaopen_math(m_pLuaState);

		if (pFunctionList)
		{
			for (size_t i = 0; i < pFunctionList->GetCount(); i++)
			{
#ifdef UNICODE
				CEasyStringA FunName;
				FunName = pFunctionList->GetObject(i)->FunName;
				RegisterLuaCFun(m_pLuaState, FunName, pFunctionList->GetObject(i)->pLuaCFun,
					pFunctionList->GetObject(i)->FunAddr, pFunctionList->GetObject(i)->FunSize);
#else
				RegisterLuaCFun(m_pLuaState, pFunctionList->GetObject(i)->FunName, pFunctionList->GetObject(i)->pLuaCFun,
					pFunctionList->GetObject(i)->FunAddr, pFunctionList->GetObject(i)->FunSize);
#endif
			}
		}
#ifdef UNICODE
		CEasyStringA ScriptContentA,ScriptNameA;
		ScriptContentA = ScriptContent;
		ScriptNameA = ScriptName;
		if (luaL_dostring(m_pLuaState, ScriptContentA, ScriptNameA) == 0)
#else
		if (luaL_dostring(m_pLuaState, ScriptContent, ScriptName) == 0)
#endif
		{
			lua_checkstack(m_pLuaState, m_ThreadCount);
			m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit);
			for (UINT ID = 1; ID <= m_LuaThreadPool.GetBufferSize();ID++)
			{
				CLuaThread * pLuaThread = m_LuaThreadPool.GetObject(ID);
				if (pLuaThread)
				{
					pLuaThread->Init(this, m_ScriptID, m_LuaStackSize);
				}
			}
			LogLuaDebug(_T("CLuaScript::LoadScript:[%s]创建了%u个lua线程"), 
				(LPCTSTR)m_ScriptName,
				m_LuaThreadPool.GetBufferSize());

			return true;
		}
		else
		{
			if (lua_type(m_pLuaState, -1) == LUA_TSTRING)
				LogLua(_T("CLuaScript::InitLuaScript:%s"), lua_tolstring(m_pLuaState, -1, NULL));
		}
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	else
	{
		LogLua(_T("CLuaScript::LoadScript:初始化lua失败"));
	}
	return false;
}

void CLuaScript::RegisterLuaCFun(lua_State * pLuaState, const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc)
{
	unsigned char* buffer = (unsigned char*)lua_newuserdata(pLuaState, sizeofFunc);

	memcpy(buffer, func, sizeofFunc), buffer += sizeofFunc;

	lua_pushcclosure(pLuaState, function, 1);

	lua_setglobal(pLuaState, funcName);
}

void CLuaScript::RecycleThread()
{	
	void * Pos = m_LuaThreadPool.GetFirstObjectPos();
	while (Pos)
	{
		CLuaThread * pLuaThread = m_LuaThreadPool.GetNext(Pos);
		if (pLuaThread->GetLastLuaStatus() != LUA_YIELD)
		{
			m_LuaThreadPool.DeleteObject(pLuaThread->GetID());
			pLuaThread->Release();
		}
	}
}