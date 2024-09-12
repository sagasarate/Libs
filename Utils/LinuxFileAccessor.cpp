/****************************************************************************/
/*                                                                          */
/*      文件名:    LinuxFileAccessor.cpp                                    */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

IMPLEMENT_CLASS_INFO_STATIC(CLinuxFileAccessor, IFileAccessor);

CLinuxFileAccessor::CLinuxFileAccessor(void)
{
	m_FileDescriptor = INVALID_HANDLE_VALUE;
	m_IsWriteFlush = false;
	m_HaveError = false;
}

CLinuxFileAccessor::~CLinuxFileAccessor(void)
{
	Close();
}


bool CLinuxFileAccessor::Open(LPCTSTR FileName, int OpenMode)
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
		Close();

	int Flag = 0;

	switch (OpenMode & modeAccessMask)
	{
	case modeRead:
		Flag = O_RDONLY;
		break;
	case modeWrite:
		Flag = O_WRONLY;
		break;
	case modeReadWrite:
		Flag = O_RDWR;
		break;
	}

	switch (OpenMode & modeCreationMask)
	{
	case modeOpenAlways:
		Flag |= O_CREAT;
		break;
	case modeCreate:
		Flag |= O_EXCL | O_CREAT | O_TRUNC;
		break;
	case modeCreateAlways:
		Flag |= O_CREAT | O_TRUNC;
		break;
	case modeTruncate:
		Flag |= O_TRUNC;
		break;
	case modeAppend:
		Flag |= O_CREAT | O_APPEND;
		break;
	}

	if (OpenMode & (osWriteThrough | osNoBuffer))
		m_IsWriteFlush = true;
	else
		m_IsWriteFlush = false;

	m_FileDescriptor = open(FileName, Flag, S_IRWXU);

	if (m_FileDescriptor == INVALID_HANDLE_VALUE)
	{
		printf(_T("File Create Failed:%d\r\n"), errno);
		return false;
	}
	else
	{
		if ((OpenMode & KeepOnExec) == 0)
		{
			fcntl(m_FileDescriptor, F_SETFD, FD_CLOEXEC);
		}
		m_FileInfo.FetchFileInfo(FileName);
		m_HaveError = true;
		return true;
	}

}
void CLinuxFileAccessor::Close()
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		if (close(m_FileDescriptor) == -1)
			m_HaveError = true;
		m_FileDescriptor = INVALID_HANDLE_VALUE;
	}
}

ULONG64 CLinuxFileAccessor::GetSize()
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		off_t CurPos = lseek(m_FileDescriptor, 0, SEEK_CUR);
		off_t FileSize = lseek(m_FileDescriptor, 0, SEEK_END);
		if ((CurPos != -1) && (FileSize != -1) && (lseek(m_FileDescriptor, CurPos, SEEK_SET) != -1))
		{
			return FileSize;
		}
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CLinuxFileAccessor::Read(LPVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		LONG64 ReadSize = read(m_FileDescriptor, pBuff, (size_t)Size);
		if (ReadSize != -1)
		{
			return (ULONG64)ReadSize;
		}
	}
	m_HaveError = true;
	return 0;
}

ULONG64 CLinuxFileAccessor::Write(LPCVOID pBuff, ULONG64 Size)
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		LONG64 WriteSize = write(m_FileDescriptor, pBuff, (size_t)Size);
		if (WriteSize != -1)
		{
			if (m_IsWriteFlush)
			{
				fsync(m_FileDescriptor);
			}
			return (ULONG64)WriteSize;
		}
	}
	m_HaveError = true;
	return 0;
}


bool CLinuxFileAccessor::IsEOF()
{
	return GetCurPos() == GetSize();
}

bool CLinuxFileAccessor::Seek(LONG64 Offset, int SeekMode)
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		int Whence = SEEK_SET;
		switch (SeekMode)
		{
		case seekBegin:
			Whence = SEEK_SET;
			break;
		case seekCurrent:
			Whence = SEEK_CUR;
			break;
		case seekEnd:
			Whence = SEEK_END;
			break;
		}
		if (lseek(m_FileDescriptor, (off_t)Offset, Whence) != -1)
			return true;

	}
	m_HaveError = true;
	return false;
}
ULONG64 CLinuxFileAccessor::GetCurPos()
{
	m_HaveError = false;
	if (m_FileDescriptor != INVALID_HANDLE_VALUE)
	{
		LONG64 Result = lseek(m_FileDescriptor, 0, SEEK_CUR);
		if (Result != -1)
			return Result;
	}
	m_HaveError = true;
	return 0;
}

bool CLinuxFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	return false;
}
bool CLinuxFileAccessor::GetCreateTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_FileInfo.IsOK())
	{
		Time = m_FileInfo.GetCreateTime();
		return true;
	}
	m_HaveError = true;
	return false;
}

bool CLinuxFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	m_HaveError = false;
	if (m_FileInfo.SetLastAccessTime(Time))
	{
		return true;
	}
	m_HaveError = true;
	return false;
}
bool CLinuxFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_FileInfo.IsOK())
	{
		Time = m_FileInfo.GetLastAccessTime();
		return true;
	}
	m_HaveError = true;
	return false;
}

bool CLinuxFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	m_HaveError = false;
	if (m_FileInfo.SetLastWriteTime(Time))
	{
		return true;
	}
	m_HaveError = true;
	return false;
}
bool CLinuxFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	m_HaveError = false;
	if (m_FileInfo.IsOK())
	{
		Time = m_FileInfo.GetLastWriteTime();
		return true;
	}
	m_HaveError = true;
	return false;
}
