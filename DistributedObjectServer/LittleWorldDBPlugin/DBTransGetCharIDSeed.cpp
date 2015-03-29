#include "StdAfx.h"


CDBTransGetCharIDSeed::CDBTransGetCharIDSeed(void)
{
	FUNCTION_BEGIN;
	m_CharIDSeed=0;
	FUNCTION_END;	
}

CDBTransGetCharIDSeed::~CDBTransGetCharIDSeed(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransGetCharIDSeed::OnExecute(IDBConnection * pConnection)
{
	FUNCTION_BEGIN;
	char szSQL[1024];
	CSmartPtr<IDBRecordSet> pRecordSet;
	int DBResult;

	pRecordSet=pConnection->CreateRecordSet(DB_RS_TYPE_DYNAMIC);
	if(pRecordSet==NULL)
	{
		Log("CDBTransAccountLogin::OnExecute:无法创建结果集");
		return false;
	}

	m_CharIDSeed=0;

	sprintf_s(szSQL,1024,"select max(CharID) from T_PLAYER_CHAR_INFO ");

	

	LogDebug("CDBTransAccountLogin::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,NULL);

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
		if(pRecordSet->GetField(0).IsNull())
		{
			m_CharIDSeed=MAKE_LW_ID(LWIT_CHAR_ID,1);
		}
		else
		{
			m_CharIDSeed=pRecordSet->GetField(0);
			m_CharIDSeed++;
		}		
	}
	else if(DBResult==DBERR_NO_RECORDS)
	{
		m_CharIDSeed=MAKE_LW_ID(LWIT_CHAR_ID,1);
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
void CDBTransGetCharIDSeed::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetGetCharIDSeed(m_CharIDSeed);
	FUNCTION_END;
}