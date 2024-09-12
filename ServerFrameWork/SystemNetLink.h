/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemNetLink.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CSystemNetLink :
	public CEasyNetLink
{
protected:
	CBaseServer *		m_pServer;
	BOOL				m_IsLinkLog;
public:
	CSystemNetLink(CBaseServer * pServer);
	~CSystemNetLink(void);

	void SendMsg(WORD Msg,LPCVOID pData=NULL,int DataLen=0);


	virtual void OnData(const BYTE * pData, UINT DataSize);

	BOOL IsLinkLog()
	{
		return m_IsLinkLog;
	}
};
