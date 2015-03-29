/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemNetLinkManager.cpp                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

CSystemNetLinkManager::CSystemNetLinkManager(void)
{
	FUNCTION_BEGIN;
	m_pServer=NULL;
	FUNCTION_END;
}

CSystemNetLinkManager::~CSystemNetLinkManager(void)
{
}


CEasyNetLinkConnection * CSystemNetLinkManager::OnCreateConnection(UINT ReportID)
{
	FUNCTION_BEGIN;
	return new CSystemNetLink(m_pServer);
	FUNCTION_END;
	return NULL;
}


void CSystemNetLinkManager::SendLogMsg(LPCTSTR LogMsg)
{
	FUNCTION_BEGIN;

	static char s_SendBuffer[65536];

	if(m_ConnectionMap.GetObjectCount())
	{
		CSmartStruct LogStr(5000);

		LogStr.AddMember(SC_SST_SL_LOG_STR,LogMsg);


		CEasyBuffer	SendBuffer(s_SendBuffer,65536);
		SMSG_HEADER MsgHeader;

		MsgHeader.MsgID=SC_MSG_SERVER_LOG;
		MsgHeader.Size=(WORD)(sizeof(SMSG_HEADER)+LogStr.GetDataLen());
		SendBuffer.PushBack(&MsgHeader,sizeof(SMSG_HEADER));

		SendBuffer.PushBack(LogStr.GetData(),LogStr.GetDataLen());

		void * Pos=m_ConnectionMap.GetFirstObjectPos();
		while(Pos)
		{
			UINT Key;
			CEasyNetLinkConnection ** ppValue=m_ConnectionMap.GetNextObject(Pos,Key);
			if(ppValue)
			{
				CSystemNetLink * pLink=(CSystemNetLink *)(*ppValue);
				if(pLink->IsLinkLog())
				{
					pLink->SendData(SendBuffer.GetBuffer(),SendBuffer.GetUsedSize());
				}
			}
		}
	}
	FUNCTION_END;
}
