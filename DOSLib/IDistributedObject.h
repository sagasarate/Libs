/****************************************************************************/
/*                                                                          */
/*      文件名:    IDistributedObject.h                                     */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

#include "DOSMessage.h"

#define MAKE_MSG_ID(ModuleID,InterfaceID,MethodID,IsAck)		((((DWORD)ModuleID&0x7F)<<24)|(((DWORD)InterfaceID&0xFF)<<16)|(((DWORD)MethodID)&0xFFFF)|(IsAck?0x80000000:0))
#define GET_MODULE_ID(MsgID)									((((DWORD)MsgID)>>24)&0x7F)
#define GET_INTERFACE_ID(MsgID)									((((DWORD)MsgID)>>16)&0xFF)
#define GET_METHOD_ID(MsgID)									((DWORD)MsgID&0xFFFF)
#define IS_ACK_MSG(MsgID)										((DWORD)MsgID&0x7FFFFFFF)




#define DECLARE_DOS_MSG_MAP(ClassName)		\
	protected:\
	OBJECT_ID& GetCurMsgSenderID()\
	{\
		return m_CurMsgSenderID;\
	}\
	CDOSMessage * GetCurMsg()\
	{\
		return m_pCurHandleMsg;\
	}\
	void InitDOSMsgMap();\
	int HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket);\
	virtual int OnMessage(CDOSMessage * pMessage);


#define START_DOS_MSG_MAP(ClassName)	\
	void ClassName::InitDOSMsgMap()\
	{\
		m_MsgFnMap.SetTag(_T(#ClassName));\
		m_MsgFnMap.Create(64,16384);

#define DOS_MSG_MAP(MsgHandlerClassName) \
		MsgHandlerClassName::InitMsgMap(m_MsgFnMap);

#define END_DOS_MSG_MAP(ClassName) \
	}\
	int ClassName::HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket)\
	{\
		DOS_MSG_HANDLE_INFO * pHandleInfo=m_MsgFnMap.Find(MsgID);\
		if(pHandleInfo)\
		{\
			return ((pHandleInfo->pObject)->*(pHandleInfo->pFN))(MsgPacket);\
		}\
		else\
		{\
			PrintDOSDebugLog(_T("[0x%llX]无法找到消息[0x%X]的处理函数"), m_pOperator->GetObjectID(), MsgID);\
		}\
		return COMMON_RESULT_MSG_NO_HANDLER;\
	}\
	int ClassName::OnMessage(CDOSMessage * pMessage)\
	{\
		m_CurMsgSenderID=pMessage->GetSenderID();\
		m_pCurHandleMsg=pMessage;\
		CSmartStruct MsgPacket=pMessage->GetDataPacket();\
		int Ret=HandleMsg(pMessage->GetMsgID(),MsgPacket);\
		m_CurMsgSenderID=0;\
		m_pCurHandleMsg=NULL;\
		return Ret;\
	}

#define INIT_DOS_MSG_MAP	InitDOSMsgMap();



#define DECLARE_DOS_MSG_MAP_CLIENT(ClassName) \
	protected:\
	CDOSSimpleMessage * GetCurMsg()\
	{\
		return m_pCurHandleMsg;\
	}\
	void InitDOSMsgMap();\
	int HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket);\
	virtual int OnDOSMessage(CDOSSimpleMessage * pMessage);

#define START_DOS_MSG_MAP_CLIENT(ClassName)	\
	void ClassName::InitDOSMsgMap()\
	{\
		m_MsgFnMap.SetTag(_T(#ClassName));\
		m_MsgFnMap.Create(64,16384);


#define DOS_MSG_MAP_CLIENT(MsgHandlerClassName) \
		MsgHandlerClassName::InitMsgMap(m_MsgFnMap);

#define END_DOS_MSG_MAP_CLIENT(ClassName) \
	}\
	int ClassName::HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket)\
	{\
		DOS_MSG_HANDLE_INFO * pHandleInfo=m_MsgFnMap.Find(MsgID);\
		if(pHandleInfo)\
		{\
			return ((pHandleInfo->pObject)->*(pHandleInfo->pFN))(MsgPacket);\
		}\
		return COMMON_RESULT_MSG_NO_HANDLER;\
	}\
	int ClassName::OnDOSMessage(CDOSSimpleMessage * pMessage)\
	{\
		m_pCurHandleMsg=pMessage;\
		return HandleMsg(pMessage->GetMsgID(),pMessage->GetDataPacket());\
	}


class CBaseMsgHandler
{
public:
	CBaseMsgHandler()
	{

	}
	virtual ~CBaseMsgHandler()
	{

	}

};

typedef int (CBaseMsgHandler::* DOS_MSG_HANDLE_FN)(CSmartStruct& MsgPacket);

struct DOS_MSG_HANDLE_INFO
{
	CBaseMsgHandler* pObject;
	DOS_MSG_HANDLE_FN	pFN;
};

enum COMMON_RESULT_CODE
{
	COMMON_RESULT_SUCCEED = 0,
	COMMON_RESULT_FAILED = -2,
	COMMON_RESULT_MSG_PACK_ERROR = -3,
	COMMON_RESULT_MSG_UNPACK_ERROR = -4,
	COMMON_RESULT_MSG_ALLOC_ERROR = -5,
	COMMON_RESULT_MSG_SEND_ERROR = -6,
	COMMON_RESULT_MSG_NO_HANDLER = -7,
};



class IDistributedObject;

struct DOS_OBJ_REGISTER_INFO
{
	OBJECT_ID				ObjectID;
	LPCSTR					szObjectTypeName;
	int						Weight;
	int						ObjectGroupIndex;
	UINT					MsgQueueSize;
	UINT					MsgProcessLimit;
	UINT					Flag;
	IDistributedObject* pObject;


	DOS_OBJ_REGISTER_INFO()
	{
		ObjectID = 0;
		szObjectTypeName = NULL;
		Weight = 1;
		ObjectGroupIndex = -1;
		MsgQueueSize = 0;
		MsgProcessLimit = 0;
		Flag = 0;
		pObject = NULL;
	}
};

enum DOS_OBJECT_REGISTER_FLAG
{
	DOS_OBJECT_REGISTER_FLAG_USE_PRIVATE_OBJECT_GROUP = 1,
	DOS_OBJECT_REGISTER_FLAG_SEND_DESTORY_NOTIFY = (1 << 1),
	DOS_OBJECT_REGISTER_FLAG_BROADCAST_DESTORY_NOTIFY = (1 << 2),
};

class IDistributedObjectOperator
{
public:
	virtual UINT GetRouterID() = 0;
	virtual OBJECT_ID GetObjectID() = 0;
	virtual int GetGroupIndex() = 0;
	virtual bool SendMessage(OBJECT_ID ReceiverID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual bool SendMessageMulti(OBJECT_ID* pReceiverIDList, UINT ReceiverCount, bool IsSorted, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag = 0, LPCVOID pData = 0, UINT DataSize = 0) = 0;
	virtual bool BroadcastMessageToProxyByMask(WORD RouterID, BYTE ProxyType, UINT64 Mask, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize) = 0;
	virtual bool BroadcastMessageToProxyByGroup(WORD RouterID, BYTE ProxyType, UINT64 GroupID, OBJECT_ID SenderID, MSG_ID_TYPE MsgID, WORD MsgFlag, LPCVOID pData, UINT DataSize) = 0;

	virtual CDOSMessagePacket* NewMessagePacket(UINT DataSize, UINT ReceiverCount) = 0;
	virtual bool ReleaseMessagePacket(CDOSMessagePacket* pPacket) = 0;
	virtual bool SendMessagePacket(CDOSMessagePacket* pPacket) = 0;

	virtual bool RegisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE* pMsgIDList, int CmdCount) = 0;
	virtual bool UnregisterMsgMap(OBJECT_ID ProxyObjectID, MSG_ID_TYPE* pMsgIDList, int CmdCount) = 0;
	virtual bool RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType) = 0;
	virtual bool UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgIDList) = 0;
	virtual bool SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType) = 0;

	virtual bool AddConcernedObject(OBJECT_ID ObjectID, bool NeedTest) = 0;
	virtual bool DeleteConcernedObject(OBJECT_ID ObjectID) = 0;

	virtual bool FindObject(UINT ObjectType, bool OnlyLocal) = 0;
	virtual bool ReportObject(OBJECT_ID TargetID, const void* pObjectInfoData, UINT DataSize) = 0;
	virtual bool CloseProxyObject(OBJECT_ID ProxyObjectID, UINT Delay) = 0;
	virtual bool RequestProxyObjectIP(OBJECT_ID ProxyObjectID) = 0;

	virtual bool RegisterObject(DOS_OBJ_REGISTER_INFO& ObjectRegisterInfo) = 0;
	virtual void Release() = 0;

	virtual bool QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param) = 0;
	virtual void ShutDown(UINT PluginID) = 0;
	virtual bool RegisterCommandReceiver() = 0;
	virtual bool UnregisterCommandReceiver() = 0;

	virtual bool RegisterLogger(UINT LogChannel, LPCTSTR FileName) = 0;
	virtual bool RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader) = 0;

	virtual void SetServerWorkStatus(BYTE WorkStatus) = 0;
	virtual UINT AddTimer(UINT64 TimeOut, UINT64 Param, bool IsRepeat) = 0;
	virtual bool DeleteTimer(UINT ID) = 0;

	virtual bool SetBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) = 0;
	virtual bool AddBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) = 0;
	virtual bool RemoveBroadcastMask(OBJECT_ID ProxyObjectID, UINT64 Mask) = 0;
	virtual bool AddBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID) = 0;
	virtual bool RemoveBroadcastGroup(OBJECT_ID ProxyObjectID, UINT64 GroupID) = 0;
};

class IDistributedObject
{
protected:
	OBJECT_ID									m_CurMsgSenderID;
	CDOSMessage* m_pCurHandleMsg;
	CHashMap<MSG_ID_TYPE, DOS_MSG_HANDLE_INFO>	m_MsgFnMap;
public:
	virtual bool Initialize(IDistributedObjectOperator* pOperator) = 0;
	virtual void Destory() = 0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual int OnPreTranslateMessage(CDOSMessage* pMessage) { return COMMON_RESULT_MSG_NO_HANDLER; }
	virtual int OnMessage(CDOSMessage* pMessage) { return COMMON_RESULT_MSG_NO_HANDLER; }
	virtual int OnSystemMessage(CDOSMessage* pMessage) { return COMMON_RESULT_MSG_NO_HANDLER; }
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID) {}
	virtual bool OnFindObject(OBJECT_ID CallerID) { return false; }
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void* pObjectInfoData, UINT DataSize) {}
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString) {}
	virtual void OnShutDown(BYTE Level, UINT Param) {}
	virtual int Update(int ProcessPacketLimit) { return 0; }
	virtual bool OnConsoleCommand(LPCTSTR szCommand) { return false; }
	virtual void OnTimer(UINT ID, UINT64 Param, bool IsRepeat) {};
	virtual void OnTimerRelease(UINT ID, UINT64 Param) {}
};

class IDistributedObjectManager
{
public:
	virtual bool RegisterObject(DOS_OBJ_REGISTER_INFO& ObjectRegisterInfo) = 0;
};

