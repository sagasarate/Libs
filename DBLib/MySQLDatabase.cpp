/****************************************************************************/
/*                                                                          */
/*      文件名:    MySQLDatabase.cpp                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(CMySQLDatabase,IDatabase);

CMySQLDatabase::CMySQLDatabase(void):IDatabase()
{
	mysql_library_init(0,NULL,NULL);
}

CMySQLDatabase::~CMySQLDatabase(void)
{
	mysql_library_end();
}


IDBConnection * CMySQLDatabase::CreateConnection()
{
	CMySQLConnection * pConnection = MONITORED_NEW(_T("CMySQLDatabase"), CMySQLConnection);
	if(pConnection->Init(this)==DBERR_SUCCEED)
		return pConnection;
	pConnection->Release();
	return NULL;
}

int CMySQLDatabase::DeleteConnection(IDBConnection * pDBConnection)
{
	if(pDBConnection)
	{
		pDBConnection->Release();
		return DBERR_SUCCEED;
	}
	return DBERR_INVALID_CONNECTION;
}

}
