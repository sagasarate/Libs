#include "StdAfx.h"


CDBTransGetAccountChar::CDBTransGetAccountChar(OBJECT_ID CallerID,UINT64 AccountID)
{
	FUNCTION_BEGIN;
	m_CallerID=CallerID;
	m_AccountID=AccountID;
	m_Result=CMRT_FAILED;
	FUNCTION_END;
}

CDBTransGetAccountChar::~CDBTransGetAccountChar(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransGetAccountChar::OnExecute(IDBConnection * pConnection)
{
	FUNCTION_BEGIN;
	char szSQL[1024];
	CSmartPtr<IDBRecordSet> pRecordSet;
	int DBResult;

	pRecordSet=pConnection->CreateRecordSet(DB_RS_TYPE_DYNAMIC);
	if(pRecordSet==NULL)
	{
		Log("CDBTransGetAccountChar::OnExecute:无法创建结果集");
		return false;
	}

	m_Result=CMRT_FAILED;

	sprintf_s(szSQL,1024,"select CharID,CharName,Race,Gender,SkinColor,HairColor,FaceType,HairType, "
		"BeardType,IsCharBald,MapID,PosX,PosY,Height,Direction "
		"from T_PLAYER_CHAR_INFO where AccountID=%lld and IsDel=0",m_AccountID);
	

	LogDebug("CDBTransGetAccountChar::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,NULL);

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransGetAccountChar::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransGetAccountChar::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	DBResult=pConnection->GetResults(pRecordSet);
	if(DBResult==DBERR_SUCCEED||DBResult==DBERR_NO_RECORDS)
	{
		while(!pRecordSet->IsEOF())
		{
			BASE_CHAR_INFO CharInfo;

			CharInfo.CharID=					pRecordSet->GetField(0);
			CharInfo.CharName=(LPCTSTR)			pRecordSet->GetField(1);
			CharInfo.Race=						pRecordSet->GetField(2);
			CharInfo.Gender=					pRecordSet->GetField(3);
			CharInfo.SkinColor=					pRecordSet->GetField(4);
			CharInfo.HairColor=					pRecordSet->GetField(5);
			CharInfo.FaceType=					pRecordSet->GetField(6);
			CharInfo.HairType=					pRecordSet->GetField(7);

			CharInfo.BeardType=					pRecordSet->GetField(8);
			CharInfo.IsCharBald=				pRecordSet->GetField(9);
			CharInfo.MapID=						pRecordSet->GetField(10);
			CharInfo.PosX=						pRecordSet->GetField(11);
			CharInfo.PosY=						pRecordSet->GetField(12);
			CharInfo.PosHeight=					pRecordSet->GetField(13);
			CharInfo.Direction=					pRecordSet->GetField(14);

			m_CharList.Add(CharInfo);

			pRecordSet->MoveNext();
		}
		m_Result=CMRT_OK;
	}
	else
	{
		Log("CDBTransGetAccountChar::OnExecute:获取结果集出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransGetAccountChar::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	return true;
	FUNCTION_END;
	return false;
}
void CDBTransGetAccountChar::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetGetAccountChar(m_CallerID,m_Result,m_AccountID,m_CharList);
	FUNCTION_END;
}