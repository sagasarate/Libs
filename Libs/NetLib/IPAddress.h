/****************************************************************************/
/*                                                                          */
/*      文件名:    IPAddress.h                                              */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CIPAddress
{
protected:

	union IP_ADDRESS
	{
		sockaddr_in6 SocketAddress6;
		sockaddr_in	SocketAddress;
	};

	IP_ADDRESS m_IPAddress;
public:
	CIPAddress(void)
	{
		ZeroMemory(&m_IPAddress, sizeof(m_IPAddress));
		m_IPAddress.SocketAddress.sin_family=AF_INET;
	}
	CIPAddress(const CIPAddress& IPAddress)
	{
		m_IPAddress=IPAddress.m_IPAddress;
	}
	CIPAddress(const sockaddr_in& SocketAddress)
	{
		m_IPAddress.SocketAddress=SocketAddress;
		m_IPAddress.SocketAddress.sin_family=AF_INET;
	}
	CIPAddress(const sockaddr_in6& SocketAddress)
	{
		m_IPAddress.SocketAddress6 = SocketAddress;
		m_IPAddress.SocketAddress6.sin6_family = AF_INET6;
	}
	CIPAddress(DWORD IP,WORD Port)
	{
		ZeroMemory(&m_IPAddress, sizeof(m_IPAddress));
		m_IPAddress.SocketAddress.sin_addr.s_addr=htonl(IP);
		m_IPAddress.SocketAddress.sin_port=htons(Port);
		m_IPAddress.SocketAddress.sin_family=AF_INET;
	}
	CIPAddress(LPCTSTR szHost, WORD Port, bool IsIPv6 = false)
	{
		SetAddress(szHost, Port, IsIPv6);
	}
	~CIPAddress(void)
	{
	}
	void Clear()
	{
		ZeroMemory(&m_IPAddress, sizeof(m_IPAddress));
		m_IPAddress.SocketAddress.sin_family=AF_INET;
	}


	CIPAddress& operator=(const CIPAddress& IPAddress)
	{
		m_IPAddress=IPAddress.m_IPAddress;
		return *this;
	}

	bool operator==(const CIPAddress& IPAddress)
	{
		if (m_IPAddress.SocketAddress6.sin6_family == IPAddress.m_IPAddress.SocketAddress6.sin6_family)
		{
			if (m_IPAddress.SocketAddress6.sin6_family == AF_INET)
			{
                return m_IPAddress.SocketAddress.sin_addr.s_addr == IPAddress.m_IPAddress.SocketAddress.sin_addr.s_addr&&
					m_IPAddress.SocketAddress.sin_port == IPAddress.m_IPAddress.SocketAddress.sin_port;
			}
			else
			{
				return memcmp(&m_IPAddress.SocketAddress6.sin6_addr, &IPAddress.m_IPAddress.SocketAddress6.sin6_addr, 16) == 0 &&
					m_IPAddress.SocketAddress6.sin6_port == IPAddress.m_IPAddress.SocketAddress6.sin6_port;
			}
		}
		else
		{
			return false;
		}
	}

	bool operator!=(const CIPAddress& IPAddress)
	{
		return !((*this) == IPAddress);
	}

	const sockaddr * GetSockAddrPtr() const
	{
		return (sockaddr *)&m_IPAddress;
	}

	sockaddr * GetSockAddrPtr()
	{
		return (sockaddr *)&m_IPAddress;
	}

	operator sockaddr_in() const
	{
		return m_IPAddress.SocketAddress;
	}

	const sockaddr_in& GetSockAddr4() const
	{
		return m_IPAddress.SocketAddress;
	}

	operator sockaddr_in6() const
	{
		return m_IPAddress.SocketAddress6;
	}

	const sockaddr_in6& GetSockAddr6() const
	{
		return m_IPAddress.SocketAddress6;
	}

	bool IsIPv4() const
	{
		return m_IPAddress.SocketAddress.sin_family == AF_INET;
	}

	bool IsIPv6() const
	{
		return m_IPAddress.SocketAddress6.sin6_family == AF_INET6;
	}
	bool IsAnyAddress() const
	{
		if (m_IPAddress.SocketAddress.sin_family == AF_INET)
		{
			return m_IPAddress.SocketAddress.sin_addr.s_addr == 0;
		}
		else if (m_IPAddress.SocketAddress6.sin6_family == AF_INET6)
		{
			return IN6_IS_ADDR_UNSPECIFIED(&m_IPAddress.SocketAddress6.sin6_addr);
		}
		else
		{
			return false;
		}
	}

	bool IsLoopbackAddress() const
	{
		if (m_IPAddress.SocketAddress.sin_family == AF_INET)
		{
			return m_IPAddress.SocketAddress.sin_addr.s_addr == 0x0100007f;
		}
		else if (m_IPAddress.SocketAddress6.sin6_family == AF_INET6)
		{
			return IN6_IS_ADDR_LOOPBACK(&m_IPAddress.SocketAddress6.sin6_addr);
		}
		else
		{
			return false;
		}
	}

	bool IsLocalAddress() const
	{
		if (m_IPAddress.SocketAddress.sin_family == AF_INET)
		{
			BYTE B1 = m_IPAddress.SocketAddress.sin_addr.s_addr & 0xFF;
			if (B1 == 10)
			{
				return true;
			}
			else
			{
				BYTE B2 = (m_IPAddress.SocketAddress.sin_addr.s_addr >> 8) & 0xFF;
				if (B1 == 172 && B2 >= 16 && B2 <= 31)
					return true;
				else if (B1 == 192 && B2 == 168)
					return true;
			}
			return false;
		}
		else if (m_IPAddress.SocketAddress6.sin6_family == AF_INET6)
		{
			return IN6_IS_ADDR_LINKLOCAL(&m_IPAddress.SocketAddress6.sin6_addr) || IN6_IS_ADDR_SITELOCAL(&m_IPAddress.SocketAddress6.sin6_addr);
		}
		else
		{
			return false;
		}
	}

	UINT GetIPv4() const
	{
		return ntohl(m_IPAddress.SocketAddress.sin_addr.s_addr);
	}

	LPCTSTR GetIPString() const
	{
		static TCHAR Buff[1024];
		GetIPString(Buff, 1024);
		return Buff;
	}
	LPCTSTR GetAddressString() const
	{
		static TCHAR Buff[1024];
		GetAddressString(Buff, 1024);
		return Buff;
	}

	void GetIPString(LPTSTR IPString, size_t BuffLen) const
	{
		if (m_IPAddress.SocketAddress.sin_family == AF_INET)
			InetNtop(AF_INET, (LPVOID)&m_IPAddress.SocketAddress.sin_addr, IPString, BuffLen);
		else
			InetNtop(AF_INET6, (LPVOID)&m_IPAddress.SocketAddress6.sin6_addr, IPString, BuffLen);
	}

	void GetAddressString(LPTSTR AddrString, size_t BuffLen) const
	{
		AddrString[0] = 0;
		GetIPString(AddrString, BuffLen);
		size_t IPStrLen = _tcslen(AddrString);
		if (BuffLen - IPStrLen > 1)
		{
			AddrString[IPStrLen] = '@';
			AddrString[IPStrLen + 1] = 0;
		}
		if (BuffLen - IPStrLen - 1 > 1)			
			_itot_s(GetPort(), AddrString + IPStrLen + 1, BuffLen - IPStrLen - 2, 10);
		AddrString[BuffLen - 1] = 0;
	}

	WORD GetPort() const
	{
		return ntohs(m_IPAddress.SocketAddress.sin_port);
	}

	void SetIP(LPCTSTR IP)
	{
		if(IP)
		{
			if(_tcsicmp(IP,_T("0")) == 0)
			{
				m_IPAddress.SocketAddress.sin_family = AF_INET;
				m_IPAddress.SocketAddress.sin_addr.s_addr=0;
			}
			else
			{
				SetAddress(IP, (LPCTSTR)NULL, false);
			}
		}

	}

	void SetIPv4(const BYTE * pIPData)
	{
		m_IPAddress.SocketAddress.sin_family = AF_INET;
		memcpy(&m_IPAddress.SocketAddress.sin_addr, pIPData, sizeof(m_IPAddress.SocketAddress.sin_addr));
	}

	void SetIPv6(const BYTE * pIPData)
	{
		m_IPAddress.SocketAddress6.sin6_family = AF_INET6;
		memcpy(&m_IPAddress.SocketAddress6.sin6_addr, pIPData, sizeof(m_IPAddress.SocketAddress6.sin6_addr));
	}

	void SetAddress(const sockaddr_in& SocketAddress)
	{
		m_IPAddress.SocketAddress = SocketAddress;
	}

	void SetAddress(const sockaddr_in6& SocketAddress)
	{
		m_IPAddress.SocketAddress6 = SocketAddress;
	}

	void SetAddress(const sockaddr * pSocketAddress, size_t Len)
	{
		if (pSocketAddress->sa_family == AF_INET && Len >= sizeof(sockaddr_in))
			SetAddress(*((const sockaddr_in *)pSocketAddress));
		else if (pSocketAddress->sa_family == AF_INET6 && Len >= sizeof(sockaddr_in6))
			SetAddress(*((const sockaddr_in6 *)pSocketAddress));
	}


	bool SetAddress(LPCTSTR szAddress, LPCTSTR szPort, bool IsIPv6)
	{
		ADDRINFOT hints;
		ADDRINFOT * pInfos;

		if (szAddress)
		{
			//自动检测IPv6地址
			if (_tcschr(szAddress, ':'))
				IsIPv6 = true;
		}

		memset(&hints, 0, sizeof(struct addrinfo));
		hints.ai_family = IsIPv6 ? AF_INET6 : AF_INET;     /* Allow IPv4 */
		hints.ai_flags = AI_PASSIVE;/* For wildcard IP address */
		hints.ai_protocol = 0;         /* Any protocol */
		hints.ai_socktype = SOCK_STREAM;

		if (GetAddrInfo(szAddress, szPort, &hints, &pInfos) == 0)
		{
			if (pInfos)
			{
				m_IPAddress.SocketAddress = *((struct sockaddr_in *)pInfos->ai_addr);
			}
			FreeAddrInfo(pInfos);
			return true;
		}
		return false;
	}

	bool SetAddress(LPCTSTR szAddress, WORD Port, bool IsIPv6)
	{
		if (SetAddress(szAddress, (LPCTSTR)NULL, IsIPv6))
		{
			SetPort(Port);
			return true;
		}
		return false;
	}


#ifdef WIN32
	static bool FetchAllHostAddress(CEasyArray<CIPAddress>& AddressList)
	{
		TCHAR szHost[256];
		if (GetHostName(szHost, 256) == 0)
		{
			ADDRINFOT hints;
			ADDRINFOT * pInfos;

			memset(&hints, 0, sizeof(struct addrinfo));
			hints.ai_family = AF_UNSPEC;
			hints.ai_flags = AI_PASSIVE;
			hints.ai_protocol = 0;
			hints.ai_socktype = SOCK_STREAM;

			if (GetAddrInfo(szHost, NULL, &hints, &pInfos) == 0)
			{
				for (ADDRINFOT * Ptr = pInfos; Ptr != NULL; Ptr = Ptr->ai_next)
				{
					if (Ptr->ai_family == AF_INET)
					{
						CIPAddress * pAddress = AddressList.AddEmpty();
						pAddress->SetAddress(*((struct sockaddr_in *)Ptr->ai_addr));
					}
					else if (Ptr->ai_family == AF_INET6)
					{
						struct sockaddr_in6 * pAddr = (struct sockaddr_in6 *)Ptr->ai_addr;
						if (!IN6_IS_ADDR_MULTICAST(&pAddr->sin6_addr) && !IN6_IS_ADDR_LOOPBACK(&pAddr->sin6_addr) 
							&& !IN6_IS_ADDR_UNSPECIFIED(&pAddr->sin6_addr))
						{
							CIPAddress * pAddress = AddressList.AddEmpty();
							pAddress->SetAddress(*pAddr);
						}

					}
				}
				FreeAddrInfo(pInfos);
				return true;
			}
		}
		return false;
	}
#else
	static bool FetchAllHostAddress(CEasyArray<CIPAddress>& AddressList)
	{
		struct ifaddrs * pIFInfo=NULL, * pCurInfo=NULL;

		int Ret=getifaddrs(&pIFInfo);
		if (Ret!=-1)
		{
			for (pCurInfo = pIFInfo; pCurInfo != NULL; pCurInfo = pCurInfo->ifa_next)
			{
				if (_strnicmp(pCurInfo->ifa_name, "eth", 3) == 0)
				{
					//if name is eth...
					if (pCurInfo->ifa_addr->sa_family == AF_INET)
					{
						CIPAddress * pAddress = AddressList.AddEmpty();
						pAddress->SetAddress(*((struct sockaddr_in *)pCurInfo->ifa_addr));
					}
					else if (pCurInfo->ifa_addr->sa_family == AF_INET6)
					{
						struct sockaddr_in6 * pAddr = (struct sockaddr_in6 *)pCurInfo->ifa_addr;
						if (!IN6_IS_ADDR_MULTICAST(&pAddr->sin6_addr) && !IN6_IS_ADDR_LOOPBACK(&pAddr->sin6_addr) 
							&& !IN6_IS_ADDR_UNSPECIFIED(&pAddr->sin6_addr))
						{
							CIPAddress * pAddress = AddressList.AddEmpty();
							pAddress->SetAddress(*pAddr);
						}

					}
				}
			}
			freeifaddrs(pIFInfo);
			return true;
		}
		else
		{
			PrintNetLog(_T("NetLib"),"getifaddrs 发生错误 %d！",errno);
			return false;
		}
	}
#endif


	void SetPort(WORD Port)
	{
		m_IPAddress.SocketAddress.sin_port=htons(Port);
	}

};

class CIPAddressPair
{
protected:
	CIPAddress	m_LocalIPAddress;
	CIPAddress	m_RemoteIPAddress;
public:
	CIPAddressPair()
	{
	}
	CIPAddressPair(const CIPAddressPair& IPAddressPair)
	{
		m_LocalIPAddress=IPAddressPair.m_LocalIPAddress;
		m_RemoteIPAddress=IPAddressPair.m_RemoteIPAddress;
	}
	CIPAddressPair(const CIPAddress& LocalIPAddress,const CIPAddress& RemoteIPAddress)
	{
		m_LocalIPAddress=LocalIPAddress;
		m_RemoteIPAddress=RemoteIPAddress;
	}
	~CIPAddressPair()
	{
	}
	void Clear()
	{
		m_LocalIPAddress.Clear();
		m_RemoteIPAddress.Clear();
	}
	CIPAddressPair& operator=(const CIPAddressPair& IPAddressPair)
	{
		m_LocalIPAddress=IPAddressPair.m_LocalIPAddress;
		m_RemoteIPAddress=IPAddressPair.m_RemoteIPAddress;
		return *this;
	}
	bool operator==(const CIPAddressPair& IPAddressPair)
	{
		return m_LocalIPAddress==IPAddressPair.m_LocalIPAddress&&
			m_RemoteIPAddress==IPAddressPair.m_RemoteIPAddress;
	}
	bool operator!=(const CIPAddressPair& IPAddressPair)
	{
		return m_LocalIPAddress!=IPAddressPair.m_LocalIPAddress||
			m_RemoteIPAddress!=IPAddressPair.m_RemoteIPAddress;
	}
	void SetLocalIPAddress(const CIPAddress& LocalIPAddress)
	{
		m_LocalIPAddress=LocalIPAddress;
	}
	void SetRemoteIPAddress(const CIPAddress& RemoteIPAddress)
	{
		m_RemoteIPAddress=RemoteIPAddress;
	}
	CIPAddress& GetLocalIPAddress()
	{
		return m_LocalIPAddress;
	}
	CIPAddress& GetRemoteIPAddress()
	{
		return m_RemoteIPAddress;
	}
};
