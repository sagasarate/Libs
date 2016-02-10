/****************************************************************************/
/*                                                                          */
/*      文件名:    ControlPanel.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

CControlPanel::CControlPanel(void)
{
	m_MsgPool.Create(1024);
	m_CommandPool.Create(1024);
	m_ServerStatus.Create(SERVER_STATUS_BLOCK_SIZE);
#ifdef WIN32
	m_hWnd=NULL;
#endif
}

CControlPanel::~CControlPanel(void)
{

}

BOOL CControlPanel::PushMsg(LPCTSTR szMsg)
{
	FUNCTION_BEGIN;
	if(g_ServiceWorkMode==SWM_APPLICATION)
	{
		return PushMsg(CON_MSG_LOG,szMsg,(int)strlen(szMsg)+1);
	}
	FUNCTION_END;
	return FALSE;
}

BOOL CControlPanel::PushMsg(UINT Type,LPCVOID pData,int Len)
{
	FUNCTION_BEGIN;

	PANEL_MSG * pMsg;
	UINT ID=m_MsgPool.NewObject(&pMsg);
	if(ID)
	{
		pMsg->ID=ID;
		pMsg->MsgType=Type;
		if(Len>=MAX_CONSOLE_MSG_LEN)
			Len=MAX_CONSOLE_MSG_LEN-1;
		memcpy(pMsg->Msg,pData,Len);
		pMsg->Msg[Len]=0;
		return TRUE;
	}

	FUNCTION_END;
	return FALSE;
}

PANEL_MSG * CControlPanel::GetMsg()
{
	FUNCTION_BEGIN;
	LPVOID Pos=m_MsgPool.GetFirstObjectPos();

	if(Pos)
		return m_MsgPool.GetNext(Pos);
	else
		return NULL;
	FUNCTION_END;
	return NULL;
}

BOOL CControlPanel::ReleaseMsg(UINT ID)
{
	FUNCTION_BEGIN;

	return m_MsgPool.DeleteObject(ID);

	FUNCTION_END;
	return FALSE;
}


BOOL CControlPanel::PushCommand(LPCTSTR szCommand)
{
	FUNCTION_BEGIN;

	PANEL_MSG * pMsg;
	UINT ID=m_CommandPool.NewObject(&pMsg);
	if(ID)
	{
		pMsg->ID=ID;
		strncpy_0(pMsg->Msg,MAX_CONSOLE_MSG_LEN,szCommand,MAX_CONSOLE_MSG_LEN);
		return TRUE;
	}


	FUNCTION_END;
	return FALSE;
}

PANEL_MSG * CControlPanel::GetCommand()
{
	FUNCTION_BEGIN;
	LPVOID Pos=m_CommandPool.GetFirstObjectPos();

	if(Pos)
		return m_CommandPool.GetNext(Pos);
	else
		return NULL;
	FUNCTION_END;
	return NULL;
}

BOOL CControlPanel::ReleaseCommand(UINT ID)
{
	FUNCTION_BEGIN;

	return m_CommandPool.DeleteObject(ID);

	FUNCTION_END;
	return FALSE;
}

void CControlPanel::SetServerStatus(LPCVOID pData,UINT DataSize)
{
	CAutoLock AutoLock(m_CriticalSection);
	m_ServerStatus.SetUsedSize(0);
	m_ServerStatus.PushBack(pData,DataSize);
}
UINT CControlPanel::GetServerStatus(LPVOID pBuffer,UINT BufferSize)
{
	CAutoLock AutoLock(m_CriticalSection);
	if(BufferSize>=m_ServerStatus.GetUsedSize())
	{
		UINT Pos=0;
		m_ServerStatus.Peek(Pos,pBuffer,m_ServerStatus.GetUsedSize());
	}
	return m_ServerStatus.GetUsedSize();
}
void CControlPanel::SetServerStatusFormat(WORD StatusID,LPCTSTR szStatusName,int FormatType)
{
	CAutoLock AutoLock(m_CriticalSection);

	SERVER_STATUS_FORMAT_INFO FormatInfo;
	strncpy_0(FormatInfo.szName,MAX_SERVER_STATUS_NAME_LEN,szStatusName,MAX_SERVER_STATUS_NAME_LEN);
	FormatInfo.FormatType=FormatType;
	m_ServerStatusFormats.Insert(StatusID,FormatInfo);
}
SERVER_STATUS_FORMAT_INFO * CControlPanel::GetServerStatusFormat(WORD StatusID)
{
	return m_ServerStatusFormats.Find(StatusID);
}
