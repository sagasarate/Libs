/****************************************************************************/
/*                                                                          */
/*      文件名:    ODBCRecordSet.cpp                                        */
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

IMPLEMENT_CLASS_INFO_STATIC(CODBCRecordSet,IDBRecordSet);

CODBCRecordSet::CODBCRecordSet(void)
{
	m_pDBConnection=NULL;
	m_hStmt=NULL;
	m_CurRow=-1;
	m_RecordCount=0;
	m_IsBOF=true;
	m_IsEOF=true;
	m_IsForwardOnly=true;
	m_ColInfos.SetTag(_T("CODBCRecordSet"));
	m_BindTypes.SetTag(_T("CODBCRecordSet"));
	m_FieldSize.SetTag(_T("CODBCRecordSet"));
	m_RecordLineBuffer.SetTag(_T("CODBCRecordSet"));
	m_RowBuffer.SetTag(_T("CODBCRecordSet"));
	m_ParamInfos.SetTag(_T("CODBCRecordSet"));
	m_ParamBuffer.SetTag(_T("CODBCRecordSet"));
}

CODBCRecordSet::~CODBCRecordSet(void)
{
	Destory();	
}

int CODBCRecordSet::Init(CODBCConnection * pDBConnection,SQLHSTMT hStmt)
{
	if(pDBConnection==NULL||hStmt==NULL)
		return DBERR_INVALID_PARAM;
	Destory();
	m_pDBConnection=pDBConnection;
	m_hStmt=hStmt;

	int nResult;
	UINT ColNum=0;


	int CursorType;
	SQLINTEGER AttrLen;
	nResult=SQLGetStmtAttr(m_hStmt,SQL_ATTR_CURSOR_TYPE,&CursorType,sizeof(int),&AttrLen);
	if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
	{
		m_pDBConnection->ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取环境句柄属性失败！\r\n", TRUE);
		return DBERR_ODBC_GETCOLNUMFAIL;
	}
	if(CursorType==SQL_CURSOR_FORWARD_ONLY)
		m_IsForwardOnly=true;
	else
		m_IsForwardOnly=false;

	//获取结果集列数
	nResult=SQLNumResultCols(m_hStmt,(SQLSMALLINT *)&ColNum);
	if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
	{
		m_pDBConnection->ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取结果集列数失败！\r\n", TRUE);
		return DBERR_ODBC_GETCOLNUMFAIL;
	}

	if(ColNum<=0)
		return DBERR_NO_RECORDS;	

	m_ColInfos.Resize(ColNum);
	m_BindTypes.Resize(ColNum);
	m_RowBuffer.Resize(ColNum);

	UINT RecordLineLen=0;


	//获取结果集列信息
	for(UINT i=0;i<ColNum;i++)
	{		
		int ColNameLen;
		BOOL CanNULL;

		nResult=SQLDescribeCol(m_hStmt,i+1,(SQLCHAR *)m_ColInfos[i].Name,MAX_COLUMN_NAME,
			(SQLSMALLINT *)&ColNameLen,	(SQLSMALLINT *)&m_ColInfos[i].Type,
			(SQLULEN *)&m_ColInfos[i].Size,(SQLSMALLINT *)&m_ColInfos[i].DigitSize,
			(SQLSMALLINT *)&CanNULL);
		if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
		{
			m_pDBConnection->ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取结果集列信息失败！\r\n", TRUE);
			return DBERR_ODBC_GETCOLINFOFAIL;
		}
		m_ColInfos[i].Name[MAX_COLUMN_NAME-1]=0;
		m_BindTypes[i]=m_pDBConnection->ODBCSQLTypeTOODBCCType(m_ColInfos[i].Type,m_ColInfos[i].Size);		
		m_ColInfos[i].Type=m_pDBConnection->ODBCCTypeTODBLibType(m_BindTypes[i],m_ColInfos[i].Size);		
		if(m_ColInfos[i].Size>MAX_FEILD_LEN)
			m_ColInfos[i].Size=MAX_FEILD_LEN;
		RecordLineLen+=(UINT)m_ColInfos[i].Size;		
	}	


	


	m_RecordLineBuffer.Create(RecordLineLen);
	
	m_FieldSize.Resize(ColNum);
	char * pFieldBuffer=(char *)m_RecordLineBuffer.GetFreeBuffer();	
	for(UINT i=0;i<ColNum;i++)
	{		
		nResult = SQLBindCol(m_hStmt,i+1,m_BindTypes[i],
			pFieldBuffer,
			m_ColInfos[i].Size, 
			&(m_FieldSize[i]));
		if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO)
		{
			m_pDBConnection->ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"绑定结果集列失败！\r\n", TRUE);
			return DBERR_BINDCOLFAIL;
		}		
		pFieldBuffer+=m_ColInfos[i].Size;
	}	
	
	return FetchRow(SQL_FETCH_NEXT,0);
}

void CODBCRecordSet::Destory()
{
	m_pDBConnection=NULL;
	if(m_hStmt)
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;			
	}
	m_ColInfos.Clear();
	m_BindTypes.Clear();
	m_RecordLineBuffer.Destory();
	m_CurRow=-1;
	m_RecordCount=0;
	m_RowBuffer.Clear();
}

SQLHSTMT CODBCRecordSet::GetHSTMT()
{
	return m_hStmt;
}

SQLHSTMT CODBCRecordSet::StealHSTMT()
{
	SQLHSTMT hStmt=m_hStmt;
	m_hStmt=NULL;
	return hStmt;
}

int CODBCRecordSet::FetchRow(int Orientation,int Offset)
{
	int Ret=DBERR_SUCCEED;
	SQLRETURN nResult;

	
	nResult=SQLFetchScroll(m_hStmt,Orientation,Offset+1);
	if(nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO)
	{	
		if(nResult == SQL_NO_DATA )		
		{			
			Ret=DBERR_NO_RECORDS;			
		}	
		else
		{
			m_pDBConnection->ProcessMessagesODBC(SQL_HANDLE_STMT, m_hStmt,"获取结果集数据失败！\r\n", TRUE);
			Ret=DBERR_FETCH_RESULT_FAIL;
			m_IsBOF=true;
			m_IsEOF=true;
			return Ret;
		}		
	}		
	
	if(Ret==DBERR_SUCCEED)
	{	
		int ColNum=(int)m_ColInfos.GetCount();
		m_RowBuffer.Resize(ColNum);		
		char * pFieldBuffer=(char *)m_RecordLineBuffer.GetFreeBuffer();	
		for(int i=0;i<ColNum;i++)
		{
			if(m_FieldSize[i]==SQL_NULL_DATA)
			{
				m_RowBuffer[i].SetValue(m_ColInfos[i].Type,NULL,0,m_ColInfos[i].DigitSize);
			}
			else
			{
				m_RowBuffer[i].SetValue(m_ColInfos[i].Type,pFieldBuffer,(int)m_FieldSize[i],m_ColInfos[i].DigitSize);
			}
			
			pFieldBuffer+=m_ColInfos[i].Size;
		}
	}	

	UpdateStatus(Ret==DBERR_SUCCEED,Orientation,Offset);
	return Ret;
}

void CODBCRecordSet::UpdateStatus(bool IsValid,int Orientation,int Offset)
{
	m_IsBOF=false;
	m_IsEOF=false;
	switch(Orientation)
	{	
	case SQL_FETCH_NEXT:
		if(!IsValid)
		{
			m_IsEOF=true;
		}
		else
		{
			m_CurRow++;			
		}
		break;
	case SQL_FETCH_PRIOR:
		if(!IsValid)
		{
			m_IsBOF=true;
		}
		else
		{
			m_CurRow--;
		}
		break;
	case SQL_FETCH_FIRST:
		if(!IsValid)
		{
			m_IsBOF=true;
			m_IsEOF=true;
		}
		else
		{
			m_CurRow=0;
		}
		break;
	case SQL_FETCH_LAST:
		if(!IsValid)
		{
			m_IsBOF=true;
			m_IsEOF=true;
		}
		else
		{
			m_CurRow=-1;
		}
		break;
	case SQL_FETCH_ABSOLUTE:
		if(!IsValid)
		{
			if(m_RecordCount)
			{
				if(Offset>m_RecordCount)
					m_IsEOF=true;
				else
					m_IsBOF=true;
			}
			else
			{
				m_IsBOF=true;
				m_IsEOF=true;
			}			
		}
		else
		{
			m_CurRow=Offset-1;
		}
		break;
	case SQL_FETCH_RELATIVE:
		break;
	case SQL_FETCH_BOOKMARK:
		break;
	}
	if(m_CurRow+1>m_RecordCount)
		m_RecordCount=m_CurRow+1;
}

int CODBCRecordSet::GetRecordCount()
{	
	return m_RecordCount;
}

int CODBCRecordSet::GetColumnCount()
{
	return (int)m_ColInfos.GetCount();
}

LPCSTR CODBCRecordSet::GetColumnName(int Index)
{
	if(Index>=0&&Index<(int)m_ColInfos.GetCount())
	{
		return m_ColInfos[Index].Name;
	}
	return NULL;
}

int CODBCRecordSet::GetIndexByColumnName(LPCSTR Name)
{
	for(int i=0;i<(int)m_ColInfos.GetCount();i++)
	{
		if(_strnicmp(m_ColInfos[i].Name,Name,MAX_COLUMN_NAME)==0)
			return i;
	}
	return -1;
}

DB_COLUMN_INFO * CODBCRecordSet::GetColumnInfo(int Index)
{
	if(Index>=0&&Index<(int)m_ColInfos.GetCount())
	{
		return &(m_ColInfos[Index]);
	}
	return NULL;
}


CDBValue& CODBCRecordSet::GetField(int Index)
{
	if(Index>=0&&Index<(int)m_RowBuffer.GetCount())
	{
		return m_RowBuffer[Index];
	}
	return m_EmptyValue;
}

CDBValue& CODBCRecordSet::GetField(LPCSTR Name)
{
	int Index=GetIndexByColumnName(Name);
	if(Index>=0)
	{
		return m_RowBuffer[Index];
	}
	return m_EmptyValue;
}

int CODBCRecordSet::MoveFirst()
{	
	return FetchRow(SQL_FETCH_FIRST,0);
}

int CODBCRecordSet::MoveLast()
{
	return FetchRow(SQL_FETCH_LAST,0);
}

int CODBCRecordSet::MoveNext()
{	
	return FetchRow(SQL_FETCH_NEXT,0);
}

int CODBCRecordSet::MovePrevious()
{	
	return FetchRow(SQL_FETCH_PRIOR,0);
}

int CODBCRecordSet::MoveTo(int Index)
{	
	return FetchRow(SQL_FETCH_ABSOLUTE,Index);
}

bool CODBCRecordSet::IsEOF()
{
	return m_IsEOF;
}

bool CODBCRecordSet::IsBOF()
{
	return m_IsBOF;
}

bool CODBCRecordSet::Close()
{
	if(m_hStmt)
	{	
		SQLRETURN Ret=SQLCloseCursor(m_hStmt);
		if(Ret==SQL_SUCCESS||Ret==SQL_SUCCESS_WITH_INFO)
		{
			return true;
		}
	}
	return false;
}

bool CODBCRecordSet::SetBlobMaxProcessSize(UINT64 MaxSize)
{
	return false;
}

}