#include "stdafx.h"

UINT CLuaScript::m_LogChannel = 0;
CLuaScript::CLuaScript()
{
	m_NeedReload = false;
	m_IsLoadFromFile = false;
	m_IsCloneScript = false;
	m_ScriptID = 0;
	m_pLuaState = NULL;
	m_LuaStackSize = 0;
	m_ThreadCount = 0;
	m_GrowSize = 0;
	m_GrowLimit = 0;
	m_pFunctionList = NULL;
}
CLuaScript::CLuaScript(const CLuaScript& LuaScript, bool ShareLuaState)
{
	m_NeedReload = LuaScript.m_NeedReload;
	m_IsLoadFromFile = LuaScript.m_IsLoadFromFile;	
	m_ScriptID = LuaScript.m_ScriptID;
	m_ScriptName = LuaScript.m_ScriptName;
	m_ScriptContent = LuaScript.m_ScriptName;
	m_LuaStackSize = LuaScript.m_LuaStackSize;
	m_ThreadCount = LuaScript.m_ThreadCount;
	m_GrowSize = LuaScript.m_GrowSize;
	m_GrowLimit = LuaScript.m_GrowLimit;
	m_ReloadScriptContent = LuaScript.m_ReloadScriptContent;
	m_pFunctionList = LuaScript.m_pFunctionList;

	if (ShareLuaState)
	{
		m_IsCloneScript = true;
		m_pLuaState = LuaScript.m_pLuaState;
		m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit);
	}
	else
	{
		if (m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit))
		{
			LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
		}
	}
	
	
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

	if (m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit))
	{
		return LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
	}
	return false;	
}

CLuaThread * CLuaScript::AllocScriptThread(CBaseScriptHost * pObject, LPCTSTR szFunctionName)
{
	CLuaThread * pLuaThread = m_LuaThreadPool.NewObject();
	if (pLuaThread)
	{
		if (!pLuaThread->IsInit())
		{
			pLuaThread->Init(this, m_pLuaState, 0);
		}		
		if (!pLuaThread->Prepare(pObject, szFunctionName))		
		{
			m_LuaThreadPool.DeleteObject(pLuaThread->GetID());
			pLuaThread = NULL;
		}
	}
	else
	{
		Log(_T("CLuaScript::AllocScriptThread:Thread Pool Full (%u/%u)"), m_LuaThreadPool.GetObjectCount(), m_LuaThreadPool.GetBufferSize());
	}
	return pLuaThread;
}

bool CLuaScript::DeleteScriptThread(CLuaThread * pLuaThread)
{
	pLuaThread->Destory();
	return m_LuaThreadPool.DeleteObject(pLuaThread->GetID());
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
	if (m_pLuaState)
	{
		if (!m_IsCloneScript)
			lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	m_LuaThreadPool.Clear();
}

void CLuaScript::Update()
{
	if (m_ThreadRecyleTimer.IsTimeOut(LUA_THREAD_RECYLE_CHECK_TIME))
	{
		m_ThreadRecyleTimer.SaveTime();
		void * Pos = m_LuaThreadPool.GetFirstObjectPos();
		{
			CLuaThread * pLuaThread = m_LuaThreadPool.GetNext(Pos);
			if (pLuaThread->GetLastLuaStatus() != LUA_YIELD)			
			{
				m_LuaThreadPool.DeleteObject(pLuaThread->GetID());
			}
		}
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
			if (!m_IsCloneScript)
			{
				if (!LoadScript(m_ScriptName, m_ReloadScriptContent, m_pFunctionList))
				{
					LoadScript(m_ScriptName, m_ScriptContent, m_pFunctionList);
					Log(_T("CLuaScript::Update:重新加载脚本失败，返回加载源脚本"));
				}
			}
		}
	}
}

bool CLuaScript::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent, CEasyArray<LUA_CFUN_INFO>* pFunctionList)
{	
	if (m_pLuaState)
	{
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	m_pLuaState = luaL_newstate();
	if (m_pLuaState)
	{
		luaopen_base(m_pLuaState);
		luaopen_string(m_pLuaState);
		luaopen_math(m_pLuaState);

		if (pFunctionList)
		{
			for (size_t i = 0; i < pFunctionList->GetCount(); i++)
			{
#ifdef UNICODE
				CEasyStringA FunName;
				FunName=pFunctionList->GetObject(i)->FunName;
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
		ScriptContentA=ScriptContent;
		ScriptNameA = ScriptName;
		if (luaL_dostring(m_pLuaState, ScriptContentA, ScriptNameA) == 0)
#else
		if (luaL_dostring(m_pLuaState, ScriptContent, ScriptName) == 0)
#endif
		{
			lua_checkstack(m_pLuaState, m_LuaStackSize);
			m_LuaThreadPool.Create(m_ThreadCount, m_GrowSize, m_GrowLimit);
			LogDebug(_T("CLuaScript::LoadScript:创建了%u个lua线程"),
				m_LuaThreadPool.GetBufferSize());

			return true;
		}
		else
		{
			if (lua_type(m_pLuaState, -1) == LUA_TSTRING)
				Log(_T("CLuaScript::InitLuaScript:%s"), lua_tolstring(m_pLuaState, -1, NULL));
		}
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	else
	{
		Log(_T("CLuaScript::LoadScript:初始化lua失败"));
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