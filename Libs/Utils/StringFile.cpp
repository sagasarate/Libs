/****************************************************************************/
/*                                                                          */
/*      文件名:    StringFile.cpp                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CStringFile::CStringFile( LPCTSTR pszTextFile ,bool bSplitLine,int FileChannel)
{
	m_FileChannel=FileChannel;
	m_pData = NULL;
	m_iDataSize = 0;
	m_pLines = NULL;
	m_iLineCount = 0;
	m_LineDelimiter=0;
	m_LocalCodePage=CP_ACP;
	m_SaveCodePage=CP_ACP;
	LoadFile( pszTextFile ,bSplitLine);
}
CStringFile::CStringFile(int FileChannel)
{
	m_FileChannel=FileChannel;
	m_pData = NULL;
	m_iDataSize = 0;
	m_pLines = NULL;
	m_iLineCount = 0;
	m_LineDelimiter=0;
	m_LocalCodePage=CP_ACP;
	m_SaveCodePage=CP_ACP;
}
CStringFile::~CStringFile()
{
	Destroy();
}
void CStringFile::Destroy()
{
	if( m_pLines != NULL )
	{
		delete []m_pLines;
		m_pLines = NULL;
	}
	m_iDataSize = 0;
	if( m_pData != NULL )
	{
		delete []m_pData;
		m_pData = NULL;
	}
	m_iLineCount = 0;
}
void CStringFile::MakeDeflate()
{
	UINT i = 0;
	TCHAR * p;
	TCHAR * p1;
	BOOL	bInString = FALSE;
	for( i = 0;i < GetLineCount();i ++ )
	{
		p = (*this)[i];
		p1 = p;
		while( *p != '\0' )
		{
			if( *p == '\"' )bInString = !bInString;
			if( (!bInString) && ( *p == ' ' || *p == '	' ) )
			{
				p++;
				continue;
			}
			*p1++=*p++;
		}
		*p1 = 0;
	}
}
BOOL CStringFile::LoadFile( LPCTSTR pszTextFile ,bool bSplitLine)
{
	Destroy();

	IFileAccessor * pFile;

	pFile=CFileSystemManager::GetInstance()->CreateFileAccessor(m_FileChannel);
	if(pFile==NULL)
		return false;
	if(!pFile->Open(pszTextFile,IFileAccessor::modeRead|IFileAccessor::shareShareAll))
	{
		pFile->Release();
		return false;
	}
	BOOL Ret=LoadFile(pFile,bSplitLine);
	pFile->Release();
	return Ret;
}

BOOL CStringFile::LoadFile( IFileAccessor * pFile ,bool bSplitLine)
{
	Destroy();

	CEasyBuffer Buffer;

	UINT FileSize=(UINT)pFile->GetSize();


	if (FileSize)
	{
		Buffer.Create(FileSize);
		if (pFile->Read(Buffer.GetBuffer(), FileSize) < FileSize)
		{
			return false;
		}
	}
	else
	{
		return true;
	}

	UINT BomHeader = GetBomHeader(Buffer.GetBuffer(), FileSize);


#ifdef UNICODE
	if(BomHeader==0)
	{
		m_iDataSize=AnsiToUnicode((char *)Buffer.GetBuffer(),FileSize,NULL,0);
		m_pData=new TCHAR[m_iDataSize+1];
		AnsiToUnicode((char *)Buffer.GetBuffer(),FileSize,m_pData,m_iDataSize);
	}
	else if(BomHeader==BMT_UNICODE)
	{
		m_iDataSize=(FileSize-2)/2;
		m_pData=new TCHAR[m_iDataSize+1];
		memcpy(m_pData,(char *)Buffer.GetBuffer()+2,m_iDataSize*sizeof(TCHAR));
	}
	else if(BomHeader==BMT_UTF_8)
	{
		m_iDataSize=MultiByteToWideChar(CP_UTF8,0,(char *)Buffer.GetBuffer()+3,FileSize-3,NULL,0);
		m_pData=new TCHAR[m_iDataSize+1];
		MultiByteToWideChar(CP_UTF8,0,(char *)Buffer.GetBuffer()+3,FileSize-3,m_pData,m_iDataSize);
	}
	else
	{
		return false;
	}
	m_pData[m_iDataSize]=0;
#else
	if(BomHeader==0)
	{
		if(m_LocalCodePage==CP_UTF8)
		{
			m_iDataSize=AnsiToUTF8((char *)Buffer.GetBuffer(),FileSize,NULL,0);
			m_pData=new TCHAR[m_iDataSize+1];
			AnsiToUTF8((char *)Buffer.GetBuffer(),FileSize,m_pData,m_iDataSize);
		}
		else
		{
			m_iDataSize=FileSize;
			m_pData=new TCHAR[m_iDataSize+1];
			memcpy(m_pData,Buffer.GetBuffer(),m_iDataSize*sizeof(TCHAR));
			m_pData[m_iDataSize]=0;
		}
	}
	else if(BomHeader==BMT_UNICODE)
	{
		if(m_LocalCodePage==CP_UTF8)
		{
			m_iDataSize=UnicodeToUTF8((wchar_t *)Buffer.GetBuffer()+1,FileSize/2-1,NULL,0);
			m_pData=new TCHAR[m_iDataSize+1];
			UnicodeToUTF8((wchar_t *)Buffer.GetBuffer()+1,FileSize/2-1,m_pData,m_iDataSize);
		}
		else
		{
			m_iDataSize=UnicodeToAnsi((wchar_t *)Buffer.GetBuffer()+1,FileSize/2-1,NULL,0);
			m_pData=new TCHAR[m_iDataSize+1];
			UnicodeToAnsi((wchar_t *)Buffer.GetBuffer()+1,FileSize/2-1,m_pData,m_iDataSize);
		}
	}
	else if(BomHeader==BMT_UTF_8)
	{
		if(m_LocalCodePage==CP_UTF8)
		{
			m_iDataSize=FileSize-3;
			m_pData=new TCHAR[m_iDataSize+1];
			memcpy(m_pData,(char *)Buffer.GetBuffer()+3,m_iDataSize*sizeof(TCHAR));
			m_pData[m_iDataSize]=0;
		}
		else
		{
			m_iDataSize=UTF8ToAnsi((char *)Buffer.GetBuffer()+3,FileSize-3,NULL,0);
			m_pData=new TCHAR[m_iDataSize+1];
			UTF8ToAnsi((char *)Buffer.GetBuffer()+3,FileSize-3,m_pData,m_iDataSize);
		}
	}
	else
	{
		return false;
	}
	m_pData[m_iDataSize]=0;
#endif
	if(bSplitLine)
	{
		m_iLineCount = ProcData();
		return BuildLines();
	}
	else
	{
		return true;
	}
}

BOOL CStringFile::LoadFromString(LPCTSTR pStr,int Len,bool bSplitLine)
{
	Destroy();

	if(Len<0)
		Len=(UINT)_tcslen(pStr);
	m_iDataSize=Len;
	m_pData=new TCHAR[m_iDataSize+2];
	_tcsncpy_s(m_pData,m_iDataSize+2,pStr,Len);
	m_pData[m_iDataSize] = 0;
	m_pData[m_iDataSize+1] = 0;
	if(bSplitLine)
	{
		m_iLineCount = ProcData();
		return BuildLines();
	}
	else
	{
		return true;
	}
}

BOOL CStringFile::SaveToFile(LPCTSTR pszTextFile)
{
	IFileAccessor * pFile;

	pFile=CFileSystemManager::GetInstance()->CreateFileAccessor(m_FileChannel);
	if(pFile==NULL)
		return false;
	if(!pFile->Open(pszTextFile,IFileAccessor::modeWrite|IFileAccessor::modeCreateAlways|IFileAccessor::shareShareAll))
	{
		pFile->Release();
		return false;
	}
	BOOL Ret=SaveToFile(pFile);
	pFile->Release();
	return Ret;
}

BOOL CStringFile::SaveToFile(IFileAccessor * pFile)
{
	BOOL Ret=TRUE;
#ifdef UNICODE
	if(m_SaveCodePage==CP_UTF8)
	{
		UINT BomHeader=BMT_UTF_8;
		pFile->Write(&BomHeader,3);
		size_t StrLen=_tcslen(m_pData);
		UINT64 WriteLen=UnicodeToUTF8(m_pData,StrLen,NULL,0);
		char * pBuffer=new char[WriteLen+1];
		WriteLen=UnicodeToUTF8(m_pData,StrLen,pBuffer,WriteLen);
		if(pFile->Write(pBuffer,WriteLen)!=WriteLen)
			Ret=FALSE;
		delete[] pBuffer;
	}
	else if (m_SaveCodePage == CP_ACP)
	{
		size_t StrLen = _tcslen(m_pData);
		UINT64 WriteLen = UnicodeToAnsi(m_pData, StrLen, NULL, 0);
		char * pBuffer = new char[WriteLen + 1];
		WriteLen = UnicodeToAnsi(m_pData, StrLen, pBuffer, WriteLen);
		if (pFile->Write(pBuffer, WriteLen) != WriteLen)
			Ret = FALSE;
		delete[] pBuffer;
	}
	else
	{
		UINT BomHeader=BMT_UNICODE;
		pFile->Write(&BomHeader,2);
		UINT64 WriteLen=_tcslen(m_pData)*sizeof(TCHAR);
		if(pFile->Write(m_pData,WriteLen)!=WriteLen)
			Ret=FALSE;
	}
#else
	if(m_SaveCodePage==CP_UTF8)
	{
		UINT BomHeader=BMT_UTF_8;
		pFile->Write(&BomHeader,3);
		size_t StrLen=_tcslen(m_pData);
		UINT64 WriteLen=AnsiToUTF8(m_pData,StrLen,NULL,0);
		char * pBuffer=new char[WriteLen+1];
		WriteLen=AnsiToUTF8(m_pData,StrLen,pBuffer,WriteLen);
		if(pFile->Write(pBuffer,WriteLen)!=WriteLen)
			Ret=FALSE;
		delete[] pBuffer;
	}
	else if (m_SaveCodePage == CP_UNICODE)
	{
		UINT BomHeader = BMT_UNICODE;
		pFile->Write(&BomHeader, 2);
		size_t StrLen = _tcslen(m_pData);
		UINT64 WriteLen = AnsiToUnicode(m_pData, StrLen, NULL, 0);
		wchar_t * pBuffer = new wchar_t[WriteLen + 1];
		WriteLen = AnsiToUnicode(m_pData, StrLen, pBuffer, WriteLen)*sizeof(wchar_t);
		if (pFile->Write(pBuffer, WriteLen) != WriteLen)
			Ret = FALSE;
		delete[] pBuffer;
	}
	else
	{
		UINT64 WriteLen=_tcslen(m_pData);
		if(pFile->Write(m_pData,WriteLen)!=WriteLen)
			Ret=FALSE;
	}
#endif
	return Ret;
}

TCHAR * CStringFile::operator[]( UINT line )
{
	if( line >= m_iLineCount )return NULL;
	return m_pLines[line];
}

BOOL CStringFile::BuildLines( )
{
	if( m_iLineCount == 0 )return FALSE;
	TCHAR * p = m_pData;
	m_pLines = new TCHAR*[m_iLineCount];
	UINT len = 0;
	UINT ptr = 0;
	for( UINT i = 0;i < m_iLineCount;i ++ )
	{
		len = (UINT)_tcslen( p );
		if( len > 0 )
			m_pLines[ptr++] = p;
		else
			break;
		p = p+len+1;
	}
	return TRUE;
}
UINT	CStringFile::ProcData()
{
	UINT i = 0;
	TCHAR * p = NULL;
	UINT linecount = 0;

	UINT rptr = 0;
	bool	binstring = false;
	bool	newlinestart = false;

	for( i = 0;i < m_iDataSize;i ++ )
	{
		p = m_pData + i;
		if(m_LineDelimiter&&m_LineDelimiter==(*p)&&newlinestart)
		{
			*(m_pData+rptr++) = 0;
			newlinestart = false;
			linecount++;
		}
		switch( *p )
		{
			//case	' ':
			//case	'	':
			//	if( binstring )
			//	{
			//		*(m_pData+rptr++) = *p;
			//		if( !newlinestart )newlinestart = true;
			//	}
			//	break;
		case	'\n':
		case	'\r':
			{
				if( newlinestart )
				{
					*(m_pData+rptr++) = 0;
					newlinestart = false;
					linecount++;
				}
			}
			break;
			//case	'\"':
			//	binstring = !binstring;
		default:
			{
				*(m_pData+rptr++) = *p;
				if( !newlinestart )newlinestart = true;
			}
			break;
		}
	}
	if( newlinestart )
		linecount++;

	m_pData[rptr++] = 0;
	m_pData[rptr++] = 0;
	m_iDataSize = rptr;
	return linecount;
}
