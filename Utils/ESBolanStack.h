/****************************************************************************/
/*                                                                          */
/*      文件名:    ESBolanStack.h		                                    */
/*      创建日期:  2010年08月26日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CESThread;

class CESBolanStack
{
protected:
	ES_BOLAN *	m_BolanBuff;
	UINT		m_BolanBuffSize;
	UINT		m_BolanCount;
	UINT		m_GrowSize;	
public:
	CESBolanStack(int InitBuffSize=64,int GrowSize=16);
	~CESBolanStack();
	void Grow();
	UINT GetSize();
	bool Push(const ES_BOLAN * pBolan);
	bool PushValue(const ES_BOLAN * pBolan);
	ES_BOLAN * Pop();	
	bool Pop(UINT Count);
	ES_BOLAN * GetTop();
	ES_BOLAN * GetAt(UINT Index);
	bool Delete(UINT Index);
	void Clear()
	{
		m_BolanCount=0;
	}


	int PushScript(LPCTSTR ExpStr,CESVariableList* pVariableList,CESFunctionList * pFunctionList,int& LastLine);
	
	
	int FindIdentifier(int Start,LPCTSTR Name);

	int FindKeyWord(int KeyWord,int StartPos,int StopKeyWord);
	int FindKeyWordReverse(int KeyWord,int StartPos,int StopKeyWord);
	int FindCoupleKeyWord(int KeyWord,int CoupleKeyWord1,int CoupleKeyWord2,int StartPos,int EndPos=-1);
	int FindCoupleKeyWordReverse(int KeyWord,int CoupleKeyWord1,int CoupleKeyWord2,int StartPos,int EndPos=-1);
	int GetPrevKeyWord(int StartPos);
	int GetNextKeyWord(int StartPos);

	int DealIdentifiers(CESThread * pESThread,int StartPos=0,int EndPos=-1,bool IsDealLocalVar=false);	
protected:	
	
	//int DealFunctionDefine(CESBolanStack * pScript,CESFunctionList * pFunctionList,int& LastLine);
	int FindKeyWord(LPCTSTR KeyWord);
	int GetFunctionParamCount(CESBolanStack * pScript,int StartPos,int& ParamCount);
	void DeleteBolan(CESBolanStack * pScript,CESFunctionList * pFunctionList,UINT Index);
	int ReverseFunctionParam(CESBolanStack * pScript,UINT StartIndex,UINT ParamCount);
	int DoVariableBind(CESThread * pESThread,int& LastLine);
};

__forceinline UINT CESBolanStack::GetSize()
{
	return m_BolanCount;
}

__forceinline bool CESBolanStack::Push(const ES_BOLAN * pBolan)
{
	if(m_BolanCount>=m_BolanBuffSize)
		Grow();
	m_BolanBuff[m_BolanCount]=*pBolan;
	m_BolanCount++;
	return true;
}

__forceinline bool CESBolanStack::PushValue(const ES_BOLAN * pBolan)
{
	if(m_BolanCount>=m_BolanBuffSize)
		Grow();
	m_BolanBuff[m_BolanCount].CopyValue(*pBolan);	
	m_BolanCount++;
	return true;
}

__forceinline ES_BOLAN *  CESBolanStack::Pop()
{
	if(m_BolanCount>0)
	{
		m_BolanCount--;
		return m_BolanBuff+(m_BolanCount);
	}
	else
		return NULL;
}

__forceinline bool CESBolanStack::Pop(UINT Count)
{
	if(Count<=m_BolanCount)
	{
		m_BolanCount-=Count;
		return true;
	}
	return false;
}

__forceinline ES_BOLAN * CESBolanStack::GetAt(UINT Index)
{
	if(Index<m_BolanCount)
	{
		return m_BolanBuff+Index;
	}
	else
		return NULL;
}

__forceinline bool CESBolanStack::Delete(UINT Index)
{
	if(Index<m_BolanCount)
	{
		for(UINT i=Index;i<GetSize()-1;i++)
		{
			m_BolanBuff[i]=m_BolanBuff[i+1];
		}
		m_BolanCount--;
		return true;
	}
	return false;
}

__forceinline ES_BOLAN * CESBolanStack::GetTop()
{
	if(m_BolanCount>0)
	{
		return m_BolanBuff+(m_BolanCount-1);
	}
	else
		return NULL;
}