#include "stdafx.h"


CDistributedObjectOperator::CDistributedObjectOperator()
{
	FUNCTION_BEGIN;
	m_pManager=NULL;
	m_pDistributedObject=NULL;
	m_PoolID=0;
	FUNCTION_END;
}

CDistributedObjectOperator::~CDistributedObjectOperator()
{
}

BOOL CDistributedObjectOperator::Init(CDistributedObjectManager * pManager,UINT PoolID,IDistributedObject * pDistributedObject)
{
	FUNCTION_BEGIN;
	m_pManager=pManager;
	m_pDistributedObject=pDistributedObject;
	m_pDistributedObject->AddUseRef();
	m_PoolID=PoolID;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

bool CDistributedObjectOperator::Initialize()
{
	FUNCTION_BEGIN;
	if(!CDOSBaseObject::Initialize())
		return false;
	return m_pDistributedObject->Initialize(this)!=FALSE;
	FUNCTION_END;
	return false;
}

void CDistributedObjectOperator::Destory()
{
	FUNCTION_BEGIN;
	m_pDistributedObject->Destory();
	SAFE_RELEASE(m_pDistributedObject);
	if(m_pManager)
		m_pManager->DeleteObjectOperator(this);
	m_PoolID=0;
	m_pManager=NULL;
	CDOSBaseObject::Destory();
	FUNCTION_END;
}


UINT CDistributedObjectOperator::GetRouterID()
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::GetRouterID();
	FUNCTION_END;
	return 0;
}

OBJECT_ID CDistributedObjectOperator::GetObjectID()
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::GetObjectID();
	FUNCTION_END;
	return 0;
}

int CDistributedObjectOperator::GetGroupIndex()
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::GetGroupIndex();
	FUNCTION_END;
	return -1;
}

BOOL CDistributedObjectOperator::SendMessage(OBJECT_ID ReceiverID,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::SendMessage(ReceiverID,MsgID,MsgFlag,pData,DataSize);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::SendMessageMulti(OBJECT_ID * pReceiverIDList,UINT ReceiverCount,bool IsSorted,MSG_ID_TYPE MsgID,WORD MsgFlag,LPCVOID pData,UINT DataSize)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::SendMessageMulti(pReceiverIDList,ReceiverCount,IsSorted,MsgID,MsgFlag,pData,DataSize);
	FUNCTION_END;
	return FALSE;
}
CDOSMessagePacket * CDistributedObjectOperator::NewMessagePacket(UINT DataSize,UINT ReceiverCount)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::NewMessagePacket(DataSize,ReceiverCount);
	FUNCTION_END;
	return NULL;
}
BOOL CDistributedObjectOperator::ReleaseMessagePacket(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::ReleaseMessagePacket(pPacket);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::SendMessagePacket(CDOSMessagePacket * pPacket)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::SendMessagePacket(pPacket);
	FUNCTION_END;
	return FALSE;
}


BOOL CDistributedObjectOperator::RegisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::RegisterMsgMap(ProxyObjectID,pMsgIDList,CmdCount);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::UnregisterMsgMap(OBJECT_ID ProxyObjectID,MSG_ID_TYPE * pMsgIDList,int CmdCount)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::UnregisterMsgMap(ProxyObjectID,pMsgIDList,CmdCount);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pCmdIDList, int CmdCount)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::RegisterGlobalMsgMap(ProxyRouterID, ProxyType, pCmdIDList, CmdCount);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE * pMsgIDList, int CmdCount)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::UnregisterGlobalMsgMap(ProxyRouterID, ProxyType, pMsgIDList, CmdCount);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::AddConcernedObject(OBJECT_ID ObjectID,bool NeedTest)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::AddConcernedObject(ObjectID,NeedTest);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::DeleteConcernedObject(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::DeleteConcernedObject(ObjectID);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::FindObject(UINT ObjectType)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::FindObject(ObjectType);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::ReportObject(OBJECT_ID TargetID,const CSmartStruct& ObjectInfo)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::ReportObject(TargetID,ObjectInfo);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::CloseProxyObject(OBJECT_ID ProxyObjectID,UINT Delay)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::CloseProxyObject(ProxyObjectID,Delay);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::RequestProxyObjectIP(OBJECT_ID ProxyObjectID)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::RequestProxyObjectIP(ProxyObjectID);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::RegisterObject(DOS_OBJECT_REGISTER_INFO_EX& ObjectRegisterInfo)
{
	FUNCTION_BEGIN;
	return m_pManager->RegisterObject(ObjectRegisterInfo);
	FUNCTION_END;
	return FALSE;
}
void CDistributedObjectOperator::Release()
{
	FUNCTION_BEGIN;
	if(m_pManager)
		m_pManager->UnregisterObject(this);
	FUNCTION_END;
}

BOOL CDistributedObjectOperator::QueryShutDown(OBJECT_ID TargetID,int Level)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::QueryShutDown(TargetID,Level);
	FUNCTION_END;
}

void CDistributedObjectOperator::ShutDown(UINT PluginID)
{
	FUNCTION_BEGIN;
	CMainThread::GetInstance()->QueryFreePlugin(PluginID);
	FUNCTION_END;
}

BOOL CDistributedObjectOperator::RegisterLogger(UINT LogChannel, LPCTSTR FileName)
{
	FUNCTION_BEGIN;
	CEasyString LogFileName;
	CEasyString ModulePath = GetModulePath(NULL);

	CServerLogPrinter * pLog;

	LogFileName.Format("%s/Log/%s", (LPCTSTR)ModulePath, FileName);
	pLog = new CServerLogPrinter(CMainThread::GetInstance(), CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName);
	CLogManager::GetInstance()->AddChannel(LogChannel, pLog);
	SAFE_RELEASE(pLog);
	return TRUE;

	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader)
{
	FUNCTION_BEGIN;
	CEasyString LogFileName;
	CEasyString ModulePath = GetModulePath(NULL);

	CCSVFileLogPrinter * pLog;

	LogFileName.Format("%s/Log/%s", (LPCTSTR)ModulePath, FileName);
	pLog = new CCSVFileLogPrinter();
	pLog->Init(CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSVLogHeader);
	CLogManager::GetInstance()->AddChannel(LogChannel, pLog);
	SAFE_RELEASE(pLog);
	return TRUE;

	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::OnMessage(CDOSMessage * pMessage)
{
	OBJECT_EXCEPTION_CATCH_START;
	if(!m_pDistributedObject->OnPreTranslateMessage(pMessage))
	{
		return m_pDistributedObject->OnMessage(pMessage);
	}
	return TRUE;
	OBJECT_EXCEPTION_CATCH_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::OnSystemMessage(CDOSMessage * pMessage)
{
	OBJECT_EXCEPTION_CATCH_START;
	if(!m_pDistributedObject->OnSystemMessage(pMessage))
	{
		return CDOSBaseObject::OnSystemMessage(pMessage);
	}
	return TRUE;
	OBJECT_EXCEPTION_CATCH_END;
	return FALSE;
}
void CDistributedObjectOperator::OnConcernedObjectLost(OBJECT_ID ObjectID)
{
	OBJECT_EXCEPTION_CATCH_START;
	m_pDistributedObject->OnConcernedObjectLost(ObjectID);
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnFindObject(OBJECT_ID CallerID)
{
	OBJECT_EXCEPTION_CATCH_START;
	if(!m_pDistributedObject->OnFindObject(CallerID))
	{
		CDOSBaseObject::OnFindObject(CallerID);
	}
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnObjectReport(OBJECT_ID ObjectID,const CSmartStruct& ObjectInfo)
{
	OBJECT_EXCEPTION_CATCH_START;
	m_pDistributedObject->OnObjectReport(ObjectID,ObjectInfo);
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnProxyObjectIPReport(OBJECT_ID ProxyObjectID,UINT IP,UINT Port,LPCSTR szIPString)
{
	OBJECT_EXCEPTION_CATCH_START;
	m_pDistributedObject->OnProxyObjectIPReport(ProxyObjectID,IP,Port,szIPString);
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnShutDown(int Level)
{
	OBJECT_EXCEPTION_CATCH_START;
	m_pDistributedObject->OnShutDown(Level);
	OBJECT_EXCEPTION_CATCH_END;
}
int CDistributedObjectOperator::Update(int ProcessPacketLimit)
{
	OBJECT_EXCEPTION_CATCH_START;
	return m_pDistributedObject->Update(ProcessPacketLimit);
	OBJECT_EXCEPTION_CATCH_END;
	return 0;
}
