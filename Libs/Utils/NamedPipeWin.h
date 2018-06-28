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

	HANDLE			m_hPipe;
	CEasyString		m_PipeName;
	PIPE_STATUS		m_PipeStatus;
	OVERLAPPED		m_Overlapped;
	CEasyBuffer		m_RecvBuffer;
	bool			m_IsServer;
	CEasyTimer		m_ConnectTimer;
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
	void StartWork();
	bool QueryRead();
};

