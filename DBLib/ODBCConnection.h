/****************************************************************************/
/*                                                                          */
/*      文件名:    ODBCConnection.h                                         */
/*      创建日期:  2009年09月11日                                           */
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

class CODBCDatabase;
class CDBRecordSet;

class CODBCConnection :
	public IDBConnection
{
protected:
	SQLHDBC					m_hDBConn;	
	CODBCDatabase *			m_pDatabase;
	SQLHSTMT				m_hStmt;
	CEasyString				m_LastErrorString;
	int						m_LastErrorCode;
	CEasyString				m_LastSQL;
	
	DECLARE_CLASS_INFO_STATIC(CODBCConnection)
public:
	CODBCConnection(void);
	virtual ~CODBCConnection(void);

	int Init(CODBCDatabase * pDatabase);
	virtual void Destory();

	virtual IDBRecordSet * CreateRecordSet(int RecordSetType);
	virtual IDBParameterSet * CreateParameterSet(int RecordSetType);

	virtual int Connect(LPCSTR ConnectStr);
	virtual int Disconnect();
	virtual BOOL IsConnected();
	virtual int ExecuteSQL(LPCSTR SQLStr,int StrLen=0,IDBParameterSet * pParamSet=NULL);
	virtual int ExecuteSQLWithoutResultSet(LPCSTR SQLStr, int StrLen = 0, IDBParameterSet * pParamSet = NULL);
	virtual int GetTables();
	virtual int GetResults(IDBRecordSet * pDBRecordset);
	virtual int NextResults(IDBRecordSet * pDBRecordset);
	virtual int GetAffectedRowCount();
	virtual int EnableTransaction(BOOL IsEnable);
	virtual int Commit();
	virtual int RollBack();
	virtual UINT GetLastDatabaseErrorCode();
	virtual LPCSTR GetLastDatabaseErrorString();
	virtual LPCSTR GetLastSQL();
	virtual int TranslateString(LPCSTR szSource,int SrcLen,LPTSTR szTarget,int MaxLen);
	virtual void Reset();
	virtual int SelectDefaultDatabase(LPCTSTR szDBName);
	
	void ProcessMessagesODBC(SQLSMALLINT plm_handle_type,SQLHANDLE plm_handle,char *logstring,int ConnInd);
	static int ODBCCTypeTODBLibType(int Type,UINT64& Size);
	static int DBLibTypeToODBCCType(int Type,UINT64& Size);
	static int DBLibTypeToODBCSQLType(int Type,UINT64& Size);
	static int ODBCSQLTypeTOODBCCType(int Type,UINT64& Size);
	
	static int DBParamTypeToODBCParamType(int Type);
	

protected:
	virtual int FetchStaticResult(SQLHSTMT hStmt,CDBStaticRecordSet * pDBRecordset);
	int ExecuteSQLDirect(LPCSTR SQLStr,int StrLen);
	int ExecuteSQLWithParam(LPCSTR SQLStr,int StrLen,CDBParameterSet * pParamSet);
	void SetConnectFlags(LPCSTR szFlags);
	
};



}