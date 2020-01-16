/****************************************************************************/
/*                                                                          */
/*      文件名:    MySQLRecordSet.h                                         */
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

class CMySQLConnection;
typedef MYSQL_RES * MYSQL_RES_HANDLE;

class CMySQLRecordSet :
	public IDBRecordSet
{
protected:
	CMySQLConnection *				m_pDBConnection;
	MYSQL_RES_HANDLE				m_hResults;
	CEasyArray<DB_COLUMN_INFO>		m_pColumnInfos;
	CEasyArray<CDBValue>			m_RowBuffer;
	int								m_CurRow;
	CDBValue						m_EmptyValue;

	DECLARE_CLASS_INFO_STATIC(CMySQLRecordSet)
public:
	CMySQLRecordSet(void);
	virtual ~CMySQLRecordSet(void);

	int Init(CMySQLConnection * pDBConnection,MYSQL_RES_HANDLE hResults);
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
protected:
	int FetchRow();
};

}
