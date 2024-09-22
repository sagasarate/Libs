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

enum class SST_PAIR
{
	KEY = 101,
	DATA = 102,
};

const WORD VARIED_MEMEBR_ID = 52354;

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
		VT_ARRAY,
		VT_BOOL,
		VT_UNKNOWN,
	};

	static BYTE NULL_DATA;

	static int GetTypeFromData(LPCVOID pData, UINT DataSize);
	static UINT GetTypeLen(BYTE Type);

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
		SetValue(Value);
	}
	CSmartValue(unsigned char Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UCHAR,0);
		SetValue(Value);
	}
	CSmartValue(short Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_SHORT,0);
		SetValue(Value);
	}
	CSmartValue(unsigned short Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_USHORT,0);
		SetValue(Value);
	}
	CSmartValue(int Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_INT,0);
		SetValue(Value);
	}
	CSmartValue(unsigned int Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UINT,0);
		SetValue(Value);
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
		SetValue(Value);
	}
	CSmartValue(unsigned __int64 Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_UBIGINT,0);
		SetValue(Value);
	}	
	CSmartValue(float Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_FLOAT,0);
		SetValue(Value);
	}	
	CSmartValue(double Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Create(VT_DOUBLE,0);
		SetValue(Value);
	}	

	CSmartValue(const char * Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData = false;
		*this = Value;
	}

	CSmartValue(const WCHAR * Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData = false;
		*this = Value;
	}	
	CSmartValue(bool Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		if(Value)
		{
			Create(VT_BOOL,0);
			SetValue(Value);
		}
	}
	CSmartValue(const CVariedValue& Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData = false;
		*this = Value;
	}
	CSmartValue(const CEasyBuffer& Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData = false;
		Create(VT_BINARY, (UINT)Value.GetUsedSize());
		SetValue(Value);
	}
	CSmartValue(const CSmartValue& Value)
	{
		m_pData = &NULL_DATA;
		m_DataLen = sizeof(NULL_DATA);
		m_IsSelfData= false;
		Attach((void*)Value.GetData(),Value.GetDataLen(),VT_UNKNOWN);
	}
	CSmartValue(CSmartValue&& Value)  noexcept
	{
		m_pData = Value.m_pData;
		m_DataLen = Value.m_DataLen;
		m_IsSelfData = Value.m_IsSelfData;
		Value.m_pData = &NULL_DATA;
		Value.m_DataLen = sizeof(NULL_DATA);
		Value.m_IsSelfData = false;
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

	bool Create(int Type, UINT Len)
	{
		Destory();
		switch (Type)
		{
		case VT_NULL:
			break;
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen = sizeof(char) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			m_pData[1] = 0;
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen = sizeof(short) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((short*)(m_pData + 1)) = 0;
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen = sizeof(int) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((int*)(m_pData + 1)) = 0;
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen = sizeof(__int64) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((__int64*)(m_pData + 1)) = 0;
			break;
		case VT_FLOAT:
			m_DataLen = sizeof(float) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((float*)(m_pData + 1)) = 0;
			break;
		case VT_DOUBLE:
			m_DataLen = sizeof(double) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((double*)(m_pData + 1)) = 0;
			break;
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			m_DataLen = (Len + 1) + sizeof(BYTE) + sizeof(UINT);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
			*((char*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
			break;
		case VT_STRING_UCS16:
			m_DataLen = sizeof(WCHAR) * ((Len / sizeof(WCHAR)) + 1) + sizeof(BYTE) + sizeof(UINT);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
			*((WCHAR*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
			break;
		case VT_STRUCT:
		case VT_BINARY:
		case VT_ARRAY:
			m_DataLen = Len + sizeof(BYTE) + sizeof(UINT);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
			break;
		case VT_BOOL:
			m_DataLen = sizeof(bool) + sizeof(BYTE);
			m_pData = MONITORED_NEW_ARRAY(_T("CSmartValue"), BYTE, m_DataLen);
			m_pData[0] = Type;
			*(m_pData + 1) = 0;
			break;
		default:
			return false;
		}

		m_IsSelfData = true;
		return true;
	}

	bool Attach(void* pData, UINT DataLen, int ClearType = VT_UNKNOWN)
	{
		Destory();
		if (pData == NULL || DataLen == 0 || ClearType > VT_UNKNOWN)
			return false;
		m_IsSelfData = false;
		m_pData = (BYTE*)pData;

		int DataType = ClearType;
		UINT RealDataLen = 0;

		if (ClearType == VT_UNKNOWN)
		{
			m_DataLen = DataLen;
			DataType = GetType();
			RealDataLen = GetLength();
		}

		switch (DataType)
		{
		case VT_NULL:
			m_DataLen = sizeof(BYTE);
			break;
		case VT_CHAR:
		case VT_UCHAR:
			m_DataLen = sizeof(char) + sizeof(BYTE);
			break;
		case VT_SHORT:
		case VT_USHORT:
			m_DataLen = sizeof(short) + sizeof(BYTE);
			break;
		case VT_INT:
		case VT_UINT:
			m_DataLen = sizeof(int) + sizeof(BYTE);
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			m_DataLen = sizeof(__int64) + sizeof(BYTE);
			break;
		case VT_FLOAT:
			m_DataLen = sizeof(float) + sizeof(BYTE);
			break;
		case VT_DOUBLE:
			m_DataLen = sizeof(double) + sizeof(BYTE);
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
		case VT_ARRAY:
			m_DataLen = RealDataLen + sizeof(BYTE) + sizeof(UINT);
			break;
		case VT_BOOL:
			m_DataLen = sizeof(BYTE) + sizeof(BYTE);
			break;
		default:
			return false;
		}

		if (DataLen < m_DataLen)
		{
			Destory();
			return false;
		}

		m_DataLen = DataLen;

		if (ClearType == VT_UNKNOWN)
		{
			switch (DataType)
			{
			case VT_STRING_ANSI:
			case VT_STRING_UTF8:
				*((char*)(m_pData + RealDataLen + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRING_UCS16:
				*((WCHAR*)(m_pData + RealDataLen + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			}
		}
		else
		{
			switch (ClearType)
			{
			case VT_NULL:
				m_pData[0] = ClearType;
				break;
			case VT_CHAR:
			case VT_UCHAR:
				m_pData[0] = ClearType;
				m_pData[1] = 0;
				break;
			case VT_SHORT:
			case VT_USHORT:
				m_pData[0] = ClearType;
				*((short*)(m_pData + 1)) = 0;
				break;
			case VT_INT:
			case VT_UINT:
				m_pData[0] = ClearType;
				*((int*)(m_pData + 1)) = 0;
				break;
			case VT_BIGINT:
			case VT_UBIGINT:
				m_pData[0] = ClearType;
				*((__int64*)(m_pData + 1)) = 0;
				break;
			case VT_FLOAT:
				m_pData[0] = ClearType;
				*((float*)(m_pData + 1)) = 0;
				break;
			case VT_DOUBLE:
				m_pData[0] = ClearType;
				*((double*)(m_pData + 1)) = 0;
				break;
			case VT_STRING_ANSI:
			case VT_STRING_UTF8:
				m_pData[0] = ClearType;
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				*((char*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRING_UCS16:
				m_pData[0] = ClearType;
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				*((WCHAR*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRUCT:
			case VT_BINARY:
			case VT_ARRAY:
				m_pData[0] = ClearType;
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				break;
			case VT_BOOL:
				m_pData[0] = ClearType;
				*(m_pData + 1) = 0;
				break;
			}
		}

		return true;
	}

	bool ChangeType(int Type, UINT Len = 0)
	{
		UINT NeedSize = CSmartValue::GetDataLenByType(Type, Len);
		if (NeedSize && NeedSize <= m_DataLen)
		{
			m_pData[0] = Type;

			switch (Type)
			{
			case VT_NULL:
				break;
			case VT_CHAR:
			case VT_UCHAR:
				m_pData[1] = 0;
				break;
			case VT_SHORT:
			case VT_USHORT:
				*((short*)(m_pData + 1)) = 0;
				break;
			case VT_INT:
			case VT_UINT:
				*((int*)(m_pData + 1)) = 0;
				break;
			case VT_BIGINT:
			case VT_UBIGINT:
				*((__int64*)(m_pData + 1)) = 0;
				break;
			case VT_FLOAT:
				*((float*)(m_pData + 1)) = 0;
				break;
			case VT_DOUBLE:
				*((double*)(m_pData + 1)) = 0;
				break;
			case VT_STRING_ANSI:
			case VT_STRING_UTF8:
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				*((char*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRING_UCS16:
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				*((WCHAR*)(m_pData + sizeof(BYTE) + sizeof(UINT))) = 0;
				break;
			case VT_STRUCT:
			case VT_BINARY:
			case VT_ARRAY:
				*((UINT*)(m_pData + sizeof(BYTE))) = 0;
				break;
			case VT_BOOL:
				*(m_pData + 1) = 0;
				break;
			}

			return true;
		}
		return false;
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
		if (m_pData == NULL || m_DataLen == 0)
			return VT_UNKNOWN;
		return m_pData[0];
	}	
	UINT GetLength() const
	{
		switch (GetType())
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
			if (m_DataLen >= sizeof(BYTE) + sizeof(UINT))
				return *((UINT*)(m_pData + sizeof(BYTE)));
			break;
		case VT_STRING_UCS16:
			if (m_DataLen >= sizeof(BYTE) + sizeof(UINT))
				return *((UINT*)(m_pData + sizeof(BYTE)));
			break;
		case VT_STRUCT:
		case VT_BINARY:
		case VT_ARRAY:
			if (m_DataLen >= sizeof(BYTE) + sizeof(UINT))
				return *((UINT*)(m_pData + sizeof(BYTE)));
			break;
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
			if (m_DataLen >= sizeof(BYTE) + sizeof(UINT))
				return *((UINT*)(m_pData + sizeof(BYTE))) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			break;
		case VT_STRING_UCS16:
			return *((UINT *)(m_pData + sizeof(BYTE))) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		case VT_STRUCT:
		case VT_BINARY:		
		case VT_ARRAY:
			if (m_DataLen >= sizeof(BYTE) + sizeof(UINT))
				return *((UINT*)(m_pData + sizeof(BYTE))) + sizeof(BYTE) + sizeof(UINT);
			break;
		case VT_BOOL:
			return sizeof(BYTE) + sizeof(BYTE);		
		}
		return 0;
	}	
	UINT GetBufferLen() const
	{
		return m_DataLen;
	}
	LPVOID GetData()
	{
		return m_pData;
	}
	LPCVOID GetData() const
	{
		return m_pData;
	}
	LPVOID GetValueData()
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
		case VT_ARRAY:
			return m_pData + sizeof(BYTE) + sizeof(UINT);
		case VT_BOOL:
			return m_pData + sizeof(BYTE);
		}
		return NULL;
	}
	LPCVOID GetValueData() const
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
		case VT_ARRAY:
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
	explicit operator char* ()
	{
		switch (GetType())
		{
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return (char*)(m_pData + sizeof(BYTE) + sizeof(UINT));
		}
		return (char*)"";
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
	explicit operator WCHAR* ()
	{
		static WCHAR pEmptyStr[1] = { 0 };
		switch (GetType())
		{
		case VT_STRING_UCS16:
			return (WCHAR*)(m_pData + sizeof(BYTE) + sizeof(UINT));
		}
		return pEmptyStr;
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
	explicit operator unsigned char* ()
	{
		switch (GetType())
		{
		case VT_BINARY:
			return (m_pData + sizeof(BYTE) + sizeof(UINT));
		}
		return NULL;
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
		GetString(String);		
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

	//operator CEasyBuffer()
	//{
	//	CEasyBuffer Data((unsigned char*)this, GetLength());
	//	Data.SetUsedSize(GetLength());
	//	return Data;
	//}

	operator const CEasyBuffer() const
	{
		CEasyBuffer Data((const unsigned char*)(*this), GetLength());
		return Data;
	}

	operator CVariedValue() const
	{
		CVariedValue Value;
		GetVariedValue(Value);		
		return Value;
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
			String.SetString((WCHAR*)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength() / sizeof(WCHAR), CEasyString::STRING_CODE_PAGE_UCS16);
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
			String.SetStringRef((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT)), GetLength() / sizeof(WCHAR), CEasyString::STRING_CODE_PAGE_UCS16);
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

	void GetVariedValue(CVariedValue& Value) const
	{
		switch (GetType())
		{
		case VT_CHAR:
			Value = (char)(*this);
			break;
		case VT_UCHAR:
			Value = (unsigned char)(*this);
			break;
		case VT_SHORT:
			Value = (short)(*this);
			break;
		case VT_USHORT:
			Value = (unsigned short)(*this);
			break;
		case VT_INT:
			Value = (int)(*this);
			break;
		case VT_UINT:
			Value = (unsigned int)(*this);
			break;
		case VT_BIGINT:
			Value = (__int64)(*this);
			break;
		case VT_UBIGINT:
			Value = (unsigned __int64)(*this);
			break;
		case VT_FLOAT:
			Value = (float)(*this);
			break;
		case VT_DOUBLE:
			Value = (double)(*this);
			break;
		case VT_STRING_UTF8:
		case VT_STRING_UCS16:
			Value = this->GetString();
			break;
		case VT_STRUCT:
			FetchVariedData(Value);
			break;
		case VT_STRING_ANSI:
			Value = this->GetString();
			break;
		case VT_BINARY:
			Value.SetBinary((*this), GetLength());
			break;
		case VT_ARRAY:
			FetchVariedData(Value);
			break;
		case VT_BOOL:
			Value = (bool)(*this);
			break;
		default:
			Value.Clear();
		}
	}

	CSmartValue& operator=(char Value)
	{		
		Create(VT_CHAR, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(unsigned char Value)
	{
		Create(VT_UCHAR, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(short Value)
	{
		Create(VT_SHORT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(unsigned short Value)
	{
		Create(VT_USHORT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(int Value)
	{
		Create(VT_INT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(unsigned int Value)
	{
		Create(VT_UINT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(__int64 Value)
	{
		Create(VT_BIGINT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(unsigned __int64 Value)
	{
		Create(VT_UBIGINT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(float Value)
	{
		Create(VT_FLOAT, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(double Value)
	{
		Create(VT_DOUBLE, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(bool Value)
	{
		Create(VT_BOOL, 0);
		SetValue(Value);
		return *this;
	}
	CSmartValue& operator=(const char* Value)
	{
		Destory();
		if (Value)
		{
			UINT DataLen;
			int ValueType;
			switch (INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				ValueType = VT_STRING_ANSI;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = VT_STRING_UTF8;
				break;
			case  CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = VT_STRING_UCS16;
				break;
			default:
				return *this;
			}
			DataLen = GetValueSize(Value);
			Create(ValueType, DataLen);
			SetString(Value);
		}
		return *this;
	}
	CSmartValue& operator=(const WCHAR* Value)
	{
		Destory();
		if (Value)
		{
			UINT DataLen;
			int ValueType;
			switch (INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				ValueType = VT_STRING_ANSI;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = VT_STRING_UTF8;
				break;
			case  CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = VT_STRING_UCS16;
				break;
			default:
				return *this;
			}
			DataLen = GetValueSize(Value);
			Create(ValueType, DataLen);
			SetString(Value);
		}
		return *this;
	}
	CSmartValue& operator=(const CEasyBuffer& Value)
	{
		if (Create(VT_BINARY, (UINT)Value.GetUsedSize()))
		{
			SetValue(Value);
		}
		return *this;
	}
	CSmartValue& operator=(const CVariedValue& Value)
	{
		switch (Value.GetType())
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			Create(VT_BOOL, 0);
			SetValue((bool)Value);
			break;
		case VARIED_VALUE_TYPE::INT8:
			Create(VT_CHAR, 0);
			SetValue((char)Value);
			break;
		case VARIED_VALUE_TYPE::UINT8:
			Create(VT_UCHAR, 0);
			SetValue((unsigned char)Value);
			break;
		case VARIED_VALUE_TYPE::INT16:
			Create(VT_SHORT, 0);
			SetValue((short)Value);
			break;
		case VARIED_VALUE_TYPE::UINT16:
			Create(VT_USHORT, 0);
			SetValue((unsigned short)Value);
			break;
		case VARIED_VALUE_TYPE::INT32:
			Create(VT_INT, 0);
			SetValue((int)Value);
			break;
		case VARIED_VALUE_TYPE::UINT32:
			Create(VT_UINT, 0);
			SetValue((unsigned int)Value);
			break;
		case VARIED_VALUE_TYPE::INT64:
			Create(VT_BIGINT, 0);
			SetValue((__int64)Value);
			break;
		case VARIED_VALUE_TYPE::UINT64:
			Create(VT_UBIGINT, 0);
			SetValue((unsigned __int64)Value);
			break;
		case VARIED_VALUE_TYPE::FLOAT32:
			Create(VT_FLOAT, 0);
			SetValue((float)Value);
			break;
		case VARIED_VALUE_TYPE::FLOAT64:
			Create(VT_DOUBLE, 0);
			SetValue((double)Value);
			break;
		case VARIED_VALUE_TYPE::STRING:
			*this = (LPCTSTR)Value;
			break;
		case VARIED_VALUE_TYPE::BINARY:
			Create(VT_ARRAY, GetBinaryValueSize(Value.GetLength()));
			SetValue((const unsigned char*)Value, Value.GetLength());
			break;
		case VARIED_VALUE_TYPE::ARRAY:
			Create(VT_ARRAY, GetVariedValueSize(Value));
			SetValue(Value, true);
			break;
		case VARIED_VALUE_TYPE::TABLE:
			Create(VT_STRUCT, GetVariedValueSize(Value));
			SetValue(Value, true);
			break;
		default:
			Destory();
		}
		return *this;
	}
	CSmartValue& operator=(const CSmartValue& Value)
	{
		Attach((void*)Value.GetData(), Value.GetDataLen(), VT_UNKNOWN);
		return *this;
	}	
	template<typename T, 
		std::enable_if_t<
		std::is_same<std::decay_t<T>, char>::value ||
		std::is_same<std::decay_t<T>, unsigned char>::value ||
		std::is_same<std::decay_t<T>, short>::value ||
		std::is_same<std::decay_t<T>, unsigned short>::value ||
		std::is_same<std::decay_t<T>, int>::value ||
		std::is_same<std::decay_t<T>, unsigned int>::value ||
		std::is_same<std::decay_t<T>, __int64>::value ||
		std::is_same<std::decay_t<T>, unsigned __int64>::value ||
		std::is_same<std::decay_t<T>, float>::value ||
		std::is_same<std::decay_t<T>, double>::value ||
		std::is_same<std::decay_t<T>, bool>::value,
		bool> = true>
		bool SetValue(T Value, bool CanChangeType = false)
	{
		if (m_pData == NULL)
			return false;
		int Type = GetType();
		if (CanChangeType)
		{
			Type = CSmartValue::GetTypeByValue(Value);
			if (!ChangeType(Type, false))
				return false;
		}
		switch (Type)
		{
		case VT_CHAR:
			*((char*)(m_pData + 1)) = (char)Value;
			return true;
		case VT_UCHAR:
			*((unsigned char*)(m_pData + 1)) = (unsigned char)Value;
			return true;
		case VT_SHORT:
			*((short*)(m_pData + 1)) = (short)Value;
			return true;
		case VT_USHORT:
			*((unsigned short*)(m_pData + 1)) = (unsigned short)Value;
			return true;
		case VT_INT:
			*((int*)(m_pData + 1)) = (int)Value;
			return true;
		case VT_UINT:
			*((unsigned int*)(m_pData + 1)) = (unsigned int)Value;
			return true;
		case VT_BIGINT:
			*((__int64*)(m_pData + 1)) = (__int64)Value;
			return true;
		case VT_UBIGINT:
			*((unsigned __int64*)(m_pData + 1)) = (unsigned __int64)Value;
			return true;
		case VT_FLOAT:
			*((float*)(m_pData + 1)) = (float)Value;
			return true;
		case VT_DOUBLE:
			*((double*)(m_pData + 1)) = (double)Value;
			return true;
		case VT_BOOL:
			(Value != 0) ? (*(m_pData + 1)) = 1 : (*(m_pData + 1)) = 0;
			return true;
		}
		return false;
	}
	bool SetValue(const char* Value, bool CanChangeType = false)
	{
		return SetString(Value, -1, CEasyString::STRING_CODE_PAGE_AUTO, CanChangeType);
	}

	bool SetValue(const WCHAR* Value, bool CanChangeType = false)
	{
		return SetString(Value, -1, CanChangeType);
	}

	//bool SetValue(const CEasyString& Value, bool CanChangeType = false)
	//{
	//	return SetString(Value, CanChangeType);
	//}
	bool SetValue(const unsigned char* pData, size_t DataLen, bool CanChangeType = false)
	{
		if ((pData == NULL) || (DataLen == 0))
			return false;
		if (GetType() != VT_BINARY)
		{
			if (CanChangeType)
			{
				if (!ChangeType(VT_BINARY, (UINT)DataLen))
					return false;
			}
			else
			{
				return false;
			}
		}
		
		size_t MaxLen = m_DataLen - sizeof(BYTE) - sizeof(UINT);
		if (DataLen > MaxLen)
			DataLen = MaxLen;
		*((UINT*)(m_pData + sizeof(BYTE))) = (UINT)DataLen;
		memcpy(m_pData + sizeof(BYTE) + sizeof(UINT), pData, DataLen);
		return true;
	}
	bool SetValue(const CEasyBuffer& Value, bool CanChangeType = false)
	{
		return SetValue((const unsigned char*)Value.GetBuffer(), Value.GetUsedSize(), CanChangeType);
	}
	bool SetValue(const CVariedValue& Value, bool CanChangeType = false);
	bool SetValue(const CSmartValue& Value, bool CanChangeType = false)
	{
		if (m_pData == NULL)
			return false;
		int Type = Value.GetType();
		switch (Type)
		{
		case VT_CHAR:
			return SetValue((char)Value, CanChangeType);
		case VT_UCHAR:
			return SetValue((unsigned char)Value, CanChangeType);
		case VT_SHORT:
			return SetValue((short)Value, CanChangeType);
		case VT_USHORT:
			return SetValue((unsigned short)Value, CanChangeType);
		case VT_INT:
			return SetValue((int)Value, CanChangeType);
		case VT_UINT:
			return SetValue((unsigned int)Value, CanChangeType);
		case VT_BIGINT:
			return SetValue((__int64)Value, CanChangeType);
		case VT_UBIGINT:
			return SetValue((unsigned __int64)Value, CanChangeType);
		case VT_FLOAT:
			return SetValue((float)Value, CanChangeType);
		case VT_DOUBLE:
			return SetValue((double)Value, CanChangeType);
		case VT_STRING_ANSI:
			return SetString((LPCSTR)Value, Value.GetLength(), CEasyString::STRING_CODE_PAGE_ANSI, CanChangeType);
		case VT_STRING_UTF8:
			return SetString((LPCSTR)Value, Value.GetLength(), CEasyString::STRING_CODE_PAGE_UTF8, CanChangeType);
		case VT_STRING_UCS16:
			return SetString((LPCWSTR)Value, Value.GetLength() / sizeof(WCHAR), CanChangeType);
		case VT_STRUCT:
		case VT_BINARY:
		case VT_ARRAY:
			{
				UINT CopyLen = Value.GetLength();
				if (GetType() != Type)
				{
					if (CanChangeType)
					{
						if (!ChangeType(Type, CopyLen))
							return false;
					}
					else
					{
						return false;
					}
				}			
				if (CopyLen + sizeof(BYTE) + sizeof(UINT) > m_DataLen)
					return false;
				memcpy(GetValueData(), Value.GetValueData(), CopyLen);
				*((UINT*)(m_pData + sizeof(BYTE))) = CopyLen;
				return true;
			}
			break;
		case VT_BOOL:
			return SetValue((bool)Value, CanChangeType);
		}
		return false;
	}

	bool SetString(const char * pStr, int Len = -1, CEasyString::STRING_CODE_PAGE CodePage = CEasyString::STRING_CODE_PAGE_AUTO, bool CanChangeType = false)
	{
		if (pStr == NULL)
			return false;
		if (Len < 0)
			Len = (UINT)strlen(pStr);
		if (CodePage == CEasyString::STRING_CODE_PAGE_AUTO)
			CodePage = CEasyString::SYSTEM_STRING_CODE_PAGE;
		int Type = GetType();
		if (CanChangeType && Type != VT_STRING_ANSI && Type != VT_STRING_UTF8 && Type != VT_STRING_UCS16 && Type != VT_BINARY)
		{
			if (CodePage == CEasyString::STRING_CODE_PAGE_ANSI)
			{
				if (!ChangeType(VT_STRING_ANSI, Len))
					return false;
				Type = VT_STRING_ANSI;
			}
			else if (CodePage == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				if (!ChangeType(VT_STRING_UTF8, Len))
					return false;
				Type = VT_STRING_UTF8;
			}
			else if (CodePage == CEasyString::STRING_CODE_PAGE_UCS16)
			{
				if (!ChangeType(VT_STRING_UCS16, Len))
					return false;
				Type = VT_STRING_UCS16;
			}
			else
			{
				return false;
			}			
		}
		switch (Type)
		{		
		case VT_STRING_ANSI:
			if (CodePage == CEasyString::STRING_CODE_PAGE_UTF8)
			{
				UINT DestStrLen = (UINT)UTF8ToAnsi(pStr, Len, NULL, 0);
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
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
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
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
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
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
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
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
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
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
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
				UINT MaxLen = (m_DataLen - HeaderLen - sizeof(WCHAR)) / sizeof(WCHAR);
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				UTF8ToUnicode(pStr, Len, (WCHAR *)(m_pData + HeaderLen), DestStrLen);
				*((WCHAR *)(m_pData + HeaderLen + DestStrLen)) = 0;
				*((UINT *)(m_pData + sizeof(BYTE))) = DestStrLen * sizeof(WCHAR);
			}
			return true;
		case VT_BINARY:
			{
				UINT DestStrLen = Len;
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen;
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				memcpy(m_pData + HeaderLen, pStr, DestStrLen);
				*((UINT*)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			return true;
		}
		return false;
	}

	bool SetString(const WCHAR * pStr,int Len = -1, bool CanChangeType = false)
	{
		if (pStr == NULL)
			return false;

		if (Len < 0)
			Len = (UINT)wcslen(pStr);
		int Type = GetType();
		if (CanChangeType && Type != VT_STRING_ANSI && Type != VT_STRING_UTF8 && Type != VT_STRING_UCS16 && Type != VT_BINARY)
		{
			if (!ChangeType(VT_STRING_UCS16, Len * sizeof(WCHAR)))
				return false;
			Type = VT_STRING_UCS16;
		}
		CEasyString::STRING_CODE_PAGE CodePage;
		switch (Type)
		{
		case VT_STRING_ANSI:
			CodePage = CEasyString::STRING_CODE_PAGE_ANSI;
			break;
		case VT_STRING_UTF8:
			CodePage = CEasyString::STRING_CODE_PAGE_UTF8;
			break;
		case VT_STRING_UCS16:
			CodePage = CEasyString::STRING_CODE_PAGE_UCS16;
			break;
		case VT_BINARY:
			{
				UINT DestStrLen = Len * sizeof(WCHAR);
				UINT HeaderLen = (UINT)(sizeof(BYTE) + sizeof(UINT));
				UINT MaxLen = m_DataLen - HeaderLen;
				if (DestStrLen > MaxLen)
					DestStrLen = MaxLen;
				memcpy(m_pData + HeaderLen, pStr, DestStrLen);
				*((UINT*)(m_pData + sizeof(BYTE))) = DestStrLen;
			}
			return true;
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
				if ((UINT)Len > MaxLen)
					Len = MaxLen;
				*((UINT *)(m_pData + sizeof(BYTE))) = Len * sizeof(WCHAR);
				memcpy(m_pData + sizeof(BYTE) + sizeof(UINT), pStr, Len * sizeof(WCHAR));
				*((WCHAR *)(m_pData + sizeof(BYTE) + sizeof(UINT) + Len * sizeof(WCHAR))) = 0;
			}
			return true;
		}
		return false;
	}	
	bool SetNull()
	{
		if (m_pData == NULL)
			return false;
		if (m_pData == &NULL_DATA)
			return true;
		*m_pData = VT_NULL;
		return true;
	}
	bool IsNull()
	{
		return GetType() == VT_NULL;
	}
	void Dump(CStringBuilder& OutBuffer, bool HaveType);	

	inline static SMART_VALUE_TYPE GetTypeByValue(char Value)
	{
		return VT_CHAR;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(unsigned char Value)
	{
		return VT_UCHAR;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(short Value)
	{
		return VT_SHORT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(unsigned short Value)
	{
		return VT_USHORT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(int Value)
	{
		return VT_INT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(unsigned int Value)
	{
		return VT_UINT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(__int64 Value)
	{
		return VT_BIGINT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(unsigned __int64 Value)
	{
		return VT_UBIGINT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(float Value)
	{
		return VT_FLOAT;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(double Value)
	{
		return VT_DOUBLE;
	}
	inline static SMART_VALUE_TYPE GetTypeByValue(bool Value)
	{
		return VT_BOOL;
	}
	template<typename T,
		std::enable_if_t<
		std::is_same<std::decay_t<T>, char>::value ||
		std::is_same<std::decay_t<T>, unsigned char>::value ||
		std::is_same<std::decay_t<T>, short>::value ||
		std::is_same<std::decay_t<T>, unsigned short>::value ||
		std::is_same<std::decay_t<T>, int>::value ||
		std::is_same<std::decay_t<T>, unsigned int>::value ||
		std::is_same<std::decay_t<T>, __int64>::value ||
		std::is_same<std::decay_t<T>, unsigned __int64>::value ||
		std::is_same<std::decay_t<T>, float>::value ||
		std::is_same<std::decay_t<T>, double>::value ||
		std::is_same<std::decay_t<T>, bool>::value,
		bool> = true>
	inline static UINT GetValueSize(T Value)
	{
		return sizeof(Value);
	}
	inline static UINT GetValueSize(const char* Value)
	{
		UINT StrLen = (UINT)strlen(Value);
		UINT DataLen = 0;
		switch (INTERNAL_STRING_CODE_PAGE)
		{
		case CEasyString::STRING_CODE_PAGE_ANSI:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
				DataLen = (UINT)UTF8ToAnsi(Value, StrLen, NULL, 0);
			else
				DataLen = StrLen;
			break;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
				DataLen = (UINT)AnsiToUTF8(Value, StrLen, NULL, 0);
			else
				DataLen = StrLen;
			break;
		case  CEasyString::STRING_CODE_PAGE_UCS16:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
				DataLen = (UINT)AnsiToUnicode(Value, StrLen, NULL, 0) * sizeof(WCHAR);
			else
				DataLen = (UINT)UTF8ToUnicode(Value, StrLen, NULL, 0) * sizeof(WCHAR);
			break;
		}
		return DataLen;
	}
	inline static UINT GetValueSize(const WCHAR* Value)
	{
		UINT StrLen = (UINT)wcslen(Value);
		UINT DataLen = 0;
		switch (INTERNAL_STRING_CODE_PAGE)
		{
		case CEasyString::STRING_CODE_PAGE_ANSI:
			DataLen = (UINT)UnicodeToAnsi(Value, StrLen, NULL, 0);
			break;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			DataLen = (UINT)UnicodeToUTF8(Value, StrLen, NULL, 0);
			break;
		case  CEasyString::STRING_CODE_PAGE_UCS16:
			DataLen = StrLen * sizeof(WCHAR);
			break;
		}
		return DataLen;
	}
	inline static UINT GetBinaryValueSize(size_t Len)
	{
		return (UINT)Len;
	}
	static UINT GetVariedValueSize(const CVariedValue& Value);
	static UINT GetDataLenByType(int Type, UINT Len)
	{
		switch (Type)
		{
		case VT_NULL:
			return sizeof(BYTE);
			break;
		case VT_CHAR:
		case VT_UCHAR:
			return sizeof(char) + sizeof(BYTE);
			break;
		case VT_SHORT:
		case VT_USHORT:
			return sizeof(short) + sizeof(BYTE);
			break;
		case VT_INT:
		case VT_UINT:
			return sizeof(int) + sizeof(BYTE);
			break;
		case VT_BIGINT:
		case VT_UBIGINT:
			return sizeof(__int64) + sizeof(BYTE);
			break;
		case VT_FLOAT:
			return sizeof(float) + sizeof(BYTE);
			break;
		case VT_DOUBLE:
			return sizeof(double) + sizeof(BYTE);
			break;
		case VT_STRING_ANSI:
		case VT_STRING_UTF8:
			return (Len + 1) + sizeof(BYTE) + sizeof(UINT);
			break;
		case VT_STRING_UCS16:
			return sizeof(WCHAR) * ((Len / sizeof(WCHAR)) + 1) + sizeof(BYTE) + sizeof(UINT);
			break;
		case VT_STRUCT:
		case VT_BINARY:
		case VT_ARRAY:
			return Len + sizeof(BYTE) + sizeof(UINT);
			break;
		case VT_BOOL:
			return sizeof(bool) + sizeof(BYTE);
			break;
		default:
			return 0;
		}
	}
#ifdef RAPIDJSON_DOCUMENT_H_
	rapidjson::Value ToJson(rapidjson::Document::AllocatorType& Alloc) const;
#endif
protected:
	bool FetchVariedData(CVariedValue& Value) const;
};
