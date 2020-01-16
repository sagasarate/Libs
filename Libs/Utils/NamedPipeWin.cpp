#include "stdafx.h"



CNamedPipe::CNamedPipe()
	:m_RecvBuffer(_T("CNamedPipe"))
{
	m_hPipe = INVALID_HANDLE_VALUE;
	m_PipeStatus = PIPE_STATUS_DISCONNECTED;
	ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
	m_IsServer = false;
}


CNamedPipe::~CNamedPipe()
{
	Destory();
}


void CNamedPipe::Destory()
{
	if (m_hPipe != INVALID_HANDLE_VALUE)
	{
		OnDisconnection();
		CloseHandle(m_hPipe);
		m_hPipe = INVALID_HANDLE_VALUE;
	}

	if (m_Overlapped.hEvent)
	{
		CloseHandle(m_Overlapped.hEvent);
	}
	ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
	m_PipeName.Clear();
	m_RecvBuffer.Destory();
	m_IsServer = false;
	m_PipeStatus = PIPE_STATUS_DISCONNECTED;
}

bool CNamedPipe::Create(LPCTSTR PipeName, UINT BufferSize)
{
	Destory();
	m_PipeName.Format(_T("\\\\.\\pipe\\%s"), PipeName);
	m_RecvBuffer.Create(BufferSize);

	m_hPipe = CreateNamedPipe(m_PipeName, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, 0, PIPE_UNLIMITED_INSTANCES, BufferSize, BufferSize, 0, NULL);

	if (m_hPipe == INVALID_HANDLE_VALUE)
	{
		PrintImportantLog(_T("无法创建命名管道[%s]%d"), (LPCTSTR)m_PipeName, GetLastError());
		return false;
	}

	m_Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_Overlapped.hEvent == NULL)
	{
		PrintImportantLog(_T("无法创建事件%d"), GetLastError());
		return false;
	}

	if (!ConnectNamedPipe(m_hPipe, &m_Overlapped))
	{
		if (ERROR_IO_PENDING != GetLastError())
		{
			PrintImportantLog(_T("等待管道连接失败%d"), GetLastError());
			return false;
		}
	}

	m_PipeStatus = PIPE_STATUS_CONNECTING;
	m_IsServer = true;
	return true;
}
bool CNamedPipe::Open(LPCTSTR PipeName, UINT BufferSize, UINT ConnectTimeout)
{
	Destory();
	m_PipeName.Format(_T("\\\\.\\pipe\\%s"), PipeName);
	m_RecvBuffer.Create(BufferSize);

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
		if (m_IsServer)
		{
			DWORD Ret = WaitForSingleObject(m_Overlapped.hEvent, 0);
			if (Ret == WAIT_OBJECT_0)
			{
				CloseHandle(m_Overlapped.hEvent);
				m_Overlapped.hEvent = NULL;
				StartWork();
				ProcessCount++;
			}
			else if (Ret != WAIT_TIMEOUT)
			{
				PrintImportantLog(_T("等待管道连接失败%d"), GetLastError());
				Destory();
			}
		}
		else
		{
			if (WaitNamedPipe(m_PipeName, 1))
			{
				m_hPipe = CreateFile(m_PipeName, GENERIC_READ | GENERIC_WRITE,
					0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, NULL);
				if (m_hPipe != INVALID_HANDLE_VALUE)
				{
					if (GetLastError() == 0)
					{
						StartWork();
						ProcessCount++;
					}					
					else
					{
						PrintImportantLog(_T("打开管道%s失败[%d]"), (LPCTSTR)m_PipeName, GetLastError());
						Destory();
					}
				}
				else
				{
					PrintImportantLog(_T("无法打开管道%s[%d]"), (LPCTSTR)m_PipeName, GetLastError());
					Destory();
				}
			}
			else if (m_ConnectTimer.IsTimeOut())
			{
				PrintImportantLog(_T("连接管道超时"));
				Destory();
			}
		}
		break;
	case PIPE_STATUS_CONNECTED:
		{
			DWORD Ret = WaitForSingleObject(m_Overlapped.hEvent, 0);
			if (Ret == WAIT_OBJECT_0)
			{
				DWORD ReadSize = 0;
				if (GetOverlappedResult(m_hPipe, &m_Overlapped, &ReadSize,FALSE))
				{
					OnRecvData((const BYTE *)m_RecvBuffer.GetBuffer(), ReadSize);
					if (!QueryRead())
					{
						Destory();
					}
					ProcessCount++;
				}
				else
				{
					PrintImportantLog(_T("管道读取失败%d"), GetLastError());
					Destory();
				}
			}
			else if (Ret != WAIT_TIMEOUT)
			{
				PrintImportantLog(_T("等待管道读取失败%d"), GetLastError());
				Destory();
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
		DWORD WriteSize = 0;
		if (WriteFile(m_hPipe, pData, DataSize, &WriteSize, NULL))
		{
			return true;
		}
		PrintImportantLog(_T("管道写入失败%d"), GetLastError());
	}
	return false;
}

void CNamedPipe::StartWork()
{
	if (QueryRead())
	{
		m_PipeStatus = PIPE_STATUS_CONNECTED;
		OnConnection(true);
	}
	else
	{
		OnConnection(false);
		Destory();
	}	
}

bool CNamedPipe::QueryRead()
{
	if (m_hPipe == INVALID_HANDLE_VALUE)
		return false;
	if (m_Overlapped.hEvent)
		CloseHandle(m_Overlapped.hEvent);
	ZeroMemory(&m_Overlapped, sizeof(OVERLAPPED));
	m_Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_Overlapped.hEvent)
	{
		if (ReadFile(m_hPipe, m_RecvBuffer.GetBuffer(), m_RecvBuffer.GetBufferSize(), NULL, &m_Overlapped))
		{
			return true;
		}
		else 
		{
			int Err = GetLastError();
			if (Err == ERROR_IO_PENDING)
			{
				return true;
			}
			else
			{
				PrintImportantLog(_T("读取管道失败%d"), GetLastError());
			}
			
		}
	}
	else
	{
		PrintImportantLog(_T("无法创建事件%d"), GetLastError());
	}
	return false;
}