/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSServer.h                                              */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CDOSServer :
	public CNetServer
{
protected:
	DOS_CONFIG					m_ServerConfig;
	CDOSProxyManager *			m_pProxyManager;
	CDOSRouter*					m_pDOSRouter;		
	CDOSObjectManager *			m_pObjectManager;
	CDOSRouterLinkManager *		m_pRouterLinkManager;

	CFastMemoryPool				m_MemoryPool;	

	DECLARE_CLASS_INFO_STATIC(CDOSServer);
public:
	CDOSServer(void);
	virtual ~CDOSServer(void);		

	void SetConfig(const DOS_CONFIG& Config);
	DOS_CONFIG& GetConfig();

	CDOSProxyManager * GetProxyManager();
	CDOSRouter * GetRouter();
	CDOSObjectManager * GetObjectManager();
	CFastMemoryPool * GetMemoryPool();
	CDOSRouterLinkManager * GetRouterLinkManager();

	CDOSMessagePacket * NewMessagePacket(UINT Size);
	bool ReleaseMessagePacket(CDOSMessagePacket * pPacket);
	void AddRefMessagePacket(CDOSMessagePacket * pPacket);
protected:
	virtual bool OnStartUp();
	virtual void OnShutDown();
	
};


inline DOS_CONFIG& CDOSServer::GetConfig()
{
	return m_ServerConfig;
}

inline CDOSProxyManager * CDOSServer::GetProxyManager()
{
	return m_pProxyManager;
}
inline CDOSRouter * CDOSServer::GetRouter()
{
	return m_pDOSRouter;
}
inline CDOSObjectManager * CDOSServer::GetObjectManager()
{
	return m_pObjectManager;
}
inline CFastMemoryPool * CDOSServer::GetMemoryPool()
{
	return &m_MemoryPool;
}
inline CDOSRouterLinkManager * CDOSServer::GetRouterLinkManager()
{
	return m_pRouterLinkManager;
}
inline CDOSMessagePacket * CDOSServer::NewMessagePacket(UINT Size)
{	
	MSG_LEN_TYPE PacketLen=CDOSMessagePacket::CaculateRealPacketLength(Size);
	CDOSMessagePacket * pPacket = NULL;
	if (m_ServerConfig.MemoryPoolConfig.Enable)
		pPacket = (CDOSMessagePacket *)m_MemoryPool.Alloc(PacketLen);
	else
		pPacket = (CDOSMessagePacket *)MONITORED_MALLOC(_T("DOSMessage"), PacketLen);
	if(pPacket)
	{
		pPacket->Init();	
		pPacket->SetAllocSize(Size);
		pPacket->SetPacketLength(Size);
#ifdef _DEBUG
		pPacket->SetAllocTime(0);
#endif
		UINT RefCount=pPacket->IncRefCount();
#ifdef LOG_MEM_CALL_STACK
		if (m_ServerConfig.MemoryPoolConfig.Enable)
			m_MemoryPool.LogMemUse(pPacket,RefCount);
#endif
	}
	else
	{
		PrintDOSLog(_T("分配%u大小的消息包失败"),Size);
	}
	return pPacket;
}
inline bool CDOSServer::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	UINT RefCount=pPacket->DecRefCount();
#ifdef LOG_MEM_CALL_STACK
	if (m_ServerConfig.MemoryPoolConfig.Enable)
		m_MemoryPool.LogMemUse(pPacket,RefCount);
#endif
	if (RefCount <= 0)
	{
		if (m_ServerConfig.MemoryPoolConfig.Enable)
		{
			return m_MemoryPool.Free(pPacket);
		}
		else
		{
			MONITORED_FREE(pPacket);
			return true;
		}
	}
	return true;
}
inline void CDOSServer::AddRefMessagePacket(CDOSMessagePacket * pPacket)
{
	UINT RefCount=pPacket->IncRefCount();
#ifdef LOG_MEM_CALL_STACK
	if (m_ServerConfig.MemoryPoolConfig.Enable)
		m_MemoryPool.LogMemUse(pPacket,RefCount);
#endif
}
