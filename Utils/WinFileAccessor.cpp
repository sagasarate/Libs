/****************************************************************************/
/*                                                                          */
/*      文件名:    WinFileAccessor.cpp                                      */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\winfileaccessor.h"

IMPLEMENT_CLASS_INFO_STATIC(CWinFileAccessor, IFileAccessor);

CWinFileAccessor::CWinFileAccessor(void)
{
	m_hFile = INVALID_HANDLE_VALUE;
	m_HaveError = false;
}

CWinFileAccessor::~CWinFileAccessor(void)
{
	Close();
}

bool CWinFileAccessor::Open(LPCTSTR FileName, int OpenMode)
{
	m_HaveError = false;

	if (m_hFile != INVALID_HANDLE_VALUE)
		Close();

	DWORD dwDesiredAccess;
	DWORD dwShareMode;
	DWORD dwCreationDispostion;
	DWORD dwFlagsAndAttributes;


	switch (OpenMode & modeAccessMask)
	{
	case modeRead:
		dwDesiredAccess = GENERIC_READ;
		break;
	case modeWrite:
		dwDesiredAccess = GENERIC_WRITE;
		break;
	case modeReadWrite:
		dwDesiredAccess = GENERIC_READ | GENERIC_WRITE;
		break;
	default:
		dwDesiredAccess = GENERIC_READ;
		break;
	}

	dwShareMode = 0;
	if (OpenMode & shareShareRead)
		dwShareMode |= FILE_SHARE_READ;
	if (OpenMode & shareShareWrite)
		dwShareMode |= FILE_SHARE_WRITE;


	switch (OpenMode & modeCreationMask)
	{
	case modeOpenAlways:
		dwCreationDispostion = OPEN_ALWAYS;
		break;
	case modeCreate:
		dwCreationDispostion = CREATE_NEW;
		break;
	case modeCreateAlways:
		dwCreationDispostion = CREATE_ALWAYS;
		break;
	case modeTruncate:
		dwCreationDispostion = TRUNCATE_EXISTING;
		break;
	case modeAppend:
		dwCreationDispostion = OPEN_ALWAYS;
		break;
	default:
		dwCreationDispostion = OPEN_EXISTING;
	}

	dwFlagsAndAttributes = FILE_ATTRIBUTE_NORMAL;
	if (OpenMode & AttrHidden)
		dwFlagsAndAttributes |= FILE_ATTRIBUTE_HIDDEN;
	if (OpenMode & AttrReadonly)
		dwFlagsAndAttributes |= FILE_ATTRIBUTE_READONLY;
	if (OpenMode & AttrSystem)
		dwFlagsAndAttributes |= FILE_ATTRIBUTE_SYSTEM;
	if (OpenMode & AttrArchive)
		dwFlagsAndAttributes |= FILE_ATTRIBUTE_ARCHIVE;

	if (OpenMode & osNoBuffer)
		dwFlagsAndAttributes |= FILE_FLAG_NO_BUFFERING;

	if (OpenMode & osWriteThrough)
		dwFlagsAndAttributes |= FILE_FLAG_WRITE_THROUGH;

	if (OpenMode & osRandomAccess)
		dwFlagsAndAttributes |= FILE_FLAG_RANDOM_ACCESS;

	if (OpenMode & osSequentialScan)
		dwFlagsAndAttributes |= FILE_FLAG_SEQUENTIAL_SCAN;



	m_hFile = CreateFile(FileName, dwDesiredAccess, dwShareMode, NULL,
		dwCreationDispostion, dwFlagsAndAttributes, NULL);


	if (m_hFile == INVALID_HANDLE_VALUE)
	{
		m_HaveError = true;
		return false;
	}
	else
	{
		if ((OpenMode & modeCreationMask) == modeAppend)
		{
			Seek(0, seekEnd);
		}
		return true;
	}
}

void CWinFileAccessor::Close()
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		if(!CloseHandle(m_hFile))
			m_HaveError = true;
		m_hFile = INVALID_HANDLE_VALUE;
	}
	else
	{
		m_HaveError = true;
	}
}

ULONG64 CWinFileAccessor::GetSize()
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		UINT64_CONVERTER Size;

		Size.QuadPart = 0;

		Size.LowPart = GetFileSize(m_hFile, (LPDWORD) & (Size.HighPart));
		if (Size.LowPart == INVALID_FILE_SIZE)
		{
			if (GetLastError() == NO_ERROR)
				return Size.QuadPart;
		}
		else
		{
			return Size.QuadPart;
		}
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CWinFileAccessor::Read(LPVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD RealSize = 0;
		if (ReadFile(m_hFile, pBuff, (DWORD)Size, &RealSize, NULL))
			return RealSize;
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CWinFileAccessor::Write(LPCVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		DWORD RealSize = 0;
		if (WriteFile(m_hFile, pBuff, (DWORD)Size, &RealSize, NULL))
			return RealSize;
	}
	m_HaveError = true;
	return 0;
}


bool CWinFileAccessor::IsEOF()
{
	return GetCurPos() == GetSize();
}

bool CWinFileAccessor::Seek(LONG64 Offset, int SeekMode)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		UINT64_CONVERTER Size;
		Size.QuadPart = Offset;
		Size.LowPart = SetFilePointer(m_hFile, Size.LowPart, (PLONG) & (Size.HighPart), SeekMode);
		if (Size.LowPart != INVALID_SET_FILE_POINTER || GetLastError() == NO_ERROR)
			return true;
	}
	m_HaveError = true;
	return false;
}

ULONG64 CWinFileAccessor::GetCurPos()
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		UINT64_CONVERTER Size;
		Size.QuadPart = 0;
		Size.LowPart = SetFilePointer(m_hFile, 0, (PLONG) & (Size.HighPart), FILE_CURRENT);
		if (Size.LowPart == INVALID_SET_FILE_POINTER)
		{
			if (GetLastError() == NO_ERROR)
			{
				return Size.QuadPart;
			}				
			else
			{
				m_HaveError = true;
				return 0;
			}				
		}
		else
		{
			return Size.QuadPart;
		}

	}
	else
	{
		m_HaveError = true;
		return 0;
	}		
}

bool CWinFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime = Time;
		if (SetFileTime(m_hFile, &FTime, NULL, NULL))
		{
			return true;
		}
	}
	m_HaveError = true;
	return false;
}
bool CWinFileAccessor::GetCreateTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime;
		if (GetFileTime(m_hFile, &FTime, NULL, NULL))
		{
			Time = FTime;
			return true;
		}
	}
	m_HaveError = true;
	return false;
}

bool CWinFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime = Time;
		if (SetFileTime(m_hFile, NULL, &FTime, NULL))
		{
			return true;
		}
	}
	m_HaveError = true;
	return false;
}
bool CWinFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime;
		if (GetFileTime(m_hFile, NULL, &FTime, NULL))
		{
			Time = FTime;
			return true;
		}
	}
	m_HaveError = true;
	return false;
}

bool CWinFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime = Time;
		if (SetFileTime(m_hFile, NULL, NULL, &FTime))
		{
			return true;
		}
	}
	m_HaveError = true;
	return false;
}
bool CWinFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_hFile != INVALID_HANDLE_VALUE)
	{
		FILETIME FTime;
		if (GetFileTime(m_hFile, NULL, NULL, &FTime))
		{
			Time = FTime;
			return true;
		}
	}
	m_HaveError = true;
	return false;
}