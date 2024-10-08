﻿#pragma once
class CLuaVM:
	public CBaseLuaVM
{
protected:
	CStaticMap<CEasyString, time_t>	m_IncludeLoadHistory;
public:
	CLuaVM();
	~CLuaVM();

	virtual void Destory();

	bool Init(int LuaStackSize, UINT ThreadCount, UINT GrowSize, UINT GrowLimit);
	bool LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent, LPCTSTR IncludePath);
	bool LoadScript(LPCTSTR ScriptName, LPCTSTR ScriptContent);

	
	

protected:
	bool ProcessScriptInclude(const CEasyString& ScriptContent, LPCTSTR SearchDir, int Depth);
};

