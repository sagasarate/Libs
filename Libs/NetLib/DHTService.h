#pragma once
class CDHTService :
	public CNetService
{
protected:
	NODE_ID								m_NodeID;
	CStaticMap<NODE_ID, DHT_NODE>		m_NodePool;	
	CStaticMap<CIPAddress, NODE_PEER>	m_PeerPool;
	CEasyBuffer							m_SendBuffer1;
	CEasyBuffer							m_SendBuffer2;

	CStaticMap<NODE_ID, SEARCH_INFO>	m_SearchPool;

	CIDStorage<CTRConnection>			m_TRConnectionList;
	CEasyArray<PEER_SERVER_INFO>		m_PeerServerList;

	CEasyString							m_NodesFileName;
	CEasyTimer							m_SaveTimer;

	UINT								m_PeerCheckPtr;
public:
	CDHTService();
	~CDHTService();

	bool Init(CNetServer * pServer, const CIPAddress& ListenAddress, const NODE_ID& NodeID);
	void AddPeerServer(const CIPAddress& NodeAddress);
	UINT AddSearch(const NODE_ID& InfoHash, WORD DownloadPort, UINT Timeout, UINT64 Param);
	SEARCH_INFO * GetSearch(UINT ID);
	SEARCH_INFO * FindSearch(const NODE_ID& InfoHash);
	bool DeleteSearch(UINT SearchID);
	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);

	bool LoadNodes(LPCTSTR FileName);
	bool SaveNodes(LPCTSTR FileName);


	void OnTorrent(UINT SearchID, const BYTE * pTorrentData, UINT DataSize);
	virtual void OnSearchFinish(const NODE_ID& InfoHash, UINT64 Param, const BYTE * pTorrentData, UINT DataSize);
	virtual void OnSearchTimeout(const NODE_ID& InfoHash, UINT64 Param);
protected:
	
	virtual void OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize);

	int UpdateSearch(int ProcessPacketLimit);


	DHT_NODE * InsertNode(const NODE_ID& NodeID, const CIPAddress& IPAddress, NODE_FROM_TYPE FromType);
	NODE_PEER * InsertNodePeer(DHT_NODE * pNode, const CIPAddress& IPAddress, NODE_FROM_TYPE FromType);

	DHT_NODE * FindNode(const NODE_ID& NodeID, bool MustEqual = true);
	NODE_PEER * FindPeer(const CIPAddress& IPAddress);
	void ShrinkPeerNodes(DHT_NODE * pNode);
	
	void DeleteNodePeer(NODE_PEER * pPeer);

	SEARCH_PEER * FindSearchPeer(SEARCH_INFO * pSearchInfo, const CIPAddress& PeerAddress);
	SEARCH_PEER * AddSearchPeer(SEARCH_INFO * pSearchInfo, DHT_NODE * pNode);
	void AddSearchPeers(SEARCH_INFO * pSearchInfo);
	void AddPackedNodes(const BYTE * pPackedNode, UINT DataSize, const CIPAddress& FromIPAddress, SEARCH_INFO * pSearchInfo);


	void ProcessRequest(const CIPAddress& IPAddress, LPCSTR RequestType, LPCVOID TID, UINT TIDLen, const CBencodingValue& Data);
	void ProcessRespond(const CIPAddress& IPAddress, LPCVOID pTID, UINT TIDLen, const CBencodingValue& Data);

	void OnNodePong(const CIPAddress& IPAddress, const NODE_ID& NodeID, WORD ID);
	void OnGetPeersRespond(const CIPAddress& IPAddress, UINT SearchID, const CBencodingValue& Data);
	void OnAnnouncePeerRespond(const CIPAddress& IPAddress, const NODE_ID& NodeID, WORD SearchID);

	void SendRequest(const CIPAddress& IPAddress, LPCSTR Type, LPCVOID TID, UINT TIDLen, char * pData, UINT DataLen);
	void SendRespond(const CIPAddress& IPAddress, LPCVOID TID, UINT TIDLen, char * pData, UINT DataLen);
	void SendPing(const CIPAddress& IPAddress, WORD ID);
	void SendPong(const CIPAddress& IPAddress, LPCVOID TID, UINT TIDLen);
	void SendFindNode(const CIPAddress& IPAddress, const NODE_ID& NodeID);
	void SendGetPeers(const CIPAddress& IPAddress, WORD TID, const NODE_ID& InfoHash);
	void SendAnnouncePeer(const CIPAddress& IPAddress, WORD TID, const NODE_ID& InfoHash, WORD DownloadPort, const BYTE * pToken, UINT TolenLen);
};

