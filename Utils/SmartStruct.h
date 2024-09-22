/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartStruct.h                                            */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define CHECK_SMART_STRUCT_ADD(Opt,FailCount)		{if(!(Opt)) FailCount++;}
#define CHECK_SMART_STRUCT_ADD_AND_RETURN(Opt)		{if(!(Opt)) return false;}
//#define SMART_STRUCT_FIX_MEMBER_SIZE(DataSize)		(sizeof(WORD)+sizeof(BYTE)+DataSize)
//#define SMART_STRUCT_STRING_MEMBER_SIZE(StrLen)		(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(char)*(StrLen+1))
//#define SMART_STRUCT_WSTRING_MEMBER_SIZE(StrLen)	(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+sizeof(WCHAR)*(StrLen+1))
//#define SMART_STRUCT_STRUCT_MEMBER_SIZE(StructLen)	(sizeof(WORD)+sizeof(BYTE)+sizeof(UINT)+StructLen)

class CSmartStruct
{
protected:
	BYTE *			m_pData;
	UINT			m_DataLen;
	bool			m_IsSelfData;
	LPCTSTR			m_Tag;
public:
	enum
	{
		HEAD_SIZE = (sizeof(BYTE) + sizeof(UINT)),
		INVALID_MEMBER_ID = ((WORD)-1),
	};	
	struct Pair
	{
		WORD		MemberID;
		CSmartValue	Value;
	};
	class iterator
	{
	protected:
		CSmartStruct* m_pParent;
		void* m_Pos;
	public:
		iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		iterator(CSmartStruct* pArray, void* Pos)
		{
			m_pParent = pArray;
			m_Pos = Pos;
		}
		iterator(const iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		Pair operator*()
		{
			return m_pParent->GetPairByPos(m_Pos);
		}
		iterator& operator++()
		{
			m_Pos = m_pParent->GetNextMemberPos(m_Pos);
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
			return (m_pParent != itr.m_pParent) || (m_Pos != itr.m_Pos);
		}
	};
	class const_iterator
	{
	protected:
		const CSmartStruct* m_pParent;
		void* m_Pos;
	public:
		const_iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		const_iterator(const CSmartStruct* pArray, void* Pos)
		{
			m_pParent = pArray;
			m_Pos = Pos;
		}
		const_iterator(const const_iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		const Pair operator*()
		{
			return m_pParent->GetPairByPos(m_Pos);
		}
		const_iterator& operator++()
		{
			m_Pos = m_pParent->GetNextMemberPos(m_Pos);
			return *this;
		}
		const_iterator& operator=(const const_iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
			return *this;
		}
		bool operator !=(const const_iterator& itr)
		{
			return (m_pParent != itr.m_pParent) || (m_Pos != itr.m_Pos);
		}
	};

	CSmartStruct(LPCTSTR Tag = _T("CSmartStruct"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
	}
	CSmartStruct(int BufferLen, LPCTSTR Tag = _T("CSmartStruct"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Create(BufferLen);
	}
	CSmartStruct(LPVOID pData, UINT DataLen, bool IsEmpty, LPCTSTR Tag = _T("CSmartStruct"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach(pData, DataLen, IsEmpty);
	}
	CSmartStruct(const CSmartStruct& Struct, LPCTSTR Tag = _T("CSmartStruct"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach(Struct.GetData(), Struct.GetBufferLen(), false);
	}
	CSmartStruct(const CSmartValue& Value, LPCTSTR Tag = _T("CSmartStruct"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach((void*)Value.GetData(), Value.GetBufferLen(), false);
	}
	CSmartStruct(CSmartStruct&& Struct)  noexcept
	{
		m_pData = Struct.m_pData;
		m_DataLen = Struct.m_DataLen;
		m_IsSelfData = Struct.m_IsSelfData;
		m_Tag = Struct.m_Tag;
		Struct.m_pData = NULL;
		Struct.m_DataLen = 0;
		Struct.m_IsSelfData = true;
		Struct.m_Tag = _T("CSmartStruct");
	}
	LPCTSTR GetTag()
	{
		return m_Tag;
	}
	void SetTag(LPCTSTR Tag)
	{
		m_Tag = Tag;
	}
	bool Create(UINT BufferLen)
	{
		Destory();
		m_DataLen = sizeof(BYTE) + sizeof(UINT) + BufferLen;
		m_pData = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_DataLen);
		ZeroMemory(m_pData, m_DataLen);
		*m_pData = CSmartValue::VT_STRUCT;
		*((UINT *)(m_pData + sizeof(BYTE))) = 0;
		m_IsSelfData = true;
		return true;
	}
	bool Expand(UINT ExpandSize)
	{
		UINT NewDataLen = 0;
		if (m_DataLen == 0)
		{
			NewDataLen = sizeof(BYTE) + sizeof(UINT) + ExpandSize;
		}
		else
		{
			NewDataLen += ExpandSize;
		}

		BYTE* pNewData = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_DataLen);
		if (m_DataLen == 0)
		{
			ZeroMemory(m_pData, m_DataLen);
			*m_pData = CSmartValue::VT_STRUCT;
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
		}
		else
		{
			memcpy(pNewData, m_pData, m_DataLen);
		}
		SAFE_DELETE_ARRAY(m_pData);
		m_pData = pNewData;
		m_DataLen = NewDataLen;
		m_IsSelfData = true;
		return true;
	}

	bool Attach(LPVOID pData, UINT DataLen, bool IsEmpty)
	{
		if ((pData == NULL) || (DataLen < sizeof(BYTE) + sizeof(UINT)))
			return false;
		Destory();
		m_IsSelfData = false;
		m_pData = (BYTE *)pData;
		if (IsEmpty)
		{
			m_pData[0] = CSmartValue::VT_STRUCT;
			Clear();
		}
		else
		{
			if (m_pData[0] != CSmartValue::VT_STRUCT)
			{
				Destory();
				return false;
			}
		}
		m_DataLen = DataLen;
		return true;
	}

	bool CloneFrom(const CSmartStruct& Value)
	{
		Destory();
		if (Value.GetData() && Value.m_DataLen)
		{
			m_IsSelfData = true;
			m_DataLen = Value.GetDataLen();
			m_pData = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_DataLen);
			memcpy(m_pData, Value.GetData(), Value.GetDataLen());
		}
		return true;
	}

	virtual ~CSmartStruct(void)
	{
		Destory();
	}


	void Destory()
	{
		if (m_IsSelfData)
		{
			SAFE_DELETE_ARRAY(m_pData);
		}
		else
		{
			m_pData = NULL;
		}
	}

	void Clear()
	{
		if (m_pData)
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
	}

	operator CSmartValue() const
	{
		return CSmartValue(GetData(), GetDataLen());
	}

	void operator=(const CSmartStruct& Struct)
	{
		Attach(Struct.GetData(), Struct.GetBufferLen(), false);
	}


	UINT GetLength() const
	{
		if (m_pData == NULL)
			return 0;
		return *((UINT *)(m_pData + sizeof(BYTE)));
	}

	UINT GetDataLen() const
	{
		if (m_pData == NULL)
			return 0;
		UINT DataLen = GetLength() + sizeof(BYTE) + sizeof(UINT);
		if (DataLen > m_DataLen)
			DataLen = m_DataLen;
		return DataLen;
	}
	UINT GetBufferLen() const
	{
		return m_DataLen;
	}
	LPVOID GetData() const
	{
		return m_pData;
	}
	bool AddMember(WORD ID, char Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_CHAR))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, unsigned char Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_UCHAR))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, short Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_SHORT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, unsigned short Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_USHORT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	//bool AddMember(WORD ID,long Value)
	//{
	//	UINT BufferSize;
	//	void * pBuffer=PrepareMember(BufferSize);
	//	if(pBuffer)
	//	{
	//		CSmartValue SmartValue;
	//		if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_INT))
	//		{
	//			SmartValue=Value;
	//			return FinishMember(ID,SmartValue.GetDataLen());
	//		}			
	//	}
	//	return false;
	//}
	//bool AddMember(WORD ID,unsigned long Value)
	//{
	//	UINT BufferSize;
	//	void * pBuffer=PrepareMember(BufferSize);
	//	if(pBuffer)
	//	{
	//		CSmartValue SmartValue;
	//		if(SmartValue.Attach(pBuffer,BufferSize,CSmartValue::VT_UINT))
	//		{
	//			SmartValue=Value;
	//			return FinishMember(ID,SmartValue.GetDataLen());
	//		}			
	//	}
	//	return false;
	//}
	bool AddMember(WORD ID, int Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_INT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, unsigned int Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_UINT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, __int64 Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_BIGINT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, unsigned __int64 Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_UBIGINT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, float Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_FLOAT))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, double Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_DOUBLE))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, bool Value)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_BOOL))
			{
				SmartValue.SetValue(Value);
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, const char * pszStr)
	{
		if (pszStr == NULL)
			return true;
		return AddMember(ID, pszStr, (UINT)strlen(pszStr));
	}
	bool AddMember(WORD ID, const WCHAR * pszStr)
	{
		if (pszStr == NULL)
			return true;
		return AddMember(ID, pszStr, (UINT)wcslen(pszStr));
	}
	bool AddMember(WORD ID, const CSmartValue& Value)
	{
		if (m_pData == NULL)
			return false;
		if (ID == 0)
			return false;
		if (Value.GetDataLen() <= 0)
		{
			//如果是空数据，插入一个NULL
			return AddMemberNull(ID);
		}
		UINT NeedSize = sizeof(WORD) + Value.GetDataLen();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD *)pFreeBuffer) = ID;
		pFreeBuffer += sizeof(WORD);
		memcpy(pFreeBuffer, Value.GetData(), Value.GetDataLen());
		*((UINT *)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	bool AddMember(WORD ID, const CSmartArray& Value)
	{
		if (m_pData == NULL)
			return false;
		if (ID == 0)
			return false;
		if (Value.GetDataLen() <= 0)
		{
			//如果是空数据，插入一个NULL
			return AddMemberNull(ID);
		}
		UINT NeedSize = sizeof(WORD) + Value.GetDataLen();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD*)pFreeBuffer) = ID;
		pFreeBuffer += sizeof(WORD);
		memcpy(pFreeBuffer, Value.GetData(), Value.GetDataLen());
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	bool AddMemberNull(WORD ID)
	{
		UINT BufferSize;
		void * pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			CSmartValue SmartValue;
			if (SmartValue.Attach(pBuffer, BufferSize, CSmartValue::VT_NULL))
			{
				return FinishMember(ID, SmartValue.GetDataLen());
			}
		}
		return false;
	}
	bool AddMember(WORD ID, const char * pszStr, UINT nStrLen, CEasyString::STRING_CODE_PAGE CodePage = CEasyString::STRING_CODE_PAGE_AUTO)
	{
		if (m_pData == NULL)
			return false;
		if (ID == 0)
			return false;

		UINT StrRealLen;
		UINT StrByteCount;
		UINT NeedSize;
		BYTE ValueType;
		if (CodePage == CEasyString::STRING_CODE_PAGE_AUTO)
			CodePage = CEasyString::SYSTEM_STRING_CODE_PAGE;
		switch (CodePage)
		{
		case CEasyString::STRING_CODE_PAGE_ANSI:
			switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				ValueType = CSmartValue::VT_STRING_ANSI;
				StrRealLen = nStrLen;
				StrByteCount = StrRealLen;
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = CSmartValue::VT_STRING_UTF8;
				StrRealLen = (UINT)AnsiToUTF8(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen;
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = CSmartValue::VT_STRING_UCS16;
				StrRealLen = (UINT)AnsiToUnicode(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen * sizeof(WCHAR);
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
				break;
			default:
				return false;
			}
			break;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				ValueType = CSmartValue::VT_STRING_ANSI;
				StrRealLen = (UINT)UTF8ToAnsi(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen;
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = CSmartValue::VT_STRING_UTF8;
				StrRealLen = nStrLen;
				StrByteCount = StrRealLen;
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = CSmartValue::VT_STRING_UCS16;
				StrRealLen = (UINT)UTF8ToUnicode(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen * sizeof(WCHAR);
				NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
				break;
			default:
				return false;
			}
			break;
		default:
			return false;
		}

		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD *)pFreeBuffer) = ID;
		pFreeBuffer += sizeof(WORD);
		*((BYTE *)pFreeBuffer) = ValueType;
		pFreeBuffer += sizeof(BYTE);
		*((UINT *)pFreeBuffer) = StrByteCount;
		pFreeBuffer += sizeof(UINT);

		if (pszStr&&nStrLen)
		{
			switch (CodePage)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
				{
				case CEasyString::STRING_CODE_PAGE_ANSI:
					memcpy(pFreeBuffer, pszStr, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UTF8:
					AnsiToUTF8(pszStr, nStrLen, (char *)pFreeBuffer, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UCS16:
					AnsiToUnicode(pszStr, nStrLen, (WCHAR *)pFreeBuffer, StrRealLen);
					((WCHAR *)pFreeBuffer)[StrRealLen] = 0;
					break;
				default:
					return false;
				}
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
				{
				case CEasyString::STRING_CODE_PAGE_ANSI:
					UTF8ToAnsi(pszStr, nStrLen, (char *)pFreeBuffer, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UTF8:
					memcpy(pFreeBuffer, pszStr, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UCS16:
					UTF8ToUnicode(pszStr, nStrLen, (WCHAR *)pFreeBuffer, StrRealLen);
					((WCHAR *)pFreeBuffer)[StrRealLen] = 0;
					break;
				default:
					return false;
				}
				break;
			}
		}
		*((UINT *)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	bool AddMember(WORD ID, const WCHAR * pszStr, UINT nStrLen)
	{
		if (m_pData == NULL)
			return false;
		if (ID == 0)
			return false;

		UINT StrRealLen;
		UINT StrByteCount;
		UINT NeedSize;
		BYTE ValueType;

		switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
		{
		case CEasyString::STRING_CODE_PAGE_ANSI:
			ValueType = CSmartValue::VT_STRING_ANSI;
			StrRealLen = (UINT)UnicodeToAnsi(pszStr, nStrLen, NULL, 0);
			StrByteCount = StrRealLen;
			NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
			break;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			ValueType = CSmartValue::VT_STRING_UTF8;
			StrRealLen = (UINT)UnicodeToUTF8(pszStr, nStrLen, NULL, 0);
			StrByteCount = StrRealLen;
			NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
			break;
		case CEasyString::STRING_CODE_PAGE_UCS16:
			ValueType = CSmartValue::VT_STRING_UCS16;
			StrRealLen = nStrLen;
			StrByteCount = StrRealLen * sizeof(WCHAR);
			NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
			break;
		default:
			return false;
		}
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD *)pFreeBuffer) = ID;
		pFreeBuffer += sizeof(WORD);
		*((BYTE *)pFreeBuffer) = ValueType;
		pFreeBuffer += sizeof(BYTE);
		*((UINT *)pFreeBuffer) = StrByteCount;
		pFreeBuffer += sizeof(UINT);
		if (pszStr&&StrRealLen)
		{
			switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				UnicodeToAnsi(pszStr, nStrLen, (char *)pFreeBuffer, StrRealLen);
				pFreeBuffer[StrRealLen] = 0;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				UnicodeToUTF8(pszStr, nStrLen, (char *)pFreeBuffer, StrRealLen);
				pFreeBuffer[StrRealLen] = 0;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				memcpy(pFreeBuffer, pszStr, sizeof(WCHAR)*StrRealLen);
				((WCHAR *)pFreeBuffer)[StrRealLen] = 0;
				break;
			default:
				return false;
			}
		}
		*((UINT *)(m_pData + 1)) += NeedSize;
		return true;
	}
	bool AddMember(WORD ID, const CEasyString& Str)
	{
		return AddMember(ID, Str, (UINT)Str.GetLength());
	}
	bool AddMember(WORD ID, const unsigned char * pData, UINT DataLen)
	{
		if (ID == 0)
			return false;
		if ((DataLen > 0) && (m_pData == NULL))
			return false;

		UINT NeedSize = sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + DataLen;
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD *)pFreeBuffer) = ID;
		pFreeBuffer += sizeof(WORD);
		*((BYTE *)pFreeBuffer) = CSmartValue::VT_BINARY;
		pFreeBuffer += sizeof(BYTE);
		*((UINT *)pFreeBuffer) = DataLen;
		pFreeBuffer += sizeof(UINT);
		memcpy(pFreeBuffer, pData, DataLen);
		*((UINT *)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	bool AddMember(WORD ID, const CEasyBuffer& Data)
	{
		return AddMember(ID, (const unsigned char*)Data.GetBuffer(), (UINT)Data.GetUsedSize());
	}
	bool AddMember(WORD ID, const CVariedValue& Value)
	{
		switch (Value.GetType())
		{
		case VARIED_VALUE_TYPE::NIL:
			return AddMemberNull(ID);
		case VARIED_VALUE_TYPE::BOOLEAN:
			return AddMember(ID, (bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return AddMember(ID, (char)Value);
		case VARIED_VALUE_TYPE::UINT8:
			return AddMember(ID, (unsigned char)Value);
		case VARIED_VALUE_TYPE::INT16:
			return AddMember(ID, (short)Value);
		case VARIED_VALUE_TYPE::UINT16:
			return AddMember(ID, (unsigned short)Value);
		case VARIED_VALUE_TYPE::INT32:
			return AddMember(ID, (int)Value);
		case VARIED_VALUE_TYPE::UINT32:
			return AddMember(ID, (unsigned int)Value);
		case VARIED_VALUE_TYPE::INT64:
			return AddMember(ID, (__int64)Value);
		case VARIED_VALUE_TYPE::UINT64:
			return AddMember(ID, (unsigned __int64)Value);
		case VARIED_VALUE_TYPE::FLOAT32:
			return AddMember(ID, (float)Value);
		case VARIED_VALUE_TYPE::FLOAT64:
			return AddMember(ID, (double)Value);
		case VARIED_VALUE_TYPE::STRING:
			return AddMember(ID, (LPCTSTR)Value);
		case VARIED_VALUE_TYPE::BINARY:
			return AddMember(ID, (const unsigned char*)Value, (UINT)Value.GetLength());
		case VARIED_VALUE_TYPE::ARRAY:
			{
				CSmartValue Member;
				if (PrepareMember(CSmartValue::VT_ARRAY, Member))
				{
					if (Member.SetValue(Value))
						return FinishMember(ID, Member.GetDataLen());
				}				
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			{
				CSmartValue Member;
				if (PrepareMember(CSmartValue::VT_STRUCT, Member))
				{
					if (Member.SetValue(Value))
						return FinishMember(ID, Member.GetDataLen());
				}					
			}
			break;
		}
		return false;
	}
	void * PrepareMember(UINT& nUsableSize)
	{
		if (m_pData == NULL)
			return NULL;
		if (GetFreeLen() < sizeof(WORD))
		{
			nUsableSize = 0;
			return NULL;
		}
		nUsableSize = GetFreeLen() - sizeof(WORD);
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength() + sizeof(WORD);
		return pFreeBuffer;
	}
	bool PrepareMember(CSmartValue::SMART_VALUE_TYPE Type, CSmartValue& Value)
	{
		UINT BufferSize;
		void* pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			Value.Attach(pBuffer, BufferSize, Type);
			return true;
		}
		return false;
	}
	CSmartValue PrepareMember(CSmartValue::SMART_VALUE_TYPE Type)
	{
		CSmartValue Value;
		UINT BufferSize;
		void* pBuffer = PrepareMember(BufferSize);
		if (pBuffer)
		{
			Value.Attach(pBuffer, BufferSize, Type);
		}
		return Value;
	}
	CSmartArray PrepareSubArray()
	{
		CSmartArray SubArray;
		if (m_pData)
		{
			if (GetFreeLen() >= GetStructMemberSize(0))
			{
				UINT nUsableSize = GetFreeLen() - sizeof(WORD);
				BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength() + sizeof(WORD);
				SubArray.Attach(pFreeBuffer, nUsableSize, true);
			}
		}
		return SubArray;
	}
	CSmartStruct PrepareSubStruct()
	{
		CSmartStruct SubStruct;
		if (m_pData)
		{
			if (GetFreeLen() >= GetStructMemberSize(0))
			{
				UINT nUsableSize = GetFreeLen() - sizeof(WORD);
				BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength() + sizeof(WORD);
				SubStruct.Attach(pFreeBuffer, nUsableSize, true);
			}
		}
		return SubStruct;
	}
	bool FinishMember(WORD ID, UINT MemberSize)
	{
		if (m_pData == NULL)
			return false;
		if (ID == 0)
			return false;
		UINT NeedSize = sizeof(WORD) + MemberSize;
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((WORD *)pFreeBuffer) = ID;
		*((UINT *)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}

	bool Append(const CSmartStruct& Struct)
	{
		if (m_pData == NULL)
			return false;
		if (Struct.m_pData == NULL)
			return false;
		if (Struct.m_pData[0] != CSmartValue::VT_STRUCT)
			return false;
		UINT NeedSize = Struct.GetLength();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		BYTE * pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();

		memcpy(pFreeBuffer, Struct.m_pData + sizeof(BYTE) + sizeof(UINT), NeedSize);
		*((UINT *)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	int GetMemberCount() const
	{
		if (m_pData == NULL)
			return 0;
		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();
		int MemberCount = 0;
		pHead += sizeof(BYTE) + sizeof(UINT);
		while (pHead + sizeof(WORD) < pTail)
		{
			pHead += sizeof(WORD);

			CSmartValue Value(pHead, (UINT)(pTail - pHead));

			pHead += Value.GetDataLen();
			MemberCount++;
		}
		return MemberCount;
	}
	CSmartValue GetMemberByIndex(UINT Index) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
			return Value;

		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();

		pHead += sizeof(BYTE) + sizeof(UINT);
		while (pHead + sizeof(WORD) < pTail)
		{
			pHead += sizeof(WORD);
			if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				break;
			pHead += Value.GetDataLen();
			if (Index == 0)
				return Value;
			Index--;
		}
		Value.Destory();
		return Value;

	}
	CSmartValue GetMember(WORD ID) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
			return Value;

		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();

		pHead += sizeof(BYTE) + sizeof(UINT);
		while (pHead + sizeof(WORD) < pTail)
		{
			WORD MemberID = *((WORD *)pHead);
			pHead += sizeof(WORD);
			if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				break;
			if (MemberID == ID)
				return Value;
			pHead += Value.GetDataLen();
		}
		Value.Destory();
		return Value;
	}
	bool IsMemberExist(WORD ID) const
	{
		if (IDToIndex(ID) == INVALID_MEMBER_ID)
		{
			return false;
		}
		return true;
	}
	UINT IDToIndex(WORD ID) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
			return INVALID_MEMBER_ID;

		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();

		UINT Index = 0;

		pHead += sizeof(BYTE) + sizeof(UINT);
		while (pHead + sizeof(WORD) < pTail)
		{
			WORD MemberID = *((WORD *)pHead);
			pHead += sizeof(WORD);
			if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				break;
			if (MemberID == ID)
				return Index;
			pHead += Value.GetDataLen();
			Index++;
		}
		return INVALID_MEMBER_ID;
	}
	WORD IndexToID(UINT Index) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
			return 0;

		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();
		WORD MemberID = 0;

		pHead += sizeof(BYTE) + sizeof(UINT);
		while (pHead + sizeof(WORD) < pTail)
		{
			MemberID = *((WORD *)pHead);
			if (Index == 0)
				return MemberID;
			pHead += sizeof(WORD);
			if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				break;
			pHead += Value.GetDataLen();
			Index--;
		}
		return 0;
	}
	Pair GetPairByPos(void* Pos)
	{
		Pair pair;
		if (m_pData == NULL)
			return pair;
		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();
			pHead += sizeof(WORD);
			if (pHead < pTail)			
			{
				if (!pair.Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
					return pair;
			}
		}
		return pair;
	}
	const Pair GetPairByPos(void* Pos) const
	{
		Pair pair;
		if (m_pData == NULL)
			return pair;
		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();
			pHead += sizeof(WORD);
			if (pHead < pTail)
			{
				if (!pair.Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
					return pair;
			}
		}
		return pair;
	}
	LPVOID GetFirstMemberPosition() const
	{
		if (m_pData == NULL)
			return NULL;


		BYTE * pHead = m_pData;
		BYTE * pTail = m_pData + GetDataLen();
		pHead += sizeof(BYTE) + sizeof(UINT);

		if (pHead < pTail)
			return pHead;
		else
			return NULL;
	}
	CSmartValue GetNextMember(void *& Pos, WORD& MemberID) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
		{
			Pos = NULL;
			return Value;
		}


		if (Pos)
		{
			BYTE * pHead = (BYTE *)Pos;
			BYTE * pTail = m_pData + GetDataLen();
			MemberID = *((WORD *)pHead);

			pHead += sizeof(WORD);

			if (pHead >= pTail)
			{
				Pos = NULL;
			}
			else
			{
				if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				{
					Pos = NULL;
					return Value;
				}
				Pos = pHead + Value.GetDataLen();
				if (Pos >= pTail)
				{
					Pos = NULL;
				}
			}

		}
		return Value;
	}	
	void* GetNextMemberPos(void* Pos) const
	{
		if (m_pData == NULL)
			return NULL;
		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();		
			pHead += sizeof(WORD);
			if (pHead >= pTail)
			{
				return NULL;
			}
			else
			{
				CSmartValue Value;
				if (!Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
				{
					return NULL;
				}
				Pos = pHead + Value.GetDataLen();
				if (Pos >= pTail)
				{
					return NULL;
				}
			}

		}
		return Pos;
	}
	iterator begin()
	{
		return iterator(this, GetFirstMemberPosition());
	}

	iterator end()
	{
		return iterator(this, NULL);
	}
	const_iterator begin() const
	{
		return const_iterator(this, GetFirstMemberPosition());
	}

	const_iterator end() const
	{
		return const_iterator(this, NULL);
	}
	void Dump(CStringBuilder& OutBuffer, bool HaveType);
	static inline UINT GetDataLenFromData(LPCVOID pData, UINT DataSize)
	{
		if (pData == NULL || DataSize < HEAD_SIZE)
			return 0;
		return *((UINT *)((BYTE *)pData + sizeof(BYTE))) + HEAD_SIZE;
	}

	static inline UINT GetEmptyStructSize()
	{
		return sizeof(BYTE) + sizeof(UINT);
	}

	static inline UINT GetFixMemberSize(UINT TypeLen)
	{
		return sizeof(WORD) + sizeof(BYTE) + TypeLen;
	}
	static inline UINT GetArrayMemberSize(UINT ArraySize)
	{
		return sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + ArraySize;
	}
	static inline UINT GetStructMemberSize(UINT StructSize)
	{
		return sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + StructSize;
	}
	static inline UINT GetStringMemberSize(const CEasyString& Str)
	{
		return GetStringMemberSize(Str, Str.GetLength());	
	}
	static inline UINT GetStringMemberSize(LPCWSTR szStr)
	{
		return GetStringMemberSize(szStr, wcslen(szStr));
	}
	static inline UINT GetStringMemberSize(LPCWSTR szStr, size_t StrLen)
	{
		CEasyString::STRING_CODE_PAGE TargetCodePage = CSmartValue::INTERNAL_STRING_CODE_PAGE;
		if (TargetCodePage == CEasyString::STRING_CODE_PAGE_AUTO)
			TargetCodePage = CEasyString::SYSTEM_STRING_CODE_PAGE;
		switch (TargetCodePage)
		{
		case CEasyString::STRING_CODE_PAGE_UTF8:
			return (szStr ? (UINT)UnicodeToUTF8(szStr, StrLen, NULL, 0) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		case CEasyString::STRING_CODE_PAGE_UCS16:
			return (UINT)StrLen * sizeof(WCHAR) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		default:
			return (szStr ? (UINT)UnicodeToAnsi(szStr, StrLen, NULL, 0) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		}			
	}
	static inline UINT GetStringMemberSize(LPCSTR szStr)
	{
		return GetStringMemberSize(szStr, strlen(szStr));
	}
	static inline UINT GetStringMemberSize(LPCSTR szStr, size_t StrLen)
	{
		CEasyString::STRING_CODE_PAGE TargetCodePage = CSmartValue::INTERNAL_STRING_CODE_PAGE;
		if (TargetCodePage == CEasyString::STRING_CODE_PAGE_AUTO)
			TargetCodePage = CEasyString::SYSTEM_STRING_CODE_PAGE;
		switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
		{
		case CEasyString::STRING_CODE_PAGE_UTF8:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
				return (szStr ? (UINT)AnsiToUTF8(szStr, StrLen, NULL, 0) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			else
				return (UINT)StrLen + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		case CEasyString::STRING_CODE_PAGE_UCS16:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
				return (szStr ? (UINT)AnsiToUnicode(szStr, StrLen, NULL, 0) * sizeof(WCHAR) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
			else
				return (szStr ? (UINT)UTF8ToUnicode(szStr, StrLen, NULL, 0) * sizeof(WCHAR) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		default:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
				return (szStr ? (UINT)UTF8ToAnsi(szStr, StrLen, NULL, 0) : 0) + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			else
				return (UINT)StrLen + sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		}
		
	}
	inline static UINT GetBinaryMemberSize(UINT DataLen)
	{
		return sizeof(WORD) + sizeof(BYTE) + sizeof(UINT) + DataLen;
	}
	static UINT GetVariedMemberSize(const CVariedValue& Value);
	UINT GetFreeLen()
	{
		return m_DataLen-GetDataLen();
	}
	
#ifdef RAPIDJSON_DOCUMENT_H_
	rapidjson::Value ToJson(rapidjson::Document::AllocatorType& Alloc) const;
#endif
};

template<typename T>
bool CSmartArray::PackArray(const CEasyArray<T>& ObjArray)
{
	for (const T& Obj : ObjArray)
	{
		CSmartStruct SubPacket = PrepareSubStruct();
		if (!Obj.MakePacket(SubPacket))
			return false;
		if (!FinishMember(SubPacket.GetDataLen()))
			return false;
	}
	return true;
}

template<typename T, typename F>
bool CSmartArray::PackArray(const CEasyArray<T>& ObjArray, const F& Flags)
{
	for (const T& Obj : ObjArray)
	{
		CSmartStruct SubPacket = PrepareSubStruct();
		if (!Obj.MakePacket(SubPacket, Flags))
			return false;
		if (!FinishMember(SubPacket.GetDataLen()))
			return false;
	}
	return true;
}