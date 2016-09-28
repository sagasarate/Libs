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
	BOOL HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket);\
	virtual BOOL OnMessage(CDOSMessage * pMessage);


#define START_DOS_MSG_MAP(ClassName)	\
	void ClassName::InitDOSMsgMap()\
	{\
		m_MsgFnMap.Create(64,16384);

#define DOS_MSG_MAP(MsgHandlerClassName) \
		MsgHandlerClassName::InitMsgMap(m_MsgFnMap);

#define END_DOS_MSG_MAP(ClassName) \
	}\
	BOOL ClassName::HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket)\
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
		return COMMON_RESULT_MSG_HANDLER_NOT_FIND;\
	}\
	BOOL ClassName::OnMessage(CDOSMessage * pMessage)\
	{\
		m_CurMsgSenderID=pMessage->GetSenderID();\
		m_pCurHandleMsg=pMessage;\
		CSmartStruct MsgPacket=pMessage->GetDataPacket();\
		BOOL Ret=HandleMsg(pMessage->GetMsgID(),MsgPacket);\
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
	BOOL HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket);\
	virtual BOOL OnDOSMessage(CDOSSimpleMessage * pMessage);

#define START_DOS_MSG_MAP_CLIENT(ClassName)	\
	void ClassName::InitDOSMsgMap()\
	{\
		m_MsgFnMap.Create(64,16384);


#define DOS_MSG_MAP_CLIENT(MsgHandlerClassName) \
		MsgHandlerClassName::InitMsgMap(m_MsgFnMap);

#define END_DOS_MSG_MAP_CLIENT(ClassName) \
	}\
	BOOL ClassName::HandleMsg(MSG_ID_TYPE MsgID,CSmartStruct& MsgPacket)\
	{\
		DOS_MSG_HANDLE_INFO * pHandleInfo=m_MsgFnMap.Find(MsgID);\
		if(pHandleInfo)\
		{\
			return ((pHandleInfo->pObject)->*(pHandleInfo->pFN))(MsgPacket);\
		}\
		return COMMON_RESULT_MSG_HANDLER_NOT_FIND;\
	}\
	BOOL ClassName::OnDOSMessage(CDOSSimpleMessage * pMessage)\
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

typedef int (CBaseMsgHandler::*DOS_MSG_HANDLE_FN)(CSmartStruct& MsgPacket);

struct DOS_MSG_HANDLE_INFO
{
	CBaseMsgHandler *	pObject;
	DOS_MSG_HANDLE_FN	pFN;
};

enum COMMON_RESULT_CODE
{
	COMMON_RESULT_SUCCEED=0,	
	COMMON_RESULT_MSG_PACK_ERROR=189001,
	COMMON_RESULT_MSG_UNPACK_ERROR=189002,
	COMMON_RESULT_MSG_ALLOC_ERROR=189101,
	COMMON_RESULT_MSG_SEND_ERROR=189102,
	COMMON_RESULT_MSG_HANDLER_NOT_FIND=189103,
	COMMON_RESULT_FAILED=-2,
};

class IDistributedObject;

struct DOS_OBJECT_REGISTER_INFO_EX
{
	OBJECT_ID				ObjectID;
	LPCSTR					szObjectTypeName;
	int						Weight;
	int						ObjectGroupIndex;
	UINT					MsgQueueSize;
	UINT					MsgProcessLimit;
	IDistributedObject *	pObject;
	//UINT					Param;

	DOS_OBJECT_REGISTER_INFO_EX()
	{
		ObjectID=0;
		szObjectTypeName = NULL;
		Weight=1;
		ObjectGroupIndex=-1;
		MsgQueueSize=0;
		MsgProcessLimit=0;
		pObject=NULL;
		//Param=0;
	}
};

class IDistributedObjectOperator
{
public:
	virtual UINT GetRouterID()=0;
	virtual OBJECT_ID GetObjectID()=0;
	virtual int GetGroupIndex()=0;
	virtual BOOL SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0)=0;
	virtual BOOL SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag=0,LPCVOID pData=0,UINT DataSize=0)=0;

	virtual CDOSMessagePacket * NewMessagePacket(UINT DataSize,UINT ReceiverCount)=0;
	virtual BOOL ReleaseMessagePacket(CDOSMessagePacket * pPacket)=0;
	virtual BOOL SendMessagePacket(CDOSMessagePacket * pPacket)=0;

	virtual BOOL RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)=0;
	virtual BOOL UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)=0;
	virtual BOOL RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount) = 0;
	virtual BOOL UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount) = 0;
	virtual BOOL SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType) = 0;

	virtual BOOL AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest)=0;
	virtual BOOL DeleteConcernedObject(OBJECT_ID ObjectID)=0;

	virtual BOOL FindObject(UINT ObjectType)=0;
	virtual BOOL ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize) = 0;
	virtual BOOL CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay)=0;
	virtual BOOL RequestProxyObjectIP(OBJECT_ID ProxyObjectID)=0;

	virtual BOOL RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo)=0;
	virtual void Release()=0;

	virtual BOOL QueryShutDown(OBJECT_ID TargetID,int Level)=0;
	virtual void ShutDown(UINT PluginID)=0;

	virtual BOOL RegisterLogger(UINT LogChannel, LPCTSTR FileName) = 0;
	virtual BOOL RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader) = 0;
};

class IDistributedObject
{
protected:
	OBJECT_ID									m_CurMsgSenderID;
	CDOSMessage *								m_pCurHandleMsg;
	CHashMap<MSG_ID_TYPE,DOS_MSG_HANDLE_INFO>	m_MsgFnMap;
public:
	virtual BOOL Initialize(IDistributedObjectOperator * pOperator)=0;
	virtual void Destory()=0;
	virtual UINT AddUseRef() = 0;
	virtual void Release() = 0;

	virtual BOOL OnPreTranslateMessage(CDOSMessage * pMessage){return FALSE;}
	virtual BOOL OnMessage(CDOSMessage * pMessage){return FALSE;}
	virtual BOOL OnSystemMessage(CDOSMessage * pMessage){return FALSE;}
	virtual void OnConcernedObjectLost(OBJECT_ID ObjectID){}
	virtual BOOL OnFindObject(OBJECT_ID CallerID){return FALSE;}
	virtual void OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize){}
	virtual void OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString){}
	virtual void OnShutDown(int Level){}
	virtual int Update(int ProcessPacketLimit){return 0;}

};

class IDistributedObjectManager
{
public:
	virtual BOOL RegisterObject( DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo)=0;
};

