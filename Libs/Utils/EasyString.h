/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyString.h                                             */
/*      创建日期:  2009年07月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#ifdef UNICODE

#define _tcsistr	wcsistr
#define _tcsichr	wcsichr
#define _tcsrichr	wcsrichr

#else

#define _tcsistr	stristr
#define _tcsichr	strichr
#define _tcsrichr	strrichr

#endif



#define RELEASE_STR_BUFF(pBuffer) \
	if(pBuffer != EMPTY_STR)\
	{\
		SAFE_DELETE_ARRAY(pBuffer);\
	}\
	else\
	{\
		pBuffer = NULL;\
	}\

inline int strncpy_0(char *strDest,size_t DestSize, const char *strSource, size_t count)
{
	if(strDest&&strSource)
	{
		int Ret=strncpy_s(strDest,DestSize,strSource,count-1);
		strDest[count-1]=0;
		return Ret;
	}
	return 0;
}

inline int strncpy_0( WCHAR *strDest,size_t DestSize, const WCHAR *strSource, size_t count )
{
	if(strDest&&strSource)
	{
		int Ret=wcsncpy_s(strDest,DestSize,strSource,count-1);
		strDest[count-1]=0;
		return Ret;
	}
	return 0;
}

inline char *  stristr (
	const char * str1,
	const char * str2
					   )
{
	char *cp = (char *)str1;
	char *s1, *s2;

	if ( !*str2 )
		return((char *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char *)str2;

		while (*s1 && *s2 && !(tolower(*s1) - tolower(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}
	return(NULL);

}

inline WCHAR *  wcsistr(
	const WCHAR * str1,
	const WCHAR * str2
	)
{
	WCHAR *cp = (WCHAR *)str1;
	WCHAR *s1, *s2;

	if (!*str2)
		return((WCHAR *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (WCHAR *)str2;

		while (*s1 && *s2 && !(towlower(*s1) - towlower(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}
	return(NULL);

}

inline char *  strichr(const char * str, char chr)
{
	while (*str && tolower(*str) != tolower(chr))
		str++;

	if (*str != 0)
		return((char *)str);

	return(NULL);
}

inline WCHAR *  wcsichr(const WCHAR * str, WCHAR chr)
{
	while (*str && towlower(*str) != towlower(chr))
		str++;

	if (*str != 0)
		return((WCHAR *)str);

	return(NULL);
}

inline char * strrichr(const char * str, int chr)
{
	char *start = (char *)str;
	while (*str++);                       /* find end of string */
	/* search towards front */
	while (--str != start && tolower(*str) != tolower(chr));

	if (str != start)               /* char found ? */
		return((char *)str);
	return(NULL);
}

inline WCHAR * wcsrichr(const WCHAR * str, int chr)
{
	WCHAR *start = (WCHAR *)str;
	while (*str++);                       /* find end of string */
	/* search towards front */
	while (--str != start && towlower(*str) != towlower(chr));

	if (str != start)               /* char found ? */
		return((WCHAR *)str);
	return(NULL);
}


inline int CompareString(const char * pStr1,const char * pStr2)
{
	return strcmp(pStr1,pStr2);
}
inline int CompareString(const WCHAR * pStr1,const WCHAR * pStr2)
{
	return wcscmp(pStr1,pStr2);
}
inline int CompareStringWithLen(const char * pStr1,const char * pStr2,size_t Len)
{
	return strncmp(pStr1,pStr2,Len);
}
inline int CompareStringWithLen(const WCHAR * pStr1,const WCHAR * pStr2,size_t Len)
{
	return wcsncmp(pStr1,pStr2,Len);
}
inline int CompareStringNoCase(const char * pStr1,const char * pStr2)
{
	return _stricmp(pStr1,pStr2);
}
inline int CompareStringNoCase(const WCHAR * pStr1,const WCHAR * pStr2)
{
	return _wcsicmp(pStr1,pStr2);
}
inline int CompareStringNoCaseWithLen(const char * pStr1,const char * pStr2,size_t Len)
{
	return _strnicmp(pStr1,pStr2,Len);
}
inline int CompareStringNoCaseWithLen(const WCHAR * pStr1,const WCHAR * pStr2,size_t Len)
{
	return _wcsnicmp(pStr1,pStr2,Len);
}

template <typename T>
inline T * StringTrimRightT(T * pStr,T TrimChar)
{
	T * pStrTail = pStr;
	while(*pStrTail)
	{
		pStrTail++;
	}
	if(pStrTail==pStr)
		return pStr;
	pStrTail--;
	while(pStrTail!=pStr&&(*pStrTail==TrimChar))
	{
		*pStrTail=0;
	}
	return pStr;
}

template < typename T>
inline T * StringTrimLeftT(T * pStr,T TrimChar)
{
	T * ResultStr=pStr;
	while(*ResultStr==TrimChar)
	{
		ResultStr++;
	}
	return ResultStr;
}

template < typename T>
inline T * StringTrimT(T * pStr,T TrimChar)
{
	T * ResultStr=pStr;
	while(*ResultStr==TrimChar)
	{
		ResultStr++;
	}
	pStr=ResultStr;


	T * pStrTail=pStr;
	while(*pStrTail)
	{
		pStrTail++;
	}
	if(pStrTail==pStr)
		return pStr;
	pStrTail--;
	while(pStrTail!=pStr&&(*pStrTail==TrimChar))
	{
		*pStrTail=0;
	}
	return pStr;
}

#define StringTrimLeft	StringTrimLeftT<TCHAR>
#define StringTrimLeftA	StringTrimLeftT<char>
#define StringTrimLeftW	StringTrimLeftT<WCHAR>

#define StringTrimRight		StringTrimRightT<TCHAR>
#define StringTrimRightA	StringTrimRightT<char>
#define StringTrimRightW	StringTrimRightT<WCHAR>

#define StringTrim	StringTrimT<TCHAR>
#define StringTrimA	StringTrimT<char>
#define StringTrimW	StringTrimT<WCHAR>



template < typename T>
class CEasyStringT
{
public:
	typedef size_t SIZE_TYPE;
	enum
	{
		ERROR_CHAR_POS=-1,
	};
	static T ZERO_CHAR;
	static T * EMPTY_STR;
protected:
	T*			m_pBuffer;
	SIZE_TYPE	m_BufferSize;
	SIZE_TYPE	m_StringLength;
	UINT		m_HashCode;

	static int	m_DefaultCodePage;	
public:

	static void SetDefaultCodePage(int CodePage)
	{
		m_DefaultCodePage = CodePage;
	}

	static int GetDefaultCodePage()
	{
		return m_DefaultCodePage;
	}

	CEasyStringT()
	{
		m_pBuffer = CEasyStringT<T>::EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
	}
	CEasyStringT(const char * pStr)
	{
		m_pBuffer = EMPTY_STR;		
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const WCHAR * pStr)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const char * pStr,SIZE_TYPE Size)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(pStr,Size);
	}
	CEasyStringT(const WCHAR * pStr,SIZE_TYPE Size)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(pStr,Size);
	}
	CEasyStringT(const char Char)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(&Char,1);
	}
	CEasyStringT(const WCHAR Char)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		m_HashCode = 0;
		SetString(&Char,1);
	}
	CEasyStringT(const CEasyStringT<char>& Str)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.GetBuffer(),Str.GetLength());
	}
	CEasyStringT(const CEasyStringT<WCHAR>& Str)
	{
		m_pBuffer = EMPTY_STR;
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.GetBuffer(),Str.GetLength());
	}
	~CEasyStringT()
	{
		RELEASE_STR_BUFF(m_pBuffer);
	}
	void Clear()
	{
		Resize(0,false);
	}	
	bool IsUnicode()
	{
		return sizeof(T)==sizeof(WCHAR);
	}
	void Resize(SIZE_TYPE Size,bool ReserveData=true)
	{
		if (Size == m_StringLength)
			return;
		T * pNewBuffer=new T[Size+1];
		SIZE_TYPE NewStringLen=0;
		if(m_pBuffer&&ReserveData)
		{
			if(m_StringLength>Size)
				NewStringLen=Size;
			else
				NewStringLen=m_StringLength;
			memcpy(pNewBuffer,m_pBuffer,sizeof(T)*NewStringLen);
		}
		pNewBuffer[NewStringLen]=0;
		pNewBuffer[Size]=0;
		m_BufferSize=Size+1;
		m_StringLength=NewStringLen;
		RELEASE_STR_BUFF(m_pBuffer);
		m_pBuffer=pNewBuffer;
		m_HashCode = 0;
	}
	void TrimBuffer(SIZE_TYPE Size=0)
	{
		if(Size<=0)
		{
			Size=m_BufferSize-1;
			for(SIZE_TYPE i=0;i<m_BufferSize;i++)
			{
				if(m_pBuffer[i]==0)
				{
					Size=i;
					break;
				}
			}
		}
		m_StringLength=Size;
		Resize(m_StringLength);
	}
	UINT GetHashCode() const
	{
		if (m_HashCode == 0)
		{
			UINT * pHashCode = (UINT *)(&m_HashCode);
			*pHashCode = 1;
			for (SIZE_TYPE i = 0; i < m_StringLength; i++)
			{
				*pHashCode = (*pHashCode) * 31 + m_pBuffer[i];
			}
		}
		return m_HashCode;
	}
	void SetString(const char * pStr,SIZE_TYPE Size);
	void SetString(const WCHAR * pStr,SIZE_TYPE Size);
	void AppendString(const char * pStr,SIZE_TYPE Size);
	void AppendString(const WCHAR * pStr,SIZE_TYPE Size);
	SIZE_TYPE GetLength() const
	{
		return m_StringLength;
	}
	void SetLength(SIZE_TYPE Len)
	{
		if(Len<m_BufferSize)
		{
			m_StringLength=Len;
			m_pBuffer[m_StringLength]=0;
		}
	}
	SIZE_TYPE GetBufferSize() const
	{
		return m_BufferSize;
	}
	bool IsEmpty() const
	{
		return m_StringLength==0;
	}
	const T* GetBuffer() const
	{
		return m_pBuffer;
	}
	operator const T*() const
	{
		return m_pBuffer;
	}
	operator T*()
	{
		return m_pBuffer;
	}
	//const T& operator[](SIZE_TYPE Index) const
	//{
	//	return m_pBuffer[Index];
	//}
	//T& operator[](SIZE_TYPE Index)
	//{
	//	return m_pBuffer[Index];
	//}
	void SetChar(SIZE_TYPE Index, T Char)
	{
		m_pBuffer[Index]=Char;
		m_HashCode = 0;
	}
	CEasyStringT<T>& operator=(const char* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(const WCHAR* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(char Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(WCHAR Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<char>& Str)
	{
		SetString(Str.GetBuffer(),Str.GetLength());
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<WCHAR>& Str)
	{
		SetString(Str.GetBuffer(),Str.GetLength());
		return *this;
	}
	int Compare(const T* pStr) const
	{
		if(m_pBuffer)
			return CompareString(m_pBuffer,pStr);
		else
			return ERROR_CHAR_POS;
	}
	int Compare(SIZE_TYPE Start,SIZE_TYPE Len,const T* pStr) const
	{
		if(m_pBuffer&&Start+Len<=m_StringLength)
			return CompareStringWithLen(m_pBuffer+Start,pStr,Len);
		else
			return ERROR_CHAR_POS;
	}
	int CompareNoCase(const T * pStr) const
	{
		if(m_pBuffer)
			return CompareStringNoCase(m_pBuffer,pStr);
		else
			return ERROR_CHAR_POS;
	}
	int CompareNoCase(SIZE_TYPE Start,SIZE_TYPE Len,const T * pStr) const
	{
		if(m_pBuffer)
			return CompareStringNoCaseWithLen(m_pBuffer+Start,pStr,Len);
		else
			return ERROR_CHAR_POS;
	}	


	friend bool operator==(const T* pStr1,const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) == 0;
	}
	friend bool operator==(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) == 0;
	}
	friend bool operator==(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) == 0;
	}


	friend bool operator!=(const T* pStr1, const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) != 0;
	}
	friend bool operator!=(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) != 0;
	}
	friend bool operator!=(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) != 0;
	}


	friend bool operator>(const T* pStr1, const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) > 0;
	}
	friend bool operator>(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) > 0;
	}
	friend bool operator>(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) > 0;
	}


	friend bool operator<(const T* pStr1, const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) < 0;
	}
	friend bool operator<(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) < 0;
	}
	friend bool operator<(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) < 0;
	}

	friend bool operator>=(const T* pStr1, const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) >= 0;
	}
	friend bool operator>=(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) >= 0;
	}
	friend bool operator>=(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) >= 0;
	}

	friend bool operator<=(const T* pStr1, const CEasyStringT& Str2)
	{
		return Str2.Compare(pStr1) <= 0;
	}
	friend bool operator<=(const CEasyStringT& Str1, const T* pStr2)
	{
		return Str1.Compare(pStr2) <= 0;
	}
	friend bool operator<=(const CEasyStringT& Str1, const CEasyStringT& Str2)
	{
		return Str1.Compare(Str2) <= 0;
	}


	const CEasyStringT<T>& operator+=(const char * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(const WCHAR * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(char Char)
	{
		AppendString(&Char,1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(WCHAR Char)
	{
		AppendString(&Char,1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(const CEasyStringT<T>& Str)
	{
		AppendString(Str.m_pBuffer,Str.m_StringLength);
		return *this;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const char * pStr2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const WCHAR * pStr2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const  char * pStr1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=pStr1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const  WCHAR * pStr1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=pStr1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String.SetString(Str1.m_pBuffer,Str1.m_StringLength);
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,char Char2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,WCHAR Char2)
	{
		CEasyStringT<T> String;
		String=Str1;
		String+=Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(char Char1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=Char1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(WCHAR Char1,const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String=Char1;
		String.AppendString(Str2.m_pBuffer,Str2.m_StringLength);
		return String;
	}
	void TrimLeft(T TrimChar=' ')
	{
		T* pTrimedStr=StringTrimLeftT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	void TrimRight(T TrimChar=' ')
	{
		T* pTrimedStr=StringTrimRightT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	void Trim(T TrimChar=' ')
	{
		T* pTrimedStr=StringTrimT<T>(m_pBuffer,TrimChar);
		SetString(pTrimedStr,GetStrLen(pTrimedStr));
	}
	CEasyStringT<T> SubStr(SIZE_TYPE Start,SIZE_TYPE Number) const
	{
		if(Start>m_StringLength)
			Start=m_StringLength;
		if(Start+Number>m_StringLength)
			Number=m_StringLength-Start;
		return CEasyStringT<T>(m_pBuffer+Start,Number);
	}
	CEasyStringT<T> Left(SIZE_TYPE Number) const
	{
		if(Number>m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer,Number);
	}
	CEasyStringT<T> Right(SIZE_TYPE Number) const
	{
		if(Number>m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer+m_StringLength-Number,Number);
	}
	void FormatVL(const char * pFormat,va_list vl);
	void FormatVL(const WCHAR * pFormat,va_list vl);
	void Format(const char * pFormat,...);
	void Format(const WCHAR * pFormat,...);
	void MakeUpper();
	void MakeLower();
	int Find(const char * pDestStr, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	int Find(const WCHAR * pDestStr, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	int Find(char DestChar, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	int Find(WCHAR DestChar, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	int ReverseFind(char DestChar, bool IgnoreCase = false) const;
	int ReverseFind(WCHAR DestChar, bool IgnoreCase = false) const;
	void Replace(T OldChar,T NewChar)
	{
		if(OldChar!=NewChar)
		{
			for(SIZE_TYPE i=0;i<m_StringLength;i++)
			{
				if(m_pBuffer[i]==OldChar)
					m_pBuffer[i]=NewChar;
			}
			m_HashCode = 0;
		}
	}
	void Replace(const T * pOldStr,const T * pNewStr)
	{
		if(CompareString(pOldStr,pNewStr)!=0)
		{
			int StartPos=0;
			int ReplaceCount=0;
			int ReplaceSrcLen=(int)GetStrLen(pOldStr);
			int ReplaceDestLen=(int)GetStrLen(pNewStr);
			int * pReplacePos=new int[m_StringLength/ReplaceSrcLen+2];
			int FindPos=Find(pOldStr,StartPos);
			while(FindPos>=0)
			{
				pReplacePos[ReplaceCount]=FindPos;
				ReplaceCount++;
				StartPos=FindPos+ReplaceSrcLen;
				FindPos=Find(pOldStr,StartPos);
			}


			SIZE_TYPE NewStrLen=(int)m_StringLength+ReplaceCount*(ReplaceDestLen-ReplaceSrcLen);
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);

			if(NewStrLen>=m_StringLength)
			{
				int SrcPos=(int)m_StringLength;
				int DestPos=(int)NewStrLen;
				while(ReplaceCount)
				{
					ReplaceCount--;
					int OldSrcPos=SrcPos;
					SrcPos=pReplacePos[ReplaceCount]+ReplaceSrcLen;
					int CopySize=OldSrcPos-SrcPos;
					DestPos-=CopySize;
					if(SrcPos!=DestPos&&CopySize!=0)
						memmove(m_pBuffer+DestPos,m_pBuffer+SrcPos,CopySize*sizeof(T));
					SrcPos-=ReplaceSrcLen;
					DestPos-=ReplaceDestLen;
					if(ReplaceDestLen)
						memmove(m_pBuffer+DestPos,pNewStr,ReplaceDestLen*sizeof(T));
				}
			}
			else
			{
				pReplacePos[ReplaceCount]=(int)m_StringLength;
				int SrcPos=pReplacePos[0];
				int DestPos=pReplacePos[0];
				for(int i=0;i<ReplaceCount;i++)
				{
					if(ReplaceDestLen)
						memmove(m_pBuffer+DestPos,pNewStr,ReplaceDestLen*sizeof(T));
					SrcPos+=ReplaceSrcLen;
					DestPos+=ReplaceDestLen;

					int CopySize=pReplacePos[i+1]-SrcPos;
					if(SrcPos!=DestPos&&CopySize!=0)
						memmove(m_pBuffer+DestPos,m_pBuffer+SrcPos,CopySize*sizeof(T));
					SrcPos+=CopySize;
					DestPos+=CopySize;
				}
			}
			m_StringLength=NewStrLen;
			m_pBuffer[m_StringLength]=0;
			SAFE_DELETE_ARRAY(pReplacePos);
			m_HashCode = 0;
		}
	}
	void Replace(SIZE_TYPE Start,SIZE_TYPE Len,const T * pNewStr)
	{
		if(Start+Len<=m_StringLength&&Len)
		{
			SIZE_TYPE ReplaceDestLen=GetStrLen(pNewStr);
			SIZE_TYPE NewStrLen=m_StringLength+ReplaceDestLen-Len;
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);

			memmove(m_pBuffer + Start + ReplaceDestLen, m_pBuffer + Start + Len, (m_StringLength - Start - Len)*sizeof(T));
			memmove(m_pBuffer + Start, pNewStr, ReplaceDestLen*sizeof(T));

			m_StringLength=NewStrLen;
			m_pBuffer[m_StringLength]=0;
			m_HashCode = 0;
		}
	}
	void Replace(SIZE_TYPE Start,SIZE_TYPE Len,SIZE_TYPE NewLen,T NewChar)
	{
		if(Start+Len<=m_StringLength&&Len)
		{
			SIZE_TYPE NewStrLen=m_StringLength+NewLen-Len;
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);

			memmove(m_pBuffer+Start+NewLen,m_pBuffer+Start+Len,(m_StringLength-Start-Len)*sizeof(T));
			while(NewLen)
			{
				m_pBuffer[Start+NewLen-1]=NewChar;
			}

			m_StringLength=NewStrLen;
			m_pBuffer[m_StringLength]=0;
			m_HashCode = 0;
		}
	}
	void Delete(SIZE_TYPE StartPos,SIZE_TYPE Len)
	{
		if(StartPos<m_StringLength)
		{
			if(StartPos+Len>m_StringLength)
				Len=m_StringLength-StartPos;
			memmove(m_pBuffer+StartPos,m_pBuffer+StartPos+Len,(m_StringLength-StartPos-Len)*sizeof(T));
			m_StringLength-=Len;
			m_pBuffer[m_StringLength]=0;
			m_HashCode = 0;
		}
	}
	void Remove(T Char)
	{
		int Pos=0;
		while((Pos=Find(Char))>=0)
		{
			Delete(Pos,1);
		}
	}
	void Remove(const T * pStr)
	{
		SIZE_TYPE Len=GetStrLen(pStr);
		if(Len>0)
		{
			int Pos=0;
			while((Pos=Find(pStr))>=0)
			{
				Delete(Pos,Len);
			}
		}
	}
	void Insert(SIZE_TYPE Pos,T Char)
	{
		Insert(Pos,&Char,1);
	}
	void Insert(SIZE_TYPE Pos,const T * pStr,SIZE_TYPE StrLen=0)
	{

		if(Pos<=m_StringLength)
		{
			if(StrLen==0)
				StrLen=GetStrLen(pStr);
			SIZE_TYPE NewStrLen=m_StringLength+StrLen;
			if(NewStrLen>=m_BufferSize)
				Resize(NewStrLen);
			memmove(m_pBuffer+Pos+StrLen,m_pBuffer+Pos,(m_StringLength-Pos)*sizeof(T));
			memmove(m_pBuffer+Pos,pStr,StrLen*sizeof(T));
			m_StringLength+=StrLen;
			m_pBuffer[m_StringLength]=0;
			m_HashCode = 0;
		}
	}
protected:
	SIZE_TYPE GetStrLen(const char * pStr)
	{
		if(pStr)
			return strlen(pStr);
		else
			return 0;
	}
	SIZE_TYPE GetStrLen(const WCHAR * pStr)
	{
		if(pStr)
			return wcslen(pStr);
		else
			return 0;
	}
};

template<typename T>
int	CEasyStringT<T>::m_DefaultCodePage = CP_ACP;

template<typename T>
T CEasyStringT<T>::ZERO_CHAR = 0;

template<typename T>
T * CEasyStringT<T>::EMPTY_STR = &CEasyStringT<T>::ZERO_CHAR;



template<>
inline void CEasyStringT<char>::SetString(const char * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(Size+1>m_BufferSize)
			Resize(Size,false);
		if(pStr)
			memmove(m_pBuffer,pStr,Size);
		m_pBuffer[Size]=0;
		m_StringLength=Size;
		m_HashCode = 0;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<WCHAR>::SetString(const WCHAR * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(Size+1>m_BufferSize)
			Resize(Size,false);
		if(pStr)
			memmove(m_pBuffer,pStr,sizeof(WCHAR)*Size);
		m_pBuffer[Size]=0;
		m_StringLength=Size;
		m_HashCode = 0;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::SetString(const WCHAR * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE AnsiSize;
		if (m_DefaultCodePage == CP_UTF8)
			AnsiSize = UnicodeToUTF8(pStr, Size, NULL, 0);
		else
			AnsiSize=UnicodeToAnsi(pStr,Size,NULL,0);

		if(AnsiSize>=m_BufferSize)
			Resize(AnsiSize,false);
		if (pStr)
		{
			if (m_DefaultCodePage==CP_UTF8)
				UnicodeToUTF8(pStr, Size, m_pBuffer, AnsiSize);
			else
				UnicodeToAnsi(pStr, Size, m_pBuffer, AnsiSize);
		}
			
		m_pBuffer[AnsiSize]=0;
		m_StringLength=AnsiSize;
		m_HashCode = 0;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<WCHAR>::SetString(const char * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE UnicodeSize;
		if (m_DefaultCodePage == CP_UTF8)
			UnicodeSize = UTF8ToUnicode(pStr, Size, NULL, 0);
		else
			UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(UnicodeSize>=m_BufferSize)
			Resize(UnicodeSize,false);
		if (pStr)
		{
			if (m_DefaultCodePage == CP_UTF8)
				UTF8ToUnicode(pStr, Size, m_pBuffer, UnicodeSize);
			else
				AnsiToUnicode(pStr, Size, m_pBuffer, UnicodeSize);
		}
		m_pBuffer[UnicodeSize]=0;
		m_StringLength=UnicodeSize;
		m_HashCode = 0;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::AppendString(const char * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(m_StringLength+Size>=m_BufferSize)
			Resize(m_StringLength+Size);
		if(pStr)
			memmove(m_pBuffer+m_StringLength,pStr,Size);
		m_pBuffer[m_StringLength+Size]=0;
		m_StringLength+=Size;
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<WCHAR>::AppendString(const WCHAR * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(m_StringLength+Size>=m_BufferSize)
			Resize(m_StringLength+Size);
		if(pStr)
			memmove(m_pBuffer+m_StringLength,pStr,sizeof(WCHAR)*Size);
		m_pBuffer[m_StringLength+Size]=0;
		m_StringLength+=Size;
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<char>::AppendString(const WCHAR * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		SIZE_TYPE AnsiSize;
		if (m_DefaultCodePage == CP_UTF8)
			AnsiSize = UnicodeToUTF8(pStr, Size, NULL, 0);
		else
			AnsiSize = UnicodeToAnsi(pStr, Size, NULL, 0);
		if(m_StringLength+AnsiSize>=m_BufferSize)
			Resize(m_StringLength+AnsiSize);

		if (pStr)
		{
			if (m_DefaultCodePage == CP_UTF8)
				UnicodeToUTF8(pStr, Size, m_pBuffer + m_StringLength, AnsiSize);
			else
				UnicodeToAnsi(pStr, Size, m_pBuffer + m_StringLength, AnsiSize);
		}
			
		m_pBuffer[m_StringLength+AnsiSize]=0;
		m_StringLength+=AnsiSize;
		m_HashCode = 0;
	}
}


template<>
inline void CEasyStringT<WCHAR>::AppendString(const char * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE UnicodeSize;
		if (m_DefaultCodePage == CP_UTF8)
			UnicodeSize = UTF8ToUnicode(pStr, Size, NULL, 0);
		else
			UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(m_StringLength+UnicodeSize>=m_BufferSize)
			Resize(m_StringLength+UnicodeSize);

		if (pStr)
		{
			if (m_DefaultCodePage == CP_UTF8)
				UTF8ToUnicode(pStr, Size, m_pBuffer + m_StringLength, UnicodeSize);
			else
				AnsiToUnicode(pStr, Size, m_pBuffer + m_StringLength, UnicodeSize);
		}
		m_pBuffer[m_StringLength+UnicodeSize]=0;
		m_StringLength+=UnicodeSize;
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<char>::FormatVL(const char * pFormat,va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len=_vscprintf(pFormat,vl2);
	va_end(vl2);
	m_BufferSize = Len + 1;
	char * pNewBuffer = new char[m_BufferSize];		
	vsprintf_s(pNewBuffer, m_BufferSize, pFormat, vl);
	m_StringLength=Len;
	RELEASE_STR_BUFF(m_pBuffer);
	m_pBuffer = pNewBuffer;
	m_HashCode = 0;
}

template<>
inline void CEasyStringT<WCHAR>::FormatVL(const WCHAR * pFormat,va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len=_vscwprintf(pFormat,vl);
	va_end(vl2);
	m_BufferSize = Len + 1;
	WCHAR * pNewBuffer = new WCHAR[m_BufferSize];
	vswprintf_s(pNewBuffer, m_BufferSize, pFormat, vl);
	m_StringLength=Len;
	RELEASE_STR_BUFF(m_pBuffer);
	m_pBuffer = pNewBuffer;
	m_HashCode = 0;
}

template<>
inline void CEasyStringT<char>::Format(const char * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	FormatVL(pFormat,vl);
	va_end( vl);
}


template<>
inline void CEasyStringT<WCHAR>::Format(const WCHAR * pFormat,...)
{
	va_list	vl;
	va_start(vl,pFormat);
	FormatVL(pFormat,vl);
	va_end( vl);
}


template<>
inline void CEasyStringT<char>::MakeUpper()
{
	_strupr_s(m_pBuffer,m_BufferSize);
	m_HashCode = 0;
}

template<>
inline void CEasyStringT<WCHAR>::MakeUpper()
{
	_wcsupr_s(m_pBuffer,m_BufferSize);
	m_HashCode = 0;
}

template<>
inline void CEasyStringT<char>::MakeLower()
{
	_strlwr_s(m_pBuffer,m_BufferSize);
	m_HashCode = 0;
}

template<>
inline void CEasyStringT<WCHAR>::MakeLower()
{
	_wcslwr_s(m_pBuffer,m_BufferSize);
	m_HashCode = 0;
}

template<>
inline int CEasyStringT<char>::Find(const char * pDestStr, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const char * pResult;
	if (IgnoreCase)
		pResult = stristr(m_pBuffer + StartPos, pDestStr);
	else
		pResult = strstr(m_pBuffer + StartPos, pDestStr);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<WCHAR>::Find(const WCHAR * pDestStr, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const WCHAR * pResult;
	if (IgnoreCase)
		pResult = wcsistr(m_pBuffer + StartPos, pDestStr);
	else
		pResult = wcsstr(m_pBuffer + StartPos, pDestStr);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<char>::Find(char DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const char * pResult;
	if (IgnoreCase)
		pResult = strichr(m_pBuffer + StartPos, DestChar);
	else
		pResult = strchr(m_pBuffer + StartPos, DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<WCHAR>::Find(WCHAR DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const WCHAR * pResult;
	if (IgnoreCase)
		pResult = wcsichr(m_pBuffer + StartPos, DestChar);
	else
		pResult = wcschr(m_pBuffer + StartPos, DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<WCHAR>::Find(char DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	return Find((WCHAR)DestChar, StartPos, IgnoreCase);
}

template<>
inline int CEasyStringT<char>::ReverseFind(char DestChar, bool IgnoreCase) const
{
	const char * pResult;
	if (IgnoreCase)
		pResult = strrichr(m_pBuffer, DestChar);
	else
		pResult = strrchr(m_pBuffer, DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<WCHAR>::ReverseFind(WCHAR DestChar, bool IgnoreCase) const
{
	const WCHAR * pResult;
	if (IgnoreCase)
		pResult = wcsrichr(m_pBuffer, DestChar);
	else
		pResult = wcsrchr(m_pBuffer, DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}
template<>
inline int CEasyStringT<WCHAR>::ReverseFind(char DestChar, bool IgnoreCase) const
{
	return ReverseFind((WCHAR)DestChar);
}

typedef CEasyStringT<WCHAR> CEasyStringW;

typedef CEasyStringT<char> CEasyStringA;

#ifdef UNICODE

typedef CEasyStringT<WCHAR> CEasyString;

#else

typedef CEasyStringT<TCHAR> CEasyString;

#endif



