#include "stdafx.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>


CNamedPipe::CNamedPipe()
	:m_RecvBuffer(_T("CNamedPipe"))
	, m_SendBuffer(_T("CNamedPipe"))
{
	m_fdRead = INVALID_HANDLE_VALUE;
	m_fdWrite = INVALID_HANDLE_VALUE;
	m_PipeStatus = PIPE_STATUS_DISCONNECTED;
	m_IsServer = false;
}


CNamedPipe::~CNamedPipe()
{
	Destory();
}


void CNamedPipe::Destory()
{
	if (m_fdRead != INVALID_HANDLE_VALUE || m_fdWrite != INVALID_HANDLE_VALUE)
	{
		OnDisconnection();
	}
	if (m_fdRead != INVALID_HANDLE_VALUE)
	{
		close(m_fdRead);
		m_fdRead = INVALID_HANDLE_VALUE;
	}

	if (m_fdWrite != INVALID_HANDLE_VALUE)
	{
		close(m_fdWrite);
		m_fdWrite = INVALID_HANDLE_VALUE;
	}
	if (m_IsServer)
	{
		if (!m_PipeNameRead.IsEmpty())
		{
			unlink(m_PipeNameRead);
			m_PipeNameRead.Clear();
		}
		if (!m_PipeNameWrite.IsEmpty())
		{
			unlink(m_PipeNameWrite);
			m_PipeNameWrite.Clear();
		}
	}
	m_RecvBuffer.Destory();
	m_SendBuffer.Destory();
	m_IsServer = false;
	m_PipeStatus = PIPE_STATUS_DISCONNECTED;
}

bool CNamedPipe::Create(LPCTSTR PipeName, UINT BufferSize)
{
	Destory();
	m_PipeNameRead.Format(_T("/tmp/%s_Read"), PipeName);
	m_PipeNameWrite.Format(_T("/tmp/%s_Write"), PipeName);


	if (access(m_PipeNameRead, F_OK) == 0)
	{
		unlink(m_PipeNameRead);
	}

	if (access(m_PipeNameWrite, F_OK) == 0)
	{
		unlink(m_PipeNameWrite);
	}

	int Result = mkfifo(m_PipeNameRead, 0777);
	if (Result != 0)
	{
		PrintImportantLog(_T("无法创建命名管道[%s]%d"), (LPCTSTR)m_PipeNameRead, errno);
		return false;
	}

	Result = mkfifo(m_PipeNameWrite, 0777);
	if (Result != 0)
	{
		PrintImportantLog(_T("无法创建命名管道[%s]%d"), (LPCTSTR)m_PipeNameRead, errno);
		return false;
	}

	m_RecvBuffer.Create(BufferSize);
	m_SendBuffer.Create(BufferSize);
	m_PipeStatus = PIPE_STATUS_CONNECTING;
	m_IsServer = true;
	return true;
}
bool CNamedPipe::Open(LPCTSTR PipeName, UINT BufferSize, UINT ConnectTimeout)
{
	Destory();
	m_PipeNameRead.Format(_T("/tmp/%s_Write"), PipeName);
	m_PipeNameWrite.Format(_T("/tmp/%s_Read"), PipeName);
	m_RecvBuffer.Create(BufferSize);
	m_SendBuffer.Create(BufferSize);

	m_ConnectTimer.SetTimeOut(ConnectTimeout);

	m_PipeStatus = PIPE_STATUS_CONNECTING;
	m_IsServer = false;

	return true;
}

int CNamedPipe::Update(int ProcessLimit)
{
	int ProcessCount = 0;
	switch (m_PipeStatus)
	{
	case PIPE_STATUS_CONNECTING:
		{
			if ((!m_IsServer) && m_ConnectTimer.IsTimeOut())
			{
				OnConnection(false);
				PrintImportantLog(_T("连接管道超时"));
				Destory();
			}
			else if (m_ConnectTryTimer.IsTimeOut(100))
			{
				m_ConnectTryTimer.SaveTime();
				bool HaveError = false;
				if (m_fdRead == INVALID_HANDLE_VALUE)
				{
					m_fdRead = open(m_PipeNameRead, O_RDONLY | O_NONBLOCK);
					if (m_fdRead == INVALID_HANDLE_VALUE)
					{
						PrintImportantLog(_T("无法打开命名管道进行读取[%s]%d"), (LPCTSTR)m_PipeNameRead, errno);
						HaveError = true;
					}
				}
				if (m_fdWrite == INVALID_HANDLE_VALUE)
				{
					m_fdWrite = open(m_PipeNameWrite, O_WRONLY | O_NONBLOCK);
					if (m_fdWrite == INVALID_HANDLE_VALUE)
					{
						if (errno != ENXIO && errno != ENOENT)
						{
							PrintImportantLog(_T("无法打开命名管道进行写入[%s]%d"), (LPCTSTR)m_PipeNameWrite, errno);
							HaveError = true;
						}
					}
				}
				if (HaveError)
				{
					OnConnection(false);
					PrintImportantLog(_T("连接管道失败"));
					Destory();
				}
				else if ((m_fdRead != INVALID_HANDLE_VALUE) && (m_fdWrite != INVALID_HANDLE_VALUE))
				{
					m_PipeStatus = PIPE_STATUS_CONNECTED;
					OnConnection(true);
					ProcessCount++;
				}
			}
		}
		break;
	case PIPE_STATUS_CONNECTED:
		{
			if (m_SendBuffer.GetUsedSize())
			{
				DoSend();
			}
			if (m_fdRead != INVALID_HANDLE_VALUE)
			{
				LONG64 ReadSize = read(m_fdRead, m_RecvBuffer.GetBuffer(), (size_t)m_RecvBuffer.GetBufferSize());
				if (ReadSize == -1)
				{
					if (errno != EAGAIN)
					{
						PrintImportantLog(_T("管道读取失败[%s]%d"), (LPCTSTR)m_PipeNameRead, errno);
						Destory();
					}
				}
				else
				{
					OnRecvData((BYTE *)m_RecvBuffer.GetBuffer(), (UINT)ReadSize);
				}
			}
		}
		break;
	}
	return ProcessCount;
}

void CNamedPipe::OnConnection(bool Succeed)
{

}
void CNamedPipe::OnDisconnection()
{

}


bool CNamedPipe::SendData(const void * pData, UINT DataSize)
{
	if (IsConnected())
	{
		m_SendBuffer.PushBack(pData, DataSize);
		DoSend();
		return true;
	}
	return false;
}

void CNamedPipe::DoSend()
{
	LONG64 WriteSize = write(m_fdWrite, m_SendBuffer.GetBuffer(), (size_t)m_SendBuffer.GetUsedSize());
	if (WriteSize == -1)
	{
		WriteSize = 0;
	}

	if (WriteSize < m_SendBuffer.GetUsedSize())
	{
		if (errno == EAGAIN)
		{
			m_SendBuffer.PopFront(NULL, (UINT)WriteSize);
		}
		else
		{
			PrintImportantLog(_T("管道写入失败[%s]%d"), (LPCTSTR)m_PipeNameWrite, errno);
			Destory();
		}
	}
	else
	{
		m_SendBuffer.SetUsedSize(0);
	}
}
