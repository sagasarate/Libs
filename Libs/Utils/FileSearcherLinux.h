/****************************************************************************/
/*                                                                          */
/*      文件名:    FileSearcherLinux.h                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CFileSearcher 
{
protected:
	struct FILE_INFO
	{
		struct dirent		FileInfo;
		struct stat			FileStat;	
	};	
	CEasyArray<FILE_INFO>	m_FileInfoList;	
	CEasyString				m_SearchDir;
	int						m_FetchIndex;
public:
	CFileSearcher(void);
	~CFileSearcher(void);

	BOOL FindFirst(LPCTSTR FindPattern);
	BOOL FindNext();
	void Close();

	CEasyString GetFileName();
	CEasyString GetFilePath();

	CEasyString GetFileDirect();
	CEasyString GetFileTitle();
	CEasyString GetFileExt();
	CEasyString GetFileURL();
	DWORD GetFileAttributes();

	ULONG64 GetFileSize();

	CEasyTime GetLastWriteTime();
	CEasyTime GetLastAccessTime();
	CEasyTime GetCreationTime();

	BOOL IsDots();

	BOOL MatchesMask(DWORD dwMask);

	BOOL IsReadOnly();
	BOOL IsDirectory();
	BOOL IsCompressed();
	BOOL IsSystem();
	BOOL IsHidden();
	BOOL IsTemporary();
	BOOL IsNormal();
	BOOL IsArchived();

	BOOL CanRead();
	BOOL CanWrite();
	BOOL CanExec();

	int FetchLogicalDrive();
	int GetLogicalDriveCount();
	CEasyString GetLogicDriveName(UINT Index);
	
};




