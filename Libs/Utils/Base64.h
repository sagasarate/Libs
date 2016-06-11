#pragma once

class CBase64
{
public:
	CBase64(){};

	static bool Encode(const CEasyStringA& InData, CEasyStringA& OutData, bool AddCRLF=false);
	static bool Encode(LPCVOID pInData,size_t DataLen,CEasyStringA& OutData, bool AddCRLF=false);
	static CEasyStringA Encode(const CEasyStringA& InData, bool AddCRLF=false);
	static CEasyStringA Encode(LPCVOID pInData,size_t DataLen,bool AddCRLF=false);
	static bool Encode(LPCVOID pInData,size_t DataLen,char * pOutBuff, size_t& OutLen, bool AddCRLF=false);

	static bool Decode(const CEasyStringA& InData,CEasyBuffer& OutData);
	static bool Decode(const CEasyStringA& InData,BYTE * pOutData, size_t& OutLen);
	static bool Decode(LPCSTR szInData,size_t InLen, CEasyBuffer& OutData);
	static bool Decode(LPCSTR szInData,size_t InLen, BYTE * pOutData, size_t& OutLen);

	static inline size_t GetEncodeLen(size_t InLen)
	{
		return (InLen +(InLen%3 == 0? 0:(3-InLen%3)))/3*4;
	}

	static inline size_t GetDecodeLen(LPCSTR szInData,size_t InLen)
	{
		if (szInData==0 || InLen==0 || InLen % 4)
			return 0;
		size_t l = 3 * (InLen / 4 - 1) + 1;
		if (szInData[InLen - 2] != '=')
			l++;
		if (szInData[InLen - 1] != '=')
			l++;
		return l;
	}

private:
	

	static	const char *bstr;
	static	const char rstr[128];
};






