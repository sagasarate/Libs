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

#define GET_IP_SECTION(IP,Section)		((IP>>(Section*8))&0xff)

class CIPPattern
{
protected:
	short	m_Pattern[4];
public:
	CIPPattern()
	{
		m_Pattern[0]=-1;
		m_Pattern[1]=-1;
		m_Pattern[2]=-1;
		m_Pattern[3]=-1;
	}
	CIPPattern(short p1,short p2,short p3,short p4)
	{
		SetPattern(p1,p2,p3,p4);
	}
	CIPPattern(LPCSTR Pattern)
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
	void SetPattern(short p1,short p2,short p3,short p4)
	{
		m_Pattern[0]=p1;
		m_Pattern[1]=p2;
		m_Pattern[2]=p3;
		m_Pattern[3]=p4;
	}
	void SetPattern(LPCSTR Pattern)
	{
		if(Pattern)
		{
			CStringSplitterA Split(Pattern,'.');

			if(Split.GetCount()>=4)
			{				
				for(int i=0;i<4;i++)
				{				
					if(strchr(Split[i],'*')!=NULL)
						m_Pattern[i]=-1;
					else
						m_Pattern[i]=atoi(Split[i]);
				}
				
			}
		}
	}
	bool IsMatch(CIPAddress& IPAddress)
	{
		return (m_Pattern[0]<0||GET_IP_SECTION(IPAddress.GetSockAddr().sin_addr.s_addr,0)==(BYTE)m_Pattern[0])&&
			(m_Pattern[1]<0||GET_IP_SECTION(IPAddress.GetSockAddr().sin_addr.s_addr,1)==(BYTE)m_Pattern[1])&&
			(m_Pattern[2]<0||GET_IP_SECTION(IPAddress.GetSockAddr().sin_addr.s_addr,2)==(BYTE)m_Pattern[2])&&
			(m_Pattern[3]<0||GET_IP_SECTION(IPAddress.GetSockAddr().sin_addr.s_addr,3)==(BYTE)m_Pattern[3]);
	}
	bool IsMatch(LPCSTR IPStr)
	{
		CIPAddress IPAddress(IPStr,0);
		return IsMatch(IPAddress);
	}	
};