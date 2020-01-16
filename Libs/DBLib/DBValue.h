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


namespace DBLib
{

class CDBValue
{
protected:
	int				m_ValueType;
	int				m_DigitSize;
	BYTE *			m_pData;
	int				m_DataSize;
	DECLARE_CLASS_INFO_STATIC(CDBValue)
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
	//CDBValue(long Value);
	//CDBValue(unsigned long Value);
	CDBValue(__int64 Value);
	CDBValue(unsigned __int64 Value);
	CDBValue(float Value);
	CDBValue(double Value);
	CDBValue(const char * Value);
	CDBValue(const WCHAR * Value);
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

	operator bool() const;
	operator char() const;
	operator unsigned char() const;
	operator short() const;
	operator unsigned short() const;
	operator int() const;
	operator unsigned int() const;
	//operator long() const;
	//operator unsigned long() const;
	operator __int64() const;
	operator unsigned __int64() const;
	operator float() const;
	operator double() const;
	operator const char *() const;
	operator const WCHAR *() const;
	//operator SYSTEMTIME();
	//operator FILETIME();
	operator DB_DATE() const;
	operator DB_TIME() const;
	operator DB_TIMESTAMP() const;
	operator CEasyTime() const;
	operator DB_GUID() const;
	operator const void *() const;

	void operator=(const CDBValue& Value);
	void operator=(bool Value);
	void operator=(char Value);
	void operator=(unsigned char Value);
	void operator=(short Value);
	void operator=(unsigned short Value);
	void operator=(int Value);
	void operator=(unsigned int Value);
	//void operator=(long Value);
	//void operator=(unsigned long Value);
	void operator=(__int64 Value);
	void operator=(unsigned __int64 Value);
	void operator=(float Value);
	void operator=(double Value);
	void operator=(const char * Value);
	void operator=(const WCHAR * Value);
	//void operator=(const SYSTEMTIME& Value);
	//void operator=(const FILETIME& Value);
	void operator=(const DB_DATE& Value);
	void operator=(const DB_TIME& Value);
	void operator=(const DB_TIMESTAMP& Value);
	void operator=(const CEasyTime& Value);
	void operator=(const DB_GUID& Value);
};

}
