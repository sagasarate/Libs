#pragma once

class CNamedPipe :
	public CNameObject
{
protected:
	enum PIPE_STATUS
	{
		PIPE_STATUS_DISCONNECTED,
		PIPE_STATUS_CONNECTING,
		PIPE_STATUS_CONNECTED,
	};

	int				m_fdRead;
	int				m_fdWrite;
	CEasyString		m_PipeNameRead;
	CEasyString		m_PipeNameWrite;
	PIPE_STATUS		m_PipeStatus;	
	CEasyBuffer		m_RecvBuffer;
	CEasyBuffer		m_SendBuffer;
	bool			m_IsServer;
	CEasyTimer		m_ConnectTimer;
	CEasyTimer		m_ConnectTryTimer;
public:
	CNamedPipe();
	virtual ~CNamedPipe();

	virtual void Destory();

	bool Create(LPCTSTR PipeName, UINT BufferSize);
	bool Open(LPCTSTR PipeName, UINT BufferSize, UINT ConnectTimeout = 30000);

	virtual int Update(int ProcessLimit = 32);

	virtual void OnConnection(bool Succeed);
	virtual void OnDisconnection();

	virtual void OnRecvData(const BYTE * pData, UINT DataSize) = 0;
	bool SendData(const void * pData, UINT DataSize);

	bool IsConnected()
	{
		return m_PipeStatus == PIPE_STATUS_CONNECTED;
	}

	bool IsDisconnected()
	{
		return m_PipeStatus == PIPE_STATUS_DISCONNECTED;
	}
protected:
	void DoSend();
};

