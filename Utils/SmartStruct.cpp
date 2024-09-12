/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartStruct.cpp                                          */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"






void CSmartStruct::Dump(CStringBuilder& OutBuffer, bool HaveType)
{
	OutBuffer.Append(_T("{"));
	bool IsFirst = true;
	void* Pos = GetFirstMemberPosition();
	while (Pos)
	{
		WORD MemberID = 0;
		CSmartValue Member = GetNextMember(Pos, MemberID);
		if (IsFirst)
		{
			IsFirst = false;
			OutBuffer.Append(_T(","));
		}
		OutBuffer.Append(_T("\"%u\":"), MemberID);
		Member.Dump(OutBuffer, HaveType);
	}
	OutBuffer.Append(_T("}"));
}

UINT CSmartStruct::GetVariedMemberSize(const CVariedValue& Value)
{
	switch (Value.GetType())
	{
	case VARIED_VALUE_TYPE::NIL:
		return sizeof(WORD) + sizeof(BYTE);
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
	return sizeof(WORD) + sizeof(BYTE);
}

rapidjson::Value CSmartStruct::ToJson(rapidjson::Document::AllocatorType& Alloc)
{
	rapidjson::Value Object(rapidjson::kObjectType);
	TCHAR IDStr[256];
	for (Pair& Member : *this)
	{
		_stprintf_s(IDStr, 255, _T("%u"), Member.MemberID);
		rapidjson::Value Name = rapidjson::Value((LPCTSTR)IDStr, Alloc);
		Object.AddMember(Name, Member.Value.ToJson(Alloc), Alloc);
	}
	return Object;
}