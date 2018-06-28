#pragma once

class CSystemControlPipe :
	public CNamedPipe
{
protected:
	CBaseServer *	m_pServer;
	CEasyBuffer		m_AssembleBuffer;
	CSmartStruct	m_SendBuffer;
	CEasyTimer		m_RecreateTime;
	bool			m_IsLinkLog;
	CEasyString		m_ControlPipeName;
public:
	CSystemControlPipe();
	~CSystemControlPipe();

	bool Init(CBaseServer * pServer);

	virtual int Update(int ProcessLimit);
	virtual void OnConnection(bool Succeed);
	virtual void OnDisconnection();

	virtual void OnRecvData(const BYTE * pData, UINT DataSize);
	void OnMsg(CSmartStruct& MsgPacket);

	void SendLogMsg(LPCTSTR LogMsg);
};

