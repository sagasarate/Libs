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
#include "stdafx.h"

CSmartStruct CSmartArray::PrepareSubStruct()
{
	CSmartStruct Struct;	
	BYTE* pTail = m_pData + GetDataLen();
	UINT FreeSize = GetFreeLen();
	Struct.Attach(pTail, FreeSize, true);	
	return Struct;
}
CSmartStruct CSmartArray::GetNextStruct(void*& Pos) const
{
	CSmartStruct Value;

	if (m_pData == NULL)
	{
		Pos = NULL;
		return Value;
	}


	if (Pos)
	{
		BYTE* pHead = (BYTE*)Pos;
		BYTE* pTail = m_pData + GetDataLen();

		pHead += sizeof(WORD);

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
bool CSmartArray::AddMember(const CSmartStruct& Data)
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
bool CSmartArray::GetArray(CEasyArray<CSmartStruct>& Array) const
{
	if (GetLength())
	{
		const BYTE* pHead = (BYTE*)GetValueData();
		const BYTE* pTail = pHead + GetLength();
		Array.Resize(GetArrayLength());
		UINT Index = 0;
		while ((pHead + sizeof(BYTE) <= pTail) && (Index < Array.GetCount()))
		{
			Array[Index].Attach((BYTE*)pHead, (UINT)(pTail - pHead), false);
			Index++;
			pHead += Array[Index].GetDataLen();
		}
		return true;
	}
	else
	{
		return true;
	}
}
bool CSmartArray::AddArray(const CEasyArray<CSmartStruct>& Array)
{
	if (m_pData)
	{
		UINT NeedSize = 0;
		for (const CSmartStruct& Value : Array)
		{
			NeedSize += GetStructMemberSize(Value.GetDataLen());
		}
		if (GetFreeLen() >= NeedSize)
		{
			for (const CSmartStruct& Value : Array)
			{
				if (!AddMember(Value))
					return false;
			}
		}
		return true;
	}
	return false;
}
void CSmartArray::Dump(CStringBuilder& OutBuffer, bool HaveType)
{
	OutBuffer.Append(_T("["));
	bool IsFirst = true;
	void* Pos = GetFirstMemberPosition();
	while (Pos)
	{
		CSmartValue Member = GetNextMember(Pos);
		if (IsFirst)
		{
			IsFirst = false;
			OutBuffer.Append(_T(","));
		}
		Member.Dump(OutBuffer, HaveType);
	}
	OutBuffer.Append(_T("]"));
}

UINT CSmartArray::GetVariedMemberSize(const CVariedValue& Value)
{
	switch (Value.GetType())
	{
	case VARIED_VALUE_TYPE::NIL:
		return sizeof(BYTE);
	case VARIED_VALUE_TYPE::BOOLEAN:
		return GetFixMemberSize(sizeof(bool));
	case VARIED_VALUE_TYPE::INT8:
		return GetFixMemberSize(sizeof(char));
	case VARIED_VALUE_TYPE::UINT8:
		return GetFixMemberSize(sizeof(unsigned char));
	case VARIED_VALUE_TYPE::INT16:
		return GetFixMemberSize(sizeof(short));
	case VARIED_VALUE_TYPE::UINT16:
		return GetFixMemberSize(sizeof(unsigned short));
	case VARIED_VALUE_TYPE::INT32:
		return GetFixMemberSize(sizeof(int));
	case VARIED_VALUE_TYPE::UINT32:
		return GetFixMemberSize(sizeof(unsigned int));
	case VARIED_VALUE_TYPE::INT64:
		return GetFixMemberSize(sizeof(__int64));
	case VARIED_VALUE_TYPE::UINT64:
		return GetFixMemberSize(sizeof(unsigned __int64));
	case VARIED_VALUE_TYPE::FLOAT32:
		return GetFixMemberSize(sizeof(float));
	case VARIED_VALUE_TYPE::FLOAT64:
		return GetFixMemberSize(sizeof(double));
	case VARIED_VALUE_TYPE::STRING:
		return GetStringMemberSize((LPCTSTR)Value);
	case VARIED_VALUE_TYPE::BINARY:
		return GetBinaryMemberSize(Value.GetLength());
	case VARIED_VALUE_TYPE::ARRAY:
		{
			UINT Size = 0;
			for (const CVariedValue& Child : Value)
			{
				Size += CSmartArray::GetVariedMemberSize(Child);
			}
			return GetArrayMemberSize(Size);
		}
	case VARIED_VALUE_TYPE::TABLE:
		{
			UINT Size = 0;
			LPVOID Pos = Value.GetFirstChildPos();
			while (Pos)
			{
				CVariedValue Key;
				const CVariedValue* pChild = Value.GetNextChild(Pos, Key);
				UINT SubSize = CSmartStruct::GetVariedMemberSize(Key) + CSmartStruct::GetVariedMemberSize(*pChild);
				Size += CSmartStruct::GetStructMemberSize(SubSize);
			}
			return GetStructMemberSize(Size);
		}
	}
	return sizeof(BYTE);
}