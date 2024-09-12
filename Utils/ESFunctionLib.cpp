/****************************************************************************/
/*                                                                          */
/*      文件名:    ESFunctionLib.cpp	                                    */
/*      创建日期:  2010年08月27日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include <math.h>

CESFunctionLib::CESFunctionLib(void)
{
}

CESFunctionLib::~CESFunctionLib(void)
{
}

void CESFunctionLib::AddFunction(CESFunctionList * pFunctionList)
{
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,SinFN,_T("Sin"),1,pFunctionList,this);
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,CosFN,_T("Cos"),1,pFunctionList,this);
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,RoundFN,_T("Round"),1,pFunctionList,this);
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,StrCmpFN,_T("StrCmp"),1,pFunctionList,this);
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,NumToStrFN,_T("NumToStr"),1,pFunctionList,this);
	ADD_SCRIPT_CFUNCTION(CESFunctionLib,RandomFN,_T("Random"),2,pFunctionList,this);
}

int CESFunctionLib::SinFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	*pResult=sin((double)pParams[0]);
	return 0;
}

int CESFunctionLib::CosFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	*pResult=cos((double)pParams[0]);
	return 0;
}

int CESFunctionLib::RoundFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	pResult->ValueType=VALUE_TYPE_DOUBLE;
	pResult->Level=0;
	if(pParams[0].ValueDouble>0)
		pResult->ValueDouble=floor(pParams[0].ValueDouble*pow(10.0f,pParams[1].ValueInt)+0.5)/pow(10.0f,pParams[1].ValueInt);
	else
		pResult->ValueDouble=ceil(pParams[0].ValueDouble*pow(10.0f,pParams[1].ValueInt)-0.5)/pow(10.0f,pParams[1].ValueInt);
	return 0;
}

int CESFunctionLib::StrCmpFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	*pResult=_tcscmp(pParams[0].StrValue,pParams[1].StrValue);
	return 0;
}

int CESFunctionLib::NumToStrFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	pResult->ValueType=VALUE_TYPE_STRING;
	switch(pParams[0].ValueType)
	{
	case VALUE_TYPE_INT:
		pResult->StrValue.Format(_T("%d"),pParams[0].ValueInt);
		break;
	case VALUE_TYPE_INT64:
		pResult->StrValue.Format(_T("%lld"),pParams[0].ValueInt64);
		break;
	case VALUE_TYPE_FLOAT:
		pResult->StrValue.Format(_T("%g"),pParams[0].ValueFloat);
		break;
	case VALUE_TYPE_DOUBLE:
		pResult->StrValue.Format(_T("%g"),pParams[0].ValueDouble);
		break;
	}

	return 0;
}

int CESFunctionLib::RandomFN(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	*pResult=GetRand((int)pParams[0],(int)pParams[1]);
	return 0;
}
