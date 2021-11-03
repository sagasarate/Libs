/****************************************************************************/
/*                                                                          */
/*      文件名:    CENLConnectionNoBuff.h                                     */
/*      创建日期:  2016年04月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CENLConnectionNoBuff :
	public CENLBaseConnection,
	public CNetConnectionNoBuff	
{
protected:
	enum STATUS
	{
		STATUS_ACCEPTING,
		STATUS_ACCEPTED,
	};
	CDOSRouterLinkManager *						m_pManager;
	STATUS										m_Status;
	CIPAddress									m_ConnectionAddress;
	CEasyTimer									m_KeepAliveTimer;
	int											m_KeepAliveCount;
	CEasyTimer									m_ReconnectTimer;

	UINT										m_MaxPacketSize;
	CEasyNetLinkManager::DATA_COMPRESS_TYPE		m_DataCompressType;
	UINT										m_MinCompressSize;
public:
	CENLConnectionNoBuff();
	~CENLConnectionNoBuff();

	virtual void Release() override;
	virtual UINT AddUseRef() override;

	bool Init(CDOSRouterLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress,
		UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize);
	bool Init(CDOSRouterLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize,
		CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize);
	

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;
	virtual bool AllocRecvBuffer(const BYTE * pPacketHeader, UINT HeaderSize, LPBYTE& pBuffer, UINT& BufferSize, LPCVOID& Param) override;
	virtual void ReleaseRecvBuffer(BYTE * pBuffer, LPCVOID Param) override;
	virtual void ReleaseSendBuffer(LPCVOID pBuffer, LPCVOID Param) override;
	virtual void OnRecvData(const BYTE * pData, UINT DataSize, LPCVOID Param) override;
	virtual void OnConnection(bool IsSucceed) override;
	virtual void OnDisconnection() override;

	void SendLinkMsg(DWORD MsgID, LPCVOID pData = NULL, UINT DataSize = 0);
	bool SendPacket(CDOSMessagePacket * pPacket);

	virtual bool SendData(LPCVOID pData, UINT DataSize)
	{
		return false;
	}

	virtual void Disconnect() override;
	virtual bool IsDisconnected() override;
	virtual bool IsConnected() override;
	virtual CBaseNetConnection * GetBaseConnection() override;
};

