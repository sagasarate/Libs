/****************************************************************************/
/*                                                                          */
/*      文件名:    ODBCDatabase.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



#pragma comment(lib,"odbc32.lib")
#pragma comment(lib,"odbccp32.lib")
 

#define ODBC_MAX_CONNECTION		256


namespace DBLib
{

enum DB_ODBC_ERROR_INFO
{
	
	DBERR_ODBC_SQLSETENVATTRFAIL=DBERR_MAX,
	DBERR_ODBC_SQLCONNECTFAIL,
	DBERR_ODBC_SQLDISCONNECTFAIL,
	DBERR_ODBC_ENABLETRANSACTIONFAIL,
	DBERR_ODBC_COMMITFAIL,
	DBERR_ODBC_ROLLBACKFAIL,	
	DBERR_ODBC_GETCOLNUMFAIL,
	DBERR_ODBC_GETCOLINFOFAIL,
	
	DBERR_ODBC_MAX,
};

#define MAX_FEILD_LEN		1024*1024

class CODBCDatabase :
	public IDatabase
{
protected:
	SQLHENV							m_hEnv;	

	DECLARE_CLASS_INFO_STATIC(CODBCDatabase)
public:
	CODBCDatabase(void);
	virtual ~CODBCDatabase(void);

	int Init();

	virtual IDBConnection * CreateConnection();
	virtual int DeleteConnection(IDBConnection * pDBConnection);	


	SQLHENV GetHandle();
};

inline SQLHENV CODBCDatabase::GetHandle()
{
	return m_hEnv;
}

}