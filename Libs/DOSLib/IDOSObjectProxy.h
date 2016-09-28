#pragma once


#include "DOSMessage.h"
#include "DOSDefines.h"


class IDOSObjectProxyConnectionOperator
{
public:
	virtual OBJECT_ID GetObjectID() = 0;
	virtual const CLIENT_PROXY_CONFIG& GetConfig() = 0;

	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual bool SendDisconnectNotify(OBJECT_ID * pTargetObjectID, UINT TargetCount) = 0;
};

class IDOSObjectProxyConnectionBase
{
public:
	virtual void Destory() = 0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual bool PushMessage(CDOSMessagePacket * pPacket){ return false; }
};

class IDOSObjectProxyConnection 
{
public:
	virtual bool Initialize(IDOSObjectProxyConnectionOperator * pOperator) = 0;
	virtual void Destory() = 0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual bool OnMessage(CDOSMessage * pMessage) = 0;
	virtual bool OnSystemMessage(CDOSMessage * pMessage){ return false; }
	virtual OBJECT_ID GetMsgMap(MSG_ID_TYPE MsgID) = 0;
	virtual bool OnRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID) = 0;
	virtual bool OnUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID) = 0;
	virtual void OnClearMsgMapByRouterID(UINT RouterID) = 0;
	virtual int Update(int ProcessPacketLimit) = 0;
	virtual void QueryDisconnect(UINT Delay) = 0;
	virtual bool IsConnected() = 0;
	virtual bool GetRemoteAddress(CIPAddress& Address) = 0;
};


class IDOSObjectProxyServiceOperator
{
public:
	virtual OBJECT_ID GetObjectID() = 0;
	virtual const CLIENT_PROXY_CONFIG& GetConfig() = 0;
	virtual CNetServer * GetNetServer() = 0;

	virtual bool SendMessage(OBJECT_ID ReceiverID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual IDOSObjectProxyConnection * CreateConnection(CIPAddress& RemoteAddress) = 0;
	virtual bool DeleteConnection(UINT ID) = 0;
};

class IDOSObjectProxyServiceBase
{
public:
	virtual void Destory() = 0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;
	virtual BYTE GetProxyType() = 0;
	virtual void SetID(UINT ID) = 0;
	virtual UINT GetID() = 0;

	virtual bool StartService(){ return false; }
	virtual void StopService(){}
	virtual bool PushMessage(CDOSMessagePacket * pPacket){ return false; }
	virtual bool PushBroadcastMessage(CDOSMessagePacket * pPacket){ return false; }
	virtual IDOSObjectProxyConnectionBase * GetConnection(UINT ID) { return NULL; }

	virtual UINT GetConnectionCount(){ return 0; }
	virtual float GetCPUUsedRate(){ return 0; }
	virtual float GetCycleTime(){ return 0; }
	virtual UINT GetGroupCount(){ return 0; }
	virtual float GetGroupCPUUsedRate(UINT Index){ return 0; }
	virtual float GetGroupCycleTime(UINT Index){ return 0; }
};

class IDOSObjectProxyService
{
public:
	virtual bool Initialize(IDOSObjectProxyServiceOperator * pOperator) = 0;
	virtual void Destory() = 0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual bool OnMessage(CDOSMessage * pMessage) = 0;
	virtual bool OnSystemMessage(CDOSMessage * pMessage){ return false; }	
	//virtual OBJECT_ID GetMsgMap(MSG_ID_TYPE MsgID) = 0;	
	virtual bool OnRegisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID) = 0;
	virtual bool OnUnregisterMsgMap(MSG_ID_TYPE MsgID, OBJECT_ID ObjectID) = 0;
	virtual void SetUnhandleMsgReceiver(OBJECT_ID ObjectID) = 0;
	//virtual OBJECT_ID GetUnhandleMsgReceiver() = 0;
	virtual void OnClearMsgMapByRouterID(UINT RouterID) = 0;
	virtual int Update(int ProcessPacketLimit) = 0;
};