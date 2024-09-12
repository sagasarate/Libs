#include "stdafx.h"

CLuaScript::CLuaScript()
{

	m_NeedReload = false;
	m_IsLoadFromFile = false;
	m_ScriptID = 0;	
}


CLuaScript::~CLuaScript()
{
	m_ScriptID = 0;
	m_IsLoadFromFile = false;
	m_NeedReload = false;
	m_ScriptContent.Clear();
	m_ReloadScriptContent.Clear();
	m_CFunctionList.Clear();
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
	
	m_MainThread.CreateMainThread(this, m_LuaStackSize + m_ThreadCount);
	if (GetLuaState())
	{
		InitEnv();

		AddCFunctions(NULL, m_CFunctionList);

		if (!InitThreadPool())
			return false;

		return LoadScript(m_ScriptName, m_ScriptContent);
	}
	return false;
}




void CLuaScript::ReloadScript(LPCTSTR ScriptContent)
{
	m_NeedReload = true;
	m_ReloadScriptContent = ScriptContent;	
}

void CLuaScript::Destory()
{
	m_ScriptID = 0;
	m_IsLoadFromFile = false;
	m_NeedReload = false;
	m_ScriptContent.Clear();
	m_ReloadScriptContent.Clear();
	m_CFunctionList.Clear();
	CBaseLuaVM::Destory();
}

int CLuaScript::Update(int ProcessLimit)
{
	int ProcessCount = CBaseLuaVM::Update(ProcessLimit);
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
			ProcessLimit++;
			m_NeedReload = false;
			
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
	return ProcessCount;
}

bool CLuaScript::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{	
	int Ret = m_MainThread.DoCall(ScriptName, ScriptContent);
	m_MainThread.ClearStack();
	return Ret == LUA_OK;
}
