#pragma once
class CNetAdapterInfos
{
public:
	struct NET_ADAPTER_INFO
	{
		UINT						Index;
		CEasyString					Name;
		CEasyStringA				MAC;
		CEasyArray<CIPAddress>		IPAddressList;
		UINT64						SendBytes;
		float						SendFlux;
		UINT64						RecvBytes;
		float						RecvFlux;
		NET_ADAPTER_INFO()
			:IPAddressList(_T("NET_ADAPTER_INFO"))
		{

		}
	};
protected:
	CEasyArray<NET_ADAPTER_INFO>	m_NetAdapterInfoList;
	UINT							m_RecentUpdateTime;
	CEasyBuffer						m_WorkBuffer1;
	CEasyBuffer						m_WorkBuffer2;
	FILE *							m_NetInfoFile;
public:
	CNetAdapterInfos();
	~CNetAdapterInfos();

	bool Init();
	bool FetchStatus();
	const CEasyArray<NET_ADAPTER_INFO>& GetNetAdapterInfoList()
	{
		return m_NetAdapterInfoList;
	}
protected:
	void SkipLine(FILE * pFile);
};

