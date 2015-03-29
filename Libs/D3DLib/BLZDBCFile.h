#pragma once

namespace D3DLib{

class CBLZDBCFile:
	public CNameObject
{
protected:
	BYTE *		m_pData;
	UINT		m_DataSize;
	bool		m_IsSelfRelease;
	UINT		m_RecordCount;
	UINT		m_FieldCount;
	UINT		m_RecordSize;
	UINT		m_StringTableSize;
	BYTE *		m_pRecordData;
	char *		m_pStringTable;

	DECLARE_FILE_CHANNEL_MANAGER
public:
	CBLZDBCFile(void);
	~CBLZDBCFile(void);

	bool Load(LPCTSTR FileName,UINT RecordSize);
	bool Load(IFileAccessor * pFile,UINT RecordSize);
	bool Load(BYTE * pData,UINT DataSize,UINT RecordSize,bool IsSelfRelease);
	void Close();

	UINT GetRecordSize();
	UINT GetRecordCount();
	UINT GetFieldCount();
	int GetDataInt(UINT Record,UINT Field);
	UINT GetDataUint(UINT Record,UINT Field);
	float GetDataFloat(UINT Record,UINT Field);
	LPCSTR GetDataString(UINT Record,UINT Field);
	CEasyString GetDataLocalString(UINT Record,UINT Field);
};

}