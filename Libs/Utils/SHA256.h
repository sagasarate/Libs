#pragma once


class CSHA256
{
public:
	CSHA256();
	~CSHA256();

	static LPCTSTR SHA256(const BYTE * pData, size_t DataLen, LPTSTR pOutBuff);

	static CEasyString SHA256(const BYTE * pData, size_t DataLen);
};

