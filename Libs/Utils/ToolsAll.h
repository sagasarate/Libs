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


#define IS_TYPE_OF(pObject,Type)	((dynamic_cast<Type *>(pObject))!=NULL)



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

template<typename T>
inline T Max(T a, T b)
{
	return a > b ? a : b;
}

template<typename T>
inline T Min(T a, T b)
{
	return a < b ? a : b;
}

inline void Swap(FLOAT& v1,FLOAT& v2)
{
	FLOAT temp=v1;
	v1=v2;
	v2=temp;
}


template<typename T>
inline T Saturate(T Value, T Min, T Max)
{
	if (Value<Min)
		return Min;
	if (Value>Max)
		return Max;
	return Value;
}

template<typename T>
inline T Saturate(T Value, T Min = 0)
{
	if (Value < Min)
		return Min;
	return Value;
}


inline int GetRand(int Min,int Max)
{
	float RandValue = (float)rand();
	if (Min > Max)
		return (int)round(RandValue *((float)Min - (float)Max) / (float)RAND_MAX + Max);
	else
		return (int)round(RandValue *((float)Max - (float)Min) / (float)RAND_MAX + Min);
}

inline UINT GetRand(UINT Min, UINT Max)
{
	float RandValue = (float)rand();
	if (Min > Max)
		return (UINT)round(RandValue *((float)Min - (float)Max) / (float)RAND_MAX + Max);
	else
		return (UINT)round(RandValue *((float)Max - (float)Min) / (float)RAND_MAX + Min);
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


extern UINT WalkCallStack(void * pFrame, LPVOID * AddressList, UINT MaxStep);

