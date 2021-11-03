/****************************************************************************/
/*                                                                          */
/*      文件名:    ODBCConnection.cpp                                       */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include <odbcss.h>


#define MAX_MSG_BUFF	1024


// SQLSetConnectAttr driver specific defines.
// Microsoft has 1200 thru 1249 reserved for Microsoft SQL Native Client driver usage.
// Connection attributes
#define SQL_COPT_SS_BASE                            1200
#define SQL_COPT_SS_REMOTE_PWD                      (SQL_COPT_SS_BASE+1) // dbrpwset SQLSetConnectOption only
#define SQL_COPT_SS_USE_PROC_FOR_PREP               (SQL_COPT_SS_BASE+2) // Use create proc for SQLPrepare
#define SQL_COPT_SS_INTEGRATED_SECURITY             (SQL_COPT_SS_BASE+3) // Force integrated security on login
#define SQL_COPT_SS_PRESERVE_CURSORS                (SQL_COPT_SS_BASE+4) // Preserve server cursors after SQLTransact
#define SQL_COPT_SS_USER_DATA                       (SQL_COPT_SS_BASE+5) // dbgetuserdata/dbsetuserdata
#define SQL_COPT_SS_ENLIST_IN_DTC                   SQL_ATTR_ENLIST_IN_DTC // Enlist in a DTC transaction
#define SQL_COPT_SS_ENLIST_IN_XA                    SQL_ATTR_ENLIST_IN_XA // Enlist in a XA transaction
#define SQL_COPT_SS_FALLBACK_CONNECT                (SQL_COPT_SS_BASE+10) // Enables FallBack connections
#define SQL_COPT_SS_PERF_DATA                       (SQL_COPT_SS_BASE+11) // Used to access SQL Server ODBC driver performance data
#define SQL_COPT_SS_PERF_DATA_LOG                   (SQL_COPT_SS_BASE+12) // Used to set the logfile name for the Performance data
#define SQL_COPT_SS_PERF_QUERY_INTERVAL             (SQL_COPT_SS_BASE+13) // Used to set the query logging threshold in milliseconds.
#define SQL_COPT_SS_PERF_QUERY_LOG                  (SQL_COPT_SS_BASE+14) // Used to set the logfile name for saving queryies.
#define SQL_COPT_SS_PERF_QUERY                      (SQL_COPT_SS_BASE+15) // Used to start and stop query logging.
#define SQL_COPT_SS_PERF_DATA_LOG_NOW               (SQL_COPT_SS_BASE+16) // Used to make a statistics log entry to disk.
#define SQL_COPT_SS_QUOTED_IDENT                    (SQL_COPT_SS_BASE+17) // Enable/Disable Quoted Identifiers
#define SQL_COPT_SS_ANSI_NPW                        (SQL_COPT_SS_BASE+18) // Enable/Disable ANSI NULL, Padding and Warnings
#define SQL_COPT_SS_BCP                             (SQL_COPT_SS_BASE+19) // Allow BCP usage on connection
#define SQL_COPT_SS_TRANSLATE                       (SQL_COPT_SS_BASE+20) // Perform code page translation
#define SQL_COPT_SS_ATTACHDBFILENAME                (SQL_COPT_SS_BASE+21) // File name to be attached as a database
#define SQL_COPT_SS_CONCAT_NULL                     (SQL_COPT_SS_BASE+22) // Enable/Disable CONCAT_NULL_YIELDS_NULL
#define SQL_COPT_SS_ENCRYPT                         (SQL_COPT_SS_BASE+23) // Allow strong encryption for data
#define SQL_COPT_SS_MARS_ENABLED                    (SQL_COPT_SS_BASE+24) // Multiple active result set per connection
#define SQL_COPT_SS_FAILOVER_PARTNER                (SQL_COPT_SS_BASE+25) // Failover partner server
#define SQL_COPT_SS_OLDPWD                          (SQL_COPT_SS_BASE+26) // Old Password, used when changing password during login
#define SQL_COPT_SS_TXN_ISOLATION                   (SQL_COPT_SS_BASE+27) // Used to set/get any driver-specific or ODBC-defined TXN iso level
#define SQL_COPT_SS_TRUST_SERVER_CERTIFICATE        (SQL_COPT_SS_BASE+28) // Trust server certificate

namespace DBLib
{

IMPLEMENT_CLASS_INFO_STATIC(CODBCConnection,IDBConnection);

CODBCConnection::CODBCConnection(void):IDBConnection()
{
	m_pDatabase=NULL;
	m_hDBConn=NULL;	
	m_hStmt=NULL;	
	m_LastErrorCode=0;
}

CODBCConnection::~CODBCConnection(void)
{
	Destory();
}

int CODBCConnection::Init(CODBCDatabase * pDatabase)
{
	m_pDatabase=pDatabase;
	int nResult;

	nResult = SQLAllocHandle( SQL_HANDLE_DBC,m_pDatabase->GetHandle() , &m_hDBConn );
	if ( nResult != SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_ENV, m_pDatabase->GetHandle(),"分配连接句柄失败！\r\n", TRUE);
		return DBERR_SQLALLOCHANDLEFAIL;
	}	

	return DBERR_SUCCEED;
}

void CODBCConnection::Destory()
{
	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;			
	}
	if( m_hDBConn != NULL )
	{
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDBConn );
		m_hDBConn = NULL;		
	}	
	m_pDatabase=NULL;	
}

IDBRecordSet * CODBCConnection::CreateRecordSet(int RecordSetType)
{
	switch(RecordSetType)
	{
	case DB_RS_TYPE_STATIC:		
	case DB_RS_TYPE_GENERAL_STATIC:
		return MONITORED_NEW(_T("CODBCConnection"), CDBStaticRecordSet);
	case DB_RS_TYPE_DYNAMIC:
		return MONITORED_NEW(_T("CODBCConnection"), CODBCRecordSet);
	}
	return NULL;
}

IDBParameterSet * CODBCConnection::CreateParameterSet(int RecordSetType)
{
	return MONITORED_NEW(_T("CODBCConnection"), CDBParameterSet);
}


int CODBCConnection::Connect(LPCSTR ConnectStr)
{
	int nResult;	

	char szBuffer[1025];
	int swStrLen; 

	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;
	}

	CSettingFile StrAnalyzer;
	StrAnalyzer.Load(ConnectStr,';',0);
	const char * FlagsStr=StrAnalyzer.GetString(NULL,"Flag","");

	SetConnectFlags(FlagsStr);


	nResult = SQLDriverConnect( m_hDBConn, NULL, (SQLCHAR *)ConnectStr, (SQLSMALLINT)strlen(ConnectStr),
		(SQLCHAR *)szBuffer,1024,(SQLSMALLINT *)&swStrLen,
		SQL_DRIVER_COMPLETE_REQUIRED|SQL_DRIVER_NOPROMPT); 
	if ( nResult != SQL_SUCCESS )
	{
		if ( nResult != SQL_SUCCESS_WITH_INFO )
		{
			ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"连接失败！\r\n", TRUE);
			return DBERR_ODBC_SQLCONNECTFAIL;
		}
	}
	
	return DBERR_SUCCEED;
}

int CODBCConnection::Disconnect()
{
	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;
	}
	if( SQLDisconnect( m_hDBConn ) != SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"关闭连接失败！\r\n", TRUE);
		return DBERR_ODBC_SQLDISCONNECTFAIL;
	}
	return DBERR_SUCCEED;
}

BOOL CODBCConnection::IsConnected()
{
	if(m_hDBConn==NULL)
		return FALSE;
	int nResult;

	SQLINTEGER Value; 
	nResult=SQLGetConnectAttr(m_hDBConn,SQL_ATTR_CONNECTION_DEAD,&Value,sizeof(Value),NULL);
	if(nResult!=SQL_SUCCESS)
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"检查连接状态失败！\r\n", TRUE);
		return FALSE;
	}
	if(Value==SQL_CD_TRUE)
		return FALSE;
	else
		return TRUE;
}

int CODBCConnection::ExecuteSQL(LPCSTR SQLStr,int StrLen,IDBParameterSet * pParamSet)
{
	if(pParamSet)
	{	
		return ExecuteSQLWithParam(SQLStr,StrLen,(CDBParameterSet *)pParamSet);
	}
	else
	{
		return ExecuteSQLDirect(SQLStr,StrLen);
	}
}
int CODBCConnection::ExecuteSQLWithoutResultSet(LPCSTR SQLStr, int StrLen, IDBParameterSet * pParamSet)
{
	return ExecuteSQL(SQLStr, StrLen, pParamSet);
}
int CODBCConnection::GetTables()
{
	int nResult;

	
	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;			
	}

	//分配句柄

	nResult=SQLAllocHandle( SQL_HANDLE_STMT, m_hDBConn, &m_hStmt );	
	if (  nResult!= SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"分配静态Statement句柄失败！\r\n", TRUE);
		return DBERR_SQLALLOCHANDLEFAIL;
	}	


	// 执行
	
	nResult=SQLTables( m_hStmt, NULL, 0, NULL, 0, NULL, 0, NULL, 0);

	if ( (nResult != SQL_SUCCESS) && (nResult != SQL_SUCCESS_WITH_INFO) && (nResult != SQL_NO_DATA)) 
	{	
		ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取所有表", TRUE);		
		return DBERR_EXE_SQL_FAIL;
	}	

	return DBERR_SUCCEED;
}

int CODBCConnection::GetResults(IDBRecordSet * pDBRecordset)
{	
	if(!pDBRecordset)
	{
		return DBERR_INVALID_PARAM;
	}	
	if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CODBCRecordSet)))
	{
		int Ret=((CODBCRecordSet *)pDBRecordset)->Init(this,m_hStmt);
		m_hStmt=NULL;
		return Ret;
	}
	else if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CDBStaticRecordSet))||
		pDBRecordset->IsKindOf(GET_CLASS_INFO(CDBFakeRecordSet)))
	{
		int Ret=FetchStaticResult(m_hStmt,(CDBStaticRecordSet *)pDBRecordset);
		return Ret;
	}
	
	
	return DBERR_INVALID_PARAM;	
}

int CODBCConnection::NextResults(IDBRecordSet * pDBRecordset)
{
	int nResult;
	

	if(pDBRecordset&&pDBRecordset->IsKindOf(GET_CLASS_INFO(CODBCRecordSet)))
	{
		if(m_hStmt)
		{
			return DBERR_ERROR_CALL_SEQUENCE;		
		}
		m_hStmt=((CODBCRecordSet *)pDBRecordset)->StealHSTMT();				
	}
	

	if(m_hStmt)
	{
		SQLCloseCursor(m_hStmt);
		nResult=SQLMoreResults(m_hStmt);
		if(nResult==SQL_SUCCESS||nResult==SQL_SUCCESS_WITH_INFO)
			return DBERR_SUCCEED;
		else if(nResult==SQL_NO_DATA)
			return DBERR_NO_MORE_RESULTS;
		else 
		{
			ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"执行SQL失败！\r\n", TRUE);
			return DBERR_EXE_SQL_FAIL;
		}
	}
	else
	{
		return DBERR_INVALID_PARAM;
	}
}

int CODBCConnection::GetAffectedRowCount()
{
	SQLLEN RowCount;
	int nResult=SQLRowCount(m_hStmt,&RowCount);
	if(nResult==SQL_SUCCESS||nResult==SQL_SUCCESS_WITH_INFO)
	{
		return (int)RowCount;
	}
	ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取影响行数失败\r\n", TRUE);
	return -1;
}

int CODBCConnection::FetchStaticResult(SQLHSTMT hStmt,CDBStaticRecordSet * pDBRecordset)
{
	CGrowBuffer ResultBuff;
	int nResult;
	UINT ColNum=0;

	
	//获取结果集列数
	nResult=SQLNumResultCols(hStmt,(SQLSMALLINT *)&ColNum);
	if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
	{
		ProcessMessagesODBC(SQL_HANDLE_STMT, hStmt,"获取结果集列数失败！\r\n", TRUE);
		return DBERR_ODBC_GETCOLNUMFAIL;
	}
	if(ColNum<=0)
		return DBERR_NO_RECORDS;

	CEasyArray<DB_COLUMN_INFO> ColInfos(_T("CODBCConnection"));
	CEasyArray<int> BindTypes(_T("CODBCConnection"));
	UINT RecordLineLen=0;	

	ColInfos.Resize(ColNum);
	BindTypes.Resize(ColNum);
	

	//获取结果集列信息
	for(UINT i=0;i<ColNum;i++)
	{		
		int ColNameLen;
		BOOL CanNULL;

		nResult=SQLDescribeCol(hStmt,i+1,(SQLCHAR *)ColInfos[i].Name,MAX_COLUMN_NAME,
			(SQLSMALLINT *)&ColNameLen,	(SQLSMALLINT *)&ColInfos[i].Type,
			(SQLULEN *)&ColInfos[i].Size,(SQLSMALLINT *)&ColInfos[i].DigitSize,
			(SQLSMALLINT *)&CanNULL);
		if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
		{
			ProcessMessagesODBC(SQL_HANDLE_STMT, hStmt,"获取结果集列信息失败！\r\n", TRUE);
			return DBERR_ODBC_GETCOLINFOFAIL;
		}
		ColInfos[i].Name[MAX_COLUMN_NAME-1]=0;
		BindTypes[i]=ODBCSQLTypeTOODBCCType(ColInfos[i].Type,ColInfos[i].Size);		
		ColInfos[i].Type=ODBCCTypeTODBLibType(BindTypes[i],ColInfos[i].Size);	
		if(ColInfos[i].Size>MAX_FEILD_LEN)
			ColInfos[i].Size=MAX_FEILD_LEN;
		RecordLineLen+=(UINT)ColInfos[i].Size;		
	}

	////获取结果集行数
	//UINT RecordNum=0;

	//do{	
	//	nResult=SQLFetchScroll(hStmt,SQL_FETCH_NEXT,0);
	//	if(nResult == SQL_SUCCESS || nResult == SQL_SUCCESS_WITH_INFO)
	//		RecordNum++;		
	//}while(nResult == SQL_SUCCESS || nResult == SQL_SUCCESS_WITH_INFO);


	//计算需要的缓冲头部大小
	int HeadSize=sizeof(UINT)+sizeof(DB_COLUMN_INFO)*ColNum+sizeof(UINT);

	ResultBuff.Create(HeadSize,RecordLineLen+sizeof(UINT)*ColNum);

	
	ResultBuff.PushBack(&ColNum,sizeof(UINT));
	for(UINT i=0;i<ColNum;i++)
	{
		ResultBuff.PushBack(&(ColInfos[i]),sizeof(DB_COLUMN_INFO));
	}
	UINT * pRecordNum=(UINT *)ResultBuff.GetFreeBuffer();
	ResultBuff.PushConstBack(0,sizeof(UINT));
	
	
	//绑定结果集列
	CEasyBuffer RecordLineBuffer(_T("CODBCConnection"));
	RecordLineBuffer.Create(RecordLineLen);
	CEasyArray<SQLLEN> FieldSize(_T("CODBCConnection"));
	FieldSize.Resize(ColNum);
	char * pFieldBuffer=(char *)RecordLineBuffer.GetFreeBuffer();	
	for(UINT i=0;i<ColNum;i++)
	{		
		nResult = SQLBindCol(hStmt,i+1,BindTypes[i],
			pFieldBuffer,
			ColInfos[i].Size, 
			&(FieldSize[i]));
		if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO)
		{
			ProcessMessagesODBC(SQL_HANDLE_STMT, hStmt,"绑定结果集列失败！\r\n", TRUE);
			return DBERR_BINDCOLFAIL;
		}		
		pFieldBuffer+=ColInfos[i].Size;
	}
	
	//获取结果集数据	
	do{
		ZeroMemory(RecordLineBuffer.GetBuffer(),RecordLineLen);
		nResult=SQLFetch(hStmt);
		if(nResult == SQL_SUCCESS || nResult == SQL_SUCCESS_WITH_INFO)
		{		
			(*pRecordNum)++;
			pFieldBuffer=(char *)RecordLineBuffer.GetFreeBuffer();	
			for(UINT i=0;i<ColNum;i++)
			{
				ResultBuff.PushConstBack((UINT)FieldSize[i], sizeof(int));
				if(FieldSize[i]>0)
					ResultBuff.PushBack(pFieldBuffer, (UINT)FieldSize[i]);
				pFieldBuffer += ColInfos[i].Size;
			}				
		}
	}while(nResult == SQL_SUCCESS || nResult == SQL_SUCCESS_WITH_INFO);

	if(nResult != SQL_NO_DATA )
	{
		ProcessMessagesODBC(SQL_HANDLE_STMT, hStmt,"获取结果集数据！\r\n", TRUE);
		return DBERR_FETCH_RESULT_FAIL;
	}
	ResultBuff.MakeSmooth();

	if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CDBStaticRecordSet)))
		return ((CDBStaticRecordSet *)pDBRecordset)->Init(ResultBuff.GetBuffer(),ResultBuff.GetUsedSize());	
	else if(pDBRecordset->IsKindOf(GET_CLASS_INFO(CDBFakeRecordSet)))
		return ((CDBFakeRecordSet *)pDBRecordset)->Init(ResultBuff.GetBuffer(),ResultBuff.GetUsedSize());	
	else
		return DBERR_INVALID_PARAM;
}

int CODBCConnection::EnableTransaction(BOOL IsEnable)
{
	int nResult;

	if(IsEnable)
		nResult=SQLSetConnectAttr(m_hDBConn, SQL_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_OFF,0);
	else
		nResult=SQLSetConnectAttr(m_hDBConn, SQL_AUTOCOMMIT, (SQLPOINTER)SQL_AUTOCOMMIT_ON,0);

	if ( nResult != SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"改变事务设置失败！\r\n", TRUE);
		return DBERR_ODBC_ENABLETRANSACTIONFAIL;
	}
	else
		return DBERR_SUCCEED;
}	

int CODBCConnection::Commit()
{
	if (SQLEndTran(SQL_HANDLE_DBC , m_hDBConn, SQL_COMMIT)!= SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"提交事务失败！\r\n", TRUE);
		return DBERR_ODBC_COMMITFAIL;
	}
	else
		return DBERR_SUCCEED;
}

int CODBCConnection::RollBack()
{
	if (SQLEndTran(SQL_HANDLE_DBC , m_hDBConn, SQL_ROLLBACK)!= SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"回滚事务失败！\r\n", TRUE);
		return DBERR_ODBC_ROLLBACKFAIL;
	}
	else
		return DBERR_SUCCEED;
}

UINT CODBCConnection::GetLastDatabaseErrorCode()
{
	return m_LastErrorCode;
}

LPCSTR CODBCConnection::GetLastDatabaseErrorString()
{
	return m_LastErrorString;
}

LPCSTR CODBCConnection::GetLastSQL()
{
	return m_LastSQL;
}

int CODBCConnection::TranslateString(LPCSTR szSource,int SrcLen,LPTSTR szTarget,int MaxLen)
{
	int Len;
	if(SrcLen>MaxLen)
		Len=MaxLen;
	else
		Len=SrcLen;
	for(int i=0;i<Len;i++)
	{
		//if(szSource[i]<' ')
		//	szTarget[i]='?';
		if(szSource[i]=='\'')
			szTarget[i]='?';
		else
			szTarget[i]=szSource[i];
	}	
	return Len;
}

void CODBCConnection::Reset()
{

}

int CODBCConnection::SelectDefaultDatabase(LPCTSTR szDBName)
{
	return DBERR_NOT_IMPLEMENT;
}

void CODBCConnection::ProcessMessagesODBC(SQLSMALLINT plm_handle_type,SQLHANDLE plm_handle,char *logstring,int ConnInd)
{
	RETCODE		plm_retcode = SQL_SUCCESS;
	UCHAR		plm_szSqlState[MAX_MSG_BUFF] = "";
	UCHAR		plm_szErrorMsg[MAX_MSG_BUFF] = "";
	SDWORD		plm_pfNativeError = 0L;
	SWORD		plm_pcbErrorMsg = 0;
	SQLSMALLINT	plm_cRecNmbr = 1;
	SDWORD		plm_SS_MsgState = 0, plm_SS_Severity = 0;
	SQLINTEGER	plm_Rownumber = 0;
	USHORT		plm_SS_Line;
	SQLSMALLINT	plm_cbSS_Procname, plm_cbSS_Srvname;
	SQLCHAR		plm_SS_Procname[MAXNAME], plm_SS_Srvname[MAXNAME];
	CEasyString	ErrorString;

	plm_SS_Procname[0]=0;
	plm_SS_Srvname[0]=0;


	m_LastErrorString=logstring;
	m_LastErrorString+="\r\n";
	
	while (plm_retcode == SQL_SUCCESS||plm_retcode == SQL_SUCCESS_WITH_INFO) 
	{
		plm_retcode = SQLGetDiagRec(plm_handle_type, 
			plm_handle,
			plm_cRecNmbr, 
			plm_szSqlState, 
			&plm_pfNativeError,
			plm_szErrorMsg, 
			MAX_MSG_BUFF - 1, 
			&plm_pcbErrorMsg);

		// Note that if the application has not yet made a
		// successful connection, the SQLGetDiagField
		// information has not yet been cached by ODBC
		// Driver Manager and these calls to SQLGetDiagField
		// will fail.
		if (plm_retcode == SQL_SUCCESS||plm_retcode == SQL_SUCCESS_WITH_INFO) 
		{
			if (ConnInd) 
			{
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_ROW_NUMBER, 
					&plm_Rownumber,
					SQL_IS_INTEGER,
					NULL);
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_SS_LINE, 
					&plm_SS_Line,
					SQL_IS_INTEGER,
					NULL);
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_SS_MSGSTATE, 
					&plm_SS_MsgState,
					SQL_IS_INTEGER,
					NULL);
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_SS_SEVERITY, 
					&plm_SS_Severity,
					SQL_IS_INTEGER,
					NULL);
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_SS_PROCNAME, 
					&plm_SS_Procname,
					sizeof(plm_SS_Procname),
					&plm_cbSS_Procname);
				plm_retcode = SQLGetDiagField(
					plm_handle_type, 
					plm_handle, 
					plm_cRecNmbr,
					SQL_DIAG_SS_SRVNAME, 
					&plm_SS_Srvname,
					sizeof(plm_SS_Srvname),
					&plm_cbSS_Srvname);
			}		
			m_LastErrorCode=plm_pfNativeError;

			ErrorString.Format("szSqlState = %s\r\n",plm_szSqlState);
			m_LastErrorString+=ErrorString;
			ErrorString.Format("pfNativeError = %d\r\n",plm_pfNativeError);
			m_LastErrorString+=ErrorString;
			ErrorString.Format("szErrorMsg = %s\r\n",plm_szErrorMsg);
			m_LastErrorString+=ErrorString;
			ErrorString.Format("pcbErrorMsg = %d\r\n\r\n",plm_pcbErrorMsg);
			m_LastErrorString+=ErrorString;
			if (ConnInd) 
			{
				ErrorString.Format("ODBCRowNumber = %d\r\n", plm_Rownumber);
				m_LastErrorString+=ErrorString;
				ErrorString.Format("SSrvrLine = %d\r\n", plm_Rownumber);
				m_LastErrorString+=ErrorString;
				ErrorString.Format("SSrvrMsgState = %d\r\n",plm_SS_MsgState);
				m_LastErrorString+=ErrorString;
				ErrorString.Format("SSrvrSeverity = %d\r\n",plm_SS_Severity);
				m_LastErrorString+=ErrorString;
				ErrorString.Format("SSrvrProcname = %s\r\n",plm_SS_Procname);
				m_LastErrorString+=ErrorString;
				ErrorString.Format("SSrvrSrvname = %s\r\n\r\n",plm_SS_Srvname);
				m_LastErrorString+=ErrorString;
			}
		}
		plm_cRecNmbr++; 
	} 
	PrintDBLog("%s", (LPCSTR)m_LastErrorString);
}

int CODBCConnection::ODBCCTypeTODBLibType(int Type,UINT64& Size)
{
	switch(Type)
	{	
	case SQL_C_CHAR:
		return DB_TYPE_STRING;
	case SQL_C_WCHAR:		
		return DB_TYPE_WSTRING;
	case SQL_C_SSHORT:
	case SQL_C_USHORT:		
		return DB_TYPE_SMALLINT;
	case SQL_C_SLONG:
	case SQL_C_ULONG:		
		return DB_TYPE_INTEGER;
	case SQL_C_FLOAT:		
		return DB_TYPE_FLOAT;
	case SQL_C_DOUBLE:		
		return DB_TYPE_DOUBLE;
	case SQL_C_BIT:
	case SQL_C_TINYINT:
	case SQL_C_STINYINT:
	case SQL_C_UTINYINT:		
		return DB_TYPE_TINY;
	case SQL_C_SBIGINT:
	case SQL_C_UBIGINT:		
		return DB_TYPE_BIGINT;
	case SQL_C_BINARY:		
		return DB_TYPE_BINARY;
	case SQL_C_TYPE_DATE:		
		return DB_TYPE_DATE;
	case SQL_C_TYPE_TIME:
		return DB_TYPE_TIME;
	case SQL_C_TYPE_TIMESTAMP:
		return DB_TYPE_TIMESTAMP;	
	case SQL_C_GUID:
		return DB_TYPE_GUID;
	}
	return DB_TYPE_UNKNOW;
}

int CODBCConnection::DBLibTypeToODBCCType(int Type,UINT64& Size)
{
	switch(Type)
	{
	case DB_TYPE_STRING:
		return SQL_C_CHAR;
	case DB_TYPE_WSTRING:
		return SQL_C_WCHAR;
	case DB_TYPE_TINY:
		return DB_TYPE_TINY;
	case DB_TYPE_SMALLINT:
		return SQL_C_SSHORT;
	case DB_TYPE_INTEGER:
		return SQL_C_SLONG;
	case DB_TYPE_BIGINT:
		return SQL_C_SBIGINT;
	case DB_TYPE_FLOAT:
		return SQL_C_FLOAT;
	case DB_TYPE_DOUBLE:
		return SQL_C_DOUBLE;
	case DB_TYPE_DATE:
		return SQL_C_TYPE_DATE;
	case DB_TYPE_TIME:
		return SQL_C_TYPE_TIME;
	case DB_TYPE_TIMESTAMP:
		return SQL_C_TYPE_TIMESTAMP;
	case DB_TYPE_GUID:
		return SQL_C_GUID;
	case DB_TYPE_BINARY:
		return SQL_C_BINARY;	
	}
	return Type;
}

int CODBCConnection::DBLibTypeToODBCSQLType(int Type,UINT64& Size)
{
	switch(Type)
	{
	case DB_TYPE_STRING:
		return SQL_VARCHAR;
	case DB_TYPE_WSTRING:
		return SQL_WVARCHAR;
	case DB_TYPE_TINY:
		return SQL_TINYINT;
	case DB_TYPE_SMALLINT:
		return SQL_SMALLINT;
	case DB_TYPE_INTEGER:
		return SQL_INTEGER;
	case DB_TYPE_BIGINT:
		return SQL_BIGINT;
	case DB_TYPE_FLOAT:
		return SQL_FLOAT;
	case DB_TYPE_DOUBLE:
		return SQL_DOUBLE;
	case DB_TYPE_DATE:
		return SQL_TYPE_DATE;
	case DB_TYPE_TIME:
		return SQL_TYPE_TIME;
	case DB_TYPE_TIMESTAMP:
		return SQL_TYPE_TIMESTAMP;
	case DB_TYPE_GUID:
		return SQL_GUID;
	case DB_TYPE_BINARY:
		return SQL_VARBINARY;	
	}
	return Type;
}

int CODBCConnection::ODBCSQLTypeTOODBCCType(int Type,UINT64& Size)
{
	switch(Type)
	{	
	case SQL_CHAR:
	case SQL_VARCHAR:
	case SQL_LONGVARCHAR:
		Size=Size+1;
		return SQL_C_CHAR;
	case SQL_WCHAR:
	case SQL_WVARCHAR:
	case SQL_WLONGVARCHAR:
		Size=Size+2;
		return SQL_C_WCHAR ;
	case SQL_BIT:
	case SQL_TINYINT:
		Size=sizeof(BYTE);
		return SQL_C_TINYINT;
	case SQL_SMALLINT:
		Size=sizeof(WORD);
		return SQL_C_SSHORT;
	case SQL_INTEGER:
		Size=sizeof(DWORD);
		return SQL_C_SLONG;
	case SQL_BIGINT:
		Size=sizeof(INT64);
		return SQL_C_SBIGINT;
	case SQL_FLOAT:
		Size=sizeof(float);
		return SQL_C_FLOAT;
	case SQL_DECIMAL:
	case SQL_NUMERIC:
		Size=MAX_DECIMAL_LEN;
		return SQL_C_CHAR;
	case SQL_REAL:		
	case SQL_DOUBLE:	
		Size=sizeof(double);
		return SQL_C_DOUBLE;
	case SQL_BINARY:
	case SQL_VARBINARY:
	case SQL_LONGVARBINARY:
		return SQL_C_BINARY;
	case SQL_TYPE_DATE:
		Size=sizeof(DATE_STRUCT);
		return SQL_C_TYPE_DATE;
	case SQL_TYPE_TIME:
		Size=sizeof(TIME_STRUCT);
		return SQL_C_TYPE_TIME;
	case SQL_TYPE_TIMESTAMP:
		Size=sizeof(TIMESTAMP_STRUCT);
		return SQL_C_TYPE_TIMESTAMP;
	case SQL_INTERVAL_MONTH:
	case SQL_INTERVAL_YEAR:
	case SQL_INTERVAL_YEAR_TO_MONTH:
	case SQL_INTERVAL_DAY:
	case SQL_INTERVAL_HOUR:
	case SQL_INTERVAL_MINUTE:
	case SQL_INTERVAL_SECOND:
	case SQL_INTERVAL_DAY_TO_HOUR:
	case SQL_INTERVAL_DAY_TO_MINUTE:
	case SQL_INTERVAL_DAY_TO_SECOND:
	case SQL_INTERVAL_HOUR_TO_MINUTE:
	case SQL_INTERVAL_HOUR_TO_SECOND:
	case SQL_INTERVAL_MINUTE_TO_SECOND:
		Size=sizeof(double);
		return SQL_C_DOUBLE;
	case SQL_GUID:
		Size=sizeof(SQLGUID);
		return SQL_C_GUID;
	default:
		Size=MAX_DECIMAL_LEN;
		return SQL_C_CHAR;
	}
}

int CODBCConnection::DBParamTypeToODBCParamType(int Type)
{
	switch(Type)
	{
	case DB_PARAM_TYPE_INPUT:
		return SQL_PARAM_INPUT;
	case DB_PARAM_TYPE_OUTPUT:
		return SQL_PARAM_OUTPUT;
	case DB_PARAM_TYPE_INPUT_OUTPUT:
		return SQL_PARAM_INPUT_OUTPUT;
	}
	return Type;
}

int CODBCConnection::ExecuteSQLDirect(LPCSTR SQLStr,int StrLen)
{
	int nResult;	

	if(!SQLStr)
	{
		return DBERR_INVALID_PARAM;
	}
	if (StrLen)
		m_LastSQL.SetString(SQLStr, StrLen);
	else
		m_LastSQL = SQLStr;

	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;			
	}

	//分配句柄

	nResult=SQLAllocHandle( SQL_HANDLE_STMT, m_hDBConn, &m_hStmt );	
	if (  nResult!= SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"分配静态Statement句柄失败！\r\n", TRUE);
		return DBERR_SQLALLOCHANDLEFAIL;
	}	


	// 执行
	if(StrLen==0)
		StrLen=SQL_NTS;
	nResult = SQLExecDirect(m_hStmt,(SQLCHAR*)SQLStr, StrLen);


	if ( (nResult != SQL_SUCCESS) && (nResult != SQL_SUCCESS_WITH_INFO) && (nResult != SQL_NO_DATA)) 
	{		
		CEasyString ErrorSQL;
		ErrorSQL="执行SQL[";
		if(StrLen<=0)
			StrLen=(int)strlen(SQLStr);
		ErrorSQL.AppendString(SQLStr,StrLen);
		ErrorSQL+="失败]";
		ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,ErrorSQL, TRUE);		
		return DBERR_EXE_SQL_FAIL;
	}	

	return DBERR_SUCCEED;
}

int  CODBCConnection::ExecuteSQLWithParam(LPCSTR SQLStr,int StrLen,CDBParameterSet * pParamSet)
{
	int nResult;	

	if(!SQLStr)
	{
		return DBERR_INVALID_PARAM;
	}
	if (StrLen)
		m_LastSQL.SetString(SQLStr, StrLen);
	else
		m_LastSQL = SQLStr;

	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;			
	}

	//分配句柄

	nResult=SQLAllocHandle( SQL_HANDLE_STMT, m_hDBConn, &m_hStmt );	
	if (  nResult!= SQL_SUCCESS )
	{
		ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"分配静态Statement句柄失败！\r\n", TRUE);
		return DBERR_SQLALLOCHANDLEFAIL;
	}	


	// 预执行
	if(StrLen==0)
		StrLen=SQL_NTS;
	nResult = SQLPrepare(m_hStmt,(SQLCHAR*)SQLStr, StrLen);

	if ( (nResult != SQL_SUCCESS) && (nResult != SQL_SUCCESS_WITH_INFO) ) 
	{		
		CEasyString ErrorSQL;
		ErrorSQL="预执行SQL[";
		if(StrLen<=0)
			StrLen=(int)strlen(SQLStr);
		ErrorSQL.AppendString(SQLStr,StrLen);
		ErrorSQL+="失败]";
		ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,ErrorSQL, TRUE);		
		return DBERR_EXE_SQL_FAIL;
	}	

	//获取参数数量
	int ParamNum=0;
	nResult=SQLNumParams (m_hStmt,(SQLSMALLINT *)&ParamNum);
	if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
	{
		ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取参数数量失败！\r\n", TRUE);
		return DBERR_PARAMCOUNTFAIL;
	}

	CEasyBuffer ParamDataLenBuffer(_T("CODBCConnection"));

	if(ParamNum>0)
	{

		if(ParamNum>pParamSet->GetCount())
		{
			return DBERR_NOTENOUGHPARAM;
		}	
		
		ParamDataLenBuffer.Create(sizeof(SQLLEN)*ParamNum);

		SQLLEN * pParamDataLen=(SQLLEN *)ParamDataLenBuffer.GetBuffer();

		//绑定参数
		for(int i=0;i<ParamNum;i++)
		{
			int ParamType=DBParamTypeToODBCParamType(pParamSet->GetParamInfo(i)->ParamType);
			UINT64 Size=pParamSet->GetParam(i).GetLength();
			int DigitalSize=pParamSet->GetParam(i).GetDigitalLength();
			int ODBCSQLType=DBLibTypeToODBCSQLType(pParamSet->GetParamInfo(i)->Type,Size);
			int ODBCCType=DBLibTypeToODBCCType(pParamSet->GetParamInfo(i)->Type,Size);

			SQLSMALLINT tParamType,tParamDigitalSize,tParamCanNull;
			SQLULEN tParamSize;

			nResult=SQLDescribeParam(m_hStmt,i+1,&tParamType,&tParamSize,&tParamDigitalSize,&tParamCanNull);
			if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
			{
				ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"绑定参数失败！\r\n", TRUE);
				return DBERR_BINDPARAMFAIL;
			}
			
			if(pParamSet->GetParamInfo(i)->IsNull||pParamSet->GetParam(i).IsNull())
				pParamDataLen[i]=SQL_NULL_DATA;
			else if(ODBCSQLType==SQL_BINARY||ODBCSQLType==SQL_VARBINARY||ODBCSQLType==SQL_LONGVARBINARY)
				pParamDataLen[i]=pParamSet->GetParam(i).GetLength();
			else
				pParamDataLen[i]=SQL_NTS;
			int ColumnSize=0;
			if(ODBCSQLType==SQL_CHAR||ODBCSQLType==SQL_VARCHAR||ODBCSQLType==SQL_LONGVARCHAR||
				ODBCSQLType==SQL_BINARY||ODBCSQLType==SQL_VARBINARY||ODBCSQLType==SQL_LONGVARBINARY)
			{
				ColumnSize=(int)tParamSize;
			}
			if(ODBCSQLType==SQL_DECIMAL||ODBCSQLType==SQL_NUMERIC||ODBCSQLType==SQL_FLOAT||
				ODBCSQLType==SQL_REAL||ODBCSQLType==SQL_DOUBLE)
			{
				ColumnSize=SQL_DESC_PRECISION ;
			}
			nResult=SQLBindParameter(m_hStmt,i+1,
				ParamType,ODBCCType,ODBCSQLType,ColumnSize,DigitalSize,
				(SQLPOINTER)((LPCVOID)pParamSet->GetParam(i)),
				pParamSet->GetParam(i).GetLength(),pParamDataLen+i);


			if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
			{
				ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"绑定参数失败！\r\n", TRUE);
				return DBERR_BINDPARAMFAIL;
			}
		}
	}


	//正式执行
	nResult = SQLExecute(m_hStmt);

	if ( (nResult != SQL_SUCCESS) && (nResult != SQL_SUCCESS_WITH_INFO) && (nResult != SQL_NO_DATA)) 
	{		
		CEasyString ErrorSQL;
		ErrorSQL="预执行SQL[";
		if(StrLen<=0)
			StrLen=(int)strlen(SQLStr);
		ErrorSQL.AppendString(SQLStr,StrLen);
		ErrorSQL+="失败]";
		ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,ErrorSQL, TRUE);		
		return DBERR_EXE_SQL_FAIL;
	}	
	return DBERR_SUCCEED;
}

void CODBCConnection::SetConnectFlags(LPCSTR szFlags)
{
	CStringSplitter Splitter(szFlags,'|');
	for(int i=0;i<(int)Splitter.GetCount();i++)
	{
		CEasyString Flag=Splitter[i];
		Flag.Trim(' ');
		if(Flag.CompareNoCase("NC_MARS_ENABLED")==0)
		{
			PrintDBDebugLog("应用参数NC_MARS_ENABLED");
			int nResult=SQLSetConnectAttr(m_hDBConn, SQL_COPT_SS_MARS_ENABLED, (SQLPOINTER)1, SQL_IS_UINTEGER);
			if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
			{
				ProcessMessagesODBC(SQL_HANDLE_DBC, m_hDBConn,"应用参数NC_MARS_ENABLED失败！\r\n", TRUE);
			}
		}		
	}
}

}