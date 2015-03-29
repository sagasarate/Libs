/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyScriptExecutor.h	                                    */
/*      创建日期:  2010年08月26日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CEasyScriptExecutor
{
public:
	CEasyScriptExecutor(void);
	~CEasyScriptExecutor(void);	

	int ExecScript(CESThread& ESThread);
	int ContinueScript(CESThread& ESThread);
	int CallFunction(CESThread& ESThread,LPCTSTR szFunctionName);
	
		

protected:
	int ExecScript(CESThread& ESThread,int StartPos,int EndPos);
	//int CallFunction(CESThread& ESThread,ES_FUNCTION * pFunction);
	int DoEva(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,CESThread& ESThread);
	int DoAdd(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoSub(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoMul(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoDiv(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoEqu(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoDiff(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoLess(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoMore(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoLessEqu(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoMoreEqu(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoNeg(ES_BOLAN * pValue,ES_BOLAN * pResult);
	int DoAnd(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoOr(ES_BOLAN * pLeftValue,ES_BOLAN * pRightValue,ES_BOLAN * pResult);
	int DoNot(ES_BOLAN * pValue,ES_BOLAN * pResult);
	int IsZero(ES_BOLAN * pValue);
};
