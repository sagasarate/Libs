/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyFileServerConnection.h                               */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CEasyFileServerConnection :
	public CNetConnection
{
protected:

	DECLARE_CLASS_INFO_STATIC(CEasyFileServerConnection);
public:
	CEasyFileServerConnection(void);
	virtual ~CEasyFileServerConnection(void);

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	virtual void OnRecvData(const CEasyBuffer& DataBuffer);
	virtual void OnConnection(BOOL IsSucceed);
	virtual void OnDisconnection();
};
