/****************************************************************************/
/*                                                                          */
/*      文件名:    FileSystemManager.cpp                                    */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

TCHAR FILE_SYSTEM_MANAGER_INSTANCE[] = _T("CFileSystemManager");

CFileSystemManager::CFileSystemManager(void)
{
#ifdef WIN32
	m_FileObjectCreators[FILE_CHANNEL_NORMAL1] = MONITORED_NEW(_T("CFileSystemManager"), CWinFileAccessorObjectCreator);
#else
	m_FileObjectCreators[FILE_CHANNEL_NORMAL1] = MONITORED_NEW(_T("CFileSystemManager"), CLinuxFileAccessorObjectCreator);
#endif
	m_FileObjectCreators[FILE_CHANNEL_NORMAL2] = MONITORED_NEW(_T("CFileSystemManager"), CStandardFileObjectCreator);
	m_FileObjectCreators[FILE_CHANNEL_NORMAL3] = MONITORED_NEW(_T("CFileSystemManager"), CStandardFileObjectCreator);
}

CFileSystemManager::~CFileSystemManager(void)
{
	for(int i=0;i<FILE_CHANNEL_MAX;i++)
		SAFE_RELEASE(m_FileObjectCreators[i]);
}

void CFileSystemManager::SetFileObjectCreator(int Channel,IFileObjectCreator * pFileObjectCreator)
{
	if(Channel<0||Channel>=FILE_CHANNEL_MAX)
		return;
	SAFE_RELEASE(m_FileObjectCreators[Channel]);
	m_FileObjectCreators[Channel]=pFileObjectCreator;
	pFileObjectCreator->AddUseRef();
}

IFileAccessor * CFileSystemManager::CreateFileAccessor(int Channel)
{
	if(Channel<0||Channel>=FILE_CHANNEL_MAX)
		return NULL;
	if(m_FileObjectCreators[Channel]==NULL)
		return NULL;
	return m_FileObjectCreators[Channel]->CreateFileAccessor();
}

