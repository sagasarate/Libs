#pragma once
class CWebSocketProxyService :
    public CDOSObjectProxyServiceDefault
{
protected:
    CIDStorage<CWebSocketProxyConnection, EDSM_NEW_ONCE>		m_WSConnectionPool;
public:
    CWebSocketProxyService();
    ~CWebSocketProxyService();


    virtual bool OnStart() override;
    virtual UINT GetConnectionCount() override;
	virtual void OnClose() override;
	virtual CBaseNetConnection* CreateConnection(CIPAddress& RemoteAddress) override;
	virtual bool DeleteConnection(CBaseNetConnection* pConnection) override;
	virtual CDOSObjectProxyConnectionDefault* GetConnection(UINT ID) override;
	virtual LPVOID GetFirstConnectionPos() override;
	virtual CDOSObjectProxyConnectionDefault* GetNextConnection(LPVOID& Pos) override;
protected:
	virtual int CheckFreeObject() override;
};

