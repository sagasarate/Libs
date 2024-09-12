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

IMPLEMENT_CLASS_INFO_STATIC(CResourceFileAccessor, IFileAccessor);

CResourceFileAccessor::CResourceFileAccessor(void)
{
	m_hResource = NULL;
	m_hResourceData = NULL;
	m_ResourceDataSize = 0;
	m_ReadPtr = 0;
	m_HaveError = false;
}

CResourceFileAccessor::~CResourceFileAccessor(void)
{
	Close();
}


bool CResourceFileAccessor::Open(LPCTSTR FileName, int OpenMode)
{
	CSettingFile ResourceInfo;

	Close();

	m_HaveError = true;
	if (!ResourceInfo.Load(FileName, ';'))
	{
		return false;
	}

	HMODULE hModule = (HMODULE)ResourceInfo.GetInteger64(NULL, _T("Module"), 0);
	LPCTSTR Type = ResourceInfo.GetString(NULL, _T("Type"), _T(""));
	LPCTSTR Name = ResourceInfo.GetString(NULL, _T("Name"), _T(""));
	UINT ID = ResourceInfo.GetInteger(NULL, _T("ID"), 0);

	if (ID)
	{
		m_hResource = FindResource(hModule, MAKEINTRESOURCE(ID), Type);
	}
	else
		m_hResource = FindResource(hModule, Name, Type);
	if (m_hResource == NULL)
		return false;

	m_hResourceData = LoadResource(hModule, m_hResource);
	if (m_hResourceData == NULL)
		return false;

	m_ResourceDataSize = SizeofResource(hModule, m_hResource);
	m_HaveError = false;
	return true;
}

void CResourceFileAccessor::Close()
{
	m_hResource = NULL;
	m_hResourceData = NULL;
	m_ResourceDataSize = 0;
	m_ReadPtr = 0;
}

ULONG64 CResourceFileAccessor::GetSize()
{
	return m_ResourceDataSize;
}

ULONG64 CResourceFileAccessor::Read(LPVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	char* pData = (char*)LockResource(m_hResourceData);
	if (pData)
	{
		UINT ReadSize = (UINT)Size;
		if ((UINT)m_ReadPtr + ReadSize > m_ResourceDataSize)
			ReadSize = (UINT)((LONG64)m_ResourceDataSize - m_ReadPtr);
		memcpy(pBuff, pData + (UINT)m_ReadPtr, ReadSize);
		m_ReadPtr += ReadSize;
		return ReadSize;
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CResourceFileAccessor::Write(LPCVOID pBuff, ULONG64 Size)
{
	return 0;
}


bool CResourceFileAccessor::IsEOF()
{
	if (m_ReadPtr >= (LONG64)m_ResourceDataSize)
		return true;
	else
		return false;
}

bool CResourceFileAccessor::Seek(LONG64 Offset, int SeekMode)
{
	switch (SeekMode)
	{
	case seekBegin:
		m_ReadPtr = Offset;
		break;
	case seekCurrent:
		m_ReadPtr += Offset;
		break;
	case seekEnd:
		m_ReadPtr = (LONG64)m_ResourceDataSize - Offset;
		break;
	}
	if (m_ReadPtr < 0)
		m_ReadPtr = 0;
	if (m_ReadPtr >= (LONG64)m_ResourceDataSize)
		m_ReadPtr = (LONG64)m_ResourceDataSize;
	return true;
}

ULONG64 CResourceFileAccessor::GetCurPos()
{
	return m_ReadPtr;
}

bool CResourceFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	return false;
}
bool CResourceFileAccessor::GetCreateTime(CEasyTime& Time)
{
	return false;
}

bool CResourceFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	return false;
}
bool CResourceFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	return false;
}

bool CResourceFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	return false;
}
bool CResourceFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	return false;
}