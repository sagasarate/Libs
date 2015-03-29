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

inline BOOL PrintSystemLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_SYSTEM_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
}

inline BOOL PrintImportantLog(DWORD Color,LPCTSTR Format,...)
{
	va_list vl;
	va_start(vl,Format);
	BOOL ret=CLogManager::GetInstance()->PrintLogVL(LOG_IMPORTANT_CHANNEL,ILogPrinter::LOG_LEVEL_NORMAL,Color,Format,vl);
	va_end(vl);
	return ret;
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

inline float GetRandf(float Min,float Max)
{
	return (((float)rand())/RAND_MAX)*(Max-Min)+Min;
}

inline float GetRandGaussf(float Min,float Max)
{
	float sigma=(Max-Min)/2.0f;
	float mu=(Max+Min)/2.0f;
	return Saturate((mu + (rand() % 2 ? -1.0 : 1.0)*sigma*pow(-log(0.99999f*((double)rand() / RAND_MAX)), 0.5)),Min,Max);
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



inline UINT GetBomHeader(LPVOID pData)
{
	BYTE * pByte=(BYTE *)pData;
	if(pByte[0]==0xFF&&pByte[1]==0xFE)
		return BMT_UNICODE;
	if(pByte[0]==0xFE&&pByte[1]==0xFF)
		return BMT_UNICODE_BIG_ENDIAN;
	if(pByte[0]==0xEF&&pByte[1]==0xBB&&pByte[2]==0xBF)
		return BMT_UTF_8;
	return 0;
}


inline CEasyString BinToString(BYTE * pData,UINT Len)
{
	CEasyString BinString,temp;
	int i;

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
		return *((UINT64 *)&Value);
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