#pragma once

class CCSVReader :
	public CNameObject
{
protected:
	TCHAR *									m_pData;
	UINT									m_DataSize;
	UINT									m_BufferSize;
	UINT									m_GrowSize;
	int										m_LocalCodePage;
	int										m_SaveCodePage;
	CEasyArray<CEasyArray<UINT> >			m_Records;
	CEasyArray<UINT>						m_ColumnNames;

	DECLARE_FILE_CHANNEL_MANAGER

	DECLARE_CLASS_INFO(CCSVReader)
public:
	CCSVReader(void);
	~CCSVReader(void);

	virtual void Destory();

	bool Open(LPCTSTR szFileName,bool HaveHeader=true);
	bool Open(IFileAccessor * pFileAccessor,bool HaveHeader=true);

	UINT GetRowCount();
	UINT GetColCount();

	int GetColIndex(LPCTSTR ColName);

	LPCTSTR GetDataString(UINT Row,UINT Col,LPCTSTR Default);
	LPCTSTR GetDataString(UINT Row,LPCTSTR ColName,LPCTSTR Default);

	int GetDataInt(UINT Row,UINT Col,int Default);
	int GetDataInt(UINT Row,LPCTSTR ColName,int Default);

	INT64 GetDataInt64(UINT Row,UINT Col,INT64 Default);
	INT64 GetDataInt64(UINT Row,LPCTSTR ColName,INT64 Default);	

	double GetDataDouble(UINT Row,UINT Col,double Default);
	double GetDataDouble(UINT Row,LPCTSTR ColName,double Default);

	bool GetDataBool(UINT Row,UINT Col,bool Default);
	bool GetDataBool(UINT Row,LPCTSTR ColName,bool Default);

	void SetLocalCodePage(int CodePage)
	{
		m_LocalCodePage=CodePage;
	}
	int GetLocalCodePage()
	{
		return m_LocalCodePage;
	}
	void SetSaveCodePage(int CodePage)
	{
		m_SaveCodePage=CodePage;
	}
	int GetSaveCodePage()
	{
		return m_SaveCodePage;
	}

public:
	bool Create(UINT BufferSize,UINT GrowSize);
	bool Save(LPCTSTR szFileName,bool WriteHeader=true);
	bool Save(IFileAccessor * pFileAccessor,bool WriteHeader=true);
	bool AddColumn(LPCTSTR ColName);
	bool AddRow();
	bool AddDataString(LPCTSTR Data);	
	bool AddDataInt(int Data);
	bool AddDataInt64(INT64 Data);
	bool AddDataDouble(double Data);
	bool AddDataBool(bool Data);
protected:
	UINT GetLineCount(LPCTSTR szData);
	LPTSTR ParseLine(LPTSTR szLine,CEasyArray<UINT>& LineRecord);
	void ConfirmBufferFreeSize(UINT NeedSize);
	LPTSTR AllocBuffer(UINT NeedSize);
	UINT GetSavedDataLen();
	UINT GetSavedLineLen(CEasyArray<UINT>& LineRecord);
	UINT SaveLine(LPTSTR pSaveBuffer,UINT BufferSize,CEasyArray<UINT>& LineRecord);

	LPTSTR OffsetToStr(UINT Offset);
	UINT StrToOffset(LPCTSTR szStr);


};

inline LPTSTR CCSVReader::OffsetToStr(UINT Offset)
{
	return m_pData+Offset;
}
inline UINT CCSVReader::StrToOffset(LPCTSTR szStr)
{
	return (UINT)(szStr-m_pData);
}