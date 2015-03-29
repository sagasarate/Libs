/****************************************************************************/
/*                                                                          */
/*      文件名:    ResourceFileAccessor.cpp                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CResourceFileAccessor,IFileAccessor);

CResourceFileAccessor::CResourceFileAccessor(void)
{
	m_hResource=NULL;
	m_hResourceData=NULL;
	m_ResourceDataSize=0;
	m_ReadPtr=0;
}

CResourceFileAccessor::~CResourceFileAccessor(void)
{
	Close();
}


BOOL CResourceFileAccessor::Open(LPCTSTR FileName,int OpenMode)
{
	CSettingFile ResourceInfo;

	Close();

	if(!ResourceInfo.Load(FileName,';'))
	{
		return FALSE;
	}

	HMODULE hModule=(HMODULE)ResourceInfo.GetInteger64(NULL,_T("Module"),0);
	LPCTSTR Type=ResourceInfo.GetString(NULL,_T("Type"),_T(""));
	LPCTSTR Name=ResourceInfo.GetString(NULL,_T("Name"),_T(""));
	UINT ID=ResourceInfo.GetInteger(NULL,_T("ID"),0);
	
	if(ID)
	{
		m_hResource=FindResource(hModule,MAKEINTRESOURCE(ID),Type);		
	}
	else
		m_hResource=FindResource(hModule,Name,Type);
	if(m_hResource==NULL)
		return FALSE;

	m_hResourceData=LoadResource(hModule,m_hResource);
	if(m_hResourceData==NULL)
		return FALSE;

	m_ResourceDataSize=SizeofResource(hModule,m_hResource);
	return TRUE;
}

void CResourceFileAccessor::Close()
{	
	m_hResource=NULL;	
	m_hResourceData=NULL;
	m_ResourceDataSize=0;
	m_ReadPtr=0;
}

ULONG64 CResourceFileAccessor::GetSize()
{
	return m_ResourceDataSize;
}

ULONG64 CResourceFileAccessor::Read(LPVOID pBuff,ULONG64 Size)
{
	char * pData=(char *)LockResource(m_hResourceData);
	if(pData)
	{
		UINT ReadSize=(UINT)Size;
		if((UINT)m_ReadPtr+ReadSize>m_ResourceDataSize)
			ReadSize=(UINT)((LONG64)m_ResourceDataSize-m_ReadPtr);
		memcpy(pBuff,pData+(UINT)m_ReadPtr,ReadSize);
		m_ReadPtr+=ReadSize;
		return ReadSize;
	}
	return 0;
}

ULONG64 CResourceFileAccessor::Write(LPCVOID pBuff,ULONG64 Size)
{
	return 0;
}


BOOL CResourceFileAccessor::IsEOF()
{
	if(m_ReadPtr>=(LONG64)m_ResourceDataSize)
		return TRUE;
	else
		return FALSE;
}

BOOL CResourceFileAccessor::Seek(LONG64 Offset,int SeekMode)
{
	switch(SeekMode)
	{	
	case seekBegin:
		m_ReadPtr=Offset;
		break;
	case seekCurrent:
		m_ReadPtr+=Offset;
		break;
	case seekEnd:
		m_ReadPtr=(LONG64)m_ResourceDataSize-Offset;
		break;
	}
	if(m_ReadPtr<0)
		m_ReadPtr=0;
	if(m_ReadPtr>=(LONG64)m_ResourceDataSize)
		m_ReadPtr=(LONG64)m_ResourceDataSize;
	return TRUE;
}

ULONG64 CResourceFileAccessor::GetCurPos()
{
	return m_ReadPtr;
}

BOOL CResourceFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CResourceFileAccessor::GetCreateTime(CEasyTime& Time)
{
	return FALSE;
}

BOOL CResourceFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CResourceFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	return FALSE;
}

BOOL CResourceFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CResourceFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	return FALSE;
}