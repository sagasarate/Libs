#include "stdafx.h"



CDHTService::CDHTService()
{
	m_SendBuffer1.Create(NET_DATA_BLOCK_SIZE);
	m_SendBuffer2.Create(NET_DATA_BLOCK_SIZE);	
	m_PeerCheckPtr = 1;
}


CDHTService::~CDHTService()
{
	
}

bool CDHTService::Init(CNetServer * pServer, const CIPAddress& ListenAddress, const NODE_ID& NodeID)
{
	SetServer(pServer);
	m_NodeID = NodeID;
	m_NodePool.Create(4096, 4096, 1024);
	m_PeerPool.Create(4096, 4096, 1024);
	m_SearchPool.Create(16, 16, 16);
	m_TRConnectionList.Create(64);
	m_SaveTimer.SaveTime();
	m_PeerCheckPtr = 1;
	if (Create(IPPROTO_UDP))
	{
		if (StartListen(ListenAddress))
		{			
			return true;
		}
	}	
	return false;
}

void CDHTService::AddPeerServer(const CIPAddress& NodeAddress)
{
	PEER_SERVER_INFO * pServer = m_PeerServerList.AddEmpty();
	pServer->Address = NodeAddress;
}
UINT CDHTService::AddSearch(const NODE_ID& InfoHash, WORD DownloadPort, UINT Timeout, UINT64 Param)
{
	SEARCH_INFO * pSearchInfo = m_SearchPool.Find(InfoHash);
	if (pSearchInfo == NULL)
	{
		UINT ID = m_SearchPool.New(InfoHash, &pSearchInfo);
		if (pSearchInfo)
		{
			pSearchInfo->SearchID = ID;
			pSearchInfo->Param = Param;
			pSearchInfo->InfoHash = InfoHash;
			pSearchInfo->DownloadPort = DownloadPort;
			pSearchInfo->SearchStartTime = time(NULL);
			pSearchInfo->SearchTimeout = Timeout;
			AddSearchPeers(pSearchInfo);
			return ID;
		}
		else
		{
			return 0;
		}
	}
	else
	{
		return pSearchInfo->SearchID;
	}
}
SEARCH_INFO * CDHTService::GetSearch(UINT ID)
{
	return m_SearchPool.GetObject(ID);
}

SEARCH_INFO * CDHTService::FindSearch(const NODE_ID& InfoHash)
{
	return m_SearchPool.Find(InfoHash);
}
bool CDHTService::DeleteSearch(UINT SearchID)
{
	SEARCH_INFO * pSearchInfo = GetSearch(SearchID);
	if (pSearchInfo)
	{
		for (UINT i = 0; i < pSearchInfo->DataPortAddressList.GetCount(); i++)
		{
			if (pSearchInfo->DataPortAddressList[i].ConnectionID)
			{
				CTRConnection * pConnection = m_TRConnectionList.GetObject(pSearchInfo->DataPortAddressList[i].ConnectionID);
				if (pConnection)
				{
					pConnection->Disconnect();
					m_TRConnectionList.DeleteObject(pSearchInfo->DataPortAddressList[i].ConnectionID);
				}
			}
		}		
		pSearchInfo->Clear();
		m_SearchPool.DeleteByID(SearchID);
	}
	return false;
}

bool CDHTService::LoadNodes(LPCTSTR FileName)
{	
	m_NodesFileName = FileName;
	CSmartPtr<IFileAccessor> pNodesFile = CFileSystemManager::GetInstance()->CreateFileAccessor(FILE_CHANNEL_NORMAL1);

	if (pNodesFile == NULL)
		return false;

	CEasyTimerEx CostTimer;

	CostTimer.SaveTime();
	if (pNodesFile->Open(FileName, IFileAccessor::modeOpen | IFileAccessor::modeRead | IFileAccessor::shareShareAll))
	{
		UINT FileSize = (UINT)pNodesFile->GetSize();
		if (FileSize < sizeof(BYTE) + sizeof(UINT))
		{
			PrintNetLog( _T("节点文件不合法"));
			return false;
		}
			
		BYTE Version;
		UINT NodeCount;

		pNodesFile->Read(&Version, sizeof(BYTE));
		pNodesFile->Read(&NodeCount, sizeof(UINT));

		if (Version != CUR_NODES_FILE_VERSION)
		{
			PrintNetLog( _T("节点文件版本错误"));
			return false;
		}

		UINT Size = sizeof(BYTE) + sizeof(UINT) + (sizeof(NODE_ID) + sizeof(CIPAddress))*NodeCount;
		if (FileSize < Size)
		{
			PrintNetLog( _T("节点文件大小错误"));
			return false;
		}

		for (UINT i = 0; i < NodeCount; i++)
		{
			NODE_ID NodeID;
			CIPAddress Address;

			pNodesFile->Read(&NodeID, sizeof(NodeID));
			pNodesFile->Read(&Address, sizeof(Address));
			InsertNode(NodeID, Address, NODE_FROM_TYPE_NEW);
		}

		pNodesFile->Close();

		PrintNetLog( _T("节点文件加载完毕，一共%u个节点%u个Peer,花费%g秒"),
			m_NodePool.GetObjectCount(), m_PeerPool.GetObjectCount(), (float)CostTimer.GetPastTime() / (float)CEasyTimerEx::TIME_UNIT_PER_SECOND);

		//void * Pos = m_PeerPool.GetFirstObjectPos();
		//while (Pos)
		//{
		//	CIPAddress Key;
		//	NODE_PEER * pPeer = m_PeerPool.GetNextObject(Pos, Key);
		//	if (pPeer->PeerAddress != Key)
		//	{
		//		PrintNetLog( _T("节点文件加载异常"));
		//	}
		//}

		return true;
	}
	return false;
}
bool CDHTService::SaveNodes(LPCTSTR FileName)
{
	m_NodesFileName = FileName;
	CSmartPtr<IFileAccessor> pNodesFile = CFileSystemManager::GetInstance()->CreateFileAccessor(FILE_CHANNEL_NORMAL1);

	PrintNetLog( _T("准备保存节点，一共%u个节点%u个Peer"), m_NodePool.GetObjectCount(), m_PeerPool.GetObjectCount());
	CEasyTimerEx CostTimer;

	CostTimer.SaveTime();

	if (pNodesFile->Open(FileName, IFileAccessor::modeCreateAlways | IFileAccessor::modeWrite | IFileAccessor::shareShareAll))
	{
		BYTE Version = CUR_NODES_FILE_VERSION;
		UINT NodeCount = m_NodePool.GetObjectCount();

		pNodesFile->Write(&Version, sizeof(BYTE));
		pNodesFile->Write(&NodeCount, sizeof(UINT));

		NodeCount = 0;
		void * Pos = m_NodePool.GetSortedFirstObjectPos();
		while (Pos)
		{
			DHT_NODE * pNode = m_NodePool.GetSortedNextObject(Pos);

			pNodesFile->Write(&pNode->NodeID, sizeof(NODE_ID));
			pNodesFile->Write(&pNode->pPeer->PeerAddress, sizeof(CIPAddress));
		}

		PrintNetLog( _T("节点文件保存完毕，花费%g秒"),
			(float)CostTimer.GetPastTime() / (float)CEasyTimerEx::TIME_UNIT_PER_SECOND);

		pNodesFile->Close();
		return true;
	}
	
	return false;
}


void CDHTService::OnTorrent(UINT SearchID, const BYTE * pTorrentData, UINT DataSize)
{
	SEARCH_INFO * pSearchInfo = GetSearch(SearchID);
	if (pSearchInfo && (!pSearchInfo->IsFinded))
	{
		CSHA1 Hasher;
		Hasher.Update(pTorrentData, DataSize);
		Hasher.Final();
		NODE_ID HashCode;
		Hasher.GetHash(HashCode.NodeID);
		if (pSearchInfo->InfoHash == HashCode)
		{
			pSearchInfo->IsFinded = true;
			OnSearchFinish(pSearchInfo->InfoHash, pSearchInfo->Param, pTorrentData, DataSize);
			DeleteSearch(SearchID);
		}
	}
	else
	{
		PrintNetLog( _T("种子下载完毕，但找不到对应搜索%u"), SearchID);
	}
}

void CDHTService::OnSearchFinish(const NODE_ID& InfoHash, UINT64 Param, const BYTE * pTorrentData, UINT DataSize)
{

}
void CDHTService::OnSearchTimeout(const NODE_ID& InfoHash, UINT64 Param)
{

}

int CDHTService::Update(int ProcessPacketLimit)
{
	int ProcessCount = CNetService::Update(ProcessPacketLimit);
	ProcessCount += UpdateSearch(ProcessPacketLimit);
	void * Pos = m_TRConnectionList.GetFirstObjectPos();
	while (Pos)
	{
		CTRConnection * pConnection = m_TRConnectionList.GetNextObject(Pos);
		ProcessCount += pConnection->Update(ProcessPacketLimit);

		if (pConnection->IsDisconnected())
		{
			m_TRConnectionList.DeleteObject(pConnection->GetID());
		}
	}	

	time_t CurTime = time(NULL);
	for (UINT i = 0; i < m_PeerServerList.GetCount(); i++)
	{
		if (!m_PeerServerList[i].IsReplied)
		{
			if (CurTime - m_PeerServerList[i].RequestTime > SEARCH_TIME_OUT)
			{
				m_PeerServerList[i].RequestTime = CurTime;
				SendPing(m_PeerServerList[i].Address, i + 1);
			}
		}
	}
	if (!m_NodesFileName.IsEmpty())
	{
		if (m_SaveTimer.IsTimeOut(NODES_SAVE_TIME))
		{
			m_SaveTimer.SaveTime();
			SaveNodes(m_NodesFileName);
		}
	}
	UINT CheckCount = 0;
	UINT CheckHitCount = 0;
	while (CheckCount < PEER_ONCE_CHECK_COUNT && CheckHitCount < PEER_ONCE_CHECK_HIT_COUNT)
	{
		NODE_PEER * pPeer = m_PeerPool.GetObject(m_PeerCheckPtr);
		m_PeerCheckPtr++;
		if (m_PeerCheckPtr>m_PeerPool.GetBufferSize())
			m_PeerCheckPtr = 1;
		CheckCount++;
		if (pPeer)
		{
			CheckHitCount++;
			if (CurTime - pPeer->LastReplyTime > PEER_EXPIRE_TIME)
			{
				if (pPeer->PingCount > MAX_PEER_PING_TEST_COUNT)
				{
					PrintNetLog( _T("Peer[%s]长时间无响应已删除"), pPeer->PeerAddress.GetAddressString());
					DeleteNodePeer(pPeer);
				}
				else if (CurTime - pPeer->LastPingTime > PEER_PING_TIME_COUNT)
				{
					pPeer->LastPingTime = CurTime;
					pPeer->PingCount++;
					SendPing(pPeer->PeerAddress, 0);
					PrintNetLog( _T("对Peer[%s]进行Ping测试"), pPeer->PeerAddress.GetAddressString());
					ProcessCount++;
				}
			}
		}		
	}
	return ProcessCount;
}
void CDHTService::OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize)
{
	CBencodingValue Value;

	Value.Parser(pData, DataSize);
	if (Value.GetType() == BENCODING_TYPE_DICTIONARY)
	{
		CBencodingValue& TID = Value.GetDictValue("t");
		CBencodingValue& MsgType = Value.GetDictValue("y");
		if (MsgType.GetType() == BENCODING_TYPE_STRING)
		{
			if (MsgType.GetStrValue() == "r")
			{
				CBencodingValue& Data = Value.GetDictValue("r");
				ProcessRespond(IPAddress, TID.GetStrValue().GetBuffer(), (UINT)TID.GetStrValue().GetLength(), Data);
			}
			else if (MsgType.GetStrValue() == "q")
			{
				CBencodingValue& RequestType = Value.GetDictValue("q");

				CBencodingValue& Data = Value.GetDictValue("r");

				ProcessRequest(IPAddress, RequestType.GetStrValue(), TID.GetStrValue().GetBuffer(), (UINT)TID.GetStrValue().GetLength(), Data);
			}
			else if (MsgType.GetStrValue() == "e")
			{
				CBencodingValue& Data = Value.GetDictValue("e");
				if (Data.GetType() == BENCODING_TYPE_LIST && Data.GetListCount() >= 2)
				{
					int ErrCode = 0;
					CEasyString ErrMsg;
					if (Data.GetListValue(0).GetType() == BENCODING_TYPE_INTEGER)
						ErrCode = (int)Data.GetListValue(0).GetIntValue();
					if (Data.GetListValue(1).GetType() == BENCODING_TYPE_STRING)
						ErrMsg = Data.GetListValue(1).GetStrValue();
					PrintNetLog( _T("收到错误消息(%d)%s"), ErrCode, ErrMsg);
				}
				else
				{
					PrintNetLog( _T("收到未知格式的错误消息"));
				}
			}
			else
			{
				CEasyString Msg = MsgType.GetStrValue();
				PrintNetLog( _T("收到未知消息类型=%s"), (LPCTSTR)Msg);
			}
		}
	}
}


int CDHTService::UpdateSearch(int ProcessPacketLimit)
{
	int ProcessCount = 0;

	time_t CurTime = time(NULL);
	void * Pos = m_SearchPool.GetFirstObjectPos();
	while (Pos) 
	{
		SEARCH_INFO * pSearchInfo = m_SearchPool.GetNextObject(Pos);

		if (!pSearchInfo->IsFinded)
		{
			for (int i = (int)pSearchInfo->PeerList.GetCount() - 1; i >= 0; i--)
			{
				SEARCH_PEER& PeerInfo = pSearchInfo->PeerList[i];
				if (!PeerInfo.IsReplied)
				{
					if (CurTime - PeerInfo.RequestTime>SEARCH_TIME_OUT)
					{
						if (PeerInfo.RequestCount<MAX_GET_PEERS_TRY_COUNT)
						{
							//PrintNetDebugLog( _T("向%s发送GetPeers请求%u"), PeerInfo.PeerAddress.GetAddressString(), PeerInfo.RequestCount);
							PeerInfo.RequestTime = CurTime;
							PeerInfo.RequestCount++;
							WORD TID = TID_GET_PEERS + pSearchInfo->SearchID;
							SendGetPeers(PeerInfo.PeerAddress, TID, pSearchInfo->InfoHash);
							ProcessCount++;
						}
						else
						{
							PrintNetDebugLog( _T("向%s发送GetPeers请求%u多次无响应，已删除"), 
								PeerInfo.PeerAddress.GetAddressString(), PeerInfo.RequestCount);
							pSearchInfo->PeerList.Delete(i);
						}
					}
				}
				else if (PeerInfo.IsFinded && (!PeerInfo.IsAcked))
				{
					if (CurTime - PeerInfo.RequestTime > SEARCH_TIME_OUT)
					{
						if (PeerInfo.RequestCount < MAX_ANNOUNCE_PEER_TRY_COUNT)
						{
							//PrintNetDebugLog( _T("向%s发送AnnouncePeer请求%u"), PeerInfo.PeerAddress.GetAddressString(), PeerInfo.RequestCount);
							PeerInfo.RequestTime = CurTime;
							PeerInfo.RequestCount++;
							SendAnnouncePeer(PeerInfo.PeerAddress, TID_ANNOUNCE_PEER + pSearchInfo->SearchID, pSearchInfo->InfoHash,
								pSearchInfo->DownloadPort, PeerInfo.Token, PeerInfo.TokenLen);
							ProcessCount++;
						}
						else
						{
							PrintNetDebugLog( _T("向%s发送AnnouncePeer请求%u多次无响应，已删除"), 
								PeerInfo.PeerAddress.GetAddressString(), PeerInfo.RequestCount);
							pSearchInfo->PeerList.Delete(i);
						}
					}
				}

			}

			NODE_ID PeerID;
			memcpy(PeerID.NodeID, "MGT_GET", 7);
			for (int i = (int)pSearchInfo->DataPortAddressList.GetCount() - 1; i >= 0; i--)
			{
				TORRENT_DOWNLOAD_ADDRESS_INFO& Info = pSearchInfo->DataPortAddressList[i];
				if (Info.RequestCount < MAX_TORRENT_DOWNLOAD_TRY_COUNT)
				{
					CTRConnection * pConnection = NULL;
					if (Info.ConnectionID)
					{
						pConnection = m_TRConnectionList.GetObject(Info.ConnectionID);
					}
					if (pConnection == NULL)
					{
						pConnection = m_TRConnectionList.NewObject();
						if (pConnection)
							Info.ConnectionID = pConnection->GetID();
							
					}

					if (pConnection)
					{
						if (pConnection->IsDisconnected())
						{
							Info.RequestCount++;
							if (pConnection->Init(this, pSearchInfo->SearchID, PeerID, pSearchInfo->InfoHash, 
								Info.PeerAddress))
							{
								//PrintNetDebugLog( _T("尝试从%s下载种子%u"),
								//	Info.PeerAddress.GetAddressString(),
								//	Info.RequestCount);
							}
							else
							{
								PrintNetDebugLog( _T("尝试从%s下载种子%u失败"),
									Info.PeerAddress.GetAddressString(),
									Info.RequestCount);
							}
							ProcessCount++;
						}
					}
				}
				else
				{
					PrintNetDebugLog( _T("种子下载%s多次尝试失败，已移除"),
						Info.PeerAddress.GetAddressString(),
						Info.RequestCount);
					pSearchInfo->DataPortAddressList.Delete(i);
				}
				
			}
		}

		if ((CurTime - pSearchInfo->SearchStartTime) > pSearchInfo->SearchTimeout)
		{
			PrintNetDebugLog( _T("搜索%u超时，放弃"), pSearchInfo->SearchID);
			OnSearchTimeout(pSearchInfo->InfoHash, pSearchInfo->Param);
			DeleteSearch(pSearchInfo->SearchID);
		}
	}

	return ProcessCount;
}


DHT_NODE * CDHTService::InsertNode(const NODE_ID& NodeID, const CIPAddress& IPAddress, NODE_FROM_TYPE FromType)
{
	DHT_NODE * pNode = NULL;
	m_NodePool.New(NodeID, &pNode);
	if (pNode)
	{
		if (pNode->AccessCount == 0)
		{
			//PrintNetDebugLog( _T("增加新节点ID=%s,Address=%s"),
			//	(LPCTSTR)BinToString(NodeID.NodeID, NODE_ID_BYTE_COUNT), IPAddress.GetAddressString());

			pNode->NodeID = NodeID;			
		}
		pNode->pPeer = InsertNodePeer(pNode, IPAddress, FromType);
		if (pNode->pPeer == NULL)
		{
			m_NodePool.Delete(NodeID);
		}
		else
		{
			pNode->AccessCount++;
			ShrinkPeerNodes(pNode);
		}
	}
	else
	{
		PrintNetLog( _T("Node池已满%u/%u"), m_NodePool.GetObjectCount(), m_NodePool.GetBufferSize());
	}
	return pNode;
}

NODE_PEER * CDHTService::InsertNodePeer(DHT_NODE * pNode, const CIPAddress& IPAddress, NODE_FROM_TYPE FromType)
{
	NODE_PEER * pPeer = NULL;
	m_PeerPool.New(IPAddress, &pPeer);
	if (pPeer)
	{		
		if (pPeer->AccessCount == 0)
		{
			//PrintNetDebugLog( _T("增加Peer,Address=%s"), IPAddress.GetAddressString());
			pPeer->PeerAddress = IPAddress;
		}
		
		pPeer->NodeList.Insert(pNode->NodeID, pNode);
		switch (FromType)
		{
		case NODE_FROM_TYPE_REPLAY:
			pPeer->LastReplyTime = time(NULL);
		case NODE_FROM_TYPE_REQUEST:
			pPeer->LastRecvTime = time(NULL);
			break;
		}
		pPeer->AccessCount++;
	}
	else
	{
		PrintNetLog( _T("Peer池已满%u/%u"), m_PeerPool.GetObjectCount(), m_PeerPool.GetBufferSize());
	}
	return pPeer;
}

DHT_NODE * CDHTService::FindNode(const NODE_ID& NodeID, bool MustEqual)
{
	if (MustEqual)
		return m_NodePool.Find(NodeID);
	else
		return m_NodePool.FindNear(NodeID);
}

NODE_PEER * CDHTService::FindPeer(const CIPAddress& IPAddress)
{
	return m_PeerPool.Find(IPAddress);
}

void CDHTService::ShrinkPeerNodes(DHT_NODE * pNode)
{
	NODE_PEER * pPeer = pNode->pPeer;
	if (pPeer->NodeList.GetObjectCount() >= MAX_PEER_NODE_COUNT)
	{
		//同一个Peer的Node达到上限后进行减半收缩
		PrintNetDebugLog( _T("Peer(%s)的节点过多，开始收缩"), pPeer->PeerAddress.GetAddressString());
		UINT Count = pPeer->NodeList.GetObjectCount() + 32;
		void * Pos = pPeer->NodeList.GetSortedFirstObjectPos();
		bool NeedDelete = false;
		while (Pos)
		{
			void * ThisPos = Pos;
			DHT_NODE ** ppNode = pPeer->NodeList.GetSortedNextObject(Pos);
			if (NeedDelete && ((*ppNode) != pNode))
			{
				m_NodePool.Delete((*ppNode)->NodeID);
				(*ppNode)->Clear();
				pPeer->NodeList.DeleteByPos(ThisPos);
			}
			NeedDelete = !NeedDelete;
			Count--;
			if (Count <= 0)
			{
				PrintNetLog( _T("ShrinkPeerNodes遍历异常"));
				break;
			}
		}
	}
}

void CDHTService::DeleteNodePeer(NODE_PEER * pPeer)
{
	void * Pos = pPeer->NodeList.GetFirstObjectPos();
	while (Pos)
	{
		DHT_NODE ** ppNode = pPeer->NodeList.GetNextObject(Pos);
		if (ppNode)
		{
			m_NodePool.Delete((*ppNode)->NodeID);
		}
	}
	m_PeerPool.Delete(pPeer->PeerAddress);
}


SEARCH_PEER * CDHTService::FindSearchPeer(SEARCH_INFO * pSearchInfo, const CIPAddress& PeerAddress)
{
	for (UINT i = 0; i < pSearchInfo->PeerList.GetCount(); i++)
	{
		if (pSearchInfo->PeerList[i].PeerAddress == PeerAddress)
			return pSearchInfo->PeerList.GetObject(i);
	}
	return NULL;
}
SEARCH_PEER * CDHTService::AddSearchPeer(SEARCH_INFO * pSearchInfo, DHT_NODE * pNode)
{
	SEARCH_PEER * pSearchPeer = FindSearchPeer(pSearchInfo, pNode->pPeer->PeerAddress);
	if (pSearchPeer == NULL)
	{
		pSearchPeer = pSearchInfo->PeerList.AddEmpty();
		pSearchPeer->PeerAddress = pNode->pPeer->PeerAddress;
		//PrintNetDebugLog( _T("为%s增加搜索Peer,ID=%s,Address=%s"),
		//	(LPCTSTR)BinToString(pSearchInfo->InfoHash.NodeID, NODE_ID_BYTE_COUNT),
		//	(LPCTSTR)BinToString(pNode->NodeID.NodeID, NODE_ID_BYTE_COUNT), pSearchPeer->PeerAddress.GetAddressString());
	}	
	pSearchPeer->NodeIDList.Add(pNode->NodeID);	
	
	return pSearchPeer;
}
void CDHTService::AddSearchPeers(SEARCH_INFO * pSearchInfo)
{	
	void * Pos = m_NodePool.FindNearPos(pSearchInfo->InfoHash);
	if (Pos)
	{
		void * FrontPos = Pos;
		void * BackPos = Pos;

		DHT_NODE * pNode = m_NodePool.GetSortedPrevObject(FrontPos);
		m_NodePool.GetSortedNextObject(BackPos);
		AddSearchPeer(pSearchInfo, pNode);
		UINT SearchCount = 0;
		while (pSearchInfo->PeerList.GetCount() < START_SEARCH_PEER_COUNT && (FrontPos || BackPos))
		{
			if (FrontPos)
			{
				pNode = m_NodePool.GetSortedPrevObject(FrontPos);
				AddSearchPeer(pSearchInfo, pNode);
			}
			if (BackPos)
			{
				pNode = m_NodePool.GetSortedPrevObject(BackPos);
				AddSearchPeer(pSearchInfo, pNode);
			}
			SearchCount++;
			if (SearchCount > m_NodePool.GetObjectCount())
			{
				PrintNetLog( _T("添加搜索节点异常，可能死循环了"));
			}
		}
	}
}

void CDHTService::AddPackedNodes(const BYTE * pPackedNode, UINT DataSize, const CIPAddress& FromIPAddress, SEARCH_INFO * pSearchInfo)
{
	if (FromIPAddress.IsIPv4())
	{
		DataSize = (DataSize / 26) * 26;
		for (UINT i = 0; i < DataSize; i += 26)
		{
			NODE_ID NodeID;
			CIPAddress NodeIPAddress;
			memcpy(NodeID.NodeID, pPackedNode + i, NODE_ID_BYTE_COUNT);
			NodeIPAddress.SetIPv4(pPackedNode + 20);
			NodeIPAddress.SetPort(ntohs(*((WORD *)(pPackedNode + 24))));
			DHT_NODE * pNode = InsertNode(NodeID, NodeIPAddress, NODE_FROM_TYPE_NEW);
			if (pSearchInfo&&pNode)
				AddSearchPeer(pSearchInfo, pNode);
		}
	}
	else
	{
		DataSize = (DataSize / 38) * 38;
		for (UINT i = 0; i < DataSize; i += 38)
		{
			NODE_ID NodeID;
			CIPAddress NodeIPAddress;
			memcpy(NodeID.NodeID, pPackedNode + i, NODE_ID_BYTE_COUNT);
			NodeIPAddress.SetIPv6(pPackedNode + 20);
			NodeIPAddress.SetPort(ntohs(*((WORD *)(pPackedNode + 36))));
			DHT_NODE * pNode = InsertNode(NodeID, NodeIPAddress, NODE_FROM_TYPE_NEW);
			if (pSearchInfo&&pNode)
				AddSearchPeer(pSearchInfo, pNode);
		}
	}
}

void CDHTService::ProcessRequest(const CIPAddress& IPAddress, LPCSTR RequestType, LPCVOID TID, UINT TIDLen, const CBencodingValue& Data)
{

}
void CDHTService::ProcessRespond(const CIPAddress& IPAddress, LPCVOID pTID, UINT TIDLen, const CBencodingValue& Data)
{
	if (TIDLen >= 2)
	{
		WORD TID = *((WORD *)pTID);
		if (TID >= TID_PING && TID < TID_FIND_NODE)
		{
			if (Data.HaveDictValue("id"))
			{
				const CBencodingValue& ID = Data.GetDictValue("id");
				NODE_ID NodeID;
				memcpy(NodeID.NodeID, ID.GetStrValue().GetBuffer(), min((UINT)ID.GetStrValue().GetLength(), (UINT)NODE_ID_BYTE_COUNT));
				OnNodePong(IPAddress, NodeID, TID - TID_PING);
			}
		}
		else if (TID == TID_FIND_NODE)
		{

		}
		else if (TID >= TID_ANNOUNCE_PEER && TID < TID_GET_PEERS)
		{
			if (Data.HaveDictValue("id"))
			{
				const CBencodingValue& ID = Data.GetDictValue("id");
				NODE_ID NodeID;
				memcpy(NodeID.NodeID, ID.GetStrValue().GetBuffer(), min((UINT)ID.GetStrValue().GetLength(), (UINT)NODE_ID_BYTE_COUNT));
				OnAnnouncePeerRespond(IPAddress, NodeID, TID - TID_ANNOUNCE_PEER);
			}
		}
		else if (TID >= TID_GET_PEERS)
		{
			OnGetPeersRespond(IPAddress, TID - TID_GET_PEERS, Data);			
		}
	}
}

void CDHTService::OnNodePong(const CIPAddress& IPAddress, const NODE_ID& NodeID, WORD ID)
{	
	PrintNetLog( _T("收到Peer[%s]Pong"), IPAddress.GetAddressString());
	DHT_NODE * pNode = InsertNode(NodeID, IPAddress, NODE_FROM_TYPE_REPLAY);
	if (ID > 0)
	{
		ID--;
		if (ID < m_PeerServerList.GetCount())
		{
			m_PeerServerList[ID].IsReplied = true;
			m_PeerServerList[ID].NodeID = NodeID;
		}
	}	
}
void CDHTService::OnGetPeersRespond(const CIPAddress& IPAddress, UINT SearchID, const CBencodingValue& Data)
{
	SEARCH_INFO * pSearchInfo = GetSearch(SearchID);
	if (pSearchInfo)
	{
		SEARCH_PEER * pSearchPeer = FindSearchPeer(pSearchInfo, IPAddress);
		DHT_NODE * pNode = NULL;
		NODE_ID NodeID;
		if (Data.HaveDictValue("id"))
		{
			const CBencodingValue& ID = Data.GetDictValue("id");			
			memcpy(NodeID.NodeID, ID.GetStrValue().GetBuffer(), min((UINT)ID.GetStrValue().GetLength(), (UINT)NODE_ID_BYTE_COUNT));
			pNode = InsertNode(NodeID, IPAddress, NODE_FROM_TYPE_REPLAY);			
		}		
		if (pSearchPeer)
		{
			pSearchPeer->IsReplied = true;
			pSearchPeer->ReplyTime = time(NULL);
			if (Data.HaveDictValue("token"))
			{
				const CBencodingValue& Token = Data.GetDictValue("token");
				if (Token.GetStrValue().GetLength() <= MAX_TOKEN_LEN)
				{
					pSearchPeer->TokenLen = (UINT)Token.GetStrValue().GetLength();
					memcpy(pSearchPeer->Token, Token.GetStrValue().GetBuffer(), pSearchPeer->TokenLen);
				}
			}
		}
		else
		{
			PrintNetLog( _T("搜索到了目标，但没有对应搜索Peer(%s)"),
				IPAddress.GetAddressString());
		}
		if (Data.HaveDictValue("values"))
		{
			const CBencodingValue& Values = Data.GetDictValue("values");

			if (pSearchPeer)
			{
				pSearchPeer->IsFinded = true;
				pSearchPeer->RequestTime = 0;
				pSearchPeer->RequestCount = 0;
				if (Data.HaveDictValue("token"))
				{
					const CBencodingValue& Token = Data.GetDictValue("token");
					pSearchPeer->TokenLen = (UINT)Token.GetStrValue().GetLength();
					memcpy(pSearchPeer->Token, Token.GetStrValue().GetBuffer(), pSearchPeer->TokenLen);					
				}
				for (UINT i = 0; i < Values.GetListCount();i++)
				{
					const CBencodingValue& Value = Values.GetListValue(i);
					if (IPAddress.IsIPv4())
					{
						BYTE * pData = (BYTE *)Value.GetStrValue().GetBuffer();
						UINT DataSize = (UINT)Value.GetStrValue().GetLength();
						if (DataSize>=6)
						{
							CIPAddress * pAddress = pSearchPeer->DataPortAddressList.AddEmpty();
							pAddress->SetIPv4(pData);
							pAddress->SetPort(ntohs(*((WORD *)(pData + 4))));
							
							//PrintNetDebugLog( _T("已找到种子下载地址%s"), pAddress->GetAddressString());
						}
					}
					else
					{
						BYTE * pData = (BYTE *)Value.GetStrValue().GetBuffer();
						UINT DataSize = (UINT)Value.GetStrValue().GetLength();
						if (DataSize >= 18)
						{
							CIPAddress * pAddress = pSearchPeer->DataPortAddressList.AddEmpty();
							pAddress->SetIPv6(pData);
							pAddress->SetPort(ntohs(*((WORD *)(pData + 16))));
							
							//PrintNetDebugLog( _T("已找到种子下载地址%s"), pAddress->GetAddressString());
						}
					}
				}
			}
		}

		if (Data.HaveDictValue("nodes"))
		{
			const CBencodingValue& Nodes = Data.GetDictValue("nodes");
			AddPackedNodes((BYTE *)Nodes.GetStrValue().GetBuffer(), (UINT)Nodes.GetStrValue().GetLength(), IPAddress, pSearchInfo);
		}
	}
}

void CDHTService::OnAnnouncePeerRespond(const CIPAddress& IPAddress, const NODE_ID& NodeID, WORD SearchID)
{
	InsertNode(NodeID, IPAddress, NODE_FROM_TYPE_REPLAY);
	SEARCH_INFO * pSearchInfo = GetSearch(SearchID);
	if (pSearchInfo)
	{
		SEARCH_PEER * pSearchPeer = FindSearchPeer(pSearchInfo, IPAddress);
		if (pSearchPeer)
		{
			pSearchPeer->IsAcked = true;
			for (UINT i = 0; i < pSearchPeer->DataPortAddressList.GetCount(); i++)
			{
				CIPAddress& DataPortAddress = pSearchPeer->DataPortAddressList[i];
				bool IsExist = false;
				for (UINT j = 0; j < pSearchInfo->DataPortAddressList.GetCount(); j++)
				{
					if (pSearchInfo->DataPortAddressList[j].PeerAddress == DataPortAddress)
					{
						IsExist = true;
						break;
					}
				}
				if (!IsExist)
				{
					TORRENT_DOWNLOAD_ADDRESS_INFO * pAddrInfo = pSearchInfo->DataPortAddressList.AddEmpty();
					pAddrInfo->PeerAddress = DataPortAddress;					
					//PrintNetLog( _T("已添加种子下载地址(%s)一共%u个"), 
					//	DataPortAddress.GetAddressString(), pSearchInfo->DataPortAddressList.GetCount());
				}
			}			
		}
		else
		{
			PrintNetLog( _T("收到AnnouncePeer回应，但找不到Peer(%s)"), IPAddress.GetAddressString());
		}
	}
	else
	{
		PrintNetLog( _T("收到AnnouncePeer回应，但找不到搜索项(%d)"), SearchID);
	}
}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////
void CDHTService::SendRequest(const CIPAddress& IPAddress, LPCSTR Type, LPCVOID TID, UINT TIDLen, char * pData, UINT DataLen)
{
	char * pBuff = (char *)m_SendBuffer1.GetBuffer();
	UINT BuffSize = m_SendBuffer1.GetBufferSize();
	UINT BuffPtr = 0;
	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d1:a");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, pData, DataLen);
	BuffPtr += DataLen;
	BuffSize -= DataLen;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "1:q%u:%s1:t%u:", (UINT)strlen(Type), Type, TIDLen);
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, TID, TIDLen);
	BuffPtr += TIDLen;
	BuffSize -= TIDLen;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "1:y1:qe");
	BuffPtr += Len;
	BuffSize -= Len;
	QueryUDPSend(IPAddress, m_SendBuffer1.GetBuffer(), BuffPtr);
}
void CDHTService::SendRespond(const CIPAddress& IPAddress, LPCVOID TID, UINT TIDLen, char * pData, UINT DataLen)
{
	char * pBuff = (char *)m_SendBuffer1.GetBuffer();
	UINT BuffSize = m_SendBuffer1.GetBufferSize();
	UINT BuffPtr = 0;
	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d1:r");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, pData, DataLen);
	BuffPtr += DataLen;
	BuffSize -= DataLen;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "1:t%u:", TIDLen);
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, TID, TIDLen);
	BuffPtr += TIDLen;
	BuffSize -= TIDLen;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "1:y1:re");
	BuffPtr += Len;
	BuffSize -= Len;
	QueryUDPSend(IPAddress, m_SendBuffer1.GetBuffer(), BuffPtr);
}
void CDHTService::SendPing(const CIPAddress& IPAddress, WORD ID)
{
	WORD TID = TID_PING + ID;
	char * pBuff = (char *)m_SendBuffer2.GetBuffer();
	UINT BuffSize = m_SendBuffer2.GetBufferSize();
	UINT BuffPtr = 0;
	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d2:id20:"); 
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, m_NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "e");
	BuffPtr += Len;
	BuffSize -= Len;
	SendRequest(IPAddress, "ping", &TID, 2, (char *)m_SendBuffer2.GetBuffer(), BuffPtr);
}
void CDHTService::SendPong(const CIPAddress& IPAddress, LPCVOID TID, UINT TIDLen)
{
	char * pBuff = (char *)m_SendBuffer2.GetBuffer();
	UINT BuffSize = m_SendBuffer2.GetBufferSize();
	UINT BuffPtr = 0;

	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d2:id20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, m_NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;
	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "e");
	BuffPtr += Len;
	BuffSize -= Len;
	SendRespond(IPAddress, TID, TIDLen, (char *)m_SendBuffer2.GetBuffer(), BuffPtr);
}

void CDHTService::SendFindNode(const CIPAddress& IPAddress, const NODE_ID& NodeID)
{
	WORD TID = TID_PING;
	char * pBuff = (char *)m_SendBuffer2.GetBuffer();
	UINT BuffSize = m_SendBuffer2.GetBufferSize();
	UINT BuffPtr = 0;

	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d2:id20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, m_NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "6:target20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "e");
	BuffPtr += Len;
	BuffSize -= Len;
	SendRequest(IPAddress, "find_node", &TID, 2, (char *)m_SendBuffer2.GetBuffer(), BuffPtr);
}

void CDHTService::SendGetPeers(const CIPAddress& IPAddress, WORD TID, const NODE_ID& InfoHash)
{
	char * pBuff = (char *)m_SendBuffer2.GetBuffer();
	UINT BuffSize = m_SendBuffer2.GetBufferSize();
	UINT BuffPtr = 0;
	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d2:id20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, m_NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "9:info_hash20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, InfoHash.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "e");
	BuffPtr += Len;
	BuffSize -= Len;
	SendRequest(IPAddress, "get_peers", &TID, 2, (char *)m_SendBuffer2.GetBuffer(), BuffPtr);
}

void CDHTService::SendAnnouncePeer(const CIPAddress& IPAddress, WORD TID, const NODE_ID& InfoHash, WORD DownloadPort, const BYTE * pToken, UINT TolenLen)
{
	char * pBuff = (char *)m_SendBuffer2.GetBuffer();
	UINT BuffSize = m_SendBuffer2.GetBufferSize();
	UINT BuffPtr = 0;
	int Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "d2:id20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, m_NodeID.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "9:info_hash20:");
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, InfoHash.NodeID, NODE_ID_BYTE_COUNT);
	BuffPtr += NODE_ID_BYTE_COUNT;
	BuffSize -= NODE_ID_BYTE_COUNT;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "4:porti%ue", DownloadPort);
	BuffPtr += Len;
	BuffSize -= Len;

	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "5:token%u:", TolenLen);
	BuffPtr += Len;
	BuffSize -= Len;
	memcpy_s(pBuff + BuffPtr, BuffSize, pToken, TolenLen);
	BuffPtr += TolenLen;
	BuffSize -= TolenLen;


	Len = _snprintf_s(pBuff + BuffPtr, BuffSize, BuffSize, "e");
	BuffPtr += Len;
	BuffSize -= Len;

	SendRequest(IPAddress, "announce_peer", &TID, 2, (char *)m_SendBuffer2.GetBuffer(), BuffPtr);
}