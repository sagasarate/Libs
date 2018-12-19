/****************************************************************************/
/*                                                                          */
/*      文件名:    HashMD5.h                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CHashMD5 
{
protected:
	/* typedef a 32 bit type */
	typedef unsigned int UINT4;

	/* Data structure for MD5 (Message Digest) computation */
	typedef struct {
		UINT4 i[2];                   /* number of _bits_ handled mod 2^64 */
		UINT4 buf[4];                                    /* scratch buffer */
		unsigned char in[64];                              /* input buffer */
		unsigned char digest[16];     /* actual digest after MD5Final call */
	} MD5_CTX;

	MD5_CTX m_mdContext;

public:
	CHashMD5(void);
	~CHashMD5(void);
	
	void Init();
	void AddData(BYTE * inBuf, size_t inLen);
	void MD5Final(void);
	CEasyString GetHashCodeString(void);
	void GetHashCode(BYTE * pBuff);

protected:
	void Transform(UINT4 * buf, UINT4 * in);
};
