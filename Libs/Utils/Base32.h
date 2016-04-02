#pragma once
class CBase32
{
public:
	static bool Encode(const CEasyStringA& InData, CEasyStringA& OutData);
	static bool Encode(LPCVOID pInData, size_t DataLen, CEasyStringA& OutData);
	static CEasyStringA Encode(const CEasyStringA& InData);
	static CEasyStringA Encode(LPCVOID pInData, size_t DataLen);
	static inline bool Encode(LPCVOID pInData, size_t DataLen, char * pOutBuff, size_t& OutLen)
	{
		if (OutLen < base32EncodeGetLength(DataLen))
			return false;
		OutLen = base32Encode(pOutBuff, (const char *)pInData, DataLen);
		return true;
	}

	static bool Decode(const CEasyStringA& InData, CEasyBuffer& OutData);
	static bool Decode(const CEasyStringA& InData, BYTE * pOutData, size_t& OutLen);
	static bool Decode(LPCSTR szInData, size_t InLen, CEasyBuffer& OutData);
	static inline bool Decode(LPCSTR szInData, size_t InLen, BYTE * pOutData, size_t& OutLen)
	{
		if (OutLen < base32DecodeGetLength(InLen))
			return false;
		if (cyoBase32Validate(szInData, InLen))
			return false;
		OutLen = base32Decode(pOutData, szInData, InLen);
		return true;
	}

	static inline size_t GetEncodeLen(size_t InLen)
	{
		return base32EncodeGetLength(InLen);
	}

	static inline size_t GetDecodeLen(LPCSTR szInData, size_t InLen)
	{
		return base32DecodeGetLength(InLen);
	}

protected:

	static size_t base32EncodeGetLength(size_t size);
	static size_t base32Encode(char* dest, const void* src, size_t size);
	static int cyoBase32Validate(const char* src, size_t size);
	static size_t base32DecodeGetLength(size_t size);
	static size_t base32Decode(void* dest, const char* src, size_t size);
};

