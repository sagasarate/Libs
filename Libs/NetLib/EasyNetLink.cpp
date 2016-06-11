/****************************************************************************/
/*                                                                          */
/*      文件名:    EasyNetLink.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"


static char s_LZOCompressWorkMemory[LZO1X_1_MEM_COMPRESS];

LPCTSTR g_szLinkStatus[ENL_LINK_MAX]={_T("None"),_T("Accepting"),_T("Accepted")};

IMPLEMENT_CLASS_INFO_STATIC(CEasyNetLink, CNameObject);

CEasyNetLink::CEasyNetLink(void)
{
	m_pManager=NULL;
	m_pConnection = NULL;
	m_MaxPacketSize = 0;
	m_DataCompressType = CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE;
	m_MinCompressSize = CEasyNetLinkManager::DEFAULT_MIN_COMPRESS_SIZE;
	m_Status=ENL_LINK_NONE;
	m_ReportID=0;
	m_NeedReallocConnectionID = false;
}

CEasyNetLink::~CEasyNetLink(void)
{
	if (m_pConnection)
	{
		PrintNetLog(_T("NetLib"), _T("[%s]连接[%s:%u]释放"),
			CClassifiedID(GetReportID()).ToStr(),
			m_pConnection->GetRemoteAddress().GetIPString(),
			m_pConnection->GetRemoteAddress().GetPort());
	}
	SAFE_RELEASE(m_pConnection);
}

bool CEasyNetLink::Init(CEasyNetLinkManager * pManager, const CIPAddress& ConnectionAddress, UINT ReportID, 
	UINT RecvQueueSize, UINT SendQueueSize, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pManager;
	m_ReportID = ReportID;
	m_MaxPacketSize = MaxPacketSize;
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
		if (m_DataCompressType != CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE&&m_CompressBuffer.GetBufferSize() < m_MaxPacketSize)
		{
			m_CompressBuffer.Create(m_MaxPacketSize);
		}
	}
	
	m_pConnection = new CENLConnection();
	if (m_pConnection->Init(m_pManager, this, ConnectionAddress, RecvQueueSize, SendQueueSize, MaxPacketSize))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CEasyNetLink::Init(CEasyNetLinkManager * pManager, UINT ReportID, UINT MaxPacketSize, CEasyNetLinkManager::DATA_COMPRESS_TYPE DataCompressType, UINT MinCompressSize, bool NeedReallocConnectionID)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pManager;
	m_ReportID = ReportID;
	m_MaxPacketSize = MaxPacketSize;
	if (DataCompressType < CEasyNetLinkManager::DATA_COMPRESS_TYPE_MAX)
	{
		m_DataCompressType = DataCompressType;
		m_MinCompressSize = MinCompressSize;
		if (m_DataCompressType != CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE&&m_CompressBuffer.GetBufferSize()<m_MaxPacketSize)
		{
			m_CompressBuffer.Create(m_MaxPacketSize);
		}
	}
	m_NeedReallocConnectionID = NeedReallocConnectionID;
	m_pConnection = new CENLConnection();
	if (m_pConnection->Init(m_pManager, this, MaxPacketSize))
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CEasyNetLink::Init(CEasyNetLink * pLink)
{
	SAFE_RELEASE(m_pConnection);
	m_pManager = pLink->m_pManager;
	m_ReportID = pLink->m_ReportID;	
	m_MaxPacketSize = pLink->m_MaxPacketSize;
	m_DataCompressType = pLink->m_DataCompressType;
	m_MinCompressSize = pLink->m_MinCompressSize;	
	if (m_DataCompressType != CEasyNetLinkManager::DATA_COMPRESS_TYPE_NONE&&m_CompressBuffer.GetBufferSize() < m_MaxPacketSize)
	{
		m_CompressBuffer.Create(m_MaxPacketSize);
	}
	m_pConnection = pLink->m_pConnection;
	m_pConnection->AddUseRef();
	m_pConnection->SetParent(this);
	return true;
	
}

void CEasyNetLink::OnData(const BYTE * pData, UINT DataSize, bool IsCompressed)
{
	if (IsCompressed)
	{
		lzo_uint OutLen = m_CompressBuffer.GetBufferSize();
		int Result = lzo1x_decompress_safe(pData, DataSize, (BYTE *)m_CompressBuffer.GetBuffer(), &OutLen, s_LZOCompressWorkMemory);
		if (Result == LZO_E_OK)
		{
			OnData((BYTE *)m_CompressBuffer.GetBuffer(), (UINT)OutLen);
		}
		else
		{
			PrintNetLog(_T("NetLib"), _T("lzo解压数据失败(%d)"),
				Result);
			Disconnect();
		}
	}
	else
	{
		OnData(pData, DataSize);
	}
}

void CEasyNetLink::OnLinkStart()
{
}

void CEasyNetLink::OnLinkEnd()
{
}

void CEasyNetLink::SendData(LPCVOID pData, UINT DataSize)
{
	if (pData&&DataSize>0)
	{
		if (m_pConnection)
		{
			if (m_DataCompressType == CEasyNetLinkManager::DATA_COMPRESS_TYPE_LZO && DataSize >= m_MinCompressSize)
			{
				lzo_uint OutLen = m_CompressBuffer.GetBufferSize();
				int Result = lzo1x_1_compress((BYTE *)pData, DataSize,
					(BYTE *)m_CompressBuffer.GetBuffer(), &OutLen,
					s_LZOCompressWorkMemory);

				if (Result == LZO_E_OK)
				{
					m_pConnection->SendLinkMsg(EASY_NET_LINK_MSG_COMPRESSED_USER_DATA, m_CompressBuffer.GetBuffer(), (UINT)OutLen);
				}
				else
				{
					PrintNetLog(_T("NetLib"), _T("lzo压缩数据失败(%d)，将直接发送"),
						Result);
					m_pConnection->SendLinkMsg(EASY_NET_LINK_MSG_USER_DATA, pData, DataSize);
				}
			}
			else
			{
				m_pConnection->SendLinkMsg(EASY_NET_LINK_MSG_USER_DATA, pData, DataSize);
			}			
		}
		else
		{
			PrintNetLog(_T("NetLib"), _T("CEasyNetLinkConnection::SendData 连接未初始化"));
		}
			
	}
	else
	{
		PrintNetLog(_T("NetLib"),_T("CEasyNetLinkConnection::SendData 发送的数据大小不合法"));
	}
}

int CEasyNetLink::Update(int ProcessPacketLimit)
{
	int Process = 0;
	if (m_pConnection)
		Process = m_pConnection->Update(ProcessPacketLimit);	
	return Process;
}







void CEasyNetLink::PrintInfo(UINT LogChannel)
{
	if (m_pConnection)
	{
		CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0,
			_T("LinkID=[%s] RemoteAddress=%s:%u %s %s"),
			CClassifiedID(GetID()).ToStr(),
			m_pConnection->GetRemoteAddress().GetIPString(),
			m_pConnection->GetRemoteAddress().GetPort(),
			m_pConnection->IsConnected() ? _T("Connected") : _T("Disconnected"),
			g_szLinkStatus[m_Status]);
	}
}
