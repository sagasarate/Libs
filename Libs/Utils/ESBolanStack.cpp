/****************************************************************************/
/*                                                                          */
/*      文件名:    ESBolanStack.cpp		                                    */
/*      创建日期:  2010年08月26日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


CESBolanStack::CESBolanStack(int InitBuffSize,int GrowSize)
{
	m_GrowSize=GrowSize;

	m_BolanBuff=new ES_BOLAN[InitBuffSize];
	m_BolanBuffSize=InitBuffSize;
	m_BolanCount=0;
}

CESBolanStack::~CESBolanStack()
{
	if(m_BolanBuff)
	{
		delete[] m_BolanBuff;
		m_BolanBuff=0;
		m_BolanBuffSize=0;
		m_BolanCount=0;
	}
}

void CESBolanStack::Grow()
{
	ES_BOLAN * NewBuff=new ES_BOLAN[m_BolanBuffSize+m_GrowSize];
	for(UINT i=0;i<m_BolanBuffSize;i++)
		NewBuff[i]=m_BolanBuff[i];
	//memcpy(NewBuff,m_BolanBuff,sizeof(CBolan)*m_BolanBuffSize);
	delete[] m_BolanBuff;
	m_BolanBuff=NewBuff;
	m_BolanBuffSize+=m_GrowSize;
}



int  CESBolanStack::PushScript(LPCTSTR ExpStr,CESVariableList* pVariableList,CESFunctionList * pFunctionList,int& LastLine)
{
	TCHAR TempID[MAX_IDENTIFIER_LENGTH];

	CESBolanStack TempStack;
	CESBolanStack OrginScript;
	ES_BOLAN Bolan;

	bool IsInStr;
	LastLine=1;

	CEasyBuffer Buffer(((UINT)_tcslen(ExpStr)+1)*sizeof(TCHAR));
	CEasyBuffer TempBuffer(((UINT)_tcslen(ExpStr)+1)*sizeof(TCHAR));

	TCHAR * ExpBuff=(TCHAR *)Buffer.GetBuffer();
	TCHAR * TempBuff=(TCHAR *)TempBuffer.GetBuffer();


	UINT Len=0;
	IsInStr=false;
	while(*ExpStr)
	{
		if(*ExpStr=='"') IsInStr=!IsInStr;

		if(IsInStr)
			ExpBuff[Len++]=(*ExpStr++);
		else
			ExpBuff[Len++]=toupper(*ExpStr++);

	}
	ExpBuff[Len]=0;
	//CharUpper(ExpBuff);
	ExpStr=ExpBuff;

	//先提取原始布兰式
	while(*ExpStr)
	{
		Bolan.Clear();
		Bolan.Line=LastLine;
		if(*ExpStr==' '||*ExpStr=='\r'||*ExpStr=='\n'||*ExpStr=='	')
		{
			//空格、空行忽略
			if(*ExpStr=='\n')
				LastLine++;
			ExpStr++;
		}
		else if(*ExpStr=='/'&&*(ExpStr+1)=='/')
		{
			while((*ExpStr!='\r')&&(*ExpStr!='\n')&&(*ExpStr))
			{
				if(*ExpStr=='\n')
					LastLine++;
				ExpStr++;
			}
		}
		else if(*ExpStr=='"')//字符串
		{
			UINT i=0;
			ExpStr++;
			while(*ExpStr!='"')
			{
				//if(i>MAX_STRING_LENGTH)
				//	return 1001;
				if(*ExpStr==0)
					return 1002;
				TempBuff[i++]=*ExpStr++;
				if(*ExpStr=='"'&&*(ExpStr+1)=='"')
				{
					TempBuff[i++]=*ExpStr++;
					ExpStr++;
				}
			}
			TempBuff[i]=0;
			ExpStr++;

			Bolan.Type=BOLAN_TYPE_VALUE;
			Bolan.ValueType=VALUE_TYPE_STRING;
			Bolan.StrValue=TempBuff;
			Bolan.Level=0;
			OrginScript.Push(&Bolan);
		}
		else if((*ExpStr>='0'&&*ExpStr<='9')||*ExpStr=='.')	//数字
		{
			UINT i=0;
			while((*ExpStr>='0'&&*ExpStr<='9')||*ExpStr=='.'||*ExpStr=='F'||*ExpStr=='D'||*ExpStr=='I'||*ExpStr=='L'||*ExpStr=='E'||
				*ExpStr=='A'||*ExpStr=='B'||*ExpStr=='C'||*ExpStr=='X')
			{
				TempBuff[i++]=*ExpStr++;
			}
			TempBuff[i]=0;
			Bolan.Type=BOLAN_TYPE_VALUE;
			Bolan.Level=0;
			StrToNumber(TempBuff,Bolan);
			OrginScript.Push(&Bolan);
		}
		else if(*ExpStr==':')		//跳转标识
		{
			UINT i=0;
			ExpStr++;
			while(CanMakeIdentifier(*ExpStr))
			{
				if(i>MAX_IDENTIFIER_LENGTH)
					return 1003;
				TempID[i++]=*ExpStr++;
			}
			TempID[i]=0;
			Bolan.Type=BOLAN_TYPE_IDENTIFIER;
			Bolan.Index=IDENTIFIER_TYPE_JUMP_DEFINE;
			Bolan.Level=0;
			Bolan.StrValue=TempID;
			OrginScript.Push(&Bolan);
		}
		else if(*ExpStr==';'||*ExpStr=='\r'||*ExpStr=='\n')//行结束符
		{
			Bolan.Type=BOLAN_TYPE_KEYWORD;
			Bolan.Level=0;
			Bolan.Index=KW_LINEEND;
			OrginScript.Push(&Bolan);
			//除去多余的行结束符
			while(*ExpStr==' '||*ExpStr==';'||*ExpStr=='\r'||*ExpStr=='\n')
			{
				if(*ExpStr=='\n')
					LastLine++;
				ExpStr++;
			}
		}
		else if(*ExpStr>='A'&&*ExpStr<='Z')//标识符
		{
			UINT i=0;
			while(CanMakeIdentifier(*ExpStr))
			{
				if(i>MAX_IDENTIFIER_LENGTH)
					return 1003;
				TempBuff[i++]=*ExpStr++;
			}
			TempBuff[i]=0;
			if(_tcsicmp(TempBuff,_T("AND"))==0)
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=15;
				Bolan.Index=OPERATOR_AND;
				OrginScript.Push(&Bolan);
			}
			else if(_tcsicmp(TempBuff,_T("OR"))==0)
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=15;
				Bolan.Index=OPERATOR_OR;
				OrginScript.Push(&Bolan);
			}
			else if(_tcsicmp(TempBuff,_T("NOT"))==0)
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=16;
				Bolan.Index=OPERATOR_NOT;
				OrginScript.Push(&Bolan);
			}
			else
			{
				int KeyWord=FindKeyWord(TempBuff);
				if(KeyWord>=0)//关键字
				{
					Bolan.Type=BOLAN_TYPE_KEYWORD;
					Bolan.Level=0;
					Bolan.Index=KeyWord;
					OrginScript.Push(&Bolan);

				}
				else
				{
					Bolan.Type=BOLAN_TYPE_IDENTIFIER;
					Bolan.Index=IDENTIFIER_TYPE_UNKNOW;
					Bolan.Level=0;
					Bolan.StrValue=TempBuff;
					OrginScript.Push(&Bolan);
				}
			}
		}
		else if(*ExpStr=='(')
		{
			Bolan.Type=BOLAN_TYPE_OPERATOR;
			Bolan.Level=0;
			Bolan.Index=OPERATOR_LP;
			OrginScript.Push(&Bolan);
			ExpStr++;
		}
		else if(*ExpStr==')')
		{
			Bolan.Type=BOLAN_TYPE_OPERATOR;
			Bolan.Level=0;
			Bolan.Index=OPERATOR_RP;
			OrginScript.Push(&Bolan);
			ExpStr++;
		}
		else if(*ExpStr=='=')
		{
			if(*(ExpStr+1)=='=')
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_EQU;
				OrginScript.Push(&Bolan);
				ExpStr+=2;
			}
			else
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=10;
				Bolan.Index=OPERATOR_EVA;
				OrginScript.Push(&Bolan);
				ExpStr++;
			}
		}
		else if(*ExpStr=='>')
		{
			if(*(ExpStr+1)=='=')
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_MORE_EQU;
				OrginScript.Push(&Bolan);
				ExpStr+=2;
			}
			else
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_MORE;
				OrginScript.Push(&Bolan);
				ExpStr++;
			}
		}
		else if(*ExpStr=='<')
		{
			if(*(ExpStr+1)=='=')
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_LESS_EQU;
				OrginScript.Push(&Bolan);
				ExpStr+=2;
			}
			else if(*(ExpStr+1)=='>')
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_DIFF;
				OrginScript.Push(&Bolan);
				ExpStr+=2;
			}
			else
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=20;
				Bolan.Index=OPERATOR_LESS;
				OrginScript.Push(&Bolan);
				ExpStr++;
			}

		}
		else if(*ExpStr=='+'||*ExpStr=='-')
		{
			//if(*ExpStr=='-'&&(ExpStr==ExpBuff||*(ExpStr-1)=='('))
			//{
			//	Bolan.Type=BOLAN_TYPE_OPERATOR;
			//	Bolan.Index=OPERATOR_NEG;
			//	Bolan.Level=50;
			//	OrginScript.Push(&Bolan);
			//	ExpStr++;
			//}
			//else
			{
				Bolan.Type=BOLAN_TYPE_OPERATOR;
				Bolan.Level=30;
				if(*ExpStr=='+') Bolan.Index=OPERATOR_ADD;
				else Bolan.Index=OPERATOR_SUB;
				OrginScript.Push(&Bolan);
				ExpStr++;
			}
		}
		else if(*ExpStr=='*'||*ExpStr=='/')
		{
			Bolan.Type=BOLAN_TYPE_OPERATOR;
			Bolan.Level=40;
			if(*ExpStr=='*') Bolan.Index=OPERATOR_MUL;
			else Bolan.Index=OPERATOR_DIV;
			OrginScript.Push(&Bolan);
			ExpStr++;
		}
		else if(*ExpStr==',')
		{
			Bolan.Type=BOLAN_TYPE_OPERATOR;
			Bolan.Index=OPERATOR_COMMA;
			Bolan.Level=0;
			OrginScript.Push(&Bolan);
			ExpStr++;
		}
		else
		{
			return 1005;				//非法字符
		}
	}

	CESThread ESThread;

	ESThread.SetScript(&OrginScript);
	ESThread.SetVariableList(pVariableList);
	ESThread.SetFunctionList(pFunctionList);

	int RetCode=DealIdentifiers(&ESThread);
	LastLine=ESThread.GetLastLine();
	if(RetCode)
		return RetCode;




	for(UINT i=0;i<OrginScript.GetSize();i++)
	{
		ES_BOLAN * pBolan=OrginScript.GetAt(i);
		LastLine=pBolan->Line;
		switch(pBolan->Type)
		{
		case BOLAN_TYPE_VALUE:
			Push(pBolan);
			break;
		case BOLAN_TYPE_OPERATOR:
			if(pBolan->Index==OPERATOR_RP)
			{
				while(TempStack.GetTop()!=NULL&&(TempStack.GetTop()->Type!=BOLAN_TYPE_OPERATOR||TempStack.GetTop()->Index!=OPERATOR_LP))
				{
					ES_BOLAN * pTemp=TempStack.Pop();
					Push(pTemp);
				}
				if(TempStack.GetTop()==NULL||TempStack.GetTop()->Type!=BOLAN_TYPE_OPERATOR||TempStack.GetTop()->Index!=OPERATOR_LP)
					return 2021;
				TempStack.Pop();
			}
			else if(pBolan->Index==OPERATOR_LP)
			{
				TempStack.Push(pBolan);
			}
			else if(pBolan->Index==OPERATOR_NOP)
			{

			}
			else if(pBolan->Index==OPERATOR_COMMA)
			{
				while(TempStack.GetTop()!=NULL&&(TempStack.GetTop()->Type!=BOLAN_TYPE_OPERATOR||TempStack.GetTop()->Index!=OPERATOR_LP))
				{
					ES_BOLAN * pTemp=TempStack.Pop();
					Push(pTemp);
				}
			}
			else
			{
				while(TempStack.GetTop()!=NULL&&TempStack.GetTop()->Level>=pBolan->Level)
				{
					ES_BOLAN * pTemp=TempStack.Pop();
					Push(pTemp);
				}
				TempStack.Push(pBolan);
			}
			break;
		case BOLAN_TYPE_FUNCTION:
			while(TempStack.GetTop()!=NULL&&TempStack.GetTop()->Level>=pBolan->Level)
			{
				ES_BOLAN * pTemp=TempStack.Pop();
				Push(pTemp);
			}
			TempStack.Push(pBolan);
			break;
		case BOLAN_TYPE_VARIABLE:
			Push(pBolan);
			break;
		case BOLAN_TYPE_KEYWORD:
			while(TempStack.GetTop()!=NULL)
			{
				ES_BOLAN * pTemp=TempStack.Pop();
				Push(pTemp);
			}
			switch(pBolan->Index)
			{
			case KW_EXIT:
			case KW_IF:
			case KW_THEN:
			case KW_ELSE:
			case KW_ELSEIF:
			case KW_ENDIF:
			case KW_WHILE:
			case KW_DO:
			case KW_ENDWHILE:
			case KW_BREAK:
			case KW_CONTINUE:
			case KW_GOTO:
				Push(pBolan);
				break;
			case KW_LINEEND:
			case KW_INT:
			case KW_INT64:
			case KW_FLOAT:
			case KW_DOUBLE:
			case KW_STRING:
				break;
			case KW_FUNCTION:
			case KW_ENDFUN:
				Push(pBolan);
				break;
			};
			break;
		case BOLAN_TYPE_IDENTIFIER:
			while(TempStack.GetTop()!=NULL)
			{
				ES_BOLAN * pTemp=TempStack.Pop();
				Push(pTemp);
			}
			if(pBolan->Index!=IDENTIFIER_TYPE_VARIABLE_DEFINE)
				Push(pBolan);
			break;
		}
	}
	while(TempStack.GetTop()!=NULL)
	{
		ES_BOLAN * pTemp=TempStack.Pop();
		if(pTemp->Type==BOLAN_TYPE_OPERATOR&&pTemp->Index==OPERATOR_LP)
			return 2021;
		Push(pTemp);
	}

	//设置各种控制语句
	ES_FUNCTION * pCurFunction=NULL;

	for(UINT i=0;i<GetSize();i++)
	{
		ES_BOLAN * pBolan=GetAt(i);
		LastLine=pBolan->Line;
		if(pBolan->Type==BOLAN_TYPE_FUNCTION)
		{
			pBolan->Type=BOLAN_TYPE_OPERATOR;
			pBolan->Level=pBolan->Index;
			pBolan->Index=OPERATOR_CALL;
		}
		else if(pBolan->Type==BOLAN_TYPE_KEYWORD)
		{
			switch(pBolan->Index)
			{
			case KW_EXIT:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_EXIT;
				break;
			case KW_IF:
			case KW_ELSEIF:
				{
					int EndPos=FindCoupleKeyWord(KW_ENDIF,KW_IF,KW_ENDIF,i+1);
					if(EndPos<0)
						return 2001;
					if(pBolan->Index==KW_ELSEIF)
					{
						pBolan->Type=BOLAN_TYPE_OPERATOR;
						pBolan->Index=OPERATOR_JMP;
						pBolan->Level=EndPos+1;
					}
					int Pos=GetNextKeyWord(i+1);
					if(Pos<0)
						return 2001;
					ES_BOLAN * pThenBolan=GetAt(Pos);
					if(pThenBolan->Type!=BOLAN_TYPE_KEYWORD&&pThenBolan->Index!=KW_THEN)
						return 2001;
					int ElseIfPos=FindCoupleKeyWord(KW_ELSEIF,KW_IF,KW_ENDIF,i+1,EndPos+1);
					int ElsePos=FindCoupleKeyWord(KW_ELSE,KW_IF,KW_ENDIF,i+1,EndPos+1);
					pThenBolan->Type=BOLAN_TYPE_OPERATOR;
					pThenBolan->Index=OPERATOR_JZ;
					if(ElseIfPos>=0)
						pThenBolan->Level=ElseIfPos+1;
					else if(ElsePos>=0)
						pThenBolan->Level=ElsePos+1;
					else
						pThenBolan->Level=EndPos+1;
				}
				break;
			case KW_ELSE:
				{
					int EndPos=FindCoupleKeyWord(KW_ENDIF,KW_IF,KW_ENDIF,i+1);
					if(EndPos<0)
						return 2001;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_JMP;
					pBolan->Level=EndPos+1;
				}
				break;
			case KW_WHILE:
				{
					int Pos=GetNextKeyWord(i+1);
					if(Pos<0)
						return 2011;
					ES_BOLAN * pDoBolan=GetAt(Pos);
					if(pDoBolan->Type!=BOLAN_TYPE_KEYWORD||pDoBolan->Index!=KW_DO)
						return 2011;
					Pos=FindCoupleKeyWord(KW_ENDWHILE,KW_WHILE,KW_ENDWHILE,i+1);
					if(Pos<0)
						return 2011;
					ES_BOLAN * pEndBolan=GetAt(Pos);
					pDoBolan->Type=BOLAN_TYPE_OPERATOR;
					pDoBolan->Index=OPERATOR_JZ;
					pDoBolan->Level=Pos+1;
					pEndBolan->Type=BOLAN_TYPE_OPERATOR;
					pEndBolan->Index=OPERATOR_JMP;
					pEndBolan->Level=i+1;
				}
				break;
			case KW_ENDWHILE:
				{
					return 2011;
					//int Pos=FindCoupleKeyWordReverse(KW_WHILE,KW_WHILE,KW_ENDWHILE,i-1);
					//if(Pos<0)
					//	return 2011;
					//pBolan->Type=BOLAN_TYPE_OPERATOR;
					//pBolan->Index=OPERATOR_JMP;
					//pBolan->Level=Pos+1;
				}
				break;
			case KW_BREAK:
				{
					int Pos=FindCoupleKeyWord(KW_ENDWHILE,KW_WHILE,KW_ENDWHILE,i);
					if(Pos<0)
						return 2012;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_JMP;
					pBolan->Level=Pos+1;
				}
				break;
			case KW_CONTINUE:
				{
					int Pos=FindCoupleKeyWordReverse(KW_WHILE,KW_WHILE,KW_ENDWHILE,i);
					if(Pos<0)
						return 2013;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_JMP;
					pBolan->Level=Pos+1;
				}
				break;
			case KW_GOTO:
				{
					ES_BOLAN *pIDBolan=GetAt(i+1);
					if(pIDBolan==NULL)
						return 2014;
					if(pIDBolan->Type!=BOLAN_TYPE_IDENTIFIER)
						return 2014;
					int Pos=FindIdentifier(0,pIDBolan->StrValue);
					if(Pos<0)
						return 2015;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_JMP;
					pBolan->Level=Pos+1;
				}
				break;
			case KW_FUNCTION:
				{
					int Pos=FindKeyWord(KW_ENDFUN,i,-1);
					if(Pos<0)
						return 2016;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_JMP_FUNC;
					pBolan->Level=Pos+1;


				}
				break;
			case KW_ENDFUN:
				{
					if(pCurFunction==NULL)
						return 2019;
					pCurFunction->FunEndPos=i+1;
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_RET;
				}
				break;
			}
		}
		else if(pBolan->Type==BOLAN_TYPE_IDENTIFIER&&pBolan->Index==IDENTIFIER_TYPE_FUNCTION_DEFINE)
		{
			pCurFunction=pFunctionList->FindFunction(pBolan->StrValue);
			if(pCurFunction==NULL)
			{
				return 2019;
			}
			if(pCurFunction->Type!=FUNCTION_TYPE_SCRIPT)
			{
				return 2019;
			}
			pCurFunction->FunStartPos=i;

			//反转函数参数顺序
			RetCode=ReverseFunctionParam(this,i+1,pCurFunction->ParaCount);
			if(RetCode)
				return RetCode;
		}
	}

	//删除多余的布兰式
	for(int i=(int)GetSize()-1;i>=0;i--)
	{
		ES_BOLAN * pBolan=GetAt(i);
		if(pBolan->Type!=BOLAN_TYPE_VALUE&&
			pBolan->Type!=BOLAN_TYPE_OPERATOR&&
			pBolan->Type!=BOLAN_TYPE_VARIABLE&&(pBolan->Type!=BOLAN_TYPE_KEYWORD||pBolan->Index!=KW_FUNCTION))
		{
			DeleteBolan(this,pFunctionList,i);
		}
	}

	//进行变量绑定

	ESThread.SetScript(this);
	RetCode=DoVariableBind(&ESThread,LastLine);
	if(RetCode)
		return RetCode;

	return 0;

}

int CESBolanStack::FindIdentifier(int Start,LPCTSTR Name)
{
	for(int i=Start;i<(int)GetSize();i++)
	{
		if(GetAt(i)->Type==BOLAN_TYPE_IDENTIFIER)
		{
			if(i>0)
				if(GetAt(i-1)->Type==BOLAN_TYPE_KEYWORD&&GetAt(i-1)->Index==KW_GOTO)
					continue;
			if(GetAt(i)->StrValue.CompareNoCase(Name)==0)
				return i;
		}
	}
	return -1;
}
int CESBolanStack::FindKeyWord(int KeyWord,int StartPos,int StopKeyWord)
{
	for(int i=StartPos;i<(int)GetSize();i++)
	{
		if(StopKeyWord>0&&GetAt(i)->Type==BOLAN_TYPE_KEYWORD&&GetAt(i)->Index==(UINT)StopKeyWord)
			return -1;
		if (GetAt(i)->Type == BOLAN_TYPE_KEYWORD&&GetAt(i)->Index == (UINT)KeyWord)
			return i;
	}
	return -1;
}

int CESBolanStack::FindKeyWordReverse(int KeyWord,int StartPos,int StopKeyWord)
{
	if(StartPos>=(int)GetSize())
		return -1;
	for(;StartPos>=0;StartPos--)
	{
		if(GetAt(StartPos)->Type==BOLAN_TYPE_KEYWORD)
		{
			if (StopKeyWord>0 && GetAt(StartPos)->Index == (UINT)StopKeyWord)
				return -1;
			if (GetAt(StartPos)->Index == (UINT)KeyWord)
				return StartPos;
		}
	}
	return -1;
}
int CESBolanStack::FindCoupleKeyWord(int KeyWord,int CoupleKeyWord1,int CoupleKeyWord2,int StartPos,int EndPos)
{
	int CoupleCount=0;
	if(EndPos<0)
		EndPos=GetSize();
	for(int i=StartPos;i<EndPos;i++)
	{
		if(GetAt(i)->Type==BOLAN_TYPE_KEYWORD)
		{
			if (GetAt(i)->Index == (UINT)KeyWord&&CoupleCount == 0)
				return i;
			if (GetAt(i)->Index == (UINT)CoupleKeyWord1)
				CoupleCount++;
			if (GetAt(i)->Index == (UINT)CoupleKeyWord2)
				CoupleCount--;
		}
	}
	return -1;
}

int CESBolanStack::FindCoupleKeyWordReverse(int KeyWord,int CoupleKeyWord1,int CoupleKeyWord2,int StartPos,int EndPos)
{
	int CoupleCount=0;
	if(EndPos<0)
		EndPos=-1;
	for(int i=StartPos;i>EndPos;i--)
	{
		if(GetAt(i)->Type==BOLAN_TYPE_KEYWORD)
		{
			if (GetAt(i)->Index == (UINT)KeyWord&&CoupleCount == 0)
				return i;
			if (GetAt(i)->Index == (UINT)CoupleKeyWord2)
				CoupleCount++;
			if (GetAt(i)->Index == (UINT)CoupleKeyWord1)
				CoupleCount--;
		}
	}
	return -1;
}



int CESBolanStack::GetPrevKeyWord(int StartPos)
{
	for(int i=StartPos;i>=0;i--)
	{
		if(GetAt(i)->Type==BOLAN_TYPE_KEYWORD)
			return i;
	}
	return -1;
}
int CESBolanStack::GetNextKeyWord(int StartPos)
{
	for(int i=StartPos;i<(int)GetSize();i++)
	{
		if(GetAt(i)->Type==BOLAN_TYPE_KEYWORD)
			return i;
	}
	return -1;
}



int CESBolanStack::DealIdentifiers(CESThread * pESThread,int StartPos,int EndPos,bool IsDealLocalVar)
{
	ES_VARIABLE * pVar;
	int FunctionLayer=0;

	if(IsDealLocalVar)
		FunctionLayer++;

	int CurPos=StartPos;

	while(CurPos<(int)pESThread->GetScript()->GetSize())
	{
		ES_BOLAN * pBolan=pESThread->GetScript()->GetAt(CurPos);
		pESThread->SetLastLine(pBolan->Line);
		switch(pBolan->Type)
		{
		case BOLAN_TYPE_IDENTIFIER:
			if(pBolan->Index==IDENTIFIER_TYPE_UNKNOW&&(!IsDealLocalVar))
			{
				pVar=pESThread->FindVariable(pBolan->StrValue);
				if(pVar)
				{
					pBolan->Type=BOLAN_TYPE_VARIABLE;
					pBolan->ValueType=pVar->Type;
				}
				else
				{
					ES_FUNCTION * pFun=pESThread->GetFunctionList()->FindFunction(pBolan->StrValue);
					if(pFun)
					{
						pBolan->Type=BOLAN_TYPE_FUNCTION;
						pBolan->Level=60;
						pBolan->Index=pFun->ID;
					}
					else
					{
						//暂时不返回错误，等下回再出理
						//return 4012;
					}
				}
			}
			CurPos++;
			break;
		case BOLAN_TYPE_OPERATOR:
			//取负值操作符判定
			if(pBolan->Index==OPERATOR_SUB&&(!IsDealLocalVar))
			{
				int Pos=CurPos-1;
				int IsInBrackets=0;
				int IsNeg=0;
				while(pESThread->GetScript()->GetAt(Pos)&&IsNeg==0)
				{
					switch(pESThread->GetScript()->GetAt(Pos)->Type)
					{
					case BOLAN_TYPE_OPERATOR:
						switch(pESThread->GetScript()->GetAt(Pos)->Index)
						{
						case OPERATOR_LP:
							if(IsInBrackets)
							{
								IsInBrackets--;
							}
							else
							{
								IsNeg=1;
								break;
							}
						case OPERATOR_RP:
							IsInBrackets++;
							break;
						default:
							IsNeg=1;
							break;
						}
						break;
					case BOLAN_TYPE_VALUE:
					case BOLAN_TYPE_FUNCTION:
					case BOLAN_TYPE_VARIABLE:
						IsNeg=2;
						break;
					case BOLAN_TYPE_KEYWORD:
					case BOLAN_TYPE_IDENTIFIER:
						IsNeg=1;
						break;
					}
					Pos--;
				}
				if(IsNeg!=2)
				{
					pBolan->Index=OPERATOR_NEG;
					pBolan->Level=50;
				}
			}
			CurPos++;
			break;
		case BOLAN_TYPE_KEYWORD:
			switch((int)(pBolan->Index))
			{
			case KW_INT:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_NOP;
				CurPos++;
				pBolan=pESThread->GetScript()->GetAt(CurPos);
				if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
					return 4010;
				if(FunctionLayer)
				{
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_ADD_VAR;
					pBolan->Level=5;
					pBolan->ValueType=VALUE_TYPE_INT;
					pESThread->AddLocalVariable(pBolan->StrValue,(int)0);
				}
				else
				{
					pBolan->Index=IDENTIFIER_TYPE_VARIABLE_DEFINE;
					pESThread->GetVariableList()->AddVariable(pBolan->StrValue,(int)0);
				}

				CurPos++;
				break;
			case KW_INT64:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_NOP;
				CurPos++;
				pBolan=pESThread->GetScript()->GetAt(CurPos);
				if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
					return 4010;
				if(FunctionLayer)
				{
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_ADD_VAR;
					pBolan->Level=5;
					pBolan->ValueType=VALUE_TYPE_INT64;
					pESThread->AddLocalVariable(pBolan->StrValue,(INT64)0);
				}
				else
				{
					pBolan->Index=IDENTIFIER_TYPE_VARIABLE_DEFINE;
					pESThread->GetVariableList()->AddVariable(pBolan->StrValue,(INT64)0);
				}
				CurPos++;
				break;
			case KW_FLOAT:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_NOP;
				CurPos++;
				pBolan=pESThread->GetScript()->GetAt(CurPos);
				if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
					return 4010;
				if(FunctionLayer)
				{
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_ADD_VAR;
					pBolan->Level=5;
					pBolan->ValueType=VALUE_TYPE_FLOAT;
					pESThread->AddLocalVariable(pBolan->StrValue,(float)0);
				}
				else
				{
					pBolan->Index=IDENTIFIER_TYPE_VARIABLE_DEFINE;
					pESThread->GetVariableList()->AddVariable(pBolan->StrValue,(float)0);
				}
				CurPos++;
				break;
			case KW_DOUBLE:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_NOP;
				CurPos++;
				pBolan=pESThread->GetScript()->GetAt(CurPos);
				if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
					return 4010;
				if(FunctionLayer)
				{
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_ADD_VAR;
					pBolan->Level=5;
					pBolan->ValueType=VALUE_TYPE_DOUBLE;
					pESThread->AddLocalVariable(pBolan->StrValue,(double)0);
				}
				else
				{
					pBolan->Index=IDENTIFIER_TYPE_VARIABLE_DEFINE;
					pESThread->GetVariableList()->AddVariable(pBolan->StrValue,(double)0);
				}

				CurPos++;
				break;
			case KW_STRING:
				pBolan->Type=BOLAN_TYPE_OPERATOR;
				pBolan->Index=OPERATOR_NOP;
				CurPos++;
				pBolan=pESThread->GetScript()->GetAt(CurPos);
				if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
					return 4010;
				if(FunctionLayer)
				{
					pBolan->Type=BOLAN_TYPE_OPERATOR;
					pBolan->Index=OPERATOR_ADD_VAR;
					pBolan->Level=5;
					pBolan->ValueType=VALUE_TYPE_STRING;
					pESThread->AddLocalVariable(pBolan->StrValue,_T(""));
				}
				else
				{
					pBolan->Index=IDENTIFIER_TYPE_VARIABLE_DEFINE;
					pESThread->GetVariableList()->AddVariable(pBolan->StrValue,_T(""));
				}
				CurPos++;
				break;
			case KW_FUNCTION:
				if(!IsDealLocalVar)
				{
					if(FunctionLayer)
						return 2018;
					FunctionLayer++;
					CurPos++;
					pBolan=pESThread->GetScript()->GetAt(CurPos);
					if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
						return 2019;
					int ParaCount=0;
					int Ret=GetFunctionParamCount(pESThread->GetScript(),CurPos+1,ParaCount);
					if(Ret)
						return Ret;
					pBolan->Index=IDENTIFIER_TYPE_FUNCTION_DEFINE;
					int EndFunPos=pESThread->GetScript()->FindKeyWord(KW_ENDFUN,CurPos,-1);
					if(EndFunPos<0)
					{
						return 2017;
					}
					pESThread->GetFunctionList()->AddFunction(pBolan->StrValue,ParaCount,0,0);
					pESThread->NewLocalVariableList();
				}
				CurPos++;
				break;
			case KW_GOTO:
				if(!IsDealLocalVar)
				{
					CurPos++;
					pBolan=pESThread->GetScript()->GetAt(CurPos);
					if(pBolan->Type!=BOLAN_TYPE_IDENTIFIER)
						return 2014;
					pBolan->Index=IDENTIFIER_TYPE_JUMP;
				}
				CurPos++;
				break;
			case KW_ENDFUN:
				if(!IsDealLocalVar)
				{
					if(FunctionLayer==0)
						return 2016;
					pESThread->ReleaseLocalVariableList();
					FunctionLayer--;
				}
				CurPos++;
				break;
			default:
				CurPos++;
			}
			break;
		default:
			CurPos++;
		}

		if(EndPos>=0)
		{
			if(CurPos>EndPos)
				break;
		}
	}

	CurPos=StartPos;

	while(CurPos<(int)pESThread->GetScript()->GetSize())
	{
		ES_BOLAN * pBolan=pESThread->GetScript()->GetAt(CurPos);
		pESThread->SetLastLine(pBolan->Line);
		switch(pBolan->Type)
		{
		case BOLAN_TYPE_IDENTIFIER:
			if(pBolan->Index==IDENTIFIER_TYPE_UNKNOW&&(!IsDealLocalVar))
			{
				pVar=pESThread->FindVariable(pBolan->StrValue);
				if(pVar)
				{
					pBolan->Type=BOLAN_TYPE_VARIABLE;
					pBolan->ValueType=pVar->Type;
				}
				else
				{
					ES_FUNCTION * pFun=pESThread->GetFunctionList()->FindFunction(pBolan->StrValue);
					if(pFun)
					{
						pBolan->Type=BOLAN_TYPE_FUNCTION;
						pBolan->Level=60;
						pBolan->Index=pFun->ID;
					}
					else
					{
						return 4012;
					}
				}
			}
			CurPos++;
			break;
		default:
			CurPos++;
		}

		if(EndPos>=0)
		{
			if(CurPos>EndPos)
				break;
		}
	}
	return 0;
}

//int CESBolanStack::DealFunctionDefine(CESBolanStack * pScript,CESFunctionList * pFunctionList,int& LastLine)
//{
//	int StartPos=0;
//	ES_FUNCTION * pFun;
//	int FunctionLayer=0;
//
//	while(StartPos<pScript->GetSize())
//	{
//		ES_BOLAN * pBolan=GetAt(StartPos);
//		LastLine=pBolan->Line;
//		switch(pBolan->Type)
//		{
//		case BOLAN_TYPE_KEYWORD:
//			switch((int)(pBolan->Index))
//			{
//			case KW_FUNCTION:
//				StartPos++;
//
//
//				break;
//			case KW_ENDFUN:
//
//				break;
//			default:
//				StartPos++;
//			}
//			break;
//		default:
//			StartPos++;
//		}
//	}
//	if(FunctionLayer)
//		return 2017;
//	return 0;
//}

int CESBolanStack::FindKeyWord(LPCTSTR KeyWord)
{
	for(int i=0;i<KW_MAX;i++)
	{
		if(_tcsicmp(KeyWord,KEYWORD_STRINGS[i])==0)
			return i;
	}
	return -1;
}
int CESBolanStack::GetFunctionParamCount(CESBolanStack * pScript,int StartPos,int& ParamCount)
{
	ParamCount=0;
	ES_BOLAN * pBolan=pScript->GetAt(StartPos);
	if(pBolan==NULL)
		return 2019;
	if(pBolan->Type==BOLAN_TYPE_OPERATOR&&pBolan->Index==OPERATOR_LP)
	{
		StartPos++;
		pBolan=pScript->GetAt(StartPos);
		while(pBolan)
		{
			switch(pBolan->Type)
			{
			case BOLAN_TYPE_VALUE:
				return 2019;
			case BOLAN_TYPE_OPERATOR:
				if(pBolan->Index==OPERATOR_RP)
					return 0;
				if(pBolan->Index!=OPERATOR_COMMA)
					return 2019;
				break;
			case BOLAN_TYPE_FUNCTION:
			case BOLAN_TYPE_VARIABLE:
				return 2019;
			case BOLAN_TYPE_KEYWORD:
				if(pBolan->Index!=KW_INT&&pBolan->Index!=KW_INT64&&pBolan->Index!=KW_FLOAT&&
					pBolan->Index!=KW_DOUBLE&&pBolan->Index!=KW_STRING)
				{
					return 2019;
				}
				break;
			case BOLAN_TYPE_IDENTIFIER:
				ParamCount++;
				break;
			}
			StartPos++;
			pBolan=pScript->GetAt(StartPos);
		}
	}
	return 2019;
}

void CESBolanStack::DeleteBolan(CESBolanStack * pScript,CESFunctionList * pFunctionList,UINT Index)
{
	if(Delete(Index))
	{
		for(UINT i=0;i<GetSize();i++)
		{
			ES_BOLAN * pBolan=pScript->GetAt(i);
			if(pBolan->Type==BOLAN_TYPE_OPERATOR&&
				(pBolan->Index==OPERATOR_JMP||pBolan->Index==OPERATOR_JMP_FUNC||pBolan->Index==OPERATOR_JZ))
			{
				if(pBolan->Level>Index)
					pBolan->Level--;
			}
		}
		void * Pos=pFunctionList->GetMap().GetFirstObjectPos();
		while(Pos)
		{
			CEasyString Key;
			ES_FUNCTION * pFun=pFunctionList->GetMap().GetNextObject(Pos,Key);
			if(pFun)
			{
				if(pFun->Type==FUNCTION_TYPE_SCRIPT)
				{
					if(pFun->FunStartPos>(int)Index)
						pFun->FunStartPos--;
					if(pFun->FunEndPos>(int)Index)
						pFun->FunEndPos--;
				}
			}
		}
	}
}


int CESBolanStack::ReverseFunctionParam(CESBolanStack * pScript,UINT StartIndex,UINT ParamCount)
{
	for(UINT i=0;i<ParamCount;i++)
	{
		ES_BOLAN * pBolan=pScript->GetAt(i+StartIndex);
		if(pBolan==NULL)
			return 2019;
		if(pBolan->Type!=BOLAN_TYPE_OPERATOR||pBolan->Index!=OPERATOR_ADD_VAR)
			return 2019;
		pBolan->Index=OPERATOR_ADD_CALL_PARAM;
	}
	for(UINT i=0;i<ParamCount/2;i++)
	{
		ES_BOLAN Temp=*pScript->GetAt(i+StartIndex);
		*pScript->GetAt(i+StartIndex)=*pScript->GetAt(ParamCount-i-1+StartIndex);
		*pScript->GetAt(ParamCount-i-1+StartIndex)=Temp;
	}
	return 0;
}

int CESBolanStack::DoVariableBind(CESThread * pESThread,int& LastLine)
{
	for(UINT i=0;i<pESThread->GetScript()->GetSize();i++)
	{
		ES_BOLAN * pBolan=pESThread->GetScript()->GetAt(i);
		LastLine=pBolan->Line;
		switch (pBolan->Type)
		{
		case BOLAN_TYPE_VARIABLE:
			{
				ES_VARIABLE * pVar=pESThread->FindVariable(pBolan->StrValue);
				if(pVar)
				{
					pBolan->Index=pVar->ID;
					pBolan->ValueType=pVar->Type;
				}
				else
				{
					return 4016;
				}
			}
			break;
		case BOLAN_TYPE_OPERATOR:
			{
				if(pBolan->Index==OPERATOR_ADD_VAR||pBolan->Index==OPERATOR_ADD_CALL_PARAM)
				{
					ES_VARIABLE * pVar=pESThread->AddEmptyLocalVariable(pBolan->StrValue,pBolan->ValueType);
					if(pVar==NULL)
					{
						return 6009;
					}
				}
				else if(pBolan->Index==OPERATOR_JMP_FUNC)
				{
					pESThread->NewLocalVariableList();
				}
				else if(pBolan->Index==OPERATOR_RET)
				{
					pESThread->ReleaseLocalVariableList();
				}
			}
			break;
		}

	}
	return 0;
}
