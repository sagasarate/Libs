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
#include "stdafx.h"

CSystemNetLinkManager::CSystemNetLinkManager(void)
{
	FUNCTION_BEGIN;
	m_pServer=NULL;
	FUNCTION_END;
}

CSystemNetLinkManager::~CSystemNetLinkManager(void)
{
}


CEasyNetLink * CSystemNetLinkManager::OnCreateLink(UINT ReportID)
{
	FUNCTION_BEGIN;
	return MONITORED_NEW(_T("CSystemNetLinkManager"), CSystemNetLink, m_pServer);
	FUNCTION_END;
	return NULL;
}


void CSystemNetLinkManager::SendLogMsg(LPCTSTR LogMsg)
{
	FUNCTION_BEGIN;

	static BYTE s_SendBuffer[65536];

	if(m_LinkMap.GetObjectCount())
	{
		CEasyBuffer	SendBuffer(s_SendBuffer, 65536, _T("CSystemNetLinkManager"));
		SMSG_HEADER * pMsgHeader = (SMSG_HEADER *)s_SendBuffer;
		UINT StrLne = _tcslen(LogMsg);

		pMsgHeader->MsgID = SC_MSG_SERVER_LOG;
		pMsgHeader->Size = (WORD)(sizeof(SMSG_HEADER) + StrLne*sizeof(TCHAR));
		memcpy(s_SendBuffer + sizeof(SMSG_HEADER), LogMsg, StrLne*sizeof(TCHAR));

		SendBuffer.SetUsedSize(pMsgHeader->Size);

		void * Pos = m_LinkMap.GetFirstObjectPos();
		while(Pos)
		{
			UINT Key;
			CEasyNetLink ** ppValue = m_LinkMap.GetNextObject(Pos, Key);
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
