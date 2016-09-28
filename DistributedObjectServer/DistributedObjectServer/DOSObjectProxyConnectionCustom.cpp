#include "stdafx.h"



CDOSObjectProxyConnectionCustom::CDOSObjectProxyConnectionCustom()
{
	m_pProxyConnection = NULL;
	m_Inited = false;
}


CDOSObjectProxyConnectionCustom::~CDOSObjectProxyConnectionCustom()
{
}


//IDOSObjectProxyConnectionBase
void CDOSObjectProxyConnectionCustom::Destory()
{
	m_Inited = false;
	if (m_pProxyConnection)
		m_pProxyConnection->Destory();
	SAFE_RELEASE(m_pProxyConnection);
}
UINT CDOSObjectProxyConnectionCustom::AddUseRef()
{
	return CNameObject::AddUseRef();
}
void CDOSObjectProxyConnectionCustom::Release()
{
	CNameObject::Release();
}
bool CDOSObjectProxyConnectionCustom::PushMessage(CDOSMessagePacket * pPacket)
{
	if (!m_Inited)
		return false;

	m_pService->GetDOSServer()->AddRefMessagePacket(pPacket);
#ifdef _DEBUG
	pPacket->SetAllocTime(5);
#endif
	if (m_MsgQueue.PushBack(&pPacket))
	{
#ifdef _DEBUG
		pPacket->SetAllocTime(51);
#endif
		return true;
	}
	else
	{
		if (!m_pService->GetDOSServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("释放消息内存块失败！"));
		}
		return false;
	}
}

//IDOSObjectProxyConnectionOperator

OBJECT_ID CDOSObjectProxyConnectionCustom::GetObjectID()
{
	return m_ObjectID;
}

const CLIENT_PROXY_CONFIG& CDOSObjectProxyConnectionCustom::GetConfig()
{
	return m_pService->GetConfig();
}

bool CDOSObjectProxyConnectionCustom::SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize)
{
	return m_pService->GetDOSServer()->GetRouter()->RouterMessage(m_ObjectID, ReceiverID, MsgID, MsgFlag, pData, DataSize) != FALSE;
}

bool CDOSObjectProxyConnectionCustom::SendDisconnectNotify(OBJECT_ID * pTargetObjectID, UINT TargetCount)
{
	int PacketSize = CDOSMessagePacket::CaculatePacketLength(0, TargetCount);

	CDOSMessagePacket * pNewPacket = m_pService->GetDOSServer()->NewMessagePacket(PacketSize);
	if (pNewPacket == NULL)
	{
		PrintDOSLog(_T("DOSLib"), _T("分配消息内存块失败！"));
		return false;
	}

	pNewPacket->SetTargetIDs(TargetCount, pTargetObjectID);
	pNewPacket->GetMessage().SetMsgID(DSM_PROXY_DISCONNECT);
	pNewPacket->GetMessage().SetMsgFlag(DOS_MESSAGE_FLAG_SYSTEM_MESSAGE);
	pNewPacket->GetMessage().SetSenderID(m_ObjectID);
	pNewPacket->MakePacketLength();

	bool Ret = m_pService->GetDOSServer()->GetRouter()->RouterMessage(pNewPacket) != FALSE;
	m_pService->GetDOSServer()->ReleaseMessagePacket(pNewPacket);
	return Ret;
}

BOOL CDOSObjectProxyConnectionCustom::Init(CDOSObjectProxyServiceCustom * pService, UINT ID, IDOSObjectProxyConnection * pProxyConnection)
{
	SetID(ID);
	m_pService = pService;

	m_ObjectID.ObjectIndex = GetID();
	m_ObjectID.GroupIndex = MAKE_PROXY_GROUP_INDEX(m_pService->GetConfig().ProxyType);
	m_ObjectID.ObjectTypeID = DOT_PROXY_OBJECT;
	m_ObjectID.RouterID = m_pService->GetRouterID();

	if (pProxyConnection->Initialize(this))
	{
		m_pProxyConnection = pProxyConnection;
		m_pProxyConnection->AddUseRef();
		m_Inited = true;
		return TRUE;
	}	
	return FALSE;
}





int CDOSObjectProxyConnectionCustom::Update(int ProcessPacketLimit)
{
	int ProcessCount = 0;

	CDOSMessagePacket * pPacket;
	while (m_MsgQueue.PopFront(&pPacket))
	{
#ifdef _DEBUG
		pPacket->SetAllocTime(52);
#endif
		if (pPacket->GetMessage().GetMsgFlag()&DOS_MESSAGE_FLAG_SYSTEM_MESSAGE)
			OnSystemMessage(pPacket);
		else
			OnMessage(pPacket);
		if (!m_pService->GetDOSServer()->ReleaseMessagePacket(pPacket))
		{
			PrintDOSLog(_T("DOSLib"), _T("释放消息内存块失败！"));
		}
		ProcessCount++;
		if (ProcessCount >= ProcessPacketLimit)
			break;
	}


	ProcessCount += m_pProxyConnection->Update(ProcessPacketLimit);
	return ProcessCount;
}

void CDOSObjectProxyConnectionCustom::OnMessage(CDOSMessagePacket * pPacket)
{
	m_pProxyConnection->OnMessage(&(pPacket->GetMessage()));
}

void CDOSObjectProxyConnectionCustom::OnSystemMessage(CDOSMessagePacket * pPacket)
{
	if (!m_pProxyConnection->OnSystemMessage(&(pPacket->GetMessage())))
	{
		switch (pPacket->GetMessage().GetMsgID())
		{
		case DSM_PROXY_REGISTER_MSG_MAP:
			if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
			{
				int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
				MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
				for (int i = 0; i < Count; i++)
				{
					m_pProxyConnection->OnRegisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
				}
			}
			break;
		case DSM_PROXY_UNREGISTER_MSG_MAP:
			if (pPacket->GetMessage().GetDataLength() >= sizeof(MSG_ID_TYPE))
			{
				int Count = (pPacket->GetMessage().GetDataLength()) / sizeof(MSG_ID_TYPE);
				MSG_ID_TYPE * pMsgIDs = (MSG_ID_TYPE *)(pPacket->GetMessage().GetDataBuffer());
				for (int i = 0; i < Count; i++)
				{
					m_pProxyConnection->OnUnregisterMsgMap(pMsgIDs[i], pPacket->GetMessage().GetSenderID());
				}
			}
			break;
		case DSM_PROXY_DISCONNECT:
			if (pPacket->GetMessage().GetDataLength() >= sizeof(UINT))
			{
				UINT Delay = *((UINT *)(pPacket->GetMessage().GetDataBuffer()));
				m_pProxyConnection->QueryDisconnect(Delay);
				PrintDOSLog(_T("DOSLib"), _T("0x%llX请求在%uMS后断开连接！"), pPacket->GetMessage().GetSenderID().ID, Delay);
			}
			break;
		case DSM_PROXY_GET_IP:
			{
				char Buff[256];
				char * pIPStr = Buff + sizeof(WORD);
				pIPStr[0] = 0;

				CIPAddress RemoteAddress;
				m_pProxyConnection->GetRemoteAddress(RemoteAddress);
				RemoteAddress.GetIPString(pIPStr, 250);
				*((WORD *)Buff) = RemoteAddress.GetPort();
				UINT Len = (UINT)strlen(pIPStr) + sizeof(WORD) + 1;


				m_pService->GetDOSServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
					DSM_PROXY_IP_REPORT, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, Buff, Len);
			}
			break;
		case DSM_ROUTE_LINK_LOST:
			m_pProxyConnection->OnClearMsgMapByRouterID(pPacket->GetMessage().GetSenderID().RouterID);
			break;
		case DSM_OBJECT_ALIVE_TEST:
			{
				BYTE IsEcho = 1;
				m_pService->GetDOSServer()->GetRouter()->RouterMessage(m_ObjectID, pPacket->GetMessage().GetSenderID(),
					DSM_OBJECT_ALIVE_TEST, DOS_MESSAGE_FLAG_SYSTEM_MESSAGE, &IsEcho, sizeof(IsEcho));
			}
			break;
		default:
			PrintDOSLog(_T("DOSLib"), _T("收到未知系统消息0x%llX"), pPacket->GetMessage().GetMsgID());
		}
	}
}
