/****************************************************************************/
/*                                                                          */
/*      �ļ���:    MySQLDynamicRecordSet.h                                  */
/*      ��������:  2009��07��06��                                           */
/*      ����:      Sagasarate                                               */
/*                                                                          */
/*      ��������Ȩ��Sagasarate(sagasarate@sina.com)����                     */
/*      ����Խ������������κ���ҵ�ͷ���ҵ������������                      */
/*      ���뱣���˰�Ȩ����                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace DBLib
{
class CMySQLConnection;
typedef MYSQL_STMT * MYSQL_STMT_HANDLE;

class CMySQLDynamicRecordSet :
	public IDBRecordSet
{
protected:
	CMySQLConnection *				m_pDBConnection;
	MYSQL_STMT_HANDLE				m_hStmt;
	CEasyArray<DB_COLUMN_INFO>		m_pColumnInfos;
	CEasyArray<CDBValue>			m_RowBuffer;
	CEasyArray<MYSQL_BIND>			m_FetchBuffer;
	CEasyBuffer						m_FetchDataBuffer;
	CDBValue						m_EmptyValue;
	bool							m_IsBOF;
	bool							m_IsEOF;
	bool							m_CacheAllData;

	DECLARE_CLASS_INFO(CMySQLDynamicRecordSet)
public:
	CMySQLDynamicRecordSet(bool CacheAllData=true);
	~CMySQLDynamicRecordSet(void);

	int Init(CMySQLConnection * pDBConnection,MYSQL_STMT_HANDLE hStmt);
	int NextResults();
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
protected:
	int FetchRow();
};

}