/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyScriptTypes.h	                                    */
/*      创建日期:  2010年08月26日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define MAX_EXP_LENGTH			40960
#define MAX_IDENTIFIER_LENGTH	128
//#define MAX_STRING_LENGTH		4096
//#define MAX_STACK				16

enum BOLAN_TYPE
{
	BOLAN_TYPE_VALUE,
	BOLAN_TYPE_OPERATOR,
	BOLAN_TYPE_FUNCTION,
	BOLAN_TYPE_VARIABLE,
	BOLAN_TYPE_KEYWORD,
	BOLAN_TYPE_IDENTIFIER,
};

enum OPERATOR_TYPE
{
	OPERATOR_ADD,
	OPERATOR_SUB,
	OPERATOR_MUL,
	OPERATOR_DIV,
	OPERATOR_RP,
	OPERATOR_LP,
	OPERATOR_NEG,
	OPERATOR_COMMA,
	OPERATOR_LESS,
	OPERATOR_MORE,
	OPERATOR_LESS_EQU,
	OPERATOR_MORE_EQU,
	OPERATOR_EQU,
	OPERATOR_DIFF,
	OPERATOR_EVA,
	OPERATOR_AND,
	OPERATOR_OR,
	OPERATOR_NOT,
	OPERATOR_JMP,
	OPERATOR_JMP_FUNC,
	OPERATOR_JZ,
	OPERATOR_CALL,
	OPERATOR_RET,
	OPERATOR_CLEAR_STACK,
	OPERATOR_ADD_VAR,
	OPERATOR_ADD_CALL_PARAM,
	OPERATOR_EXIT,
	OPERATOR_NOP,
	OPERATOR_MAX,
};

enum VALUE_TYPE
{
	VALUE_TYPE_INT,
	VALUE_TYPE_INT64,
	VALUE_TYPE_FLOAT,
	VALUE_TYPE_DOUBLE,
	VALUE_TYPE_STRING,
};

enum SCRIPT_KEYWORDS
{		
	KW_EXIT=0,
	KW_IF,
	KW_THEN,
	KW_ELSE,
	KW_ELSEIF,
	KW_ENDIF,
	KW_WHILE,
	KW_DO,
	KW_ENDWHILE,
	KW_BREAK,
	KW_CONTINUE,
	KW_GOTO,
	KW_LINEEND,
	KW_INT,
	KW_INT64,
	KW_FLOAT,
	KW_DOUBLE,
	KW_STRING,
	KW_FUNCTION,
	KW_ENDFUN,
	KW_MAX,
};

enum FUNCTION_TYPE
{
	FUNCTION_TYPE_SCRIPT,
	FUNCTION_TYPE_C,
};

enum IDENTIFIER_TYPE
{
	IDENTIFIER_TYPE_UNKNOW,
	IDENTIFIER_TYPE_JUMP,
	IDENTIFIER_TYPE_JUMP_DEFINE,
	IDENTIFIER_TYPE_FUNCTION_DEFINE,
	IDENTIFIER_TYPE_VARIABLE_DEFINE,
};

extern LPCTSTR OPERATOR_STRINGS[OPERATOR_MAX];
extern LPCTSTR KEYWORD_STRINGS[KW_MAX];



struct ES_BOLAN
{
	BOLAN_TYPE		Type;
	VALUE_TYPE		ValueType;
	UINT			Index;
	UINT			Line;
	UINT			Level;
	CEasyString		StrValue;
	union
	{
		int			ValueInt;
		INT64		ValueInt64;
		float		ValueFloat;
		double		ValueDouble;
	};
	

	ES_BOLAN()
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
	}
	ES_BOLAN(char Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(unsigned char Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(short Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(unsigned short Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(int Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(unsigned int Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	//ES_BOLAN(long Value)
	//{
	//	ValueInt64=0;
	//	Index=0;
	//	Level=0;
	//	Line=0;
	//	*this=Value;
	//}
	//ES_BOLAN(unsigned long Value)
	//{
	//	ValueInt64=0;
	//	Index=0;
	//	Level=0;
	//	Line=0;
	//	*this=Value;
	//}
	ES_BOLAN(__int64 Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(unsigned __int64 Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(float Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(double Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(LPCTSTR Value)
	{
		ValueInt64=0;
		Index=0;
		Level=0;
		Line=0;
		*this=Value;
	}
	ES_BOLAN(const ES_BOLAN& Bolan)
	{
		Type=Bolan.Type;
		ValueType=Bolan.ValueType;
		ValueInt64=Bolan.ValueInt64;
		StrValue=Bolan.StrValue;		
		Index=Bolan.Index;
		Level=Bolan.Level;
		Line=Bolan.Line;
	}
	ES_BOLAN& operator=(const ES_BOLAN& Bolan)
	{
		Type=Bolan.Type;
		ValueType=Bolan.ValueType;
		ValueInt64=Bolan.ValueInt64;
		StrValue=Bolan.StrValue;
		Index=Bolan.Index;
		Level=Bolan.Level;
		Line=Bolan.Line;
		return *this;
	}
	void CopyValue(const ES_BOLAN& Bolan)
	{
		Type=Bolan.Type;
		Index=Bolan.Index;
		ValueType=Bolan.ValueType;
		if(ValueType==VALUE_TYPE_STRING)
		{
			StrValue=Bolan.StrValue;
		}
		else
		{
			ValueInt64=Bolan.ValueInt64;
		}
	}	
	void Clear()
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt64=0;
		StrValue.Clear();
		Index=0;
		Level=0;
		Line=0;
	}
	operator char()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (char)ValueInt;
		case VALUE_TYPE_INT64:
			return (char)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (char)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (char)ValueDouble;
		}		
		return 0;
	}
	operator unsigned char()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (unsigned char)ValueInt;
		case VALUE_TYPE_INT64:
			return (unsigned char)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (unsigned char)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (unsigned char)ValueDouble;
		}		
		return 0;
	}
	operator short()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (short)ValueInt;
		case VALUE_TYPE_INT64:
			return (short)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (short)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (short)ValueDouble;
		}		
		return 0;
	}
	operator unsigned short()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (unsigned short)ValueInt;
		case VALUE_TYPE_INT64:
			return (unsigned short)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (unsigned short)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (unsigned short)ValueDouble;
		}		
		return 0;
	}
	operator int()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (int)ValueInt;
		case VALUE_TYPE_INT64:
			return (int)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (int)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (int)ValueDouble;
		}		
		return 0;
	}
	operator unsigned int()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (unsigned int)ValueInt;
		case VALUE_TYPE_INT64:
			return (unsigned int)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (unsigned int)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (unsigned int)ValueDouble;
		}		
		return 0;
	}
	//operator long()
	//{
	//	switch(ValueType)
	//	{
	//	case VALUE_TYPE_INT:
	//		return (long)ValueInt;
	//	case VALUE_TYPE_INT64:
	//		return (long)ValueInt64;
	//	case VALUE_TYPE_FLOAT:
	//		return (long)ValueFloat;
	//	case VALUE_TYPE_DOUBLE:
	//		return (long)ValueDouble;
	//	}		
	//	return 0;
	//}
	//operator unsigned long()
	//{
	//	switch(ValueType)
	//	{
	//	case VALUE_TYPE_INT:
	//		return (unsigned long)ValueInt;
	//	case VALUE_TYPE_INT64:
	//		return (unsigned long)ValueInt64;
	//	case VALUE_TYPE_FLOAT:
	//		return (unsigned long)ValueFloat;
	//	case VALUE_TYPE_DOUBLE:
	//		return (unsigned long)ValueDouble;
	//	}		
	//	return 0;
	//}
	operator __int64()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (__int64)ValueInt;
		case VALUE_TYPE_INT64:
			return (__int64)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (__int64)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (__int64)ValueDouble;
		}		
		return 0;
	}
	operator unsigned __int64()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (unsigned __int64)ValueInt;
		case VALUE_TYPE_INT64:
			return (unsigned __int64)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (unsigned __int64)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (unsigned __int64)ValueDouble;
		}		
		return 0;
	}
	operator float()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (float)ValueInt;
		case VALUE_TYPE_INT64:
			return (float)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (float)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (float)ValueDouble;
		}		
		return 0;
	}
	operator double()
	{
		switch(ValueType)
		{
		case VALUE_TYPE_INT:
			return (double)ValueInt;
		case VALUE_TYPE_INT64:
			return (double)ValueInt64;
		case VALUE_TYPE_FLOAT:
			return (double)ValueFloat;
		case VALUE_TYPE_DOUBLE:
			return (double)ValueDouble;
		}		
		return 0;
	}
	operator LPCTSTR()
	{
		return StrValue;
	}
	char operator=(char Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	unsigned char operator=(unsigned char Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	short operator=(short Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	unsigned short operator=(unsigned short Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	int operator=(int Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	unsigned int operator=(unsigned int Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT;
		ValueInt=Value;
		return ValueInt;
	}
	//int operator=(long Value)
	//{
	//	Type=BOLAN_TYPE_VALUE;
	//	ValueType=VALUE_TYPE_INT;
	//	ValueInt=Value;
	//	return ValueInt;
	//}
	//unsigned int operator=(unsigned long Value)
	//{
	//	Type=BOLAN_TYPE_VALUE;
	//	ValueType=VALUE_TYPE_INT;
	//	ValueInt=Value;
	//	return ValueInt;
	//}
	__int64 operator=(__int64 Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT64;
		ValueInt64=Value;
		return ValueInt64;
	}
	unsigned __int64 operator=(unsigned __int64 Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_INT64;
		ValueInt64=Value;
		return ValueInt64;
	}
	float operator=(float Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_FLOAT;
		ValueFloat=Value;
		return ValueFloat;
	}
	double operator=(double Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_DOUBLE;
		ValueDouble=Value;
		return ValueDouble;
	}
	LPCTSTR operator=(LPCTSTR Value)
	{
		Type=BOLAN_TYPE_VALUE;
		ValueType=VALUE_TYPE_STRING;
		StrValue=Value;
		return StrValue;
	}
};


inline bool CanMakeIdentifier(TCHAR c)
{
	if(c!='+'&&c!='-'&&c!='*'&&c!='/'&&
		c!=','&&c!='('&&c!=')'&&c!=' '&&
		c!='='&&c!='>'&&c!='<'&&c!='~'&&
		c!=0&&c!=':'&&c!=';'&&c>'\r'&&c!='\n')
		return true;
	else 
		return false;
}

inline void StrToNumber(LPCTSTR szNumberStr,ES_BOLAN& Value)
{
	bool IsReal=false;
	bool IsFloat=false;
	bool IsDouble=false;
	bool IsInt=_tcschr(szNumberStr,'I')!=NULL;
	bool IsInt64=_tcschr(szNumberStr,'L')!=NULL;
	bool IsHex=(*szNumberStr=='0'&&*(szNumberStr+1)=='X');
	if(!IsHex)
	{
		IsReal=_tcschr(szNumberStr,'.')!=NULL||_tcschr(szNumberStr,'E')!=NULL;
		IsFloat=_tcschr(szNumberStr,'F')!=NULL;
		IsDouble=_tcschr(szNumberStr,'D')!=NULL;
	}
	

	if(IsReal||IsFloat||IsDouble)
	{
		double ValueMax=_tstof(szNumberStr);
		if(IsDouble)
		{
			Value.ValueType=VALUE_TYPE_DOUBLE;
			Value.ValueDouble=ValueMax;
		}
		else if(IsFloat)
		{
			Value.ValueType=VALUE_TYPE_FLOAT;
			Value.ValueFloat=(float)ValueMax;
		}
		else
		{
			if(ValueMax<(1.175494351E-38)||ValueMax>(3.402823466E+38))
			{
				Value.ValueType=VALUE_TYPE_DOUBLE;
				Value.ValueDouble=ValueMax;
			}
			else
			{
				Value.ValueType=VALUE_TYPE_FLOAT;
				Value.ValueFloat=(float)ValueMax;
			}
		}
	}
	else
	{		
		INT64 ValueMax=0;
		if(IsHex)
#pragma warning( push )
#pragma warning (disable : 4996)
#pragma warning (disable : 6031)
			_stscanf(szNumberStr,_T("0X%llX"),&ValueMax);
#pragma warning( pop )
		else
			ValueMax=_tstoi64(szNumberStr);

		if(IsInt)
		{
			Value.ValueType=VALUE_TYPE_INT;
			Value.ValueInt=(int)ValueMax;
		}
		else if(IsInt64)
		{
			Value.ValueType=VALUE_TYPE_INT64;
			Value.ValueInt64=ValueMax;
		}
		else
		{
			if(ValueMax>=(-((INT64)2147483648LL))&&ValueMax<=((INT64)2147483647LL))
			{
				Value.ValueType=VALUE_TYPE_INT;
				Value.ValueInt=(int)ValueMax;
			}			
			else
			{
				Value.ValueType=VALUE_TYPE_INT64;
				Value.ValueInt64=ValueMax;
			}
		}

	}
}

inline CEasyString BolanToString(const ES_BOLAN& bolan)
{
	CEasyString temp;
	
	
	switch(bolan.Type)
	{
	case BOLAN_TYPE_VALUE:
		switch(bolan.ValueType)
		{
		case VALUE_TYPE_INT:
			temp.Format(_T("值Int=%d"),bolan.ValueInt);
			break;
		case VALUE_TYPE_INT64:
			temp.Format(_T("值Int64=%lld"),bolan.ValueInt64);
			break;
		case VALUE_TYPE_FLOAT:
			temp.Format(_T("值Float=%g"),bolan.ValueFloat);
			break;
		case VALUE_TYPE_DOUBLE:
			temp.Format(_T("值Double=%g"),bolan.ValueDouble);
			break;
		case VALUE_TYPE_STRING:
			temp.Format(_T("值Str=\"%s\""),(LPCTSTR)bolan.StrValue);
			break;
		}
		break;	
	case BOLAN_TYPE_OPERATOR:
		switch(bolan.Index)
		{
		case OPERATOR_JMP:
		case OPERATOR_JZ:
			temp.Format(_T("操作符<%s>%d"),
				OPERATOR_STRINGS[bolan.Index],bolan.Level);
			break;
		case OPERATOR_CALL:
		case OPERATOR_ADD_VAR:
		case OPERATOR_ADD_CALL_PARAM:
			temp.Format(_T("操作符<%s>%s"),
				OPERATOR_STRINGS[bolan.Index],(LPCTSTR)bolan.StrValue);
			break;
		default:
			if(bolan.Index<OPERATOR_MAX)
			{
				temp.Format(_T("操作符<%s>"),
					OPERATOR_STRINGS[bolan.Index]);
			}
			else
			{
				temp.Format(_T("未知操作符<%d>"),
					bolan.Index);
			}
		}
		
		break;
	case BOLAN_TYPE_FUNCTION:
		temp.Format(_T("函数(%s)"),(LPCTSTR)bolan.StrValue);
		break;
	case BOLAN_TYPE_VARIABLE:	
		temp.Format(_T("变量(%s)"),(LPCTSTR)bolan.StrValue);
		break;
	case BOLAN_TYPE_KEYWORD:
		temp.Format(_T("关键字<%s>(%d)"),
			KEYWORD_STRINGS[bolan.Index],bolan.Level);
		break;
	case BOLAN_TYPE_IDENTIFIER:
		temp.Format(_T("标识符(%s)"),(LPCTSTR)bolan.StrValue);
		break;
	default:
		temp=_T("不可解释的符号");
	}
	return temp;
}

extern LPCTSTR ESGetErrorMsg(int ErrCode);