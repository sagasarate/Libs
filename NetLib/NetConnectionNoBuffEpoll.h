/****************************************************************************/
/*                                                                          */
/*      文件名:    NetConnectionEpoll.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#define NO_CONNECTION_TIME_OUT		0


class CNetService;

class CNetConnectionNoBuff :
	public CBaseNetConnection,public IEpollEventHandler
{
protected:
	struct SEND_BUFFER
	{
		LPCBYTE	pData;
		UINT	DataSize;
		UINT	SendedSize;
		LPCVOID	Param;
	};
	CNetServer*									m_pServer;
	volatile bool								m_WantClose;
	volatile bool								m_IsEventProcessing;
	volatile bool								m_HaveDataToRecv;
	int											m_CurAddressFamily;
	bool										m_UseSafeDisconnect;
	CEasyBuffer									m_PacketHeaderBuffer;
	CCycleQueue<SEND_BUFFER>					m_SendQueue;
	BYTE *										m_pRecvBuffer;
	const void *								m_RecvBufferParam;
	UINT										m_RecvBufferSize;
	UINT										m_RecvedDataSize;
	CEpollEventRouter *							m_pEpollEventRouter;

	CEasyCriticalSection						m_SendLock;



	DECLARE_CLASS_INFO_STATIC(CNetConnectionNoBuff);
public:
	CNetConnectionNoBuff(void);
	virtual ~CNetConnectionNoBuff(void);

	virtual bool OnEpollEvent(UINT EventID) override;

	virtual bool Create(UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE) override;
	virtual bool Create(SOCKET Socket, UINT RecvQueueSize = DEFAULT_SERVER_RECV_DATA_QUEUE,
		UINT SendQueueSize = DEFAULT_SERVER_SEND_DATA_QUEUE) override;
	virtual void Destory() override;
	void Close();


	bool Connect(const CIPAddress& Address, DWORD TimeOut = NO_CONNECTION_TIME_OUT);
	void Disconnect();
	void QueryDisconnect();


	virtual bool StartWork() override;


	bool Send(LPCVOID pData, UINT Size, LPCVOID Param);
	bool SendMulti(LPCVOID * pDataBuffers, const UINT * pDataSizes, LPCVOID * pParams, UINT BufferCount);

	virtual bool AllocRecvBuffer(const BYTE * pPacketHeader, UINT HeaderSize, LPBYTE& pBuffer, UINT& BufferSize, LPCVOID& Param);
	virtual void ReleaseRecvBuffer(BYTE * pBuffer, LPCVOID Param);
	virtual void ReleaseSendBuffer(LPCVOID pBuffer, LPCVOID Param);


	virtual void OnRecvData(const BYTE * pData, UINT DataSize, LPCVOID Param) = 0;
	virtual void OnRecvData(const BYTE * pData, UINT DataSize)
	{

	}

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT) override;


	void SetServer(CNetServer* pServer);

	CNetServer* GetServer();

	void EnableSafeDisconnect(bool Enable);

	void SetPacketHeaderSize(UINT Size);

protected:
	void DoRecv();
	void DoSend();
	UINT TrySend(LPCVOID pData, UINT Size);
};




inline void CNetConnectionNoBuff::SetServer(CNetServer* pServer)
{
	m_pServer=pServer;
}

inline CNetServer* CNetConnectionNoBuff::GetServer()
{
	return m_pServer;
}


inline void CNetConnectionNoBuff::EnableSafeDisconnect(bool Enable)
{
	m_UseSafeDisconnect=Enable;
}

inline bool CNetConnectionNoBuff::Send(LPCVOID pData, UINT Size, LPCVOID Param)
{
	return SendMulti(&pData,&Size,&Param,1);
}

inline void CNetConnectionNoBuff::SetPacketHeaderSize(UINT Size)
{
	m_PacketHeaderBuffer.Create(Size);
}