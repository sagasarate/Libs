#pragma once

class CDHTService;
class CTRConnection :
	public CNetConnection
{
protected:
	CDHTService *	m_pParent;
	UINT			m_SearchID;
	NODE_ID			m_PeerID;
	NODE_ID			m_InfoHash;
	NODE_ID			m_RemotePeerID;
	int				m_FinishHandshake;
	CEasyBuffer		m_AssembleBuffer;
	CEasyBuffer		m_SendBuffer;
	int				m_MetaDataMsgID;
	UINT			m_MetaDataSize;
	CEasyBuffer		m_MetaDataBuffer;
	UINT			m_CurMetaDataPiece;
	CEasyString		m_LogTag;
public:
	CTRConnection();
	~CTRConnection();

	bool Init(CDHTService * pParent, UINT SearchID, const NODE_ID& PeerID, const NODE_ID& InfoHash, const CIPAddress& Address);

	virtual void OnConnection(bool IsSucceed);
	virtual void OnDisconnection();

	virtual void OnRecvData(const BYTE * pData, UINT DataSize);

	UINT GetSearchID()
	{
		return m_SearchID;
	}

protected:
	bool SendHandshake1();
	bool SendHandshake2();
	bool SendRequest(UINT Piece);
	void OnMsg(BYTE * pData, UINT DataSize);
};


