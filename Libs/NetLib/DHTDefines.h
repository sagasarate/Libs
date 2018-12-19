#pragma once


#define NODE_ID_BYTE_COUNT		20
#define MAX_PEER_NODE_COUNT		512
#define SEARCH_TIME_OUT			20
#define MAX_TOKEN_LEN			40
#define NODES_SAVE_TIME			(60*1000)
#define PEER_EXPIRE_TIME				5*60*60
#define PEER_PING_TIME_COUNT			20
#define MAX_PEER_PING_TEST_COUNT		5
#define MAX_TORRENT_DOWNLOAD_TRY_COUNT	5
#define MAX_GET_PEERS_TRY_COUNT			20
#define MAX_ANNOUNCE_PEER_TRY_COUNT		20
#define START_SEARCH_PEER_COUNT			10
#define MAX_TORRENT_SIZE				(10*1024*1024)
#define CUR_NODES_FILE_VERSION			1
#define PEER_ONCE_CHECK_COUNT			2000
#define PEER_ONCE_CHECK_HIT_COUNT		100

enum NODE_FROM_TYPE
{
	NODE_FROM_TYPE_NEW,
	NODE_FROM_TYPE_REQUEST,
	NODE_FROM_TYPE_REPLAY,
};

enum TRANSACTION_ID
{
	TID_PING = 0x0101,
	TID_FIND_NODE = 0x0201,
	TID_ANNOUNCE_PEER = 0x0301,
	TID_GET_PEERS=0x1001,
};


inline int CompareNodeID(const void * p1, const void * p2);


struct NODE_ID
{
	BYTE			NodeID[NODE_ID_BYTE_COUNT];
	void Clear()
	{
		ZeroMemory(NodeID, sizeof(NodeID));
	}

	bool operator ==(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) == 0;
	}

	bool operator !=(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) != 0;
	}

	bool operator >(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) > 0;
	}

	bool operator <(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) < 0;
	}
	bool operator >=(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) >= 0;
	}

	bool operator <=(const NODE_ID& TargetNodeID) const
	{
		return CompareNodeID(this, &TargetNodeID) <= 0;
	}
};
struct NODE_PEER;

struct DHT_NODE 
{
	NODE_ID			NodeID;
	NODE_PEER *		pPeer;	
	UINT			AccessCount;

	DHT_NODE()
	{
		Clear();
	}
	void Clear()
	{
		NodeID.Clear();
		pPeer = NULL;
		AccessCount = 0;
	}
};

struct NODE_PEER
{
	CIPAddress							PeerAddress;
	time_t								LastRecvTime;                /* time of last message received */
	time_t								LastReplyTime;          /* time of last correct reply received */
	time_t								LastPingTime;         /* time of last request */
	UINT								PingCount;                 /* how many requests we sent since last reply */
	UINT								AccessCount;
	CStaticMap<NODE_ID, DHT_NODE *>		NodeList;

	NODE_PEER()
	{
		NodeList.Create(MAX_PEER_NODE_COUNT, MAX_PEER_NODE_COUNT, 16);
		Clear();
	}
	void Clear()
	{		
		PeerAddress.Clear();
		LastRecvTime = 0;
		LastReplyTime = 0;
		LastPingTime = 0;
		PingCount = 0;
		AccessCount = 0;
		NodeList.Clear();
	}
};

struct SEARCH_PEER 
{
	CIPAddress				PeerAddress;	
	CEasyArray<NODE_ID>		NodeIDList;
	UINT					RequestCount;
	time_t					RequestTime;        /* the time of the last unanswered request */
	time_t					ReplyTime;          /* the time of the last reply */
	//int pinged;
	BYTE					Token[MAX_TOKEN_LEN];
	UINT					TokenLen;
	CEasyArray<CIPAddress>	DataPortAddressList;
	bool					IsReplied;                /* whether we have received a reply */
	bool					IsFinded;
	bool					IsAcked;                  /* whether they acked our announcement */
	SEARCH_PEER()
	{
		Clear();
	}
	void Clear()
	{
		NodeIDList.Clear();
		PeerAddress.Clear();
		RequestCount = 0;
		RequestTime = 0;
		ReplyTime = 0;
		ZeroMemory(Token, sizeof(Token));
		TokenLen = 0;
		IsReplied = false;
		IsFinded = false;
		IsAcked = false;
		NodeIDList.Create(16, 16);
		DataPortAddressList.Create(16, 16);
	}
};

struct TORRENT_DOWNLOAD_ADDRESS_INFO
{
	CIPAddress				PeerAddress;
	UINT					RequestCount;
	UINT					ConnectionID;
	TORRENT_DOWNLOAD_ADDRESS_INFO()
	{
		RequestCount = 0;
		ConnectionID = 0;
	}
};


struct SEARCH_INFO
{
	UINT										SearchID;
	UINT64										Param;
	UINT										SearchTimeout;
	NODE_ID										InfoHash;
	bool										IsFinded;
	CEasyArray<SEARCH_PEER>						PeerList;
	CEasyArray<TORRENT_DOWNLOAD_ADDRESS_INFO>	DataPortAddressList;
	WORD										DownloadPort;
	time_t										SearchStartTime;
	SEARCH_INFO()
	{
		Clear();
	}
	void Clear()
	{
		SearchID = 0;
		Param = 0;
		SearchTimeout = -1;
		InfoHash.Clear();
		IsFinded = false;
		DownloadPort = 0;
		PeerList.Create(16, 16);
		DataPortAddressList.Create(16, 16);
		SearchStartTime = 0;
	}
};

struct PEER_SERVER_INFO
{
	NODE_ID					NodeID;
	CIPAddress				Address;
	bool					IsReplied;
	time_t					RequestTime;
	UINT					RequestCount;
	PEER_SERVER_INFO()
	{
		IsReplied = false;
		RequestTime = 0;
		RequestCount = 0;
	}
};


inline int CompareNodeID(const void * p1, const void * p2)
{
	NODE_ID * pNode1 = (NODE_ID *)p1;
	NODE_ID * pNode2 = (NODE_ID *)p2;

	return memcmp(pNode1->NodeID, pNode2->NodeID, NODE_ID_BYTE_COUNT);
}

inline int CompareNodeDistance(const NODE_ID * pNode1, const NODE_ID * pNode2, const NODE_ID * pRefNode)
{
	for (UINT i = 0; i < 20; i++) 
	{
		BYTE xor1, xor2;
		if (pNode1->NodeID[i] == pNode2->NodeID[i])
			continue;
		xor1 = pNode1->NodeID[i] ^ pRefNode->NodeID[i];
		xor2 = pNode2->NodeID[i] ^ pRefNode->NodeID[i];
		if (xor1 < xor2)
			return -1;
		else
			return 1;
	}
	return 0;
}