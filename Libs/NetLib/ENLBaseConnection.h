/****************************************************************************/
/*                                                                          */
/*      文件名:    CENLBaseConnection.h                                     */
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

class CENLBaseConnection 
{
protected:	
	CEasyNetLink *								m_pParent;	
	ENL_ACTIVE_TYPE								m_ActiveType;

public:
	CENLBaseConnection()
	{

	}
	~CENLBaseConnection()
	{

	}
	virtual void Release() = 0;
	virtual UINT AddUseRef() = 0;

	void SetParent(CEasyNetLink * pParent)
	{
		m_pParent = pParent;
	}
	CEasyNetLink * GetParent()
	{
		return m_pParent;
	}

	virtual int Update(int ProcessPacketLimit = DEFAULT_SERVER_PROCESS_PACKET_LIMIT) = 0;

	virtual bool SendData(LPCVOID pData, UINT DataSize) = 0;

	virtual void Disconnect() = 0;
	virtual bool IsDisconnected() = 0;
	virtual bool IsConnected() = 0;
	virtual CBaseNetConnection * GetBaseConnection() = 0;

	ENL_ACTIVE_TYPE GetActiveType()
	{
		return m_ActiveType;
	}
};

