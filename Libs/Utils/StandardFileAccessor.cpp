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


IMPLEMENT_CLASS_INFO_STATIC(CStandardFileAccessor,IFileAccessor);

CStandardFileAccessor::CStandardFileAccessor(void):IFileAccessor()
{
	m_hFile=NULL;
	m_IsWriteFlush=false;
}

CStandardFileAccessor::~CStandardFileAccessor(void)
{
	Close();
}


BOOL CStandardFileAccessor::Open(LPCTSTR FileName,int OpenMode)
{
	if(m_hFile)
		Close();

	TCHAR ModeStr[20];

	ModeStr[0]=0;
	switch(OpenMode&modeCreationMask)
	{
	case modeOpenAlways:
		_tcscat_s(ModeStr,20,_T("r"));
		break;
	case modeCreate:
		_tcscat_s(ModeStr,20,_T("w"));
		break;
	case modeCreateAlways:
		_tcscat_s(ModeStr,20,_T("w"));
		break;
	case modeTruncate:
		_tcscat_s(ModeStr,20,_T("w"));
		break;
	case modeAppend:
		_tcscat_s(ModeStr,20,_T("a+"));
		break;
	default:
		_tcscat_s(ModeStr,20,_T("r"));
	}

	if(((OpenMode&modeCreationMask)!=modeAppend)&&(OpenMode&modeReadWrite))
		_tcscat_s(ModeStr,20,_T("+"));

	if(OpenMode&(osWriteThrough|osNoBuffer))
		m_IsWriteFlush=true;
	else
		m_IsWriteFlush=false;

	m_hFile=NULL;
	BOOL Ret=FALSE;
	_tfopen_s(&m_hFile,FileName,ModeStr);
	if(m_hFile)
	{
		Ret=TRUE;
	}

	if(Ret&&((OpenMode&modeCreationMask)==modeAppend))
	{
		fseek(m_hFile,0,SEEK_END);
	}

	return Ret;
}

void CStandardFileAccessor::Close()
{
	if(m_hFile)
	{
		fclose(m_hFile);
		m_hFile=NULL;
	}
}

ULONG64 CStandardFileAccessor::GetSize()
{
	if(m_hFile)
	{
		long CurPos=ftell(m_hFile);
		fseek(m_hFile,0,SEEK_END);
		long Size=ftell(m_hFile);
		fseek(m_hFile,CurPos,SEEK_SET);
		return Size;
	}
	else
		return 0;
}

ULONG64 CStandardFileAccessor::Read(LPVOID pBuff,ULONG64 Size)
{
	return fread(pBuff,1,(size_t)Size,m_hFile);
}

ULONG64 CStandardFileAccessor::Write(LPCVOID pBuff,ULONG64 Size)
{
	ULONG64 WriteSize=fwrite(pBuff,1,(size_t)Size,m_hFile);
	if(m_IsWriteFlush)
	{
		fflush(m_hFile);
	}
	return WriteSize;
}


BOOL CStandardFileAccessor::IsEOF()
{
	return feof(m_hFile);
}

BOOL CStandardFileAccessor::Seek(LONG64 Offset,int SeekMode)
{
	if(fseek(m_hFile,(long)Offset,SeekMode)==0)
		return true;
	else
		return false;
}

ULONG64 CStandardFileAccessor::GetCurPos()
{
	return ftell(m_hFile);
}

BOOL CStandardFileAccessor::SetCreateTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CStandardFileAccessor::GetCreateTime(CEasyTime& Time)
{
	return FALSE;
}

BOOL CStandardFileAccessor::SetLastAccessTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CStandardFileAccessor::GetLastAccessTime(CEasyTime& Time)
{
	return FALSE;
}

BOOL CStandardFileAccessor::SetLastWriteTime(const CEasyTime& Time)
{
	return FALSE;
}
BOOL CStandardFileAccessor::GetLastWriteTime(CEasyTime& Time)
{
	return FALSE;
}
