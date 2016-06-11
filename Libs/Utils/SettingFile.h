/****************************************************************************/
/*                                                                          */
/*      文件名:    SettingFile.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CSettingFile
{
protected:
	CEasyArray<CEasyString> m_SettingStrings;
	int	m_FileChannel;
public:
	CSettingFile(int FileChannel=0)
	{
		m_FileChannel=FileChannel;
	}
	bool LoadFromFile(LPCTSTR FileName)
	{
		IFileAccessor * pFile;

		pFile=CFileSystemManager::GetInstance()->CreateFileAccessor(m_FileChannel);
		if(pFile==NULL)
			return false;
		if(!pFile->Open(FileName,IFileAccessor::modeRead|IFileAccessor::shareShareAll))
		{
			pFile->Release();
			return false;	
		}
		int FileSize=(int)pFile->GetSize();	
		BYTE * Str=new BYTE[FileSize+2];
		pFile->Read(Str,FileSize);
		pFile->Release();
		Str[FileSize]=0;
		Str[FileSize+1]=0;
		bool IsUnicode=false;
		if(FileSize>=2)
		{
			if(Str[0]==0xFF&&Str[1]==0xFE)
				IsUnicode=true;
		}
#ifdef UNICODE
		if(!IsUnicode)
		{
			size_t DestLen=AnsiToUnicode((char *)Str,FileSize,NULL,0);
			WCHAR * pNewStr=new WCHAR[DestLen+1];
			DestLen=AnsiToUnicode((char *)Str,FileSize,pNewStr,DestLen);
			delete[] Str;
			Str=(BYTE *)pNewStr;
		}
		
#else
		if(IsUnicode)
		{
			size_t DestLen=UnicodeToAnsi((WCHAR *)Str,FileSize/sizeof(WCHAR),NULL,0);
			char * pNewStr=new char[DestLen+1];
			DestLen=UnicodeToAnsi((WCHAR *)Str,FileSize/sizeof(WCHAR),pNewStr,DestLen);
			delete[] Str;
			Str=(BYTE *)pNewStr;
		}
#endif
		bool ret=Load((LPCTSTR)Str);
		delete[] Str;
		return ret;		
	}

	bool Load(LPCTSTR SettingString,TCHAR LineDelimiter='\r',TCHAR CommentHead=';')
	{
		TCHAR * Source;
		TCHAR * Line;
		size_t Len=_tcslen(SettingString)+1;
		TCHAR * pBuff=new TCHAR[Len];
		Source=pBuff;
		_tcscpy_s(Source,Len,SettingString);
		m_SettingStrings.Clear();
		while(Source)
		{
			TCHAR * Find=(TCHAR *)FindNextLine(Source,LineDelimiter);
			if(Find)
			{
				Find[0]=0;
				Find++;
				Line=Source;
				Source=Find;
			}
			else
			{
				Line=Source;
				Source=NULL;
			}
			Find=(TCHAR *)FindNextLine(Line,CommentHead);
			if(Find)
				Find[0]=0;
			CEasyString temp=Line;
			StringTrim(temp,' ');
			StringTrim(temp,LineDelimiter);
			StringTrim(temp,'\r');
			StringTrim(temp,'\n');
			if (temp.GetLength() >= 2)
			{
				//对于用引号包起的字串，去掉引号
				if (temp[0] == '"' && temp[temp.GetLength() - 1] == '"')
				{
					temp = temp.SubStr(1, temp.GetLength() - 2);
				}
			}
			if(!temp.IsEmpty())
				m_SettingStrings.Add(temp);
		}
		delete[] pBuff;
		return true;
	}

	const TCHAR * GetString( const TCHAR * pszSection, const TCHAR * pszItemName, const TCHAR * pszDefValue = NULL )
	{
		LPCTSTR p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return pszDefValue;
		return p;
	}
	int	GetInteger( const TCHAR * pszSection, const TCHAR * pszItemName, int DefValue = 0 )
	{
		LPCTSTR p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return DefValue;
		return _tstoi(p);
	}
	__int64	GetInteger64( const TCHAR * pszSection, const TCHAR * pszItemName, int DefValue = 0 )
	{
		LPCTSTR p = GetSettingString( pszSection, pszItemName );
		if( p == NULL )return DefValue;
		return _tstoi64(p);
	}
	double GetDouble(const TCHAR * pszSection, const TCHAR * pszItemName, double DefValue = 0 )
	{
		LPCTSTR p = GetSettingString( pszSection, pszItemName );
		if( p == NULL ) return DefValue;
		return _tstof(p);
	}
protected:
	int	FindSectionLine( const TCHAR * pszSection )
	{
		if( pszSection == NULL )
			return 0;
		int linecount = (int)m_SettingStrings.GetCount();
		int sectionlength = (int)_tcslen( pszSection );
		for( int i = 0;i < linecount;i ++ )
		{
			LPCTSTR p = m_SettingStrings[i];

			if( *p == '[' && *(p+sectionlength+1)==']' )
			{
				if( _tcsnicmp( p+1, pszSection, sectionlength ) == 0 )
					return (i+1);
			}
		}
		return -1;
	}
	LPCTSTR GetSettingString(  const TCHAR * pszSection, const TCHAR * pszItemName )
	{
		int startindex = 0;
		if( pszSection != NULL )
		{
			startindex = FindSectionLine( pszSection );
			if( startindex == -1 )return NULL;
		}

		int itemnamelength = (int)_tcslen( pszItemName );
		if( itemnamelength == 0 )return NULL;

		int linecount = (int)m_SettingStrings.GetCount();
		for( int i = startindex;i < linecount;i ++ )
		{
			LPCTSTR p = m_SettingStrings[i];
			//	如果到达下一个section， 返回错误
			if( *p == '[' )return NULL;
			//	如果是ItemName=这样的句式，进入进一步搜索
			if( *(p+itemnamelength) == '=' )
			{
				if( _tcsnicmp( p, pszItemName, itemnamelength ) == 0 )
				{
					if( *(p+itemnamelength+1) == '\"' )
					{
						TCHAR * pret = (TCHAR *)(p+itemnamelength+2);
						int length = (int)_tcslen( pret );
						if( *( pret + length-1 ) == '\"' )
							*( pret + length-1 ) = 0;
						return pret;
					}
					return (p+itemnamelength+1);
				}
			}
		}
		return NULL;

	}		
protected:
	LPCTSTR FindNextLine(LPCTSTR pStr,TCHAR LineDelimiter)
	{
		bool InStr=false;
		while(*pStr)
		{
			if(*pStr=='"')
				InStr=!InStr;
			if(*pStr==LineDelimiter&&(!InStr))
				return pStr;
			pStr++;

		}
		return NULL;
	}
};
