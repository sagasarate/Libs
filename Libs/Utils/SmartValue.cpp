/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartValue.cpp                                           */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

#ifdef WIN32
#ifdef UNICODE
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UCS16;
#else
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_ANSI;
#endif
#else
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UTF8;
#endif

BYTE CSmartValue::NULL_DATA = VT_NULL;

int CSmartValue::GetTypeFromData(LPCVOID pData, UINT DataSize)
{
	if (pData == NULL || DataSize < sizeof(BYTE))
		return VT_UNKNOWN;

	return *((BYTE *)pData);
}
UINT CSmartValue::GetTypeLen(BYTE Type)
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
		return sizeof(BYTE) + sizeof(UINT) + sizeof(char);
		break;
	case VT_STRING_UCS16:
		return sizeof(BYTE) + sizeof(UINT) + sizeof(WCHAR);
		break;
	case VT_STRUCT:
	case VT_BINARY:
	case VT_ARRAY:
		return sizeof(BYTE) + sizeof(UINT);
		break;
	case VT_BOOL:
		return sizeof(BYTE) + sizeof(BYTE);
		break;
	}
	return 0;
}

void CSmartValue::Dump(CStringBuilder& OutBuffer, bool HaveType)
{
	switch (GetType())
	{
	case VT_NULL:
		OutBuffer.Append(_T("\"NULL\""));
		break;
	case VT_CHAR:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(char)"));
		OutBuffer.AppendFormated(_T("\"%d\""), (char)(*this));
		break;
	case VT_UCHAR:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(byte)"));
		OutBuffer.AppendFormated(_T("\"%u\""), (BYTE)(*this));
		break;
	case VT_SHORT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(short)"));
		OutBuffer.AppendFormated(_T("\"%d\""), (short)(*this));
		break;
	case VT_USHORT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(ushort)"));
		OutBuffer.AppendFormated(_T("\"%u\""), (WORD)(*this));
		break;
	case VT_INT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(int)"));
		OutBuffer.AppendFormated(_T("\"%d\""), (int)(*this));
		break;
	case VT_UINT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(uint)"));
		OutBuffer.AppendFormated(_T("\"%u\""), (UINT)(*this));
		break;
	case VT_BIGINT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(int64)"));
		OutBuffer.AppendFormated(_T("\"%lld\""), (int)(*this));
		break;
	case VT_UBIGINT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(uint64)"));
		OutBuffer.AppendFormated(_T("\"%llu\""), (UINT)(*this));
		break;
	case VT_FLOAT:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(float)"));
		OutBuffer.AppendFormated(_T("\"%f\""), (float)(*this));
		break;
	case VT_DOUBLE:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(double)"));
		OutBuffer.AppendFormated(_T("\"%lf\""), (double)(*this));
		break;
	case VT_STRING_ANSI:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(ansi)"));
		OutBuffer.Append(_T("\""));
		OutBuffer.Append(GetString());
		OutBuffer.Append(_T("\""));
		break;
	case VT_STRING_UTF8:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(utf8)"));
		OutBuffer.Append(_T("\""));
		OutBuffer.Append(GetString());
		OutBuffer.Append(_T("\""));
		break;
	case VT_STRING_UCS16:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(ucs2)"));
		OutBuffer.Append(_T("\""));
		OutBuffer.Append(GetString());
		OutBuffer.Append(_T("\""));
		break;
	case VT_STRUCT:
		{
			CSmartStruct Data(*this);
			Data.Dump(OutBuffer, HaveType);
		}
		break;
	case VT_BINARY:
		if (HaveType)
			OutBuffer.AppendFormated(_T("(bin)"));
		OutBuffer.Append(_T("\""));
		OutBuffer.Append(BinToString((const BYTE*)(*this), GetLength()));
		OutBuffer.Append(_T("\""));
		break;
	case VT_ARRAY:
		{
			CSmartArray Data(*this);
			Data.Dump(OutBuffer, HaveType);
		}
		break;
	case VT_BOOL:
		OutBuffer.AppendFormated((bool)(*this) ? _T("\"true\"") : _T("\"false\""));
		break;
	}
}

UINT CSmartValue::GetVariedValueSize(const CVariedValue& Value)
{
	switch (Value.GetType())
	{
	case VARIED_VALUE_TYPE::BOOLEAN:
		return sizeof(bool);
	case VARIED_VALUE_TYPE::INT8:
		return sizeof(char);
	case VARIED_VALUE_TYPE::UINT8:
		return sizeof(unsigned char);
	case VARIED_VALUE_TYPE::INT16:
		return sizeof(short);
	case VARIED_VALUE_TYPE::UINT16:
		return sizeof(unsigned short);
	case VARIED_VALUE_TYPE::INT32:
		return sizeof(int);
	case VARIED_VALUE_TYPE::UINT32:
		return sizeof(unsigned int);
	case VARIED_VALUE_TYPE::INT64:
		return sizeof(__int64);
	case VARIED_VALUE_TYPE::UINT64:
		return sizeof(unsigned __int64);
	case VARIED_VALUE_TYPE::FLOAT32:
		return sizeof(float);
	case VARIED_VALUE_TYPE::FLOAT64:
		return sizeof(double);
	case VARIED_VALUE_TYPE::STRING:
		return GetValueSize((LPCTSTR)Value);
	case VARIED_VALUE_TYPE::BINARY:
		return GetBinaryValueSize(Value.GetLength());
	case VARIED_VALUE_TYPE::ARRAY:
		{
			UINT Size = 0;
			for (const CVariedValue& Child : Value)
			{
				Size += CSmartArray::GetVariedMemberSize(Child);
			}
			return Size;
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
			return Size;
		}
	}
	return 0;
}
bool CSmartValue::SetValue(const CVariedValue& Value, bool CanChangeType)
{
	if (m_pData == NULL)
		return false;
	switch (Value.GetType())
	{
	case VARIED_VALUE_TYPE::BOOLEAN:
		return SetValue((bool)Value, CanChangeType);
	case VARIED_VALUE_TYPE::INT8:
		return SetValue((char)Value, CanChangeType);
	case VARIED_VALUE_TYPE::UINT8:
		return SetValue((unsigned char)Value, CanChangeType);
	case VARIED_VALUE_TYPE::INT16:
		return SetValue((short)Value);
	case VARIED_VALUE_TYPE::UINT16:
		return SetValue((unsigned short)Value, CanChangeType);
	case VARIED_VALUE_TYPE::INT32:
		return SetValue((int)Value, CanChangeType);
	case VARIED_VALUE_TYPE::UINT32:
		return SetValue((unsigned int)Value, CanChangeType);
	case VARIED_VALUE_TYPE::INT64:
		return SetValue((__int64)Value, CanChangeType);
	case VARIED_VALUE_TYPE::UINT64:
		return SetValue((unsigned __int64)Value, CanChangeType);
	case VARIED_VALUE_TYPE::FLOAT32:
		return SetValue((float)Value, CanChangeType);
	case VARIED_VALUE_TYPE::FLOAT64:
		return SetValue((double)Value, CanChangeType);
	case VARIED_VALUE_TYPE::STRING:
		return SetValue((LPCTSTR)Value, CanChangeType);
	case VARIED_VALUE_TYPE::BINARY:
		return SetValue((const unsigned char*)Value, Value.GetLength(), CanChangeType);
	case VARIED_VALUE_TYPE::ARRAY:
		{
			UINT NeedSize = GetVariedValueSize(Value) + sizeof(BYTE) + sizeof(UINT);
			if (NeedSize > m_DataLen)
				return false;
			if (GetType() != VT_ARRAY)
			{
				if (CanChangeType)
				{
					if (!ChangeType(VT_ARRAY, NeedSize))
						return false;
				}
				else
				{
					return false;
				}
			}
			CSmartArray Array(*this);
			Array.Clear();
			for (const CVariedValue& Child : Value)
			{
				if (!Array.AddMember(Child))
					return false;
			}
			return true;
		}
		break;
	case VARIED_VALUE_TYPE::TABLE:
		{
			UINT NeedSize = GetVariedValueSize(Value) + sizeof(BYTE) + sizeof(UINT);
			if (NeedSize > m_DataLen)
				return false;
			if (GetType() != VT_STRUCT)
			{
				if (CanChangeType)
				{
					if (!ChangeType(VT_STRUCT, NeedSize))
						return false;
				}
				else
				{
					return false;
				}				
			}
			CSmartStruct Struct(*this);
			Struct.Clear();
			void* Pos = Value.GetFirstChildPos();
			while (Pos)
			{
				CVariedValue Key;
				const CVariedValue* pChild = Value.GetNextChild(Pos, Key);
				CSmartStruct SubStruct = Struct.PrepareSubStruct();
				if (!SubStruct.AddMember((WORD)SST_PAIR::KEY, Key))
					return false;
				if (!SubStruct.AddMember((WORD)SST_PAIR::DATA, *pChild))
					return false;
				if (!Struct.FinishMember(VARIED_MEMEBR_ID, SubStruct.GetDataLen()))
					return false;
			}
			return true;
		}
		break;
	}
	return false;
}

bool CSmartValue::FetchVariedData(CVariedValue& Value) const
{
	if (GetType() == VT_ARRAY)
	{
		CSmartArray Array(*this);
		Value.CreateArray();
		for (CSmartValue Member : Array)
		{
			CVariedValue* pChild = Value.AddChildNil();
			Member.GetVariedValue(*pChild);
		}
	}	
	else if (GetType() == VT_STRUCT)
	{
		CSmartStruct Struct(*this);
		Value.CreateTable();
		for (CSmartStruct::Pair pair : Struct)
		{
			CSmartStruct SubStruct(pair.Value);
			if (SubStruct.IsMemberExist((WORD)SST_PAIR::KEY) && SubStruct.IsMemberExist((WORD)SST_PAIR::DATA))
			{
				CVariedValue* pChild = Value.AddChildNil((LPCTSTR)SubStruct.GetMember((WORD)SST_PAIR::KEY));
				SubStruct.GetMember((WORD)SST_PAIR::DATA).GetVariedValue(*pChild);
			}
			
		}
	}
	return false;
}

rapidjson::Value CSmartValue::ToJson(rapidjson::Document::AllocatorType& Alloc)
{
	switch (this->GetType())
	{
	case VT_CHAR:
	case VT_SHORT:
	case VT_INT:
		return rapidjson::Value((int)(*this));
	case VT_UCHAR:
	case VT_USHORT:
	case VT_UINT:
		return rapidjson::Value((unsigned int)(*this));
	case VT_BIGINT:
		return rapidjson::Value((__int64)(*this));
	case VT_UBIGINT:
		return rapidjson::Value((unsigned __int64)(*this));
	case VT_FLOAT:
		return rapidjson::Value((float)(*this));
	case VT_DOUBLE:
		return rapidjson::Value((double)(*this));
	case VT_STRING_ANSI:
	case VT_STRING_UTF8:
	case VT_STRING_UCS16:
		return rapidjson::Value((LPCTSTR)GetString(), Alloc);
	case VT_STRUCT:
		return CSmartStruct(*this).ToJson(Alloc);
	case VT_BINARY:
		{
			CEasyString BinStr = BinToString((BYTE*)(*this), GetLength());
			return rapidjson::Value((LPCTSTR)BinStr, Alloc);
		}
	case VT_ARRAY:
		return CSmartArray(*this).ToJson(Alloc);
	case VT_BOOL:
		return rapidjson::Value((bool)(*this));
	default:
		return rapidjson::Value(rapidjson::kNullType);
	}
}