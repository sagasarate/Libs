#pragma once
class CCryptTools
{
public:
	CCryptTools();
	~CCryptTools();

	static bool DESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool DESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static size_t GetDESEncryptECBSize(size_t DataLen)
	{
		return ((DataLen / DES_BLOCK_SIZE) + 1)*DES_BLOCK_SIZE;
	}

	static bool AESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool AESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static size_t GetAESEncryptECBSize(size_t DataLen)
	{
		return ((DataLen / AES_BLOCK_SIZE) + 1)*AES_BLOCK_SIZE;
	}

	static bool TEAEncryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static bool TEADecryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen);
	static size_t GetTEAEncryptECBSize(size_t DataLen)
	{
		return ((DataLen / TEA_BLOCK_SIZE) + 1)*TEA_BLOCK_SIZE;
	}
};

