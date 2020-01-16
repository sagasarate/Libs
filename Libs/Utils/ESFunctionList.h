/****************************************************************************/
/*                                                                          */
/*      文件名:    ESFactionList.h		                                    */
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

#define SCRIPT_FUN_CALL_PARAM_LEN	32

typedef int (*LPSCRIPT_FUNCTION)(BYTE * pCallParam,CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);



template<typename ClassName,typename FunName>
class CScriptCCallDispatcherHelper
{
public:
	static int ScriptCCallDispatcher(BYTE * pCallParam,CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
	{		
		ClassName * pObject=*((ClassName **)pCallParam);
		FunName pFun=*(FunName*)(pCallParam+sizeof(ClassName *));
		return (pObject->*pFun)(pESThread,pResult,pParams,ParamCount);
	}
};

#define ADD_SCRIPT_CFUNCTION(ClassName,FunctionName,ScriptFunName,ParamCount,pFunctionList,pObject)  \
	(pFunctionList)->AddCFunction<ClassName>(ScriptFunName,ParamCount,pObject,&ClassName::FunctionName)


struct ES_FUNCTION
{

	UINT				ID;
	BYTE				Type;
	BYTE				ParaCount;
	CEasyString			Name;	
	union
	{
		struct
		{
			LPSCRIPT_FUNCTION 	FnPtr;
			BYTE				CallParam[SCRIPT_FUN_CALL_PARAM_LEN];			
		};
		struct  
		{
			int				FunStartPos;
			int				FunEndPos;
		};
	};
	

	ES_FUNCTION()
	{
		ID=0;
		Type=FUNCTION_TYPE_SCRIPT;
		ParaCount=0;
		FnPtr=NULL;
		ZeroMemory(CallParam,sizeof(CallParam));
		FunStartPos=0;
		FunEndPos=0;
	}
	ES_FUNCTION(ES_FUNCTION& FN)
	{
		ID=FN.ID;
		Type=FN.Type;
		Name=FN.Name;
		ParaCount=FN.ParaCount;		
		FnPtr=FN.FnPtr;
		memcpy(CallParam,FN.CallParam,sizeof(CallParam));
		FunStartPos=FN.FunStartPos;
		FunEndPos=FN.FunEndPos;
	}

	bool operator==(const ES_FUNCTION& FN) const
	{
		if(Name.CompareNoCase(FN.Name)) 
			return false; 
		else 
			return true;
	}
};


class CESFunctionList
{
protected:
	CStaticMap<CEasyString,ES_FUNCTION>	m_Map;
public:
	CESFunctionList()
	{
		m_Map.SetTag(_T("CESFunctionList"));
	}
	~CESFunctionList()
	{

	}
	BOOL Create(UINT Size)
	{
		return m_Map.Create(Size);
	}
	void Clear()
	{
		m_Map.Clear();
	}
	CStaticMap<CEasyString,ES_FUNCTION>& GetMap()
	{
		return m_Map;
	}
	UINT AddFunction(LPCTSTR ScriptFunName, BYTE ParaCount,int StartPos,int EndPos)
	{
		CEasyString Key(ScriptFunName);
		Key.MakeUpper();
		ES_FUNCTION * pFn=NULL;
		UINT ID=m_Map.New(Key,&pFn);
		if(pFn)
		{
			pFn->ID=ID;
			pFn->Type=FUNCTION_TYPE_SCRIPT;
			pFn->Name=ScriptFunName;
			pFn->ParaCount=ParaCount;			
			pFn->FunStartPos=StartPos;
			pFn->FunEndPos=EndPos;
		}
		return 0;
	}
	UINT AddCFunction(LPCTSTR ScriptFunName, BYTE ParaCount,LPSCRIPT_FUNCTION FnPtr,void * pCallParam,UINT ParamLen)
	{
		CEasyString Key(ScriptFunName);
		Key.MakeUpper();
		ES_FUNCTION * pFn=NULL;
		UINT ID=m_Map.New(Key,&pFn);
		if(pFn)
		{
			pFn->ID=ID;
			pFn->Type=FUNCTION_TYPE_C;
			pFn->Name=ScriptFunName;
			pFn->ParaCount=ParaCount;
			pFn->FnPtr=FnPtr;
			if(ParamLen>SCRIPT_FUN_CALL_PARAM_LEN)
				ParamLen=SCRIPT_FUN_CALL_PARAM_LEN;
			memcpy(pFn->CallParam,pCallParam,ParamLen);
			
		}
		return 0;
	}
	template<typename ClassName,typename FunName>
	UINT AddCFunction(LPCTSTR ScriptFunName, BYTE ParaCount,ClassName * pObject,FunName Fun)
	{
		CEasyString Key(ScriptFunName);
		Key.MakeUpper();
		ES_FUNCTION * pFn=NULL;
		UINT ID=m_Map.New(Key,&pFn);
		if(pFn)
		{
			pFn->ID=ID;
			pFn->Type=FUNCTION_TYPE_C;
			pFn->Name=ScriptFunName;
			pFn->ParaCount=ParaCount;
			pFn->FnPtr=CScriptCCallDispatcherHelper<ClassName,FunName>::ScriptCCallDispatcher;			
			memcpy(pFn->CallParam,&pObject,sizeof(pObject));
			memcpy(pFn->CallParam+sizeof(pObject),&Fun,sizeof(Fun));

		}
		return 0;	
	}
	ES_FUNCTION * FindFunction(UINT ID)
	{
		return m_Map.GetObject(ID);
	}
	ES_FUNCTION * FindFunction(LPCTSTR FunctionName)
	{
		CEasyString Key(FunctionName);
		Key.MakeUpper();
		return m_Map.Find(Key);
	}
};