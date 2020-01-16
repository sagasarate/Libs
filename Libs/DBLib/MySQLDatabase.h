/****************************************************************************/
/*                                                                          */
/*      文件名:    MySQLDatabase.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



//#pragma comment(lib,"mysqlclient.lib")





namespace DBLib
{

enum DB_MYSQL_ERROR_INFO
{
	DBERR_MYSQL_MYSQLINITFAIL=DBERR_MAX,
	DBERR_MYSQL_CONNECTFAIL,
	DBERR_MYSQL_WANTCONNECT,
	DBERR_MYSQL_COMMITFAIL,
	DBERR_MYSQL_ROLLBACKFAIL,
	DBERR_MYSQL_ENABLETRANSACTIONFAIL,
	DBERR_MYSQL_MAX,
};


class CMySQLDatabase :
	public IDatabase
{
protected:

	DECLARE_CLASS_INFO_STATIC(CMySQLDatabase)
public:
	CMySQLDatabase(void);
	virtual ~CMySQLDatabase(void);

	virtual IDBConnection * CreateConnection();
	virtual int DeleteConnection(IDBConnection * pDBConnection);

};



}
