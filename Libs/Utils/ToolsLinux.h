/****************************************************************************/
/*                                                                          */
/*      文件名:    ToolsLinux.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

//#ifndef max
//#define max(a,b)            (((a) > (b)) ? (a) : (b))
//#endif
//
//#ifndef min
//#define min(a,b)            (((a) < (b)) ? (a) : (b))
//#endif

template<typename T>
inline T max(T a, T b)
{
	return a > b ? a : b;
}

template<typename T>
inline T min(T a, T b)
{
	return a < b ? a : b;
}

inline void _split_whole_name(const char *whole_name, char *fname, char *ext)
{
	const char *p_ext;

	if (whole_name == NULL)
	{
		if (ext != NULL)
			ext[0] = '\0';
		if (fname != NULL)
			fname[0];
		return;
	}
		

	p_ext = rindex(whole_name, '.');
	if (NULL != p_ext)
	{
		if (ext != NULL)
			strcpy(ext, p_ext);
		if (fname != NULL)
			snprintf(fname, p_ext - whole_name + 1, "%s", whole_name);
	}
	else
	{
		if (ext != NULL)
			ext[0] = '\0';
		if (fname != NULL)
			strcpy(fname, whole_name);
	}
}

inline void _splitpath(const char *path, char *drive, char *dir, char *fname, char *ext)
{
    const char *p_whole_name;

	if (drive != NULL)
		drive[0] = '\0';
    if (NULL == path)
    {
		if (dir != NULL)
			dir[0] = '\0';
		if (fname != NULL)
			fname[0] = '\0';
		if (ext != NULL)
			ext[0] = '\0';
        return;
    }

    if ('/' == path[strlen(path)])
    {
		if (dir != NULL)
			strcpy(dir, path);
		if (fname != NULL)
			fname[0] = '\0';
		if (ext != NULL)
			ext[0] = '\0';
        return;
    }

    p_whole_name = rindex(path, '/');
    if (NULL != p_whole_name)
    {
        p_whole_name++;
		_split_whole_name(p_whole_name, fname, ext);

		if (dir != NULL)
			snprintf(dir, p_whole_name - path, "%s", path);
    }
    else
    {
		_split_whole_name(path, fname, ext);
		if (dir != NULL)
			dir[0] = '\0';
    }
}





inline CEasyString GetModulePath(HMODULE hModule)
{
	CEasyString ModulePath;

	char   exePath[NAME_MAX+1];
	char   fullPath[MAX_PATH+1];
	sprintf(exePath,_T("/proc/%d/exe"),getpid());
	int Len=readlink(exePath,fullPath,MAX_PATH);
	if(Len>0)
	{
	    fullPath[Len]=0;
		ModulePath=fullPath;
		ModulePath.TrimBuffer();
		int Pos=ModulePath.ReverseFind('/');
		if(Pos>=0)
		{
			ModulePath.Resize(Pos+1);
		}
	}
	return ModulePath;
}

inline CEasyString GetModuleFilePath(HMODULE hModule)
{
	CEasyString ModulePath;

	char   exePath[NAME_MAX+1];
	char   fullPath[MAX_PATH+1];
	sprintf(exePath,_T("/proc/%d/exe"),getpid());
	int Len=readlink(exePath,fullPath,MAX_PATH);
	if(Len>0)
	{
		fullPath[Len]=0;
		ModulePath=fullPath;
		ModulePath.TrimBuffer();
	}
	return ModulePath;
}

inline CEasyString MakeFullPath(LPCTSTR Path)
{
	TCHAR FilePath[MAX_PATH];
	realpath(Path,FilePath);
	return FilePath;
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
	CEasyString Dir=Path;
	int Pos=Dir.ReverseFind('/');
	Dir.Resize(Pos+1,true);
	return Dir;
}

inline CEasyString GetPathFileNameExt(LPCTSTR Path)
{
	CEasyString FileName=Path;
	int Pos=FileName.ReverseFind('/');
	return FileName.Right(FileName.GetLength()-Pos-1);
}

inline CEasyString GetPathFileName(LPCTSTR Path)
{
	CEasyString FileName;

	FileName.Resize(MAX_PATH);
	_splitpath(Path, NULL, NULL, FileName, NULL);
	FileName.TrimBuffer();
	return FileName;
}


inline CEasyString GetPathFileExtName(LPCTSTR Path)
{
	CEasyString ExtName;

	ExtName.Resize(8);
	_splitpath(Path, NULL, NULL, NULL, ExtName);
	ExtName.TrimBuffer();
	return ExtName;
}

inline void DoSleep(UINT nMilliseconds)
{
	usleep(nMilliseconds*1000);
}

inline int GetSystemCPUCount()
{
	FILE * pFile=NULL;
	int CPUCount=0;

	pFile=fopen(_T("/proc/cpuinfo"),_T("r"));
	if(pFile)
	{
		char LineBuffer[1024];
		while(fgets(LineBuffer,1023,pFile))
		{
			if(_strnicmp(LineBuffer,_T("processor"),9)==0)
			{
				CPUCount++;
			}
		}

	}
	if(CPUCount<=0)
		CPUCount++;
	return CPUCount;
}

inline bool CreateDirEx(LPCTSTR szDirName)
{
	CEasyString DirName=MakeFullPath(szDirName);

	for(UINT i=1; i<DirName.GetLength(); i++)
	{
		if(DirName[i]=='/')
		{
			DirName[i]=0;
			if(access(DirName,F_OK)!=0)
			{
				if(mkdir(DirName,S_IRWXU)==-1)
				{
					PrintImportantLog(0,_T("mkdir failed:%d"),errno);
					return false;
				}
				else
				{
					return true;
				}
			}
			DirName[i]='/';
		}
	}
	if(access(DirName,F_OK)!=0)
	{
		if(mkdir(DirName,S_IRWXU)==-1)
		{
			PrintImportantLog(0,_T("mkdir failed:%d"),errno);
			return false;
		}
		else
		{
			return true;
		}
	}
	return true;
}
