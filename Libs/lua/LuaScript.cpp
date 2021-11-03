#include "stdafx.h"

CLuaScript::CLuaScript()
	
{
	m_NeedReload = false;
	m_IsLoadFromFile = false;
	m_IsSharedLuaState = false;
	m_ScriptID = 0;	
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
	if (pFunctionList)
		m_CFunctionList = *pFunctionList;

	return LoadScript(m_ScriptName, m_ScriptContent);
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
	m_CFunctionList.Clear();
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
	m_CFunctionList = LuaScript.m_CFunctionList;

	
	LoadScript(m_ScriptName, m_ScriptContent);
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
			CLuaThread * pLuaThread = m_LuaThreadPool.GetNextObject(Pos);
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
				if (LoadScript(m_ScriptName, m_ReloadScriptContent))
				{
					LogLuaDebug(_T("已重新加载脚本[%s]"), (LPCTSTR)m_ScriptName);
				}
				else
				{
					LoadScript(m_ScriptName, m_ScriptContent);
					LogLua(_T("重新加载脚本[%s]失败，返回加载源脚本"), (LPCTSTR)m_ScriptName);
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
	if (m_pLuaState)
	{
		DestoryThreadPool();
		if (!m_IsSharedLuaState)
		{
			lua_reset_state(m_pLuaState);
			lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);
			lua_close(m_pLuaState);
		}
		m_pLuaState = NULL;
	}
	m_IsSharedLuaState = false;
}
bool CLuaScript::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{	
	CloseLuaState();
	m_pLuaState = luaL_newstate();
	if (m_pLuaState)
	{
		lua_checkstack(m_pLuaState, m_ThreadCount);

		luaL_openlibs(m_pLuaState);
		//luaopen_bit32(m_pLuaState);
		//luaopen_base(m_pLuaState);
		//luaopen_string(m_pLuaState);
		//luaopen_math(m_pLuaState);
		CLuaLibCommon::GetInstance()->RegisterToVM(this);
		CLuaLibSmartStruct::GetInstance()->RegisterToVM(this);

		AddCFunctions(NULL, m_CFunctionList);

#ifdef UNICODE
		CEasyStringA ScriptContentA,ScriptNameA;
		ScriptContentA = ScriptContent;
		ScriptNameA = ScriptName;
		if (luaL_dostring(m_pLuaState, ScriptContentA, ScriptNameA) == 0)
#else
		if (luaL_dostring(m_pLuaState, ScriptContent, ScriptName) == 0)
#endif
		{
			return InitThreadPool();
		}
		else
		{
			if (lua_type(m_pLuaState, -1) == LUA_TSTRING)
				LogLua(_T("%s"), lua_tolstring(m_pLuaState, -1, NULL));
		}
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
	else
	{
		LogLua(_T("初始化lua失败"));
	}
	return false;
}
