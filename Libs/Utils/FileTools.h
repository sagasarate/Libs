#pragma once


#ifdef WIN32
const TCHAR DIR_SLASH = '\\';
#else
const char DIR_SLASH = '/';
#endif

#ifndef WIN32

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

inline void _splitpath_s(const char *path, char *drive, size_t MaxDrvLen, char *dir, size_t MaxDirLen,
	char *fname, size_t MaxFNameLen, char *ext, size_t MaxExtLen)
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
#endif




class CFileTools
{
public:
	CFileTools();
	~CFileTools();

	inline static bool IsDirSlash(TCHAR Char)
	{
		return Char == '/' || Char == '\\';
	}




#ifdef WIN32
	inline static CEasyString GetModuleFilePath(HMODULE hModule)
	{
		CEasyString ModulePath;
		ModulePath.Resize(MAX_PATH);
		GetModuleFileName(hModule, ModulePath, MAX_PATH);
		ModulePath.TrimBuffer();
		return ModulePath;
	}
#else
	inline static CEasyString GetModuleFilePath(HMODULE hModule)
	{
		CEasyString ModulePath;

		if (hModule == NULL)
		{
			char   exePath[NAME_MAX + 1];
			char   fullPath[MAX_PATH + 1];
			sprintf(exePath, _T("/proc/%d/exe"), getpid());
			int Len = readlink(exePath, fullPath, MAX_PATH);
			if (Len > 0)
			{
				fullPath[Len] = 0;
				ModulePath = fullPath;
				ModulePath.TrimBuffer();
			}
		}
		else
		{
			Dl_info DLInfo;
			if (dladdr(hModule, &DLInfo))
			{
				ModulePath = DLInfo.dli_fname;
			}
		}
		return ModulePath;
	}
#endif


	inline static CEasyString GetModulePath(HMODULE hModule)
	{
		CEasyString ModulePath = GetModuleFilePath(hModule);
		int Pos = ModulePath.ReverseFind(DIR_SLASH);
		if (Pos >= 0)
		{
			ModulePath.Resize(Pos + 1);
		}
		return ModulePath;
	}


#ifdef WIN32
	inline static CEasyString MakeFullPath(LPCTSTR Path)
	{
		CEasyString FilePath;
		FilePath.Resize(MAX_PATH);
		if (_tfullpath(FilePath, Path, MAX_PATH))
		{
			FilePath.TrimBuffer();
			return FilePath;
		}
		else
		{
			return CEasyString();
		}
	}
#else
	inline static CEasyString MakeFullPath(LPCTSTR Path)
	{
		TCHAR FilePath[MAX_PATH];
		if(realpath(Path, FilePath))
			return FilePath;
		else
			return Path;
	}
#endif


	inline static CEasyString MakeModuleFullPath(HMODULE hModule, LPCTSTR Path)
	{
		CEasyString FilePath;
		if (!IsAbsolutePath(Path))
		{
			CEasyString ModulePath = GetModulePath(hModule);
			FilePath = MakeFullPath(ModulePath + Path);
		}
		else
		{
			FilePath = Path;
		}
		return FilePath;
	}



	inline static CEasyString GetPathDirectory(LPCTSTR Path)
	{
		CEasyString Drv, Dir;
		Drv.Resize(8);
		Dir.Resize(MAX_PATH);
		_tsplitpath_s(Path, Drv, 8, Dir, MAX_PATH, NULL, 0, NULL, 0);
		Drv.TrimBuffer();
		Dir.TrimBuffer();
		Drv += Dir;
		return Drv;
	}


	inline static CEasyString GetPathFileNameExt(LPCTSTR Path)
	{
		CEasyString FileName, ExtName;

		FileName.Resize(MAX_PATH);
		ExtName.Resize(8);
		_tsplitpath_s(Path, NULL, 0, NULL, 0, FileName, MAX_PATH, ExtName, 8);
		FileName.TrimBuffer();
		ExtName.TrimBuffer();
		FileName += ExtName;
		return FileName;
	}

	inline static CEasyString GetPathFileName(LPCTSTR Path)
	{
		CEasyString FileName;

		FileName.Resize(MAX_PATH);
		_tsplitpath_s(Path, NULL, 0, NULL, 0, FileName, MAX_PATH, NULL, 0);
		FileName.TrimBuffer();
		return FileName;
	}



	inline static CEasyString GetPathFileExtName(LPCTSTR Path)
	{
		CEasyString ExtName;

		ExtName.Resize(8);
		_tsplitpath_s(Path, NULL, 0, NULL, 0, NULL, 0, ExtName, 8);
		ExtName.TrimBuffer();
		return ExtName;
	}

#ifdef WIN32
	inline static bool CreateDirEx(LPCTSTR szDirName)
	{
		int Ret = SHCreateDirectoryEx(NULL, szDirName, NULL);
		if (Ret == ERROR_SUCCESS || Ret == ERROR_ALREADY_EXISTS || Ret == ERROR_FILE_EXISTS)
			return true;
		else
			return false;
	}
#else
	inline static bool CreateDirEx(LPCTSTR szDirName)
	{
		TCHAR SrcDir[MAX_PATH];
		TCHAR DirBuffer[MAX_PATH];
		size_t Len=_tcslen(szDirName);
		_tcscpy_s(SrcDir,MAX_PATH,szDirName);

		PrintImportantLog(_T("try mkdir %s"), SrcDir);

		for (size_t i = 1; i < Len; i++)
		{
			if (SrcDir[i] == '/')
			{
				SrcDir[i] = 0;
				realpath(SrcDir, DirBuffer);
				if (access(DirBuffer, F_OK) != 0)
				{
					PrintImportantLog(_T("mkdir %s "), DirBuffer);
					if (mkdir(DirBuffer, S_IRWXU) == -1)
					{
						PrintImportantLog(_T("mkdir %s failed:%d"), DirBuffer, errno);
						return false;
					}					
				}
				else
				{
					PrintImportantLog(_T("exist %s "), DirBuffer);
				}
				SrcDir[i] = '/';
			}
		}
		realpath(SrcDir, DirBuffer);
		if (access(DirBuffer, F_OK) != 0)
		{
			PrintImportantLog(_T("mkdir %s "), DirBuffer);
			if (mkdir(DirBuffer, S_IRWXU) == -1)
			{
				PrintImportantLog(_T("mkdir %s failed:%d"), DirBuffer, errno);
				return false;
			}		
			else
			{
				PrintImportantLog(_T("exist %s "), DirBuffer);
			}
		}
		return true;
	}

#endif

#ifdef WIN32
	inline static bool IsFileExist(LPCTSTR szFileName)
	{
		WIN32_FILE_ATTRIBUTE_DATA FileAttr;
		if (GetFileAttributesEx(szFileName, GetFileExInfoStandard, &FileAttr))
		{
			return (FileAttr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0;
		}
		return false;
	}
#else
	inline static bool IsFileExist(LPCTSTR szFileName)
	{
		struct stat FileStat;
		if (stat(szFileName, &FileStat) == 0)
		{
			return (FileStat.st_mode&S_IFREG) != 0;
		}
		return false;
	}
#endif

#ifdef WIN32
	inline static bool IsDirExist(LPCTSTR szDirName)
	{
		WIN32_FILE_ATTRIBUTE_DATA FileAttr;
		if (GetFileAttributesEx(szDirName, GetFileExInfoStandard, &FileAttr))
		{
			return (FileAttr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0;
		}
		return false;
	}
#else
	inline static bool IsDirExist(LPCTSTR szFileName)
	{
		struct stat FileStat;
		if (stat(szFileName, &FileStat) == 0)
		{
			return (FileStat.st_mode&S_IFDIR) != 0;
		}
		return false;
	}
#endif

#ifdef WIN32
	inline static bool DeleteFile(LPCTSTR szFilePath, bool IsRecursive = false)
	{
		if (IsRecursive)
		{
			TCHAR FileNameBuffer[MAX_PATH + 2];
			size_t Len = _tcslen(szFilePath);
			if (Len > MAX_PATH)
				Len = MAX_PATH;
			_tcsncpy_s(FileNameBuffer, MAX_PATH + 2, szFilePath, Len);
			FileNameBuffer[Len] = 0;
			FileNameBuffer[Len + 1] = 0;

			SHFILEOPSTRUCT OPInfo;
			OPInfo.hwnd = NULL;
			OPInfo.wFunc = FO_DELETE;
			OPInfo.pFrom = FileNameBuffer;
			OPInfo.pTo = NULL;
			OPInfo.fFlags = FOF_NO_UI;
			OPInfo.fAnyOperationsAborted = FALSE;
			OPInfo.hNameMappings = NULL;
			OPInfo.lpszProgressTitle = NULL;
			if (SHFileOperation(&OPInfo) == 0)
				return true;
		}
		else
		{
			WIN32_FILE_ATTRIBUTE_DATA FileAttr;
			if (GetFileAttributesEx(szFilePath, GetFileExInfoStandard, &FileAttr))
			{
				if ((FileAttr.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0)
				{
					if (::RemoveDirectory(szFilePath))
						return true;
				}
				else
				{
					if (::DeleteFile(szFilePath))
						return true;
				}
			}
		}
		return false;
	}
#else
	inline static bool DeleteFile(LPCTSTR szFilePath, bool IsRecursive = false)
	{
		if (IsRecursive)
		{
			char Buffer[MAX_PATH + 16];
			sprintf_s(Buffer, MAX_PATH + 10, "rm -rf \"%s\"", szFilePath);
			int Ret = system(Buffer);
			if (Ret != -1 && WIFEXITED(Ret) && 0 == WEXITSTATUS(Ret))
			{
				return true;
			}
		}
		else
		{
			if (remove(szFilePath) == 0)
				return true;
		}
		return false;
	}
#endif

	inline static CEasyString GetRelativePath(LPCTSTR szSrcDir, LPCTSTR szSrcPath)
	{

		if (szSrcDir == NULL || szSrcPath == NULL)
			return CEasyString();



		CEasyString SrcDir = MakeFullPath(szSrcDir);

		CEasyString SrcPath = MakeFullPath(szSrcPath);

		SrcDir.Trim();
		if (SrcDir.IsEmpty() || SrcPath.IsEmpty())
			return CEasyString();

		//if (!IsDirSlash(SrcDir[SrcDir.GetLength() - 1]))
		//	SrcDir += DIR_SLASH;

		UINT DirReturnCount = 0;
		UINT SameDirLen = 0;
		bool IsInSameDir = true;
		UINT SrcLeftLen = 0;
		for (UINT i = 0; i < SrcDir.GetLength(); i++)
		{
			if (IsInSameDir)
			{
				if (i >= SrcPath.GetLength() || _totlower(SrcDir[i]) != _totlower(SrcPath[i]))
				{
					IsInSameDir = false;
				}
				else if (IsDirSlash(SrcPath[i]))
				{
					SameDirLen = i + 1;
				}
			}

			if (!IsInSameDir)
			{
				SrcLeftLen++;
				if (IsDirSlash(SrcDir[i]))
				{
					DirReturnCount++;
					SrcLeftLen = 0;
				}
			}
		}

		if (IsInSameDir)
		{
			SameDirLen = (UINT)SrcDir.GetLength();
			if (SameDirLen < SrcPath.GetLength())
			{
				if (IsDirSlash(SrcPath[SameDirLen]))
					SameDirLen++;
			}
		}

		if (SrcLeftLen)
		{
			DirReturnCount++;
		}

		CEasyString RelativePath;

		RelativePath.Resize(DirReturnCount * 3 + SrcPath.GetLength() - SameDirLen);

		for (UINT i = 0; i < DirReturnCount; i++)
		{
			RelativePath[i * 3] = '.';
			RelativePath[i * 3 + 1] = '.';
			RelativePath[i * 3 + 2] = DIR_SLASH;
		}

		memcpy((LPTSTR)RelativePath.GetBuffer() + DirReturnCount * 3, SrcPath.GetBuffer() + SameDirLen, (SrcPath.GetLength() - SameDirLen)*sizeof(TCHAR));
		RelativePath.TrimBuffer(DirReturnCount * 3 + SrcPath.GetLength() - SameDirLen);
		return RelativePath;
	}

#ifdef WIN32
	inline static bool IsAbsolutePath(LPCTSTR Path)
	{
		return PathIsRelative(Path) == FALSE;
	}
#else
	inline static bool IsAbsolutePath(LPCTSTR Path)
	{
		if (*Path == '/')
			return true;
		while (*Path == ' ')
		{
			Path++;
			if (*Path == '/')
				return true;
		}
		return false;
	}
#endif

#ifdef WIN32
	inline static bool MoveFile(LPCTSTR SrcPath, LPCTSTR TargetPath)
	{
		return ::MoveFile(SrcPath, TargetPath) != FALSE;
	}
#else
	inline static bool MoveFile(LPCTSTR SrcPath, LPCTSTR TargetPath)
	{
		return rename(SrcPath, TargetPath) == 0;
	}
#endif
};

