/****************************************************************************/
/*                                                                          */
/*      文件名:    ESThread.cpp			                                    */
/*      创建日期:  2010年08月26日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CESThread::CESThread(void)
{
	m_pVariableList=NULL;
	m_MaxLocalVariableCount=DEFAULT_LOCAL_VARIABLE_COUNT;
	m_pCurLocalVariableList=NULL;
	m_pFunctionList=NULL;
	m_pScript=NULL;
	m_IsInInterrupt=false;
	m_InterruptPos=-1;
	m_ResultCode=0;
	m_CallStack.Create(DEFAULT_CALL_STACK_SIZE,DEFAULT_CALL_STACK_SIZE);

}

CESThread::CESThread(UINT MaxLocalVariableCount)
{
	m_pVariableList=NULL;
	m_MaxLocalVariableCount=MaxLocalVariableCount;
	m_pCurLocalVariableList=NULL;
	m_pFunctionList=NULL;
	m_pScript=NULL;
	m_IsInInterrupt=false;
	m_InterruptPos=-1;
	m_ResultCode=0;
	m_CallStack.Create(DEFAULT_CALL_STACK_SIZE,DEFAULT_CALL_STACK_SIZE);
}

CESThread::~CESThread(void)
{
}
