#pragma once


enum BENCODING_TYPE
{
	BENCODING_TYPE_NONE,
	BENCODING_TYPE_INTEGER,
	BENCODING_TYPE_STRING,
	BENCODING_TYPE_LIST,
	BENCODING_TYPE_DICTIONARY,
};

class CBencodingValue;

struct BENCODING_VALUE_PAIR
{
	CEasyStringA					Key;
	CSmartPtr<CBencodingValue>		Value;
};

class CBencodingValue :
	public CNameObject
{
protected:
	BENCODING_TYPE								m_Type;
	INT64										m_IntValue;
	CEasyStringA								m_StrValue;
	CEasyArray<CSmartPtr<CBencodingValue> >		m_ListValue;
	CEasyArray<BENCODING_VALUE_PAIR>			m_DictValue;
public:
	CBencodingValue();
	~CBencodingValue();
	
	CBencodingValue(int Value);
	CBencodingValue(INT64 Value);
	CBencodingValue(LPCSTR Value);
	CBencodingValue(const CBencodingValue& Value);

	void Clear();
	void SetType(BENCODING_TYPE Type);
	BENCODING_TYPE GetType();

	const CBencodingValue& operator=(int Value);
	const CBencodingValue& operator=(INT64 Value);
	const CBencodingValue& operator=(LPCSTR Value);
	const CBencodingValue& operator=(const CBencodingValue& Value);

	INT64 GetIntValue();	
	CEasyStringA& GetStrValue();
	const CEasyStringA& GetStrValue() const;
	UINT GetListCount() const;
	CBencodingValue& GetListValue(UINT Index);
	const CBencodingValue& GetListValue(UINT Index) const;
	UINT GetDictCount() const;
	bool HaveDictValue(LPCSTR Key) const;
	CBencodingValue& GetDictValue(LPCSTR Key);
	const CBencodingValue& GetDictValue(LPCSTR Key) const;
	BENCODING_VALUE_PAIR& GetDictPair(UINT Index);
	const BENCODING_VALUE_PAIR& GetDictPair(UINT Index) const;
	
	void SetStrValue(LPCSTR Value, UINT Len);
	void AddListValue(const CBencodingValue& Value);
	void AddDictValue(LPCSTR Key, const CBencodingValue& Value);

	const BYTE * Parser(const BYTE * pData, UINT& DataLen);
protected:
	const BYTE * ParseInt(const BYTE * pData, UINT& DataLen);
	const BYTE * ParseStr(const BYTE * pData, UINT& DataLen);
	const BYTE * ParseList(const BYTE * pData, UINT& DataLen);
	const BYTE * ParseDict(const BYTE * pData, UINT& DataLen);

};

