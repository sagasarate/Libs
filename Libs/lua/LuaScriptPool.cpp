#include "StdAfx.h"



#define LUA_TIME_OUT_CHECK_TIME	150000
#define LUA_SCRIPT_UPDATE_TIME	100



#if defined(USE_CRT_DETAIL_NEW) && defined(_DEBUG)
#define new new( __FILE__, __LINE__ )
#endif



CLuaScriptPool::CLuaScriptPool(void)
{
	m_LuaStackSize = 32;
	m_ThreadStartSize = 0;
	m_ThreadGrowSize = 0;
	m_ThreadGrowLimit = 0;
}

CLuaScriptPool::~CLuaScriptPool(void)
{
	Destory();
}



bool CLuaScriptPool::Create(UINT StartSize, UINT GrowSize, UINT GrowLimit, UINT LuaStackSize, UINT ThreadStartSize, UINT ThreadGrowSize, UINT ThreadGrowLimit)
{
	Destory();
	if(!m_LuaScriptPool.Create(StartSize,GrowSize,GrowLimit))
		return false;
	m_LuaStackSize = LuaStackSize;
	m_ThreadStartSize = ThreadStartSize;
	m_ThreadGrowSize = ThreadGrowSize;
	m_ThreadGrowLimit = ThreadGrowLimit;
	m_UpdateTimer.SaveTime();
	return true;
}

void CLuaScriptPool::Destory()
{
	m_LuaScriptPool.Destory();
}

int CLuaScriptPool::Update()
{
	int ProcessCount=0;
	if (m_UpdateTimer.IsTimeOut(LUA_SCRIPT_UPDATE_TIME))
	{
		m_UpdateTimer.SaveTime();
		void * Pos=m_LuaScriptPool.GetFirstObjectPos();
		while(Pos)
		{
			CLuaScript * pScript=m_LuaScriptPool.GetNextObject(Pos);
			pScript->Update();			
		}
	}
	return ProcessCount;
}

CLuaScript * CLuaScriptPool::GetScript(LPCTSTR szLuaFileName)
{	
	CEasyString FileName(szLuaFileName);

	FileName.MakeUpper();

	CLuaScript * pScript = m_LuaScriptPool.Find(FileName);
	//if (pScript==NULL)
	//{
	//	CStringFile LuaFile;

	//	LuaFile.SetLocalCodePage(CP_UTF8);

	//	if (LuaFile.LoadFile(FileName, false))
	//	{
	//		pScript = LoadScript(FileName, LuaFile.GetData());
	//	}
	//	else
	//	{
	//		LogLua(_T("CLuaScriptPool::InitLuaScript:打开lua脚本失败%s"), (LPCTSTR)FileName);
	//	}
	//}
	return pScript;
}
CLuaScript * CLuaScriptPool::GetScript(UINT ScriptID)
{
	return m_LuaScriptPool.GetObject(ScriptID);
}
CLuaScript * CLuaScriptPool::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{
	CLuaScript * pScript = NULL;
	CEasyString Key(ScriptName);
	Key.MakeUpper();
	UINT ScriptID = m_LuaScriptPool.New(Key, &pScript);
	if (ScriptID)
	{
		pScript->SetID(ScriptID);

		if (pScript->Init(ScriptName, ScriptContent, &m_LuaCFunList, m_LuaStackSize, m_ThreadStartSize, m_ThreadGrowSize, m_ThreadGrowLimit))
		{
			LogLuaDebug(_T("CLuaScriptPool::InitLuaScript:加载lua脚本完毕%s"), (LPCTSTR)ScriptName);
		}
		else
		{
			LogLua(_T("CLuaScriptPool::InitLuaScript:加载lua脚本失败%s"), (LPCTSTR)ScriptName);
			m_LuaScriptPool.DeleteByID(ScriptID);
			pScript = NULL;
		}
	}
	else
	{
		LogLua(_T("CLuaScriptPool::InitLuaScript:(%s)脚本缓冲池已满"), (LPCTSTR)ScriptName);
	}
	return pScript;
}


CLuaThread * CLuaScriptPool::AllocScriptThread(LPCTSTR szLuaFileName,CBaseScriptHost * pObject,LPCTSTR szFunctionName)
{
	CLuaScript * pScript = GetScript(szLuaFileName);
	if (pScript)
	{
		return pScript->AllocScriptThread(pObject, szFunctionName);
	}	
	return NULL;
}

CLuaThread * CLuaScriptPool::AllocScriptThread(UINT ScriptID,CBaseScriptHost * pObject,LPCTSTR szFunctionName)
{
	CLuaScript * pScript = GetScript(ScriptID);
	if (pScript)
	{
		return pScript->AllocScriptThread(pObject, szFunctionName);
	}
	return NULL;
}

void CLuaScriptPool::ReloadAllScript()
{
	void * Pos=m_LuaScriptPool.GetFirstObjectPos();
	while(Pos)
	{
		CLuaScript * pScript = m_LuaScriptPool.GetNextObject(Pos);
		if (pScript->IsLoadFromFile())
		{
			CStringFile LuaFile;

			LuaFile.SetLocalCodePage(CP_UTF8);

			if (LuaFile.LoadFile(pScript->GetScriptName(), false))
			{
				pScript->ReloadScript(LuaFile.GetData());
			}
			else
			{
				LogLua(_T("CLuaScriptPool::ReloadAllScript:打开lua脚本失败%s"), (LPCTSTR)pScript->GetScriptName());
			}
		}
	}
}

void CLuaScriptPool::ReloadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{
	CEasyString Name(ScriptName);

	Name.MakeUpper();
	CLuaScript * pScript = m_LuaScriptPool.Find(Name);
	if (pScript)
	{
		pScript->ReloadScript(ScriptContent);
	}
}

void CLuaScriptPool::AddCFunction(const char* funcName, lua_CFunction function, void* func, unsigned int sizeofFunc)
{
	CLuaScript::LUA_CFUN_INFO Info;
	Info.FunName=funcName;
	Info.pLuaCFun=function;
	memcpy(Info.FunAddr,func,sizeofFunc);
	Info.FunSize=sizeofFunc;
	m_LuaCFunList.Add(Info);
}


