/****************************************************************************/
/*                                                                          */
/*      文件名:    ToolsAll.h                                               */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define CP_UNICODE		1200
#define IS_TYPE_OF(pObject,Type)	((dynamic_cast<Type *>(pObject))!=NULL)

enum BOM_HEADER_TYPE
{
	BMT_UNICODE = 0xFEFF,
	BMT_UNICODE_BIG_ENDIAN = 0xFFFE,
	BMT_UTF_8 = 0xBFBBEF,
};

//inline size_t strnlen(const char * pStr,size_t BufferLen)
//{
//	size_t Len=0;
//	while(*pStr&&BufferLen)
//	{
//		pStr++;
//		Len++;
//		BufferLen--;
//	}
//	return Len;
//}

inline void Swap(FLOAT& v1,FLOAT& v2)
{
	FLOAT temp=v1;
	v1=v2;
	v2=temp;
}



inline int Saturate(int Value, int Min, int Max)
{
	if (Value<Min)
		return Min;
	if (Value>Max)
		return Max;
	return Value;
}

inline float Saturate(float Value, float Min, float Max)
{
	if (Value<Min)
		return Min;
	if (Value>Max)
		return Max;
	return Value;
}


inline int GetRand(int Min,int Max)
{
	if(Min>Max)
	{
		int Temp=Min;
		Min=Max;
		Max=Temp;
	}
	return rand()%(Max-Min+1)+Min;
}

inline UINT GetRand(UINT Min, UINT Max)
{
	if (Min > Max)
	{
		int Temp = Min;
		Min = Max;
		Max = Temp;
	}
	return rand() % (Max - Min + 1) + Min;
}

inline float GetRand(float Min,float Max)
{
	return (((float)rand())/RAND_MAX)*(Max-Min)+Min;
}

inline float GetRandGaussf(float Min,float Max)
{
	float sigma=(Max-Min)/2.0f;
	float mu=(Max+Min)/2.0f;
	return Saturate((float)(mu + ((rand() % 2) ? -1.0 : 1.0)*sigma*pow(-log(0.99999f*((double)rand() / RAND_MAX)), 0.5)),Min,Max);
}


inline CEasyString FormatSplitNumber(ULONG64 Number)
{
	CEasyString Str;

	Str.Format(_T("%I64u"),Number);
	int i=(int)Str.GetLength()-3;
	while(i>0)
	{
		Str.Insert(i,',');
		i-=3;
	}
	return Str;
}

inline CEasyString FormatNumberWords(ULONG64 Number,bool IsTiny=false)
{
	CEasyString temp;
	

	if(Number==0) return _T("0");
	if(Number<=1024)
	{
		temp.Format(_T("%llu"),Number);
	}else if(Number<=1048576)
	{
		float size=Number/1024.0f;
		temp.Format(_T("%.2fK"),size);
	}else
	{
		float size=Number/1048576.0f;
		temp.Format(_T("%.2fM"),size);
	}
	
	if(IsTiny)
		return temp;
	else
		return temp+_T(" Byte");
}

inline CEasyString FormatNumberWordsFloat(float Number,bool IsTiny=false)
{
	CEasyString temp;


	if(Number==0.0f)
	{
		temp=_T("0");
	}
	else if(Number<=1024.0f)
	{
		temp.Format(_T("%.2f"),Number);
	}else if(Number<=1048576.0f)
	{
		float size=Number/1024.0f;
		temp.Format(_T("%.2fK"),size);
	}else
	{
		float size=Number/1048576.0f;
		temp.Format(_T("%.2fM"),size);
	}
	if(IsTiny)
		return temp;
	else
		return temp+_T(" Byte");
}



inline UINT GetBomHeader(LPVOID pData, UINT DataLen)
{
	BYTE * pByte=(BYTE *)pData;
	if (DataLen >= 2 && pByte[0] == 0xFF && pByte[1] == 0xFE)
		return BMT_UNICODE;
	if (DataLen >= 2 && pByte[0] == 0xFE && pByte[1] == 0xFF)
		return BMT_UNICODE_BIG_ENDIAN;
	if (DataLen >= 3 && pByte[0] == 0xEF && pByte[1] == 0xBB && pByte[2] == 0xBF)
		return BMT_UTF_8;
	return 0;
}


inline CEasyString BinToString(const BYTE * pData, size_t Len, bool IsLowCase = true)
{
	CEasyString BinString,temp;

	for (size_t i = 0; i < Len; i++)
	{
		if (IsLowCase)
			temp.Format(_T("%02x"), pData[i]);
		else
			temp.Format(_T("%02X"), pData[i]);
		BinString+=temp;
	}
	return BinString;
}

inline bool StringToBin(LPCTSTR szStr, size_t StrLen, BYTE * pData, size_t BuffLen)
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

inline bool IsHostBigEndian()
{
	short Temp = 1;
	return ((BYTE *)&Temp)[0] == 0;
}
inline UINT64 __htonll(UINT64 Value)
{
	if (IsHostBigEndian())
	{
		return Value;
	}
	else
	{
		UINT64 Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 8; i++)
		{
			pOutput[i] = pInput[7 - i];
		}
		return Temp;
	}
}

inline UINT __htonf(float Value)
{
	if (IsHostBigEndian())
	{		
		return *((UINT *)&Value);
	}
	else
	{
		UINT Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 4; i++)
		{
			pOutput[i] = pInput[3 - i];
		}
		return Temp;
	}
}

inline UINT64 __htond(double Value)
{
	if (IsHostBigEndian())
	{
		return *((UINT64 *)&Value);
	}
	else
	{
		UINT64 Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 8; i++)
		{
			pOutput[i] = pInput[7 - i];
		}
		return Temp;
	}
}

inline UINT64 __ntohll(UINT64 Value)
{
	if (IsHostBigEndian())
	{
		return Value;
	}
	else
	{
		UINT64 Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 8; i++)
		{
			pOutput[i] = pInput[7 - i];
		}
		return Temp;
	}
}

inline float __ntohf(UINT Value)
{
	if (IsHostBigEndian())
	{
		return *((float *)&Value);
	}
	else
	{
		float Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 4; i++)
		{
			pOutput[i] = pInput[3 - i];
		}
		return Temp;
	}
}

inline double __ntohd(UINT64 Value)
{
	if (IsHostBigEndian())
	{
		return *((double *)&Value);
	}
	else
	{
		double Temp;
		BYTE * pInput = (BYTE *)&Value;
		BYTE * pOutput = (BYTE *)&Temp;
		for (int i = 0; i < 8; i++)
		{
			pOutput[i] = pInput[7 - i];
		}
		return Temp;
	}
}



inline CEasyString CombineString(CEasyArray<CEasyString>& StrList, TCHAR Separator)
{
	CEasyString ResultStr;
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
		TCHAR * pBuff = (TCHAR *)ResultStr;
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

inline CEasyString UTF8ToLocal(LPCSTR szStr, size_t StrLen = 0)
{
	if (StrLen == 0)
		StrLen = strlen(szStr);
	CEasyString OutStr;
#ifdef UNICODE
	size_t OutLen = UTF8ToUnicode(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UTF8ToUnicode(szStr, StrLen, (TCHAR *)OutStr.GetBuffer(), OutLen);
#else
	size_t OutLen = UTF8ToAnsi(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UTF8ToAnsi(szStr, StrLen, (TCHAR *)OutStr.GetBuffer(), OutLen);
#endif
	OutStr.TrimBuffer();
	return OutStr;
}

inline CEasyStringA LocalToUTF8(LPCTSTR szStr, size_t StrLen = 0)
{
	if (StrLen == 0)
		StrLen = _tcslen(szStr);
	CEasyStringA OutStr;
#ifdef UNICODE
	size_t OutLen = UnicodeToUTF8(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	UnicodeToUTF8(szStr, StrLen, (char *)OutStr.GetBuffer(), OutLen);
#else
	size_t OutLen = AnsiToUTF8(szStr, StrLen, NULL, 0);
	OutStr.Resize(OutLen);
	AnsiToUTF8(szStr, StrLen, (char *)OutStr.GetBuffer(), OutLen);
#endif
	OutStr.TrimBuffer();
	return OutStr;
}