/****************************************************************************/
/*                                                                          */
/*      文件名:    FileSearcherLinux.cpp                                    */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include <fnmatch.h>

CFileSearcher::CFileSearcher(void)
{
	m_FetchIndex=-1;
}

CFileSearcher::~CFileSearcher(void)
{
	Close();
}

BOOL CFileSearcher::FindFirst(LPCTSTR FindPattern)
{
	Close();
	CEasyString SearchPattern=CFileTools::MakeFullPath(FindPattern);
	m_SearchDir = CFileTools::GetPathDirectory(SearchPattern) + DIR_SLASH;
	CEasyString FilePattern=CFileTools::GetPathFileNameExt(SearchPattern);

	DIR * pDir=opendir(m_SearchDir);

	if(pDir)
	{
		struct dirent * pFileInfo=readdir(pDir);
		while(pFileInfo)
		{
			if(fnmatch(FilePattern,pFileInfo->d_name,0)==0)
			{
				FILE_INFO FileInfo;
				FileInfo.FileInfo=*pFileInfo;
				if(stat(m_SearchDir+pFileInfo->d_name,&(FileInfo.FileStat))==0)
				{
					m_FileInfoList.Add(FileInfo);
				}
			}
			pFileInfo=readdir(pDir);
		}
		closedir(pDir);
		return TRUE;
	}

	return FALSE;
}

BOOL CFileSearcher::FindNext()
{
	if(m_FileInfoList.GetCount()&&m_FetchIndex<(int)m_FileInfoList.GetCount()-1)
	{
		m_FetchIndex++;
		return TRUE;
	}
	return FALSE;
}

void CFileSearcher::Close()
{
	m_FileInfoList.Clear();
	m_SearchDir.Clear();
	m_FetchIndex=-1;
}

CEasyString CFileSearcher::GetFileName()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return m_FileInfoList[m_FetchIndex].FileInfo.d_name;
	}
	else
	{
		return _T("");
	}

}

CEasyString CFileSearcher::GetFilePath()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return m_SearchDir+m_FileInfoList[m_FetchIndex].FileInfo.d_name;
	}
	else
	{
		return _T("");
	}
}

CEasyString CFileSearcher::GetFileDirect()
{
	return m_SearchDir;
}

CEasyString CFileSearcher::GetFileTitle()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return CFileTools::GetPathFileName(m_FileInfoList[m_FetchIndex].FileInfo.d_name);
	}
	else
	{
		return _T("");
	}
}

CEasyString CFileSearcher::GetFileExt()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return CFileTools::GetPathFileExtName(m_FileInfoList[m_FetchIndex].FileInfo.d_name);
	}
	else
	{
		return _T("");
	}
}

CEasyString CFileSearcher::GetFileURL()
{
	return "file://"+GetFilePath();
}

ULONG64 CFileSearcher::GetFileSize()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return m_FileInfoList[m_FetchIndex].FileStat.st_size;
	}
	else
	{
		return 0;
	}
}

CEasyTime CFileSearcher::GetLastWriteTime()
{
    CEasyTime FileTime;
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
        FileTime=m_FileInfoList[m_FetchIndex].FileStat.st_mtime;

	}
	return FileTime;
}

CEasyTime CFileSearcher::GetLastAccessTime()
{
     CEasyTime FileTime;
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		FileTime=m_FileInfoList[m_FetchIndex].FileStat.st_atime;
	}
	return FileTime;
}

CEasyTime CFileSearcher::GetCreationTime()
{
     CEasyTime FileTime;
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		FileTime=m_FileInfoList[m_FetchIndex].FileStat.st_ctime;
	}
	return FileTime;
}

DWORD CFileSearcher::GetFileAttributes()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return m_FileInfoList[m_FetchIndex].FileStat.st_mode;
	}
	else
	{
		return 0;
	}
}

BOOL CFileSearcher::MatchesMask(DWORD dwMask)
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		return (m_FileInfoList[m_FetchIndex].FileStat.st_mode & dwMask);
	}
	else
	{
		return FALSE;
	}
}

BOOL CFileSearcher::IsDots()
{
	if (IsDirectory())
	{
		if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
		{
			if (m_FileInfoList[m_FetchIndex].FileInfo.d_name[0] == '.')
			{
				if (m_FileInfoList[m_FetchIndex].FileInfo.d_name[1] == '\0' ||
					(m_FileInfoList[m_FetchIndex].FileInfo.d_name[1] == '.' &&
					m_FileInfoList[m_FetchIndex].FileInfo.d_name[2] == '\0'))
				{
					return TRUE;
				}
			}
		}
	}

	return FALSE;
}

BOOL CFileSearcher::IsReadOnly()
{
	return !CanWrite();
}
BOOL CFileSearcher::IsDirectory()
{
	return MatchesMask(S_IFDIR);
}
BOOL CFileSearcher::IsCompressed()
{
	return FALSE;
}
BOOL CFileSearcher::IsSystem()
{
	return FALSE;
}
BOOL CFileSearcher::IsHidden()
{
	return FALSE;
}
BOOL CFileSearcher::IsTemporary()
{
	return FALSE;
}
BOOL CFileSearcher::IsNormal()
{
	return MatchesMask(S_IFREG);
}
BOOL CFileSearcher::IsArchived()
{
	return MatchesMask(S_IFREG);
}

BOOL CFileSearcher::CanRead()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		uid_t UserID=getuid();
		gid_t GroupID=getgid();

		if(UserID==m_FileInfoList[m_FetchIndex].FileStat.st_uid)
		{
			return MatchesMask(S_IRUSR);
		}
		else if(GroupID==m_FileInfoList[m_FetchIndex].FileStat.st_gid)
		{
			return MatchesMask(S_IRGRP);
		}
		else
		{
			return MatchesMask(S_IROTH);
		}
	}
	else
	{
		return FALSE;
	}
}
BOOL CFileSearcher::CanWrite()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		uid_t UserID=getuid();
		gid_t GroupID=getgid();

		if(UserID==m_FileInfoList[m_FetchIndex].FileStat.st_uid)
		{
			return MatchesMask(S_IWUSR);
		}
		else if(GroupID==m_FileInfoList[m_FetchIndex].FileStat.st_gid)
		{
			return MatchesMask(S_IWGRP);
		}
		else
		{
			return MatchesMask(S_IWOTH);
		}
	}
	else
	{
		return FALSE;
	}
}
BOOL CFileSearcher::CanExec()
{
	if(m_FetchIndex>=0&&m_FetchIndex<(int)m_FileInfoList.GetCount())
	{
		uid_t UserID=getuid();
		gid_t GroupID=getgid();

		if(UserID==m_FileInfoList[m_FetchIndex].FileStat.st_uid)
		{
			return MatchesMask(S_IXUSR);
		}
		else if(GroupID==m_FileInfoList[m_FetchIndex].FileStat.st_gid)
		{
			return MatchesMask(S_IXGRP);
		}
		else
		{
			return MatchesMask(S_IXOTH);
		}
	}
	else
	{
		return FALSE;
	}
}


int CFileSearcher::FetchLogicalDrive()
{
	return 0;
}

int CFileSearcher::GetLogicalDriveCount()
{
	return 0;
}

CEasyString CFileSearcher::GetLogicDriveName(UINT Index)
{

	return "";
}
