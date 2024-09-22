/****************************************************************************/
/*                                                                          */
/*      文件名:    FileSearcherWin.h                                        */
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
	HANDLE				m_hFile;
	WIN32_FIND_DATA		m_FindedFileInfo;
	WIN32_FIND_DATA		m_FindWorkData;
	bool				m_IsFinded;
	TCHAR				m_SearchDir[MAX_PATH];
	TCHAR				m_LogicalDriveNames[MAX_PATH];
	UINT				m_LogicDriveCount;
public:
	CFileSearcher(void);
	~CFileSearcher(void);

	bool FindFirst(LPCTSTR FindPattern);
	bool FindNext();
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

	bool IsDots();

	bool MatchesMask(DWORD dwMask);

	bool IsReadOnly();
	bool IsDirectory();
	bool IsCompressed();
	bool IsSystem();
	bool IsHidden();
	bool IsTemporary();
	bool IsNormal();
	bool IsArchived();

	bool CanRead();
	bool CanWrite();
	bool CanExec();

	int FetchLogicalDrive();
	int GetLogicalDriveCount();
	CEasyString GetLogicDriveName(UINT Index);
	
};

inline DWORD CFileSearcher::GetFileAttributes()
{
	return m_FindedFileInfo.dwFileAttributes;
}

inline bool CFileSearcher::IsReadOnly()
{ return MatchesMask(FILE_ATTRIBUTE_READONLY); }
inline bool CFileSearcher::IsDirectory()
{ return MatchesMask(FILE_ATTRIBUTE_DIRECTORY); }
inline bool CFileSearcher::IsCompressed()
{ return MatchesMask(FILE_ATTRIBUTE_COMPRESSED); }
inline bool CFileSearcher::IsSystem()
{ return MatchesMask(FILE_ATTRIBUTE_SYSTEM); }
inline bool CFileSearcher::IsHidden()
{ return MatchesMask(FILE_ATTRIBUTE_HIDDEN); }
inline bool CFileSearcher::IsTemporary()
{ return MatchesMask(FILE_ATTRIBUTE_TEMPORARY); }
inline bool CFileSearcher::IsNormal()
{ return MatchesMask(FILE_ATTRIBUTE_NORMAL); }
inline bool CFileSearcher::IsArchived()
{ return MatchesMask(FILE_ATTRIBUTE_ARCHIVE); }
inline bool CFileSearcher::CanRead()
{
	return true;
}
inline bool CFileSearcher::CanWrite()
{
	return !IsReadOnly();
}
inline bool CFileSearcher::CanExec()
{
	CEasyString FileExt = GetFileExt();
	return FileExt.CompareNoCase(_T(".exe")) == 0;
}