/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemNetLink.cpp                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

CSystemNetLink::CSystemNetLink(IBaseServer * pServer)
{
	FUNCTION_BEGIN;
	m_pServer=pServer;
	m_IsLinkLog=FALSE;
	FUNCTION_END;
}

CSystemNetLink::~CSystemNetLink(void)
{
}


void CSystemNetLink::SendMsg(WORD Msg,LPCVOID pData,int DataLen)
{
	FUNCTION_BEGIN;
	static char s_SendBuffer[65536];



	CEasyBuffer	SendBuffer(s_SendBuffer,65536);
	SMSG_HEADER MsgHeader;

	MsgHeader.MsgID=Msg;
	MsgHeader.Size=(WORD)(sizeof(SMSG_HEADER)+DataLen);
	SendBuffer.PushBack(&MsgHeader,sizeof(SMSG_HEADER));

	if(DataLen)
		SendBuffer.PushBack(pData,DataLen);
	SendData(SendBuffer.GetBuffer(),SendBuffer.GetUsedSize());


	FUNCTION_END;
}


void CSystemNetLink::OnLinkStart()
{
	FUNCTION_BEGIN;
	CClassifiedID ID=GetID();
	LogDebug("建立与[%s][%s:%u]的连接",
		ID.ToStr(),
		GetRemoteAddress().GetIPString(),
		GetRemoteAddress().GetPort());
	FUNCTION_END;
}

void CSystemNetLink::OnLinkEnd()
{
	FUNCTION_BEGIN;
	CClassifiedID ID=GetID();
	LogDebug("断开与[%s]的连接",ID.ToStr());
	FUNCTION_END;
}

//接受数据
void CSystemNetLink::OnData(const CEasyBuffer& DataBuffer)
{
	FUNCTION_BEGIN;


	SMSG * pMsg=(SMSG *)DataBuffer.GetBuffer();
	UINT DataSize=pMsg->Header.Size-sizeof(SMSG_HEADER);

	switch(pMsg->Header.MsgID)
	{
	case SC_MSG_QUERY_SERVER_STATUS:
		{
			CSmartStruct QueryInfo(pMsg->Data,DataSize,false);
			CSmartStruct ServerStatus(SERVER_STATUS_BLOCK_SIZE);
			for(int i=0;i<QueryInfo.GetMemberCount();i++)
			{
				if(QueryInfo.IndexToID(i)==SC_SST_QSS_STATUS_ID)
				{
					WORD StatusID=QueryInfo.GetMemberByIndex(i);
					ServerStatus.AddMember(StatusID,
						m_pServer->GetServerStatus(StatusID));
				}
			}
			SendMsg(SC_MSG_QUERY_SERVER_STATUS_RESULT,
				ServerStatus.GetData(),ServerStatus.GetDataLen());
		}
		break;
	case SC_MSG_QUERY_ALL_SERVER_STATUS:
		{
			SendMsg(SC_MSG_QUERY_SERVER_STATUS_RESULT,
				m_pServer->GetAllServerStatus().GetData(),
				m_pServer->GetAllServerStatus().GetDataLen());
		}
		break;
	case SC_MSG_SET_SERVER_STATUS:
		{
		}
		break;
	case SC_MSG_LINK_LOG:
		m_IsLinkLog=TRUE;
		Log("已开启接收Log");
		break;
	case SC_MSG_UNLINK_LOG:
		Log("已关闭接收Log");
		m_IsLinkLog=FALSE;
		break;
	case SC_MSG_EXEC_COMMAND:
		{
			CSmartStruct CommandInfo(pMsg->Data,DataSize,false);
			LPCTSTR szCommand=CommandInfo.GetMember(SC_SST_EC_COMMAND_STR);
			m_pServer->ExecCommand(szCommand);
		}
		break;
	case SC_MSG_GET_SERVER_STATUS_FORMAT_INFO:
		{
			CEasyMap<WORD,SERVER_STATUS_FORMAT_INFO>& ServerStatusFormats=CControlPanel::GetInstance()->GetAllServerStatusFormat();
			CSmartStruct Packet(SERVER_STATUS_FORMAT_INFO_SIZE*ServerStatusFormats.GetObjectCount());

			UINT BufferSize;
			void * pBuffer;
			void * Pos=ServerStatusFormats.GetSortedFirstObjectPos();
			while(Pos)
			{
				WORD Key;
				SERVER_STATUS_FORMAT_INFO * pInfo=ServerStatusFormats.GetNextObject(Pos,Key);
				if(pInfo)
				{
					pBuffer=Packet.PrepareMember(BufferSize);
					CSmartStruct FormatInfo(pBuffer,BufferSize,true);
					FormatInfo.AddMember(SC_SST_SSFI_STATUS_ID,Key);
					FormatInfo.AddMember(SC_SST_SSFI_FORMAT_TYPE,pInfo->FormatType);
					FormatInfo.AddMember(SC_SST_SSFI_NAME,pInfo->szName);
					Packet.FinishMember(SC_SST_SSFIL_SERVER_STATUS_FORMAT_INFO,FormatInfo.GetDataLen());
				}
			}

			SendMsg(SC_MSG_GET_SERVER_STATUS_FORMAT_INFO_RESULT,
				Packet.GetData(),Packet.GetDataLen());
		}
		break;
	}

	FUNCTION_END;
}
