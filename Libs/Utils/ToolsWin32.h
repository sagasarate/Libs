/****************************************************************************/
/*                                                                          */
/*      文件名:    ToolsWin32.h                                             */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once






#define CHECK_HRESULT(hr) \
	if(FAILED(hr))\
{\
	PrintSystemLog(0xff00ff,_T("HResult Error:<%s><%d>[%x]"),__FILE__,__LINE__,hr);\
	return false;\
}

inline CEasyString GetModuleFilePath(HMODULE hModule)
{
	CEasyString ModulePath;
	ModulePath.Resize(MAX_PATH);
	GetModuleFileName(hModule,ModulePath,MAX_PATH);
	ModulePath.TrimBuffer();
	return ModulePath;
}

inline CEasyString GetModulePath(HMODULE hModule)
{
	CEasyString ModulePath;
	ModulePath.Resize(MAX_PATH);
	GetModuleFileName(hModule,ModulePath,MAX_PATH);
	ModulePath.TrimBuffer();
	int Pos=ModulePath.ReverseFind('\\');
	if(Pos>=0)
	{
		ModulePath.Resize(Pos+1);
	}
	return ModulePath;
}

inline CEasyString MakeFullPath(LPCTSTR Path)
{
	CEasyString FilePath;
	FilePath.Resize(MAX_PATH);
	if(_tfullpath(FilePath,Path,MAX_PATH))
	{
		FilePath.TrimBuffer();
		return FilePath;
	}
	else
	{
		return CEasyString();
	}
}

inline CEasyString MakeModuleFullPath(HMODULE hModule,LPCTSTR Path)
{
	CEasyString FilePath=Path;
	if(FilePath.Find(':')<0)
	{
		CEasyString ModulePath=GetModulePath(hModule);
		FilePath=MakeFullPath(ModulePath+FilePath);
	}
	return FilePath;
}



inline CEasyString GetPathDirectory(LPCTSTR Path)
{
	CEasyString Drv,Dir;
	Drv.Resize(8);
	Dir.Resize(MAX_PATH);
	_tsplitpath_s(Path,Drv,8,Dir,MAX_PATH,NULL,0,NULL,0);
	Drv.TrimBuffer();
	Dir.TrimBuffer();
	Drv+=Dir;
	return Drv;
}

inline CEasyString GetPathFileNameExt(LPCTSTR Path)
{
	CEasyString FileName,ExtName;

	FileName.Resize(MAX_PATH);
	ExtName.Resize(8);
	_tsplitpath_s(Path,NULL,0,NULL,0,FileName,MAX_PATH,ExtName,8);
	FileName.TrimBuffer();
	ExtName.TrimBuffer();
	FileName+=ExtName;
	return FileName;
}

inline CEasyString GetPathFileName(LPCTSTR Path)
{
	CEasyString FileName;

	FileName.Resize(MAX_PATH);
	_tsplitpath_s(Path,NULL,0,NULL,0,FileName,MAX_PATH,NULL,0);
	FileName.TrimBuffer();
	return FileName;
}


inline CEasyString GetPathFileExtName(LPCTSTR Path)
{
	CEasyString ExtName;

	ExtName.Resize(8);
	_tsplitpath_s(Path,NULL,0,NULL,0,NULL,0,ExtName,8);
	ExtName.TrimBuffer();
	return ExtName;
}




inline void DoSleep(UINT nMilliseconds)
{
	Sleep(nMilliseconds);
}


inline bool CreateDirEx(LPCTSTR szDirName)
{
	int Ret=SHCreateDirectoryEx(NULL,szDirName,NULL);
	if(Ret==ERROR_SUCCESS||Ret==ERROR_ALREADY_EXISTS||Ret==ERROR_FILE_EXISTS)
		return true;
	else
		return false;
}

inline bool IsFileExist(LPCTSTR szFileName)
{
	WIN32_FILE_ATTRIBUTE_DATA FileAttr;
	if (GetFileAttributesEx(szFileName, GetFileExInfoStandard, &FileAttr))
	{
		return (FileAttr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0;
	}
	return false;
}

inline bool IsDirExist(LPCTSTR szDirName)
{
	WIN32_FILE_ATTRIBUTE_DATA FileAttr;
	if (GetFileAttributesEx(szDirName, GetFileExInfoStandard, &FileAttr))
	{
		return (FileAttr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
	}
	return false;
}

inline UINT GetCurProcessID()
{
	return GetCurrentProcessId();
}

extern CEasyString GetFontNameEN(LOGFONT * pFont);
extern CEasyString GetFontNameCN(LOGFONT * pFont);

