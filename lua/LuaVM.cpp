#include "stdafx.h"

CLuaVM::CLuaVM()
{

}

CLuaVM::~CLuaVM()
{
	m_IncludeLoadHistory.Destory();
}

bool CLuaVM::Init(int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit)
{
	Destory();

	m_LuaStackSize = LuaStackSize;
	m_ThreadCount = ThreadCount;
	m_GrowSize = GrowSize;
	m_GrowLimit = GrowLimit;

	m_IncludeLoadHistory.Create(256, 256, 256);

	m_MainThread.CreateMainThread(this, m_LuaStackSize + m_ThreadCount);
	if (GetLuaState())
	{
		InitEnv();

		LogLuaDebug(_T("stack=%u"), lua_gettop(GetLuaState()));

		return InitThreadPool();
	}
	else
	{
		LogLua(_T("初始化lua失败"));
	}
	return false;
}

bool CLuaVM::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent, LPCTSTR IncludePath)
{
	if (GetLuaState())
	{
		if (ProcessScriptInclude(ScriptContent, IncludePath, 0))
			return LoadScript(ScriptName, ScriptContent);
	}
	return false;
}

bool CLuaVM::LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent)
{
	int Ret = m_MainThread.DoCall(ScriptName, ScriptContent);
	m_MainThread.ClearStack();
	return Ret == LUA_OK;
}



void CLuaVM::Destory()
{
	m_IncludeLoadHistory.Destory();
	CBaseLuaVM::Destory();
}





bool CLuaVM::ProcessScriptInclude(const CEasyString& ScriptContent, LPCTSTR SearchDir, int Depth)
{
	if (Depth > 32)
	{
		LogLua(_T("解析层次过多，可能有循环引用"));
		return false;
	}
	CEasyArray<CEasyString> IncludeList;
	size_t Pos = ScriptContent.Find("--Include<", 0, true);
	while (Pos != CEasyString::INVALID_POS)
	{
		size_t End = ScriptContent.Find(">");
		if (End != CEasyString::INVALID_POS && End >= Pos)
		{
			size_t Len = End - Pos + 1;
			IncludeList.Add(ScriptContent.SubStr(Pos + 10, Len - 11));
			Pos = End + 1;
		}
		else
		{
			Pos += 10;
		}
		Pos = ScriptContent.Find("--Include<", Pos, true);
	}
	for (CEasyString& IncludeFileName : IncludeList)
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

				time_t* pHistory = m_IncludeLoadHistory.Find(FileName);
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

