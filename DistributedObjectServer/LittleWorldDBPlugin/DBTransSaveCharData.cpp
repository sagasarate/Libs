#include "StdAfx.h"


CDBTransSaveCharData::CDBTransSaveCharData(OBJECT_ID CallerID,PLAYER_CHAR_INFO& CharInfo)
{
	FUNCTION_BEGIN;
	m_CallerID=CallerID;
	m_CharInfo=CharInfo;
	m_Result=CMRT_FAILED;
	FUNCTION_END;
}

CDBTransSaveCharData::~CDBTransSaveCharData(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransSaveCharData::OnExecute(IDBConnection * pConnection)
{
	FUNCTION_BEGIN;
	char szSQL[1024];
	CDBParameterSet ParameterSet;
	//DB_PARAM_INFO ParamInfo;
	int DBResult;


	m_Result=CMRT_FAILED;

	sprintf_s(szSQL,1024,"update T_PLAYER_CHAR_INFO set "
		"SkinColor=%d,HairColor=%d,FaceType=%d,HairType=%d, "
		"BeardType=%d,IsCharBald=%d,MapID=%lld,PosX=%g,PosY=%g,Height=%g,Direction=%g "
		"where AccountID=%lld and CharID=%lld",
		m_CharInfo.BaseCharInfo.SkinColor,
		m_CharInfo.BaseCharInfo.HairColor,
		m_CharInfo.BaseCharInfo.FaceType,
		m_CharInfo.BaseCharInfo.HairType,

		m_CharInfo.BaseCharInfo.BeardType,
		m_CharInfo.BaseCharInfo.IsCharBald,
		m_CharInfo.BaseCharInfo.MapID,
		m_CharInfo.BaseCharInfo.PosX,
		m_CharInfo.BaseCharInfo.PosY,
		m_CharInfo.BaseCharInfo.PosHeight,
		m_CharInfo.BaseCharInfo.Direction,

		m_CharInfo.AccountID,
		m_CharInfo.BaseCharInfo.CharID);


	LogDebug("CDBTransSaveCharData::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,NULL);

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransSaveCharData::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransSaveCharData::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}	

	if(pConnection->GetAffectedRowCount()<=0)
	{
		Log("CDBTransSaveCharData::OnExecute:执行未影响到实际数据");
	}

	m_Result=CMRT_OK;

	return true;
	FUNCTION_END;
	return false;
}
void CDBTransSaveCharData::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetSaveCharData(m_CallerID,m_Result,m_CharInfo);
	FUNCTION_END;
}