/****************************************************************************/
/*                                                                          */
/*      文件名:    FileSystemManager.h                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


enum FILE_CHANNELS
{
	FILE_CHANNEL_NORMAL1,
	FILE_CHANNEL_NORMAL2,	
	FILE_CHANNEL_NORMAL3,
	FILE_CHANNEL_MAX,
};


class CFileSystemManager:public CStaticObject<CFileSystemManager>
{
protected:
	IFileObjectCreator *		m_FileObjectCreators[FILE_CHANNEL_MAX];
	
public:
	CFileSystemManager(void);
	~CFileSystemManager(void);

	void SetFileObjectCreator(int Channel,IFileObjectCreator * pFileObjectCreator);
	IFileAccessor * CreateFileAccessor(int Channel);

	
};
