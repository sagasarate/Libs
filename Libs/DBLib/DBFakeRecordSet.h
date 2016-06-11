/****************************************************************************/
/*                                                                          */
/*      文件名:    DBFakeRecordSet.h                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


namespace DBLib
{

class CDBFakeRecordSet :
	public IDBRecordSet
{
protected:
	CEasyBuffer		m_DataBuffer;
	CDBValue		m_EmptyValue;
public:
	enum GET_DATA_RETURN_CODE
	{
		GET_DATA_NO_MORE_DATA,
		GET_DATA_BUFFER_FULL,
	};

	CDBFakeRecordSet(void);
	virtual ~CDBFakeRecordSet(void);

	virtual int Init(LPCVOID pData,int DataSize);
	virtual void Destory();
	virtual int GetRecordSetDataSize();
	virtual LPCVOID GetRecordSetData();
	virtual int GetRecordSetDataEx(int& Pos,LPVOID pBuffer,int& BufferSize);

	virtual int GetRecordCount();
	virtual int GetColumnCount();
	virtual LPCSTR GetColumnName(int Index);
	virtual int GetIndexByColumnName(LPCSTR Name);
	virtual DB_COLUMN_INFO * GetColumnInfo(int Index);


	virtual CDBValue& GetField(int Index);
	virtual CDBValue& GetField(LPCSTR Name);
	virtual int MoveFirst();
	virtual int MoveLast();
	virtual int MoveNext();
	virtual int MovePrevious();
	virtual int MoveTo(int Index);
	virtual bool IsEOF();
	virtual bool IsBOF();
};

}
