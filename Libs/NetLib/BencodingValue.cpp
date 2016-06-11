#include "stdafx.h"



CBencodingValue::CBencodingValue()
{
	m_Type = BENCODING_TYPE_NONE;
	m_IntValue = 0;
}


CBencodingValue::~CBencodingValue()
{
}

CBencodingValue::CBencodingValue(int Value)
{
	m_Type = BENCODING_TYPE_INTEGER;
	m_IntValue = Value;
}
CBencodingValue::CBencodingValue(INT64 Value)
{
	m_Type = BENCODING_TYPE_INTEGER;
	m_IntValue = Value;
}
CBencodingValue::CBencodingValue(LPCSTR Value)
{
	m_Type = BENCODING_TYPE_STRING;
	m_StrValue = Value;
}
CBencodingValue::CBencodingValue(const CBencodingValue& Value)
{
	m_Type = Value.m_Type;
	m_IntValue = Value.m_IntValue;
	m_StrValue = Value.m_StrValue;
	m_ListValue = Value.m_ListValue;
	m_DictValue = Value.m_DictValue;
}

void CBencodingValue::Clear()
{
	m_Type = BENCODING_TYPE_NONE;
	m_IntValue = 0;
	m_StrValue.Clear();
	m_ListValue.Clear();
	m_DictValue.Clear();
}

void CBencodingValue::SetType(BENCODING_TYPE Type)
{
	if (m_Type != Type)
	{
		Clear();
		m_Type = Type;
	}
}
BENCODING_TYPE CBencodingValue::GetType()
{
	return m_Type;
}

const CBencodingValue& CBencodingValue::operator=(int Value)
{
	Clear();
	m_Type = BENCODING_TYPE_INTEGER;
	m_IntValue = Value;
	return *this;
}
const CBencodingValue& CBencodingValue::operator=(INT64 Value)
{
	Clear();
	m_Type = BENCODING_TYPE_INTEGER;
	m_IntValue = Value;
	return *this;
}
const CBencodingValue& CBencodingValue::operator=(LPCSTR Value)
{
	Clear();
	m_Type = BENCODING_TYPE_INTEGER;
	m_StrValue = Value;
	return *this;
}
const CBencodingValue& CBencodingValue::operator=(const CBencodingValue& Value)
{
	Clear();
	m_Type = Value.m_Type;
	m_IntValue = Value.m_IntValue;
	m_StrValue = Value.m_StrValue;
	m_ListValue = Value.m_ListValue;
	m_DictValue = Value.m_DictValue;
	return *this;
}


INT64 CBencodingValue::GetIntValue()
{
	return m_IntValue;
}
CEasyStringA& CBencodingValue::GetStrValue()
{
	return m_StrValue;
}
const CEasyStringA& CBencodingValue::GetStrValue() const
{
	return m_StrValue;
}
UINT CBencodingValue::GetListCount()  const
{
	return m_ListValue.GetCount();
}
CBencodingValue& CBencodingValue::GetListValue(UINT Index)
{
	static CBencodingValue Empty;
	if (Index < m_ListValue.GetCount())
		return *m_ListValue[Index];
	return Empty;
}
const CBencodingValue& CBencodingValue::GetListValue(UINT Index) const
{
	static CBencodingValue Empty;
	if (Index < m_ListValue.GetCount())
		return *m_ListValue[Index];
	return Empty;
}
UINT CBencodingValue::GetDictCount() const
{
	return m_DictValue.GetCount();
}
bool CBencodingValue::HaveDictValue(LPCSTR Key) const
{
	for (UINT i = 0; i < m_DictValue.GetCount(); i++)
	{
		if (m_DictValue[i].Key.CompareNoCase(Key) == 0)
			return true;
	}
	return false;
}
CBencodingValue& CBencodingValue::GetDictValue(LPCSTR Key)
{
	static CBencodingValue Empty;
	for (UINT i = 0; i < m_DictValue.GetCount(); i++)
	{
		if (m_DictValue[i].Key.CompareNoCase(Key) == 0)
			return *m_DictValue[i].Value;
	}
	return Empty;
}
const CBencodingValue& CBencodingValue::GetDictValue(LPCSTR Key) const
{
	static CBencodingValue Empty;
	for (UINT i = 0; i < m_DictValue.GetCount(); i++)
	{
		if (m_DictValue[i].Key.CompareNoCase(Key) == 0)
			return *m_DictValue[i].Value;
	}
	return Empty;
}
BENCODING_VALUE_PAIR& CBencodingValue::GetDictPair(UINT Index)
{
	static BENCODING_VALUE_PAIR Empty;
	if (Index < m_DictValue.GetCount())
		return m_DictValue[Index];
	return Empty;
}


const BENCODING_VALUE_PAIR& CBencodingValue::GetDictPair(UINT Index) const
{
	static BENCODING_VALUE_PAIR Empty;
	if (Index < m_DictValue.GetCount())
		return m_DictValue[Index];
	return Empty;
}

void CBencodingValue::SetStrValue(LPCSTR Value, UINT Len)
{
	if (m_Type != BENCODING_TYPE_STRING)
		Clear();
	m_StrValue.SetString(Value, Len);
}
void CBencodingValue::AddListValue(const CBencodingValue& Value)
{
	CBencodingValue * pValue = new CBencodingValue();
	*pValue = Value;
	m_ListValue.Add(pValue);
	SAFE_RELEASE(pValue);
}
void CBencodingValue::AddDictValue(LPCSTR Key, const CBencodingValue& Value)
{
	CBencodingValue * pValue = new CBencodingValue();
	*pValue = Value;
	BENCODING_VALUE_PAIR * pPair = m_DictValue.AddEmpty();
	pPair->Key = Key;
	pPair->Value = pValue;
	SAFE_RELEASE(pValue);
}


const BYTE * CBencodingValue::Parser(const BYTE * pData, UINT& DataLen)
{
	if (pData[0] == 'i')
	{
		return ParseInt(pData, DataLen);
	}
	else if (pData[0] == 'l')
	{
		return ParseList(pData, DataLen);
	}
	else if (pData[0] == 'd')
	{
		return ParseDict(pData, DataLen);
	}
	else if (pData[0] >= '0' && pData[0] <= '9')
	{
		return ParseStr(pData, DataLen);
	}
	else
	{
		return false;
	}
}

const BYTE * CBencodingValue::ParseInt(const BYTE * pData, UINT& DataLen)
{
	if (DataLen>2)
	{
		UINT Ptr = 1;
		while (Ptr < DataLen && pData[Ptr] != 'e')
		{
			Ptr++;
		}
		if (Ptr < DataLen)
		{
			m_Type = BENCODING_TYPE_INTEGER;
			m_IntValue = _strtoui64((const char *)pData + 1, NULL, 10);
			DataLen -= Ptr + 1;
			return pData + Ptr + 1;
		}
	}
	return NULL;
}
const BYTE * CBencodingValue::ParseStr(const BYTE * pData, UINT& DataLen)
{
	if (DataLen > 2)
	{
		UINT Ptr = 1;
		while (Ptr < DataLen && pData[Ptr] != ':')
		{
			Ptr++;
		}
		if (Ptr < DataLen-1)
		{
			UINT StrLen = strtoul((const char *)pData, NULL, 10);
			if (Ptr + StrLen < DataLen)
			{
				m_Type = BENCODING_TYPE_STRING;
				m_StrValue.SetString((const char *)pData + Ptr + 1, StrLen);
				DataLen -= Ptr + 1 + StrLen;
				return pData + Ptr + 1 + StrLen;
			}				
		}
	}
	return NULL;
}
const BYTE * CBencodingValue::ParseList(const BYTE * pData, UINT& DataLen)
{
	if (DataLen >= 2)
	{
		pData++;
		DataLen--;
		while (DataLen&&pData[0] != 'e')
		{
			CBencodingValue * pValue = new CBencodingValue();
			pData = pValue->Parser(pData, DataLen);			
			if (pData)
			{
				m_ListValue.Add(pValue);
				SAFE_RELEASE(pValue);
			}
			else
			{
				SAFE_RELEASE(pValue);
				return NULL;
			}			
		}
		if (DataLen)
		{
			pData++;
			DataLen--;
		}
		m_Type = BENCODING_TYPE_LIST;
		return pData;
	}
	return NULL;
}
const BYTE * CBencodingValue::ParseDict(const BYTE * pData, UINT& DataLen)
{
	if (DataLen >= 2)
	{
		pData++;
		DataLen--;
		while (DataLen&&pData[0] != 'e')
		{
			if (pData[0] >= '0' && pData[0] <= '9')
			{
				CBencodingValue KeyValue;
				pData = KeyValue.ParseStr(pData, DataLen);
				if (pData)
				{
					CBencodingValue * pValue = new CBencodingValue();
					pData = pValue->Parser(pData, DataLen);					
					if (pData)
					{
						BENCODING_VALUE_PAIR * pPair = m_DictValue.AddEmpty();
						pPair->Key = KeyValue.GetStrValue();
						pPair->Value = pValue;
						SAFE_RELEASE(pValue);
					}
					else
					{
						SAFE_RELEASE(pValue);
						return NULL;
					}
				}
				else
				{
					return NULL;
				}
			}
			else
			{
				return NULL;
			}			
		}
		if (DataLen)
		{
			pData++;
			DataLen--;
		}
		m_Type = BENCODING_TYPE_DICTIONARY;
		return pData;
	}
	return NULL;
}