#pragma once


#include "DOSMessage.h"
#include "DOSDefines.h"


class IDOSObjectProxyServiceOperator
{
public:
	virtual OBJECT_ID GetObjectID() = 0;
	virtual const CLIENT_PROXY_CONFIG& GetConfig() = 0;
	virtual CNetServer * GetNetServer() = 0;

	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual bool SendMessageMulti(OBJECT_ID * pReceiverIDList, UINT ReceiverCount, bool IsSorted, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize, UINT ReceiverCount) = 0;
	virtual bool ReleaseMessagePacket(CDOSMessagePacket * pPacket) = 0;
	virtual bool SendMessagePacket(CDOSMessagePacket * pPacket) = 0;
};

class IDOSObjectProxyService
{
public:
	virtual bool StartService(IDOSObjectProxyServiceOperator * pOperator) = 0;
	virtual void StopService() = 0;
	virtual bool PushMessage(OBJECT_ID ObjectID, CDOSMessagePacket * pPacket) = 0;
	virtual void Release() = 0;

	virtual UINT GetConnectionCount(){ return 0; }
	virtual float GetCPUUsedRate(){ return 0; }
	virtual float GetCycleTime(){ return 0; }
	virtual UINT GetGroupCount(){ return 0; }
	virtual float GetGroupCPUUsedRate(UINT Index){ return 0; }
	virtual float GetGroupCycleTime(UINT Index){ return 0; }
};

