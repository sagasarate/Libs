/****************************************************************************/
/*                                                                          */
/*      文件名:    MySQLConnection.h                                        */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "dbinterfaces.h"


namespace DBLib
{

typedef MYSQL * MYSQL_HANDLE;
typedef MYSQL_RES * MYSQL_RES_HANDLE;
typedef MYSQL_STMT * MYSQL_STMT_HANDLE;

class CMySQLDatabase;

class CMySQLConnection :
	public IDBConnection
{
protected:
	MYSQL_HANDLE		m_MySQLHandle;
	MYSQL_STMT_HANDLE	m_MySQLStmt;
	CMySQLDatabase *	m_pDatabase;
	bool				m_UseSTMTMode;

	DECLARE_CLASS_INFO(CMySQLConnection)
public:
	CMySQLConnection(void);
	virtual ~CMySQLConnection(void);

	int Init(CMySQLDatabase * pDatabase);
	virtual void Destory();

	virtual IDBRecordSet * CreateRecordSet(int RecordSetType);
	virtual IDBParameterSet * CreateParameterSet(int RecordSetType);

	virtual int Connect(LPCSTR ConnectStr);
	virtual int Disconnect();
	virtual BOOL IsConnected();
	virtual int ExecuteSQL(LPCSTR SQLStr,int StrLen=0,IDBParameterSet * pParamSet=NULL);		
	virtual int GetResults(IDBRecordSet * pDBRecordset);
	virtual int NextResults(IDBRecordSet * pDBRecordset);	
	virtual int GetAffectedRowCount();
	virtual int EnableTransaction(BOOL IsEnable);
	virtual int Commit();
	virtual int RollBack();

	virtual UINT GetLastDatabaseErrorCode();
	virtual LPCSTR GetLastDatabaseErrorString();

	virtual int TranslateString(LPCSTR szSource,int SrcLen,LPTSTR szTarget,int MaxLen);

	static int DBLibTypeToMySQLType(int Type,UINT& Size,UINT& DitigalSize);
	static int MySQLTypeToDBLibType(int Type,UINT& Size,UINT& DitigalSize);
	static UINT GetMySQLTypeBinLength(int Type,UINT Size,UINT DitigalSize);

	static BOOL MySQLStrValueToDBValue(int ValueType,LPCVOID pData,int DataSize,int DBType,int DitigalSize,CDBValue& DBValue);

	static BOOL DBValueToMySQLBinValue(int Type,LPCVOID pDBValue,int DBValueSize,LPVOID pData,int DataSize);
	static BOOL MySQLBinValueToDBValue(int Type,LPCVOID pData,int DataSize,int DBType,int DitigalSize,CDBValue& DBValue);

	void ProcessErrorMsg(MYSQL_STMT_HANDLE hStmt,LPCSTR Msg);
protected:
	int FetchStaticResult(CDBStaticRecordSet * pDBRecordset);
	int FetchResult(CMySQLRecordSet * pDBRecordset);
	int FetchDynamicResult(CMySQLDynamicRecordSet * pDBRecordset);
	DWORD FetchConnectFlags(LPCSTR FlagsStr);

	int ExecuteSQLDirect(LPCSTR SQLStr,int StrLen);
	int ExecuteSQLWithParam(LPCSTR SQLStr,int StrLen,CDBParameterSet * pParamSet);
};

}