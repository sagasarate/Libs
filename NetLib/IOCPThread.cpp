/****************************************************************************/
/*                                                                          */
/*      文件名:    IOCPThread.cpp                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"



IMPLEMENT_CLASS_INFO_STATIC(CIOCPThread,CEasyThread);

CIOCPThread::CIOCPThread(void):CEasyThread()
{
	m_hIOCP=NULL;
}

CIOCPThread::~CIOCPThread(void)
{
}

bool CIOCPThread::OnStart()
{
	PrintNetLog( _T("IOCP工作线程启动"));
	return true;
}

bool CIOCPThread::OnRun()
{		

	DWORD	NumberOfBytes = 0;
	ULONG_PTR	CompletionKey = 0;
	OVERLAPPED_EX	* pOverlapped=NULL;
	
	

	if( GetQueuedCompletionStatus( m_hIOCP,&NumberOfBytes,&CompletionKey,(LPOVERLAPPED*)(&pOverlapped),100 ))
	{		
		pOverlapped->pOverLappedObject->GetDataBuff()->SetUsedSize(NumberOfBytes);
		pOverlapped->pOverLappedObject->SetEventID(IOE_PACKET);
		pOverlapped->pOverLappedObject->Process(IOE_PACKET);
		
	}
	else
	{		
		if(pOverlapped)
		{			
			UINT ErrorCode=GetLastError();
			if(ErrorCode!=64&&ErrorCode!=1236)
				PrintNetLog( _T("IOCP返回错误%d(%d)"),
				ErrorCode,
				pOverlapped->pOverLappedObject->GetType());
			pOverlapped->pOverLappedObject->SetErrorCode(ErrorCode);
			pOverlapped->pOverLappedObject->SetEventID(IOE_ERROR);
			pOverlapped->pOverLappedObject->Process(IOE_ERROR);				
		}

	}	

	
	return true;
}

void CIOCPThread::OnTerminate()
{
	PrintNetLog( _T("IOCP工作线程退出"));
}