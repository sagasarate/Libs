#pragma once
class CFileInfo
{
protected:
	bool		m_IsOK;
	CEasyString	m_FilePath;
	UINT64		m_FileSize;
	UINT		m_FileAttributes;
	time_t		m_CreationTime;
	time_t		m_LastAccessTime;
	time_t		m_LastWriteTime;

public:
	CFileInfo()
	{
		m_IsOK = false;
		m_FileSize = 0;
		m_FileAttributes = 0;
		m_CreationTime = 0;
		m_LastAccessTime = 0;
		m_LastWriteTime = 0;
	}
	CFileInfo(LPCTSTR szFilePath)
	{
		m_IsOK = false;
		m_FileSize = 0;
		m_FileAttributes = 0;
		m_CreationTime = 0;
		m_LastAccessTime = 0;
		m_LastWriteTime = 0;
		FetchFileInfo(szFilePath);
	}
	~CFileInfo()
	{

	}

	inline static CFileInfo GetFileInfo(LPCTSTR szFilePath)
	{
		return CFileInfo(szFilePath);
	}

#ifdef WIN32
	bool FetchFileInfo(LPCTSTR szFilePath)
	{
		m_IsOK = false;

		WIN32_FILE_ATTRIBUTE_DATA FileAttr;
		if (GetFileAttributesEx(szFilePath, GetFileExInfoStandard, &FileAttr))
		{
			m_FilePath = szFilePath;

			UINT64_CONVERTER Con;
			Con.HighPart = FileAttr.nFileSizeHigh;
			Con.LowPart = FileAttr.nFileSizeLow;
			m_FileSize = Con.QuadPart;
			m_FileAttributes = FileAttr.dwFileAttributes;
			CEasyTime Time;
			Time.SetTime(FileAttr.ftCreationTime);
			m_CreationTime = Time;
			Time.SetTime(FileAttr.ftLastAccessTime);
			m_LastAccessTime = Time;
			Time.SetTime(FileAttr.ftLastWriteTime);
			m_LastWriteTime = Time;

			m_IsOK = true;
		}

		return m_IsOK;
	}
#else
	bool FetchFileInfo(LPCTSTR szFilePath)
	{
		m_IsOK = false;

		struct stat FileStat;
		if (stat(szFilePath, &FileStat) == 0)
		{
			m_FilePath = szFilePath;

			m_FileSize = FileStat.st_size;
			m_FileAttributes = FileStat.st_mode;
			m_CreationTime = FileStat.st_ctime;
			m_LastAccessTime = FileStat.st_atime;
			m_LastWriteTime = FileStat.st_mtime;
			m_IsOK = true;
		}

		return m_IsOK;
	}
#endif

	bool IsOK()
	{
		return m_IsOK;
	}

#ifdef WIN32
	bool IsFile()
	{
		return m_IsOK && ((m_FileAttributes&FILE_ATTRIBUTE_DIRECTORY) == 0);
	}
#else
	bool IsFile()
	{
		return m_IsOK && ((m_FileAttributes&S_IFREG) != 0);
	}
#endif

#ifdef WIN32
	bool IsDir()
	{
		return m_IsOK && ((m_FileAttributes&FILE_ATTRIBUTE_DIRECTORY) != 0);
	}
#else
	bool IsDir()
	{
		return m_IsOK && ((m_FileAttributes&S_IFDIR) != 0);
	}
#endif

	UINT64 GetFileSize()
	{
		return m_FileSize;
	}

	UINT GetFileAttr()
	{
		return m_FileAttributes;
	}

	time_t GetCreateTime()
	{
		return m_CreationTime;
	}
	time_t GetLastAccessTime()
	{
		return m_LastAccessTime;
	}
	time_t GetLastWriteTime()
	{
		return m_LastWriteTime;
	}
#ifdef WIN32
	bool SetCreateTime(time_t Time)
	{
		if (m_IsOK)
		{
			HANDLE hFile = CreateFile(m_FilePath, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CEasyTime FileTime(Time);
				FILETIME FTime = FileTime;
				if (SetFileTime(hFile, &FTime, NULL, NULL))
				{
					CloseHandle(hFile);
					return true;
				}
				CloseHandle(hFile);
			}
		}
		return false;
	}
#else
	bool SetCreateTime(const CEasyTime& Time)
	{
		return false;
	}
#endif

#ifdef WIN32
	bool SetLastAccessTime(time_t Time)
	{
		if (m_IsOK)
		{
			HANDLE hFile = CreateFile(m_FilePath, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CEasyTime FileTime(Time);
				FILETIME FTime = FileTime;
				if (SetFileTime(hFile, NULL, &FTime, NULL))
				{
					CloseHandle(hFile);
					return true;
				}
				CloseHandle(hFile);
			}
		}
		return false;
	}
#else
	bool SetLastAccessTime(time_t Time)
	{
		if (m_IsOK)
		{
			struct utimbuf FileTimes;
			FileTimes.actime = Time;
			FileTimes.modtime = m_LastWriteTime;
			if (utime(m_FilePath, &FileTimes) == 0)
			{
				return true;
			}
		}
		return false;
	}
#endif

#ifdef WIN32
	bool SetLastWriteTime(time_t Time)
	{
		if (m_IsOK)
		{
			HANDLE hFile = CreateFile(m_FilePath, FILE_WRITE_ATTRIBUTES, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
				OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			if (hFile != INVALID_HANDLE_VALUE)
			{
				CEasyTime FileTime(Time);
				FILETIME FTime = FileTime;
				if (SetFileTime(hFile, NULL, NULL, &FTime))
				{
					CloseHandle(hFile);
					return true;
				}
				CloseHandle(hFile);
			}
		}
		return false;
	}
#else
	bool SetLastWriteTime(time_t Time)
	{
		if (m_IsOK)
		{
			struct utimbuf FileTimes;
			FileTimes.actime = m_LastAccessTime;
			FileTimes.modtime = Time;
			if (utime(m_FilePath, &FileTimes) == 0)
			{
				return true;
			}
		}
		return false;
	}
#endif
};

