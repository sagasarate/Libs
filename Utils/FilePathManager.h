/****************************************************************************/
/*                                                                          */
/*      文件名:    FilePathManager.h                                        */
/*      创建日期:  2009年07月14日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

typedef CEasyMap<CEasyString,CEasyString> CFilePathList;

#define DECLARE_FILE_PATH_MANAGER  \
	protected:\
	static CFilePathList m_FilePathList;\
	public:\
		static void AddPath(LPCTSTR Path);\
		static void ClearPath();\
		static CEasyString FindFileOne(LPCTSTR FileName);\
		static CFilePathList& GetFilePathList();\
		static void AddPathList(CFilePathList& PathList);\
		static void AddPathList(CEasyArray<CEasyString>& PathList);

#define DECLARE_FILE_CHANNEL_MANAGER  \
	protected:\
			  static int			m_FileChannel;\
	public:\
		   static void SetFileChannel(int Channel);\
		   static int GetFileChannel();\
		   static IFileAccessor * CreateFileAccessor();



#define IMPLEMENT_FILE_PATH_MANAGER(ClassName) \
	CFilePathList ClassName::m_FilePathList;\
	void ClassName::AddPath(LPCTSTR Path)\
	{	\
		CEasyString ThePath(Path);\
		ThePath.MakeUpper();\
		m_FilePathList.Insert(ThePath,ThePath);\
	}\
	void ClassName::ClearPath()\
	{\
		m_FilePathList.Clear();\
	}\
	CEasyString ClassName::FindFileOne(LPCTSTR FileName)\
	{\
		CFileSearcher FileFind;\
		if(FileFind.FindFirst(FileName))\
		{\
			FileFind.FindNext();\
			return FileFind.GetFilePath();\
		}\
		void * Pos=m_FilePathList.GetFirstObjectPos();\
		while(Pos)\
		{\
			CEasyString Key;\
			m_FilePathList.GetNextObject(Pos,Key);\
			if(FileFind.FindFirst(Key+_T("/")+FileName))\
			{\
				FileFind.FindNext();	\
				return FileFind.GetFilePath();\
			}\
		}\
		return FileName;\
	}\
	CFilePathList& ClassName::GetFilePathList()\
	{\
		return m_FilePathList;\
	}\
	void ClassName::AddPathList(CFilePathList& PathList)\
	{\
		void * Pos=PathList.GetFirstObjectPos();\
		while(Pos)\
		{\
			CEasyString Key;\
			PathList.GetNextObject(Pos,Key);\
			m_FilePathList.Insert(Key,Key);\
		}\
	}\
	void ClassName::AddPathList(CEasyArray<CEasyString>& PathList)\
	{\
		for(UINT i=0;i<PathList.GetCount();i++)\
		{\
			CEasyString ThePath(PathList[i]);\
			ThePath.MakeUpper();\
			m_FilePathList.Insert(ThePath,ThePath);\
		}\
	}

#define IMPLEMENT_FILE_CHANNEL_MANAGER(ClassName) \
	int	ClassName::m_FileChannel=0;\
	void ClassName::SetFileChannel(int Channel)\
	{\
		m_FileChannel=Channel;\
	}\
	int ClassName::GetFileChannel()\
	{\
		return m_FileChannel;\
	}\
	IFileAccessor * ClassName::CreateFileAccessor()\
	{\
		return CFileSystemManager::GetInstance()->CreateFileAccessor(m_FileChannel);\
	}
