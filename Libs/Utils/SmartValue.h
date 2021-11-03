/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartValue.h                                             */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

	

class CSmartValue 
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;

	
public:	
	static CEasyString::STRING_CODE_PAGE INTERNAL_STRING_CODE_PAGE;	
	enum SMART_VALUE_TYPE
	{
		VT_NULL,
		VT_CHAR,
		VT_UCHAR,
		VT_SHORT,
		VT_USHORT,
		VT_INT,
		VT_UINT,
		VT_BIGINT,
		VT_UBIGINT,
		VT_FLOAT,
		VT_DOUBLE,
		VT_STRING_UTF8,
		VT_STRING_UCS16,
		VT_STRUCT,
		VT_STRING_ANSI,
		VT_BINARY,
		VT_RESERVE2,
		VT_BOOL,
		VT_UNKNOWN,
	};

	static BYTE NULL_DATA;

	CSmartValue(void)
	{	
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData=false;
	}
	CSmartValue(char Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_CHAR,0);
		*this=Value;
	}
	CSmartValue(unsigned char Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UCHAR,0);
		*this=Value;
	}
	CSmartValue(short Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_SHORT,0);
		*this=Value;
	}
	CSmartValue(unsigned short Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_USHORT,0);
		*this=Value;
	}
	CSmartValue(int Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_INT,0);
		*this=Value;
	}
	CSmartValue(unsigned int Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UINT,0);
		*this=Value;
	}
	//CSmartValue(long Value)
	//{
	//	m_pData=NULL;
	//	m_DataLen=0;
	//	m_IsSelfData=true;
	//	Create(VT_INT,0);
	//	*this=Value;
	//}
	//CSmartValue(unsigned long Value)
	//{
	//	m_pData=NULL;
	//	m_DataLen=0;
	//	m_IsSelfData=true;
	//	Create(VT_UINT,0);
	//	*this=Value;
	//}
	CSmartValue(__int64 Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_BIGINT,0);
		*this=Value;
	}
	CSmartValue(unsigned __int64 Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UBIGINT,0);
		*this=Value;
	}	
	CSmartValue(float Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_FLOAT,0);
		*this=Value;
	}	
	CSmartValue(double Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_DOUBLE,0);
		*this=Value;
	}	

	CSmartValue(const char * Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		if(Value)
		{	
			UINT StrLen = (UINT)strlen(Value);
			UINT DataLen = StrLen;
			int ValueType;
			switch (INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				ValueType = VT_STRING_ANSI;
				if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
					DataLen = (UINT)UTF8ToAnsi(Value, StrLen, NULL, 0);
				Create(ValueType, DataLen);
				SetString(Value, StrLen);
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = VT_STRING_UTF8;
				if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
					DataLen = (UINT)AnsiToUTF8(Value, StrLen, NULL, 0);
				Create(ValueType, DataLen);
				SetString(Value, StrLen);
				break;
			case  CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = VT_STRING_UCS16;
				if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
					DataLen = (UINT)AnsiToUnicode(Value, StrLen, NULL, 0) * sizeof(WCHAR);
				else
					DataLen = (UINT)UTF8ToUnicode(Value, StrLen, NULL, 0) * sizeof(WCHAR);
				Create(ValueType, DataLen);
				SetString(Value, StrLen);
				break;
			}
		}
	}

	CSmartValue(const WCHAR * Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		if(Value)
		{
			UINT StrLen = (UINT)wcslen(Value);
			switch (INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				Create(VT_STRING_ANSI, (UINT)UnicodeToAnsi(Value, StrLen, NULL, 0));
				SetString(Value, StrLen);
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				Create(VT_STRING_UTF8, (UINT)UnicodeToUTF8(Value, StrLen, NULL, 0));
				SetString(Value, StrLen);
				break;
			case  CEasyString::STRING_CODE_PAGE_UCS16:
				Create(VT_STRING_UCS16, StrLen * sizeof(WCHAR));
				SetString(Value, StrLen);
				break;
			}
			
		}
	}	
	CSmartValue(bool Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		if(Value)
		{
			Create(VT_BOOL,0);
			*this=Value;
		}
	}	

	CSmartValue(const CSmartValue& Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Attach(Value.GetData(),Value.GetDataLen(),VT_UNKNOWN);
	}	
	CSmartValue(LPVOID pData,UINT DataLen,int ClearType=VT_UNKNOWN)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Attach(pData,DataLen,ClearType);
	}
	
	CSmartValue(int Type,UINT Len)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData=false;
		Create(Type,Len);
	}

	virtual ~CSmartValue(void)
	{
		Destory();
	}

	bool Create(int Type,UINT Len)
	{
		Destory();
		switch(Type)
		{		
		case VT_NULL:			
			break;
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen=sizeof(char)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			m_pData[1]=0;
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen=sizeof(short)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((short *)(m_pData+1))=0;
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen=sizeof(int)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((int *)(m_pData+1))=0;
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen=sizeof(__int64)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((__int64 *)(m_pData+1))=0;
			break;
		case VT_FLOAT:
			m_DataLen=sizeof(float)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((float *)(m_pData+1))=0;
			break;
		case VT_DOUBLE:
			m_DataLen=sizeof(double)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((double *)(m_pData+1))=0;
			break;
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			m_DataLen = (Len + 1) + sizeof(BYTE) + sizeof(UINT);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((UINT *)(m_pData + sizeof(BYTE))) = 0;
			*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
			break;
		case VT_STRING_UCS16:
			m_DataLen = sizeof(WCHAR)*((Len / sizeof(WCHAR)) + 1) + sizeof(BYTE) + sizeof(UINT);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((UINT *)(m_pData + sizeof(BYTE))) = 0;
			*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
			break;
		case VT_STRUCT:
		case VT_BINARY:
			m_DataLen=Len+sizeof(BYTE)+sizeof(UINT);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*((UINT *)(m_pData + sizeof(BYTE))) = 0;
			break;
		case VT_BOOL:
			m_DataLen=sizeof(bool)+sizeof(BYTE);
			m_pData=MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0]=Type;
			*(m_pData + 1) = 0;
			break;		
		default:
			return false;
		}

		m_IsSelfData=true;
		return true;
	}

	bool Attach(void * pData,UINT DataLen,int ClearType=VT_UNKNOWN)
	{
		Destory();
		if (pData == NULL || DataLen == 0)
			return false;
		m_IsSelfData=false;
		m_pData = (BYTE *)pData;

		int DataType=ClearType;
		UINT RealDataLen=0;

		if(ClearType==VT_UNKNOWN)
		{
			DataType=GetType();
			RealDataLen=GetLength();			
		}		

		switch(DataType)
		{
		case VT_NULL:
			m_DataLen = sizeof(BYTE);
			break;
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen=sizeof(char)+sizeof(BYTE);
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen=sizeof(short)+sizeof(BYTE);
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen=sizeof(int)+sizeof(BYTE);
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen=sizeof(__int64)+sizeof(BYTE);
			break;
		case VT_FLOAT:
			m_DataLen=sizeof(float)+sizeof(BYTE);
			break;
		case VT_DOUBLE:
			m_DataLen=sizeof(double)+sizeof(BYTE);
			break;
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			m_DataLen = RealDataLen + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			break;
		case VT_STRING_UCS16:
			m_DataLen = RealDataLen + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
			break;
		case VT_STRUCT:
		case VT_BINARY:
			m_DataLen=RealDataLen+sizeof(BYTE)+sizeof(UINT);
			break;		
		case VT_BOOL:
			m_DataLen = sizeof(BYTE) + sizeof(BYTE);
			break;		
		default:
			return false;
		}

		if(DataLen<m_DataLen)
		{
			Destory();
			return false;
		}

		m_DataLen=DataLen;

		if(ClearType==VT_UNKNOWN)
		{
			switch(DataType)
			{			
			case VT_STRING_ANSI:
			case VT_STRING_UTF8:
				*((char *)(m_pData + RealDataLen + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRING_UCS16:
				*((WCHAR *)(m_pData + RealDataLen + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;					
			}
		}
		else
		{
			switch(ClearType)
			{
			case VT_NULL:
				m_pData[0] = ClearType;
				break;
			case VT_CHAR:
			case VT_UCHAR:
				m_pData[0]=ClearType;
				m_pData[1]=0;
				break;
			case VT_SHORT:
			case VT_USHORT:
				m_pData[0]=ClearType;
				*((short *)(m_pData+1))=0;
				break;
			case VT_INT:
			case VT_UINT:
				m_pData[0]=ClearType;
				*((int *)(m_pData+1))=0;
				break;
			case VT_BIGINT:
			case VT_UBIGINT:
				m_pData[0]=ClearType;
				*((__int64 *)(m_pData+1))=0;
				break;
			case VT_FLOAT:
				m_pData[0]=ClearType;
				*((float *)(m_pData+1))=0;
				break;
			case VT_DOUBLE:
				m_pData[0]=ClearType;
				*((double *)(m_pData+1))=0;
				break;
			case VT_STRING_ANSI:
			case VT_STRING_UTF8:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				*((char *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
				break;
			case VT_STRING_UCS16:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				*((WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT)))=0;
				break;	
			case VT_STRUCT:
			case VT_BINARY:
				m_pData[0]=ClearType;
				*((UINT *)(m_pData+sizeof(BYTE)))=0;
				break;				
			case VT_BOOL:
				m_pData[0]=ClearType;
				*(m_pData + 1) = 0;
				break;			
			}
		}


		return true;
	}

	bool CloneFrom(const CSmartValue& Value)
	{
		Destory();
		if (Value.GetData() && Value.GetDataLen())
		{
			m_IsSelfData = true;
			m_DataLen = Value.GetDataLen();
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			memcpy(m_pData, Value.GetData(), Value.GetDataLen());
		}
		return true;
	}

	void Destory()
	{
		if(m_IsSelfData)
		{
			SAFE_DELETE_ARRAY(m_pData);
		}
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData = false;
	}

	int GetType() const
	{
		if(m_pData==NULL)
			return VT_UNKNOWN;
		return m_pData[0];
	}
	UINT GetLength() const
	{
		switch(GetType())
		{		
		case VT_NULL:
			return 0;
		case VT_CHAR:
		case VT_UCHAR:
			return sizeof(char);
		case VT_SHORT:
		case VT_USHORT:
			return sizeof(short);
		case VT_INT:
		case VT_UINT:
			return sizeof(int);
		case VT_BIGINT:
		case VT_UBIGINT:
			return sizeof(__int64);
		case VT_FLOAT:
			return sizeof(float);
		case VT_DOUBLE:
			return sizeof(double);
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return *((UINT *)(m_pData+sizeof(BYTE)));
		case VT_STRING_UCS16:
			return *((UINT *)(m_pData+sizeof(BYTE)));
		case VT_STRUCT:
		case VT_BINARY:
			return *((UINT *)(m_pData+sizeof(BYTE)));		
		case VT_BOOL:
			return sizeof(BYTE);		
		}
		return 0;
	}
	UINT GetDataLen()  const
	{		
		switch(GetType())
		{		
		case VT_NULL:
			return sizeof(BYTE);
		case VT_CHAR:
		case VT_UCHAR:
			return sizeof(char)+sizeof(BYTE);
		case VT_SHORT:
		case VT_USHORT:
			return sizeof(short)+sizeof(BYTE);
		case VT_INT:
		case VT_UINT:
			return sizeof(int)+sizeof(BYTE);
		case VT_BIGINT:
		case VT_UBIGINT:
			return sizeof(__int64)+sizeof(BYTE);
		case VT_FLOAT:
			return sizeof(float)+sizeof(BYTE);
		case VT_DOUBLE:
			return sizeof(double)+sizeof(BYTE);
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return *((UINT *)(m_pData + sizeof(BYTE))) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		case VT_STRING_UCS16:
			return *((UINT *)(m_pData + sizeof(BYTE))) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		case VT_STRUCT:
		case VT_BINARY:
			return *((UINT *)(m_pData+sizeof(BYTE)))+sizeof(BYTE)+sizeof(UINT);		
		case VT_BOOL:
			return sizeof(BYTE) + sizeof(BYTE);		
		}
		return 0;
	}
	LPVOID GetData() const
	{
		return m_pData;
	}
	LPVOID GetValueData() const 
	{
		switch (GetType())
		{		
		case VT_CHAR:
		case VT_UCHAR:
			return m_pData + sizeof(BYTE);
		case VT_SHORT:
		case VT_USHORT:
			return m_pData + sizeof(BYTE);
		case VT_INT:
		case VT_UINT:
			return m_pData + sizeof(BYTE);
		case VT_BIGINT:
		case VT_UBIGINT:
			return m_pData + sizeof(BYTE);
		case VT_FLOAT:
			return m_pData + sizeof(BYTE);
		case VT_DOUBLE:
			return m_pData + sizeof(BYTE);
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return m_pData + sizeof(BYTE) + sizeof(UINT);
		case VT_STRING_UCS16:
			return m_pData + sizeof(BYTE) + sizeof(UINT);
		case VT_STRUCT:
		case VT_BINARY:
			return m_pData + sizeof(BYTE) + sizeof(UINT);		
		case VT_BOOL:
			return m_pData + sizeof(BYTE);
		}
		return NULL;
	}

	operator char() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (char)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (char)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (char)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (char)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (char)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (char)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (char)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (char)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (char)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (char)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (*(m_pData + 1)) != 0;
		}
		return 0;
	}
	operator unsigned char() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned char)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned char)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned char)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned char)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned char)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned char)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned char)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned char)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned char)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned char)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned char)(*(m_pData+1));
		}
		return 0;
	}
	operator short() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (short)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (short)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (short)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (short)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (short)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (short)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (short)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (short)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (short)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (short)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (short)(*(m_pData + 1));
		}
		return 0;
	}
	operator unsigned short() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned short)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned short)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned short)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned short)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned short)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned short)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned short)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned short)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned short)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned short)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned short)(*(m_pData + 1));
		}
		return 0;
	}
	operator int() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (int)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (int)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (int)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (int)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (int)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (int)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (int)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (int)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (int)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (int)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (int)(*(m_pData + 1));
		}
		return 0;
	}
	operator unsigned int() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned int)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned int)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned int)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned int)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned int)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned int)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned int)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned int)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned int)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned int)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned int)(*(m_pData + 1));
		}
		return 0;
	}
	//operator long() const
	//{
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		return (long)(*((char *)(m_pData+1)));
	//	case VT_UCHAR:
	//		return (long)(*((unsigned char *)(m_pData+1)));
	//	case VT_SHORT:
	//		return (long)(*((short *)(m_pData+1)));
	//	case VT_USHORT:
	//		return (long)(*((unsigned short *)(m_pData+1)));
	//	case VT_INT:
	//		return (long)(*((int *)(m_pData+1)));
	//	case VT_UINT:
	//		return (long)(*((unsigned int *)(m_pData+1)));
	//	case VT_BIGINT:
	//		return (long)(*((__int64 *)(m_pData+1)));
	//	case VT_UBIGINT:
	//		return (long)(*((unsigned __int64 *)(m_pData+1)));
	//	case VT_FLOAT:
	//		return (long)(*((float *)(m_pData+1)));
	//	case VT_DOUBLE:
	//		return (long)(*((double *)(m_pData+1)));
	//	case VT_BOOL:
	//		return (long)(*(m_pData+1));
	//	}
	//	return 0;
	//}
	//operator unsigned long() const
	//{
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		return (unsigned long)(*((char *)(m_pData+1)));
	//	case VT_UCHAR:
	//		return (unsigned long)(*((unsigned char *)(m_pData+1)));
	//	case VT_SHORT:
	//		return (unsigned long)(*((short *)(m_pData+1)));
	//	case VT_USHORT:
	//		return (unsigned long)(*((unsigned short *)(m_pData+1)));
	//	case VT_INT:
	//		return (unsigned long)(*((int *)(m_pData+1)));
	//	case VT_UINT:
	//		return (unsigned long)(*((unsigned int *)(m_pData+1)));
	//	case VT_BIGINT:
	//		return (unsigned long)(*((__int64 *)(m_pData+1)));
	//	case VT_UBIGINT:
	//		return (unsigned long)(*((unsigned __int64 *)(m_pData+1)));
	//	case VT_FLOAT:
	//		return (unsigned long)(*((float *)(m_pData+1)));
	//	case VT_DOUBLE:
	//		return (unsigned long)(*((double *)(m_pData+1)));
	//	case VT_BOOL:
	//		return (unsigned long)(*(m_pData+1));
	//	}
	//	return 0;
	//}
	operator __int64() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (__int64)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (__int64)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (__int64)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (__int64)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (__int64)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (__int64)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (__int64)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (__int64)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (__int64)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (__int64)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (__int64)(*(m_pData + 1));
		}
		return 0;
	}
	operator unsigned __int64() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (unsigned __int64)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (unsigned __int64)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (unsigned __int64)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (unsigned __int64)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (unsigned __int64)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (unsigned __int64)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (unsigned __int64)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (unsigned __int64)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (unsigned __int64)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (unsigned __int64)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (unsigned __int64)(*(m_pData + 1));
		}
		return 0;
	}

	operator float() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (float)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (float)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (float)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (float)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (float)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (float)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (float)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (float)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (float)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (float)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (float)(*(m_pData + 1));
		}
		return 0;
	}

	operator double() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (double)(*((char *)(m_pData+1)));
		case VT_UCHAR:
			return (double)(*((unsigned char *)(m_pData+1)));
		case VT_SHORT:
			return (double)(*((short *)(m_pData+1)));
		case VT_USHORT:
			return (double)(*((unsigned short *)(m_pData+1)));
		case VT_INT:
			return (double)(*((int *)(m_pData+1)));
		case VT_UINT:
			return (double)(*((unsigned int *)(m_pData+1)));
		case VT_BIGINT:
			return (double)(*((__int64 *)(m_pData+1)));
		case VT_UBIGINT:
			return (double)(*((unsigned __int64 *)(m_pData+1)));
		case VT_FLOAT:
			return (double)(*((float *)(m_pData+1)));
		case VT_DOUBLE:
			return (double)(*((double *)(m_pData+1)));
		case VT_BOOL:
			return (double)(*(m_pData + 1));
		}
		return 0;
	}

	operator const char *() const
	{
		switch(GetType())
		{	
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return (const char *)(m_pData+sizeof(BYTE)+sizeof(UINT));
		}		
		return "";
	}

	operator const WCHAR *() const
	{
		static WCHAR pEmptyStr[1]={0};
		switch(GetType())
		{			
		case VT_STRING_UCS16:
			return (WCHAR *)(m_pData+sizeof(BYTE)+sizeof(UINT));
		}		
		return pEmptyStr;
	}
	operator const unsigned char *() const
	{
		switch (GetType())
		{
		case VT_BINARY:
			return (m_pData + sizeof(BYTE) + sizeof(UINT));
		}
		return NULL;
	}
	operator CEasyString() const
	{
		CEasyString String;
		switch (GetType())
		{
		case VT_STRING_ANSI:
			String.SetString((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_ANSI);
		case VT_STRING_UTF8:
			String.SetString((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UTF8);
		case VT_STRING_UCS16:
			String.SetString((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UCS16);
		}
		return String;
	}


	operator bool() const
	{
		switch(GetType())
		{		
		case VT_CHAR:
			return (*((char *)(m_pData+1)))!=0;
		case VT_UCHAR:
			return (*((unsigned char *)(m_pData+1)))!=0;
		case VT_SHORT:
			return (*((short *)(m_pData+1)))!=0;
		case VT_USHORT:
			return (*((unsigned short *)(m_pData+1)))!=0;
		case VT_INT:
			return (*((int *)(m_pData+1)))!=0;
		case VT_UINT:
			return (*((unsigned int *)(m_pData+1)))!=0;
		case VT_BIGINT:
			return (*((__int64 *)(m_pData+1)))!=0;
		case VT_UBIGINT:
			return (*((unsigned __int64 *)(m_pData+1)))!=0;
		case VT_FLOAT:
			return (*((float *)(m_pData+1)))!=0;
		case VT_DOUBLE:
			return (*((double *)(m_pData+1)))!=0;
		case VT_BOOL:
			return (*(m_pData + 1)) != 0;
		}
		return false;
	}


	bool GetString(CEasyString& String) const
	{
		switch (GetType())
		{
		case VT_STRING_ANSI:
			String.SetString((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_ANSI);
			return true;
		case VT_STRING_UTF8:
			String.SetString((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UTF8);
			return true;
		case VT_STRING_UCS16:
			String.SetString((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UCS16);
			return true;
		}
		return false;
	}

	CEasyString GetString() const
	{
		CEasyString String;
		GetString(String);
		return String;
	}

	bool GetStringRef(CEasyString& String) const
	{
		switch (GetType())
		{
		case VT_STRING_ANSI:
			String.SetStringRef((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_ANSI);
			return true;
		case VT_STRING_UTF8:
			String.SetStringRef((const char *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UTF8);
			return true;
		case VT_STRING_UCS16:
			String.SetStringRef((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength(), CEasyString::STRING_CODE_PAGE_UCS16);
			return true;
		}
		return false;
	}

	CEasyString GetStringRef() const
	{
		CEasyString String;
		GetStringRef(String);
		return String;
	}
	
	void operator=(const CSmartValue& Value)
	{		
		Attach(Value.GetData(),Value.GetDataLen(),VT_UNKNOWN);
	}
	void SetValue(const CSmartValue& Value)
	{
		if(m_pData==NULL)
			return;
		switch(Value.GetType())
		{		
		case VT_CHAR:
			*this=(char)Value;
			break;
		case VT_UCHAR:
			*this=(unsigned char)Value;
			break;
		case VT_SHORT:
			*this=(short)Value;
			break;
		case VT_USHORT:
			*this=(unsigned short)Value;
			break;
		case VT_INT:
			*this=(int)Value;
			break;
		case VT_UINT:
			*this=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*this=(__int64)Value;
			break;
		case VT_UBIGINT:
			*this=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*this=(float)Value;
			break;
		case VT_DOUBLE:
			*this=(double)Value;
			break;
		case VT_STRING_ANSI:
			SetString((LPCSTR)Value, Value.GetLength(), CEasyString::STRING_CODE_PAGE_ANSI);
			break;
		case VT_STRING_UTF8:
			SetString((LPCSTR)Value, Value.GetLength(), CEasyString::STRING_CODE_PAGE_UTF8);
			break;
		case VT_STRING_UCS16:
			SetString((LPCWSTR)Value, Value.GetLength());
			break;
		case VT_STRUCT:
			if (GetType() == VT_STRUCT)
			{
				UINT CopyLen = Value.GetLength();
				if (CopyLen > GetLength())
					CopyLen = GetLength();
				memcpy(GetValueData(), Value.GetValueData(), CopyLen);
			}
			break;
		case VT_BINARY:
			if (GetType() == VT_BINARY)
			{
				UINT CopyLen = Value.GetLength();
				if (CopyLen > GetLength())
					CopyLen = GetLength();
				memcpy(GetValueData(), Value.GetValueData(), CopyLen);
			}
			break;
		case VT_BOOL:
			*this=(bool)Value;
			break;
		}
	}
	void operator=(char Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(unsigned char Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(short Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(unsigned short Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}	
	void operator=(int Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(unsigned int Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	//void operator=(long Value)
	//{
	//	if(m_pData==NULL)
	//		return;
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		*((char *)(m_pData+1))=(char)Value;
	//		break;
	//	case VT_UCHAR:
	//		*((unsigned char *)(m_pData+1))=(unsigned char)Value;
	//		break;
	//	case VT_SHORT:
	//		*((short *)(m_pData+1))=(short)Value;
	//		break;
	//	case VT_USHORT:
	//		*((unsigned short *)(m_pData+1))=(unsigned short)Value;
	//		break;
	//	case VT_INT:
	//		*((int *)(m_pData+1))=(int)Value;
	//		break;
	//	case VT_UINT:
	//		*((unsigned int *)(m_pData+1))=(unsigned int)Value;
	//		break;
	//	case VT_BIGINT:
	//		*((__int64 *)(m_pData+1))=(__int64)Value;
	//		break;
	//	case VT_UBIGINT:
	//		*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
	//		break;
	//	case VT_FLOAT:
	//		*((float *)(m_pData+1))=(float)Value;
	//		break;
	//	case VT_DOUBLE:
	//		*((double *)(m_pData+1))=(double)Value;
	//		break;
	//	case VT_BOOL:
	//		(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
	//		break;
	//	}
	//}
	//void operator=(unsigned long Value)
	//{
	//	if(m_pData==NULL)
	//		return;
	//	switch(GetType())
	//	{		
	//	case VT_CHAR:
	//		*((char *)(m_pData+1))=(char)Value;
	//		break;
	//	case VT_UCHAR:
	//		*((unsigned char *)(m_pData+1))=(unsigned char)Value;
	//		break;
	//	case VT_SHORT:
	//		*((short *)(m_pData+1))=(short)Value;
	//		break;
	//	case VT_USHORT:
	//		*((unsigned short *)(m_pData+1))=(unsigned short)Value;
	//		break;
	//	case VT_INT:
	//		*((int *)(m_pData+1))=(int)Value;
	//		break;
	//	case VT_UINT:
	//		*((unsigned int *)(m_pData+1))=(unsigned int)Value;
	//		break;
	//	case VT_BIGINT:
	//		*((__int64 *)(m_pData+1))=(__int64)Value;
	//		break;
	//	case VT_UBIGINT:
	//		*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
	//		break;
	//	case VT_FLOAT:
	//		*((float *)(m_pData+1))=(float)Value;
	//		break;
	//	case VT_DOUBLE:
	//		*((double *)(m_pData+1))=(double)Value;
	//		break;
	//	case VT_BOOL:
	//		(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
	//		break;
	//	}
	//}
	void operator=(__int64 Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(unsigned __int64 Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}
	void operator=(float Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}

	void operator=(double Value)
	{
		if(m_pData==NULL)
			return;
		switch(GetType())
		{		
		case VT_CHAR:
			*((char *)(m_pData+1))=(char)Value;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData+1))=(unsigned char)Value;
			break;
		case VT_SHORT:
			*((short *)(m_pData+1))=(short)Value;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData+1))=(unsigned short)Value;
			break;
		case VT_INT:
			*((int *)(m_pData+1))=(int)Value;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData+1))=(unsigned int)Value;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData+1))=(__int64)Value;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData+1))=(unsigned __int64)Value;
			break;
		case VT_FLOAT:
			*((float *)(m_pData+1))=(float)Value;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData+1))=(double)Value;
			break;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			break;
		}
	}

	void operator=(const char * Value)
	{
		Destory();
		if(Value)
			SetString(Value,(UINT)strlen(Value));
	}

	void operator=(const WCHAR * Value)
	{
		Destory();
		if(Value)
			SetString(Value,(UINT)wcslen(Value));
	}

	void operator=(const CEasyString& Value)
	{
		Destory();
		SetString(Value,(UINT)Value.GetLength());
	}	

	void operator=(bool Value)
	{
		if (m_pData == NULL)
			return;
		switch (GetType())
		{
		case VT_CHAR:
			*((char *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_UCHAR:
			*((unsigned char *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_SHORT:
			*((short *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_USHORT:
			*((unsigned short *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_INT:
			*((int *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_UINT:
			*((unsigned int *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_BIGINT:
			*((__int64 *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_UBIGINT:
			*((unsigned __int64 *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_FLOAT:
			*((float *)(m_pData + 1)) = Value ? 1.0f : 0.0f;
			break;
		case VT_DOUBLE:
			*((double *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		case VT_BOOL:
			*((bool *)(m_pData + 1)) = Value ? 1 : 0;
			break;
		}
	}

	bool SetString(const char * pStr, UINT Len, CEasyString::STRING_CODE_PAGE CodePage = CEasyString::STRING_CODE_PAGE_AUTO)
	{
		if (CodePage == CEasyString::STRING_CODE_PAGE_AUTO)
			CodePage = CEasyString::SYSTEM_STRING_CODE_PAGE;
		switch (GetType())
		{		
		case VT_STRING_ANSI:
			if (CodePage == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				UINT DestStrLen = (UINT)UTF8ToAnsi(pStr, Len, NULL, 0);
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				UTF8ToAnsi(pStr, Len, (char *)(m_pData + HeaderLen), DestStrLen);
				*((char *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			else 
			{
				UINT DestStrLen = Len;
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				memcpy(m_pData + HeaderLen, pStr, DestStrLen);
				*((char *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			return true;
		case VT_STRING_UTF8:
			if (CodePage == CEasyString::STRING_CODE_PAGE_ANSI)
			{
				UINT DestStrLen = (UINT)AnsiToUTF8(pStr, Len, NULL, 0);
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				UTF8ToAnsi(pStr, Len, (char *)(m_pData + HeaderLen), DestStrLen);
				*((char *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			else
			{
				UINT DestStrLen = Len;
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				memcpy(m_pData + HeaderLen, pStr, DestStrLen);
				*((char *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			return true;
		case VT_STRING_UCS16:
			if (CodePage == CEasyString::STRING_CODE_PAGE_ANSI)
			{
				UINT DestStrLen = (UINT)AnsiToUnicode(pStr, Len, NULL, 0);
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = (m_DataLen - HeaderLen - sizeof(WCHAR)) / sizeof(WCHAR);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				AnsiToUnicode(pStr, Len, (WCHAR *)(m_pData + HeaderLen), DestStrLen);
				*((WCHAR *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen * sizeof(WCHAR);
			}
			else
			{
				UINT DestStrLen = (UINT)UTF8ToUnicode(pStr, Len, NULL, 0);
				UINT HeaderLen = (UINT)(sizeof(BYTE) - sizeof(UINT));
				UINT MaxLen = (m_DataLen - HeaderLen - sizeof(WCHAR)) / sizeof(WCHAR);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				UTF8ToUnicode(pStr, Len, (WCHAR *)(m_pData + HeaderLen), DestStrLen);
				*((WCHAR *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen * sizeof(WCHAR);
			}
			return true;
		}
		return false;
	}

	bool SetString(const WCHAR * pStr,UINT Len)
	{
		CEasyString::STRING_CODE_PAGE CodePage;
		switch (GetType())
		{
		case VT_STRING_ANSI:
			CodePage = CEasyString::STRING_CODE_PAGE_ANSI;
		case VT_STRING_UTF8:
			CodePage = CEasyString::STRING_CODE_PAGE_UTF8;
		case VT_STRING_UCS16:
			CodePage = CEasyString::STRING_CODE_PAGE_UCS16;
		default:
			return false;
		}
		switch (CodePage)
		{
		case CEasyString::STRING_CODE_PAGE_ANSI:
			{
				UINT DestStrLen = (UINT)UnicodeToAnsi(pStr, Len, NULL, 0);
				UINT MaxLen = m_DataLen - sizeof(BYTE) - sizeof(UINT) - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
				UnicodeToAnsi(pStr, Len, (char *)m_pData + sizeof(BYTE) + sizeof(UINT), DestStrLen);
				*((char *)(m_pData + sizeof(BYTE) + sizeof(UINT) + DestStrLen)) = 0;
			}
			return true;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			{
				UINT DestStrLen = (UINT)UnicodeToUTF8(pStr, Len, NULL, 0);
				UINT MaxLen = m_DataLen - sizeof(BYTE) - sizeof(UINT) - sizeof(char);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen;
				UnicodeToUTF8(pStr, Len, (char *)m_pData + sizeof(BYTE) + sizeof(UINT), DestStrLen);
				*((char *)(m_pData + sizeof(BYTE) + sizeof(UINT) + DestStrLen)) = 0;
			}
			return true;
		case CEasyString::STRING_CODE_PAGE_UCS16:
			{
				UINT MaxLen = (m_DataLen - sizeof(BYTE) - sizeof(UINT) - sizeof(WCHAR)) / sizeof(WCHAR);
				if (Len > MaxLen)
					Len = MaxLen;
				*((UINT *)(m_pData + sizeof(BYTE))) = Len * sizeof(WCHAR);
				memcpy(m_pData + sizeof(BYTE) + sizeof(UINT), pStr, Len * sizeof(WCHAR));
				*((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT) + Len * sizeof(WCHAR))) = 0;
			}
			return true;
		}
		return false;
	}
	bool SetBinary(const void * pData, UINT DataLen)
	{
		if (GetType() == VT_BINARY)
		{
			UINT MaxLen = m_DataLen - sizeof(BYTE) - sizeof(UINT);
			if (DataLen > MaxLen)
				DataLen = MaxLen;
			*((UINT *)(m_pData + sizeof(BYTE))) = DataLen;
			memcpy(m_pData + sizeof(BYTE) + sizeof(UINT), pData, DataLen);
			return true;
		}
		return false;
	}
	bool IsNull()
	{
		return GetType() == VT_NULL;
	}

	static int GetTypeFromData(LPCVOID pData,UINT DataSize);
	
};
