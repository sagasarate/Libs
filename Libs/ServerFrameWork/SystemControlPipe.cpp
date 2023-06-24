#include "stdafx.h"



CSystemControlPipe::CSystemControlPipe()
{
	m_pServer = NULL;
	m_IsLinkLog = false;
	m_AssembleBuffer.SetTag(_T("CSystemControlPipe"));
	m_SendBuffer.SetTag(_T("CSystemControlPipe"));
}


CSystemControlPipe::~CSystemControlPipe()
{
}

bool CSystemControlPipe::Init(CBaseServer * pServer)
{
	m_pServer = pServer;
	m_IsLinkLog = false;
	m_AssembleBuffer.Create(CSystemConfig::GetInstance()->GetControlPipeAssembleBufferSize());
	m_SendBuffer.Create(CSystemConfig::GetInstance()->GetControlPipeAssembleBufferSize());
	m_RecreateTime.SaveTime();
	m_ControlPipeName.Format("%s(%u)", (LPCTSTR)CSystemConfig::GetInstance()->GetControlPipeName(), GetCurProcessID());
	if (Create(m_ControlPipeName, CSystemConfig::GetInstance()->GetControlPipeAssembleBufferSize()))
	{
		LogDebug(_T("控制台命名管道已创建"));
		return true;
	}
	else
	{
		Log(_T("控制台命名管道创建失败"));
		return false;
	}
}
int CSystemControlPipe::Update(int ProcessLimit)
{
	if (IsDisconnected() && m_RecreateTime.IsTimeOut(5000))
	{
		if (Create(m_ControlPipeName, CSystemConfig::GetInstance()->GetControlPipeAssembleBufferSize()))
		{
			LogDebug(_T("控制台命名管道已创建"));
		}
		else
		{
			Log(_T("控制台命名管道创建失败"));
		}
	}
	return CNamedPipe::Update(ProcessLimit);
}
void CSystemControlPipe::OnConnection(bool Succeed)
{
	if (Succeed)
	{
		LogDebug(_T("控制台命名管道连接成功"));
	}
	else
	{
		Log(_T("控制台命名管道连接失败"));
	}
}
void CSystemControlPipe::OnDisconnection()
{
	LogDebug(_T("控制台命名管道连接断开"));
	m_AssembleBuffer.SetUsedSize(0);
	m_RecreateTime.SaveTime();
	m_IsLinkLog = false;
}

void CSystemControlPipe::OnRecvData(const BYTE * pData, UINT DataSize)
{
	if (m_AssembleBuffer.PushBack(pData, DataSize))
	{
		UINT PacketSize = 0;
		size_t Pos = 1;
		while (m_AssembleBuffer.Peek(Pos, &PacketSize, sizeof(UINT)))
		{
			PacketSize += CSmartStruct::GetEmptyStructSize();
			if (m_AssembleBuffer.GetUsedSize() >= PacketSize)
			{
				CSmartStruct MsgPacket(m_AssembleBuffer.GetBuffer(), PacketSize, false);
				OnMsg(MsgPacket);
				m_AssembleBuffer.PopFront(NULL, PacketSize);
				Pos = 1;
			}
			else
			{
				break;
			}
		}
	}
	else
	{
		Log(_T("装配缓冲溢出"));
	}
}

void CSystemControlPipe::OnMsg(CSmartStruct& MsgPacket)
{
	if (m_pServer == NULL)
	{
		Log(_T("未初始化"));
		return;
	}
	UINT MsgID = MsgPacket.GetMember(SC_SST_MSG_ID);
	switch (MsgID)
	{
	case SC_MSG_QUERY_SERVER_STATUS:
		{
			m_SendBuffer.Clear();
			m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_QUERY_SERVER_STATUS_RESULT);

			CSmartStruct QueryInfo = MsgPacket.GetMember(SC_SST_DATA);
			CSmartStruct ServerStatus = m_SendBuffer.PrepareSubStruct();
			void * Pos = QueryInfo.GetFirstMemberPosition();
			while (Pos)
			{
				WORD MemberID;
				CSmartValue Value = QueryInfo.GetNextMember(Pos, MemberID);
				if (MemberID == SC_SST_QSS_STATUS_ID)
				{
					WORD StatusID = Value;
					ServerStatus.AddMember(StatusID,
						m_pServer->GetServerStatus(StatusID));
				}
			}
			m_SendBuffer.FinishMember(SC_SST_DATA, ServerStatus.GetDataLen());
			m_SendBuffer.AddMember(SC_SST_PARAM1, MsgPacket.GetMember(SC_SST_PARAM1));
			m_SendBuffer.AddMember(SC_SST_PARAM2, MsgPacket.GetMember(SC_SST_PARAM2));
			SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
		}
		break;
	case SC_MSG_QUERY_ALL_SERVER_STATUS:
		{
			m_SendBuffer.Clear();
			m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_QUERY_SERVER_STATUS_RESULT);
			m_pServer->GetAllServerStatus(m_SendBuffer, SC_SST_DATA);
			m_SendBuffer.AddMember(SC_SST_PARAM1, MsgPacket.GetMember(SC_SST_PARAM1));
			m_SendBuffer.AddMember(SC_SST_PARAM2, MsgPacket.GetMember(SC_SST_PARAM2));
			SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
		}
		break;
	case SC_MSG_QUERY_SERVER_WORK_STATUS:
		{
			m_SendBuffer.Clear();
			m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_QUERY_SERVER_WORK_STATUS_RESULT);
			m_SendBuffer.AddMember(SC_SST_DATA, m_pServer->GetServerStatus(SC_SST_SS_WORK_STATUS));
			SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
		}
		break;
	case SC_MSG_GET_SERVER_STATUS_FORMAT_INFO:
		{
			CEasyArray<SERVER_STATUS_FORMAT_INFO> ServerStatusFormatList(_T("CSystemControlPipe"));
			m_pServer->GetAllServerStatusFormat(ServerStatusFormatList);

			m_SendBuffer.Clear();
			m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_GET_SERVER_STATUS_FORMAT_INFO_RESULT);

			CSmartStruct Data = m_SendBuffer.PrepareSubStruct();

			for (UINT i = 0; i < ServerStatusFormatList.GetCount(); i++)
			{
				SERVER_STATUS_FORMAT_INFO& Info = ServerStatusFormatList[i];
				
				CSmartStruct FormatInfo = Data.PrepareSubStruct();
				FormatInfo.AddMember(SC_SST_SSFI_STATUS_ID, Info.StatusID);
				FormatInfo.AddMember(SC_SST_SSFI_FORMAT_TYPE, Info.FormatType);
				FormatInfo.AddMember(SC_SST_SSFI_NAME, Info.szName);

				Data.FinishMember(SC_SST_SSFIL_SERVER_STATUS_FORMAT_INFO, FormatInfo.GetDataLen());
			}
			m_SendBuffer.FinishMember(SC_SST_DATA, Data.GetDataLen());
			m_SendBuffer.AddMember(SC_SST_PARAM1, MsgPacket.GetMember(SC_SST_PARAM1));
			m_SendBuffer.AddMember(SC_SST_PARAM2, MsgPacket.GetMember(SC_SST_PARAM2));
			SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
		}
		break;
	case SC_MSG_LINK_LOG:
		m_IsLinkLog = true;
		break;
	case SC_MSG_UNLINK_LOG:
		m_IsLinkLog = false;
		break;
	case SC_MSG_EXEC_COMMAND:
		{
			LPCTSTR szCmd = MsgPacket.GetMember(SC_SST_EC_COMMAND_STR);
			m_pServer->PushConsoleCmd(szCmd);
			m_SendBuffer.Clear();
			m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_EXEC_COMMAND_RESULT);
			m_SendBuffer.AddMember(SC_SST_DATA, (short)0);
			m_SendBuffer.AddMember(SC_SST_PARAM1, MsgPacket.GetMember(SC_SST_PARAM1));
			SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
		}
		break;
	
	}
}

void CSystemControlPipe::SendLogMsg(LPCTSTR LogMsg)
{
	if (m_IsLinkLog && IsConnected())
	{
		m_SendBuffer.Clear();
		m_SendBuffer.AddMember(SC_SST_MSG_ID, (WORD)SC_MSG_SERVER_LOG);
		m_SendBuffer.AddMember(SC_SST_DATA, LogMsg);
		SendData(m_SendBuffer.GetData(), m_SendBuffer.GetDataLen());
	}
}