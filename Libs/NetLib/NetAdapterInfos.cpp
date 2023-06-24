#include "stdafx.h"



CNetAdapterInfos::CNetAdapterInfos()
	:m_NetAdapterInfoList(_T("CNetAdapterInfos"))
	, m_WorkBuffer1(_T("CNetAdapterInfos"))
	, m_WorkBuffer2(_T("CNetAdapterInfos"))
{
	m_RecentUpdateTime = 0;
	m_NetInfoFile = NULL;
}


CNetAdapterInfos::~CNetAdapterInfos()
{
	if (m_NetInfoFile)
	{
		fclose(m_NetInfoFile);
		m_NetInfoFile = NULL;
	}
}

#ifdef WIN32
bool CNetAdapterInfos::Init()
{
	ULONG IfTableLen = 0;
	GetIfTable(NULL, &IfTableLen, TRUE);

	if (m_WorkBuffer1.GetBufferSize() < IfTableLen)
		m_WorkBuffer1.Create(IfTableLen);

	MIB_IFTABLE * pIfTable = (MIB_IFTABLE *)m_WorkBuffer1.GetBuffer();

	if (GetIfTable(pIfTable, &IfTableLen, TRUE) != NO_ERROR)
	{
		PrintNetLog( _T("获取网络适配器信息失败"));
		return false;
	}


	ULONG AdressTableLen = 0;
	GetAdaptersAddresses(AF_UNSPEC, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER, NULL, NULL, &AdressTableLen);

	if (m_WorkBuffer2.GetBufferSize() < AdressTableLen)
		m_WorkBuffer2.Create(AdressTableLen);

	IP_ADAPTER_ADDRESSES  * pAddressTable = (IP_ADAPTER_ADDRESSES  *)m_WorkBuffer2.GetBuffer();

	if (GetAdaptersAddresses(AF_INET, GAA_FLAG_SKIP_ANYCAST | GAA_FLAG_SKIP_MULTICAST | GAA_FLAG_SKIP_DNS_SERVER, NULL,
		pAddressTable, &AdressTableLen) != NO_ERROR)
	{
		PrintNetLog( _T("获取网络适配器IP地址信息失败"));
		return false;
	}


	m_NetAdapterInfoList.Resize(pIfTable->dwNumEntries);
	for (UINT i = 0; i < pIfTable->dwNumEntries; i++)
	{
		m_NetAdapterInfoList[i].Index = pIfTable->table[i].dwIndex;
		m_NetAdapterInfoList[i].Name = pIfTable->table[i].wszName;
		m_NetAdapterInfoList[i].MAC.SetString((const char *)pIfTable->table[i].bPhysAddr, pIfTable->table[i].dwPhysAddrLen);
		m_NetAdapterInfoList[i].RecvBytes = pIfTable->table[i].dwInOctets;
		m_NetAdapterInfoList[i].RecvFlux = 0;
		m_NetAdapterInfoList[i].SendBytes = pIfTable->table[i].dwOutOctets;
		m_NetAdapterInfoList[i].SendFlux = 0;
		m_NetAdapterInfoList[i].IPAddressList.Clear();

		IP_ADAPTER_ADDRESSES * pAddress = pAddressTable;
		while (pAddress)
		{
			if (pAddress->IfIndex == pIfTable->table[i].dwIndex)
			{
				PIP_ADAPTER_UNICAST_ADDRESS_LH pUnicastAddress = pAddress->FirstUnicastAddress;
				while (pUnicastAddress)
				{
					CIPAddress IPAddress;
					IPAddress.SetAddress(pAddress->FirstUnicastAddress->Address.lpSockaddr, pAddress->FirstUnicastAddress->Address.iSockaddrLength);
					m_NetAdapterInfoList[i].IPAddressList.Add(IPAddress);
					pUnicastAddress = pUnicastAddress->Next;
				}
				break;
			}
			pAddress = pAddress->Next;
		}
	}
	return FetchStatus();
}
#else
bool CNetAdapterInfos::Init()
{
	struct ifaddrs * pIFInfo = NULL, *pCurInfo = NULL;

	int Ret = getifaddrs(&pIFInfo);
	if (Ret != -1)
	{
		UINT Index = 0;
		m_NetAdapterInfoList.Create(4, 4);
		for (pCurInfo = pIFInfo; pCurInfo != NULL; pCurInfo = pCurInfo->ifa_next)
		{
			NET_ADAPTER_INFO * pNetAdapterInfo = NULL;
			for (UINT i = 0; i < m_NetAdapterInfoList.GetCount(); i++)
			{
				if (m_NetAdapterInfoList[i].Name.Compare(pCurInfo->ifa_name) == 0)
				{
					pNetAdapterInfo = m_NetAdapterInfoList.GetObject(i);
					break;
				}
			}
			if (pNetAdapterInfo == NULL)
			{
				NET_ADAPTER_INFO * pNetAdapterInfo = m_NetAdapterInfoList.AddEmpty();
				pNetAdapterInfo->Index = Index++;
				pNetAdapterInfo->Name = pCurInfo->ifa_name;
			}

			if (pCurInfo->ifa_addr->sa_family == AF_INET)
			{
				CIPAddress * pAddress = pNetAdapterInfo->IPAddressList.AddEmpty();
				pAddress->SetAddress(*((struct sockaddr_in *)pCurInfo->ifa_addr));
			}
			else if (pCurInfo->ifa_addr->sa_family == AF_INET6)
			{
				CIPAddress * pAddress = pNetAdapterInfo->IPAddressList.AddEmpty();
				pAddress->SetAddress(*((struct sockaddr_in6 *)pCurInfo->ifa_addr));
			}

		}
		freeifaddrs(pIFInfo);

		//获取MAC地址

		SOCKET s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s != INVALID_SOCKET)
		{
			struct ifreq ifr;

			for (UINT i = 0; i < m_NetAdapterInfoList.GetCount(); i++)
			{
				strncpy(ifr.ifr_name, m_NetAdapterInfoList[i].Name, IFNAMSIZ);

				if (ioctl(s, SIOCGIFHWADDR, &ifr) == 0)
				{
					char * pMac = (char *)ifr.ifr_hwaddr.sa_data;
					m_NetAdapterInfoList[i].MAC.SetString(pMac, 6);
				}
			}
			close(s);
		}

		return FetchStatus();
	}
	else
	{
		PrintNetLog( _T("getifaddrs 发生错误 %d！"), errno);
		return false;
	}
}
#endif


#ifdef WIN32
bool CNetAdapterInfos::FetchStatus()
{
	UINT CurTime = CEasyTimer::GetTime();
	UINT TimeSpan = 0;
	if (m_RecentUpdateTime)
		TimeSpan = CurTime - m_RecentUpdateTime;
	m_RecentUpdateTime = CurTime;

	ULONG IfTableLen = 0;
	GetIfTable(NULL, &IfTableLen, TRUE);

	if (m_WorkBuffer1.GetBufferSize() < IfTableLen)
		m_WorkBuffer1.Create(IfTableLen);


	MIB_IFTABLE * pIfTable = (MIB_IFTABLE *)m_WorkBuffer1.GetBuffer();

	if (GetIfTable(pIfTable, &IfTableLen, TRUE) != NO_ERROR)
	{
		PrintNetLog( _T("获取网络适配器信息失败"));
		return false;
	}

	for (UINT i = 0; i < pIfTable->dwNumEntries; i++)
	{
		for (UINT j = 0; j < m_NetAdapterInfoList.GetCount(); j++)
		{
			if (pIfTable->table[i].dwIndex == m_NetAdapterInfoList[j].Index)
			{
				if (TimeSpan)
				{
					m_NetAdapterInfoList[i].RecvFlux = (float)(pIfTable->table[i].dwInOctets - m_NetAdapterInfoList[i].RecvBytes)*1000.0f / TimeSpan;
					m_NetAdapterInfoList[i].SendFlux = (float)(pIfTable->table[i].dwOutOctets - m_NetAdapterInfoList[i].SendBytes)*1000.0f / TimeSpan;
				}
				else
				{
					m_NetAdapterInfoList[i].RecvFlux = NAN;
					m_NetAdapterInfoList[i].SendFlux = NAN;
				}
				m_NetAdapterInfoList[i].RecvBytes = pIfTable->table[i].dwInOctets;
				m_NetAdapterInfoList[i].SendBytes = pIfTable->table[i].dwOutOctets;
			}
		}
	}
	return true;
}

#else

bool CNetAdapterInfos::FetchStatus()
{
	UINT CurTime = CEasyTimer::GetTime();
	UINT TimeSpan = 0;
	if (m_RecentUpdateTime)
		TimeSpan = CurTime - m_RecentUpdateTime;
	m_RecentUpdateTime = CurTime;

	if (m_NetInfoFile == NULL)
	{
		m_NetInfoFile = fopen("/proc/net/dev", "r");
		if (m_NetInfoFile == NULL)
			return false;
	}

	fseek(m_NetInfoFile, 0, SEEK_SET);
	fflush(m_NetInfoFile);

	SkipLine(m_NetInfoFile);
	SkipLine(m_NetInfoFile);

	char IFName[256];
	UINT64 RecvBytes;
	UINT64 RecvPackets;
	UINT64 SendBytes;
	UINT64 SendPackets;
	UINT NoUse;

	while (!feof(m_NetInfoFile))
	{
		int FetchCount = fscanf(m_NetInfoFile, "%*[ ]%[^:]%*[:] %llu %llu %u %u %u %u %u %u %llu %llu %u %u %u %u %u %u",
			IFName,
			&RecvBytes, &RecvPackets, &NoUse, &NoUse, &NoUse, &NoUse, &NoUse, &NoUse,
			&SendBytes, &SendPackets, &NoUse, &NoUse, &NoUse, &NoUse, &NoUse, &NoUse);
		if (FetchCount >= 17)
		{
			IFName[255] = 0;
			int IFNameLen=strlen(IFName);
			if(IFNameLen)
			{
				if(IFName[IFNameLen-1] == ':')
					IFName[IFNameLen-1] = 0;
			}
			for (UINT i = 0; i < m_NetAdapterInfoList.GetCount(); i++)
			{
				if (m_NetAdapterInfoList[i].Name.Compare(IFName) == 0)
				{
					if(TimeSpan)
					{
						m_NetAdapterInfoList[i].RecvFlux = (float)(RecvBytes - m_NetAdapterInfoList[i].RecvBytes)*1000.0f / TimeSpan;
						m_NetAdapterInfoList[i].SendFlux = (float)(SendBytes - m_NetAdapterInfoList[i].SendBytes)*1000.0f / TimeSpan;
					}
					else
					{
						m_NetAdapterInfoList[i].RecvFlux = NAN;
						m_NetAdapterInfoList[i].SendFlux = NAN;
					}
					m_NetAdapterInfoList[i].RecvBytes = RecvBytes;
					m_NetAdapterInfoList[i].SendBytes = SendBytes;
					break;
				}
			}
		}
		SkipLine(m_NetInfoFile);
	}



	return true;
}

#endif


void CNetAdapterInfos::SkipLine(FILE * pFile)
{
	int ch;
	do{
		ch = getc(pFile);
	} while (ch != '\n' && ch != EOF);
}
