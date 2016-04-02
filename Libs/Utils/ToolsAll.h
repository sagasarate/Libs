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


inline CEasyString BinToString(BYTE * pData,UINT Len)
{
	CEasyString BinString,temp;
	UINT i;

	for (i = 0; i < Len; i++)
	{
		temp.Format(_T("%02x"), pData[i]);
		BinString+=temp;
	}
	return BinString;
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

inline bool IsDirSlash(TCHAR Char)
{
	return Char == '/' || Char == '\\';
}


inline CEasyString GetRelativePath(LPCTSTR szSrcDir, LPCTSTR szSrcPath)
{

	if (szSrcDir == NULL || szSrcPath == NULL)
		return CEasyString();

	

	CEasyString SrcDir = MakeFullPath(szSrcDir);

	CEasyString SrcPath = MakeFullPath(szSrcPath);

	if (SrcDir.IsEmpty() || SrcPath.IsEmpty())
		return CEasyString();

	UINT DirReturnCount = 0;
	UINT SameDirLen = 0;
	bool IsInSameDir = true;
	UINT SrcLeftLen = 0;
	for (UINT i = 0; i < SrcDir.GetLength(); i++)
	{
		if (IsInSameDir)
		{
			if (i >= SrcPath.GetLength() || _totlower(SrcDir[i]) != _totlower(SrcPath[i]))
			{
				IsInSameDir = false;
			}				
			else if (IsDirSlash(SrcPath[i]))
			{
				SameDirLen = i + 1;
			}
		}
		
		if (!IsInSameDir)
		{
			SrcLeftLen++;
			if (IsDirSlash(SrcDir[i]))
			{
				DirReturnCount++;
				SrcLeftLen = 0;
			}
		}
	}

	if (SrcLeftLen)
	{
		DirReturnCount++;
	}

	CEasyString RelativePath;

	RelativePath.Resize(DirReturnCount * 3 + SrcPath.GetLength() - SameDirLen);

	for (UINT i = 0; i < DirReturnCount; i++)
	{
		RelativePath[i * 3] = '.';
		RelativePath[i * 3 + 1] = '.';
		RelativePath[i * 3 + 2] = DIR_SLASH;
	}

	memcpy((LPTSTR)RelativePath.GetBuffer() + DirReturnCount * 3, SrcPath.GetBuffer() + SameDirLen, (SrcPath.GetLength() - SameDirLen)*sizeof(TCHAR));	
	RelativePath.TrimBuffer(DirReturnCount * 3 + SrcPath.GetLength() - SameDirLen);
	return RelativePath;
}


inline CEasyString CombineString(CEasyArray<CEasyString>& StrList, TCHAR Separator)
{
	CEasyString ResultStr;
	UINT Len = 0;
	for (UINT i = 0; i < StrList.GetCount(); i++)
	{
		Len += StrList[i].GetLength();
	}
	if (Len)
	{
		Len += StrList.GetCount() - 1;
		ResultStr.Resize(Len);
		ResultStr.SetLength(Len);
		UINT BuffPtr = 0;
		TCHAR * pBuff = (TCHAR *)ResultStr;
		for (UINT i = 0; i < StrList.GetCount(); i++)
		{
			_tcscpy_s(pBuff + BuffPtr, Len - BuffPtr + 1, StrList[i]);
			BuffPtr += StrList[i].GetLength();
			if (i < StrList.GetCount() - 1)
			{
				pBuff[BuffPtr] = Separator;
				BuffPtr++;
			}
		}
	}
	return ResultStr;
}