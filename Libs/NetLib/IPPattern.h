/****************************************************************************/
/*                                                                          */
/*      文件名:    IPPattern.h                                              */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#define GET_IP_SECTION(IPAddr,Section)		(((IPAddr.sin_addr.s_addr)>>(Section*8))&0xff)

#ifdef WIN32
#define GET_IP_SECTION6(IPAddr,Section)		(IPAddr.sin6_addr.u.Word[Section])
#else
#define GET_IP_SECTION6(IPAddr,Section)		(IPAddr.sin6_addr.s6_addr16[Section])
#endif


class CIPPattern
{
protected:
	int	m_Pattern[8];
public:
	CIPPattern()
	{
		SetMatchAll();
	}
	CIPPattern(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
	{
		SetPattern(p1, p2, p3, p4, p5, p6, p7, p8);
	}
	CIPPattern(LPCTSTR Pattern)
	{
		SetPattern(Pattern);
	}
	CIPPattern(const CIPPattern& IPPattern)
	{
		memcpy(m_Pattern,IPPattern.m_Pattern,sizeof(m_Pattern));
	}
	~CIPPattern()
	{
	}
	const CIPPattern& operator=(const CIPPattern& IPPattern)
	{
		memcpy(m_Pattern,IPPattern.m_Pattern,sizeof(m_Pattern));
		return *this;
	}
	void SetPattern(int p1, int p2, int p3, int p4, int p5, int p6, int p7, int p8)
	{
		m_Pattern[0] = p1;
		m_Pattern[1] = p2;
		m_Pattern[2] = p3;
		m_Pattern[3] = p4;
		m_Pattern[4] = p5;
		m_Pattern[5] = p6;
		m_Pattern[6] = p7;
		m_Pattern[7] = p8;
	}
	void SetPattern(LPCTSTR Pattern)
	{
		if(Pattern)
		{
			if (_tcschr(Pattern,'.'))
			{
				CStringSplitter Split(Pattern, '.');

				if (Split.GetCount() >= 4)
				{
					for (int i = 0; i < 4; i++)
					{
						if (_tcschr(Split[i], '*') != NULL)
							m_Pattern[i] = -1;
						else
							m_Pattern[i] = _ttoi(Split[i]);
					}

				}
				else
				{
					SetMatchAll();
				}
			}
			else if (_tcschr(Pattern, ':'))
			{
				CStringSplitter Split(Pattern, ':');
				if (Split.GetCount() >= 2 && Split.GetCount()<=8)
				{
					int p = 0;
					for (UINT i = 0; i < Split.GetCount(); i++)
					{
						if (Split[i][0] == 0)
						{
							for (UINT j = 0; j <= 8 - Split.GetCount(); j++)
							{
								m_Pattern[p + j] = 0;
							}							
							p += 8 - (int)Split.GetCount();
						}							
						else if (_tcschr(Split[i], '*') != NULL)
							m_Pattern[p] = -1;
						else
							m_Pattern[p] = htons((WORD)_tcstol(Split[i], NULL, 16));
						p++;
					}
				}
				else
				{
					SetMatchAll();
				}
			}
		}
	}
	void SetMatchAll()
	{
		for (int i = 0; i < 8; i++)
		{
			m_Pattern[i] = -1;
		}
	}
	bool IsMatch(const CIPAddress& IPAddress) const
	{
		if (IPAddress.IsIPv4())
		{
			return (m_Pattern[0] < 0 || GET_IP_SECTION(IPAddress.GetSockAddr4(), 0) == (BYTE)m_Pattern[0]) &&
				(m_Pattern[1] < 0 || GET_IP_SECTION(IPAddress.GetSockAddr4(), 1) == (BYTE)m_Pattern[1]) &&
				(m_Pattern[2] < 0 || GET_IP_SECTION(IPAddress.GetSockAddr4(), 2) == (BYTE)m_Pattern[2]) &&
				(m_Pattern[3] < 0 || GET_IP_SECTION(IPAddress.GetSockAddr4(), 3) == (BYTE)m_Pattern[3]);
		}
		else if (IPAddress.IsIPv6())
		{
			return (m_Pattern[0] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 0) == (WORD)m_Pattern[0]) &&
				(m_Pattern[1] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 1) == (WORD)m_Pattern[1]) &&
				(m_Pattern[2] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 2) == (WORD)m_Pattern[2]) &&
				(m_Pattern[3] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 3) == (WORD)m_Pattern[3]) &&
				(m_Pattern[4] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 4) == (WORD)m_Pattern[4]) &&
				(m_Pattern[5] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 5) == (WORD)m_Pattern[5]) &&
				(m_Pattern[6] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 6) == (WORD)m_Pattern[6]) &&
				(m_Pattern[7] < 0 || GET_IP_SECTION6(IPAddress.GetSockAddr6(), 7) == (WORD)m_Pattern[7]);
		}
		return false;
	}
	bool IsMatch(LPCTSTR IPStr) const
	{
		CIPAddress IPAddress(IPStr,0);
		return IsMatch(IPAddress);
	}	
};