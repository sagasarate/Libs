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
	public CEasyNetLinkConnection
{
protected:
	

	DECLARE_CLASS_INFO_STATIC(CDOSRouterLink);

	
public:
	CDOSRouterLink();
	virtual ~CDOSRouterLink(void);	
	
	CDOSRouter * GetRouter();

	virtual void OnLinkStart();
	virtual void OnLinkEnd();
	virtual void OnData(const CEasyBuffer& DataBuffer);

	
	void OnMsgPacket(CDOSMessagePacket * pPacket);

};



inline CDOSRouter * CDOSRouterLink::GetRouter()
{
	return (CDOSRouter *)GetManager();
}


