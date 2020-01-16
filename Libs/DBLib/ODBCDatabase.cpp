/****************************************************************************/
/*                                                                          */
/*      文件名:    ODBCDatabase.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include <odbcss.h>

namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(CODBCDatabase,IDatabase);

CODBCDatabase::CODBCDatabase(void):IDatabase()
{
	m_hEnv=NULL;	
}

CODBCDatabase::~CODBCDatabase(void)
{
	if ( m_hEnv )
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		m_hEnv = NULL;		
	}	
}

int CODBCDatabase::Init()
{
	if ( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv ) != SQL_SUCCESS )
		return DBERR_SQLALLOCHANDLEFAIL;

	if ( SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER ) != SQL_SUCCESS )
		return DBERR_ODBC_SQLSETENVATTRFAIL;
	return DBERR_SUCCEED;
}

IDBConnection * CODBCDatabase::CreateConnection()
{
	CODBCConnection * pConnection = MONITORED_NEW(_T("CODBCDatabase"), CODBCConnection);
	if(pConnection->Init(this)==DBERR_SUCCEED)
		return pConnection;
	pConnection->Release();	
	return NULL;
}

int CODBCDatabase::DeleteConnection(IDBConnection * pDBConnection)
{
	if(pDBConnection)
	{
		pDBConnection->Release();
		return DBERR_SUCCEED;
	}
	return DBERR_INVALID_CONNECTION;
}


}