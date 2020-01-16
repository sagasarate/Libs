/****************************************************************************/
/*                                                                          */
/*      文件名:    DBFakeRecordSet.cpp                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"



namespace DBLib
{

CDBFakeRecordSet::CDBFakeRecordSet(void)
{
	m_DataBuffer.SetTag(_T("CDBFakeRecordSet"));
}

CDBFakeRecordSet::~CDBFakeRecordSet(void)
{
	Destory();
}

int CDBFakeRecordSet::Init(LPCVOID pData,int DataSize)
{
	Destory();
	m_DataBuffer.Create(DataSize);
	m_DataBuffer.PushBack(pData,DataSize);
	return DBERR_SUCCEED;
}


void CDBFakeRecordSet::Destory()
{
	m_DataBuffer.Destory();
}

int CDBFakeRecordSet::GetRecordSetDataSize()
{
	return m_DataBuffer.GetUsedSize();
}

LPCVOID CDBFakeRecordSet::GetRecordSetData()
{
	return m_DataBuffer.GetBuffer();
}

int CDBFakeRecordSet::GetRecordSetDataEx(int& Pos,LPVOID pBuffer,int& BufferSize)
{
	int ColNum=0;
	UINT PeekPtr=0;
	CEasyBuffer OutputBuffer(pBuffer, BufferSize, _T("CDBFakeRecordSet"));

	m_DataBuffer.Peek(PeekPtr,&ColNum,sizeof(int));
	if(ColNum<=0)
	{
		BufferSize=0;
		return GET_DATA_NO_MORE_DATA;
	}

	PeekPtr+=sizeof(DB_COLUMN_INFO)*ColNum;

	int RecordNum=0;
	m_DataBuffer.Peek(PeekPtr,&RecordNum,sizeof(int));
	if(RecordNum<=0)
	{
		BufferSize=0;
		return GET_DATA_NO_MORE_DATA;
	}


	OutputBuffer.PushBack(m_DataBuffer.GetBuffer(),PeekPtr);
	int * pRecordNum=(int *)((char *)OutputBuffer.GetBuffer()+PeekPtr-sizeof(int));
	*pRecordNum=0;

	if(Pos)
		PeekPtr=Pos;
	else
		Pos=PeekPtr;

	if(PeekPtr>=m_DataBuffer.GetUsedSize())
	{
		BufferSize=0;
		return GET_DATA_BUFFER_FULL;
	}

	int RecordPtr=PeekPtr;
	while(PeekPtr<m_DataBuffer.GetUsedSize())
	{
		int RecordLen=0;
		for(int j=0;j<ColNum;j++)
		{
			int ValueSize=0;
			m_DataBuffer.Peek(PeekPtr,&ValueSize,sizeof(int));
			if(ValueSize<0)
				ValueSize=0;
			RecordLen+=sizeof(int)+ValueSize;
			PeekPtr+=ValueSize;
		}
		if(RecordPtr+RecordLen-Pos>(int)OutputBuffer.GetFreeSize())
			break;
		RecordPtr+=RecordLen;
		(*pRecordNum)++;

	}
	OutputBuffer.PushBack((char *)m_DataBuffer.GetBuffer()+Pos,RecordPtr-Pos);
	Pos=RecordPtr;
	BufferSize=OutputBuffer.GetUsedSize();

	if(PeekPtr<m_DataBuffer.GetUsedSize())
		return GET_DATA_BUFFER_FULL;
	else
		return GET_DATA_NO_MORE_DATA;
}

int CDBFakeRecordSet::GetRecordCount()
{
	return 0;
}

int CDBFakeRecordSet::GetColumnCount()
{
	return 0;
}

LPCSTR CDBFakeRecordSet::GetColumnName(int Index)
{
	return NULL;
}

int CDBFakeRecordSet::GetIndexByColumnName(LPCSTR Name)
{
	return -1;
}

DB_COLUMN_INFO * CDBFakeRecordSet::GetColumnInfo(int Index)
{
	return NULL;
}


CDBValue& CDBFakeRecordSet::GetField(int Index)
{
	return m_EmptyValue;
}

CDBValue& CDBFakeRecordSet::GetField(LPCSTR Name)
{
	return m_EmptyValue;
}

int CDBFakeRecordSet::MoveFirst()
{
	return DBERR_NOT_IMPLEMENT;
}

int CDBFakeRecordSet::MoveLast()
{
	return DBERR_NOT_IMPLEMENT;
}

int CDBFakeRecordSet::MoveNext()
{
	return DBERR_NOT_IMPLEMENT;
}

int CDBFakeRecordSet::MovePrevious()
{
	return DBERR_NOT_IMPLEMENT;
}

int CDBFakeRecordSet::MoveTo(int Index)
{
	return DBERR_NOT_IMPLEMENT;
}

bool CDBFakeRecordSet::IsEOF()
{
	return true;
}

bool CDBFakeRecordSet::IsBOF()
{
	return true;
}

}
