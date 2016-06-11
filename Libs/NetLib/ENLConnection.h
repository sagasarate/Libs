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

enum ENL_ACTIVE_TYPE
{
	ENL_ACTIVE_TYPE_PASSIVE,
	ENL_ACTIVE_TYPE_PROACTIVE,
};

class CEasyNetLink;

class CENLConnection :
	public CNetConnection
{
protected:
	CEasyNetLinkManager *						m_pManager;
	CEasyNetLink *								m_pParent;
	CEasyBuffer									m_AssembleBuffer;
	CEasyTimer									m_KeepAliveTimer;
	int											m_KeepAliveCount;
	CEasyTimer									m_ReconnectTimer;
	ENL_ACTIVE_TYPE								m_ActiveType;


	DECLARE_CLASS_INFO_STATIC(CENLConnection);
public:
	CENLConnection();
	~CENLConnection();

	bool Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize);
	bool Init(CEasyNetLinkManager * pManager, CEasyNetLink * pParent, UINT MaxPacketSize);
	void SetParent(CEasyNetLink * pParent);
	CEasyNetLink * GetParent();

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();

	void SendLinkMsg(DWORD MsgID, LPCVOID pData = NULL, UINT DataSize = 0);

	ENL_ACTIVE_TYPE GetActiveType();
};

inline void CENLConnection::SetParent(CEasyNetLink * pParent)
{
	m_pParent = pParent;
}
inline CEasyNetLink * CENLConnection::GetParent()
{
	return m_pParent;
}

inline ENL_ACTIVE_TYPE CENLConnection::GetActiveType()
{
	return m_ActiveType;
}