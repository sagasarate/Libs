#include "StdAfx.h"

const char *CBase64::bstr =
"ABCDEFGHIJKLMNOPQ"
"RSTUVWXYZabcdefgh"
"ijklmnopqrstuvwxy"
"z0123456789+/";

const char CBase64::rstr[] = {
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 
	0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  62,   0,   0,   0,  63, 
	52,  53,  54,  55,  56,  57,  58,  59,  60,  61,   0,   0,   0,   0,   0,   0, 
	0,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, 
	15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,   0,   0,   0,   0,   0, 
	0,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, 
	41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,   0,   0,   0,   0,   0};




bool CBase64::Encode(const CEasyStringA& InData, CEasyStringA& OutData, bool AddCRLF)
{
	OutData.Resize(GetEncodeLen(InData.GetLength()));
	OutData.SetLength(OutData.GetBufferSize()-1);
	size_t OutLen=OutData.GetLength();
	return Encode(InData.GetBuffer(), InData.GetLength(), (char *)OutData.GetBuffer(),OutLen, AddCRLF);
}

bool CBase64::Encode(LPCVOID pInData,size_t DataLen,CEasyStringA& OutData, bool AddCRLF)
{
	OutData.Resize(GetEncodeLen(DataLen));
	OutData.SetLength(OutData.GetBufferSize()-1);
	size_t OutLen=OutData.GetLength();
	return Encode(pInData, DataLen, (char *)OutData.GetBuffer(), OutLen,AddCRLF);
}

CEasyStringA CBase64::Encode(const CEasyStringA& InData, bool AddCRLF)
{
	CEasyStringA OutData;
	OutData.Resize(GetEncodeLen(InData.GetLength()));
	OutData.SetLength(OutData.GetBufferSize()-1);
	size_t OutLen=OutData.GetBufferSize();
	Encode(InData.GetBuffer(), InData.GetLength(), (char *)OutData.GetBuffer(),OutLen, AddCRLF);
	return OutData;
}
CEasyStringA CBase64::Encode(LPCVOID pInData,size_t DataLen,bool AddCRLF)
{
	CEasyStringA OutData;	
	OutData.Resize(GetEncodeLen(DataLen));
	OutData.SetLength(OutData.GetBufferSize()-1);
	size_t OutLen=OutData.GetLength();
	Encode(pInData, DataLen, (char *)OutData.GetBuffer(),OutLen, AddCRLF);
	return OutData;
}

bool CBase64::Encode(LPCVOID pInData,size_t DataLen,char * pOutBuff, size_t& OutLen, bool AddCRLF)
{
	size_t i = 0;
	size_t o = 0;

	BYTE * pInput=(BYTE *)pInData;

	size_t OutPtr=0;
	while (i < DataLen)
	{
		if(OutLen-OutPtr<4)
		{
			return false;
		}
		size_t remain = DataLen - i;
		if (AddCRLF && o && o % 76 == 0)
		{
			*(pOutBuff++)='\n';
			OutPtr++;
		}
		switch (remain)
		{
		case 1:
			*(pOutBuff++)= bstr[ ((pInput[i] >> 2) & 0x3f) ];
			*(pOutBuff++)= bstr[ ((pInput[i] << 4) & 0x30) ];
			*(pOutBuff++)= '=';
			*(pOutBuff++)= '=';
			break;
		case 2:
			*(pOutBuff++)= bstr[ ((pInput[i] >> 2) & 0x3f) ];
			*(pOutBuff++)= bstr[ ((pInput[i] << 4) & 0x30) + ((pInput[i + 1] >> 4) & 0x0f) ];
			*(pOutBuff++)= bstr[ ((pInput[i + 1] << 2) & 0x3c) ];
			*(pOutBuff++)= '=';
			break;
		default:
			*(pOutBuff++)= bstr[ ((pInput[i] >> 2) & 0x3f) ];
			*(pOutBuff++)= bstr[ ((pInput[i] << 4) & 0x30) + ((pInput[i + 1] >> 4) & 0x0f) ];
			*(pOutBuff++)= bstr[ ((pInput[i + 1] << 2) & 0x3c) + ((pInput[i + 2] >> 6) & 0x03) ];
			*(pOutBuff++)= bstr[ (pInput[i + 2] & 0x3f) ];
		}
		o += 4;
		i += 3;
		OutPtr+=4;
	}
	OutLen=OutPtr;
	return true;
}

bool CBase64::Decode(const CEasyStringA& InData,CEasyBuffer& OutData)
{
	OutData.Create((UINT)GetDecodeLen(InData, InData.GetLength()));
	size_t OutLen=OutData.GetBufferSize();
	bool Ret=Decode(InData,InData.GetLength(),(BYTE *)OutData.GetBuffer(),OutLen);
	OutData.SetUsedSize((UINT)OutLen);
	return Ret;
}

bool CBase64::Decode(const CEasyStringA& InData,BYTE * pOutData, size_t& OutLen)
{
	return Decode(InData,InData.GetLength(),pOutData,OutLen);
}

bool  CBase64::Decode(LPCSTR szInData,size_t InLen, CEasyBuffer& OutData)
{
	OutData.Create((UINT)GetDecodeLen(szInData, InLen));
	size_t OutLen=OutData.GetBufferSize();
	bool Ret=Decode(szInData,InLen,(BYTE *)OutData.GetBuffer(),OutLen);
	OutData.SetUsedSize((UINT)OutLen);
	return Ret;
}

bool CBase64::Decode(LPCSTR szInData,size_t InLen, BYTE * pOutData, size_t& OutLen)
{
	size_t i = 0;
	size_t l = InLen;
	size_t j = 0;

	while (i < l)
	{
		while (i < l && (szInData[i] == 13 || szInData[i] == 10))
			i++;
		if (i < l)
		{
			unsigned char b1 = (unsigned char)((rstr[(int)szInData[i]] << 2 & 0xfc) +
				(rstr[(int)szInData[i + 1]] >> 4 & 0x03));
			if (pOutData)
			{
				pOutData[j] = b1;
			}
			j++;
			if(j>OutLen)
				return false;
			if (szInData[i + 2] != '=')
			{
				unsigned char b2 = (unsigned char)((rstr[(int)szInData[i + 1]] << 4 & 0xf0) +
					(rstr[(int)szInData[i + 2]] >> 2 & 0x0f));
				if (pOutData)
				{
					pOutData[j] = b2;
				}
				j++;
				if(j>OutLen)
					return false;
			}
			if (szInData[i + 3] != '=')
			{
				unsigned char b3 = (unsigned char)((rstr[(int)szInData[i + 2]] << 6 & 0xc0) +
					rstr[(int)szInData[i + 3]]);
				if (pOutData)
				{
					pOutData[j] = b3;
				}
				j++;
				if(j>OutLen)
					return false;
			}
			i += 4;
		}
	}
	OutLen = j;
	return true;
}



