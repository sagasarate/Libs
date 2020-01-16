/****************************************************************************/
/*                                                                          */
/*      文件名:    DBStaticRecordSet.h                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



namespace DBLib
{



class CDBStaticRecordSet :
	public IDBRecordSet
{
protected:
	typedef CEasyArray<CDBValue> CDBValueRow;
	CEasyArray<DB_COLUMN_INFO>		m_pColumnInfos;
	CEasyArray<CDBValueRow>			m_Records;
	CEasyArray<CDBValue>			m_Params;
	int								m_CurRow;
	CDBValue						m_EmptyValue;

	DECLARE_CLASS_INFO_STATIC(CDBStaticRecordSet)
public:
	CDBStaticRecordSet(void);
	virtual ~CDBStaticRecordSet(void);

	virtual int Init(LPCVOID pData,int DataSize);
	virtual void Destory();

	virtual int GetRecordCount();
	virtual int GetColumnCount();
	virtual LPCSTR GetColumnName(int Index);
	virtual int GetIndexByColumnName(LPCSTR Name);
	virtual DB_COLUMN_INFO * GetColumnInfo(int Index);


	virtual CDBValue& GetField(int Index);
	virtual CDBValue& GetField(LPCSTR Name);
	virtual int MoveFirst();
	virtual int MoveLast();
	virtual int MoveNext();
	virtual int MovePrevious();
	virtual int MoveTo(int Index);
	virtual bool IsEOF();
	virtual bool IsBOF();

	virtual bool Close();

	virtual bool SetBlobMaxProcessSize(UINT64 MaxSize);
};

}
