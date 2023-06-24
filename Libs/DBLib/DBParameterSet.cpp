/****************************************************************************/
/*                                                                          */
/*      文件名:    DBParameterSet.cpp                                       */
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

IMPLEMENT_CLASS_INFO_STATIC(CDBParameterSet,IDBParameterSet);

CDBParameterSet::CDBParameterSet(void)
{
	m_Params.SetTag(_T("CDBParameterSet"));
}

CDBParameterSet::~CDBParameterSet(void)
{
}

void CDBParameterSet::Destory()
{
	m_Params.Clear();
}

int CDBParameterSet::GetCount()
{
	return (int)m_Params.GetCount();
}


DB_PARAM_INFO * CDBParameterSet::GetParamInfo(int Index)
{
	if((UINT)Index<m_Params.GetCount())
	{
		return &(m_Params[Index].ParamInfo);
	}
	return NULL;
}

CDBValue& CDBParameterSet::GetParam(int Index)
{
	if((UINT)Index<m_Params.GetCount())
	{
		return m_Params[Index].ParamValue;
	}
	return m_EmptyValue;
}

bool CDBParameterSet::AddParam(const DB_PARAM_INFO& ParamInfo, LPCVOID pData, size_t DataLen)
{
	PARAM_DATA* pParam = m_Params.AddEmpty();
	pParam->ParamInfo = ParamInfo;
	pParam->ParamValue.SetValue(ParamInfo.Type, pData, ParamInfo.Size, ParamInfo.DigitSize);	
	pParam->ParamInfo.Size = DataLen;
	return true;
}

bool CDBParameterSet::AddParam(const DB_PARAM_INFO& ParamInfo,const CDBValue& Value)
{
	PARAM_DATA * pParam = m_Params.AddEmpty();
	pParam->ParamInfo = ParamInfo;
	pParam->ParamValue = Value;
	return true;
}

bool CDBParameterSet::AddParam(const DB_PARAM_INFO& ParamInfo)
{
	PARAM_DATA * pParam = m_Params.AddEmpty();
	pParam->ParamInfo = ParamInfo;
	pParam->ParamValue.SetNULLValue(ParamInfo.Type);
	return true;
}

bool CDBParameterSet::SetParam(int Index,DB_PARAM_INFO& ParamInfo,const CDBValue& Value)
{
	if((UINT)Index<m_Params.GetCount())
	{
		m_Params[Index].ParamInfo=ParamInfo;
		m_Params[Index].ParamValue=Value;
		return true;
	}
	return false;
}

bool CDBParameterSet::SetParam(int Index,DB_PARAM_INFO& ParamInfo)
{
	if((UINT)Index<m_Params.GetCount())
	{
		m_Params[Index].ParamInfo=ParamInfo;
		m_Params[Index].ParamValue.SetNULLValue(ParamInfo.Type);
		return true;
	}
	return false;
}


}
