#include "StdAfx.h"


CDBTransAccountLogin::CDBTransAccountLogin(OBJECT_ID CallerID,LPCTSTR szAccountName,LPCTSTR szPassword)
{
	FUNCTION_BEGIN;
	m_CallerID=CallerID;
	m_AccountName=szAccountName;
	m_Password=szPassword;
	m_AccountID=0;
	FUNCTION_END;
}

CDBTransAccountLogin::~CDBTransAccountLogin(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransAccountLogin::OnExecute(IDBConnection * pConnection)
{
	FUNCTION_BEGIN;
	char szSQL[1024];
	CSmartPtr<IDBRecordSet> pRecordSet;
	CDBParameterSet ParameterSet;
	DB_PARAM_INFO ParamInfo;
	int DBResult;

	pRecordSet=pConnection->CreateRecordSet(DB_RS_TYPE_DYNAMIC);
	if(pRecordSet==NULL)
	{
		Log("CDBTransAccountLogin::OnExecute:无法创建结果集");
		return false;
	}
	m_AccountID=0;

	sprintf_s(szSQL,1024,"select AccountID from T_ACCOUNT_INFO where AccountName=? and Password=? ");
	
	ParamInfo.Type=DB_TYPE_STRING;
	ParamInfo.ParamType=DB_PARAM_TYPE_INPUT;
	ParamInfo.IsNull=false;
	ParamInfo.Size=m_AccountName.GetLength();
	ParamInfo.DigitSize=0;
	ParameterSet.AddParam(ParamInfo,(LPCTSTR)m_AccountName);

	ParamInfo.Type=DB_TYPE_STRING;
	ParamInfo.ParamType=DB_PARAM_TYPE_INPUT;
	ParamInfo.IsNull=false;
	ParamInfo.Size=m_Password.GetLength();
	ParamInfo.DigitSize=0;
	ParameterSet.AddParam(ParamInfo,(LPCTSTR)m_Password);

	LogDebug("CDBTransAccountLogin::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,&ParameterSet);

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransAccountLogin::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransAccountLogin::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	DBResult=pConnection->GetResults(pRecordSet);
	if(DBResult==DBERR_SUCCEED)
	{
		m_AccountID=pRecordSet->GetField(0);
	}
	else
	{
		Log("CDBTransAccountLogin::OnExecute:获取结果集出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransAccountLogin::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	return true;
	FUNCTION_END;
	return false;
}
void CDBTransAccountLogin::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetAccountLogin(m_CallerID,m_AccountName,m_AccountID);
	FUNCTION_END;
}