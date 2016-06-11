/****************************************************************************/
/*                                                                          */
/*      文件名:    DBTypes.h                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

//数据类型
namespace DBLib
{

#define MAX_RECORD_DATA_LEN		65536
#define MAX_COLUMN_NAME			32
#define MAX_DECIMAL_LEN			32

enum DB_TYPES
{
	DB_TYPE_UNKNOW=0,
	DB_TYPE_STRING,		
	DB_TYPE_WSTRING,
	DB_TYPE_TINY,
	DB_TYPE_SMALLINT,
	DB_TYPE_INTEGER,	
	DB_TYPE_BIGINT,
	DB_TYPE_FLOAT,	
	DB_TYPE_DOUBLE,
	DB_TYPE_DATE,
	DB_TYPE_TIME,	
	DB_TYPE_TIMESTAMP,
	DB_TYPE_GUID,
	DB_TYPE_BINARY,
	DB_TYPE_NULL,
};

enum DB_ERROR_INFO
{
	DBERR_SUCCEED=0,
	DBERR_UNKNOW_ERROR,
	DBERR_NOT_IMPLEMENT,
	DBERR_INVALID_CONNECTION,
	DBERR_INVALID_PARAM,
	DBERR_BUFFER_OVERFLOW,
	DBERR_NO_RECORDS,
	DBERR_NO_MORE_RESULTS,
	DBERR_IS_RECORDSET_HEAD,
	DBERR_IS_RECORDSET_TAIL,
	DBERR_INVALID_RECORD_POSITION,
	DBERR_EXE_SQL_FAIL,
	DBERR_FETCH_RESULT_FAIL,
	DBERR_ERROR_CALL_SEQUENCE,
	DBERR_SQLALLOCHANDLEFAIL,
	DBERR_PARAMCOUNTFAIL,
	DBERR_NOTENOUGHPARAM,
	DBERR_PARAMTYPEERROR,
	DBERR_BINDPARAMFAIL,
	DBERR_BINDCOLFAIL,
	DBERR_SCROLLFAIL,	
	DBERR_MAX,
};

enum DB_RECORD_SET_TYPE
{
	DB_RS_TYPE_STATIC,
	DB_RS_TYPE_GENERAL_STATIC,
	DB_RS_TYPE_DYNAMIC,
};

enum DB_PARAM_TYPE
{
	DB_PARAM_TYPE_INPUT,
	DB_PARAM_TYPE_OUTPUT,
	DB_PARAM_TYPE_INPUT_OUTPUT,
};

class DB_DATE;
class DB_TIME;
class DB_TIMESTAMP;

class DB_DATE
{
public:
	short   year;
	short   month;
	short   day;
	DB_DATE();
	DB_DATE(const DB_DATE& Value);
	DB_DATE(const DB_TIMESTAMP& Value);
	//DB_DATE(const SYSTEMTIME& Value);
	//DB_DATE(const FILETIME& Value);
};


class DB_TIME
{
public:
	short   hour;
	short   minute;
	short   second;
	DB_TIME();
	DB_TIME(const DB_TIME& Value);
	DB_TIME(const DB_TIMESTAMP& Value);
	//DB_TIME(const SYSTEMTIME& Value);
	//DB_TIME(const FILETIME& Value);
};


class DB_TIMESTAMP
{
public:
	short			year;
	short			month;
	short			day;
	short			hour;
	short			minute;
	short			second;
	unsigned int    fraction;
	DB_TIMESTAMP();
	DB_TIMESTAMP(const DB_TIMESTAMP& Value);
	DB_TIMESTAMP(const DB_DATE& Value);
	DB_TIMESTAMP(const DB_TIME& Value);
	//DB_TIMESTAMP(const SYSTEMTIME& Value);
	//DB_TIMESTAMP(const FILETIME& Value);
};

//typedef struct tagSYSTEMTIME_EX
//:public SYSTEMTIME
//{
//	tagSYSTEMTIME_EX()
//	{
//		ZeroMemory(this,sizeof(*this));
//	}
//	tagSYSTEMTIME_EX(const SYSTEMTIME& Value)
//	{
//		memcpy(this,&Value,sizeof(*this));
//	}
//	tagSYSTEMTIME_EX(const DB_DATE& Value)
//	{
//		wYear=Value.year;
//		wMonth=Value.month;
//		wDay=Value.day;
//		wHour=0;
//		wMinute=0;
//		wSecond=0;
//		wMilliseconds=0;
//	}
//	tagSYSTEMTIME_EX(const DB_TIME& Value)
//	{
//		wYear=0;
//		wMonth=0;
//		wDay=0;
//		wHour=Value.hour;
//		wMinute=Value.minute;
//		wSecond=Value.second;
//		wMilliseconds=0;
//	}
//	tagSYSTEMTIME_EX(const DB_TIMESTAMP& Value)
//	{
//		wYear=Value.year;
//		wMonth=Value.month;
//		wDay=Value.day;
//		wHour=Value.hour;
//		wMinute=Value.minute;
//		wSecond=Value.second;
//		wMilliseconds=Value.fraction/1000000;
//	}
//	tagSYSTEMTIME_EX(const FILETIME& Value)
//	{
//		FileTimeToSystemTime(&Value,this);
//	}
//}SYSTEMTIME_EX;

//typedef struct tagFILETIME_EX
//:public FILETIME
//{
//	tagFILETIME_EX()
//	{
//		ZeroMemory(this,sizeof(*this));
//	}
//	tagFILETIME_EX(const FILETIME& Value)
//	{
//		memcpy(this,&Value,sizeof(*this));
//	}
//	tagFILETIME_EX(const DB_DATE& Value)
//	{
//		SYSTEMTIME_EX SystemFile=Value;
//		SystemTimeToFileTime(&SystemFile,this);
//	}
//	tagFILETIME_EX(const DB_TIME& Value)
//	{
//		SYSTEMTIME_EX SystemFile=Value;
//		SystemTimeToFileTime(&SystemFile,this);
//	}
//	tagFILETIME_EX(const DB_TIMESTAMP& Value)
//	{
//		SYSTEMTIME_EX SystemFile=Value;
//		SystemTimeToFileTime(&SystemFile,this);
//	}
//	tagFILETIME_EX(const SYSTEMTIME& Value)
//	{
//		SystemTimeToFileTime(&Value,this);
//	}
//}FILETIME_EX;

typedef struct  tagDB_GUID
{
	DWORD Data1;
	WORD Data2;
	WORD Data3;
	BYTE Data4[ 8 ];
	tagDB_GUID()
	{
		ZeroMemory(this,sizeof(*this));
	}
} DB_GUID;

typedef struct tagDB_COLUMN_INFO
{
	char	Name[MAX_COLUMN_NAME];
	short	Type;	
	short	DigitSize;
	UINT64	Size;
}DB_COLUMN_INFO;


typedef struct tagDB_PARAM_INFO
{	
	short	Type;	
	short	DigitSize;
	UINT64	Size;
	short	ParamType;
	bool	IsNull;	
}DB_PARAM_INFO;

inline DB_DATE::DB_DATE()
{
	ZeroMemory(this,sizeof(*this));
}
inline DB_DATE::DB_DATE(const DB_DATE& Value)
{
	memcpy(this,&Value,sizeof(*this));
}	
inline DB_DATE::DB_DATE(const DB_TIMESTAMP& Value)
{
	year=Value.year;
	month=Value.month;
	day=Value.day;		
}

//inline DB_DATE::DB_DATE(const SYSTEMTIME& Value)
//{
//	year=Value.wYear;
//	month=Value.wMonth;
//	day=Value.wDay;
//}
//inline DB_DATE::DB_DATE(const FILETIME& Value)
//{
//	SYSTEMTIME SysTime;
//	FileTimeToSystemTime(&Value,&SysTime);
//	year=SysTime.wYear;
//	month=SysTime.wMonth;
//	day=SysTime.wDay;
//}

inline DB_TIME::DB_TIME()
{
	ZeroMemory(this,sizeof(*this));
}
inline DB_TIME::DB_TIME(const DB_TIME& Value)
{
	memcpy(this,&Value,sizeof(*this));
}	
inline DB_TIME::DB_TIME(const DB_TIMESTAMP& Value)
{		
	hour=Value.hour;
	minute=Value.minute;
	second=Value.second;
}

//inline DB_TIME::DB_TIME(const SYSTEMTIME& Value)
//{	
//	hour=Value.wHour;
//	minute=Value.wMinute;
//	second=Value.wSecond;
//}
//inline DB_TIME::DB_TIME(const FILETIME& Value)
//{
//	SYSTEMTIME SysTime;
//	FileTimeToSystemTime(&Value,&SysTime);	
//	hour=SysTime.wHour;
//	minute=SysTime.wMinute;
//	second=SysTime.wSecond;
//}

inline DB_TIMESTAMP::DB_TIMESTAMP()
{
	ZeroMemory(this,sizeof(*this));
}
inline DB_TIMESTAMP::DB_TIMESTAMP(const DB_TIMESTAMP& Value)
{
	memcpy(this,&Value,sizeof(*this));
}
inline DB_TIMESTAMP::DB_TIMESTAMP(const DB_DATE& Value)
{
	year=Value.year;
	month=Value.month;
	day=Value.day;
	hour=0;
	minute=0;
	second=0;
	fraction=0;
}
inline DB_TIMESTAMP::DB_TIMESTAMP(const DB_TIME& Value)
{
	year=0;
	month=0;
	day=0;
	hour=Value.hour;
	minute=Value.minute;
	second=Value.second;
	fraction=0;
}	

//inline DB_TIMESTAMP::DB_TIMESTAMP(const SYSTEMTIME& Value)
//{
//	year=Value.wYear;
//	month=Value.wMonth;
//	day=Value.wDay;
//	hour=Value.wHour;
//	minute=Value.wMinute;
//	second=Value.wSecond;
//	fraction=Value.wMilliseconds*1000000;
//}
//inline DB_TIMESTAMP::DB_TIMESTAMP(const FILETIME& Value)
//{
//	SYSTEMTIME SysTime;
//	FileTimeToSystemTime(&Value,&SysTime);
//	year=SysTime.wYear;
//	month=SysTime.wMonth;
//	day=SysTime.wDay;
//	hour=SysTime.wHour;
//	minute=SysTime.wMinute;
//	second=SysTime.wSecond;
//	fraction=SysTime.wMilliseconds*1000000;
//}

inline UINT BitToUINT(LPVOID pBitData,int Len)
{
	UINT Value=0;
	if(Len<0)
		Len=0;
	if(Len>4)
		Len=4;
	memcpy(&Value,pBitData,Len);
	return Value;
}

inline UINT64 BitToUINT64(LPVOID pBitData,int Len)
{
	UINT64 Value=0;
	if(Len<0)
		Len=0;
	if(Len>8)
		Len=8;
	memcpy(&Value,pBitData,Len);
	return Value;
}

}