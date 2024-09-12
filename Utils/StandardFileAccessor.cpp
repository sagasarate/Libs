/****************************************************************************/
/*                                                                          */
/*      文件名:    StandardFileAccessor.cpp                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


IMPLEMENT_CLASS_INFO_STATIC(CStandardFileAccessor, IFileAccessor);

CStandardFileAccessor::CStandardFileAccessor(void) :IFileAccessor()
{
	m_hFile = NULL;
	m_IsWriteFlush = false;
	m_HaveError = false;
}

CStandardFileAccessor::~CStandardFileAccessor(void)
{
	Close();
}


bool CStandardFileAccessor::Open(LPCTSTR FileName, int OpenMode)
{
	m_HaveError = false;
	if (m_hFile)
		Close();

	TCHAR ModeStr[20];

	ModeStr[0] = 0;
	switch (OpenMode & modeCreationMask)
	{
	case modeOpenAlways:
		_tcscat_s(ModeStr, 20, _T("r"));
		break;
	case modeCreate:
		_tcscat_s(ModeStr, 20, _T("w"));
		break;
	case modeCreateAlways:
		_tcscat_s(ModeStr, 20, _T("w"));
		break;
	case modeTruncate:
		_tcscat_s(ModeStr, 20, _T("w"));
		break;
	case modeAppend:
		_tcscat_s(ModeStr, 20, _T("a+"));
		break;
	default:
		_tcscat_s(ModeStr, 20, _T("r"));
	}

	if (((OpenMode & modeCreationMask) != modeAppend) && (OpenMode & modeReadWrite))
		_tcscat_s(ModeStr, 20, _T("+"));

	if (OpenMode & (osWriteThrough | osNoBuffer))
		m_IsWriteFlush = true;
	else
		m_IsWriteFlush = false;

	m_hFile = NULL;
	bool Ret = false;
	_tfopen_s(&m_hFile, FileName, ModeStr);
	if (m_hFile)
	{
		Ret = true;
	}

	if (Ret && ((OpenMode & modeCreationMask) == modeAppend))
	{
		fseek(m_hFile, 0, SEEK_END);
	}
	m_HaveError = (errno != 0);
	return Ret;
}

void CStandardFileAccessor::Close()
{
	m_HaveError = false;
	if (m_hFile)
	{
		fclose(m_hFile);
		m_HaveError = (errno != 0);
		m_hFile = NULL;
	}
}

ULONG64 CStandardFileAccessor::GetSize()
{
	m_HaveError = false;
	if (m_hFile)
	{
		long CurPos = ftell(m_hFile);
		fseek(m_hFile, 0, SEEK_END);
		long Size = ftell(m_hFile);
		fseek(m_hFile, CurPos, SEEK_SET);
		return Size;
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CStandardFileAccessor::Read(LPVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	ULONG64 Result = fread(pBuff, 1, (size_t)Size, m_hFile);
	m_HaveError = (errno != 0);
	return Result;
}

ULONG64 CStandardFileAccessor::Write(LPCVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	ULONG64 WriteSize = fwrite(pBuff, 1, (size_t)Size, m_hFile);
	if (m_IsWriteFlush)
	{
		fflush(m_hFile);
	}
	m_HaveError = (errno != 0);
	return WriteSize;
}


bool CStandardFileAccessor::IsEOF()
{
	m_HaveError = false;
	return feof(m_hFile);
}

bool CStandardFileAccessor::Seek(LONG64 Offset, int SeekMode)
{
	m_HaveError = false;
	if (fseek(m_hFile, (long)Offset, SeekMode) == 0)
		return true;
	m_HaveError = true;
	return 0;
}

ULONG64 CStandardFileAccessor::GetCurPos()
{
	m_HaveError = false;
	LONG64 Result = ftell(m_hFile);
	if (Result != -1)
		return Result;
	m_HaveError = true;
	return 0;
}

bool CStandardFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	return false;
}
bool CStandardFileAccessor::GetCreateTime(CEasyTime& Time)
{
	return false;
}

bool CStandardFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	return false;
}
bool CStandardFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	return false;
}

bool CStandardFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	return false;
}
bool CStandardFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	return false;
}
