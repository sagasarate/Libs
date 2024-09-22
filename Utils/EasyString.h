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

enum BOM_HEADER_TYPE
{
	BMT_UNICODE = 0xFEFF,
	BMT_UNICODE_BIG_ENDIAN = 0xFFFE,
	BMT_UTF_8 = 0xBFBBEF,
};

inline int strncpy_0(char* strDest, size_t DestSize, const char* strSource, size_t count)
{
	if (strDest && strSource)
	{
		int Ret = strncpy_s(strDest, DestSize, strSource, count - 1);
		strDest[count - 1] = 0;
		return Ret;
	}
	return 0;
}

inline int strncpy_0(WCHAR* strDest, size_t DestSize, const WCHAR* strSource, size_t count)
{
	if (strDest && strSource)
	{
		int Ret = wcsncpy_s(strDest, DestSize, strSource, count - 1);
		strDest[count - 1] = 0;
		return Ret;
	}
	return 0;
}

inline char* stristr(
	const char* str1,
	const char* str2
)
{
	char* cp = (char*)str1;
	char* s1, * s2;

	if (!*str2)
		return((char*)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (char*)str2;

		while (*s1 && *s2 && !(tolower(*s1) - tolower(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}
	return(NULL);

}

inline WCHAR* wcsistr(
	const WCHAR* str1,
	const WCHAR* str2
)
{
	WCHAR* cp = (WCHAR*)str1;
	WCHAR* s1, * s2;

	if (!*str2)
		return((WCHAR*)str1);

	while (*cp)
	{
		s1 = cp;
		s2 = (WCHAR*)str2;

		while (*s1 && *s2 && !(towlower(*s1) - towlower(*s2)))
			s1++, s2++;

		if (!*s2)
			return(cp);

		cp++;
	}
	return(NULL);

}

inline char* strichr(const char* str, char chr)
{
	while (*str && tolower(*str) != tolower(chr))
		str++;

	if (*str != 0)
		return((char*)str);

	return(NULL);
}

inline WCHAR* wcsichr(const WCHAR* str, WCHAR chr)
{
	while (*str && towlower(*str) != towlower(chr))
		str++;

	if (*str != 0)
		return((WCHAR*)str);

	return(NULL);
}

inline char* strrichr(const char* str, int chr)
{
	char* start = (char*)str;
	while (*str++);                       /* find end of string */
	/* search towards front */
	while (--str != start && tolower(*str) != tolower(chr));

	if (str != start)               /* char found ? */
		return((char*)str);
	return(NULL);
}

inline WCHAR* wcsrichr(const WCHAR* str, int chr)
{
	WCHAR* start = (WCHAR*)str;
	while (*str++);                       /* find end of string */
	/* search towards front */
	while (--str != start && towlower(*str) != towlower(chr));

	if (str != start)               /* char found ? */
		return((WCHAR*)str);
	return(NULL);
}


inline int CompareString(const char* pStr1, const char* pStr2)
{
	return strcmp(pStr1, pStr2);
}
inline int CompareString(const WCHAR* pStr1, const WCHAR* pStr2)
{
	return wcscmp(pStr1, pStr2);
}
inline int CompareStringWithLen(const char* pStr1, const char* pStr2, size_t Len)
{
	return strncmp(pStr1, pStr2, Len);
}
inline int CompareStringWithLen(const WCHAR* pStr1, const WCHAR* pStr2, size_t Len)
{
	return wcsncmp(pStr1, pStr2, Len);
}
inline int CompareStringNoCase(const char* pStr1, const char* pStr2)
{
	return _stricmp(pStr1, pStr2);
}
inline int CompareStringNoCase(const WCHAR* pStr1, const WCHAR* pStr2)
{
	return _wcsicmp(pStr1, pStr2);
}
inline int CompareStringNoCaseWithLen(const char* pStr1, const char* pStr2, size_t Len)
{
	return _strnicmp(pStr1, pStr2, Len);
}
inline int CompareStringNoCaseWithLen(const WCHAR* pStr1, const WCHAR* pStr2, size_t Len)
{
	return _wcsnicmp(pStr1, pStr2, Len);
}

template <typename T>
inline T* StringTrimRightT(T* pStr, T TrimChar)
{
	T* pStrTail = pStr;
	while (*pStrTail)
	{
		pStrTail++;
	}
	if (pStrTail == pStr)
		return pStr;
	pStrTail--;
	while (pStrTail != pStr && (*pStrTail == TrimChar))
	{
		*pStrTail = 0;
		pStrTail--;
	}
	return pStr;
}

template < typename T>
inline T* StringTrimLeftT(T* pStr, T TrimChar)
{
	T* ResultStr = pStr;
	while (*ResultStr == TrimChar)
	{
		ResultStr++;
	}
	return ResultStr;
}

template < typename T>
inline T* StringTrimT(T* pStr, T TrimChar)
{
	T* ResultStr = pStr;
	while (*ResultStr == TrimChar)
	{
		ResultStr++;
	}
	pStr = ResultStr;


	T* pStrTail = pStr;
	while (*pStrTail)
	{
		pStrTail++;
	}
	if (pStrTail == pStr)
		return pStr;
	pStrTail--;
	while (pStrTail != pStr && (*pStrTail == TrimChar))
	{
		*pStrTail = 0;
		pStrTail--;
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


inline size_t AnsiToUnicode(const char* SrcStr, size_t SrcLen, WCHAR* DestStr, size_t DestLen)
{
	return iconv_gbk2ucs2(SrcStr, SrcLen, DestStr, DestLen);
}

inline size_t UnicodeToAnsi(const WCHAR* SrcStr, size_t SrcLen, char* DestStr, size_t DestLen)
{
	return iconv_ucs22gbk(SrcStr, SrcLen, DestStr, DestLen);
}

inline size_t UTF8ToUnicode(const char* SrcStr, size_t SrcLen, WCHAR* DestStr, size_t DestLen)
{
	return iconv_utf82ucs2(SrcStr, SrcLen, DestStr, DestLen);
}

inline size_t UnicodeToUTF8(const WCHAR* SrcStr, size_t SrcLen, char* DestStr, size_t DestLen)
{
	return iconv_ucs22utf8(SrcStr, SrcLen, DestStr, DestLen);
}

inline size_t AnsiToUTF8(const char* SrcStr, size_t SrcLen, char* DestStr, size_t DestLen)
{
	return iconv_gbk2utf8(SrcStr, SrcLen, DestStr, DestLen);
}

inline size_t UTF8ToAnsi(const char* SrcStr, size_t SrcLen, char* DestStr, size_t DestLen)
{
	return iconv_utf82gbk(SrcStr, SrcLen, DestStr, DestLen);
}


template < typename T>
class CEasyStringT
{
public:
	typedef size_t SIZE_TYPE;
	static const size_t INVALID_POS = (size_t)(-1);
	enum STRING_CODE_PAGE
	{
		STRING_CODE_PAGE_AUTO = -1,
		STRING_CODE_PAGE_ANSI = 0,
		STRING_CODE_PAGE_UTF8 = 65001,
		STRING_CODE_PAGE_UCS16 = 1200,
	};
	static T	ZERO_CHAR;
	static const CEasyStringT EMPTY_STR;
	static T* EMPTY_PSTR;
	static STRING_CODE_PAGE SYSTEM_STRING_CODE_PAGE;
protected:
	T* m_pBuffer;
	SIZE_TYPE	m_BufferSize;
	SIZE_TYPE	m_StringLength;
	UINT		m_HashCode;
public:
	CEasyStringT()
	{
		m_pBuffer = CEasyStringT<T>::EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
	}
	CEasyStringT(const char* pStr, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(pStr, 0, CodePage);
	}
	CEasyStringT(const WCHAR* pStr, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(pStr, 0, CodePage);
	}
	CEasyStringT(const char* pStr, SIZE_TYPE Size, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(pStr, Size, CodePage);
	}
	CEasyStringT(const WCHAR* pStr, SIZE_TYPE Size, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(pStr, Size, CodePage);
	}
	CEasyStringT(const char Char)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(&Char, 1);
	}
	CEasyStringT(const WCHAR Char)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		m_HashCode = 0;
		SetString(&Char, 1);
	}
	CEasyStringT(const CEasyStringT<char>& Str)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		SetString(Str.GetBuffer(), Str.GetLength());
	}
	CEasyStringT(const CEasyStringT<WCHAR>& Str)
	{
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
		m_StringLength = 0;
		SetString(Str.GetBuffer(), Str.GetLength());
	}
	CEasyStringT(CEasyStringT&& Str)  noexcept
	{
		m_pBuffer = Str.m_pBuffer;
		m_BufferSize = Str.m_BufferSize;
		m_StringLength = Str.m_StringLength;
		m_HashCode = Str.m_HashCode;
		Str.m_pBuffer = EMPTY_PSTR;
		Str.m_BufferSize = 1;
		Str.m_StringLength = 0;
		Str.m_HashCode = 0;
	}
	~CEasyStringT()
	{
		ReleaseBuffer();
	}
	void ReleaseBuffer()
	{
		if (m_pBuffer && m_BufferSize && m_pBuffer != EMPTY_PSTR)
		{
			MONITORED_DELETE_ARRAY(m_pBuffer);
		}
		m_pBuffer = EMPTY_PSTR;
		m_BufferSize = 1;
	}
	void Clear()
	{
		ReleaseBuffer();
		m_StringLength = 0;
		m_HashCode = 0;
	}
	void Empty()
	{
		m_StringLength = 0;
		m_pBuffer[0] = 0;
		m_HashCode = 0;
	}
	bool IsUnicode()
	{
		return sizeof(T) == sizeof(WCHAR);
	}
	void Resize(SIZE_TYPE Size, bool ReserveData = true)
	{
		if (Size == m_StringLength)
			return;
		T* pNewBuffer = MONITORED_NEW_ARRAY(_T("CEasyStringT"), T, Size + 1);
		SIZE_TYPE NewStringLen = 0;
		if (m_pBuffer && ReserveData)
		{
			if (m_StringLength > Size)
				NewStringLen = Size;
			else
				NewStringLen = m_StringLength;
			memcpy(pNewBuffer, m_pBuffer, sizeof(T) * NewStringLen);
		}
		pNewBuffer[NewStringLen] = 0;
		pNewBuffer[Size] = 0;
		m_StringLength = NewStringLen;
		ReleaseBuffer();
		m_pBuffer = pNewBuffer;
		m_BufferSize = Size + 1;
		m_HashCode = 0;
	}
	bool MakeSelfBuffer()
	{
		if (m_pBuffer && (m_pBuffer != EMPTY_PSTR) && (m_BufferSize == 0) && m_StringLength)
		{
			m_BufferSize = m_StringLength + 1;
			T* pNewBuffer = MONITORED_NEW_ARRAY(_T("CEasyStringT"), T, m_BufferSize);
			memcpy(pNewBuffer, m_pBuffer, sizeof(T) * m_StringLength);
			pNewBuffer[m_StringLength] = 0;
			m_pBuffer = pNewBuffer;
			return true;
		}
		return false;
	}
	void TrimBuffer(SIZE_TYPE Size = 0)
	{
		if (m_pBuffer == EMPTY_PSTR)
			return;
		MakeSelfBuffer();
		if (Size <= 0)
		{
			Size = m_BufferSize - 1;
			for (SIZE_TYPE i = 0; i < m_BufferSize; i++)
			{
				if (m_pBuffer[i] == 0)
				{
					Size = i;
					break;
				}
			}
		}
		else if (Size >= m_BufferSize)
		{
			Size = m_BufferSize - 1;
		}
		m_pBuffer[Size] = 0;
		m_StringLength = Size;
		Resize(m_StringLength);
	}
	UINT GetHashCode() const
	{
		if (m_HashCode == 0)
		{
			UINT* pHashCode = (UINT*)(&m_HashCode);
			*pHashCode = 1;
			for (SIZE_TYPE i = 0; i < m_StringLength; i++)
			{
				*pHashCode = (*pHashCode) * 31 + m_pBuffer[i];
			}
		}
		return m_HashCode;
	}
	void SetStringRef(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	void SetStringRef(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	void SetString(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	void SetString(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	bool Append(const char* pStr, SIZE_TYPE StrLen = 0, bool AutoExpandBuffer = true, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	bool Append(const WCHAR* pStr, SIZE_TYPE StrLen = 0, bool AutoExpandBuffer = true, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO);
	SIZE_TYPE GetStringBytes(BYTE* pBuffer, SIZE_TYPE BufferSize, STRING_CODE_PAGE CodePage = STRING_CODE_PAGE_AUTO) const;
	SIZE_TYPE GetLength() const
	{
		return m_StringLength;
	}
	void SetLength(SIZE_TYPE Len)
	{
		if (Len < m_BufferSize)
		{
			MakeSelfBuffer();
			m_StringLength = Len;
			m_pBuffer[m_StringLength] = 0;
		}
	}
	SIZE_TYPE GetBufferSize() const
	{
		return m_BufferSize;
	}
	bool IsEmpty() const
	{
		return m_StringLength == 0;
	}
	T* GetBuffer()
	{
		return m_pBuffer;
	}
	const T* GetBuffer() const
	{
		return m_pBuffer;
	}
	operator const T* () const
	{
		return m_pBuffer;
	}
	operator T* ()
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
		m_pBuffer[Index] = Char;
		m_HashCode = 0;
	}
	CEasyStringT<T>& operator=(const char* pStr)
	{
		SetString(pStr, 0);
		return *this;
	}
	CEasyStringT<T>& operator=(const WCHAR* pStr)
	{
		SetString(pStr, 0);
		return *this;
	}
	CEasyStringT<T>& operator=(char Char)
	{
		SetString(&Char, 1);
		return *this;
	}
	CEasyStringT<T>& operator=(WCHAR Char)
	{
		SetString(&Char, 1);
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<char>& Str)
	{
		SetString(Str.GetBuffer(), Str.GetLength());
		return *this;
	}
	CEasyStringT<T>& operator=(const CEasyStringT<WCHAR>& Str)
	{
		SetString(Str.GetBuffer(), Str.GetLength());
		return *this;
	}
	CEasyStringT& operator=(CEasyStringT&& Str)  noexcept
	{
		if (m_pBuffer == Str.m_pBuffer)
			return *this;
		ReleaseBuffer();
		m_pBuffer = Str.m_pBuffer;
		m_BufferSize = Str.m_BufferSize;
		m_StringLength = Str.m_StringLength;
		m_HashCode = Str.m_HashCode;
		Str.m_pBuffer = EMPTY_PSTR;
		Str.m_BufferSize = 1;
		Str.m_StringLength = 0;
		Str.m_HashCode = 0;
		return *this;
	}
	int Compare(const T* pStr) const
	{
		if (m_pBuffer)
			return CompareString(m_pBuffer, pStr);
		else
			return -1;
	}
	int Compare(SIZE_TYPE Start, SIZE_TYPE Len, const T* pStr) const
	{
		if (m_pBuffer && Start + Len <= m_StringLength)
			return CompareStringWithLen(m_pBuffer + Start, pStr, Len);
		else
			return -1;
	}
	int CompareNoCase(const T* pStr) const
	{
		if (m_pBuffer)
			return CompareStringNoCase(m_pBuffer, pStr);
		else
			return -1;
	}
	int CompareNoCase(SIZE_TYPE Start, SIZE_TYPE Len, const T* pStr) const
	{
		if (m_pBuffer)
			return CompareStringNoCaseWithLen(m_pBuffer + Start, pStr, Len);
		else
			return -1;
	}


	friend bool operator==(const T* pStr1, const CEasyStringT& Str2)
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


	const CEasyStringT<T>& operator+=(const char* pStr)
	{
		Append(pStr);
		return *this;
	}
	const CEasyStringT<T>& operator+=(const WCHAR* pStr)
	{
		Append(pStr);
		return *this;
	}
	const CEasyStringT<T>& operator+=(char Char)
	{
		Append(&Char, 1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(WCHAR Char)
	{
		Append(&Char, 1);
		return *this;
	}
	const CEasyStringT<T>& operator+=(const CEasyStringT<T>& Str)
	{
		Append(Str.m_pBuffer, Str.m_StringLength);
		return *this;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1, const char* pStr2)
	{
		CEasyStringT<T> String;
		String = Str1;
		String += pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1, const WCHAR* pStr2)
	{
		CEasyStringT<T> String;
		String = Str1;
		String += pStr2;
		return String;
	}
	friend CEasyStringT<T> operator+(const  char* pStr1, const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String = pStr1;
		String.Append(Str2.m_pBuffer, Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const  WCHAR* pStr1, const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String = pStr1;
		String.Append(Str2.m_pBuffer, Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1, const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String.SetString(Str1.m_pBuffer, Str1.m_StringLength);
		String.Append(Str2.m_pBuffer, Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1, char Char2)
	{
		CEasyStringT<T> String;
		String = Str1;
		String += Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(const CEasyStringT<T>& Str1, WCHAR Char2)
	{
		CEasyStringT<T> String;
		String = Str1;
		String += Char2;
		return String;
	}
	friend CEasyStringT<T> operator+(char Char1, const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String = Char1;
		String.Append(Str2.m_pBuffer, Str2.m_StringLength);
		return String;
	}
	friend CEasyStringT<T> operator+(WCHAR Char1, const CEasyStringT<T>& Str2)
	{
		CEasyStringT<T> String;
		String = Char1;
		String.Append(Str2.m_pBuffer, Str2.m_StringLength);
		return String;
	}
	void TrimLeft(T TrimChar = ' ')
	{
		T* pTrimedStr = StringTrimLeftT<T>(m_pBuffer, TrimChar);
		SetString(pTrimedStr, 0);
	}
	void TrimRight(T TrimChar = ' ')
	{
		T* pTrimedStr = StringTrimRightT<T>(m_pBuffer, TrimChar);
		SetString(pTrimedStr, 0);
	}
	void Trim(T TrimChar = ' ')
	{
		T* pTrimedStr = StringTrimT<T>(m_pBuffer, TrimChar);
		SetString(pTrimedStr, 0);
	}
	CEasyStringT<T> SubStr(SIZE_TYPE Start, SIZE_TYPE Number) const
	{
		if (Start > m_StringLength)
			Start = m_StringLength;
		if (Start + Number > m_StringLength)
			Number = m_StringLength - Start;
		return CEasyStringT<T>(m_pBuffer + Start, Number);
	}
	CEasyStringT<T> Left(SIZE_TYPE Number) const
	{
		if (Number > m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer, Number);
	}
	CEasyStringT<T> Right(SIZE_TYPE Number) const
	{
		if (Number > m_StringLength)
			return *this;
		return CEasyStringT<T>(m_pBuffer + m_StringLength - Number, Number);
	}
	static SIZE_TYPE GetFormatLenVL(const char* pFormat, va_list vl);
	static SIZE_TYPE GetFormatLenVL(const WCHAR* pFormat, va_list vl);
	static SIZE_TYPE GetFormatLen(const char* pFormat, ...);
	static SIZE_TYPE GetFormatLen(const WCHAR* pFormat, ...);

	void FormatVL(const char* pFormat, va_list vl);
	void FormatVL(const WCHAR* pFormat, va_list vl);
	void Format(const char* pFormat, ...);
	void Format(const WCHAR* pFormat, ...);

	void AppendFormatVL(const char* pFormat, va_list vl);
	void AppendFormatVL(const WCHAR* pFormat, va_list vl);
	void AppendFormat(const char* pFormat, ...);
	void AppendFormat(const WCHAR* pFormat, ...);

	bool AppendFormatNoExpandVL(const char* pFormat, va_list vl);
	bool AppendFormatNoExpandVL(const WCHAR* pFormat, va_list vl);
	bool AppendFormatNoExpand(const char* pFormat, ...);
	bool AppendFormatNoExpand(const WCHAR* pFormat, ...);

	void MakeUpper();
	void MakeLower();
	SIZE_TYPE Find(const char* pDestStr, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	SIZE_TYPE Find(const WCHAR* pDestStr, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	SIZE_TYPE Find(char DestChar, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	SIZE_TYPE Find(WCHAR DestChar, SIZE_TYPE StartPos = 0, bool IgnoreCase = false) const;
	SIZE_TYPE ReverseFind(char DestChar, bool IgnoreCase = false) const;
	SIZE_TYPE ReverseFind(WCHAR DestChar, bool IgnoreCase = false) const;
	void Replace(T OldChar, T NewChar)
	{
		if (OldChar != NewChar)
		{
			MakeSelfBuffer();
			for (SIZE_TYPE i = 0; i < m_StringLength; i++)
			{
				if (m_pBuffer[i] == OldChar)
					m_pBuffer[i] = NewChar;
			}
			m_HashCode = 0;
		}
	}
	void Replace(const T* pOldStr, const T* pNewStr)
	{
		if (CompareString(pOldStr, pNewStr) != 0)
		{
			SIZE_TYPE StartPos = 0;
			SIZE_TYPE ReplaceCount = 0;
			SIZE_TYPE ReplaceSrcLen = GetStrLen(pOldStr);
			SIZE_TYPE ReplaceDestLen = GetStrLen(pNewStr);
			SIZE_TYPE* pReplacePos = MONITORED_NEW_ARRAY(_T("CEasyStringT"), SIZE_TYPE, m_StringLength / ReplaceSrcLen + 2);
			SIZE_TYPE FindPos = Find(pOldStr, StartPos);
			while (FindPos != INVALID_POS)
			{
				pReplacePos[ReplaceCount] = FindPos;
				ReplaceCount++;
				StartPos = FindPos + ReplaceSrcLen;
				FindPos = Find(pOldStr, StartPos);
			}


			SIZE_TYPE NewStrLen = m_StringLength + ReplaceCount * ReplaceDestLen - ReplaceCount * ReplaceSrcLen;
			if (NewStrLen >= m_BufferSize)
				Resize(NewStrLen);
			else
				MakeSelfBuffer();

			if (NewStrLen >= m_StringLength)
			{
				SIZE_TYPE SrcPos = m_StringLength;
				SIZE_TYPE DestPos = NewStrLen;
				while (ReplaceCount)
				{
					ReplaceCount--;
					SIZE_TYPE OldSrcPos = SrcPos;
					SrcPos = pReplacePos[ReplaceCount] + ReplaceSrcLen;
					SIZE_TYPE CopySize = OldSrcPos - SrcPos;
					DestPos -= CopySize;
					if (SrcPos != DestPos && CopySize != 0)
						memmove(m_pBuffer + DestPos, m_pBuffer + SrcPos, CopySize * sizeof(T));
					SrcPos -= ReplaceSrcLen;
					DestPos -= ReplaceDestLen;
					if (ReplaceDestLen)
						memmove(m_pBuffer + DestPos, pNewStr, ReplaceDestLen * sizeof(T));
				}
			}
			else
			{
				pReplacePos[ReplaceCount] = m_StringLength;
				SIZE_TYPE SrcPos = pReplacePos[0];
				SIZE_TYPE DestPos = pReplacePos[0];
				for (SIZE_TYPE i = 0; i < ReplaceCount; i++)
				{
					if (ReplaceDestLen)
						memmove(m_pBuffer + DestPos, pNewStr, ReplaceDestLen * sizeof(T));
					SrcPos += ReplaceSrcLen;
					DestPos += ReplaceDestLen;

					SIZE_TYPE CopySize = pReplacePos[i + 1] - SrcPos;
					if (SrcPos != DestPos && CopySize != 0)
						memmove(m_pBuffer + DestPos, m_pBuffer + SrcPos, CopySize * sizeof(T));
					SrcPos += CopySize;
					DestPos += CopySize;
				}
			}
			m_StringLength = NewStrLen;
			m_pBuffer[m_StringLength] = 0;
			SAFE_DELETE_ARRAY(pReplacePos);
			m_HashCode = 0;
		}
	}
	void Replace(SIZE_TYPE Start, SIZE_TYPE Len, const T* pNewStr)
	{
		if (Start + Len <= m_StringLength && Len)
		{
			SIZE_TYPE ReplaceDestLen = GetStrLen(pNewStr);
			SIZE_TYPE NewStrLen = m_StringLength + ReplaceDestLen - Len;
			if (NewStrLen >= m_BufferSize)
				Resize(NewStrLen);
			else
				MakeSelfBuffer();

			memmove(m_pBuffer + Start + ReplaceDestLen, m_pBuffer + Start + Len, (m_StringLength - Start - Len) * sizeof(T));
			memmove(m_pBuffer + Start, pNewStr, ReplaceDestLen * sizeof(T));

			m_StringLength = NewStrLen;
			m_pBuffer[m_StringLength] = 0;
			m_HashCode = 0;
		}
	}
	void Replace(SIZE_TYPE Start, SIZE_TYPE Len, SIZE_TYPE NewLen, T NewChar)
	{
		if (Start + Len <= m_StringLength && Len)
		{
			SIZE_TYPE NewStrLen = m_StringLength + NewLen - Len;
			if (NewStrLen >= m_BufferSize)
				Resize(NewStrLen);
			else
				MakeSelfBuffer();

			memmove(m_pBuffer + Start + NewLen, m_pBuffer + Start + Len, (m_StringLength - Start - Len) * sizeof(T));
			while (NewLen)
			{
				m_pBuffer[Start + NewLen - 1] = NewChar;
			}

			m_StringLength = NewStrLen;
			m_pBuffer[m_StringLength] = 0;
			m_HashCode = 0;
		}
	}
	void Delete(SIZE_TYPE StartPos, SIZE_TYPE Len)
	{
		if (StartPos < m_StringLength)
		{
			MakeSelfBuffer();
			if (StartPos + Len > m_StringLength)
				Len = m_StringLength - StartPos;
			memmove(m_pBuffer + StartPos, m_pBuffer + StartPos + Len, (m_StringLength - StartPos - Len) * sizeof(T));
			m_StringLength -= Len;
			m_pBuffer[m_StringLength] = 0;
			m_HashCode = 0;
		}
	}
	void Remove(T Char)
	{
		SIZE_TYPE Pos = 0;
		while ((Pos = Find(Char)) != INVALID_POS)
		{
			Delete(Pos, 1);
		}
	}
	void Remove(const T* pStr)
	{
		SIZE_TYPE Len = GetStrLen(pStr);
		if (Len > 0)
		{
			SIZE_TYPE Pos = 0;
			while ((Pos = Find(pStr)) != INVALID_POS)
			{
				Delete(Pos, Len);
			}
		}
	}
	void Insert(SIZE_TYPE Pos, T Char)
	{
		Insert(Pos, &Char, 1);
	}
	void Insert(SIZE_TYPE Pos, const T* pStr, SIZE_TYPE StrLen = 0)
	{
		if (Pos <= m_StringLength)
		{
			if (StrLen == 0)
				StrLen = GetStrLen(pStr);
			if (StrLen <= 0)
				return;
			SIZE_TYPE NewStrLen = m_StringLength + StrLen;
			if (NewStrLen >= m_BufferSize)
				Resize(NewStrLen);
			else
				MakeSelfBuffer();
			memmove(m_pBuffer + Pos + StrLen, m_pBuffer + Pos, (m_StringLength - Pos) * sizeof(T));
			memmove(m_pBuffer + Pos, pStr, StrLen * sizeof(T));
			m_StringLength += StrLen;
			m_pBuffer[m_StringLength] = 0;
			m_HashCode = 0;
		}
	}
	bool StartWith(const char* szStr);
	bool StartWith(const WCHAR* szStr);
	bool EndWith(const char* szStr);
	bool EndWith(const WCHAR* szStr);
	static int GetSystemCodePageID()
	{
		switch (SYSTEM_STRING_CODE_PAGE)
		{
		case STRING_CODE_PAGE_UTF8:
			return CP_UTF8;
		case STRING_CODE_PAGE_UCS16:
			return CP_UNICODE;
		default:
			return CP_ACP;
		}
	}

	static SIZE_TYPE GetStrLen(const char* pStr)
	{
		if (pStr)
			return strlen(pStr);
		else
			return 0;
	}
	static SIZE_TYPE GetStrLen(const WCHAR* pStr)
	{
		if (pStr)
			return wcslen(pStr);
		else
			return 0;
	}
	inline static CEasyStringT Join(CEasyArray<CEasyStringT>& StrList, T Separator)
	{
		CEasyStringT ResultStr;
		UINT Len = 0;
		for (UINT i = 0; i < StrList.GetCount(); i++)
		{
			Len += (UINT)StrList[i].GetLength();
		}
		if (Len)
		{
			Len += (UINT)StrList.GetCount() - 1;
			ResultStr.Resize(Len);
			ResultStr.SetLength(Len);
			UINT BuffPtr = 0;
			T* pBuff = (T*)ResultStr;
			for (UINT i = 0; i < StrList.GetCount(); i++)
			{
				_tcscpy_s(pBuff + BuffPtr, Len - BuffPtr + 1, StrList[i]);
				BuffPtr += (UINT)StrList[i].GetLength();
				if (i < StrList.GetCount() - 1)
				{
					pBuff[BuffPtr] = Separator;
					BuffPtr++;
				}
			}
		}
		return ResultStr;
	}
};

template<typename T>
T CEasyStringT<T>::ZERO_CHAR = 0;

template<typename T>
T* CEasyStringT<T>::EMPTY_PSTR = &CEasyStringT<T>::ZERO_CHAR;

template<typename T>
const CEasyStringT<T> CEasyStringT<T>::EMPTY_STR;

template<typename T>
#ifdef WIN32
#ifdef UNICODE
typename CEasyStringT<T>::STRING_CODE_PAGE CEasyStringT<T>::SYSTEM_STRING_CODE_PAGE = CEasyStringT<T>::STRING_CODE_PAGE_UCS16;
#else
typename CEasyStringT<T>::STRING_CODE_PAGE CEasyStringT<T>::SYSTEM_STRING_CODE_PAGE = CEasyStringT<T>::STRING_CODE_PAGE_ANSI;
#endif
#else
typename CEasyStringT<T>::STRING_CODE_PAGE CEasyStringT<T>::SYSTEM_STRING_CODE_PAGE = CEasyStringT<T>::STRING_CODE_PAGE_UTF8;
#endif



template<>
inline void CEasyStringT<char>::SetString(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	if (pStr)
	{
		if (StrLen == 0)
			StrLen = GetStrLen(pStr);
		if (StrLen > 0)
		{
			if ((CodePage == STRING_CODE_PAGE_AUTO) || (CodePage == SYSTEM_STRING_CODE_PAGE))
			{
				if (StrLen + 1 > m_BufferSize)
					Resize(StrLen, false);
				else
					MakeSelfBuffer();
				memmove(m_pBuffer, pStr, StrLen);
				m_pBuffer[StrLen] = 0;
				m_StringLength = StrLen;
				m_HashCode = 0;
			}
			else if (CodePage == STRING_CODE_PAGE_ANSI)
			{
				SIZE_TYPE DestSize = AnsiToUTF8(pStr, StrLen, NULL, 0);
				if (DestSize + 1 > m_BufferSize)
					Resize(DestSize, false);
				else
					MakeSelfBuffer();
				AnsiToUTF8(pStr, StrLen, m_pBuffer, DestSize);
				m_pBuffer[DestSize] = 0;
				m_StringLength = DestSize;
				m_HashCode = 0;
			}
			else if (CodePage == STRING_CODE_PAGE_UTF8)
			{
				SIZE_TYPE DestSize = UTF8ToAnsi(pStr, StrLen, NULL, 0);
				if (DestSize + 1 > m_BufferSize)
					Resize(DestSize, false);
				else
					MakeSelfBuffer();
				UTF8ToAnsi(pStr, StrLen, m_pBuffer, DestSize);
				m_pBuffer[DestSize] = 0;
				m_StringLength = DestSize;
				m_HashCode = 0;
			}
			else
			{
				Clear();
			}
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<WCHAR>::SetString(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	if (pStr)
	{
		if (StrLen == 0)
			StrLen = GetStrLen(pStr);
		if (StrLen > 0)
		{
			if (StrLen + 1 > m_BufferSize)
				Resize(StrLen, false);
			else
				MakeSelfBuffer();
			memmove(m_pBuffer, pStr, sizeof(WCHAR) * StrLen);
			m_pBuffer[StrLen] = 0;
			m_StringLength = StrLen;
			m_HashCode = 0;
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::SetString(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	if (pStr)
	{
		if (StrLen == 0)
			StrLen = GetStrLen(pStr);
		if (StrLen > 0)
		{
			SIZE_TYPE AnsiSize;
			if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_UTF8)
				AnsiSize = UnicodeToUTF8(pStr, StrLen, NULL, 0);
			else
				AnsiSize = UnicodeToAnsi(pStr, StrLen, NULL, 0);

			if (AnsiSize >= m_BufferSize)
				Resize(AnsiSize, false);
			else
				MakeSelfBuffer();
			if (pStr)
			{
				if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_UTF8)
					UnicodeToUTF8(pStr, StrLen, m_pBuffer, AnsiSize);
				else
					UnicodeToAnsi(pStr, StrLen, m_pBuffer, AnsiSize);
			}

			m_pBuffer[AnsiSize] = 0;
			m_StringLength = AnsiSize;
			m_HashCode = 0;
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<WCHAR>::SetString(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	if (pStr)
	{
		if (StrLen == 0)
			StrLen = GetStrLen(pStr);
		if (StrLen > 0)
		{
			SIZE_TYPE UnicodeSize;
			if (CodePage == STRING_CODE_PAGE_UTF8)
				UnicodeSize = UTF8ToUnicode(pStr, StrLen, NULL, 0);
			else
				UnicodeSize = AnsiToUnicode(pStr, StrLen, NULL, 0);
			if (UnicodeSize >= m_BufferSize)
				Resize(UnicodeSize, false);
			else
				MakeSelfBuffer();
			if (pStr)
			{
				if (CodePage == STRING_CODE_PAGE_UTF8)
					UTF8ToUnicode(pStr, StrLen, m_pBuffer, UnicodeSize);
				else
					AnsiToUnicode(pStr, StrLen, m_pBuffer, UnicodeSize);
			}
			m_pBuffer[UnicodeSize] = 0;
			m_StringLength = UnicodeSize;
			m_HashCode = 0;
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<char>::SetStringRef(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	if (pStr)
	{
		if (StrLen == 0)
			StrLen = GetStrLen(pStr);
		if (StrLen > 0)
		{
			if ((CodePage == STRING_CODE_PAGE_AUTO) || (CodePage == SYSTEM_STRING_CODE_PAGE))
			{
				ReleaseBuffer();
				m_pBuffer = (char*)pStr;
				m_BufferSize = 0;
				m_StringLength = StrLen;
				m_HashCode = 0;
			}
			else
			{
				SetString(pStr, StrLen, CodePage);
			}
		}
		else
		{
			Clear();
		}
	}
	else
	{
		Clear();
	}
}

template<>
inline void CEasyStringT<WCHAR>::SetStringRef(const char* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	SetString(pStr, StrLen, CodePage);
}

template<>
inline void CEasyStringT<char>::SetStringRef(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	SetString(pStr, StrLen, CodePage);
}

template<>
inline void CEasyStringT<WCHAR>::SetStringRef(const WCHAR* pStr, SIZE_TYPE StrLen, STRING_CODE_PAGE CodePage)
{
	ReleaseBuffer();
	m_pBuffer = (WCHAR*)pStr;
	m_BufferSize = 0;
	m_StringLength = StrLen;
	m_HashCode = 0;
}

template<>
inline bool CEasyStringT<char>::Append(const char* pStr, SIZE_TYPE StrLen, bool AutoExpandBuffer, STRING_CODE_PAGE CodePage)
{
	if (StrLen == 0)
		StrLen = GetStrLen(pStr);
	if (pStr && (StrLen > 0))
	{
		if ((CodePage == STRING_CODE_PAGE_AUTO) || (CodePage == SYSTEM_STRING_CODE_PAGE))
		{
			if (m_StringLength + StrLen >= m_BufferSize)
			{
				if (AutoExpandBuffer)
					Resize(m_StringLength + StrLen);
				else
					return false;
			}
			else
			{
				MakeSelfBuffer();
			}
			memmove(m_pBuffer + m_StringLength, pStr, StrLen);
			m_pBuffer[m_StringLength + StrLen] = 0;
			m_StringLength += StrLen;
			m_HashCode = 0;
		}
		else if (CodePage == STRING_CODE_PAGE_ANSI)
		{
			SIZE_TYPE DestSize = AnsiToUTF8(pStr, StrLen, NULL, 0);
			if (DestSize + m_StringLength >= m_BufferSize)
			{
				if (AutoExpandBuffer)
					Resize(DestSize + m_StringLength);
				else
					return false;
			}
			else
			{
				MakeSelfBuffer();
			}
			AnsiToUTF8(pStr + m_StringLength, StrLen, m_pBuffer, DestSize);
			m_pBuffer[m_StringLength + DestSize] = 0;
			m_StringLength += DestSize;
			m_HashCode = 0;
		}
		else if (CodePage == STRING_CODE_PAGE_UTF8)
		{
			SIZE_TYPE DestSize = UTF8ToAnsi(pStr, StrLen, NULL, 0);
			if (DestSize + m_StringLength >= m_BufferSize)
			{
				if (AutoExpandBuffer)
					Resize(DestSize + m_StringLength);
				else
					return false;
			}
			else
			{
				MakeSelfBuffer();
			}
			UTF8ToAnsi(pStr + m_StringLength, StrLen, m_pBuffer, DestSize);
			m_pBuffer[m_StringLength + DestSize] = 0;
			m_StringLength += DestSize;
			m_HashCode = 0;
		}
	}
	return true;
}

template<>
inline bool CEasyStringT<WCHAR>::Append(const WCHAR* pStr, SIZE_TYPE StrLen, bool AutoExpandBuffer, STRING_CODE_PAGE CodePage)
{
	if (StrLen == 0)
		StrLen = GetStrLen(pStr);
	if (pStr && (StrLen > 0))
	{
		if (m_StringLength + StrLen >= m_BufferSize)
		{
			if (AutoExpandBuffer)
				Resize(m_StringLength + StrLen);
			else
				return false;
		}
		else
		{
			MakeSelfBuffer();
		}
		memmove(m_pBuffer + m_StringLength, pStr, sizeof(WCHAR) * StrLen);
		m_pBuffer[m_StringLength + StrLen] = 0;
		m_StringLength += StrLen;
		m_HashCode = 0;
	}
	return true;
}

template<>
inline bool CEasyStringT<char>::Append(const WCHAR* pStr, SIZE_TYPE StrLen, bool AutoExpandBuffer, STRING_CODE_PAGE CodePage)
{
	if (StrLen == 0)
		StrLen = GetStrLen(pStr);
	if (pStr && (StrLen > 0))
	{
		SIZE_TYPE AnsiSize;
		if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_UTF8)
			AnsiSize = UnicodeToUTF8(pStr, StrLen, NULL, 0);
		else
			AnsiSize = UnicodeToAnsi(pStr, StrLen, NULL, 0);
		if (m_StringLength + AnsiSize >= m_BufferSize)
		{
			if (AutoExpandBuffer)
				Resize(m_StringLength + AnsiSize);
			else
				return false;
		}
		else
		{
			MakeSelfBuffer();
		}

		if (pStr)
		{
			if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_UTF8)
				UnicodeToUTF8(pStr, StrLen, m_pBuffer + m_StringLength, AnsiSize);
			else
				UnicodeToAnsi(pStr, StrLen, m_pBuffer + m_StringLength, AnsiSize);
		}

		m_pBuffer[m_StringLength + AnsiSize] = 0;
		m_StringLength += AnsiSize;
		m_HashCode = 0;
	}
	return true;
}


template<>
inline bool CEasyStringT<WCHAR>::Append(const char* pStr, SIZE_TYPE StrLen, bool AutoExpandBuffer, STRING_CODE_PAGE CodePage)
{
	if (StrLen == 0)
		StrLen = GetStrLen(pStr);
	if (pStr && (StrLen > 0))
	{
		SIZE_TYPE UnicodeSize;
		if (CodePage == STRING_CODE_PAGE_UTF8)
			UnicodeSize = UTF8ToUnicode(pStr, StrLen, NULL, 0);
		else
			UnicodeSize = AnsiToUnicode(pStr, StrLen, NULL, 0);
		if (m_StringLength + UnicodeSize >= m_BufferSize)
		{
			if (AutoExpandBuffer)
				Resize(m_StringLength + UnicodeSize);
			else
				return false;
		}
		else
		{
			MakeSelfBuffer();
		}

		if (pStr)
		{
			if (CodePage == STRING_CODE_PAGE_UTF8)
				UTF8ToUnicode(pStr, StrLen, m_pBuffer + m_StringLength, UnicodeSize);
			else
				AnsiToUnicode(pStr, StrLen, m_pBuffer + m_StringLength, UnicodeSize);
		}
		m_pBuffer[m_StringLength + UnicodeSize] = 0;
		m_StringLength += UnicodeSize;
		m_HashCode = 0;
	}
	return true;
}
template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::GetStringBytes(BYTE* pBuffer, SIZE_TYPE BufferSize, STRING_CODE_PAGE CodePage) const
{
	if ((CodePage == STRING_CODE_PAGE_AUTO) || (CodePage == SYSTEM_STRING_CODE_PAGE))
	{
		if (pBuffer)
		{
			SIZE_TYPE CopySize = (BufferSize < m_StringLength) ? BufferSize : m_StringLength;
			memcpy(pBuffer, m_pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return m_StringLength;
		}
	}
	else if (CodePage == STRING_CODE_PAGE_ANSI)
	{
		SIZE_TYPE DestSize = UTF8ToAnsi(m_pBuffer, m_StringLength, NULL, 0);
		if (pBuffer)
		{
			SIZE_TYPE CopySize = (BufferSize < DestSize) ? BufferSize : DestSize;
			CopySize = UTF8ToAnsi(m_pBuffer, m_StringLength, (char*)pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return DestSize;
		}
	}
	else if (CodePage == STRING_CODE_PAGE_UTF8)
	{
		SIZE_TYPE DestSize = AnsiToUTF8(m_pBuffer, m_StringLength, NULL, 0);
		if (pBuffer)
		{
			SIZE_TYPE CopySize = (BufferSize < DestSize) ? BufferSize : DestSize;
			CopySize = AnsiToUTF8(m_pBuffer, m_StringLength, (char*)pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return DestSize;
		}
	}
	else if (CodePage == STRING_CODE_PAGE_UCS16)
	{
		SIZE_TYPE DestSize;
		if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_ANSI)
			DestSize = AnsiToUnicode(m_pBuffer, m_StringLength, NULL, 0) * sizeof(WCHAR);
		else
			DestSize = UTF8ToUnicode(m_pBuffer, m_StringLength, NULL, 0) * sizeof(WCHAR);
		if (pBuffer)
		{
			BufferSize = ((SIZE_TYPE)(BufferSize / (sizeof(WCHAR)))) * sizeof(WCHAR);
			SIZE_TYPE CopySize = (BufferSize < DestSize) ? BufferSize : DestSize;
			if (SYSTEM_STRING_CODE_PAGE == STRING_CODE_PAGE_ANSI)
				CopySize = AnsiToUnicode(m_pBuffer, m_StringLength, (WCHAR*)pBuffer, DestSize / sizeof(WCHAR));
			else
				CopySize = UTF8ToUnicode(m_pBuffer, m_StringLength, (WCHAR*)pBuffer, DestSize / sizeof(WCHAR));
			return CopySize;
		}
		else
		{
			return DestSize;
		}
	}
	return 0;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::GetStringBytes(BYTE* pBuffer, SIZE_TYPE BufferSize, STRING_CODE_PAGE CodePage) const
{
	if ((CodePage == STRING_CODE_PAGE_AUTO) || (CodePage == SYSTEM_STRING_CODE_PAGE))
	{
		if (pBuffer)
		{
			BufferSize = (SIZE_TYPE)(BufferSize / (sizeof(WCHAR)));
			SIZE_TYPE CopySize = ((BufferSize < m_StringLength) ? BufferSize : m_StringLength) * sizeof(WCHAR);
			memcpy(pBuffer, m_pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return m_StringLength * sizeof(WCHAR);
		}
	}
	else if (CodePage == STRING_CODE_PAGE_ANSI)
	{
		SIZE_TYPE DestSize = UnicodeToAnsi(m_pBuffer, m_StringLength, NULL, 0);
		if (pBuffer)
		{
			SIZE_TYPE CopySize = (BufferSize < DestSize) ? BufferSize : DestSize;
			CopySize = UnicodeToAnsi(m_pBuffer, m_StringLength, (char*)pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return DestSize;
		}
	}
	else if (CodePage == STRING_CODE_PAGE_UTF8)
	{
		SIZE_TYPE DestSize = UnicodeToUTF8(m_pBuffer, m_StringLength, NULL, 0);
		if (pBuffer)
		{
			SIZE_TYPE CopySize = (BufferSize < DestSize) ? BufferSize : DestSize;
			CopySize = UnicodeToUTF8(m_pBuffer, m_StringLength, (char*)pBuffer, CopySize);
			return CopySize;
		}
		else
		{
			return DestSize;
		}
	}
	return 0;
}

template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::GetFormatLenVL(const char* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscprintf(pFormat, vl2);
	va_end(vl2);

	return Len;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::GetFormatLenVL(const WCHAR* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscwprintf(pFormat, vl);
	va_end(vl2);

	return Len;
}

template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::GetFormatLen(const char* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	SIZE_TYPE Len = GetFormatLenVL(pFormat, vl);
	va_end(vl);
	return Len;
}


template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::GetFormatLen(const WCHAR* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	SIZE_TYPE Len = GetFormatLenVL(pFormat, vl);
	va_end(vl);
	return Len;
}

template<>
inline void CEasyStringT<char>::FormatVL(const char* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscprintf(pFormat, vl2);
	va_end(vl2);

	Resize(Len, false);
	vsprintf_s(m_pBuffer, m_BufferSize, pFormat, vl);
	m_StringLength = Len;
}

template<>
inline void CEasyStringT<WCHAR>::FormatVL(const WCHAR* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscwprintf(pFormat, vl);
	va_end(vl2);

	Resize(Len, false);
	vswprintf_s(m_pBuffer, m_BufferSize, pFormat, vl);
	m_StringLength = Len;
}

template<>
inline void CEasyStringT<char>::Format(const char* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	FormatVL(pFormat, vl);
	va_end(vl);
}


template<>
inline void CEasyStringT<WCHAR>::Format(const WCHAR* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	FormatVL(pFormat, vl);
	va_end(vl);
}

template<>
inline void CEasyStringT<char>::AppendFormatVL(const char* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscprintf(pFormat, vl2);
	va_end(vl2);

	SIZE_TYPE Pos = m_StringLength;
	Resize(m_StringLength + Len, true);
	vsprintf_s(m_pBuffer + Pos, Len + 1, pFormat, vl);
	m_StringLength += Len;
}

template<>
inline void CEasyStringT<WCHAR>::AppendFormatVL(const WCHAR* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscwprintf(pFormat, vl2);
	va_end(vl2);

	SIZE_TYPE Pos = m_StringLength;
	Resize(m_StringLength + Len, true);
	vswprintf_s(m_pBuffer + Pos, Len + 1, pFormat, vl);
	m_StringLength += Len;
}

template<>
inline void CEasyStringT<char>::AppendFormat(const char* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	AppendFormatVL(pFormat, vl);
	va_end(vl);
}

template<>
inline void CEasyStringT<WCHAR>::AppendFormat(const WCHAR* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	AppendFormatVL(pFormat, vl);
	va_end(vl);
}

template<>
inline bool CEasyStringT<char>::AppendFormatNoExpandVL(const char* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscprintf(pFormat, vl2);
	va_end(vl2);

	SIZE_TYPE Pos = m_StringLength;
	if (m_StringLength + Len >= m_BufferSize)
		return false;
	vsprintf_s(m_pBuffer + Pos, Len + 1, pFormat, vl);
	m_StringLength += Len;
	return true;
}

template<>
inline bool CEasyStringT<WCHAR>::AppendFormatNoExpandVL(const WCHAR* pFormat, va_list vl)
{
	va_list vl2;
	va_copy(vl2, vl);
	SIZE_TYPE Len = _vscwprintf(pFormat, vl2);
	va_end(vl2);

	SIZE_TYPE Pos = m_StringLength;
	if (m_StringLength + Len >= m_BufferSize)
		return false;
	vswprintf_s(m_pBuffer + Pos, Len + 1, pFormat, vl);
	m_StringLength += Len;
	return true;
}

template<>
inline bool CEasyStringT<char>::AppendFormatNoExpand(const char* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	bool Ret = AppendFormatNoExpandVL(pFormat, vl);
	va_end(vl);
	return Ret;
}

template<>
inline bool CEasyStringT<WCHAR>::AppendFormatNoExpand(const WCHAR* pFormat, ...)
{
	va_list	vl;
	va_start(vl, pFormat);
	bool Ret = AppendFormatNoExpandVL(pFormat, vl);
	va_end(vl);
	return Ret;
}

template<>
inline void CEasyStringT<char>::MakeUpper()
{
	MakeSelfBuffer();
	if (m_BufferSize)
	{
		_strupr_s(m_pBuffer, m_BufferSize);
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<WCHAR>::MakeUpper()
{
	MakeSelfBuffer();
	if (m_BufferSize)
	{
		_wcsupr_s(m_pBuffer, m_BufferSize);
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<char>::MakeLower()
{
	MakeSelfBuffer();
	if (m_BufferSize)
	{
		_strlwr_s(m_pBuffer, m_BufferSize);
		m_HashCode = 0;
	}
}

template<>
inline void CEasyStringT<WCHAR>::MakeLower()
{
	MakeSelfBuffer();
	if (m_BufferSize)
	{
		_wcslwr_s(m_pBuffer, m_BufferSize);
		m_HashCode = 0;
	}
}

template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::Find(const char* pDestStr, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if (StartPos > m_StringLength)
		StartPos = m_StringLength;
	const char* pResult;
	if (IgnoreCase)
		pResult = stristr(m_pBuffer + StartPos, pDestStr);
	else
		pResult = strstr(m_pBuffer + StartPos, pDestStr);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::Find(const WCHAR* pDestStr, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if (StartPos > m_StringLength)
		StartPos = m_StringLength;
	const WCHAR* pResult;
	if (IgnoreCase)
		pResult = wcsistr(m_pBuffer + StartPos, pDestStr);
	else
		pResult = wcsstr(m_pBuffer + StartPos, pDestStr);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}

template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::Find(char DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if (StartPos > m_StringLength)
		StartPos = m_StringLength;
	const char* pResult;
	if (IgnoreCase)
		pResult = strichr(m_pBuffer + StartPos, DestChar);
	else
		pResult = strchr(m_pBuffer + StartPos, DestChar);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::Find(WCHAR DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	if (StartPos > m_StringLength)
		StartPos = m_StringLength;
	const WCHAR* pResult;
	if (IgnoreCase)
		pResult = wcsichr(m_pBuffer + StartPos, DestChar);
	else
		pResult = wcschr(m_pBuffer + StartPos, DestChar);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::Find(char DestChar, SIZE_TYPE StartPos, bool IgnoreCase) const
{
	return Find((WCHAR)DestChar, StartPos, IgnoreCase);
}

template<>
inline CEasyStringT<char>::SIZE_TYPE CEasyStringT<char>::ReverseFind(char DestChar, bool IgnoreCase) const
{
	const char* pResult;
	if (IgnoreCase)
		pResult = strrichr(m_pBuffer, DestChar);
	else
		pResult = strrchr(m_pBuffer, DestChar);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}

template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::ReverseFind(WCHAR DestChar, bool IgnoreCase) const
{
	const WCHAR* pResult;
	if (IgnoreCase)
		pResult = wcsrichr(m_pBuffer, DestChar);
	else
		pResult = wcsrchr(m_pBuffer, DestChar);
	if (pResult == NULL)
		return INVALID_POS;
	else
		return pResult - m_pBuffer;
}
template<>
inline CEasyStringT<WCHAR>::SIZE_TYPE CEasyStringT<WCHAR>::ReverseFind(char DestChar, bool IgnoreCase) const
{
	return ReverseFind((WCHAR)DestChar);
}

template <typename T>
inline const CEasyStringT<T> MakeConstString(const T* pStr)
{
	CEasyStringT<T> Str;

	Str.SetStringConst(pStr);
	return Str;
}
template<>
inline bool CEasyStringT<char>::StartWith(const char* szStr)
{
	size_t Len = GetStrLen(szStr);
	if (GetLength() >= Len)
	{
		return strncmp(m_pBuffer, szStr, Len) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<WCHAR>::StartWith(const char* szStr)
{
	CEasyStringT<WCHAR> Str = szStr;
	if (GetLength() >= Str.GetLength())
	{
		return _wcsnicmp(m_pBuffer, Str.GetBuffer(), Str.GetLength()) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<char>::StartWith(const WCHAR* szStr)
{
	CEasyStringT<char> Str = szStr;
	if (GetLength() >= Str.GetLength())
	{
		return strncmp(m_pBuffer, Str.GetBuffer(), Str.GetLength()) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<WCHAR>::StartWith(const WCHAR* szStr)
{
	size_t Len = GetStrLen(szStr);
	if (GetLength() >= Len)
	{
		return _wcsnicmp(m_pBuffer, szStr, Len) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<char>::EndWith(const char* szStr)
{
	size_t Len = GetStrLen(szStr);
	if (GetLength() >= Len)
	{
		return strncmp(m_pBuffer + GetLength() - Len, szStr, Len) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<WCHAR>::EndWith(const char* szStr)
{
	CEasyStringT<WCHAR> Str = szStr;
	if (GetLength() >= Str.GetLength())
	{
		return _wcsnicmp(m_pBuffer + GetLength() - Str.GetLength(), Str.GetBuffer(), Str.GetLength()) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<char>::EndWith(const WCHAR* szStr)
{
	CEasyStringT<char> Str = szStr;
	if (GetLength() >= Str.GetLength())
	{
		return strncmp(m_pBuffer + GetLength() - Str.GetLength(), Str.GetBuffer(), Str.GetLength()) == 0;
	}
	return false;
}
template<>
inline bool CEasyStringT<WCHAR>::EndWith(const WCHAR* szStr)
{
	size_t Len = GetStrLen(szStr);
	if (GetLength() >= Len)
	{
		return _wcsnicmp(m_pBuffer + GetLength() - Len, szStr, Len) == 0;
	}
	return false;
}

typedef CEasyStringT<WCHAR> CEasyStringW;

typedef CEasyStringT<char> CEasyStringA;

#ifdef UNICODE

typedef CEasyStringT<WCHAR> CEasyString;

#else

typedef CEasyStringT<TCHAR> CEasyString;

#endif


template<typename T,
	std::enable_if_t<
	std::is_same<std::decay_t<T>, unsigned char>::value ||
	std::is_same<std::decay_t<T>, unsigned short>::value ||
	std::is_same<std::decay_t<T>, unsigned int>::value,
	bool> = true>
inline CEasyString ToString(T Value)
{
	CEasyString Str;
	Str.Format(_T("%u"), Value);
	return Str;
}

template<typename T,
	std::enable_if_t<
	std::is_same<std::decay_t<T>, char>::value ||
	std::is_same<std::decay_t<T>, short>::value ||
	std::is_same<std::decay_t<T>, int>::value,
	bool> = true>
inline CEasyString ToString(T Value)
{
	CEasyString Str;
	Str.Format(_T("%d"), Value);
	return Str;
}

inline CEasyString ToString(unsigned __int64 Value)
{
	CEasyString Str;
	Str.Format(_T("%llu"), Value);
	return Str;
}

inline CEasyString ToString(__int64 Value)
{
	CEasyString Str;
	Str.Format(_T("%lld"), Value);
	return Str;
}

inline CEasyString ToString(float Value)
{
	CEasyString Str;
	Str.Format(_T("%f"), Value);
	return Str;
}

inline CEasyString ToString(double Value)
{
	CEasyString Str;
	Str.Format(_T("%f"), Value);
	return Str;
}

inline CEasyString ToString(bool Value)
{
	return  Value ? _T("true") : _T("false");
}




inline CEasyString UTF8ToLocal(LPCSTR szStr, CEasyString::SIZE_TYPE StrLen = 0)
{
	if (StrLen == 0)
		StrLen = CEasyString::GetStrLen(szStr);
	CEasyString OutStr;
#ifdef UNICODE
	size_t OutLen = UTF8ToUnicode(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UTF8ToUnicode(szStr, StrLen, (TCHAR*)OutStr.GetBuffer(), OutLen);
#else
	size_t OutLen = UTF8ToAnsi(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UTF8ToAnsi(szStr, StrLen, (TCHAR*)OutStr.GetBuffer(), OutLen);
#endif
	OutStr.TrimBuffer();
	return OutStr;
}

inline CEasyStringA LocalToUTF8(LPCTSTR szStr, CEasyStringA::SIZE_TYPE StrLen = 0)
{
	if (StrLen == 0)
		StrLen = CEasyString::GetStrLen(szStr);
	CEasyStringA OutStr;
#ifdef UNICODE
	size_t OutLen = UnicodeToUTF8(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UnicodeToUTF8(szStr, StrLen, (char*)OutStr.GetBuffer(), OutLen);
#else
	size_t OutLen = AnsiToUTF8(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	AnsiToUTF8(szStr, StrLen, (char*)OutStr.GetBuffer(), OutLen);
#endif
	OutStr.TrimBuffer();
	return OutStr;
}

inline CEasyStringW AnsiToUnicode(const char* SrcStr, CEasyStringW::SIZE_TYPE SrcLen = 0)
{
	CEasyStringW DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = AnsiToUnicode(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		AnsiToUnicode(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}

inline CEasyStringA UnicodeToAnsi(const WCHAR* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringA DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = UnicodeToAnsi(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		UnicodeToAnsi(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}

inline CEasyStringW UTF8ToUnicode(const char* SrcStr, CEasyStringW::SIZE_TYPE SrcLen = 0)
{
	CEasyStringW DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = UTF8ToUnicode(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		UTF8ToUnicode(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}

inline CEasyStringA UnicodeToUTF8(const WCHAR* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringA DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = UnicodeToUTF8(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		UnicodeToUTF8(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}
#ifdef WIN32
inline CEasyStringA AnsiToUTF8(const char* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringW UnicodeStr = AnsiToUnicode(SrcStr, SrcLen);
	return UnicodeToUTF8(UnicodeStr, UnicodeStr.GetLength());
}

inline CEasyStringA UTF8ToAnsi(const char* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringW UnicodeStr = UTF8ToUnicode(SrcStr, SrcLen);
	return UnicodeToAnsi(UnicodeStr, UnicodeStr.GetLength());
}
#else
inline CEasyStringA AnsiToUTF8(const char* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringA DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = AnsiToUTF8(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		AnsiToUTF8(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}

inline CEasyStringA UTF8ToAnsi(const char* SrcStr, CEasyStringA::SIZE_TYPE SrcLen = 0)
{
	CEasyStringA DestStr;
	if (SrcLen == 0)
		SrcLen = CEasyString::GetStrLen(SrcStr);
	size_t DestLen = UTF8ToAnsi(SrcStr, SrcLen, NULL, 0);
	if (DestLen)
	{
		DestStr.Resize(DestLen, false);
		UTF8ToAnsi(SrcStr, SrcLen, DestStr.GetBuffer(), DestLen);
		DestStr.TrimBuffer(DestLen);
	}
	return DestStr;
}
#endif

inline CEasyString FormatSplitNumber(UINT Number)
{
	CEasyString Str;

	Str.Format(_T("%u"), Number);
	int i = (int)Str.GetLength() - 3;
	while (i > 0)
	{
		Str.Insert(i, ',');
		i -= 3;
	}
	return Str;
}

inline CEasyString FormatSplitNumber(int Number)
{
	CEasyString Str;

	Str.Format(_T("%d"), Number);
	int i = (int)Str.GetLength() - 3;
	while (i > 0)
	{
		Str.Insert(i, ',');
		i -= 3;
	}
	return Str;
}
inline CEasyString FormatSplitNumber(UINT64 Number)
{
	CEasyString Str;

	Str.Format(_T("%llu"), Number);
	int i = (int)Str.GetLength() - 3;
	while (i > 0)
	{
		Str.Insert(i, ',');
		i -= 3;
	}
	return Str;
}

inline CEasyString FormatSplitNumber(INT64 Number)
{
	CEasyString Str;

	Str.Format(_T("%lld"), Number);
	int i = (int)Str.GetLength() - 3;
	while (i > 0)
	{
		Str.Insert(i, ',');
		i -= 3;
	}
	return Str;
}

inline CEasyString FormatNumberWords(ULONG64 Number, bool IsTiny = false)
{
	CEasyString temp;


	if (Number == 0) return _T("0");
	if (Number <= 1024)
	{
		temp.Format(_T("%llu"), Number);
	}
	else if (Number <= 1048576)
	{
		float size = Number / 1024.0f;
		temp.Format(_T("%.2fK"), size);
	}
	else if (Number <= 1073741824)
	{
		float size = Number / 1048576.0f;
		temp.Format(_T("%.2fM"), size);
	}
	else
	{
		float size = Number / 1073741824.0f;
		temp.Format(_T("%.2fG"), size);
	}

	if (IsTiny)
		return temp;
	else
		return temp + _T(" Byte");
}

inline CEasyString FormatNumberWordsFloat(float Number, bool IsTiny = false)
{
	CEasyString temp;


	if (Number == 0.0f)
	{
		temp = _T("0");
	}
	else if (Number <= 1024.0f)
	{
		temp.Format(_T("%.2f"), Number);
	}
	else if (Number <= 1048576.0f)
	{
		float size = Number / 1024.0f;
		temp.Format(_T("%.2fK"), size);
	}
	else
	{
		float size = Number / 1048576.0f;
		temp.Format(_T("%.2fM"), size);
	}
	if (IsTiny)
		return temp;
	else
		return temp + _T(" Byte");
}

inline CEasyString FormatTimeStr(UINT Time)
{
	CEasyString TimeStr;

	TimeStr.Format(_T("%u:%02u:%02u"), Time / 3600, (Time % 3600) / 60, Time % 60);

	return TimeStr;
}

inline UINT GetBomHeader(LPVOID pData, UINT DataLen)
{
	BYTE* pByte = (BYTE*)pData;
	if (DataLen >= 2 && pByte[0] == 0xFF && pByte[1] == 0xFE)
		return BMT_UNICODE;
	if (DataLen >= 2 && pByte[0] == 0xFE && pByte[1] == 0xFF)
		return BMT_UNICODE_BIG_ENDIAN;
	if (DataLen >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF)
		return BMT_UTF_8;
	return 0;
}

inline void BinToString(const BYTE* pData, size_t Len, CEasyString& BinString, bool IsLowCase = true)
{
	CEasyString temp;
	BinString.Empty();
	for (size_t i = 0; i < Len; i++)
	{
		if (IsLowCase)
			temp.Format(_T("%02x"), pData[i]);
		else
			temp.Format(_T("%02X"), pData[i]);
		BinString += temp;
	}
}

inline CEasyString BinToString(const BYTE* pData, size_t Len, bool IsLowCase = true)
{
	CEasyString BinString;
	BinToString(pData, Len, BinString, IsLowCase);
	return BinString;
}

inline bool StringToBin(LPCTSTR szStr, size_t StrLen, BYTE* pData, size_t BuffLen)
{
	TCHAR ConterBuff[3];
	ConterBuff[2] = 0;

	if (StrLen == 0)
		StrLen = _tcslen(szStr);
	size_t OutPtr = 0;
	while (StrLen >= 2 && OutPtr < BuffLen)
	{
		ConterBuff[0] = szStr[0];
		ConterBuff[1] = szStr[1];
		szStr += 2;
		StrLen -= 2;
		pData[OutPtr] = (BYTE)_tcstol(ConterBuff, NULL, 16);
		OutPtr++;
	}
	return true;
}