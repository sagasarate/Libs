#include "stdafx.h"

#ifndef WIN32
#include <sys/wait.h>
#endif

#define SERVER_STAT_TIME		(5*60*1000)

IMPLEMENT_CLASS_INFO_STATIC(CDOSMainThread,CDOSServerThread);

CDOSMainThread::CDOSMainThread(void)
{
	FUNCTION_BEGIN;
	m_Status = STATUS_NONE;
	m_hMCSOutRead = NULL;
	m_hMCSOutWrite = NULL;
	m_hMCSInRead = NULL;
	m_hMcsInWrite = NULL;
	m_hMcsErrWrite = NULL;
	m_pMonoMainDomain = NULL;
	FUNCTION_END;
}

CDOSMainThread::~CDOSMainThread(void)
{
}

BOOL CDOSMainThread::OnStart()
{
	FUNCTION_BEGIN;



	CDOSServerThread::SetConfig(CDOSConfig::GetInstance()->GetDOSConfig());

	if (!MonoInit())
		return FALSE;

	if(!CDOSServerThread::OnStart())
		return FALSE;



	if(!m_PluginObjectManager.Init(GetObjectManager(),CDOSConfig::GetInstance()->GetPluginObjectPoolConfig()))
	{
		Log("初始化插件对象管理器失败");
		return FALSE;
	}

	CompileLibs();




	m_ESFactionList.AddCFunction("ShowObjectCount",0,this,&CDOSMainThread::ShowObjectCount);
	m_ESFactionList.AddCFunction("ShowGroupInfo",0,this,&CDOSMainThread::ShowGroupInfo);
	m_ESFactionList.AddCFunction("ListPlugin",0,this,&CDOSMainThread::ListPlugin);
	m_ESFactionList.AddCFunction("ReleasePlugin",1,this,&CDOSMainThread::ReleasePlugin);

	return TRUE;
	FUNCTION_END;
	return FALSE;
}



int CDOSMainThread::Update(int ProcessPacketLimit)
{
	FUNCTION_BEGIN;
	int Process=0;

	Process+=CDOSServerThread::Update(SERVER_UPDATE_LIMIT);

	bool IsCompiling = false;

	switch (m_Status)
	{
	case STATUS_COMPILE_LIBS:
		{
			for (UINT i = 0; i < m_LibList.GetCount(); i++)
			{
				LIB_INFO& LibInfo = m_LibList[i];
				if (LibInfo.Status == LIB_COMPILE_STATE_COMPILING)
				{
					IsCompiling = true;
					if (LibInfo.hMCSProcess != NULL)
					{
#ifdef WIN32
						DWORD ExitCode = 0;
						if (GetExitCodeProcess(LibInfo.hMCSProcess, &ExitCode))
						{
							if (ExitCode == STILL_ACTIVE)
							{
								break;
							}
							else
							{
								LogMono("MCS已退出%d", ExitCode);
								LibInfo.Status = LIB_COMPILE_STATE_COMPILED;
								CloseHandle(LibInfo.hMCSProcess);
								LibInfo.hMCSProcess = NULL;
							}
						}
						else
						{
							LogMono("获取MCS进程退出码失败%d", GetLastError());
							LibInfo.Status = LIB_COMPILE_STATE_ERROR;
							CloseHandle(LibInfo.hMCSProcess);
							LibInfo.hMCSProcess = NULL;
						}
#else
						int ExitCode = 0;
						int Err = waitpid((pid_t)(ptrdiff_t)LibInfo.hMCSProcess, &ExitCode, WNOHANG);
						if (Err == -1)
						{
							LogMono("等待MCS出错%d", errno);
							LibInfo.Status = LIB_COMPILE_STATE_ERROR;
							LibInfo.hMCSProcess = NULL;
						}
						else if (Err != 0)
						{
							ExitCode = WEXITSTATUS(ExitCode);
							LogMono("MCS已退出%d", ExitCode);
							LibInfo.Status = LIB_COMPILE_STATE_COMPILED;
							LibInfo.hMCSProcess = NULL;
						}
#endif
					}
					else
					{
						LogMono("库[%s]编译状态异常", (LPCTSTR)LibInfo.LibName);
						LibInfo.Status = LIB_COMPILE_STATE_ERROR;
					}
				}
				else if (LibInfo.Status == LIB_COMPILE_STATE_NEED_COMPILE)
				{
					CompileCSharpLib(LibInfo);
					IsCompiling = true;
					break;
				}
			}
			if (!IsCompiling)
				m_Status = STATUS_PLUGIN_LOAD;
		}
		break;
	case STATUS_PLUGIN_LOAD:
		{
			LoadProxyPlugins();
			LoadPlugins();
			m_Status = STATUS_WORKING;
		}
		break;
	case STATUS_WORKING:
		{
			UINT PluginID;
			while (m_PluginUnloadQueue.PopFront(PluginID))
			{
				FreePlugin(PluginID);
			}


			UINT RunningPluginCount = 0;

			for (UINT i = 0; i < m_PluginList.GetCount(); i++)
			{
				PLUGIN_INFO& PluginInfo = m_PluginList[i];
				if (PluginInfo.PluginStatus == PLUGIN_STATUS_COMPILEING)
				{
					IsCompiling = true;
					if (PluginInfo.hMCSProcess != NULL)
					{
#ifdef WIN32
						DWORD ExitCode = 0;
						if (GetExitCodeProcess(PluginInfo.hMCSProcess, &ExitCode))
						{
							if (ExitCode != STILL_ACTIVE)
							{
								LogMono("MCS已退出%d", ExitCode);
								if (ExitCode == 0)
								{
									PluginInfo.PluginStatus = PLUGIN_STATUS_COMPILED;
									LoadCSharpPlugin(PluginInfo);
								}
								else
								{
									PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
								}
								CloseHandle(PluginInfo.hMCSProcess);
								PluginInfo.hMCSProcess = NULL;
							}
						}
						else
						{
							LogMono("获取MCS进程退出码失败%d", GetLastError());
							PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
							CloseHandle(PluginInfo.hMCSProcess);
							PluginInfo.hMCSProcess = NULL;
						}
#else
						int ExitCode = 0;
						int Err = waitpid((pid_t)(ptrdiff_t)PluginInfo.hMCSProcess, &ExitCode, WNOHANG);
						if (Err == -1)
						{
							LogMono("等待MCS出错%d", errno);
						}
						else if (Err != 0)
						{
							ExitCode = WEXITSTATUS(ExitCode);
							LogMono("MCS已退出%d", ExitCode);
							if (ExitCode == 0)
							{
								PluginInfo.PluginStatus = PLUGIN_STATUS_COMPILED;
								LoadCSharpPlugin(PluginInfo);
							}
							else
							{
								PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
							}
							PluginInfo.hMCSProcess = NULL;
						}
#endif
					}
					else
					{
						LogMono("插件[%s]编译状态异常", (LPCTSTR)PluginInfo.PluginName);
						PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
					}
				}
				if (PluginInfo.PluginStatus != PLUGIN_STATUS_NONE && PluginInfo.PluginStatus != PLUGIN_STATUS_ERROR)
				{
					RunningPluginCount++;
				}
			}

			if (RunningPluginCount == 0)
			{
				Log("CMainThread::Update:已无活动插件存在，服务器关闭");
				QueryShowDown();
			}
		}
		break;
	}

	if (m_hMCSOutRead&&IsCompiling)
	{

		char Buff[2100];
#ifdef WIN32
		DWORD ReadSize = 0;
		if (PeekNamedPipe(m_hMCSOutRead, NULL, 0, NULL, &ReadSize, NULL))
		{
			if (ReadSize)
			{
				if (ReadFile(m_hMCSOutRead, Buff, 2048, &ReadSize, NULL))
				{
					if (ReadSize)
					{
						Buff[ReadSize] = 0;
						PrintMCSMsg(Buff);
					}
				}
			}
		}
#else
		Buff[0] = 0;
		size_t ReadSize = read((int)(ptrdiff_t)m_hMCSOutRead, Buff, 2048);
		if (ReadSize>0&&ReadSize<2048)
		{
			Buff[ReadSize] = 0;
			PrintMCSMsg(Buff);
		}
#endif
	}


	if (CDOSConfig::GetInstance()->GetMonoConfig().EnableMono)
	{
		if (m_MonoFullGCTimer.IsTimeOut())
		{
			m_MonoFullGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().FullGCInterval);
			mono_gc_collect(mono_gc_max_generation());
		}
		else if (m_MonoAdvanceGCTimer.IsTimeOut())
		{
			m_MonoAdvanceGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().BaseGCInterval*CDOSConfig::GetInstance()->GetMonoConfig().AdvanceGCMul);
			mono_gc_collect(CDOSConfig::GetInstance()->GetMonoConfig().AdvanceGCMul - 1);
		}
		else if (m_MonoBaseGCTimer.IsTimeOut())
		{
			m_MonoBaseGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().BaseGCInterval);
			mono_gc_collect(0);
		}
	}

	return Process;
	FUNCTION_END;
	return 1;
}

void CDOSMainThread::OnBeginTerminate()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
}

BOOL CDOSMainThread::OnTerminating()
{
	FUNCTION_BEGIN;
	FUNCTION_END;
	return false;
}

void CDOSMainThread::OnTerminate()
{
	FUNCTION_BEGIN;

	//GetObjectManager()->SuspendAllGroup();
	//GetObjectManager()->WaitForSuspend(60000);

	CDOSServerThread::OnTerminate();

	FreePlugins();

	m_PluginUnloadQueue.Destory();
	m_PluginObjectManager.Destory();


	if (m_pMonoMainDomain)
	{
		mono_jit_cleanup(m_pMonoMainDomain);
		m_pMonoMainDomain = NULL;
	}
	if (m_hMCSOutRead)
	{
#ifdef WIN32
		CloseHandle(m_hMCSOutRead);
#else
		close((int)(ptrdiff_t)m_hMCSOutRead);
#endif
		m_hMCSOutRead = NULL;
	}
	if (m_hMCSOutWrite)
	{
#ifdef WIN32
		CloseHandle(m_hMCSOutWrite);
#else
		close((int)(ptrdiff_t)m_hMCSOutWrite);
#endif
		m_hMCSOutWrite = NULL;
	}
	if (m_hMCSInRead)
	{
#ifdef WIN32
		CloseHandle(m_hMCSInRead);
#endif
		m_hMCSInRead = NULL;
	}
	if (m_hMcsInWrite)
	{
#ifdef WIN32
		CloseHandle(m_hMcsInWrite);
#endif
		m_hMcsInWrite = NULL;
	}
	if (m_hMcsErrWrite)
	{
#ifdef WIN32
		CloseHandle(m_hMcsErrWrite);
#endif
		m_hMcsErrWrite = NULL;
	}



	FUNCTION_END;
}



int CDOSMainThread::GetClientCount()
{
	FUNCTION_BEGIN;

	FUNCTION_END;
	return 0;
}

LPCTSTR CDOSMainThread::GetConfigFileName()
{
	FUNCTION_BEGIN;
	return CONFIG_FILE_NAME;
	FUNCTION_END;
	return "";
}

bool CDOSMainThread::QueryFreePlugin(UINT PluginID)
{
	return m_PluginUnloadQueue.PushBack(PluginID) != NULL;
}

bool CDOSMainThread::DosGroupInitFn(UINT GroupIndex)
{
	//if (CDOSMainThread::GetInstance()->m_pMonoMainDomain)
	//{
	//	MonoThread * pMonoThread = mono_thread_attach(CDOSMainThread::GetInstance()->m_pMonoMainDomain);
	//	if (pMonoThread)
	//		LogMono("已将对象组%u的线程关联到mono", GroupIndex);
	//	else
	//		LogMono("将对象组%u的线程关联到mono失败", GroupIndex);
	//}
	//else
	//{
	//	LogMono("mono还未初始化");
	//}
	return true;
}

bool CDOSMainThread::DosGroupDestoryFn(UINT GroupIndex)
{

	//MonoThread * pMonoThread = mono_thread_current();
	//if (pMonoThread)
	//{
	//	mono_thread_detach(pMonoThread);
	//	mono_thread_info_detach();
	//	//mono_thread_cleanup();
	//	LogMono("对象组线程%u执行mono_thread_detach", GroupIndex);
	//}
	//else
	//{
	//	LogMono("对象组线程%u没有关联到mono", GroupIndex);
	//}
	return true;
}


MONO_DOMAIN_INFO * CDOSMainThread::GetMonoDomainInfo(UINT PluginID)
{
	for (UINT i = 0; i < m_PluginList.GetCount(); i++)
	{
		if (m_PluginList[i].ID == PluginID&&m_PluginList[i].PluginType == PLUGIN_TYPE_CSHARP)
		{
			return &m_PluginList[i].MonoDomainInfo;
		}
	}
	return NULL;
}

bool CDOSMainThread::IsMonoArrayElementTypeMatch(MonoArray * pArray, MonoClass * pClass)
{
	MonoClass * pArrayClass = mono_object_get_class((MonoObject *)pArray);
	if (pArrayClass)
	{
		MonoClass * pElementClass = mono_class_get_element_class(pArrayClass);
		if (pElementClass)
		{
			return mono_class_get_type_token(pElementClass) == mono_class_get_type_token(pClass);
		}
	}

	return false;
}


MonoArray * CDOSMainThread::MonoCreateByteArray(MONO_DOMAIN_INFO& DomainInfo, const void * pData, size_t DataSize)
{
	if (DomainInfo.pMonoDomain)
	{
		MonoArray * pArray = mono_array_new(DomainInfo.pMonoDomain, mono_get_byte_class(), DataSize);
		if (pArray)
		{
			if (pData && DataSize)
			{
				char * pBuff = mono_array_addr_with_size(pArray, sizeof(BYTE), 0);
				if (pBuff)
				{
					memcpy(pBuff, pData, DataSize);
				}
				else
				{
					Log("CDOSMainThread::MonoCreateByteArray:获得字节数组内存地址失败");
				}
			}
			return pArray;
		}
		else
		{
			Log("CDOSMainThread::MonoCreateByteArray:无法创建字节数组");
		}
	}
	return NULL;
}



BYTE * CDOSMainThread::MonoGetByteArray(MonoArray * pArray, size_t& DataSize)
{
	DataSize = mono_array_length(pArray);
	if (DataSize)
	{
		return (BYTE *)mono_array_addr_with_size(pArray, sizeof(BYTE), 0);
	}
	return NULL;
}

MSG_ID_TYPE * CDOSMainThread::MonoGetMsgIDArray(MonoArray * pArray, size_t& DataSize)
{
	DataSize = mono_array_length(pArray);
	if (DataSize)
	{
		return (MSG_ID_TYPE *)mono_array_addr_with_size(pArray, sizeof(MSG_ID_TYPE), 0);
	}
	return NULL;
}

//MonoObject * CDOSMainThread::MonoCreateDOSObjectID(MONO_DOMAIN_INFO& DomainInfo, OBJECT_ID ObjectID)
//{
//	if (DomainInfo.pMonoDomain && DomainInfo.pMonoClass_ObjectID && DomainInfo.pMonoClassMethod_ObjectID_Ctor)
//	{
// 		MonoObject * pObject = mono_object_new(DomainInfo.pMonoDomain, DomainInfo.pMonoClass_ObjectID);
//		if (pObject)
//		{
//			MonoObject * pException = NULL;
//			void * Params[1];
//			Params[0] = &ObjectID.ID;
//			mono_runtime_invoke(DomainInfo.pMonoClassMethod_ObjectID_Ctor, pObject, Params, &pException);
//			if (pException)
//			{
//				MonoString * pMsg = mono_object_to_string(pException, NULL);
//				if (pMsg)
//				{
//					char * pBuff = mono_string_to_utf8(pMsg);
//					LogMono("%s", pBuff);
//					mono_free(pBuff);
//				}
//			}
//			else
//			{
//				return pObject;
//			}
//		}
//		else
//		{
//			LogMono("CDOSMainThread::MonoCreateDOSObjectID:创建对象失败");
//		}
//	}
//	return NULL;
//}

OBJECT_ID CDOSMainThread::MonoGetObjectID(MONO_DOMAIN_INFO& DomainInfo, MonoObject * pObjectID)
{
	if (DomainInfo.pMonoClass_ObjectID&&DomainInfo.pMonoClassField_ObjectID_ID&&pObjectID)
	{
		MonoClass * pClass = mono_object_get_class(pObjectID);
		if (pClass)
		{
			if (mono_class_get_type_token(DomainInfo.pMonoClass_ObjectID) == mono_class_get_type_token(pClass))
			{
				OBJECT_ID ObjectID;
				mono_field_get_value(pObjectID, DomainInfo.pMonoClassField_ObjectID_ID, &ObjectID.ID);
				return ObjectID;
			}
			else
			{
				LogMono("CDOSMainThread::MonoGetObjectID:对象类型不符合");
			}
		}
		else
		{
			LogMono("CDOSMainThread::MonoGetObjectID:无法获取对象的类");
		}
	}
	return 0;
}

MonoString * CDOSMainThread::MonoCreateString(MONO_DOMAIN_INFO& DomainInfo, LPCTSTR szStr, size_t StrLen)
{
	if (DomainInfo.pMonoDomain)
	{
		if (StrLen <= 0)
			StrLen = _tcslen(szStr);
		return mono_string_new_len(DomainInfo.pMonoDomain, szStr, (UINT)StrLen);
	}
	return NULL;
}

MonoObject * CDOSMainThread::MonoCreateDistributedObjectOperator(MONO_DOMAIN_INFO& DomainInfo, CDistributedObjectOperator * pOperator)
{
	if (DomainInfo.pMonoDomain&&DomainInfo.pMonoClass_DistributedObjectOperator&&DomainInfo.pMonoClassMethod_DistributedObjectOperator_Ctor)
	{
		MonoObject * pObject = mono_object_new(DomainInfo.pMonoDomain, DomainInfo.pMonoClass_DistributedObjectOperator);
		if (pObject)
		{
			MonoObject * pException = NULL;
			void * Params[1];
			Params[0] = &pOperator;
			mono_runtime_invoke(DomainInfo.pMonoClassMethod_DistributedObjectOperator_Ctor, pObject, Params, &pException);
			if (pException)
			{
				MonoString * pMsg = mono_object_to_string(pException, NULL);
				if (pMsg)
				{
					char * pBuff = mono_string_to_utf8(pMsg);
					LogMono("%s", pBuff);
					mono_free(pBuff);
				}
			}
			else
			{
				return pObject;
			}
		}
		else
		{
			LogMono("CDOSMainThread::MonoCreateDistributedObjectOperator:创建对象失败");
		}
	}
	return NULL;
}


bool CDOSMainThread::MonoGetDORI(MONO_DOMAIN_INFO& DomainInfo, MonoObject * pObject, DOS_OBJECT_REGISTER_INFO_FOR_CS& RegisterInfo)
{
	DOS_OBJECT_REGISTER_INFO_FOR_CS Info;
	if (DomainInfo.MonoClassInfo_DORI.IsValid())
	{
		MonoClass * pClass = mono_object_get_class(pObject);
		if (pClass)
		{
			if (mono_class_get_type_token(DomainInfo.MonoClassInfo_DORI.pClass) == mono_class_get_type_token(pClass))
			{
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildObjectID, &RegisterInfo.ObjectID);
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildWeight, &RegisterInfo.Weight);
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildObjectGroupIndex, &RegisterInfo.ObjectGroupIndex);
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildMsgQueueSize, &RegisterInfo.MsgQueueSize);
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildMsgProcessLimit, &RegisterInfo.MsgProcessLimit);
				mono_field_get_value(pObject, DomainInfo.MonoClassInfo_DORI.pFeildObject, &RegisterInfo.pObject);

				return true;
			}
			else
			{
				LogMono("CDOSMainThread::MonoGetDORI:对象类型不符合");
			}
		}
		else
		{
			LogMono("CDOSMainThread::MonoGetDORI:无法获取对象的类");
		}
	}
	return false;
}

bool CDOSMainThread::MonoGetObjectIDList(MONO_DOMAIN_INFO& DomainInfo, MonoArray * pArray, CEasyArray<OBJECT_ID>& ObjectIDList)
{
	if (DomainInfo.pMonoDomain&&DomainInfo.pMonoClass_ObjectID&&DomainInfo.pMonoClassMethod_ObjectID_Ctor)
	{
		size_t Len = mono_array_length(pArray);
		if (Len)
		{
			ObjectIDList.Resize(Len);
			for (size_t i = 0; i < Len; i++)
			{
				MonoObject * pObject = (MonoObject *)mono_array_addr_with_size(pArray, sizeof(MonoObject *), i);
				if (pObject)
				{
					ObjectIDList[i] = MonoGetObjectID(DomainInfo, pObject);
				}
			}
		}
		return true;
	}
	return false;
}

bool CDOSMainThread::LoadPlugins()
{
	FUNCTION_BEGIN;

	m_PluginList = CDOSConfig::GetInstance()->GetPluginList();

	Log("一共有%u个插件", m_PluginList.GetCount());

	for (UINT i = 0; i < m_PluginList.GetCount(); i++)
	{
		PLUGIN_INFO& PluginInfo = m_PluginList[i];
		PluginInfo.ID = i + 1;
		PluginInfo.LogChannel = PLUGIN_LOG_CHANNEL_START + i + 1;
		PluginInfo.PluginStatus = PLUGIN_STATUS_NONE;
		LoadPlugin(PluginInfo);
	}

	m_PluginUnloadQueue.Create((UINT)m_PluginList.GetCount());

	Log("插件装载完毕");

#ifdef WIN32
	CExceptionParser::GetInstance()->OnFinishModuleLoad();
#endif

	return true;
	FUNCTION_END;
	return false;
}

void CDOSMainThread::FreePlugins()
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		FreePlugin(m_PluginList[i]);
	}

	FUNCTION_END;
}

void CDOSMainThread::CompileLibs()
{
	FUNCTION_BEGIN;

	m_Status = STATUS_COMPILE_LIBS;
	m_LibList = CDOSConfig::GetInstance()->GetLibList();
	for (UINT i = 0; i < m_LibList.GetCount(); i++)
	{
		CompileCSharpLib(m_LibList[i]);
		break;
	}

	FUNCTION_END;
}

bool CDOSMainThread::LoadProxyPlugins()
{
	const CEasyArray<CLIENT_PROXY_PLUGIN_INFO>& PluginList = CDOSConfig::GetInstance()->GetProxyPluginList();

	Log("一共有%u个代理", PluginList.GetCount());

	for (UINT i = 0; i < PluginList.GetCount(); i++)
	{
		CLIENT_PROXY_PLUGIN_INFO PluginInfo = PluginList[i];

		if (PluginInfo.ProxyMode == CLIENT_PROXY_MODE_CUSTOM)
		{
			PluginInfo.ID = i + 1;
			PluginInfo.LogChannel = PROXY_PLUGIN_LOG_CHANNEL_START + i + 1;
			PluginInfo.PluginStatus = PLUGIN_STATUS_NONE;
			CDOSObjectProxyServiceCustom * pProxy = new CDOSObjectProxyServiceCustom();
			if (GetProxyManager()->RegisterProxyService(pProxy))
			{
				if (!pProxy->Init(this, PluginInfo))
				{
					GetProxyManager()->UnregisterProxyService(pProxy->GetID());
				}
				SAFE_RELEASE(pProxy);
			}
			else
			{
				SAFE_RELEASE(pProxy);
			}
		}
	}

	Log("代理插件装载完毕");
	return true;
}

bool CDOSMainThread::LoadPlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;

	switch (PluginInfo.PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		return LoadNativePlugin(PluginInfo);
		break;
	case PLUGIN_TYPE_CSHARP:
		return LoadCSharpPlugin(PluginInfo);
		break;
	default:
		Log("CMainThread::LoadPlugin:不支持的插件类型%d,%s", PluginInfo.PluginType, (LPCTSTR)PluginInfo.PluginName);
	}

	FUNCTION_END;
	return false;
}
bool CDOSMainThread::FreePlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;

	switch (PluginInfo.PluginType)
	{
	case PLUGIN_TYPE_NATIVE:
		return FreeNativePlugin(PluginInfo);
		break;
	case PLUGIN_TYPE_CSHARP:
		return FreeCSharpPlugin(PluginInfo);
		break;
	default:
		Log("CMainThread::FreePlugin:不支持的插件类型%d,%s", PluginInfo.PluginType, (LPCTSTR)PluginInfo.PluginName);
	}
	return true;
	FUNCTION_END;
	return false;
}
bool CDOSMainThread::FreePlugin(UINT PluginID)
{
	FUNCTION_BEGIN;

	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		if (m_PluginList[i].ID == PluginID)
		{
			if (FreeNativePlugin(m_PluginList[i]))
			{
				m_PluginList.Delete(i);
				return true;
			}
			else
			{
				m_PluginList.Delete(i);
				return false;
			}
		}
	}

	Log("插件%u未找到",PluginID);
	FUNCTION_END;
	return false;
}


bool CDOSMainThread::LoadNativePlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;

	if (PluginInfo.ModuleFileName.IsEmpty())
#ifdef _DEBUG
		PluginInfo.ModuleFileName = PluginInfo.PluginName + "D";
#else
		PluginInfo.ModuleFileName = PluginInfo.PluginName;
#endif

#ifndef WIN32
	PluginInfo.ModuleFileName.Replace('\\', '/');
#endif
	PluginInfo.ModuleFileName = MakeModuleFullPath(NULL, PluginInfo.ModuleFileName);
	//扩展名补足
	CEasyString FileExt = GetPathFileExtName(PluginInfo.ModuleFileName);
	if (FileExt.GetLength() <= 1)
	{
#ifdef WIN32
		PluginInfo.ModuleFileName = PluginInfo.ModuleFileName + ".dll";
#else
		PluginInfo.ModuleFileName = PluginInfo.ModuleFileName + ".so";
#endif
	}

	Log("开始装载插件%s", (LPCTSTR)PluginInfo.ModuleFileName);
#ifdef WIN32
	PluginInfo.hModule = LoadLibrary(PluginInfo.ModuleFileName);
#else
	PluginInfo.hModule = dlopen(PluginInfo.ModuleFileName, RTLD_NOW | RTLD_LOCAL | RTLD_DEEPBIND);
#endif
	if (PluginInfo.hModule)
	{
#ifdef WIN32
		if (CSystemConfig::GetInstance()->IsPreLoadModuleSym())
		{
			CExceptionParser::GetInstance()->SymLoadFromModule(PluginInfo.ModuleFileName);
		}
		PluginInfo.pInitFN = (PLUGIN_INIT_FN)GetProcAddress(PluginInfo.hModule, PLUGIN_INIT_FN_NAME);
		PluginInfo.pQueryReleaseFN = (PLUGIN_QUERY_RELEASE_FN)GetProcAddress(PluginInfo.hModule, PLUGIN_QUERY_RELEASE_FN_NAME);
		PluginInfo.pReleaseFN = (PLUGIN_RELEASE_FN)GetProcAddress(PluginInfo.hModule, PLUGIN_RELEASE_FN_NAME);
		if (PluginInfo.pInitFN&&PluginInfo.pReleaseFN)
#else
		//dlerror();
		PluginInfo.pInitFN = (PLUGIN_INIT_FN)dlsym(PluginInfo.hModule, PLUGIN_INIT_FN_NAME);
		LPCTSTR InitFNError = dlerror();
		PluginInfo.pQueryReleaseFN = (PLUGIN_QUERY_RELEASE_FN)dlsym(PluginInfo.hModule, PLUGIN_QUERY_RELEASE_FN_NAME);
		LPCTSTR QueryReleaseFNError = dlerror();
		PluginInfo.pReleaseFN = (PLUGIN_RELEASE_FN)dlsym(PluginInfo.hModule, PLUGIN_RELEASE_FN_NAME);
		LPCTSTR ReleaseFNError = dlerror();
		if (InitFNError == NULL&&ReleaseFNError == NULL&&PluginInfo.pInitFN&&PluginInfo.pReleaseFN)
#endif
		{
			CEasyString LogFileName;
			CEasyString ModulePath = GetModulePath(NULL);

			CServerLogPrinter * pLog;

			LogFileName.Format("%s/Log/Plugin.%s", (LPCTSTR)ModulePath, (LPCTSTR)GetPathFileName(PluginInfo.ModuleFileName));
			pLog = new CServerLogPrinter(this, CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
				CSystemConfig::GetInstance()->GetLogLevel(), LogFileName);
			CLogManager::GetInstance()->AddChannel(PluginInfo.LogChannel, pLog);
			SAFE_RELEASE(pLog);

			if ((*(PluginInfo.pInitFN))(&m_PluginObjectManager, PluginInfo.ID, PluginInfo.LogChannel))
			{
				PluginInfo.PluginStatus = PLUGIN_STATUS_RUNNING;
				Log("插件装载成功%s", (LPCTSTR)PluginInfo.ModuleFileName);
				return true;
			}
			else
			{
				Log("插件初始化失败%s", (LPCTSTR)PluginInfo.ModuleFileName);
			}
		}
		else
		{
			Log("不合法的插件%s", (LPCTSTR)PluginInfo.ModuleFileName);
#ifndef WIN32
			Log("InitFN错误信息:%s", InitFNError);
			Log("ReleaseFN错误信息:%s", ReleaseFNError);
#endif
		}
	}
	else
	{
		Log("无法装载插件(%d)%s", GetLastError(), (LPCTSTR)PluginInfo.ModuleFileName);
#ifndef WIN32
		Log("错误信息:%s", dlerror());
#endif
	}

	PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
	FUNCTION_END;
	return false;
}
bool CDOSMainThread::FreeNativePlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;
	if (PluginInfo.pReleaseFN)
		(*(PluginInfo.pReleaseFN))();

	if (PluginInfo.hModule)
	{
#ifdef WIN32
		if (FreeLibrary(PluginInfo.hModule))
#else
		if (dlclose(PluginInfo.hModule) == 0)
#endif
		{
			Log("插件释放成功%s", (LPCTSTR)PluginInfo.ModuleFileName);
		}
		else
		{
			Log("插件释放失败%s", (LPCTSTR)PluginInfo.ModuleFileName);
#ifndef WIN32
			Log("错误信息:%s", dlerror());
#endif
			return false;
		}
	}

	PluginInfo.hModule = NULL;

	return true;
	FUNCTION_END;
	return false;
}

bool CDOSMainThread::LoadCSharpPlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;



	if (m_pMonoMainDomain == NULL)
	{
		LogMono("MonoDomain未初始化");
		return false;
	}

	if (PluginInfo.ModuleFileName.IsEmpty())
#ifdef _DEBUG
		PluginInfo.ModuleFileName = PluginInfo.PluginName + "D";
#else
		PluginInfo.ModuleFileName = PluginInfo.PluginName;
#endif

#ifndef WIN32
	PluginInfo.ModuleFileName.Replace('\\', '/');
#endif

	PluginInfo.ModuleFileName = MakeModuleFullPath(NULL, PluginInfo.PluginName);

	//扩展名补足
	CEasyString FileExt = GetPathFileExtName(PluginInfo.ModuleFileName);
	if (FileExt.GetLength() <= 1)
	{
		PluginInfo.ModuleFileName = PluginInfo.ModuleFileName + ".dll";
	}

	if (PluginInfo.LoadType == PLUGIN_LOAD_TYPE_SOURCE_CODE&&PluginInfo.PluginStatus != PLUGIN_STATUS_COMPILED)
	{
		//需要进行源码编译
		CompileCSharpPlugin(PluginInfo);
		return true;
	}
	else
	{
		PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;
		if (InitPluginDomain(PluginInfo.MonoDomainInfo, PluginInfo.PluginName))
		{
			mono_domain_set(PluginInfo.MonoDomainInfo.pMonoDomain, FALSE);
			PluginInfo.pCSPluginAssembly = mono_domain_assembly_open(PluginInfo.MonoDomainInfo.pMonoDomain, PluginInfo.ModuleFileName);
			if (PluginInfo.pCSPluginAssembly)
			{
				bool Ret = false;
				MonoClass * pClass = mono_class_from_name(mono_assembly_get_image(PluginInfo.pCSPluginAssembly),
					PluginInfo.MainClassNameSpace, PluginInfo.MainClass);
				if (pClass)
				{
					MonoObject * pMainObj = mono_object_new(PluginInfo.MonoDomainInfo.pMonoDomain, pClass);
					if (pMainObj)
					{
						MonoMethod * pCtorMethod = mono_class_get_method_from_name(pClass, ".ctor", 0);
						if (pCtorMethod)
						{
							MonoObject * pException = NULL;
							mono_runtime_invoke(pCtorMethod, pMainObj, NULL, &pException);
							if (pException)
							{
								MonoString * pMsg = mono_object_to_string(pException, NULL);
								if (pMsg)
								{
									char * pBuff = mono_string_to_utf8(pMsg);
									LogMono("%s", pBuff);
									mono_free(pBuff);
								}
							}
							else
							{
								MonoMethod * pInitMethod;
								pInitMethod = mono_class_get_method_from_name(pClass, PLUGIN_INIT_FN_NAME, 2);
								if (pInitMethod)
								{
									CEasyString LogFileName;
									CEasyString ModulePath = GetModulePath(NULL);

									CServerLogPrinter * pLog;

									LogFileName.Format("%s/Log/Plugin.%s", (LPCTSTR)ModulePath, (LPCTSTR)GetPathFileName(PluginInfo.ModuleFileName));
									pLog = new CServerLogPrinter(this, CServerLogPrinter::LOM_CONSOLE | CServerLogPrinter::LOM_FILE,
										CSystemConfig::GetInstance()->GetLogLevel(), LogFileName);
									CLogManager::GetInstance()->AddChannel(PluginInfo.LogChannel, pLog);
									SAFE_RELEASE(pLog);

									pException = NULL;
									LPVOID Params[2];
									Params[0] = &PluginInfo.ID;
									Params[1] = &PluginInfo.LogChannel;
									MonoObject * pReturnValue = mono_runtime_invoke(pInitMethod, pMainObj, Params, &pException);
									if (pException)
									{
										MonoString * pMsg = mono_object_to_string(pException, NULL);
										if (pMsg)
										{
											char * pBuff = mono_string_to_utf8(pMsg);
											LogMono("%s", pBuff);
											mono_free(pBuff);
										}
									}
									else if (pReturnValue)
									{
										void * pValue = mono_object_unbox(pReturnValue);
										if (pValue)
										{
											Ret = *((bool *)pValue);
											if (Ret)
											{
												PluginInfo.hCSMainObj = mono_gchandle_new(pMainObj, false);
												PluginInfo.PluginStatus = PLUGIN_STATUS_RUNNING;
												Log("插件装载成功%s", (LPCTSTR)PluginInfo.ModuleFileName);
												return true;
											}
										}
										else
										{
											Log("插件初始化函数返回值类型不正确");
										}
									}
									else
									{
										LogMono("插件初始化函数没有返回值");
									}
								}
								else
								{
									LogMono("无法找到插件初始化函数");
								}
							}
						}
						else
						{
							LogMono("无法找到构造函数");
						}
					}
					else
					{
						LogMono("无法创建插件对象");
					}
				}
				else
				{
					LogMono("无法找到插件类[%s.%s]", (LPCTSTR)PluginInfo.MainClassNameSpace, (LPCTSTR)PluginInfo.MainClass);
				}
			}
			else
			{
				LogMono("无法加载模块%s", (LPCTSTR)PluginInfo.ModuleFileName);
			}
			ReleasePluginDomain(PluginInfo.MonoDomainInfo);
		}
		else
		{
			LogMono("无法初始化模块domain%s",  (LPCTSTR)PluginInfo.ModuleFileName);
		}
	}



	FUNCTION_END;
	return false;
}
bool CDOSMainThread::CompileCSharpPlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;




	CEasyArray<CEasyString> SourceList;


	for (UINT i = 0; i < PluginInfo.SourceDirs.GetCount(); i++)
	{
		CEasyString RootDir = MakeModuleFullPath(NULL, PluginInfo.SourceDirs[i]);
		SourceList.Add(RootDir);
		FetchDirs(RootDir, SourceList);
	}

	for (UINT i = 0; i < SourceList.GetCount(); i++)
	{
		SourceList[i] = SourceList[i] + DIR_SLASH + "*.cs";
	}

	CEasyArray<CEasyString> LibList;

	FetchFiles(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir), _T(".dll"), LibList);

	CEasyString OutFileName = PluginInfo.ModuleFileName;

	if (CDOSConfig::GetInstance()->GetMonoConfig().CreateProj)
		CreateCSProj(PluginInfo.PluginName, ".", PluginInfo.SourceDirs, OutFileName);

	if (CallMCS(SourceList, LibList, PluginInfo.ModuleFileName, CDOSConfig::GetInstance()->GetMonoConfig().EnableDebug, PluginInfo.hMCSProcess))
	{
		PluginInfo.PluginStatus = PLUGIN_STATUS_COMPILEING;
		return true;
	}

	PluginInfo.PluginStatus = PLUGIN_STATUS_ERROR;

	FUNCTION_END;
	return false;
}
bool CDOSMainThread::FreeCSharpPlugin(PLUGIN_INFO& PluginInfo)
{
	FUNCTION_BEGIN;
	if (PluginInfo.pCSPluginAssembly)
	{
		mono_domain_set(PluginInfo.MonoDomainInfo.pMonoDomain, FALSE);

		MonoClass * pClass = mono_class_from_name(mono_assembly_get_image(PluginInfo.pCSPluginAssembly),
			PluginInfo.MainClassNameSpace, PluginInfo.MainClass);
		if (pClass && PluginInfo.hCSMainObj)
		{
			MonoObject * pMainObj = mono_gchandle_get_target(PluginInfo.hCSMainObj);
			if (pMainObj)
			{
				MonoMethod * pReleaseMethod;
				pReleaseMethod = mono_class_get_method_from_name(pClass, PLUGIN_RELEASE_FN_NAME, 0);
				if (pReleaseMethod)
				{
					MonoObject * pException = NULL;
					mono_runtime_invoke(pReleaseMethod, pMainObj, NULL, &pException);
					if (pException)
					{
						MonoString * pMsg = mono_object_to_string(pException, NULL);
						if (pMsg)
						{
							char * pBuff = mono_string_to_utf8(pMsg);
							LogMono("%s", pBuff);
							mono_free(pBuff);
						}
					}
				}
				else
				{
					LogMono("CDOSMainThread::FreeCSharpPlugin:无法找到插件[%s]释放函数",
						(LPCTSTR)PluginInfo.ModuleFileName);
				}
			}
		}
		else
		{
			LogMono("CDOSMainThread::FreeCSharpPlugin:无法找到插件[%s]类[%s.%s]",
				(LPCTSTR)PluginInfo.ModuleFileName, (LPCTSTR)PluginInfo.MainClassNameSpace, (LPCTSTR)PluginInfo.MainClass);
		}
		if (PluginInfo.hCSMainObj)
		{
			mono_gchandle_free(PluginInfo.hCSMainObj);
			PluginInfo.hCSMainObj = 0;
		}

		//mono_assembly_close(PluginInfo.pCSPluginAssembly);
		//PluginInfo.pCSPluginAssembly = NULL;

		ReleasePluginDomain(PluginInfo.MonoDomainInfo);
		LogMono("CDOSMainThread::FreeCSharpPlugin:插件[%s]已释放", (LPCTSTR)PluginInfo.ModuleFileName);
		return true;
	}
	else
	{
		LogMono("CDOSMainThread::FreeCSharpPlugin:插件[%s]未加载", (LPCTSTR)PluginInfo.ModuleFileName);
		return false;
	}
	FUNCTION_END;
	return false;
}

bool CDOSMainThread::CompileCSharpLib(LIB_INFO& LibInfo)
{
	FUNCTION_BEGIN;

	if (!LibInfo.NeedCompile)
	{
		LibInfo.Status = LIB_COMPILE_STATE_COMPILED;
		return true;
	}

	Log("开始编译库[%s]", (LPCTSTR)LibInfo.LibName);



	CEasyString OutFileName;
	OutFileName.Format("%s/%s.dll", (LPCTSTR)LibInfo.OutDir, (LPCTSTR)LibInfo.LibName);
	OutFileName = MakeModuleFullPath(NULL, OutFileName);



	CEasyArray<CEasyString> SourceList;


	for (UINT i = 0; i < LibInfo.SourceDirs.GetCount(); i++)
	{
		CEasyString RootDir = MakeModuleFullPath(NULL, LibInfo.SourceDirs[i]);
		SourceList.Add(RootDir);
		FetchDirs(RootDir, SourceList);
	}

	for (UINT i = 0; i < SourceList.GetCount(); i++)
	{
		SourceList[i] = SourceList[i] + DIR_SLASH + "*.cs";
	}


	CEasyArray<CEasyString> LibList;

	FetchFiles(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir), _T(".dll"), LibList);

	if (CDOSConfig::GetInstance()->GetMonoConfig().CreateProj)
		CreateCSProj(LibInfo.LibName, LibInfo.PrjDir, LibInfo.SourceDirs, OutFileName);

	if (CallMCS(SourceList, LibList, OutFileName, CDOSConfig::GetInstance()->GetMonoConfig().EnableDebug, LibInfo.hMCSProcess))
	{
		LibInfo.Status = LIB_COMPILE_STATE_COMPILING;
		return true;
	}

	FUNCTION_END;
	return false;
}



int CDOSMainThread::ShowObjectCount(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	GetDistributedObjectManager()->PrintObjectCount();
	FUNCTION_END;
	return 0;
}

int  CDOSMainThread::ShowGroupInfo(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	GetObjectManager()->PrintGroupInfo(SERVER_LOG_CHANNEL);
	FUNCTION_END;
	return 0;
}

int CDOSMainThread::ListPlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	for(size_t i=0;i<m_PluginList.GetCount();i++)
	{
		Log("ID=%u,Name=%s,File=%s", m_PluginList[i].ID, (LPCTSTR)m_PluginList[i].PluginName, (LPCTSTR)m_PluginList[i].ModuleFileName);
	}
	FUNCTION_END;
	return 0;
}
int CDOSMainThread::ReleasePlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount)
{
	FUNCTION_BEGIN;
	FreePlugin(pParams[0]);
	FUNCTION_END;
	return 0;
}

void CDOSMainThread::MonoLog(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *user_data)
{
	PrintImportantLog(0, "[MonoLog][%s]%s", log_level, message);
}

void CDOSMainThread::MonoPrint(const char *string, mono_bool is_stdout)
{
	PrintImportantLog(0, "[MonoPrint]%s", string);
}

void CDOSMainThread::MonoPrintErr(const char *string, mono_bool is_stdout)
{
	PrintImportantLog(0, "[MonoPrintErr]%s", string);
}


bool CDOSMainThread::MonoInit()
{
	FUNCTION_BEGIN;

	if (!CDOSConfig::GetInstance()->GetMonoConfig().EnableMono)
	{
		Log("mono未开启");
		return true;
	}

	m_MonoBaseGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().BaseGCInterval);
	m_MonoAdvanceGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().BaseGCInterval*CDOSConfig::GetInstance()->GetMonoConfig().AdvanceGCMul);
	m_MonoFullGCTimer.SetTimeOut(CDOSConfig::GetInstance()->GetMonoConfig().FullGCInterval);

#ifdef WIN32
	//初始化接管MCS标准输入输出的管道
	SECURITY_ATTRIBUTES Attr;
	Attr.nLength = sizeof(SECURITY_ATTRIBUTES);
	Attr.lpSecurityDescriptor = NULL;
	Attr.bInheritHandle = TRUE;

	if (!CreatePipe(&m_hMCSInRead, &m_hMcsInWrite, &Attr, 0))
	{
		LogMono("创建MCS输入管道失败%d", GetLastError());
	}

	if (!CreatePipe(&m_hMCSOutRead, &m_hMCSOutWrite, &Attr, 0))
	{
		LogMono("创建MCS输出管道失败%d");
	}

	if (!DuplicateHandle(GetCurrentProcess(), m_hMCSOutWrite, GetCurrentProcess(), &m_hMcsErrWrite, 0, TRUE, DUPLICATE_SAME_ACCESS))
	{
		LogMono("复制MCS输出管道失败%d");
	}
#else
	int fds[2];

	if (pipe(fds) == 0)
	{
		m_hMCSOutRead = (HANDLE)fds[0];
		m_hMCSOutWrite = (HANDLE)fds[1];
		int Flag=fcntl((int)(ptrdiff_t)m_hMCSOutRead, F_GETFL, 0);
		Flag=Flag|O_NONBLOCK;
		fcntl((int)(ptrdiff_t)m_hMCSOutRead, F_SETFL, Flag);
	}
	else
	{
		LogMono("创建MCS输出管道失败%d", errno);
	}
#endif
	mono_trace_set_log_handler(MonoLog, this);
	mono_trace_set_print_handler(MonoPrint);
	mono_trace_set_printerr_handler(MonoPrintErr);

	if (!CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir.IsEmpty())
		mono_set_assemblies_path(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir));
	if (!CDOSConfig::GetInstance()->GetMonoConfig().RunDir.IsEmpty())
		mono_set_config_dir(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().RunDir));
	if (!CDOSConfig::GetInstance()->GetMonoConfig().ConfigFilePath.IsEmpty())
		mono_config_parse(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().ConfigFilePath));

	if (CDOSConfig::GetInstance()->GetMonoConfig().EnableDebug)
	{
		CEasyString DebugConfigStr;
		DebugConfigStr.Format("transport=dt_socket,address=%s:%u,server=y,suspend=%s",
			CDOSConfig::GetInstance()->GetMonoConfig().DebugAgentListenAddress.GetIPString(),
			CDOSConfig::GetInstance()->GetMonoConfig().DebugAgentListenAddress.GetPort(),
			CDOSConfig::GetInstance()->GetMonoConfig().IsDebugSuspend ? "y" : "n");

		mono_debugger_agent_parse_options(DebugConfigStr);
		mono_debug_init(MONO_DEBUG_FORMAT_MONO);
	}

	m_pMonoMainDomain = mono_jit_init_version("DistributedObjectServer", "v4.0.30319");
	if (m_pMonoMainDomain)
	{
		RegisterMonoFunctions();

		theApp.ReInitSignals();

		LogMono("mono初始化完毕");
		return true;
	}
	else
	{
		LogMono("MonoDomain创建失败");
	}




	FUNCTION_END;
	return false;
}

void CDOSMainThread::RegisterMonoFunctions()
{
	FUNCTION_BEGIN;
	mono_add_internal_call("DOSSystem.Logger::InternalLog(uint,string)", (void *)CDOSMainThread::MonoInternalCallLog);
	mono_add_internal_call("DOSSystem.Logger::InternalLogDebug(uint,string)", (void *)CDOSMainThread::MonoInternalCallLogDebug);

	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallGetRouterID()",
		(void *)CDistributedObjectOperator::InternalCallGetRouterID);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallGetObjectID(intptr)",
		(void *)CDistributedObjectOperator::InternalCallGetObjectID);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallGetGroupIndex(intptr)",
		(void *)CDistributedObjectOperator::InternalCallGetGroupIndex);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallSendMessage(intptr,DOSSystem.OBJECT_ID,uint,uint16,byte[],int,int)",
		(void *)CDistributedObjectOperator::InternalCallSendMessage);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallSendMessageMulti(intptr,DOSSystem.OBJECT_ID[],bool,uint,uint16,byte[],int,int)",
		(void *)CDistributedObjectOperator::InternalCallSendMessageMulti);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterMsgMap(intptr,DOSSystem.OBJECT_ID,uint[])",
		(void *)CDistributedObjectOperator::InternalCallRegisterMsgMap);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallUnregisterMsgMap(intptr,DOSSystem.OBJECT_ID,uint[])",
		(void *)CDistributedObjectOperator::InternalCallUnregisterMsgMap);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterGlobalMsgMap(intptr,uint16,byte,uint[])",
		(void *)CDistributedObjectOperator::InternalCallRegisterGlobalMsgMap);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallUnregisterGlobalMsgMap(intptr,uint16,byte,uint[])",
		(void *)CDistributedObjectOperator::InternalCallUnregisterGlobalMsgMap);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallSetUnhanleMsgReceiver(intptr,uint16,byte)",
		(void *)CDistributedObjectOperator::InternalCallSetUnhanleMsgReceiver);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallAddConcernedObject(intptr,DOSSystem.OBJECT_ID,bool)",
		(void *)CDistributedObjectOperator::InternalCallAddConcernedObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallDeleteConcernedObject(intptr,DOSSystem.OBJECT_ID,bool)",
		(void *)CDistributedObjectOperator::InternalCallDeleteConcernedObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallFindObject(intptr,uint)",
		(void *)CDistributedObjectOperator::InternalCallFindObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallReportObject(intptr,DOSSystem.OBJECT_ID,byte[],int,int)",
		(void *)CDistributedObjectOperator::InternalCallReportObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallCloseProxyObject(intptr,DOSSystem.OBJECT_ID,uint)",
		(void *)CDistributedObjectOperator::InternalCallCloseProxyObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRequestProxyObjectIP(intptr,DOSSystem.OBJECT_ID)",
		(void *)CDistributedObjectOperator::InternalCallRequestProxyObjectIP);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterObject(uint,DOSSystem.DOS_OBJECT_REGISTER_INFO_EX)",
		(void *)CDistributedObjectOperator::InternalCallRegisterObjectStatic);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterObject(intptr,DOSSystem.DOS_OBJECT_REGISTER_INFO_EX)",
		(void *)CDistributedObjectOperator::InternalCallRegisterObject);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRelease(intptr)",
		(void *)CDistributedObjectOperator::InternalCallRelease);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallQueryShutDown(intptr,DOSSystem.OBJECT_ID,int)",
		(void *)CDistributedObjectOperator::InternalCallQueryShutDown);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallShutDown(intptr,uint)",
		(void *)CDistributedObjectOperator::InternalCallShutDown);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterLogger(uint,string)",
		(void *)CDistributedObjectOperator::InternalCallRegisterLogger);
	mono_add_internal_call("DOSSystem.DistributedObjectOperator::InternalCallRegisterCSVLogger(uint,string,string)",
		(void *)CDistributedObjectOperator::InternalCallRegisterCSVLogger);

	FUNCTION_END;
}

void CDOSMainThread::FetchFiles(LPCTSTR szDir, LPCTSTR szFileExt, CEasyArray<CEasyString>& FileList)
{
	FUNCTION_BEGIN;
	CFileSearcher FileSearcher;

	CEasyString SearchPattern = szDir;

	SearchPattern += "/*.*";

	FileSearcher.FindFirst(SearchPattern);

	while (FileSearcher.FindNext())
	{
		if (FileSearcher.IsDirectory() && (!FileSearcher.IsDots()))
		{
			FetchFiles(FileSearcher.GetFilePath(), szFileExt, FileList);
		}
		else if (FileSearcher.GetFileExt().CompareNoCase(szFileExt) == 0)
		{
			FileList.Add(FileSearcher.GetFilePath());
		}
	}
	FUNCTION_END;
}

void CDOSMainThread::FetchDirs(LPCTSTR szDir, CEasyArray<CEasyString>& DirList)
{
	FUNCTION_BEGIN;
	CFileSearcher FileSearcher;

	CEasyString SearchPattern = szDir;

	SearchPattern += "/*.*";

	FileSearcher.FindFirst(SearchPattern);

	while (FileSearcher.FindNext())
	{
		if (FileSearcher.IsDirectory() && (!FileSearcher.IsDots()))
		{
			DirList.Add(FileSearcher.GetFilePath());
			FetchDirs(FileSearcher.GetFilePath(), DirList);
		}
	}
	FUNCTION_END;
}

MonoClass * CDOSMainThread::MonoGetClass(MonoAssembly * pMonoAssembly, LPCTSTR szNameSpace, LPCTSTR szClassName)
{
	MonoClass * pMonoClass = NULL;
	FUNCTION_BEGIN;
	pMonoClass = mono_class_from_name(mono_assembly_get_image(pMonoAssembly),
		szNameSpace, szClassName);
	if (pMonoClass == NULL)
	{
		LogMono("CDOSMainThread::MonoGetClass:无法找到类[%s.%s]",
			szNameSpace, szClassName);
	}
	FUNCTION_END;
	return pMonoClass;
}

MonoClassField * CDOSMainThread::MonoGetClassField(MonoClass * pMonoClass, LPCTSTR szFieldName)
{
	MonoClassField * pMonoClassField = NULL;
	FUNCTION_BEGIN;
	pMonoClassField = mono_class_get_field_from_name(pMonoClass, szFieldName);
	if (pMonoClassField == NULL)
	{
		LogMono("CDOSMainThread::MonoGetClassField:无法在类[%s]中找到成员[%s]", mono_class_get_name(pMonoClass), szFieldName);
	}
	FUNCTION_END;
	return pMonoClassField;
}

MonoMethod * CDOSMainThread::MonoGetClassMethod(MonoClass * pMonoClass, LPCTSTR szMethodName, int ParamCount)
{
	MonoMethod * pMonoMethod = NULL;
	FUNCTION_BEGIN;
	pMonoMethod = mono_class_get_method_from_name(pMonoClass, szMethodName, ParamCount);
	if (pMonoMethod == NULL)
	{
		LogMono("无法在类[%s]从找到成员函数[%s,%d]", mono_class_get_name(pMonoClass), szMethodName, ParamCount);
	}
	FUNCTION_END;
	return pMonoMethod;
}

void CDOSMainThread::MonoInternalCallLog(UINT LogChannel, MonoString * pMsg)
{
	char * szMsg = mono_string_to_utf8(pMsg);
	if (szMsg)
	{
		ILogPrinter * pLogPrinter = CLogManager::GetInstance()->GetChannel(LogChannel);
		if (pLogPrinter == NULL)
		{
			pLogPrinter = CLogManager::GetInstance()->GetChannel(LOG_MONO_CHANNEL);
		}
		pLogPrinter->PrintLogDirect(ILogPrinter::LOG_LEVEL_NORMAL, 0, szMsg);
		mono_free(szMsg);
	}
}
void CDOSMainThread::MonoInternalCallLogDebug(UINT LogChannel, MonoString * pMsg)
{
	char * szMsg = mono_string_to_utf8(pMsg);
	if (szMsg)
	{
		ILogPrinter * pLogPrinter = CLogManager::GetInstance()->GetChannel(LogChannel);
		if (pLogPrinter == NULL)
		{
			pLogPrinter = CLogManager::GetInstance()->GetChannel(LOG_MONO_CHANNEL);
		}
		pLogPrinter->PrintLogDirect(ILogPrinter::LOG_LEVEL_DEBUG, 0, szMsg);
		mono_free(szMsg);
	}
}

bool CDOSMainThread::InitPluginDomain(MONO_DOMAIN_INFO& MonoDomainInfo, LPCTSTR szName)
{
	if (m_pMonoMainDomain)
	{
		MonoDomainInfo.pMonoDomain = mono_domain_create_appdomain((char *)szName,
			(CDOSConfig::GetInstance()->GetMonoConfig().ConfigFilePath.IsEmpty() ? NULL : (char *)((LPCTSTR)CDOSConfig::GetInstance()->GetMonoConfig().ConfigFilePath)));

		if (MonoDomainInfo.pMonoDomain)
		{
			CEasyString SystemDllFileName = MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir + DIR_SLASH + MONO_ASSEMBLY_DOSSYSTEM);
			MonoDomainInfo.pMonoAssembly_DOSSystem = mono_domain_assembly_open(MonoDomainInfo.pMonoDomain, SystemDllFileName);
			if (MonoDomainInfo.pMonoAssembly_DOSSystem)
			{
				MonoDomainInfo.pMonoClass_ObjectID = MonoGetClass(MonoDomainInfo.pMonoAssembly_DOSSystem, MONO_NAME_SPACE_DOSSYSTEM, MONO_CLASS_NAME_OBJECT_ID);
				if (MonoDomainInfo.pMonoClass_ObjectID)
				{
					MonoDomainInfo.pMonoClassField_ObjectID_ID = MonoGetClassField(MonoDomainInfo.pMonoClass_ObjectID, MONO_CLASS_FIELD_NAME_OBJECT_ID_ID);
					MonoDomainInfo.pMonoClassMethod_ObjectID_Ctor = MonoGetClassMethod(MonoDomainInfo.pMonoClass_ObjectID,
						MONO_CLASS_METHOD_NAME_OBJECT_ID_CTOR, MONO_CLASS_METHOD_PARAM_OBJECT_ID_CTOR);
				}


				MonoDomainInfo.pMonoClass_DistributedObjectOperator = MonoGetClass(MonoDomainInfo.pMonoAssembly_DOSSystem, MONO_NAME_SPACE_DOSSYSTEM, MONO_CLASS_NAME_DISTRIBUTED_OBJECT_OPERATOR);
				if (MonoDomainInfo.pMonoClass_DistributedObjectOperator)
				{
					MonoDomainInfo.pMonoClassMethod_DistributedObjectOperator_Ctor = MonoGetClassMethod(MonoDomainInfo.pMonoClass_DistributedObjectOperator,
						MONO_CLASS_METHOD_NAME_DOO_CTOR, MONO_CLASS_METHOD_PARAM_DOO_CTOR);
				}

				MonoDomainInfo.MonoClassInfo_DORI.pClass = MonoGetClass(MonoDomainInfo.pMonoAssembly_DOSSystem, MONO_NAME_SPACE_DOSSYSTEM, MONO_CLASS_NAME_DOS_OBJECT_REGISTER_INFO_EX);
				if (MonoDomainInfo.MonoClassInfo_DORI.pClass)
				{
					MonoDomainInfo.MonoClassInfo_DORI.pFeildObjectID = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_OBJECT_ID);
					MonoDomainInfo.MonoClassInfo_DORI.pFeildWeight = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_WEIGHT);
					MonoDomainInfo.MonoClassInfo_DORI.pFeildObjectGroupIndex = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_OBJECT_GROUP_INDEX);
					MonoDomainInfo.MonoClassInfo_DORI.pFeildMsgQueueSize = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_MSG_QUEUE_SIZE);
					MonoDomainInfo.MonoClassInfo_DORI.pFeildMsgProcessLimit = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_MSG_PROCESS_LIMIT);
					MonoDomainInfo.MonoClassInfo_DORI.pFeildObject = MonoGetClassField(MonoDomainInfo.MonoClassInfo_DORI.pClass, MONO_CLASS_FIELD_NAME_DORI_OBJECT);
				}

				return true;
			}
			else
			{
				LogMono("无法加载系统库%s", (LPCTSTR)SystemDllFileName);
			}
		}
		else
		{
			LogMono("无法创建Domain");
		}
	}
	else
	{
		LogMono("Mono未初始化");
	}

	return false;
}


bool CDOSMainThread::ReleasePluginDomain(MONO_DOMAIN_INFO& MonoDomainInfo)
{
	//if (MonoDomainInfo.pMonoAssembly_DOSSystem)
	//{
	//	mono_assembly_close(MonoDomainInfo.pMonoAssembly_DOSSystem);
	//	MonoDomainInfo.pMonoAssembly_DOSSystem = NULL;
	//}

	if (MonoDomainInfo.pMonoDomain)
	{
		mono_domain_finalize(MonoDomainInfo.pMonoDomain,MONO_DOMAIN_FINALIZE_TIMEOUT);
		mono_domain_unload(MonoDomainInfo.pMonoDomain);
		//mono_domain_free(MonoDomainInfo.pMonoDomain, true);
		MonoDomainInfo.pMonoDomain = NULL;
	}
	return true;
}

CEasyString CDOSMainThread::GetProjectGUID(LPCTSTR PrjName)
{
	pug::xml_parser Xml;

	Xml.parse_file(PrjName, pug::parse_trim_attribute);
	xml_node Project = Xml.document();
	if (Project.moveto_child(_T("Project")))
	{
		for (UINT i = 0; i < Project.children(); i++)
		{
			xml_node PropertyGroup = Project.child(i);
			if (_tcsncmp(PropertyGroup.name(), _T("PropertyGroup"), 13) == 0)
			{
				xml_node ProjectGuid = PropertyGroup;
				if (ProjectGuid.moveto_child(_T("ProjectGuid")))
				{
					return ProjectGuid.child_value();
				}
			}
		}
	}
	return _T("");
}

bool CDOSMainThread::CreateCSProj(LPCTSTR szPrjName, LPCTSTR szPrjDir, const CEasyArray<CEasyString>& SourceDirs, LPCTSTR szOutFileName)
{
	CEasyString PrjDir = MakeModuleFullPath(NULL, szPrjDir);
	CEasyString PrjFilePath;
	PrjFilePath.Format(_T("%s/%s.csproj"), (LPCTSTR)PrjDir, szPrjName);
	PrjFilePath = MakeFullPath(PrjFilePath);

	CEasyString PrjGUIDStr = GetProjectGUID(PrjFilePath);

	if (PrjGUIDStr.IsEmpty())
	{
#ifdef WIN32
		GUID guid;
		CoCreateGuid(&guid);
		PrjGUIDStr.Format(_T("{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1],
			guid.Data4[2], guid.Data4[3],
			guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);
#else
		uuid_t guid;
		uuid_generate(reinterpret_cast<unsigned char *>(&guid));
		PrjGUIDStr.Format(_T("{%02X%02X%02X%02X-%02X%02X-%02X%02X-%02X%02X-%02X%02X%02X%02X%02X%02X}"),
			guid[3], guid[2], guid[1], guid[0],
			guid[5], guid[4],
			guid[7], guid[6],
			guid[8], guid[9],
			guid[10], guid[11], guid[12], guid[13], guid[14], guid[15]);
#endif


	}


	pug::xml_parser Xml;

	Xml.create();
	xml_node Doc;
	Doc = Xml.document();
	xml_node pi = Doc.append_child(node_pi);
	pi.name(_T("xml"));
	pi.attribute(_T("version")) = _T("1.0");
	pi.attribute(_T("encoding")) = _T("utf-8");

	xml_node Project = Doc.append_child(node_element, _T("Project"));
	Project.append_attribute(_T("ToolsVersion"), _T("4.0"));
	Project.append_attribute(_T("DefaultTargets"), _T("Build"));
	Project.append_attribute(_T("xmlns"), _T("http://schemas.microsoft.com/developer/msbuild/2003"));

	{
		xml_node PropertyGroup = Project.append_child(node_element, _T("PropertyGroup"));

		xml_node Configuration = PropertyGroup.append_child(node_element, _T("Configuration"));
		Configuration.append_attribute(_T("Condition"), _T(" '$(Configuration)' == '' "));
		Configuration.set_child_value(node_pcdata, _T("Debug"));

		xml_node Platform = PropertyGroup.append_child(node_element, _T("Platform"));
		Platform.append_attribute(_T("Condition"), _T(" '$(Platform)' == '' "));
		Platform.set_child_value(node_pcdata, _T("AnyCPU"));



		xml_node ProjectGuid = PropertyGroup.append_child(node_element, _T("ProjectGuid"));
		ProjectGuid.set_child_value(node_pcdata, PrjGUIDStr);


		xml_node OutputType = PropertyGroup.append_child(node_element, _T("OutputType"));
		OutputType.set_child_value(node_pcdata, _T("Library"));

		//xml_node ProjectTypeGuids = PropertyGroup.append_child(node_element, _T("ProjectTypeGuids"));
		//ProjectTypeGuids.set_child_value(node_pcdata, _T("{E097FAD1-6243-4DAD-9C02-E9B9EFC3FFC1};{FAE04EC0-301F-11D3-BF4B-00C04F79EFBC}"));
	}

	{
		xml_node PropertyGroup = Project.append_child(node_element, _T("PropertyGroup"));
		PropertyGroup.append_attribute(_T("Condition"), _T("'$(Configuration)|$(Platform)' == 'Debug|AnyCPU' "));

		xml_node DebugType = PropertyGroup.append_child(node_element, _T("DebugType"));
		DebugType.set_child_value(node_pcdata, _T("pdbonly"));

		xml_node Optimize = PropertyGroup.append_child(node_element, _T("Optimize"));
		Optimize.set_child_value(node_pcdata, _T("false"));

		xml_node OutputPath = PropertyGroup.append_child(node_element, _T("OutputPath"));
		OutputPath.set_child_value(node_pcdata, _T("bin/debug"));

		xml_node IntermediateOutputPath = PropertyGroup.append_child(node_element, _T("IntermediateOutputPath"));
		IntermediateOutputPath.set_child_value(node_pcdata, _T("bin/debug/obj"));

		xml_node WarningLevel = PropertyGroup.append_child(node_element, _T("WarningLevel"));
		WarningLevel.set_child_value(node_pcdata, _T("4"));

		xml_node DefineConstants = PropertyGroup.append_child(node_element, _T("DefineConstants"));
		DefineConstants.set_child_value(node_pcdata, _T("DEBUG;TRACE"));

		xml_node AllowUnsafeBlocks = PropertyGroup.append_child(node_element, _T("AllowUnsafeBlocks"));
		AllowUnsafeBlocks.set_child_value(node_pcdata, _T("true"));
	}

	{
		xml_node PropertyGroup = Project.append_child(node_element, _T("PropertyGroup"));
		PropertyGroup.append_attribute(_T("Condition"), _T("'$(Configuration)|$(Platform)' == 'Release|AnyCPU' "));

		xml_node DebugType = PropertyGroup.append_child(node_element, _T("DebugType"));
		DebugType.set_child_value(node_pcdata, _T("pdbonly"));

		xml_node Optimize = PropertyGroup.append_child(node_element, _T("Optimize"));
		Optimize.set_child_value(node_pcdata, _T("false"));

		xml_node OutputPath = PropertyGroup.append_child(node_element, _T("OutputPath"));
		OutputPath.set_child_value(node_pcdata, _T("bin/release"));

		xml_node IntermediateOutputPath = PropertyGroup.append_child(node_element, _T("IntermediateOutputPath"));
		IntermediateOutputPath.set_child_value(node_pcdata, _T("bin/release/obj"));

		xml_node WarningLevel = PropertyGroup.append_child(node_element, _T("WarningLevel"));
		WarningLevel.set_child_value(node_pcdata, _T("4"));

		xml_node DefineConstants = PropertyGroup.append_child(node_element, _T("DefineConstants"));
		DefineConstants.set_child_value(node_pcdata, _T("TRACE"));

		xml_node AllowUnsafeBlocks = PropertyGroup.append_child(node_element, _T("AllowUnsafeBlocks"));
		AllowUnsafeBlocks.set_child_value(node_pcdata, _T("true"));
	}


	{
		CEasyArray<CEasyString> FileList;

		FetchFiles(MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().LibraryDir), _T(".dll"), FileList);

		xml_node ItemGroup = Project.append_child(node_element, _T("ItemGroup"));

		//{
		//	xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
		//	Reference.append_attribute(_T("Include"), _T("mscorlib"));
		//}

		//{
		//	xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
		//	Reference.append_attribute(_T("Include"), _T("System"));
		//}

		//{
		//	xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
		//	Reference.append_attribute(_T("Include"), _T("System.XML"));
		//}

		//{
		//	xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
		//	Reference.append_attribute(_T("Include"), _T("System.Core"));
		//}

		//{
		//	xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
		//	Reference.append_attribute(_T("Include"), _T("System.Xml.Linq"));
		//}

		for (UINT i = 0; i < FileList.GetCount(); i++)
		{
			if (FileList[i].CompareNoCase(szOutFileName) != 0)
			{
				xml_node Reference = ItemGroup.append_child(node_element, _T("Reference"));
				Reference.append_attribute(_T("Include"), (LPCTSTR)GetPathFileName(FileList[i]));
				xml_node HintPath = Reference.append_child(node_element, _T("HintPath"));
				CEasyString FilePath = GetRelativePath(PrjDir, FileList[i]);
				HintPath.set_child_value(node_pcdata, FilePath);
			}

		}
	}

	{
		CEasyArray<CEasyString> FileList;

		for (UINT i = 0; i < SourceDirs.GetCount(); i++)
		{
			FetchFiles(MakeModuleFullPath(NULL, SourceDirs[i]), _T(".cs"), FileList);
		}

		xml_node ItemGroup = Project.append_child(node_element, _T("ItemGroup"));

		for (UINT i = 0; i < FileList.GetCount(); i++)
		{
			xml_node Reference = ItemGroup.append_child(node_element, _T("Compile"));
			CEasyString FilePath = GetRelativePath(PrjDir, FileList[i]);
			Reference.append_attribute(_T("Include"), (LPCTSTR)FilePath);
		}
	}

	{
		xml_node Import = Project.append_child(node_element, _T("Import"));
		Import.append_attribute(_T("Project"), _T("$(MSBuildToolsPath)\\Microsoft.CSharp.targets"));
	}
	return Doc.SaveToFile(FILE_CHANNEL_NORMAL1, PrjFilePath);
}


void CDOSMainThread::PrintMCSMsg(LPTSTR szMsg)
{
	LPTSTR szMonoMsg = szMsg;
	while (*szMsg)
	{
		if ((*szMsg) == '\r' || *szMsg == '\n')
		{
			if (szMsg - szMonoMsg > 1)
			{
				*szMsg = 0;
				LogMono("MCS:%s", szMonoMsg);
			}
			szMonoMsg = szMsg + 1;
		}
		szMsg++;
	}
	if (szMsg - szMonoMsg > 1)
	{
		LogMono("MCS:%s", szMonoMsg);
	}
}

bool CDOSMainThread::CallMCS(CEasyArray<CEasyString>& SourceList, CEasyArray<CEasyString>& LibList, LPCTSTR szOutFileName, bool IsDebug, HANDLE& hMCSProcess)
{
	FUNCTION_BEGIN;


	CEasyString Sources;
	CEasyString Libs;

	for (UINT i = 0; i < SourceList.GetCount(); i++)
	{
		Sources += SourceList[i];
		Sources += " ";
	}
	Sources.Trim();



	int LibCount = 0;
	for (UINT i = 0; i < LibList.GetCount(); i++)
	{
		if (LibList[i].CompareNoCase(szOutFileName) != 0)
		{
			if (LibCount)
				Libs += "," + LibList[i];
			else
				Libs += LibList[i];
			LibCount++;
		}
	}

	if (!Libs.IsEmpty())
		Libs = "-r:" + Libs;

	LPCTSTR szDebugSwitch = "";

	if (IsDebug)
		szDebugSwitch = "-debug";

	CEasyString OutFileName = "-out:";
	OutFileName += szOutFileName;

#ifdef WIN32

	CEasyString WorkDir = GetModulePath(NULL);

	CEasyString Cmd;

	Cmd.Format("%s %s %s %s -nostdlib -target:library %s",
		(LPCTSTR)MakeModuleFullPath(NULL, CDOSConfig::GetInstance()->GetMonoConfig().CompilerPath),
		(LPCTSTR)Sources, (LPCTSTR)szDebugSwitch, (LPCTSTR)Libs, (LPCTSTR)OutFileName);

	LogMono("开始生成:%s", (LPCTSTR)Cmd);


	STARTUPINFO StartInfo;
	ZeroMemory(&StartInfo, sizeof(STARTUPINFO));
	StartInfo.cb = sizeof(STARTUPINFO);
	StartInfo.dwFlags |= STARTF_USESHOWWINDOW;
	StartInfo.dwFlags |= STARTF_USESTDHANDLES;
	StartInfo.hStdOutput = m_hMCSOutWrite;
	StartInfo.hStdError = m_hMcsErrWrite;
	StartInfo.hStdInput = m_hMCSInRead;
	StartInfo.wShowWindow = SW_HIDE;
	PROCESS_INFORMATION ProcessInfo;


	if (!CreateProcess(NULL,
		Cmd,    // 子进程的命令行
		NULL,                   // process security attributes
		NULL,                   // primary thread security attributes
		TRUE,                   // handles are inherited
		0,                          // creation flags
		NULL,                  // use parent's environment
		WorkDir,                  // use parent's current directory
		&StartInfo,      // STARTUPINFO pointer
		&ProcessInfo)     // receives PROCESS_INFORMATION
		)
	{
		LogMono("运行MCS失败%d", GetLastError());
	}
	else
	{
		hMCSProcess = ProcessInfo.hProcess;
		CloseHandle(ProcessInfo.hThread);
		return true;
	}
#else
	CEasyString CompilerPath = "mcs";



	LogMono("开始生成:%s %s %s %s -nostdlib -target:library %s",
		(LPCTSTR)CompilerPath, (LPCTSTR)Sources, (LPCTSTR)Libs, (LPCTSTR)szDebugSwitch, (LPCTSTR)OutFileName);

	pid_t pid = fork();
	if (pid > 0)
	{
		LogMono("MCS进程pid=%d", pid);
		hMCSProcess = (HANDLE)pid;
		return true;
	}
	else if (pid == 0)
	{
		dup2((int)(ptrdiff_t)m_hMCSOutWrite, STDOUT_FILENO);
		dup2((int)(ptrdiff_t)m_hMCSOutWrite, STDERR_FILENO);
		execlp((LPCTSTR)CompilerPath, (LPCTSTR)CompilerPath, (LPCTSTR)Sources, (LPCTSTR)Libs, (LPCTSTR)szDebugSwitch, "-nostdlib", "-target:library", (LPCTSTR)OutFileName, NULL);
		printf("运行MCS失败%d", errno);
		exit(2);
	}
	else
	{
		LogMono("新建进程失败%d", errno);
	}

#endif

	FUNCTION_END;
	return false;
}
