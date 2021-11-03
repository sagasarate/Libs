/****************************************************************************/
/*                                                                          */
/*      文件名:    CENLConnection.h                                     */
/*      创建日期:  2016年04月08日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CENLConnection :
	public CENLBaseConnection,
	public CNetConnection	
{
protected:
	enum STATUS
	{
		STATUS_ACCEPTING,
		STATUS_ACCEPTED,
	};
	CEasyNetLinkManager *						m_pManager;
	STATUS										m_Status;
	CIPAddress									m_ConnectionAddress;
	CEasyBuffer									m_AssembleBuffer;
	CEasyTimer									m_KeepAliveTimer;
	int											m_KeepAliveCount;
	CEasyTimer									m_ReconnectTimer;

	UINT										m_MaxPacketSize;
	CEasyNetLinkManager::DATA_COMPRESS_TYPE		m_DataCompressType;
	UINT										m_MinCompressSize;
	CEasyBuffer									m_CompressBuffer;

public:
	CENLConnection();
	~CENLConnection();

	virtual void Release() override;
	virtual UINT AddUseRef() override;

	bool Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress, 
		UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize);
	bool Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize, 
		CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize);
	

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;
	virtual void OnRecvData(const BYTE * pData, UINT DataSize) override;
	virtual void OnConnection(bool IsSucceed) override;
	virtual void OnDisconnection() override;

	virtual bool SendData(LPCVOID pData, UINT DataSize) override;

	virtual void Disconnect() override;
	virtual bool IsDisconnected() override;
	virtual bool IsConnected() override;
	virtual CBaseNetConnection * GetBaseConnection() override;

protected:
	void SendLinkMsg(DWORD MsgID, LPCVOID pData = NULL, UINT DataSize = 0);
};

