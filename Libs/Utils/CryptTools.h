#pragma once
class CCryptTools
{
public:
	CCryptTools();
	~CCryptTools();

	static bool DESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool DESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);

	static bool AESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool AESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);


	static bool TEAEncryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool TEADecryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);

};

