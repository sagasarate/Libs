#include "stdafx.h"

CLuaVM::CLuaVM()
{
	
}

CLuaVM::~CLuaVM()
{
	Destory();
}

bool CLuaVM::Init(int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit)
{
	Destory();

	m_LuaStackSize = LuaStackSize;
	m_ThreadCount = ThreadCount;
	m_GrowSize = GrowSize;
	m_GrowLimit = GrowLimit;

	m_IncludeLoadHistory.Create(256, 256, 256);

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
		
		return InitThreadPool();
	}
	else
	{
		LogLua(_T("初始化lua失败"));
	}
	return false;
}

bool CLuaVM::LoadScript(LPCTSTR ScriptName, const CEasyString& ScriptContent, LPCTSTR IncludePath)
{
	if (m_pLuaState)
	{
		if (ProcessScriptInclude(ScriptContent, IncludePath, 0))
			return LoadScript(ScriptName, ScriptContent);
	}	
	return false;
}

bool CLuaVM::LoadScript(LPCTSTR ScriptName, const CEasyString& ScriptContent)
{
	if (m_pLuaState)
	{
#ifdef UNICODE
		CEasyStringA ScriptContentA, ScriptNameA;
		ScriptContentA = ScriptContent;
		ScriptNameA = ScriptName;
		if (luaL_dostring(m_pLuaState, ScriptContentA, ScriptNameA) == 0)
#else
		if (luaL_dostring(m_pLuaState, ScriptContent, ScriptName) == 0)
#endif
		{
			return true;
		}
		else
		{
			if (lua_type(m_pLuaState, -1) == LUA_TSTRING)
				LogLua(_T("%s"), lua_tolstring(m_pLuaState, -1, NULL));
		}
	}
	return false;
}



void CLuaVM::Destory()
{
	m_IncludeLoadHistory.Destory();
	CloseLuaState();		
}



void CLuaVM::Update()
{
	if (m_ThreadRecyleTimer.IsTimeOut(LUA_THREAD_RECYLE_CHECK_TIME))
	{
		m_ThreadRecyleTimer.SaveTime();
		RecycleThread();
	}	
}

void CLuaVM::CloseLuaState()
{
	if (m_pLuaState)
	{
		DestoryThreadPool();
		lua_reset_state(m_pLuaState);
		lua_gc(m_pLuaState, LUA_GCCOLLECT, 0);
		lua_close(m_pLuaState);
		m_pLuaState = NULL;
	}
}


bool CLuaVM::ProcessScriptInclude(const CEasyString& ScriptContent, LPCTSTR SearchDir, int Depth)
{
	if (Depth > 32)
	{
		LogLua(_T("解析层次过多，可能有循环引用"));
		return false;
	}
	CEasyArray<CEasyString> IncludeList;
	int Pos = ScriptContent.Find("--Include<", 0, true);
	while (Pos >= 0)
	{
		int End = ScriptContent.Find(">");
		if (End >= Pos)
		{
			int Len = End - Pos + 1;
			IncludeList.Add(ScriptContent.SubStr(Pos + 10, Len - 11));
			Pos = End + 1;
		}
		else
		{
			Pos += 10;
		}
		Pos = ScriptContent.Find("--Include<", Pos, true);
	}
	for (CEasyString& IncludeFileName: IncludeList)
	{
		CEasyString FileName;
		if (!CFileTools::IsAbsolutePath(IncludeFileName))
		{
			FileName.Format("%s\\%s", SearchDir, (LPCTSTR)IncludeFileName);
			FileName = CFileTools::MakeFullPath(FileName);
		}
		else
		{
			FileName = CFileTools::MakeFullPath(IncludeFileName);
		}

		CFileInfo FileInfo;

		if (FileInfo.FetchFileInfo(FileName))
		{

			CStringFile ScriptFile;

			if (ScriptFile.LoadFile(FileName, false))
			{
				if (!ProcessScriptInclude(ScriptFile.GetData(), SearchDir, Depth + 1))
					return false;

				time_t * pHistory = m_IncludeLoadHistory.Find(FileName);
				if (pHistory)
				{
					if (FileInfo.GetLastWriteTime() == (*pHistory))
					{
						LogLuaDebug(_T("引用脚本已经加载过了%s"), (LPCTSTR)FileName);
						continue;
					}
				}

				if (LoadScript(CFileTools::GetPathFileNameExt(FileName), ScriptFile.GetData()))
				{
					m_IncludeLoadHistory.Insert(FileName, FileInfo.GetLastWriteTime());
					LogLuaDebug(_T("引用脚本加载成功%s"), (LPCTSTR)FileName);
				}
				else
				{
					LogLua(_T("脚本加载失败%s"), (LPCTSTR)FileName);
				}
			}
			else
			{
				LogLua(_T("无法打开脚本文件%s"), (LPCTSTR)FileName);
			}
		}
		else
		{
			LogLua(_T("无法无法获得文件信息%s"), (LPCTSTR)FileName);
		}
	}
	return true;
}

