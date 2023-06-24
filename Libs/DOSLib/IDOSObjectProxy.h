#pragma once


#include "DOSMessage.h"
#include "DOSDefines.h"


class IDOSObjectProxyServiceOperator
{
public:	

	virtual const CLIENT_PROXY_CONFIG& GetConfig() = 0;
	virtual CNetServer * GetNetServer() = 0;
	virtual UINT GetRouterID() = 0;

	virtual bool SendMessage(OBJECT_ID SenderID, OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual bool SendMessageMulti(OBJECT_ID SenderID, OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount) = 0;
	virtual bool ReleaseMessagePacket(CDOSMessagePacket * pPacket) = 0;
	virtual bool SendMessagePacket(CDOSMessagePacket * pPacket) = 0;
};

class IDOSObjectProxyService
{
public:
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual bool StartService(IDOSObjectProxyServiceOperator * pOperator) = 0;
	virtual void StopService() = 0;
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket) = 0;

	virtual UINT GetConnectionCount() = 0;
	virtual float GetCPUUsedRate() = 0;
	virtual float GetCycleTime() = 0;
	virtual UINT GetMsgQueueLen() = 0;
	virtual UINT GetGroupCount() = 0;
	virtual float GetGroupCPUUsedRate(UINT Index) = 0;
	virtual float GetGroupCycleTime(UINT Index) = 0;
	virtual UINT GetGroupMsgQueueLen(UINT Index) = 0;
};

