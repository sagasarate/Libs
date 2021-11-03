#pragma once
class CCRCMaker
{
protected:
	unsigned char	m_chCRCHi;
	unsigned char	m_chCRCLo;
	unsigned short	m_wIndex;
public:
	CCRCMaker();
	~CCRCMaker();
	unsigned short CRC16_CCITT(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_CCITT_FALSE(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_XMODEM(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_X25(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_MODBUS(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_IBM(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_MAXIM(const unsigned char *puchMsg, unsigned int usDataLen);
	unsigned short CRC16_USB(const unsigned char *puchMsg, unsigned int usDataLen);
	void CRC16_Fast_Init();
	void CRC16_Fast_Append(const unsigned char * pchMsg, unsigned int wDataLen);
	unsigned short CRC16_Fast_Final();
protected:
	void InvertUint8(unsigned char *DesBuf, const unsigned char *SrcBuf);
	void InvertUint16(unsigned short *DesBuf, const unsigned short *SrcBuf);
};

