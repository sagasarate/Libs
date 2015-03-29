/****************************************************************************/
/*                                                                          */
/*      文件名:    DBValue.h                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "dbinterfaces.h"

namespace DBLib
{

class CDBValue
{
protected:
	int				m_ValueType;
	int				m_DigitSize;
	BYTE *			m_pData;
	int				m_DataSize;
	DECLARE_CLASS_INFO(CDBValue)
public:
	CDBValue(void);

	CDBValue(const CDBValue& Value);
	CDBValue(bool Value);
	CDBValue(char Value);
	CDBValue(unsigned char Value);
	CDBValue(short Value);
	CDBValue(unsigned short Value);
	CDBValue(int Value);
	CDBValue(unsigned int Value);
	CDBValue(long Value);
	CDBValue(unsigned long Value);
	CDBValue(__int64 Value);
	CDBValue(unsigned __int64 Value);
	CDBValue(float Value);
	CDBValue(double Value);
	CDBValue(const char * Value);
	CDBValue(const wchar_t * Value);
	//CDBValue(const SYSTEMTIME& Value);
	//CDBValue(const FILETIME& Value);
	CDBValue(const DB_DATE& Value);
	CDBValue(const DB_TIME& Value);
	CDBValue(const DB_TIMESTAMP& Value);
	CDBValue(const CEasyTime& Value);
	CDBValue(const DB_GUID& Value);


	~CDBValue(void);
	void Destory();



	void SetValue(int ValueType,LPCVOID pData,int DataSize,int DitigalSize);
	void SetEmptyValue(int ValueType,int DataSize,int DitigalSize);
	void SetNULLValue(int ValueType);

	LPVOID GetBuffer();
	int GetType();
	int	GetLength();
	int GetDigitalLength();
	bool IsNull();

	operator bool();
	operator char();
	operator unsigned char();
	operator short();
	operator unsigned short();
	operator int();
	operator unsigned int();
	operator long();
	operator unsigned long();
	operator __int64();
	operator unsigned __int64();
	operator float();
	operator double();
	operator const char *();
	operator const wchar_t *();
	//operator SYSTEMTIME();
	//operator FILETIME();
	operator DB_DATE();
	operator DB_TIME();
	operator DB_TIMESTAMP();
	operator CEasyTime();
	operator DB_GUID();
	operator const void *();

	void operator=(const CDBValue& Value);
	void operator=(bool Value);
	void operator=(char Value);
	void operator=(unsigned char Value);
	void operator=(short Value);
	void operator=(unsigned short Value);
	void operator=(int Value);
	void operator=(unsigned int Value);
	void operator=(long Value);
	void operator=(unsigned long Value);
	void operator=(__int64 Value);
	void operator=(unsigned __int64 Value);
	void operator=(float Value);
	void operator=(double Value);
	void operator=(const char * Value);
	void operator=(const wchar_t * Value);
	//void operator=(const SYSTEMTIME& Value);
	//void operator=(const FILETIME& Value);
	void operator=(const DB_DATE& Value);
	void operator=(const DB_TIME& Value);
	void operator=(const DB_TIMESTAMP& Value);
	void operator=(const CEasyTime& Value);
	void operator=(const DB_GUID& Value);
};

}
