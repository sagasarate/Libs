/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartStruct.h                                            */
/*      创建日期:  2022年07月30日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CSmartStruct;

class CSmartArray
{
protected:
	BYTE*			m_pData;
	UINT			m_DataLen;
	int				m_ElementSize;
	bool			m_IsSelfData;
	LPCTSTR			m_Tag;
public:	
	enum
	{
		HEAD_SIZE = (sizeof(BYTE) + sizeof(UINT)),
	};
	class iterator
	{
	protected:
		CSmartArray* m_pParent;
		void*		 m_Pos;
	public:
		iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		iterator(CSmartArray* pArray, void* Pos)
		{
			m_pParent = pArray;
			m_Pos = Pos;			
		}
		iterator(const iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		CSmartValue operator*()
		{
			return m_pParent->GetMemberByPos(m_Pos);
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
		const CSmartArray* m_pParent;
		void* m_Pos;
	public:
		const_iterator()
		{
			m_pParent = NULL;
			m_Pos = NULL;
		}
		const_iterator(const CSmartArray* pArray, void* Pos)
		{
			m_pParent = pArray;
			m_Pos = Pos;
		}
		const_iterator(const const_iterator& itr)
		{
			m_pParent = itr.m_pParent;
			m_Pos = itr.m_Pos;
		}
		const CSmartValue operator*()
		{
			return m_pParent->GetMemberByPos(m_Pos);
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
		bool operator !=(const const_iterator & itr)
		{
			return (m_pParent != itr.m_pParent) || (m_Pos != itr.m_Pos);
		}
	};
	CSmartArray(LPCTSTR Tag = _T("CSmartArray"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_ElementSize = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
	}
	CSmartArray(int BufferLen, LPCTSTR Tag = _T("CSmartArray"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_ElementSize = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Create(BufferLen);
	}
	CSmartArray(LPVOID pData, UINT DataLen, bool IsEmpty, LPCTSTR Tag = _T("CSmartArray"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_ElementSize = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach(pData, DataLen, IsEmpty);
	}
	CSmartArray(const CSmartArray& Array, LPCTSTR Tag = _T("CSmartArray"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_ElementSize = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach((void*)Array.GetData(), Array.GetBufferLen(), false);
	}
	CSmartArray(const CSmartValue& Value, LPCTSTR Tag = _T("CSmartArray"))
	{
		m_pData = NULL;
		m_DataLen = 0;
		m_ElementSize = 0;
		m_IsSelfData = true;
		m_Tag = Tag;
		Attach((void*)Value.GetData(), Value.GetBufferLen(), false);
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
		*m_pData = CSmartValue::VT_ARRAY;
		*((UINT*)(m_pData + sizeof(BYTE))) = 0;
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
			*m_pData = CSmartValue::VT_ARRAY;
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
		m_pData = (BYTE*)pData;
		if (IsEmpty)
		{
			m_pData[0] = CSmartValue::VT_ARRAY;
			Clear();
		}
		else
		{
			if (m_pData[0] != CSmartValue::VT_ARRAY)
			{
				Destory();
				return false;
			}
		}
		m_DataLen = DataLen;
		CheckElements();
		return true;
	}

	bool CloneFrom(const CSmartArray& Value)
	{
		Destory();
		if (Value.GetData() && Value.m_DataLen)
		{
			m_IsSelfData = true;
			m_DataLen = Value.GetDataLen();
			m_ElementSize = Value.m_ElementSize;
			m_pData = MONITORED_NEW_ARRAY(GetTag(), BYTE, m_DataLen);
			memcpy(m_pData, Value.GetData(), Value.GetDataLen());
		}
		return true;
	}

	virtual ~CSmartArray(void)
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
		m_IsSelfData = false;
		m_DataLen = 0;
		m_ElementSize = 0;
	}

	void Clear()
	{
		if (m_pData)
		{
			*((UINT*)(m_pData + sizeof(BYTE))) = 0;
			m_ElementSize = 0;
		}			
	}

	operator CSmartValue() const
	{
		return CSmartValue((void*)GetData(), GetDataLen());
	}

	void operator=(const CSmartArray& Struct)
	{
		Attach((void*)Struct.GetData(), Struct.GetBufferLen(), false);
	}

	UINT GetArrayLength() const
	{
		if ((m_pData == NULL) || (m_DataLen < sizeof(BYTE) + sizeof(UINT)))
			return 0;
		const BYTE* pArrayData = (const BYTE*)m_pData;
		pArrayData += sizeof(BYTE);
		UINT ArrayDataLen = *((UINT*)pArrayData);
		pArrayData += sizeof(UINT);		
		if (ArrayDataLen + sizeof(BYTE) + sizeof(UINT)> m_DataLen)
			ArrayDataLen = m_DataLen - sizeof(BYTE) - sizeof(UINT);
		if(ArrayDataLen)
		{
			if (m_ElementSize)
			{
				//定长元素
				CSmartValue Value;
				if (Value.Attach((void*)pArrayData, ArrayDataLen, CSmartValue::VT_UNKNOWN))
				{
					return ArrayDataLen / Value.GetDataLen();
				}
			}
			else
			{
				//变长元素
				CSmartValue Value;
				UINT Count = 0;
				const BYTE* pHead = pArrayData;
				const BYTE* pTail = pArrayData + ArrayDataLen;

				while (pHead + sizeof(BYTE) < pTail)
				{
					if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
						break;
					Count++;
					pHead += Value.GetDataLen();
				}
				return Count;
			}
		}
		return  0;
	}	
	
	UINT GetLength() const
	{
		if (m_pData == NULL)
			return 0;
		return *((UINT*)(m_pData + sizeof(BYTE)));
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
		return m_pData + sizeof(BYTE) + sizeof(UINT);
	}
	LPCVOID GetValueData() const
	{
		return m_pData + sizeof(BYTE) + sizeof(UINT);
	}
	BYTE GetElementType() const
	{
		if (m_ElementSize && GetLength())
		{
			return *(m_pData + sizeof(BYTE) + sizeof(UINT));
		}
		return CSmartValue::VT_UNKNOWN;
	}
	UINT GetElementSize() const
	{		
		return m_ElementSize;
	}
	CSmartValue GetMember(UINT Index) const
	{
		if (m_ElementSize)
		{
			//定长元素
			CSmartValue Value;

			UINT ArrayLen = GetLength() / m_ElementSize;
			if (Index < ArrayLen)
			{
				Value.Attach((BYTE*)GetValueData() + m_ElementSize * Index, m_ElementSize, CSmartValue::VT_UNKNOWN);
				return Value;
			}
		}
		else
		{
			//变长元素
			CSmartValue Value;
			const BYTE* pHead = (BYTE*)GetValueData();
			const BYTE* pTail = pHead + GetLength();

			while (pHead + sizeof(BYTE) <= pTail)
			{
				if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
					break;
				if (Index == 0)
					return Value;
				Index--;
				pHead += Value.GetDataLen();
			}
		}
		return CSmartValue();
	}
	CSmartValue operator[](UINT Index) const
	{
		return GetMember(Index);
	}
	CSmartValue GetMemberByPos(void* Pos)
	{
		CSmartValue Value;
		if (m_pData == NULL)
			return Value;
		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();
			if (pHead < pTail)
			{
				Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN);
			}
		}
		return Value;
	}
	const CSmartValue GetMemberByPos(void* Pos) const
	{
		CSmartValue Value;
		if (m_pData == NULL)
			return Value;
		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();
			if (pHead < pTail)
			{
				Value.Attach(pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN);
			}
		}
		return Value;
	}
	LPVOID GetFirstMemberPosition() const
	{
		if (m_pData == NULL)
			return NULL;


		BYTE* pHead = (BYTE*)GetValueData();
		BYTE* pTail = pHead + GetLength();

		if (pHead < pTail)
			return pHead;
		else
			return NULL;
	}

	CSmartValue GetNextMember(void*& Pos) const
	{
		CSmartValue Value;

		if (m_pData == NULL)
		{
			Pos = NULL;
			return Value;
		}

		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();			

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
	CSmartStruct GetNextStruct(void*& Pos) const;
	CSmartArray GetNextArray(void*& Pos) const
	{
		CSmartArray Value;

		if (m_pData == NULL)
		{
			Pos = NULL;
			return Value;
		}


		if (Pos)
		{
			BYTE* pHead = (BYTE*)Pos;
			BYTE* pTail = m_pData + GetDataLen();

			if (pHead >= pTail)
			{
				Pos = NULL;
			}
			else
			{
				if (!Value.Attach(pHead, (UINT)(pTail - pHead), false))
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
	bool GetArray(CEasyArray<T>& Array) const
	{
		if (GetLength())
		{
			if (m_ElementSize)
			{
				//定长元素
				CSmartValue Value;

				UINT ArrayLen = GetLength() / m_ElementSize;
				Array.Resize(ArrayLen);
				for (UINT i = 0; i < ArrayLen; i++)
				{
					if (!Value.Attach((BYTE*)GetValueData() + m_ElementSize * i, m_ElementSize, CSmartValue::VT_UNKNOWN))
						return false;
					Array[i] = Value;
				}
				
			}
			else
			{
				//变长元素
				CSmartValue Value;
				const BYTE* pHead = (BYTE*)GetValueData();
				const BYTE* pTail = pHead + GetLength();
				Array.Resize(GetArrayLength());
				UINT Index = 0;
				while ((pHead + sizeof(BYTE) <= pTail) && (Index < Array.GetCount()))
				{
					if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
						return false;
					Array[Index] = Value;
					Index++;
					pHead += Value.GetDataLen();
				}
			}

			return true;
		}
		else
		{
			return true;
		}
	}	
	bool GetArray(CEasyArray<CEasyString>& Array) const
	{
		if (GetLength())
		{
			CSmartValue Value;
			const BYTE* pHead = (BYTE*)GetValueData();
			const BYTE* pTail = pHead + GetLength();
			Array.Resize(GetArrayLength());
			UINT Index = 0;
			while ((pHead + sizeof(BYTE) <= pTail) && (Index < Array.GetCount()))
			{
				if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
					return false;
				Value.GetString(Array[Index]);
				Index++;
				pHead += Value.GetDataLen();
			}

			return true;
		}
		else
		{
			return true;
		}
	}
	bool GetArray(CEasyArray<CEasyBuffer>& Array) const
	{
		if (GetLength())
		{
			CSmartValue Value;
			const BYTE* pHead = (BYTE*)GetValueData();
			const BYTE* pTail = pHead + GetLength();
			Array.Resize(GetArrayLength());
			UINT Index = 0;
			while ((pHead + sizeof(BYTE) <= pTail) && (Index < Array.GetCount()))
			{
				if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
					return false;
				Array[Index].Create(Value.GetLength());
				Array[Index].PushBack(Value.GetValueData(), Value.GetLength());
				Index++;
				pHead += Value.GetDataLen();
			}
			return true;
		}
		else
		{
			return true;
		}
	}
	bool GetArray(CEasyArray<CSmartStruct>& Array) const;
	bool GetArray(CEasyArray<CVariedValue>& Array) const
	{
		if (GetLength())
		{
			if (m_ElementSize)
			{
				//定长元素
				CSmartValue Value;

				UINT ArrayLen = GetLength() / m_ElementSize;
				Array.Resize(ArrayLen);
				for (UINT i = 0; i < ArrayLen; i++)
				{
					if (!Value.Attach((BYTE*)GetValueData() + m_ElementSize * i, m_ElementSize, CSmartValue::VT_UNKNOWN))
						return false;
					Value.GetVariedValue(Array[i]);
				}

			}
			else
			{
				//变长元素
				CSmartValue Value;
				const BYTE* pHead = (BYTE*)GetValueData();
				const BYTE* pTail = pHead + GetLength();
				Array.Resize(GetArrayLength());
				UINT Index = 0;
				while ((pHead + sizeof(BYTE) <= pTail) && (Index < Array.GetCount()))
				{
					if (!Value.Attach((BYTE*)pHead, (UINT)(pTail - pHead), CSmartValue::VT_UNKNOWN))
						return false;
					Value.GetVariedValue(Array[Index]);
					Index++;
					pHead += Value.GetDataLen();
				}
			}

			return true;
		}
		else
		{
			return true;
		}
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
	bool AddArray(const CEasyArray<T>& Array)
	{
		if (m_pData)
		{
			if(Array.GetCount())
			{
				BYTE NewElementType = CSmartValue::GetTypeByValue(Array[0]);
				UINT NewElementSize = sizeof(BYTE) + sizeof(Array[0]);
				UINT NeedSize = (UINT)(NewElementSize * Array.GetCount());
				if (GetFreeLen() >= NeedSize)
				{
					BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
					CSmartValue Element;
					for (const T& Value : Array)
					{
						if (Element.Attach(pFreeBuffer, NewElementSize, NewElementType))
							Element.SetValue(Value);
						else
							return false;
						pFreeBuffer += NewElementSize;
					}
					return FinishMember(NeedSize);
				}				
			}
			else
			{
				return true;
			}			
		}
		return false;
	}
	bool AddArray(const CEasyArray<CEasyString>& Array)
	{
		if (m_pData)
		{
			UINT NeedSize = 0;
			for (const CEasyString& Value : Array)
			{
				NeedSize += GetStringMemberSize(Value);
			}
			if (GetFreeLen() >= NeedSize)
			{
				for (const CEasyString& Value : Array)
				{
					if (!AddMember(Value))
						return false;
				}
			}
			return true;
		}
		return false;
	}
	bool AddArray(const CEasyArray<CEasyBuffer>& Array)
	{
		if (m_pData)
		{
			UINT NeedSize = 0;
			for (const CEasyBuffer& Value : Array)
			{
				NeedSize += GetBinaryMemberSize(Value.GetUsedSize());
			}
			if (GetFreeLen() >= NeedSize)
			{
				for (const CEasyBuffer& Value : Array)
				{
					if (!AddMember(Value))
						return false;
				}
				return true;
			}
		}
		return false;
	}
	bool AddArray(const CEasyArray<CSmartStruct>& Array);
	bool AddArray(const CEasyArray<CVariedValue>& Array)
	{
		if (m_pData)
		{
			UINT NeedSize = 0;
			for (const CVariedValue& Value : Array)
			{
				NeedSize += GetVariedMemberSize(Value);
			}
			if (GetFreeLen() >= NeedSize)
			{
				for (const CVariedValue& Value : Array)
				{
					if (!AddMember(Value))
						return false;
				}
				return true;
			}
		}
		return false;
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
	static inline UINT GetDataLenFromData(LPCVOID pData, UINT DataSize)
	{
		if (pData == NULL || DataSize < HEAD_SIZE)
			return 0;
		return *((UINT*)((BYTE*)pData + sizeof(BYTE))) + HEAD_SIZE;
	}
	UINT GetFreeLen()
	{
		return m_DataLen - GetDataLen();
	}
	CSmartStruct PrepareSubStruct();

	CSmartArray PrepareArrayMember()
	{
		CSmartArray Array;
		BYTE* pTail = m_pData + GetDataLen();
		UINT FreeSize = GetFreeLen();
		Array.Attach(pTail, FreeSize, true);
		return Array;
	}
	bool FinishMember(UINT MemberSize)
	{
		if (m_pData == NULL)
			return false;
		UINT NeedSize = MemberSize;
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}
		
		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		UINT NewElementSize = GetElementSizeByType(*pFreeBuffer);		
		if (GetLength())
		{
			//如果新元素和已有元素大小不同，ElementSize设置为0代表改数组不是规则数组			
			if (m_ElementSize != NewElementSize)
				m_ElementSize = 0;
		}
		else
		{
			m_ElementSize = NewElementSize;
		}		

		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		return true;
	}
	bool AddMemberNull()
	{
		CSmartValue SmartValue;
		if (PrepareMember(VT_NULL, SmartValue))
		{
			SmartValue.SetNull();
			return FinishMember(SmartValue.GetDataLen());
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
	bool AddMember(T Value)
	{
		CSmartValue SmartValue;
		if (PrepareMember(CSmartValue::GetTypeByValue(Value), SmartValue))
		{
			SmartValue.SetValue(Value);
			return FinishMember(SmartValue.GetDataLen());
		}
		return false;
	}	
	bool AddMember(const char* pszStr)
	{
		if (pszStr == NULL)
			return false;
		return AddMember(pszStr, (UINT)strlen(pszStr));
	}
	bool AddMember(const WCHAR* pszStr)
	{
		if (pszStr == NULL)
			return true;
		return AddMember(pszStr, (UINT)wcslen(pszStr));
	}
	bool AddMember(const char* pszStr, UINT nStrLen, CEasyString::STRING_CODE_PAGE CodePage = CEasyString::STRING_CODE_PAGE_AUTO)
	{
		if (m_pData == NULL)
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
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = CSmartValue::VT_STRING_UTF8;
				StrRealLen = (UINT)AnsiToUTF8(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen;
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = CSmartValue::VT_STRING_UCS16;
				StrRealLen = (UINT)AnsiToUnicode(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen * sizeof(WCHAR);
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
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
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				ValueType = CSmartValue::VT_STRING_UTF8;
				StrRealLen = nStrLen;
				StrByteCount = StrRealLen;
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				ValueType = CSmartValue::VT_STRING_UCS16;
				StrRealLen = (UINT)UTF8ToUnicode(pszStr, nStrLen, NULL, 0);
				StrByteCount = StrRealLen * sizeof(WCHAR);
				NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
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
		
		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((BYTE*)pFreeBuffer) = ValueType;
		pFreeBuffer += sizeof(BYTE);
		*((UINT*)pFreeBuffer) = StrByteCount;
		pFreeBuffer += sizeof(UINT);

		if (pszStr && nStrLen)
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
					AnsiToUTF8(pszStr, nStrLen, (char*)pFreeBuffer, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UCS16:
					AnsiToUnicode(pszStr, nStrLen, (WCHAR*)pFreeBuffer, StrRealLen);
					((WCHAR*)pFreeBuffer)[StrRealLen] = 0;
					break;
				default:
					return false;
				}
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
				{
				case CEasyString::STRING_CODE_PAGE_ANSI:
					UTF8ToAnsi(pszStr, nStrLen, (char*)pFreeBuffer, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UTF8:
					memcpy(pFreeBuffer, pszStr, StrRealLen);
					pFreeBuffer[StrRealLen] = 0;
					break;
				case CEasyString::STRING_CODE_PAGE_UCS16:
					UTF8ToUnicode(pszStr, nStrLen, (WCHAR*)pFreeBuffer, StrRealLen);
					((WCHAR*)pFreeBuffer)[StrRealLen] = 0;
					break;
				default:
					return false;
				}
				break;
			}
		}
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		m_ElementSize = 0;
		return true;
	}
	bool AddMember(const WCHAR* pszStr, UINT nStrLen)
	{
		if (m_pData == NULL)
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
			NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
			break;
		case CEasyString::STRING_CODE_PAGE_UTF8:
			ValueType = CSmartValue::VT_STRING_UTF8;
			StrRealLen = (UINT)UnicodeToUTF8(pszStr, nStrLen, NULL, 0);
			StrByteCount = StrRealLen;
			NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen + 1;
			break;
		case CEasyString::STRING_CODE_PAGE_UCS16:
			ValueType = CSmartValue::VT_STRING_UCS16;
			StrRealLen = nStrLen;
			StrByteCount = StrRealLen * sizeof(WCHAR);
			NeedSize = sizeof(BYTE) + sizeof(UINT) + StrRealLen * sizeof(WCHAR) + sizeof(WCHAR);
			break;
		default:
			return false;
		}
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}		

		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((BYTE*)pFreeBuffer) = ValueType;
		pFreeBuffer += sizeof(BYTE);
		*((UINT*)pFreeBuffer) = StrByteCount;
		pFreeBuffer += sizeof(UINT);
		if (pszStr && StrRealLen)
		{
			switch (CSmartValue::INTERNAL_STRING_CODE_PAGE)
			{
			case CEasyString::STRING_CODE_PAGE_ANSI:
				UnicodeToAnsi(pszStr, nStrLen, (char*)pFreeBuffer, StrRealLen);
				pFreeBuffer[StrRealLen] = 0;
				break;
			case CEasyString::STRING_CODE_PAGE_UTF8:
				UnicodeToUTF8(pszStr, nStrLen, (char*)pFreeBuffer, StrRealLen);
				pFreeBuffer[StrRealLen] = 0;
				break;
			case CEasyString::STRING_CODE_PAGE_UCS16:
				memcpy(pFreeBuffer, pszStr, sizeof(WCHAR) * StrRealLen);
				((WCHAR*)pFreeBuffer)[StrRealLen] = 0;
				break;
			default:
				return false;
			}
		}
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		m_ElementSize = 0;
		return true;
	}
	bool AddMember(const CEasyString& Str)
	{
		return AddMember(Str, (UINT)Str.GetLength());
	}
	bool AddMember(const unsigned char* pData, UINT DataLen)
	{
		if ((DataLen > 0) && (m_pData == NULL))
			return false;

		UINT NeedSize = sizeof(BYTE) + sizeof(UINT) + DataLen;
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}	

		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		*((BYTE*)pFreeBuffer) = CSmartValue::VT_BINARY;
		pFreeBuffer += sizeof(BYTE);
		*((UINT*)pFreeBuffer) = DataLen;
		pFreeBuffer += sizeof(UINT);
		memcpy(pFreeBuffer, pData, DataLen);
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		m_ElementSize = 0;
		return true;
	}
	bool AddMember(const CEasyBuffer& Data)
	{
		return AddMember((const unsigned char*)Data.GetBuffer(), (UINT)Data.GetUsedSize());
	}
	bool AddMember(const CSmartValue& Value)
	{
		if (m_pData == NULL)
			return false;		
		if (Value.GetDataLen() <= 0)
		{
			return false;
		}
		UINT NeedSize = Value.GetDataLen();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}

		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		memcpy(pFreeBuffer, Value.GetData(), Value.GetDataLen());
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		m_ElementSize = 0;
		return true;
	}
	bool AddMember(const CSmartStruct& Data);
	bool AddMember(const CSmartArray& Data)
	{
		if (Data.GetData() == NULL)
			return false;

		UINT NeedSize = Data.GetDataLen();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}		

		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		memcpy(pFreeBuffer, Data.GetData(), Data.GetDataLen());
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		m_ElementSize = 0;
		return true;
	}
	bool AddMember(const CVariedValue& Value)
	{
		switch (Value.GetType())
		{
		case VARIED_VALUE_TYPE::NIL:
			return AddMemberNull();
		case VARIED_VALUE_TYPE::BOOLEAN:
			return AddMember((bool)Value);
		case VARIED_VALUE_TYPE::INT8:
			return AddMember((char)Value);
		case VARIED_VALUE_TYPE::UINT8:
			return AddMember((unsigned char)Value);
		case VARIED_VALUE_TYPE::INT16:
			return AddMember((short)Value);
		case VARIED_VALUE_TYPE::UINT16:
			return AddMember((unsigned short)Value);
		case VARIED_VALUE_TYPE::INT32:
			return AddMember((int)Value);
		case VARIED_VALUE_TYPE::UINT32:
			return AddMember((unsigned int)Value);
		case VARIED_VALUE_TYPE::INT64:
			return AddMember((__int64)Value);
		case VARIED_VALUE_TYPE::UINT64:
			return AddMember((unsigned __int64)Value);
		case VARIED_VALUE_TYPE::FLOAT32:
			return AddMember((float)Value);
		case VARIED_VALUE_TYPE::FLOAT64:
			return AddMember((double)Value);
		case VARIED_VALUE_TYPE::STRING:
			return AddMember((LPCTSTR)Value);
		case VARIED_VALUE_TYPE::BINARY:
			return AddMember((const unsigned char*)Value, Value.GetLength());
		case VARIED_VALUE_TYPE::ARRAY:
			{
				CSmartValue Member;
				if (PrepareMember(CSmartValue::VT_ARRAY, Member))
				{
					if (Member.SetValue(Value))
						return FinishMember(Member.GetDataLen());
				}
			}
			break;
		case VARIED_VALUE_TYPE::TABLE:
			{
				CSmartValue Member;
				if (PrepareMember(CSmartValue::VT_STRUCT, Member))
				{
					if (Member.SetValue(Value))
						return FinishMember(Member.GetDataLen());
				}
			}
			break;
		}
		return false;
	}
	bool Append(const CSmartArray& Array)
	{
		if (m_pData == NULL)
			return false;
		if (Array.m_pData == NULL)
			return false;
		if (Array.m_pData[0] != CSmartValue::VT_ARRAY)
			return false;
		UINT NeedSize = Array.GetLength();
		if (GetFreeLen() < NeedSize)
		{
			return false;
		}		

		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();

		memcpy(pFreeBuffer, Array.m_pData + sizeof(BYTE) + sizeof(UINT), NeedSize);
		*((UINT*)(m_pData + sizeof(BYTE))) += NeedSize;
		CheckElements();
		return true;
	}
	void Dump(CStringBuilder& OutBuffer, bool HaveType);
	static inline UINT GetFixMemberSize(UINT TypeLen)
	{
		return sizeof(BYTE) + TypeLen;
	}
	static inline UINT GetArrayMemberSize(UINT ArraySize)
	{
		return sizeof(BYTE) + sizeof(UINT) + ArraySize;
	}
	static inline UINT GetStructMemberSize(UINT StructSize)
	{
		return sizeof(BYTE) + sizeof(UINT) + StructSize;
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
			return (szStr ? (UINT)UnicodeToUTF8(szStr, StrLen, NULL, 0) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		case CEasyString::STRING_CODE_PAGE_UCS16:
			return (UINT)StrLen * sizeof(WCHAR) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		default:
			return (szStr ? (UINT)UnicodeToAnsi(szStr, StrLen, NULL, 0) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
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
				return (szStr ? (UINT)AnsiToUTF8(szStr, StrLen, NULL, 0) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			else
				return (UINT)StrLen + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		case CEasyString::STRING_CODE_PAGE_UCS16:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_ANSI)
				return (szStr ? (UINT)AnsiToUnicode(szStr, StrLen, NULL, 0) * sizeof(WCHAR) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
			else
				return (szStr ? (UINT)UTF8ToUnicode(szStr, StrLen, NULL, 0) * sizeof(WCHAR) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		default:
			if (CEasyString::SYSTEM_STRING_CODE_PAGE == CEasyString::STRING_CODE_PAGE_UTF8)
				return (szStr ? (UINT)UTF8ToAnsi(szStr, StrLen, NULL, 0) : 0) + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
			else
				return (UINT)StrLen + sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		}
	}
	inline static UINT GetBinaryMemberSize(UINT DataLen)
	{
		return sizeof(BYTE) + sizeof(UINT) + DataLen;
	}
	static UINT GetVariedMemberSize(const CVariedValue& Value);
protected:
	bool PrepareMember(BYTE Type, CSmartValue& SmartValue)
	{
		BYTE* pFreeBuffer = m_pData + sizeof(BYTE) + sizeof(UINT) + GetLength();
		UINT FreeSize = GetFreeLen();
		if (SmartValue.Attach(pFreeBuffer, FreeSize, Type))
		{
			return true;
		}
		return false;
	}
	void CheckElements()
	{
		m_ElementSize = 0;
		LPVOID Pos = GetFirstMemberPosition();
		while (Pos)
		{
			CSmartValue Value = GetNextMember(Pos);
			UINT Size = GetElementSizeByType(Value.GetType());			
			if (Size)
			{
				if (m_ElementSize == 0)
				{
					m_ElementSize = Size;
				}					
				else if (m_ElementSize != Size)
				{
					m_ElementSize = 0;
					break;
				}
			}
			else
			{
				m_ElementSize = 0;
				break;
			}
		}
	}
	UINT GetElementSizeByType(int Type)
	{
		switch (Type)
		{
		case CSmartValue::VT_NULL:
			return sizeof(BYTE);
		case CSmartValue::VT_CHAR:
		case CSmartValue::VT_UCHAR:
			return sizeof(BYTE) + sizeof(char);
		case CSmartValue::VT_SHORT:
		case CSmartValue::VT_USHORT:
			return sizeof(BYTE) + sizeof(short);
		case CSmartValue::VT_INT:
		case CSmartValue::VT_UINT:
			return sizeof(BYTE) + sizeof(int);
		case CSmartValue::VT_BIGINT:
		case CSmartValue::VT_UBIGINT:
			return sizeof(BYTE) + sizeof(__int64);
		case CSmartValue::VT_FLOAT:
			return sizeof(BYTE) + sizeof(float);
		case CSmartValue::VT_DOUBLE:
			return sizeof(BYTE) + sizeof(double);
		case CSmartValue::VT_BOOL:
			return sizeof(BYTE) + sizeof(BYTE);
		}
		return 0;
	}
};

