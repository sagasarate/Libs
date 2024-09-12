#pragma once

class CBase64
{
public:
	CBase64(){};

	static bool Encode(const CEasyString& InData, CEasyString& OutData, bool AddCRLF=false);
	static bool Encode(LPCVOID pInData,size_t DataLen,CEasyString& OutData, bool AddCRLF=false);
	static CEasyString Encode(const CEasyString& InData, bool AddCRLF=false);
	static CEasyString Encode(LPCVOID pInData,size_t DataLen,bool AddCRLF=false);
	static bool Encode(LPCVOID pInData,size_t DataLen,TCHAR * pOutBuff, size_t& OutLen, bool AddCRLF=false);

	static bool Decode(const CEasyString& InData,CEasyBuffer& OutData);
	static bool Decode(const CEasyString& InData,BYTE * pOutData, size_t& OutLen);
	static bool Decode(LPCTSTR szInData,size_t InLen, CEasyBuffer& OutData);
	static bool Decode(LPCTSTR szInData,size_t InLen, BYTE * pOutData, size_t& OutLen);

	static inline size_t GetEncodeLen(size_t InLen)
	{
		return (InLen +(InLen%3 == 0? 0:(3-InLen%3)))/3*4;
	}

	static inline size_t GetDecodeLen(LPCTSTR szInData,size_t InLen)
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
	

	static	const TCHAR *bstr;
	static	const TCHAR rstr[128];
};






