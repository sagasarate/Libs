#include "stdafx.h"


CDistributedObjectOperator::CDistributedObjectOperator()
{
	FUNCTION_BEGIN;
	m_pManager=NULL;
	m_pDistributedObject=NULL;
	m_PoolID=0;
	m_PluginType = PLUGIN_TYPE_NATIVE;	
	m_hCSOperatorObject = 0;
	m_hCSObject = 0;	
	m_IsCommandReceiver = false;
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
	m_PluginType = PLUGIN_TYPE_NATIVE;
	return TRUE;
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::Init(CDistributedObjectManager * pManager, UINT PoolID, MONO_DOMAIN_INFO& MonoDomainInfo, MonoObject * pDistributedObject)
{
	FUNCTION_BEGIN;
	if (pDistributedObject == NULL)
		return FALSE;
	m_pManager = pManager;
	m_pDistributedObject = NULL;
	m_PoolID = PoolID;
	m_PluginType = PLUGIN_TYPE_CSHARP;	
	m_MonoDomainInfo = MonoDomainInfo;
	m_hCSObject = mono_gchandle_new(pDistributedObject, false);
	MonoObject * pOperatorObject = CDOSMainThread::GetInstance()->MonoCreateDistributedObjectOperator(m_MonoDomainInfo, this);
	if (pOperatorObject)
	{
		m_hCSOperatorObject = mono_gchandle_new(pOperatorObject, false);

		m_MonoClassInfo_DO.pClass = mono_object_get_class(pDistributedObject);
		m_MonoClassInfo_DO.pInitializeMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass, 
			MONO_CLASS_METHOD_NAME_DO_INITIALIZE, MONO_CLASS_METHOD_PARAM_DO_INITIALIZE);
		m_MonoClassInfo_DO.pDestoryMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_DESTORY, MONO_CLASS_METHOD_PARAM_DO_DESTORY);
		m_MonoClassInfo_DO.pOnPreTranslateMessageMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONPRETRANSLATEMESSAGE, MONO_CLASS_METHOD_PARAM_DO_ONPRETRANSLATEMESSAGE);
		m_MonoClassInfo_DO.pOnMessageMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONMESSAGE, MONO_CLASS_METHOD_PARAM_DO_ONMESSAGE);
		m_MonoClassInfo_DO.pOnSystemMessageMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONSYSTEMMESSAGE, MONO_CLASS_METHOD_PARAM_DO_ONSYSTEMMESSAGE);
		m_MonoClassInfo_DO.pOnConcernedObjectLostMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONCONCERNEDOBJECTLOST, MONO_CLASS_METHOD_PARAM_DO_ONCONCERNEDOBJECTLOST);
		m_MonoClassInfo_DO.pOnFindObjectMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONFINDOBJECT, MONO_CLASS_METHOD_PARAM_DO_ONFINDOBJECT);
		m_MonoClassInfo_DO.pOnObjectReportMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONOBJECTREPORT, MONO_CLASS_METHOD_PARAM_DO_ONOBJECTREPORT);
		m_MonoClassInfo_DO.pOnProxyObjectIPReportMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONPROXYOBJECTIPREPORT, MONO_CLASS_METHOD_PARAM_DO_ONPROXYOBJECTIPREPORT);
		m_MonoClassInfo_DO.pOnShutDownMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONSHUTDOWN, MONO_CLASS_METHOD_PARAM_DO_ONSHUTDOWN);
		m_MonoClassInfo_DO.pUpdateMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_UPDATE, MONO_CLASS_METHOD_PARAM_DO_UPDATE);
		m_MonoClassInfo_DO.pOnExceptionMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONEXCEPTION, MONO_CLASS_METHOD_PARAM_DO_ONEXCEPTION);
		m_MonoClassInfo_DO.pOnConsoleCommandMethod = CDOSMainThread::GetInstance()->MonoGetClassMethod(m_MonoClassInfo_DO.pClass,
			MONO_CLASS_METHOD_NAME_DO_ONCONSOLECOMMAND, MONO_CLASS_METHOD_PARAM_DO_ONCONSOLECOMMAND);
		return TRUE;
	}
	
	FUNCTION_END;
	return FALSE;
}

bool CDistributedObjectOperator::Initialize()
{
	FUNCTION_BEGIN;
	if(!CDOSBaseObject::Initialize())
		return false;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:		
		return m_pDistributedObject->Initialize(this) != FALSE;
	case PLUGIN_TYPE_CSHARP:
		return CallCSInitialize();
	}
	
	FUNCTION_END;
	return false;
}

void CDistributedObjectOperator::Destory()
{
	FUNCTION_BEGIN;
	if (m_IsCommandReceiver)
	{
		UnregisterCommandReceiver();
	}
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		m_pDistributedObject->Destory();
		break;
	case PLUGIN_TYPE_CSHARP:
		CallCSDestory();
		break;
	}
	if (m_hCSObject)
	{
		mono_gchandle_free(m_hCSObject);
		m_hCSObject = 0;
	}
	SAFE_RELEASE(m_pDistributedObject);
	if (m_hCSOperatorObject)
	{
		mono_gchandle_free(m_hCSOperatorObject);
		m_hCSOperatorObject = 0;
	}	
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
BOOL CDistributedObjectOperator::RegisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID, int MapType)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::RegisterGlobalMsgMap(ProxyRouterID, ProxyType, MsgID, MapType);
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::UnregisterGlobalMsgMap(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType, MSG_ID_TYPE MsgID)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::UnregisterGlobalMsgMap(ProxyRouterID, ProxyType, MsgID);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::SetUnhanleMsgReceiver(ROUTE_ID_TYPE ProxyRouterID, BYTE ProxyType)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::SetUnhanleMsgReceiver(ProxyRouterID, ProxyType);
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

BOOL CDistributedObjectOperator::ReportObject(OBJECT_ID TargetID, const void * pObjectInfoData, UINT DataSize)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::ReportObject(TargetID, pObjectInfoData, DataSize);
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

BOOL CDistributedObjectOperator::QueryShutDown(OBJECT_ID TargetID, BYTE Level, UINT Param)
{
	FUNCTION_BEGIN;
	return CDOSBaseObject::QueryShutDown(TargetID, Level, Param);
	FUNCTION_END;
}

void CDistributedObjectOperator::ShutDown(UINT PluginID)
{
	FUNCTION_BEGIN;
	CDOSMainThread::GetInstance()->QueryFreePlugin(PluginID);
	FUNCTION_END;
}
bool CDistributedObjectOperator::RegisterCommandReceiver()
{
	if (CDOSMainThread::GetInstance()->AddConsoleCommandReceiver(this))
	{
		m_IsCommandReceiver = true;
		return true;
	}
	return false;
}

bool CDistributedObjectOperator::UnregisterCommandReceiver()
{
	m_IsCommandReceiver = false;
	if (CDOSMainThread::GetInstance()->DeleteConsoleCommandReceiver(this))
	{
		return true;
	}
	return false;
}

bool CDistributedObjectOperator::OnConsoleCommand(LPCTSTR szCommand)
{
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		return m_pDistributedObject->OnConsoleCommand(szCommand);
	case PLUGIN_TYPE_CSHARP:
		return CallOnConsoleCommand(szCommand);
	}
	return false;
}

BOOL CDistributedObjectOperator::RegisterLogger(UINT LogChannel, LPCTSTR FileName)
{
	FUNCTION_BEGIN;
	return DoRegisterLogger(LogChannel, FileName);
	FUNCTION_END;
	return FALSE;
}

BOOL CDistributedObjectOperator::RegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader)
{
	FUNCTION_BEGIN;
	return DoRegisterCSVLogger(LogChannel, FileName, CSVLogHeader);
	FUNCTION_END;
	return FALSE;
}

void CDistributedObjectOperator::SetServerWorkStatus(BYTE WorkStatus)
{
	CDOSMainThread::GetInstance()->SetServerStatus(SC_SST_SS_WORK_STATUS, CSmartValue(WorkStatus));
}

BOOL CDistributedObjectOperator::OnPreTranslateMessage(CDOSMessage * pMessage)
{
	OBJECT_EXCEPTION_CATCH_START;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		return m_pDistributedObject->OnPreTranslateMessage(pMessage);
	case PLUGIN_TYPE_CSHARP:
		return CallCSOnPreTranslateMessage(pMessage->GetMsgID(), pMessage->GetMsgFlag(), pMessage->GetSenderID(), (BYTE *)pMessage->GetDataBuffer(), pMessage->GetDataLength());
	}	
	OBJECT_EXCEPTION_CATCH_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::OnMessage(CDOSMessage * pMessage)
{
	OBJECT_EXCEPTION_CATCH_START;
	if(!OnPreTranslateMessage(pMessage))
	{
		switch (m_PluginType)
		{
		case PLUGIN_TYPE_NATIVE:
			return m_pDistributedObject->OnMessage(pMessage);
		case PLUGIN_TYPE_CSHARP:
			return CallCSOnMessage(pMessage->GetMsgID(), pMessage->GetMsgFlag(), pMessage->GetSenderID(), (BYTE *)pMessage->GetDataBuffer(), pMessage->GetDataLength());
		}
	}
	return TRUE;
	OBJECT_EXCEPTION_CATCH_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::OnSystemMessage(CDOSMessage * pMessage)
{
	OBJECT_EXCEPTION_CATCH_START;
	BOOL Ret = FALSE;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		Ret = m_pDistributedObject->OnSystemMessage(pMessage);
		break;
	case PLUGIN_TYPE_CSHARP:
		Ret = CallCSOnSystemMessage(pMessage->GetMsgID(), pMessage->GetMsgFlag(), pMessage->GetSenderID(), (BYTE *)pMessage->GetDataBuffer(), pMessage->GetDataLength());
		break;
	}
	if (!Ret)
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
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		m_pDistributedObject->OnConcernedObjectLost(ObjectID);
		break;
	case PLUGIN_TYPE_CSHARP:
		CallCSOnConcernedObjectLost(ObjectID);
		break;
	}
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnFindObject(OBJECT_ID CallerID)
{
	OBJECT_EXCEPTION_CATCH_START;
	BOOL Ret = FALSE;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		Ret = m_pDistributedObject->OnFindObject(CallerID);
		break;
	case PLUGIN_TYPE_CSHARP:
		Ret = CallCSOnFindObject(CallerID);
		break;
	}
	if (!Ret)
	{
		CDOSBaseObject::OnFindObject(CallerID);
	}
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize)
{
	OBJECT_EXCEPTION_CATCH_START;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		m_pDistributedObject->OnObjectReport(ObjectID, pObjectInfoData, DataSize);
		break;
	case PLUGIN_TYPE_CSHARP:
		CallCSOnObjectReport(ObjectID, pObjectInfoData, DataSize);
		break;
	}
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString)
{
	OBJECT_EXCEPTION_CATCH_START;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		m_pDistributedObject->OnProxyObjectIPReport(ProxyObjectID, Port, szIPString);
		break;
	case PLUGIN_TYPE_CSHARP:
		CallCSOnProxyObjectIPReport(ProxyObjectID, Port, szIPString);
		break;
	}
	OBJECT_EXCEPTION_CATCH_END;
}
void CDistributedObjectOperator::OnShutDown(BYTE Level, UINT Param)
{
	OBJECT_EXCEPTION_CATCH_START;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		m_pDistributedObject->OnShutDown(Level, Param);
		break;
	case PLUGIN_TYPE_CSHARP:
		CallCSOnShutDown(Level, Param);
		break;
	}
	OBJECT_EXCEPTION_CATCH_END;
}
int CDistributedObjectOperator::Update(int ProcessPacketLimit)
{
	OBJECT_EXCEPTION_CATCH_START;
	switch (m_PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		return m_pDistributedObject->Update(ProcessPacketLimit);
		break;
	case PLUGIN_TYPE_CSHARP:
		return CallCSUpdate(ProcessPacketLimit);
		break;
	}
	OBJECT_EXCEPTION_CATCH_END;
	return 0;
}


bool CDistributedObjectOperator::CallCSInitialize()
{
	FUNCTION_BEGIN;	
	if (m_MonoClassInfo_DO.pInitializeMethod)
	{
		mono_thread_attach(m_MonoDomainInfo.pMonoDomain);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pOperatorObject = mono_gchandle_get_target(m_hCSOperatorObject);
			if (pOperatorObject)
			{
				MonoObject * pException = NULL;
				LPVOID Params[1];
				Params[0] = pOperatorObject;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pInitializeMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
				else if (pReturnValue)
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						return *((bool *)pValue);
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("没有返回值");
				}
			}
			else
			{
				LogMono("无法获得Operator对象");
			}
			
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
	}
	FUNCTION_END;
	return false;
}
void CDistributedObjectOperator::CallCSDestory()
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pDestoryMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{ 
			MonoObject * pException = NULL;			
			mono_runtime_invoke(m_MonoClassInfo_DO.pDestoryMethod, pObject, NULL, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
				CallCSOnException(pException);
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
}
BOOL CDistributedObjectOperator::CallCSOnPreTranslateMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnPreTranslateMessageMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			MonoArray * pByteArray = CDOSMainThread::GetInstance()->MonoCreateByteArray(m_MonoDomainInfo, pData, DataSize);
			if (pByteArray)
			{
				LPVOID Params[4];
				Params[0] = &MsgID;
				Params[1] = &MsgFlag;
				Params[2] = &SenderID;
				Params[3] = pByteArray;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnPreTranslateMessageMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
				else if (pReturnValue)
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						if (*((bool *)pValue))
							return TRUE;
						else
							return FALSE;
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("没有返回值");
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
		
	}
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::CallCSOnMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnMessageMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			MonoArray * pByteArray = CDOSMainThread::GetInstance()->MonoCreateByteArray(m_MonoDomainInfo, pData, DataSize);
			if (pByteArray)
			{
				LPVOID Params[4];
				Params[0] = &MsgID;
				Params[1] = &MsgFlag;
				Params[2] = &SenderID;
				Params[3] = pByteArray;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnMessageMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
				else if (pReturnValue)
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						if (*((bool *)pValue))
							return TRUE;
						else
							return FALSE;
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("没有返回值");
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
		
	}
	FUNCTION_END;
	return FALSE;
}
BOOL CDistributedObjectOperator::CallCSOnSystemMessage(MSG_ID_TYPE MsgID, WORD MsgFlag, OBJECT_ID SenderID, BYTE * pData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnSystemMessageMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			MonoArray * pByteArray = CDOSMainThread::GetInstance()->MonoCreateByteArray(m_MonoDomainInfo, pData, DataSize);
			if (pByteArray)
			{
				LPVOID Params[4];
				Params[0] = &MsgID;
				Params[1] = &MsgFlag;
				Params[2] = &SenderID;
				Params[3] = pByteArray;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnSystemMessageMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
				else if (pReturnValue)
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						if (*((bool *)pValue))
							return TRUE;
						else
							return FALSE;
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("没有返回值");
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
		
	}
	FUNCTION_END;
	return FALSE;
}
void CDistributedObjectOperator::CallCSOnConcernedObjectLost(OBJECT_ID ObjectID)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnConcernedObjectLostMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			LPVOID Params[1];
			Params[0] = &ObjectID;
			MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnConcernedObjectLostMethod, pObject, Params, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
				CallCSOnException(pException);
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
}
BOOL CDistributedObjectOperator::CallCSOnFindObject(OBJECT_ID CallerID)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnFindObjectMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			LPVOID Params[1];
			Params[0] = &CallerID;
			MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnFindObjectMethod, pObject, Params, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
				CallCSOnException(pException);
			}
			else if (pReturnValue)
			{
				void * pValue = mono_object_unbox(pReturnValue);
				if (pValue)
				{
					if (*((bool *)pValue))
						return TRUE;
					else
						return FALSE;
				}
				else
				{
					Log("无法获取返回值");
				}
			}
			else
			{
				Log("没有返回值");
			}
			
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
	return FALSE;
}
void CDistributedObjectOperator::CallCSOnObjectReport(OBJECT_ID ObjectID, const void * pObjectInfoData, UINT DataSize)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnObjectReportMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			MonoArray * pParam1 = CDOSMainThread::GetInstance()->MonoCreateByteArray(m_MonoDomainInfo, pObjectInfoData, DataSize);
			if (pParam1)
			{
				LPVOID Params[2];
				Params[0] = &ObjectID;
				Params[1] = pParam1;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnObjectReportMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
}
void CDistributedObjectOperator::CallCSOnProxyObjectIPReport(OBJECT_ID ProxyObjectID, UINT Port, LPCSTR szIPString)
{
	FUNCTION_BEGIN;	
	if (m_MonoClassInfo_DO.pOnProxyObjectIPReportMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			MonoString * pParam1 = CDOSMainThread::GetInstance()->MonoCreateString(m_MonoDomainInfo, szIPString, 0);
			if (pParam1)
			{
				LPVOID Params[4];
				Params[0] = &ProxyObjectID;
				Params[1] = &Port;
				Params[2] = pParam1;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnProxyObjectIPReportMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
}
void CDistributedObjectOperator::CallCSOnShutDown(BYTE Level, UINT Param)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnShutDownMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			LPVOID Params[2];
			Params[0] = &Level;
			Params[1] = &Param;
			MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnShutDownMethod, pObject, Params, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
				CallCSOnException(pException);
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
	}
	FUNCTION_END;
}
int CDistributedObjectOperator::CallCSUpdate(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pUpdateMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			LPVOID Params[1];
			Params[0] = &ProcessPacketLimit;
			MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pUpdateMethod, pObject, Params, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
				CallCSOnException(pException);
			}
			else if (pReturnValue)
			{
				if (mono_object_get_size(pReturnValue) >= sizeof(int))
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						return *((int *)pValue);
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("返回值类型错误");
					return 1;
				}
			}
			else
			{
				Log("没有返回值");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
		
			
	}
	FUNCTION_END;
	return 0;
}

void CDistributedObjectOperator::CallCSOnException(MonoObject * pPostException)
{
	FUNCTION_BEGIN;
	if (m_MonoClassInfo_DO.pOnExceptionMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoObject * pException = NULL;
			LPVOID Params[1];
			Params[0] = pPostException;
			MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnExceptionMethod, pObject, Params, &pException);
			if (pException)
			{
				CDOSMainThread::GetInstance()->ProcessMonoException(pException);
			}			
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}


	}
	FUNCTION_END;
}

bool CDistributedObjectOperator::CallOnConsoleCommand(LPCTSTR szCommand)
{
	if (m_MonoClassInfo_DO.pOnConsoleCommandMethod)
	{
		mono_domain_set(m_MonoDomainInfo.pMonoDomain, FALSE);
		MonoObject * pObject = mono_gchandle_get_target(m_hCSObject);
		if (pObject)
		{
			MonoString * pParam1 = CDOSMainThread::GetInstance()->MonoCreateString(m_MonoDomainInfo, szCommand, 0);
			if (pParam1)
			{
				MonoObject * pException = NULL;
				LPVOID Params[1];
				Params[0] = pParam1;
				MonoObject * pReturnValue = mono_runtime_invoke(m_MonoClassInfo_DO.pOnConsoleCommandMethod, pObject, Params, &pException);
				if (pException)
				{
					CDOSMainThread::GetInstance()->ProcessMonoException(pException);
					CallCSOnException(pException);
				}
				else if (pReturnValue)
				{
					void * pValue = mono_object_unbox(pReturnValue);
					if (pValue)
					{
						if (*((bool *)pValue))
							return true;
						else
							return false;
					}
					else
					{
						Log("无法获取返回值");
					}
				}
				else
				{
					Log("没有返回值");
				}
			}
			else
			{
				Log("无法创建对象");
			}
		}
		else
		{
			LogMono("无法获得DistributedObject对象");
		}
	}
	return false;
}

bool CDistributedObjectOperator::DoRegisterLogger(UINT LogChannel, LPCTSTR FileName)
{
	CEasyString LogFileName;
	CEasyString ModulePath = CFileTools::GetModulePath(NULL);

	CServerLogPrinter * pLog;

	LogFileName.Format("%s/Log/%s", (LPCTSTR)ModulePath, FileName);
	pLog = new CServerLogPrinter(CDOSMainThread::GetInstance(), CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
		CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSystemConfig::GetInstance()->GetLogCacheSize());
	CLogManager::GetInstance()->AddChannel(LogChannel, pLog);
	SAFE_RELEASE(pLog);
	return true;
}
bool CDistributedObjectOperator::DoRegisterCSVLogger(UINT LogChannel, LPCTSTR FileName, LPCTSTR CSVLogHeader)
{
	CEasyString LogFileName;
	CEasyString ModulePath = CFileTools::GetModulePath(NULL);

	CCSVFileLogPrinter * pLog;

	LogFileName.Format("%s/Log/%s", (LPCTSTR)ModulePath, FileName);
	pLog = new CCSVFileLogPrinter(CSystemConfig::GetInstance()->GetLogLevel(), LogFileName, CSVLogHeader, CSystemConfig::GetInstance()->GetLogCacheSize());
	CLogManager::GetInstance()->AddChannel(LogChannel, pLog);
	SAFE_RELEASE(pLog);
	return true;
}


UINT CDistributedObjectOperator::InternalCallGetRouterID()
{
	return CDOSMainThread::GetInstance()->GetRouter()->GetRouterID();
}
UINT64 CDistributedObjectOperator::InternalCallGetObjectID(CDistributedObjectOperator * pOperator)
{
	if (pOperator)
		return pOperator->GetObjectID().ID;
	return 0;
}
int CDistributedObjectOperator::InternalCallGetGroupIndex(CDistributedObjectOperator * pOperator)
{
	if (pOperator)
		return pOperator->GetGroupIndex();
	return false;
}
bool CDistributedObjectOperator::InternalCallSendMessage(CDistributedObjectOperator * pOperator, OBJECT_ID ReceiverID, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen)
{
	if (pOperator)
	{
		size_t ArrayLen = 0;
		BYTE * pBuff = CDOSMainThread::GetInstance()->MonoGetByteArray(Data, ArrayLen);
		if ((size_t)StartIndex < ArrayLen)
		{
			if ((size_t)StartIndex + (size_t)DataLen>ArrayLen)
				DataLen = (int)(ArrayLen - (size_t)StartIndex);
			return pOperator->SendMessage(ReceiverID, MsgID, MsgFlag, pBuff + StartIndex, DataLen) != FALSE;
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallSendMessageMulti(CDistributedObjectOperator * pOperator, MonoArray * ReceiverIDList, bool IsSorted, UINT MsgID, WORD MsgFlag, MonoArray * Data, int StartIndex, int DataLen)
{
	if (pOperator)
	{
		CEasyArray<OBJECT_ID> ObjectIDList;
		if (CDOSMainThread::GetInstance()->MonoGetObjectIDList(pOperator->m_MonoDomainInfo, ReceiverIDList, ObjectIDList))
		{
			size_t ArrayLen = 0;
			BYTE * pBuff = CDOSMainThread::GetInstance()->MonoGetByteArray(Data, ArrayLen);
			if ((size_t)StartIndex < ArrayLen)
			{
				if ((size_t)StartIndex + (size_t)DataLen>ArrayLen)
					DataLen = (int)(ArrayLen - (size_t)StartIndex);
				return pOperator->SendMessageMulti(ObjectIDList.GetBuffer(), (UINT)ObjectIDList.GetCount(), IsSorted, MsgID, MsgFlag, pBuff + StartIndex, DataLen) != FALSE;
			}
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRegisterMsgMap(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, MonoArray * MsgIDList)
{
	if (pOperator)
	{
		size_t ArrayLen = 0;
		MSG_ID_TYPE * pBuff = CDOSMainThread::GetInstance()->MonoGetMsgIDArray(MsgIDList, ArrayLen);
		if (pBuff)
		{
			return pOperator->RegisterMsgMap(ProxyObjectID, pBuff, (int)ArrayLen) != FALSE;
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallUnregisterMsgMap(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, MonoArray * MsgIDList)
{
	if (pOperator)
	{
		size_t ArrayLen = 0;
		MSG_ID_TYPE * pBuff = CDOSMainThread::GetInstance()->MonoGetMsgIDArray(MsgIDList, ArrayLen);
		if (pBuff)
		{
			return pOperator->UnregisterMsgMap(ProxyObjectID, pBuff, (int)ArrayLen) != FALSE;
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRegisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, UINT MsgID, int MapType)
{
	if (pOperator)
	{
		return pOperator->RegisterGlobalMsgMap(ProxyRouterID, ProxyType, MsgID, MapType) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallUnregisterGlobalMsgMap(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType, UINT MsgID)
{
	if (pOperator)
	{
		return pOperator->UnregisterGlobalMsgMap(ProxyRouterID, ProxyType, MsgID) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallSetUnhanleMsgReceiver(CDistributedObjectOperator * pOperator, WORD ProxyRouterID, BYTE ProxyType)
{
	if (pOperator)
	{
		return pOperator->SetUnhanleMsgReceiver(ProxyRouterID, ProxyType) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallAddConcernedObject(CDistributedObjectOperator * pOperator, OBJECT_ID ObjectID, bool NeedTest)
{
	return pOperator->AddConcernedObject(ObjectID, NeedTest) != FALSE;
}
bool CDistributedObjectOperator::InternalCallDeleteConcernedObject(CDistributedObjectOperator * pOperator, OBJECT_ID ObjectID)
{
	if (pOperator)
	{
		return pOperator->DeleteConcernedObject(ObjectID) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallFindObject(CDistributedObjectOperator * pOperator, UINT ObjectType)
{
	if (pOperator)
		return pOperator->FindObject(ObjectType) != FALSE;
	return false;
}
bool CDistributedObjectOperator::InternalCallReportObject(CDistributedObjectOperator * pOperator, OBJECT_ID TargetID, MonoArray * Data, int StartIndex, int DataLen)
{
	if (pOperator)
	{
		size_t ArrayLen = 0;
		BYTE * pBuff = CDOSMainThread::GetInstance()->MonoGetByteArray(Data, ArrayLen);
		if ((size_t)StartIndex < ArrayLen)
		{
			if ((size_t)StartIndex + (size_t)DataLen>ArrayLen)
				DataLen = (int)(ArrayLen - (size_t)StartIndex);
			return pOperator->ReportObject(TargetID, pBuff, DataLen) != FALSE;
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallCloseProxyObject(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID, UINT Delay)
{
	if (pOperator)
	{
		return pOperator->CloseProxyObject(ProxyObjectID, Delay) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRequestProxyObjectIP(CDistributedObjectOperator * pOperator, OBJECT_ID ProxyObjectID)
{
	if (pOperator)
	{
		return pOperator->RequestProxyObjectIP(ProxyObjectID) != FALSE;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRegisterObjectStatic(UINT PluginID, MonoObject * ObjectRegisterInfo)
{
	MONO_DOMAIN_INFO * pMonoDomainInfo = CDOSMainThread::GetInstance()->GetMonoDomainInfo(PluginID);
	if (pMonoDomainInfo)
	{
		DOS_OBJECT_REGISTER_INFO_FOR_CS RegisterInfo;
		if (CDOSMainThread::GetInstance()->MonoGetDORI(*pMonoDomainInfo, ObjectRegisterInfo, RegisterInfo))
		{
			return CDOSMainThread::GetInstance()->GetDistributedObjectManager()->RegisterObject(RegisterInfo, *pMonoDomainInfo) != FALSE;
		}
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRegisterObject(CDistributedObjectOperator * pOperator, MonoObject * ObjectRegisterInfo)
{
	if (pOperator)
	{
		DOS_OBJECT_REGISTER_INFO_FOR_CS RegisterInfo;
		if (CDOSMainThread::GetInstance()->MonoGetDORI(pOperator->m_MonoDomainInfo, ObjectRegisterInfo, RegisterInfo))
		{
			return CDOSMainThread::GetInstance()->GetDistributedObjectManager()->RegisterObject(RegisterInfo, pOperator->m_MonoDomainInfo) != FALSE;
		}
	}
	return false;
}
void CDistributedObjectOperator::InternalCallRelease(CDistributedObjectOperator * pOperator)
{
	if (pOperator)
		pOperator->Release();
}
bool CDistributedObjectOperator::InternalCallQueryShutDown(CDistributedObjectOperator * pOperator, OBJECT_ID TargetID, BYTE Level, UINT Param)
{
	if (pOperator)
	{
		return pOperator->QueryShutDown(TargetID, Level, Param) != FALSE;
	}
	return false;
}
void CDistributedObjectOperator::InternalCallShutDown(CDistributedObjectOperator * pOperator, UINT PluginID)
{
	if (pOperator)
		pOperator->ShutDown(PluginID);
}
bool CDistributedObjectOperator::InternalCallRegisterLogger(UINT LogChannel, MonoString * FileName)
{
	char * szFileName = mono_string_to_utf8(FileName);
	if (szFileName)
	{
		bool Ret = DoRegisterLogger(LogChannel, szFileName);
		mono_free(szFileName);
		return Ret;
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallRegisterCSVLogger(UINT LogChannel, MonoString * FileName, MonoString * CSVLogHeader)
{
	char * szFileName = mono_string_to_utf8(FileName);
	if (szFileName)
	{
		bool Ret = false;
		char * szCSVLogHeader = mono_string_to_utf8(CSVLogHeader);
		if (szCSVLogHeader)
		{
			Ret = DoRegisterLogger(LogChannel, szFileName);
			mono_free(szCSVLogHeader);			
		}
		mono_free(szFileName);
		return Ret;
	}
	return false;
}

bool CDistributedObjectOperator::InternalCallRegisterCommandReceiver(CDistributedObjectOperator * pOperator)
{
	if (pOperator)
	{
		return pOperator->RegisterCommandReceiver();
	}
	return false;
}
bool CDistributedObjectOperator::InternalCallUnregisterCommandReceiver(CDistributedObjectOperator * pOperator)
{
	if (pOperator)
	{
		return pOperator->UnregisterCommandReceiver();
	}
	return false;
}
void CDistributedObjectOperator::InternalCallSetServerWorkStatus(CDistributedObjectOperator * pOperator, BYTE WorkStatus)
{
	if (pOperator)
	{
		CDOSMainThread::GetInstance()->SetServerStatus(SC_SST_SS_WORK_STATUS, CSmartValue(WorkStatus));
	}
}