/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemControlPort.h                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CSystemControlPort :
	public CNetService
{
protected:
	CBaseServer * m_pServer;
public:
	CSystemControlPort(void);
	~CSystemControlPort(void);

	bool Init(CBaseServer * pServer);

	virtual void OnStartUp();
	virtual void OnClose();

	virtual void OnRecvData(const CIPAddress& IPAddress, const BYTE * pData, UINT DataSize);

	void SendMsg(const CIPAddress& Address,WORD Msg,LPCVOID pData=NULL,int DataLen=0);
};
