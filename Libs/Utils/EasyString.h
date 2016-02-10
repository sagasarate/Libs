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

inline int strncpy_0( wchar_t *strDest,size_t DestSize, const wchar_t *strSource, size_t count )
{
	if(strDest&&strSource)
	{
		int Ret=wcsncpy_s(strDest,DestSize,strSource,count-1);
		strDest[count-1]=0;
		return Ret;
	}
	return 0;
}

inline TCHAR *  strstri (
					   const TCHAR * str1,
					   const TCHAR * str2
					   )
{
	TCHAR *cp = (TCHAR *) str1;
	TCHAR *s1, *s2;

	if ( !*str2 )
		return((TCHAR *)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (TCHAR *) str2;

		while ( *s1 && *s2 && !(_totlower(*s1)-_totlower(*s2)) )
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}
	return(NULL);

}

inline int CompareString(const char * pStr1,const char * pStr2)
{
	return strcmp(pStr1,pStr2);
}
inline int CompareString(const wchar_t * pStr1,const wchar_t * pStr2)
{
	return wcscmp(pStr1,pStr2);
}
inline int CompareStringWithLen(const char * pStr1,const char * pStr2,size_t Len)
{
	return strncmp(pStr1,pStr2,Len);
}
inline int CompareStringWithLen(const wchar_t * pStr1,const wchar_t * pStr2,size_t Len)
{
	return wcsncmp(pStr1,pStr2,Len);
}
inline int CompareStringNoCase(const char * pStr1,const char * pStr2)
{
	return _stricmp(pStr1,pStr2);
}
inline int CompareStringNoCase(const wchar_t * pStr1,const wchar_t * pStr2)
{
	return _wcsicmp(pStr1,pStr2);
}
inline int CompareStringNoCaseWithLen(const char * pStr1,const char * pStr2,size_t Len)
{
	return _strnicmp(pStr1,pStr2,Len);
}
inline int CompareStringNoCaseWithLen(const wchar_t * pStr1,const wchar_t * pStr2,size_t Len)
{
	return _wcsnicmp(pStr1,pStr2,Len);
}

template <typename T>
inline T * StringTrimRightT(T * pStr,T TrimChar)
{
	char * pStrTail=pStr;
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
#define StringTrimLeftW	StringTrimLeftT<wchar_t>

#define StringTrimRight		StringTrimRightT<TCHAR>
#define StringTrimRightA	StringTrimRightT<char>
#define StringTrimRightW	StringTrimRightT<wchar_t>

#define StringTrim	StringTrimT<TCHAR>
#define StringTrimA	StringTrimT<char>
#define StringTrimW	StringTrimT<wchar_t>


template < typename T>
class CEasyStringT
{
public:
	typedef size_t SIZE_TYPE;
	enum
	{
		ERROR_CHAR_POS=-1,
	};
protected:
	T*			m_pBuffer;
	SIZE_TYPE	m_BufferSize;
	SIZE_TYPE	m_StringLength;
public:

	CEasyStringT()
	{
		m_pBuffer=new T[1];
		m_pBuffer[0]=0;
		m_BufferSize=1;
		m_StringLength=0;
	}
	CEasyStringT(const char * pStr)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const wchar_t * pStr)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,GetStrLen(pStr));
	}
	CEasyStringT(const char * pStr,SIZE_TYPE Size)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,Size);
	}
	CEasyStringT(const wchar_t * pStr,SIZE_TYPE Size)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(pStr,Size);
	}
	CEasyStringT(const char Char)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(&Char,1);
	}
	CEasyStringT(const wchar_t Char)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(&Char,1);
	}
	CEasyStringT(const CEasyStringT<char>& Str)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.GetBuffer(),Str.GetLength());
	}
	CEasyStringT(const CEasyStringT<wchar_t>& Str)
	{
		m_pBuffer=new T[1];
		m_BufferSize=1;
		m_StringLength=0;
		SetString(Str.GetBuffer(),Str.GetLength());
	}
	~CEasyStringT()
	{
		SAFE_DELETE_ARRAY(m_pBuffer);
	}
	void Clear()
	{
		Resize(0,false);
	}
	bool IsUnicode()
	{
		return sizeof(T)==sizeof(wchar_t);
	}
	void Resize(SIZE_TYPE Size,bool ReserveData=true)
	{
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
		SAFE_DELETE_ARRAY(m_pBuffer);
		m_pBuffer=pNewBuffer;
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
	void SetString(const char * pStr,SIZE_TYPE Size);
	void SetString(const wchar_t * pStr,SIZE_TYPE Size);
	void AppendString(const char * pStr,SIZE_TYPE Size);
	void AppendString(const wchar_t * pStr,SIZE_TYPE Size);
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
	operator T*() const
	{
		return m_pBuffer;
	}
	T& operator[](int Index) const
	{
		return m_pBuffer[Index];
	}
	void SetChar(int Index,T Char)
	{
		m_pBuffer[Index]=Char;
	}
	CEasyStringT<T>& operator=(const char* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(const wchar_t* pStr)
	{
		SetString(pStr,GetStrLen(pStr));
		return *this;
	}
	CEasyStringT<T>& operator=(char Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(wchar_t Char)
	{
		SetString(&Char,1);
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<char>& Str)
	{
		SetString(Str.GetBuffer(),Str.GetLength());
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<wchar_t>& Str)
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
	bool operator==(const T* pStr)
	{
		return Compare(pStr)==0;
	}
	bool operator!=(const T* pStr)
	{
		return Compare(pStr)!=0;
	}
	bool operator>(const T* pStr)
	{
		return Compare(pStr)>0;
	}
	bool operator<(const T* pStr)
	{
		return Compare(pStr)<0;
	}
	const CEasyStringT<T>& operator+=(const char * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(const wchar_t * pStr)
	{
		AppendString(pStr,GetStrLen(pStr));
		return *this;
	}
	const CEasyStringT<T>& operator+=(char Char)
	{
		AppendString(&Char,1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(wchar_t Char)
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
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,const wchar_t * pStr2)
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
	friend CEasyStringT<T> operator+(const  wchar_t * pStr1,const CEasyStringT<T>& Str2)
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
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1,wchar_t Char2)
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
	friend CEasyStringT<T> operator+(wchar_t Char1,const CEasyStringT<T>& Str2)
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
		if(Start<0)
			Start=0;
		if(Start>m_StringLength)
			Start=m_StringLength;
		if(Number<0)
			Number=m_StringLength-Start;
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
	void FormatVL(const wchar_t * pFormat,va_list vl);
	void Format(const char * pFormat,...);
	void Format(const wchar_t * pFormat,...);
	void MakeUpper();
	void MakeLower();
	int Find(const char * pDestStr,SIZE_TYPE StartPos=0);
	int Find(const wchar_t * pDestStr,SIZE_TYPE StartPos=0);
	int Find(char DestChar,SIZE_TYPE StartPos=0);
	int Find(wchar_t DestChar,SIZE_TYPE StartPos=0);
	int ReverseFind(char DestChar);
	int ReverseFind(wchar_t DestChar);
	void Replace(T OldChar,T NewChar)
	{
		if(OldChar!=NewChar)
		{
			for(SIZE_TYPE i=0;i<m_StringLength;i++)
			{
				if(m_pBuffer[i]==OldChar)
					m_pBuffer[i]=NewChar;
			}
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
	SIZE_TYPE GetStrLen(const wchar_t * pStr)
	{
		if(pStr)
			return wcslen(pStr);
		else
			return 0;
	}
};

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
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<wchar_t>::SetString(const wchar_t * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(Size+1>m_BufferSize)
			Resize(Size,false);
		if(pStr)
			memmove(m_pBuffer,pStr,sizeof(wchar_t)*Size);
		m_pBuffer[Size]=0;
		m_StringLength=Size;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::SetString(const wchar_t * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE AnsiSize=UnicodeToAnsi(pStr,Size,NULL,0);

		if(AnsiSize>=m_BufferSize)
			Resize(AnsiSize,false);
		if(pStr)
			UnicodeToAnsi(pStr,Size,m_pBuffer,AnsiSize);
		m_pBuffer[AnsiSize]=0;
		m_StringLength=AnsiSize;
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<wchar_t>::SetString(const char * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(UnicodeSize>=m_BufferSize)
			Resize(UnicodeSize,false);
		if(pStr)
			AnsiToUnicode(pStr,Size,m_pBuffer,UnicodeSize);
		m_pBuffer[UnicodeSize]=0;
		m_StringLength=UnicodeSize;
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
	}
}

template<>
inline void CEasyStringT<wchar_t>::AppendString(const wchar_t * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		if(m_StringLength+Size>=m_BufferSize)
			Resize(m_StringLength+Size);
		if(pStr)
			memmove(m_pBuffer+m_StringLength,pStr,sizeof(wchar_t)*Size);
		m_pBuffer[m_StringLength+Size]=0;
		m_StringLength+=Size;
	}
}

template<>
inline void CEasyStringT<char>::AppendString(const wchar_t * pStr,SIZE_TYPE Size)
{
	if(Size>0)
	{
		SIZE_TYPE AnsiSize=UnicodeToAnsi(pStr,Size,NULL,0);
		if(m_StringLength+AnsiSize>=m_BufferSize)
			Resize(m_StringLength+AnsiSize);

		if(pStr)
			UnicodeToAnsi(pStr,Size,m_pBuffer+m_StringLength,AnsiSize);
		m_pBuffer[m_StringLength+AnsiSize]=0;
		m_StringLength+=AnsiSize;
	}
}


template<>
inline void CEasyStringT<wchar_t>::AppendString(const char * pStr,SIZE_TYPE Size)
{
	if(pStr&&Size>0)
	{
		SIZE_TYPE UnicodeSize=AnsiToUnicode(pStr,Size,NULL,0);
		if(m_StringLength+UnicodeSize>=m_BufferSize)
			Resize(m_StringLength+UnicodeSize);

		if(pStr)
			AnsiToUnicode(pStr,Size,m_pBuffer+m_StringLength,UnicodeSize);
		m_pBuffer[m_StringLength+UnicodeSize]=0;
		m_StringLength+=UnicodeSize;
	}
}

template<>
inline void CEasyStringT<char>::FormatVL(const char * pFormat,va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len=_vscprintf(pFormat,vl2);
	va_end(vl2);
	Resize(Len,false);
	vsprintf_s(m_pBuffer,m_BufferSize,pFormat,vl);
	m_StringLength=Len;
}

template<>
inline void CEasyStringT<wchar_t>::FormatVL(const wchar_t * pFormat,va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len=_vscwprintf(pFormat,vl);
	va_end(vl2);
	Resize(Len,false);
	vswprintf_s(m_pBuffer,m_BufferSize,pFormat,vl);
	m_StringLength=Len;
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
inline void CEasyStringT<wchar_t>::Format(const wchar_t * pFormat,...)
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
}

template<>
inline void CEasyStringT<wchar_t>::MakeUpper()
{
	_wcsupr_s(m_pBuffer,m_BufferSize);
}

template<>
inline void CEasyStringT<char>::MakeLower()
{
	_strlwr_s(m_pBuffer,m_BufferSize);
}

template<>
inline void CEasyStringT<wchar_t>::MakeLower()
{
	_wcslwr_s(m_pBuffer,m_BufferSize);
}

template<>
inline int CEasyStringT<char>::Find(const char * pDestStr,SIZE_TYPE StartPos)
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const char * pResult=strstr(m_pBuffer+StartPos,pDestStr);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<wchar_t>::Find(const wchar_t * pDestStr,SIZE_TYPE StartPos)
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const wchar_t * pResult=wcsstr(m_pBuffer+StartPos,pDestStr);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<char>::Find(char DestChar,SIZE_TYPE StartPos)
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const char * pResult=strchr(m_pBuffer+StartPos,DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<wchar_t>::Find(wchar_t DestChar,SIZE_TYPE StartPos)
{
	if(StartPos>m_StringLength)
		StartPos=m_StringLength;
	const wchar_t * pResult=wcschr(m_pBuffer+StartPos,DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<wchar_t>::Find(char DestChar,SIZE_TYPE StartPos)
{
	return Find((wchar_t)DestChar,StartPos);
}

template<>
inline int CEasyStringT<char>::ReverseFind(char DestChar)
{
	const char * pResult=strrchr(m_pBuffer,DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}

template<>
inline int CEasyStringT<wchar_t>::ReverseFind(wchar_t DestChar)
{
	const wchar_t * pResult=wcsrchr(m_pBuffer,DestChar);
	if(pResult==NULL)
		return ERROR_CHAR_POS;
	else
		return (int)(pResult-m_pBuffer);
}
template<>
inline int CEasyStringT<wchar_t>::ReverseFind(char DestChar)
{
	return ReverseFind((wchar_t)DestChar);
}

typedef CEasyStringT<wchar_t> CEasyStringW;

typedef CEasyStringT<char> CEasyStringA;

#ifdef UNICODE

typedef CEasyStringT<wchar_t> CEasyString;

#else

typedef CEasyStringT<TCHAR> CEasyString;

#endif



