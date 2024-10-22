﻿/****************************************************************************/
/*                                                                          */
/*      文件名:    DOSRouter.h                                              */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CDOSServer;

class CDOSRouter :
	public CEasyThread
{
protected:

	struct MSG_STATE_INFO
	{
		UINT	CurCount;
		UINT	CurFlow;
		MSG_STATE_INFO()
		{
			CurCount = 0;
			CurFlow = 0;
		}
	};

	CDOSServer										*m_pServer;
	DOS_ROUTER_CONFIG								m_Config;
	CCycleQueue<CDOSMessagePacket *>				m_MsgQueue;
	CEasyCriticalSection							m_EasyCriticalSection;
	UINT											m_MsgProcessLimit;

	volatile UINT									m_RouteInMsgCount;
	volatile UINT									m_RouteInMsgFlow;
	volatile UINT									m_RouteOutMsgCount;
	volatile UINT									m_RouteOutMsgFlow;

	CStaticMap<UINT, MSG_STATE_INFO>				m_MsgStateInfos;
	
	CThreadPerformanceCounter						m_ThreadPerformanceCounter;
	CGuardThread									m_GuardThread;

	DECLARE_CLASS_INFO_STATIC(CDOSRouter);
public:
	CDOSRouter(void);
	virtual ~CDOSRouter(void);

	bool Init(CDOSServer * pServer);

	virtual bool OnStart();
	virtual bool OnRun();
	virtual void OnTerminate();
	

	bool RouterMessage(OBJECT_ID SenderID,OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize);
	bool RouterMessage(OBJECT_ID SenderID, OBJECT_ID * pReceiverID, UINT ReceiverCount, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize);

	bool RouterMessage(CDOSMessagePacket * pPacket);

	UINT GetRouterID();
	CDOSServer * GetServer();

	UINT GetInMsgCount();
	UINT GetInMsgFlow();
	UINT GetOutMsgCount();
	UINT GetOutMsgFlow();
	UINT GetMsgQueueLen();
	float GetCycleTime();
	float GetCPUUsedRate();
	void PrintMsgStat(UINT LogChannel);
	void ResetStatData();
protected:
	int DoMessageRoute(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);	
	bool IsSameRouter(const OBJECT_ID * pReceiverIDs,int Count);
	int GetGroupCount(const OBJECT_ID * pReceiverIDs,int Count);

	bool DispatchMessage(CDOSMessagePacket * pPacket, const OBJECT_ID * pReceiverIDs,int Count);
};

inline CDOSServer * CDOSRouter::GetServer()
{
	return m_pServer;
}

inline UINT CDOSRouter::GetInMsgCount()
{
	return m_RouteInMsgCount;
}
inline UINT CDOSRouter::GetInMsgFlow()
{
	return m_RouteInMsgFlow;
}
inline UINT CDOSRouter::GetOutMsgCount()
{
	return m_RouteOutMsgCount;
}
inline UINT CDOSRouter::GetOutMsgFlow()
{
	return m_RouteOutMsgFlow;
}
inline UINT CDOSRouter::GetMsgQueueLen()
{
	return m_MsgQueue.GetUsedSize();
}
inline float CDOSRouter::GetCycleTime()
{
	return m_ThreadPerformanceCounter.GetCycleTime();
}
inline float CDOSRouter::GetCPUUsedRate()
{
	return m_ThreadPerformanceCounter.GetCPUUsedRate();
}
inline void CDOSRouter::ResetStatData()
{
	m_RouteInMsgCount = 0;
	m_RouteInMsgFlow = 0;
	m_RouteOutMsgCount = 0;
	m_RouteOutMsgFlow = 0;
}