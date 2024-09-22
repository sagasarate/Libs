#pragma once

enum class VARIED_VALUE_TYPE :char
{
	NIL = 0,	
	BOOLEAN,
	INT8,
	UINT8,
	INT16,
	UINT16,
	INT32,
	UINT32,
	INT64,
	UINT64,
	FLOAT32,
	FLOAT64,
	STRING,
	BINARY,
	ARRAY,
	TABLE,
};
class CVariedValue
{
protected:
	VARIED_VALUE_TYPE m_Type;
	union
	{
		bool									m_bValue;
		double									m_fValue;
		unsigned __int64						m_ui64Value;
		CEasyString*							m_pStrValue;
		CEasyBuffer*							m_pBinaryValue;
		CEasyArray<CVariedValue>*				m_pArrayValue;
		CStaticMap<CVariedValue, CVariedValue>*	m_pTableValue;
	};	
public:	
	static CVariedValue NIL;
	class iterator
	{
	protected:
		CVariedValue* m_pParent;
		void* m_Pos;
	public:
		iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		iterator(CVariedValue* pParent, void* Pos)
		{
			m_pParent = pParent;
			m_Pos = Pos;
		}
		iterator(const iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		CVariedValue& operator*()
		{			
			CVariedValue* pValue = m_pParent->GetChildByPos(m_Pos);
			if (pValue)
				return *pValue;
			else
				return NIL;
		}
		iterator& operator++()
		{
			m_Pos = m_pParent->GetNextChildPos(m_Pos);
			return *this;
		}
		iterator& operator=(const iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
			return *this;
		}
		bool operator!=(const iterator& itr)
		{
			return m_Pos != itr.m_Pos || m_pParent != itr.m_pParent;
		}
	};

	class const_iterator
	{
	protected:
		const CVariedValue* m_pParent;
		void* m_Pos;
	public:
		const_iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		const_iterator(const CVariedValue* pParent, void* Pos)
		{
			m_pParent = pParent;
			m_Pos = Pos;
		}
		const_iterator(const const_iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		const CVariedValue& operator*()
		{
			const CVariedValue* pValue = m_pParent->GetChildByPos(m_Pos);
			if (pValue)
				return *pValue;
			else
				return NIL;
		}
		const_iterator& operator++()
		{
			m_Pos = m_pParent->GetNextChildPos(m_Pos);
			return *this;
		}
		const_iterator& operator=(const const_iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
			return *this;
		}
		bool operator!=(const const_iterator& itr)
		{
			return m_Pos != itr.m_Pos || m_pParent != itr.m_pParent;
		}
	};

	CVariedValue()
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
	}
	~CVariedValue()
	{
		Clear();
	}
	void Clear()
	{
		switch (m_Type)
		{		
		case VARIED_VALUE_TYPE::STRING:
			SAFE_DELETE(m_pStrValue);
			break;
		case VARIED_VALUE_TYPE::BINARY:
			SAFE_DELETE(m_pBinaryValue);
			break;
		case VARIED_VALUE_TYPE::ARRAY:
			SAFE_DELETE(m_pArrayValue);
			break;
		case VARIED_VALUE_TYPE::TABLE:
			SAFE_DELETE(m_pTableValue);
			break;
		default:
			m_ui64Value = 0;
			break;
		}
		m_Type = VARIED_VALUE_TYPE::NIL;		
	}
	VARIED_VALUE_TYPE GetType() const
	{
		return m_Type;
	}
	CVariedValue(bool Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(char Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(unsigned char Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(short Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(unsigned short Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(int Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(unsigned int Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(__int64 Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(unsigned __int64 Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(float Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}

	CVariedValue(double Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}
	CVariedValue(LPCTSTR Value, int Len = -1)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		CreateString();
		m_pStrValue->SetString(Value, Len);
	}
	CVariedValue(const CEasyString& Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}	
	CVariedValue(const unsigned char* pData, size_t DataLen)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		if (CreateBinary(DataLen))
			m_pBinaryValue->PushBack(pData, DataLen);
	}
	CVariedValue(const CEasyBuffer& Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}
	CVariedValue(const CVariedValue& Value)
	{
		m_Type = VARIED_VALUE_TYPE::NIL;
		m_ui64Value = 0;
		*this = Value;
	}
	CVariedValue(CVariedValue&& Value) noexcept
	{
		m_Type = Value.m_Type;
		m_ui64Value = Value.m_ui64Value;
		Value.m_Type = VARIED_VALUE_TYPE::NIL;
		Value.m_ui64Value = 0;
	}
	operator bool() const
	{
		switch (m_Type)
		{		
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return m_ui64Value != 0;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return m_fValue != 0;		
		}
		return false;
	}

	operator char() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (char)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (char)m_fValue;		
		}
		return 0;
	}

	operator unsigned char() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned char)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (unsigned char)m_fValue;
		}
		return 0;
	}

	operator short() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (short)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (short)m_fValue;
		}
		return 0;
	}

	operator unsigned short() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned short)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (unsigned short)m_fValue;
		}
		return 0;
	}

	operator int() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (int)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (int)m_fValue;
		}
		return 0;
	}

	operator unsigned int() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned int)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (unsigned int)m_fValue;
		}
		return 0;
	}

	operator __int64() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (__int64)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (__int64)m_fValue;
		}
		return 0;
	}

	operator unsigned __int64() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned __int64)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (unsigned __int64)m_fValue;
		}
		return 0;
	}

	operator float() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1.0f : 0.0f;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (float)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (float)m_fValue;
		}
		return 0;
	}

	operator double() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return m_bValue ? 1 : 0;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return (double)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			return (double)m_fValue;
		}
		return 0;
	}

	operator LPCTSTR () const
	{
		if (m_Type == VARIED_VALUE_TYPE::STRING)
			return *m_pStrValue;
		return _T("");
	}

	operator const CEasyString&() const
	{
		if (m_Type == VARIED_VALUE_TYPE::STRING)
			return *m_pStrValue;
		return CEasyString::EMPTY_STR;
	}
	operator CEasyString()
	{
		if (m_Type == VARIED_VALUE_TYPE::STRING)
			return *m_pStrValue;
		return CEasyString();
	}

	operator const unsigned char*() const
	{
		if (m_Type == VARIED_VALUE_TYPE::BINARY)
			return (const unsigned char*)m_pBinaryValue->GetBuffer();
		return NULL;
	}

#ifdef WIN32
	CVariedValue& operator=(const VARIANT& Value)
	{		
		switch (Value.vt)
		{
		case VT_BSTR:		
			Clear();
			m_Type = VARIED_VALUE_TYPE::STRING;
			m_pStrValue = MONITORED_NEW("CVariedValue", CEasyString);
			m_pStrValue->SetString(Value.bstrVal, ::SysStringLen(Value.bstrVal));
			break;
		case VT_I2:
			*this = Value.iVal;
			break;
		case VT_I4:
		case VT_INT:
			*this = (int)Value.lVal;
			break;
		case VT_I1:
			*this = (char)Value.bVal;
			break;
		case VT_UI1:
			*this = (unsigned char)Value.bVal;
			break;
		case VT_UI2:
			*this = Value.uiVal;
			break;
		case VT_UINT:
		case VT_UI4:
			*this = (UINT)Value.ulVal;
			break;
		case VT_R4:
			*this = Value.fltVal;
			break;
		case VT_R8:
			*this = Value.dblVal;
			break;
		case VT_BOOL:
			*this = (bool)Value.boolVal;
			break;
		case VT_I8:
			*this = Value.llVal;
			break;
		case VT_UI8:
			*this = Value.ullVal;
			break;
		}
		return *this;
	}
	operator _variant_t() const
	{		
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:	
			return (_variant_t)m_bValue;
		case VARIED_VALUE_TYPE::INT8:
			return (_variant_t)(char)m_ui64Value;
		case VARIED_VALUE_TYPE::UINT8:
			return (_variant_t)(unsigned char)m_ui64Value;
		case VARIED_VALUE_TYPE::INT16:
			return (_variant_t)(short)m_ui64Value;
		case VARIED_VALUE_TYPE::UINT16:
			return (_variant_t)(unsigned short)m_ui64Value;
		case VARIED_VALUE_TYPE::INT32:
			return (_variant_t)(int)m_ui64Value;
		case VARIED_VALUE_TYPE::UINT32:
			return (_variant_t)(unsigned int)m_ui64Value;
		case VARIED_VALUE_TYPE::INT64:
			return (_variant_t)(__int64)m_ui64Value;
		case VARIED_VALUE_TYPE::UINT64:
			return (_variant_t)(unsigned __int64)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT32:
			return (_variant_t)(float)m_ui64Value;
		case VARIED_VALUE_TYPE::FLOAT64:
			return (_variant_t)(float)m_ui64Value;
		case VARIED_VALUE_TYPE::STRING:
			return (_variant_t)(LPCTSTR)(*m_pStrValue);			
		}
		return _variant_t();
	}
#endif
	CVariedValue& operator=(bool Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::BOOLEAN;
		m_bValue = Value;
		return *this;
	}

	CVariedValue& operator=(char Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::INT8;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(unsigned char Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::UINT8;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(short Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::INT16;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(unsigned short Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::UINT16;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(int Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::INT32;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(unsigned int Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::UINT32;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(__int64 Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::INT64;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(unsigned __int64 Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::UINT64;
		m_ui64Value = Value;
		return *this;
	}

	CVariedValue& operator=(float Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::FLOAT32;
		m_fValue = Value;
		return *this;
	}

	CVariedValue& operator=(double Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::FLOAT64;
		m_fValue = Value;
		return *this;
	}
	CVariedValue& operator=(LPCTSTR Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::STRING;
		m_pStrValue = MONITORED_NEW("CVariedValue", CEasyString, Value);
		return *this;
	}
	CVariedValue& operator=(const CEasyString& Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::STRING;
		m_pStrValue = MONITORED_NEW("CVariedValue", CEasyString, Value);
		return *this;
	}
	CVariedValue& operator=(const CEasyBuffer& Value)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::BINARY;
		m_pBinaryValue = MONITORED_NEW("CVariedValue", CEasyBuffer, Value);
		return *this;
	}
	CVariedValue& operator=(const CVariedValue& Value)
	{
		Clear();
		m_Type = Value.m_Type;
		switch (Value.m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			m_bValue = Value.m_bValue;
			break;
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
		case VARIED_VALUE_TYPE::FLOAT32:
		case VARIED_VALUE_TYPE::FLOAT64:
			m_ui64Value = Value.m_ui64Value;
			break;
		case VARIED_VALUE_TYPE::STRING:
			m_pStrValue = MONITORED_NEW("CVariedValue", CEasyString, *Value.m_pStrValue);
			break;
		case VARIED_VALUE_TYPE::BINARY:
			m_pBinaryValue = MONITORED_NEW("CVariedValue", CEasyBuffer, *Value.m_pBinaryValue);
			break;
		case VARIED_VALUE_TYPE::ARRAY:
			m_pArrayValue = MONITORED_NEW("CVariedValue", CEasyArray<CVariedValue>, *Value.m_pArrayValue);
			break;
		case VARIED_VALUE_TYPE::TABLE:
			{
				CreateTable();
				void* Pos = Value.m_pTableValue->GetFirstObjectPos();
				while (Pos)
				{
					CVariedValue Key;
					CVariedValue* pData = Value.m_pTableValue->GetNextObject(Pos, Key);
					m_pTableValue->Insert(Key, *pData);
				}
			}
			break;
		}
		return *this;
	}
	CVariedValue& operator=(CVariedValue&& Value)  noexcept
	{
		if ((&m_Type) == (&Value.m_Type))
			return *this;
		Clear();
		m_Type = Value.m_Type;
		m_ui64Value = Value.m_ui64Value;
		Value.m_Type = VARIED_VALUE_TYPE::NIL;
		Value.m_ui64Value = 0;
		return *this;
	}
	bool operator<(const CVariedValue& Value) const
	{
		if (m_Type != Value.m_Type)
		{
			return m_Type < Value.m_Type;
		}
		else
		{
			switch (m_Type)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
				return (!m_bValue) && (Value.m_bValue);
			case VARIED_VALUE_TYPE::INT8:
				return (char)m_ui64Value < (char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT8:
				return (unsigned char)m_ui64Value < (unsigned char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT16:
				return (short)m_ui64Value < (short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT16:
				return (unsigned short)m_ui64Value < (unsigned short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT32:
				return (int)m_ui64Value < (int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT32:
				return (unsigned int)m_ui64Value < (unsigned int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT64:
				return (__int64)m_ui64Value < (__int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT64:
				return (unsigned __int64)m_ui64Value < (unsigned __int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::FLOAT32:
				return (float)m_fValue < (float)Value.m_fValue;
			case VARIED_VALUE_TYPE::FLOAT64:
				return (double)m_fValue < (double)Value.m_fValue;
			case VARIED_VALUE_TYPE::STRING:
				return *m_pStrValue < *Value.m_pStrValue;
			case VARIED_VALUE_TYPE::BINARY:
				if (m_pBinaryValue->GetUsedSize() == Value.m_pBinaryValue->GetUsedSize())
					return memcmp(m_pBinaryValue->GetBuffer(), Value.m_pBinaryValue->GetBuffer(), m_pBinaryValue->GetUsedSize()) < 0;
				else
					return m_pBinaryValue->GetUsedSize() > Value.m_pBinaryValue->GetUsedSize();
			case VARIED_VALUE_TYPE::ARRAY:
				return m_pArrayValue->GetCount() < Value.m_pArrayValue->GetCount();
			case VARIED_VALUE_TYPE::TABLE:
				return m_pTableValue->GetObjectCount() < Value.m_pTableValue->GetObjectCount();
			default:
				return false;
			}
		}
	}
	bool operator>(const CVariedValue& Value) const
	{
		if (m_Type != Value.m_Type)
		{
			return m_Type > Value.m_Type;
		}
		else
		{
			switch (m_Type)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
				return (m_bValue) && (!Value.m_bValue);
			case VARIED_VALUE_TYPE::INT8:
				return (char)m_ui64Value > (char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT8:
				return (unsigned char)m_ui64Value > (unsigned char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT16:
				return (short)m_ui64Value > (short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT16:
				return (unsigned short)m_ui64Value > (unsigned short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT32:
				return (int)m_ui64Value > (int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT32:
				return (unsigned int)m_ui64Value > (unsigned int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT64:
				return (__int64)m_ui64Value > (__int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT64:
				return (unsigned __int64)m_ui64Value > (unsigned __int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::FLOAT32:
				return (float)m_fValue > (float)Value.m_fValue;
			case VARIED_VALUE_TYPE::FLOAT64:
				return (double)m_fValue > (double)Value.m_fValue;
			case VARIED_VALUE_TYPE::STRING:
				return *m_pStrValue > *Value.m_pStrValue;
			case VARIED_VALUE_TYPE::BINARY:
				if (m_pBinaryValue->GetUsedSize() == Value.m_pBinaryValue->GetUsedSize())
					return memcmp(m_pBinaryValue->GetBuffer(), Value.m_pBinaryValue->GetBuffer(), m_pBinaryValue->GetUsedSize()) > 0;
				else
					return m_pBinaryValue->GetUsedSize() > Value.m_pBinaryValue->GetUsedSize();
			case VARIED_VALUE_TYPE::ARRAY:
				return m_pArrayValue->GetCount() > Value.m_pArrayValue->GetCount();
			case VARIED_VALUE_TYPE::TABLE:
				return m_pTableValue->GetObjectCount() > Value.m_pTableValue->GetObjectCount();
			default:
				return false;
			}
		}
	}
	bool operator==(const CVariedValue& Value) const
	{
		if (m_Type != Value.m_Type)
		{
			return false;
		}
		else
		{
			switch (m_Type)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
				return m_bValue == Value.m_bValue;
			case VARIED_VALUE_TYPE::INT8:
				return (char)m_ui64Value == (char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT8:
				return (unsigned char)m_ui64Value == (unsigned char)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT16:
				return (short)m_ui64Value == (short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT16:
				return (unsigned short)m_ui64Value == (unsigned short)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT32:
				return (int)m_ui64Value == (int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT32:
				return (unsigned int)m_ui64Value == (unsigned int)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::INT64:
				return (__int64)m_ui64Value == (__int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::UINT64:
				return (unsigned __int64)m_ui64Value == (unsigned __int64)Value.m_ui64Value;
			case VARIED_VALUE_TYPE::FLOAT32:
				return (float)m_fValue == (float)Value.m_fValue;
			case VARIED_VALUE_TYPE::FLOAT64:
				return (double)m_fValue == (double)Value.m_fValue;
			case VARIED_VALUE_TYPE::STRING:
				return *m_pStrValue == *Value.m_pStrValue;
			case VARIED_VALUE_TYPE::BINARY:
				if (m_pBinaryValue->GetUsedSize() == Value.m_pBinaryValue->GetUsedSize())
					return memcmp(m_pBinaryValue->GetBuffer(), Value.m_pBinaryValue->GetBuffer(), m_pBinaryValue->GetUsedSize()) == 0;
				else
					return false;
			case VARIED_VALUE_TYPE::ARRAY:
				return m_pArrayValue->GetCount() == Value.m_pArrayValue->GetCount();
			case VARIED_VALUE_TYPE::TABLE:
				return m_pTableValue->GetObjectCount() == Value.m_pTableValue->GetObjectCount();
			default:
				return false;
			}
		}
	}
	CVariedValue operator~() const
	{
		return ~((UINT64)(*this));
	}
	CVariedValue operator&(const CVariedValue& Value) const
	{
		return ((UINT64)(*this)) & ((UINT64)Value);
	}
	CVariedValue operator|(const CVariedValue& Value) const
	{
		return ((UINT64)(*this)) | ((UINT64)Value);
	}
	CVariedValue operator+(const CVariedValue& Value) const
	{
		VARIED_VALUE_TYPE Type = GetArithmeticType(m_Type, Value.m_Type);
		switch (Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return ((bool)(*this))||((bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return (char)(*this) + (char)Value;
		case VARIED_VALUE_TYPE::UINT8:
			return (unsigned char)(*this) + (unsigned char)Value;
		case VARIED_VALUE_TYPE::INT16:
			return (short)(*this) + (short)Value;
		case VARIED_VALUE_TYPE::UINT16:
			return (unsigned short)(*this) + (unsigned short)Value;
		case VARIED_VALUE_TYPE::INT32:
			return (int)(*this) + (int)Value;
		case VARIED_VALUE_TYPE::UINT32:
			return (unsigned int)(*this) + (unsigned int)Value;
		case VARIED_VALUE_TYPE::INT64:
			return (__int64)(*this) + (__int64)Value;
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned __int64)(*this) + (unsigned __int64)Value;
		case VARIED_VALUE_TYPE::FLOAT32:
			return (float)(*this) + (float)Value;
		case VARIED_VALUE_TYPE::FLOAT64:
			return (double)(*this) + (double)Value;
		}
		return CVariedValue::NIL;
	}
	CVariedValue operator-(const CVariedValue& Value) const
	{
		VARIED_VALUE_TYPE Type = GetArithmeticType(m_Type, Value.m_Type);
		switch (Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return ((bool)(*this)) || ((bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return (char)(*this) - (char)Value;
		case VARIED_VALUE_TYPE::UINT8:
			return (unsigned char)(*this) - (unsigned char)Value;
		case VARIED_VALUE_TYPE::INT16:
			return (short)(*this) - (short)Value;
		case VARIED_VALUE_TYPE::UINT16:
			return (unsigned short)(*this) - (unsigned short)Value;
		case VARIED_VALUE_TYPE::INT32:
			return (int)(*this) - (int)Value;
		case VARIED_VALUE_TYPE::UINT32:
			return (unsigned int)(*this) - (unsigned int)Value;
		case VARIED_VALUE_TYPE::INT64:
			return (__int64)(*this) - (__int64)Value;
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned __int64)(*this) - (unsigned __int64)Value;
		case VARIED_VALUE_TYPE::FLOAT32:
			return (float)(*this) - (float)Value;
		case VARIED_VALUE_TYPE::FLOAT64:
			return (double)(*this) - (double)Value;		
		}
		return CVariedValue::NIL;
	}
	CVariedValue operator*(const CVariedValue& Value) const
	{
		VARIED_VALUE_TYPE Type = GetArithmeticType(m_Type, Value.m_Type);
		switch (Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return ((bool)(*this)) && ((bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return (char)(*this) * (char)Value;
		case VARIED_VALUE_TYPE::UINT8:
			return (unsigned char)(*this) * (unsigned char)Value;
		case VARIED_VALUE_TYPE::INT16:
			return (short)(*this) * (short)Value;
		case VARIED_VALUE_TYPE::UINT16:
			return (unsigned short)(*this) * (unsigned short)Value;
		case VARIED_VALUE_TYPE::INT32:
			return (int)(*this) * (int)Value;
		case VARIED_VALUE_TYPE::UINT32:
			return (unsigned int)(*this) * (unsigned int)Value;
		case VARIED_VALUE_TYPE::INT64:
			return (__int64)(*this) * (__int64)Value;
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned __int64)(*this) * (unsigned __int64)Value;
		case VARIED_VALUE_TYPE::FLOAT32:
			return (float)(*this) * (float)Value;
		case VARIED_VALUE_TYPE::FLOAT64:
			return (double)(*this) * (double)Value;		
		}
		return CVariedValue::NIL;
	}
	CVariedValue operator/(const CVariedValue& Value) const
	{
		VARIED_VALUE_TYPE Type = GetArithmeticType(m_Type, Value.m_Type);
		switch (Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return ((bool)(*this)) && ((bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return (char)(*this) / (char)Value;
		case VARIED_VALUE_TYPE::UINT8:
			return (unsigned char)(*this) / (unsigned char)Value;
		case VARIED_VALUE_TYPE::INT16:
			return (short)(*this) / (short)Value;
		case VARIED_VALUE_TYPE::UINT16:
			return (unsigned short)(*this) / (unsigned short)Value;
		case VARIED_VALUE_TYPE::INT32:
			return (int)(*this) / (int)Value;
		case VARIED_VALUE_TYPE::UINT32:
			return (unsigned int)(*this) / (unsigned int)Value;
		case VARIED_VALUE_TYPE::INT64:
			return (__int64)(*this) / (__int64)Value;
		case VARIED_VALUE_TYPE::UINT64:
			return (unsigned __int64)(*this) / (unsigned __int64)Value;
		case VARIED_VALUE_TYPE::FLOAT32:
			return (float)(*this) / (float)Value;
		case VARIED_VALUE_TYPE::FLOAT64:
			return (double)(*this) / (double)Value;		
		}
		return CVariedValue::NIL;
	}
	CVariedValue operator-() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return !((bool)(*this));
		case VARIED_VALUE_TYPE::INT8:		
			return -(char)(*this);
		case VARIED_VALUE_TYPE::UINT8:
		case VARIED_VALUE_TYPE::INT16:		
			return -(short)(*this);		
		case VARIED_VALUE_TYPE::UINT16:
		case VARIED_VALUE_TYPE::INT32:
			return -(int)(*this);
		case VARIED_VALUE_TYPE::UINT32:
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::FLOAT64:
			return -(__int64)(*this);
		case VARIED_VALUE_TYPE::FLOAT32:
			return -(float)(*this);
		case VARIED_VALUE_TYPE::UINT64:		
			return -(double)(*this);		
		}
		return CVariedValue::NIL;
	}
	size_t GetLength() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			return sizeof(bool);
		case VARIED_VALUE_TYPE::INT8:
		case VARIED_VALUE_TYPE::UINT8:
			return sizeof(char);
		case VARIED_VALUE_TYPE::INT16:
		case VARIED_VALUE_TYPE::UINT16:
			return sizeof(short);
		case VARIED_VALUE_TYPE::INT32:
		case VARIED_VALUE_TYPE::UINT32:
			return sizeof(int);
		case VARIED_VALUE_TYPE::INT64:
		case VARIED_VALUE_TYPE::UINT64:
			return sizeof(__int64);
		case VARIED_VALUE_TYPE::FLOAT32:
			return sizeof(float);
		case VARIED_VALUE_TYPE::FLOAT64:
			return sizeof(double);
		case VARIED_VALUE_TYPE::STRING:
			return m_pStrValue->GetLength();
		case VARIED_VALUE_TYPE::BINARY:
			return m_pBinaryValue->GetUsedSize();
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->GetCount();
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetObjectCount();
		}
		return 0;
	}
	bool CreateString()
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::STRING;
		m_pStrValue = MONITORED_NEW("CVariedValue", CEasyString);
		return true;
	}
	bool CreateBinary(size_t Size)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::BINARY;
		m_pBinaryValue = MONITORED_NEW("CVariedValue", CEasyBuffer, Size);
		return true;
	}
	bool CreateArray(size_t Size = 16, size_t GrowSize = 16)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::ARRAY;
		m_pArrayValue = MONITORED_NEW("CVariedValue", CEasyArray<CVariedValue>, Size, GrowSize);
		return true;
	}

	bool CreateTable(size_t Size = 16, size_t GrowSize = 16)
	{
		Clear();
		m_Type = VARIED_VALUE_TYPE::TABLE;
		using MapType = CStaticMap<CVariedValue, CVariedValue>;
		m_pTableValue = MONITORED_NEW("CVariedValue", MapType, (UINT)Size, (UINT)GrowSize, -1);
		return true;
	}
	bool SetString(LPCTSTR pStr, int Len)
	{
		if (CreateString())
		{
			m_pStrValue->SetString(pStr, Len);
			return true;
		}
		return false;
	}
	bool SetBinary(const unsigned char* pData, size_t Len)
	{
		if (CreateBinary(Len))
		{
			return m_pBinaryValue->PushBack(pData, Len);
		}
		return false;
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
	CVariedValue* GetChild(T Key) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->GetObject((size_t)Key);
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->Find(Key);
		}
		return NULL;
	}
	CVariedValue* GetChild(LPCTSTR Key) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->Find(Key);
		}
		return NULL;
	}
	CVariedValue* AddChild(const CVariedValue& Data)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->Add(Data);
		}
		return NULL;
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
	CVariedValue* AddChild(T Key, const CVariedValue& Data)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->Insert((size_t)Key, Data);
		case VARIED_VALUE_TYPE::TABLE:
			{
				UINT ID = m_pTableValue->Insert(Key, Data);
				if (ID)
					return m_pTableValue->GetObject(ID);
			}
			break;
		}
		return NULL;
	}
	CVariedValue* AddChild(LPCTSTR Key, const CVariedValue& Data)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::TABLE:
			{
				UINT ID = m_pTableValue->Insert(Key, Data);
				if (ID)
					return m_pTableValue->GetObject(ID);
			}
			break;
		}
		return NULL;
	}

	CVariedValue* AddChildNil()
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->AddEmpty();
		}
		return NULL;
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
	CVariedValue* AddChildNil(T Key)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			return m_pArrayValue->InsertEmpty((size_t)Key);
		case VARIED_VALUE_TYPE::TABLE:
			{
				CVariedValue* pValue = NULL;
				UINT ID = m_pTableValue->New(Key, &pValue);
				return pValue;
			}
			break;
		}
		return NULL;
	}
	CVariedValue* AddChildNil(LPCTSTR Key)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::TABLE:
			{
				CVariedValue* pValue = NULL;
				UINT ID = m_pTableValue->New(Key, &pValue);
				return pValue;
			}
			break;
		}
		return NULL;
	}

	void* GetFirstChildPos() const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (m_pArrayValue->GetCount())
				return (void*)1;
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetFirstObjectPos();
		}
		return NULL;
	}
	CVariedValue* GetNextChild(LPVOID& Pos)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if(Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{
					
					Pos = (void*)(Index + 2);
					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetNextObject(Pos);
		}
		return NULL;
	}
	const CVariedValue* GetNextChild(LPVOID& Pos) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{

					Pos = (void*)(Index + 2);
					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetNextObject(Pos);
		}
		return NULL;
	}
	CVariedValue* GetNextChild(LPVOID& Pos, CVariedValue& Key)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{

					Pos = (void*)(Index + 2);
					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetNextObject(Pos, Key);
		}
		return NULL;
	}
	const CVariedValue* GetNextChild(LPVOID& Pos, CVariedValue& Key) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{

					Pos = (void*)(Index + 2);
					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetNextObject(Pos, Key);
		}
		return NULL;
	}
	void* GetNextChildPos(void* Pos) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos;
				if (Index < m_pArrayValue->GetCount())
					return (void*)(Index + 1);
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetNextObjectPos(Pos);
		}
		return NULL;
	}
	CVariedValue* GetChildByPos(LPVOID Pos)
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{
					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetObject(Pos);
		}
		return NULL;
	}
	const CVariedValue* GetChildByPos(LPVOID Pos) const
	{
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::ARRAY:
			if (Pos)
			{
				size_t Index = (size_t)Pos - 1;
				if (Index < m_pArrayValue->GetCount())
				{

					return m_pArrayValue->GetObject(Index);
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			return m_pTableValue->GetObject(Pos);
		}
		return NULL;
	}

	iterator begin()
	{
		return iterator(this, GetFirstChildPos());
	}

	iterator end()
	{
		return iterator(this, NULL);
	}
	const_iterator begin() const
	{
		return const_iterator(this, GetFirstChildPos());
	}

	const_iterator end() const
	{
		return const_iterator(this, NULL);
	}
	CEasyString ToString(bool AddQuotationToString = true)
	{
		CEasyString Str;
		switch (m_Type)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			Str = m_bValue ? _T("true") : _T("false");
			break;
		case VARIED_VALUE_TYPE::INT8:
			Str.Format(_T("%d"), (char)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::UINT8:
			Str.Format(_T("%u"), (unsigned char)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::INT16:
			Str.Format(_T("%d"), (short)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::UINT16:
			Str.Format(_T("%u"), (unsigned short)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::INT32:
			Str.Format(_T("%d"), (int)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::UINT32:
			Str.Format(_T("%u"), (unsigned int)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::INT64:
			Str.Format(_T("%lld"), (__int64)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::UINT64:
			Str.Format(_T("%llu"), (unsigned __int64)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::FLOAT32:
			Str.Format(_T("%f"), (float)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::FLOAT64:
			Str.Format(_T("%f"), (double)m_ui64Value);
			break;
		case VARIED_VALUE_TYPE::STRING:
			if (AddQuotationToString)
				Str.Format(_T(""), m_pStrValue->GetBuffer());
			else
				Str = *m_pStrValue;
		case VARIED_VALUE_TYPE::BINARY:
			BinToString((const unsigned char*)m_pBinaryValue->GetBuffer(), m_pBinaryValue->GetUsedSize(), Str);
		case VARIED_VALUE_TYPE::ARRAY:
			break;
		case VARIED_VALUE_TYPE::TABLE:
			break;
		}
		return Str;
	}
protected:
	VARIED_VALUE_TYPE GetArithmeticType(VARIED_VALUE_TYPE LeftType, VARIED_VALUE_TYPE RightType) const
	{
		switch (LeftType)
		{
		case VARIED_VALUE_TYPE::BOOLEAN:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:				
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:				
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:				
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:				
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:				
			case VARIED_VALUE_TYPE::FLOAT32:				
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::INT8:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:				
			case VARIED_VALUE_TYPE::INT8:
				return LeftType;
			case VARIED_VALUE_TYPE::UINT8:
				return VARIED_VALUE_TYPE::INT16;
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::UINT8:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
				return LeftType;
			case VARIED_VALUE_TYPE::INT8:
				return VARIED_VALUE_TYPE::INT16;
			case VARIED_VALUE_TYPE::UINT8:
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::INT16:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:				
			case VARIED_VALUE_TYPE::INT8:				
			case VARIED_VALUE_TYPE::UINT8:				
			case VARIED_VALUE_TYPE::INT16:
				return LeftType;
			case VARIED_VALUE_TYPE::UINT16:
				return VARIED_VALUE_TYPE::INT32;
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::UINT16:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:
				return LeftType;
			case VARIED_VALUE_TYPE::INT16:
				return VARIED_VALUE_TYPE::INT32;
			case VARIED_VALUE_TYPE::UINT16:				
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::INT32:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:				
			case VARIED_VALUE_TYPE::INT16:				
			case VARIED_VALUE_TYPE::UINT16:				
			case VARIED_VALUE_TYPE::INT32:
				return LeftType;
			case VARIED_VALUE_TYPE::UINT32:
				return VARIED_VALUE_TYPE::INT64;
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
				return RightType;
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:				
				return VARIED_VALUE_TYPE::FLOAT64;
			}
			break;
		case VARIED_VALUE_TYPE::UINT32:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
				return LeftType;
			case VARIED_VALUE_TYPE::INT32:
				return VARIED_VALUE_TYPE::INT64;
			case VARIED_VALUE_TYPE::UINT32:				
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:
				return RightType;
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:				
				return VARIED_VALUE_TYPE::FLOAT64;
			}
			break;
		case VARIED_VALUE_TYPE::INT64:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
			case VARIED_VALUE_TYPE::INT32:				
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
				return LeftType;				
			case VARIED_VALUE_TYPE::UINT64:
				return RightType;
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return VARIED_VALUE_TYPE::FLOAT64;
			}
			break;
		case VARIED_VALUE_TYPE::UINT64:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:				
			case VARIED_VALUE_TYPE::UINT64:
				return LeftType;
			case VARIED_VALUE_TYPE::FLOAT32:
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::FLOAT32:
			switch (RightType)
			{
			case VARIED_VALUE_TYPE::BOOLEAN:
			case VARIED_VALUE_TYPE::INT8:
			case VARIED_VALUE_TYPE::UINT8:
			case VARIED_VALUE_TYPE::INT16:
			case VARIED_VALUE_TYPE::UINT16:
				return LeftType;
			case VARIED_VALUE_TYPE::INT32:
			case VARIED_VALUE_TYPE::UINT32:
			case VARIED_VALUE_TYPE::INT64:
			case VARIED_VALUE_TYPE::UINT64:		
				return VARIED_VALUE_TYPE::FLOAT64;
			case VARIED_VALUE_TYPE::FLOAT32:
				return LeftType;
			case VARIED_VALUE_TYPE::FLOAT64:
				return RightType;
			}
			break;
		case VARIED_VALUE_TYPE::FLOAT64:
			return LeftType;
		}
		return VARIED_VALUE_TYPE::NIL;
	}
};

