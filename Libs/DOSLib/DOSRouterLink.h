/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSRouterLink.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CDOSServer;
class CDOSRouter;

class CDOSRouterLink :
	public CEasyNetLink
{
protected:
	

	DECLARE_CLASS_INFO_STATIC(CDOSRouterLink);

	
public:
	CDOSRouterLink();
	virtual ~CDOSRouterLink(void);	
	
	CDOSRouter * GetRouter();

	virtual void OnLinkStart();
	virtual void OnLinkEnd();
	virtual void OnData(const BYTE * pData, UINT DataSize);

	
	void OnMsgPacket(CDOSMessagePacket * pPacket);
	bool SendPacket(CDOSMessagePacket * pPacket);
protected:
	virtual CENLBaseConnection * NewConnection(UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail);
	virtual CENLBaseConnection * NewConnection(const CIPAddress& ConnectionAddress, UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize,
		CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool DisconnectOnTransferFail);
};



inline CDOSRouter * CDOSRouterLink::GetRouter()
{
	return ((CDOSRouterLinkManager *)GetManager())->GetRouter();
}


