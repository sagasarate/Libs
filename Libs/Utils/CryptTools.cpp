#include "stdafx.h"



CCryptTools::CCryptTools()
{
}


CCryptTools::~CCryptTools()
{
}


bool CCryptTools::DESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < DES_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}
		

	if (pData == NULL || DataLen == 0)
	{
		OutLen = 0;
		return false;
	}

	size_t BlockCount = (DataLen / DES_BLOCK_SIZE) + 1;

	if (pOutBuffer == NULL)
	{
		OutLen = BlockCount * DES_BLOCK_SIZE;
		return true;
	}

	if (OutLen < BlockCount*DES_BLOCK_SIZE)
	{
		return false;
	}
	OutLen = BlockCount * DES_BLOCK_SIZE;


	BYTE WorkBuffer[DES_BLOCK_SIZE];
	struct des_ctx CTX;
	des_setkey(&CTX, pKey, (UINT)KeyLen);

	

	while (BlockCount)
	{
		if (BlockCount == 1)
		{
			//最后一块，需要填充
			if (DataLen)
				memcpy(WorkBuffer, pData, DataLen);
			BYTE Padding = (BYTE)(DES_BLOCK_SIZE - DataLen);
			for (BYTE i = 0; i < Padding; i++)
			{
				WorkBuffer[DataLen + i] = Padding;
			}
			des_encrypt(&CTX, pOutBuffer, WorkBuffer);
			pData += DataLen;
			DataLen = 0;
		}
		else
		{
			des_encrypt(&CTX, pOutBuffer, pData);
			pData += DES_BLOCK_SIZE;
			DataLen -= DES_BLOCK_SIZE;
		}
		pOutBuffer += DES_BLOCK_SIZE;
		BlockCount--;
	}
	return true;
}
bool CCryptTools::DESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < DES_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}


	if (pData == NULL || DataLen == 0 || (DataLen%DES_BLOCK_SIZE) != 0)
	{
		OutLen = 0;
		return false;
	}

	if (OutLen < DataLen)
	{
		OutLen = DataLen;
		return false;
	}
	OutLen = DataLen;

	struct des_ctx CTX;
	des_setkey(&CTX, pKey, (UINT)KeyLen);
	while (DataLen)
	{
		des_decrypt(&CTX, pOutBuffer, pData);
		if (DataLen == DES_BLOCK_SIZE)
		{
			//最后一块，去除填充
			BYTE Padding = pOutBuffer[DES_BLOCK_SIZE - 1];
			if (Padding <= DES_BLOCK_SIZE)
			{
				bool HavingPadding = true;
				for (BYTE i = 0; i < Padding; i++)
				{
					if (pOutBuffer[DES_BLOCK_SIZE - 1 - i] != Padding)
					{
						HavingPadding = false;
						break;
					}
				}
				if (HavingPadding)
				{
					OutLen -= Padding;
				}
			}
		}
		pData += DES_BLOCK_SIZE;
		DataLen -= DES_BLOCK_SIZE;		
		pOutBuffer += DES_BLOCK_SIZE;
		
	}
	return true;
}



bool CCryptTools::AESEncryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < AES_MIN_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}

	if (KeyLen < AES_KEYSIZE_192)
		KeyLen = AES_KEYSIZE_128;
	else if (KeyLen < AES_KEYSIZE_256)
		KeyLen = AES_KEYSIZE_192;
	else
		KeyLen = AES_KEYSIZE_256;


	if (pData == NULL || DataLen == 0)
	{
		OutLen = 0;
		return false;
	}

	size_t BlockCount = (DataLen / AES_BLOCK_SIZE) + 1;

	if (pOutBuffer == NULL)
	{
		OutLen = BlockCount * AES_BLOCK_SIZE;
		return true;
	}

	if (OutLen < BlockCount*AES_BLOCK_SIZE)
	{
		return false;
	}

	OutLen = BlockCount * AES_BLOCK_SIZE;

	BYTE WorkBuffer[AES_BLOCK_SIZE];
	struct crypto_aes_ctx CTX;
	crypto_aes_expand_key(&CTX, pKey, (UINT)KeyLen);


	

	while (BlockCount)
	{
		if (BlockCount == 1)
		{
			//最后一块，需要填充
			if (DataLen)
				memcpy(WorkBuffer, pData, DataLen);
			BYTE Padding = (BYTE)(AES_BLOCK_SIZE - DataLen);
			for (BYTE i = 0; i < Padding; i++)
			{
				WorkBuffer[DataLen + i] = Padding;
			}
			aes_encrypt(&CTX, pOutBuffer, WorkBuffer);
			pData += DataLen;
			DataLen = 0;
		}
		else
		{
			aes_encrypt(&CTX, pOutBuffer, pData);
			pData += AES_BLOCK_SIZE;
			DataLen -= AES_BLOCK_SIZE;
		}
		pOutBuffer += AES_BLOCK_SIZE;
		BlockCount --;
	}
	return true;
}
bool CCryptTools::AESDecryptECB(const BYTE * pKey, size_t KeyLen, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < AES_MIN_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}

	if (KeyLen < AES_KEYSIZE_192)
		KeyLen = AES_KEYSIZE_128;
	else if (KeyLen < AES_KEYSIZE_256)
		KeyLen = AES_KEYSIZE_192;
	else
		KeyLen = AES_KEYSIZE_256;


	if (pData == NULL || DataLen == 0 || (DataLen%AES_BLOCK_SIZE) != 0)
	{
		OutLen = 0;
		return false;
	}

	if (OutLen < DataLen)
	{
		OutLen = DataLen;
		return false;
	}
	OutLen = DataLen;

	struct crypto_aes_ctx CTX;
	crypto_aes_expand_key(&CTX, pKey, (UINT)KeyLen);
	while (DataLen)
	{
		aes_decrypt(&CTX, pOutBuffer, pData);
		if (DataLen == AES_BLOCK_SIZE)
		{
			//最后一块，去除填充
			BYTE Padding = pOutBuffer[AES_BLOCK_SIZE - 1];
			if (Padding <= AES_BLOCK_SIZE)
			{
				bool HavingPadding = true;
				for (BYTE i = 0; i < Padding; i++)
				{
					if (pOutBuffer[AES_BLOCK_SIZE - 1 - i] != Padding)
					{
						HavingPadding = false;
						break;
					}
				}
				if (HavingPadding)
				{
					OutLen -= Padding;
				}
			}
		}
		pData += AES_BLOCK_SIZE;
		DataLen -= AES_BLOCK_SIZE;
		pOutBuffer += AES_BLOCK_SIZE;

	}
	return true;
}

bool CCryptTools::TEAEncryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < TEA_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}
	KeyLen = TEA_KEY_SIZE;

	if (pData == NULL || DataLen == 0)
	{
		OutLen = 0;
		return false;
	}

	

	size_t BlockCount = (DataLen / TEA_BLOCK_SIZE) + 1;

	if (pOutBuffer == NULL)
	{
		OutLen = BlockCount * TEA_BLOCK_SIZE;
		return true;
	}


	if (OutLen < BlockCount*TEA_BLOCK_SIZE)
	{
		return false;
	}
	OutLen = BlockCount*TEA_BLOCK_SIZE;

	while (BlockCount)
	{
		if (BlockCount == 1)
		{
			//最后一块，需要填充
			if (DataLen)
				memmove(pOutBuffer, pData, DataLen);
			BYTE Padding = (BYTE)(TEA_BLOCK_SIZE - DataLen);
			for (BYTE i = 0; i < Padding; i++)
			{
				pOutBuffer[DataLen + i] = Padding;
			}
			tea_encrypt((UINT *)pOutBuffer, (UINT *)pKey, Cycle);
			pData += DataLen;
			DataLen = 0;
		}
		else
		{
			memmove(pOutBuffer, pData, TEA_BLOCK_SIZE);
			tea_encrypt((UINT *)pOutBuffer, (UINT *)pKey, Cycle);
			pData += TEA_BLOCK_SIZE;
			DataLen -= TEA_BLOCK_SIZE;
		}
		pOutBuffer += TEA_BLOCK_SIZE;
		BlockCount--;
	}
	return true;
}
bool CCryptTools::TEADecryptECB(const BYTE * pKey, size_t KeyLen, UINT Cycle, const BYTE * pData, size_t DataLen, BYTE * pOutBuffer, size_t& OutLen)
{
	if (pKey == NULL || KeyLen < TEA_KEY_SIZE)
	{
		OutLen = 0;
		return false;
	}
	KeyLen = TEA_KEY_SIZE;

	if (pData == NULL || DataLen == 0 || (DataLen%TEA_BLOCK_SIZE) != 0)
	{
		OutLen = 0;
		return false;
	}

	if (OutLen < DataLen)
	{
		OutLen = DataLen;
		return false;
	}
	OutLen = DataLen;

	while (DataLen)
	{
		memmove(pOutBuffer, pData, TEA_BLOCK_SIZE);
		tea_decrypt((UINT *)pOutBuffer, (UINT *)pKey, Cycle);
		if (DataLen == TEA_BLOCK_SIZE)
		{
			//最后一块，去除填充
			BYTE Padding = pOutBuffer[TEA_BLOCK_SIZE - 1];
			if (Padding <= TEA_BLOCK_SIZE)
			{
				bool HavingPadding = true;
				for (BYTE i = 0; i < Padding; i++)
				{
					if (pOutBuffer[TEA_BLOCK_SIZE - 1 - i] != Padding)
					{
						HavingPadding = false;
						break;
					}
				}
				if (HavingPadding)
				{
					OutLen -= Padding;
				}
			}
		}
		pData += TEA_BLOCK_SIZE;
		DataLen -= TEA_BLOCK_SIZE;
		pOutBuffer += TEA_BLOCK_SIZE;

	}
	return true;
}
