#include "StdAfx.h"


CDBTransGetCharData::CDBTransGetCharData(OBJECT_ID CallerID,UINT64 AccountID,UINT64 CharID)
{
	FUNCTION_BEGIN;
	m_CallerID=CallerID;
	m_CharInfo.AccountID=AccountID;
	m_CharInfo.BaseCharInfo.CharID=CharID;
	m_Result=CMRT_FAILED;
	FUNCTION_END;
}

CDBTransGetCharData::~CDBTransGetCharData(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransGetCharData::OnExecute(IDBConnection * pConnection)
{
	FUNCTION_BEGIN;
	char szSQL[1024];
	CSmartPtr<IDBRecordSet> pRecordSet;
	int DBResult;

	pRecordSet=pConnection->CreateRecordSet(DB_RS_TYPE_DYNAMIC);
	if(pRecordSet==NULL)
	{
		Log("CDBTransGetCharData::OnExecute:无法创建结果集");
		return false;
	}

	m_Result=CMRT_FAILED;

	sprintf_s(szSQL,1024,"select CharName,Race,Gender,SkinColor,HairColor,FaceType,HairType, "
		"BeardType,IsCharBald,MapID,PosX,PosY,Height,Direction "
		"from T_PLAYER_CHAR_INFO where AccountID=%lld and CharID=%lld and IsDel=0",
		m_CharInfo.AccountID,
		m_CharInfo.BaseCharInfo.CharID);


	LogDebug("CDBTransGetCharData::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,NULL);

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransGetCharData::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransGetCharData::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	DBResult=pConnection->GetResults(pRecordSet);
	if(DBResult==DBERR_SUCCEED)
	{
		m_CharInfo.BaseCharInfo.CharName=(LPCTSTR)			pRecordSet->GetField(0);
		m_CharInfo.BaseCharInfo.Race=						pRecordSet->GetField(1);
		m_CharInfo.BaseCharInfo.Gender=						pRecordSet->GetField(2);
		m_CharInfo.BaseCharInfo.SkinColor=					pRecordSet->GetField(3);
		m_CharInfo.BaseCharInfo.HairColor=					pRecordSet->GetField(4);
		m_CharInfo.BaseCharInfo.FaceType=					pRecordSet->GetField(5);
		m_CharInfo.BaseCharInfo.HairType=					pRecordSet->GetField(6);

		m_CharInfo.BaseCharInfo.BeardType=					pRecordSet->GetField(7);
		m_CharInfo.BaseCharInfo.IsCharBald=					pRecordSet->GetField(8);
		m_CharInfo.BaseCharInfo.MapID=						pRecordSet->GetField(9);
		m_CharInfo.BaseCharInfo.PosX=						pRecordSet->GetField(10);
		m_CharInfo.BaseCharInfo.PosY=						pRecordSet->GetField(11);
		m_CharInfo.BaseCharInfo.PosHeight=					pRecordSet->GetField(12);
		m_CharInfo.BaseCharInfo.Direction=					pRecordSet->GetField(13);
		m_Result=CMRT_OK;
	}
	else
	{
		Log("CDBTransGetCharData::OnExecute:获取结果集出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransGetCharData::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	return true;
	FUNCTION_END;
	return false;
}
void CDBTransGetCharData::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetGetCharData(m_CallerID,m_Result,m_CharInfo);
	FUNCTION_END;
}