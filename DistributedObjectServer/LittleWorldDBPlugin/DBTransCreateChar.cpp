#include "StdAfx.h"


CDBTransCreateChar::CDBTransCreateChar(OBJECT_ID CallerID,CHAR_CREATE_INFO& CreateInfo)
{
	FUNCTION_BEGIN;
	m_CallerID=CallerID;
	m_CreateInfo=CreateInfo;
	m_Result=PCCRT_FAILED;
	FUNCTION_END;
}

CDBTransCreateChar::~CDBTransCreateChar(void)
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

bool CDBTransCreateChar::OnExecute(IDBConnection * pConnection)
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
		Log("CDBTransCreateChar::OnExecute:无法创建结果集");
		return false;
	}

	m_Result=PCCRT_FAILED;

	sprintf_s(szSQL,1024,"select CharName from T_PLAYER_CHAR_INFO where CharName=?");

	ParamInfo.Type=DB_TYPE_STRING;
	ParamInfo.ParamType=DB_PARAM_TYPE_INPUT;
	ParamInfo.IsNull=false;
	ParamInfo.Size=m_CreateInfo.CharName.GetLength();
	ParamInfo.DigitSize=0;
	ParameterSet.AddParam(ParamInfo,(LPCTSTR)m_CreateInfo.CharName);

	LogDebug("CDBTransCreateChar::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,&ParameterSet);

	

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransCreateChar::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransCreateChar::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}
	DBResult=pConnection->GetResults(pRecordSet);
	if(DBResult==DBERR_SUCCEED)
	{
		m_Result=PCCRT_CHAR_NAME_EXIST;
		return true;
	}
	else if(DBResult!=DBERR_NO_RECORDS)
	{
		Log("CDBTransCreateChar::OnExecute:获取结果集出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransCreateChar::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	sprintf_s(szSQL,1024,"insert into T_PLAYER_CHAR_INFO"
		"(CharID,AccountID,CharName,Race,Gender,SkinColor,HairColor,FaceType,HairType,"
		"BeardType,IsCharBald,MapID,PosX,PosY,Height,Direction) "
		"values(%lld,%lld,?,%d,%d,%d,%d,%d,%d,"
		"	%d,%d,%lld,%g,%g,%g,%g)",
		m_CreateInfo.CharID,m_CreateInfo.AccountID,m_CreateInfo.Race,m_CreateInfo.Gender,
		m_CreateInfo.SkinColor,m_CreateInfo.HairColor,m_CreateInfo.FaceType,m_CreateInfo.HairType,
		
		m_CreateInfo.BeardType,m_CreateInfo.IsCharBald,m_CreateInfo.MapID,m_CreateInfo.PosX,
		m_CreateInfo.PosY,m_CreateInfo.PosHeight,m_CreateInfo.Direction);

	LogDebug("CDBTransCreateChar::OnExecute:执行SQL:%s",szSQL);
	DBResult=pConnection->ExecuteSQL(szSQL,0,&ParameterSet);

	if(DBResult!=DBERR_SUCCEED)
	{
		Log("CDBTransCreateChar::OnExecute:执行SQL出错:(%u)%s",
			pConnection->GetLastDatabaseErrorCode(),
			pConnection->GetLastDatabaseErrorString());
		Log("CDBTransCreateChar::OnExecute:出错的SQL:%s",szSQL);
		return false;
	}

	m_PlayerCharInfo.AccountID=m_CreateInfo.AccountID;
	m_PlayerCharInfo.BaseCharInfo.CharID=m_CreateInfo.CharID;
	m_PlayerCharInfo.BaseCharInfo.CharName=m_CreateInfo.CharName;
	m_PlayerCharInfo.BaseCharInfo.Race=m_CreateInfo.Race;
	m_PlayerCharInfo.BaseCharInfo.Gender=m_CreateInfo.Gender;
	m_PlayerCharInfo.BaseCharInfo.SkinColor=m_CreateInfo.SkinColor;
	m_PlayerCharInfo.BaseCharInfo.HairColor=m_CreateInfo.HairColor;
	m_PlayerCharInfo.BaseCharInfo.FaceType=m_CreateInfo.FaceType;
	m_PlayerCharInfo.BaseCharInfo.HairType=m_CreateInfo.HairType;
	m_PlayerCharInfo.BaseCharInfo.BeardType=m_CreateInfo.BeardType;
	m_PlayerCharInfo.BaseCharInfo.IsCharBald=m_CreateInfo.IsCharBald;
	m_PlayerCharInfo.BaseCharInfo.MapID=m_CreateInfo.MapID;
	m_PlayerCharInfo.BaseCharInfo.PosX=m_CreateInfo.PosX;
	m_PlayerCharInfo.BaseCharInfo.PosY=m_CreateInfo.PosY;
	m_PlayerCharInfo.BaseCharInfo.PosHeight=m_CreateInfo.PosHeight;
	m_PlayerCharInfo.BaseCharInfo.Direction=m_CreateInfo.Direction;

	m_Result=PCCRT_OK;

	return true;
	FUNCTION_END;
	return false;
}
void CDBTransCreateChar::OnFinish()
{
	FUNCTION_BEGIN;
	GetPlayerCharDataDBAdapter()->OnDBRetCreateChar(m_CallerID,m_Result,m_PlayerCharInfo);
	FUNCTION_END;
}