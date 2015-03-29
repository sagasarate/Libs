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
		sockaddr_in	SocketAddress;
		struct
		{
			ULONG64 Address;
			ULONG64 Reserve;
		};

	};

	IP_ADDRESS m_IPAddress;
public:
	CIPAddress(void)
	{
		m_IPAddress.Address=0;
		m_IPAddress.Reserve=0;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	CIPAddress(const CIPAddress& IPAddress)
	{
		m_IPAddress=IPAddress.m_IPAddress;
	}
	CIPAddress(const sockaddr_in& SocketAddress)
	{
		m_IPAddress.SocketAddress=SocketAddress;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	CIPAddress(ULONG64 Address)
	{
		m_IPAddress.Address=Address;
		m_IPAddress.Reserve=0;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	CIPAddress(DWORD IP,WORD Port)
	{
		m_IPAddress.SocketAddress.sin_addr.s_addr=htonl(IP);
		m_IPAddress.SocketAddress.sin_port=htons(Port);
		m_IPAddress.Reserve=0;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	CIPAddress(LPCSTR szHost,WORD Port)
	{
		if(szHost)
		{
			struct hostent * pHost = gethostbyname(szHost);
			if(pHost)
			{
				m_IPAddress.SocketAddress.sin_addr.s_addr=*(ULONG *)pHost->h_addr_list[0];
			}
		}
		else
		{
			m_IPAddress.SocketAddress.sin_addr.s_addr=htonl( INADDR_ANY );
		}
		m_IPAddress.SocketAddress.sin_port= htons(Port);
		m_IPAddress.Reserve=0;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	~CIPAddress(void)
	{
	}
	void Clear()
	{
		m_IPAddress.Address=0;
		m_IPAddress.Reserve=0;
		m_IPAddress.SocketAddress.sin_family=PF_INET;
	}
	static bool FetchAllHostAddress(CEasyArray<CIPAddress>& AddressList)
	{
		char szHost[256];
		if(gethostname(szHost,256)==0)
		{
			struct hostent * pHost = gethostbyname(szHost);
			if(pHost)
			{
				int Index=0;
				while(pHost->h_addr_list[Index])
				{
					CIPAddress * pAddress=AddressList.AddEmpty();
					pAddress->m_IPAddress.SocketAddress.sin_addr.s_addr=*(ULONG *)pHost->h_addr_list[Index];
					Index++;
				}
			}
		}
		return false;
	}

	CIPAddress& operator=(const CIPAddress& IPAddress)
	{
		m_IPAddress=IPAddress.m_IPAddress;
		return *this;
	}

	bool operator==(const CIPAddress& IPAddress)
	{
		return m_IPAddress.Address==IPAddress.m_IPAddress.Address;
	}

	bool operator!=(const CIPAddress& IPAddress)
	{
		return m_IPAddress.Address!=IPAddress.m_IPAddress.Address;
	}

	operator sockaddr_in() const
	{
		return m_IPAddress.SocketAddress;
	}

	const sockaddr_in& GetSockAddr() const
	{
		return m_IPAddress.SocketAddress;
	}

	operator ULONG64() const
	{
		return m_IPAddress.Address;
	}

	DWORD GetIP() const
	{
		return ntohl(m_IPAddress.SocketAddress.sin_addr.s_addr);
	}

	LPCSTR GetIPString() const
	{
		return inet_ntoa(m_IPAddress.SocketAddress.sin_addr);
	}

	WORD GetPort() const
	{
		return ntohs(m_IPAddress.SocketAddress.sin_port);
	}

	void SetIP(DWORD IP)
	{
		m_IPAddress.SocketAddress.sin_addr.s_addr=htonl(IP);
	}

	void SetIP(LPCSTR IP)
	{
		if(IP)
		{
			if(_stricmp(IP,"0") == 0)
			{
				m_IPAddress.SocketAddress.sin_addr.s_addr=0;
			}
			else
			{
				struct hostent * pHost = gethostbyname(IP);
				if(pHost)
				{
					m_IPAddress.SocketAddress.sin_addr.s_addr=*(ULONG *)pHost->h_addr_list[0];
				}
				else
				{
					m_IPAddress.SocketAddress.sin_addr.s_addr=0;
				}
			}			
		}
		
	}

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
