#pragma once

#define SERVER_UPDATE_LIMIT			16
#define SERVER_LINK_UPDATE_LIMIT	128
#define CLIENT_UPDATE_LIMIT			128		
#define MS_UDP_UPDATE_LIMIT			16
#define AS_UDP_UPDATE_LIMIT			64
#define CHAT_UDP_UPDATE_LIMIT		16

class CDOSMainThread :
	public CDOSServerThread,public CStaticObject4<CDOSMainThread>
{
protected:		
	enum STATUS
	{
		STATUS_NONE,
		STATUS_COMPILE_LIBS,
		STATUS_PROXY_PLUGIN_LOAD,
		STATUS_PLUGIN_LOAD,
		STATUS_WORKING,
		STATUS_SHUTDOWN,
	};
	
	STATUS										m_Status;

	CDistributedObjectManager					m_PluginObjectManager;
	CEasyArray<PLUGIN_INFO>						m_PluginList;
	CEasyArray<LIB_INFO>						m_LibList;
	CEasyArray<UINT>							m_PluginReleaseList;
	CEasyCriticalSection						m_PluginReleaseLock;

	HANDLE										m_hMCSOutRead;
	HANDLE										m_hMCSOutWrite;
	HANDLE										m_hMCSInRead;
	HANDLE										m_hMcsInWrite;
	HANDLE										m_hMcsErrWrite;

	MonoDomain *								m_pMonoMainDomain;

	CEasyTimer									m_MonoBaseGCTimer;
	CEasyTimer									m_MonoAdvanceGCTimer;
	CEasyTimer									m_MonoFullGCTimer;

	CEasyCriticalSection						m_MonoExcpetionLock;

	CEasyTimer									m_PluginReleaseCheckTimer;
	

	DECLARE_CLASS_INFO_STATIC(CDOSMainThread)

public:
	CDOSMainThread(void);
	virtual ~CDOSMainThread(void);

	virtual BOOL OnStart();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	
	virtual void OnBeginTerminate();
	virtual BOOL OnTerminating();	

	virtual void OnTerminate();

	

	virtual int GetClientCount();
	virtual LPCTSTR GetConfigFileName();

	CDistributedObjectManager * GetDistributedObjectManager();

	bool QueryFreePlugin(UINT PluginID);
	static bool DosGroupInitFn(UINT GroupIndex);
	static bool DosGroupDestoryFn(UINT GroupIndex);

	MONO_DOMAIN_INFO * GetMonoDomainInfo(UINT PluginID);

	bool IsMonoArrayElementTypeMatch(MonoArray * pArray, MonoClass * pClass);
	MonoArray * MonoCreateByteArray(MONO_DOMAIN_INFO& DomainInfo, const void * pData, size_t DataSize);
	BYTE * MonoGetByteArray(MonoArray * pArray, size_t& DataSize);
	MSG_ID_TYPE * MonoGetMsgIDArray(MonoArray * pArray, size_t& DataSize);
	//MonoObject * MonoCreateDOSObjectID(MONO_DOMAIN_INFO& DomainInfo, OBJECT_ID ObjectID);
	OBJECT_ID MonoGetObjectID(MONO_DOMAIN_INFO& DomainInfo, MonoObject * pObjectID);
	MonoString * MonoCreateString(MONO_DOMAIN_INFO& DomainInfo, LPCTSTR szStr, size_t StrLen);
	MonoObject * MonoCreateDistributedObjectOperator(MONO_DOMAIN_INFO& DomainInfo, CDistributedObjectOperator * pOperator);
	bool MonoGetDORI(MONO_DOMAIN_INFO& DomainInfo, MonoObject * pObject, DOS_OBJECT_REGISTER_INFO_FOR_CS& RegisterInfo);
	bool MonoGetObjectIDList(MONO_DOMAIN_INFO& DomainInfo, MonoArray * pArray, CEasyArray<OBJECT_ID>& ObjectIDList);

	MonoClass * MonoGetClass(MonoAssembly * pMonoAssembly, LPCTSTR szNameSpace, LPCTSTR szClassName);
	MonoClassField * MonoGetClassField(MonoClass * pMonoClass, LPCTSTR szFieldName);
	MonoMethod * MonoGetClassMethod(MonoClass * pMonoClass, LPCTSTR szMethodName, int ParamCount);

	void ProcessMonoException(MonoObject * pException);
	bool AddConsoleCommandReceiver(CDistributedObjectOperator * pOperator);
	bool DeleteConsoleCommandReceiver(CDistributedObjectOperator * pOperator);
protected:
	void StartPluginsLoad();
	bool DoPluginsLoad();
	void FreePlugins();
	void CompileLibs();
	void StartProxyPluginsLoad();
	bool DoProxyPluginsLoad();

	bool LoadPlugin(PLUGIN_INFO& PluginInfo);
	bool FreePlugin(PLUGIN_INFO& PluginInfo);
	bool FreePlugin(UINT PluginID);

	bool LoadNativePlugin(PLUGIN_INFO& PluginInfo);
	bool FreeNativePlugin(PLUGIN_INFO& PluginInfo);

	bool LoadCSharpPlugin(PLUGIN_INFO& PluginInfo);
	bool CompileCSharpPlugin(PLUGIN_INFO& PluginInfo);
	bool FreeCSharpPlugin(PLUGIN_INFO& PluginInfo);
	bool CompileCSharpLib(LIB_INFO& LibInfo);


	int ShowObjectCount(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ShowGroupInfo(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ListPlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ReleasePlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);

	static void MonoLog(const char *log_domain, const char *log_level, const char *message, mono_bool fatal, void *user_data);
	static void MonoPrint(const char *string, mono_bool is_stdout);
	static void MonoPrintErr(const char *string, mono_bool is_stdout);
	bool MonoInit();
	void RegisterMonoFunctions();
	void FetchFiles(LPCTSTR szDir, LPCTSTR szFileExt, CEasyArray<CEasyString>& FileList);
	void FetchDirs(LPCTSTR szDir, CEasyArray<CEasyString>& DirList);


	static void MonoInternalCallLog(UINT LogChannel, MonoString * pMsg);
	static void MonoInternalCallLogDebug(UINT LogChannel, MonoString * pMsg);

	bool InitPluginDomain(MONO_DOMAIN_INFO& MonoDomainInfo, LPCTSTR szName);
	bool ReleasePluginDomain(PLUGIN_INFO& PluginInfo);

	CEasyString GetProjectGUID(LPCTSTR PrjName);
	bool CreateCSProj(LPCTSTR szPrjName, LPCTSTR szPrjDir, const CEasyArray<CEasyString>& SourceDirs, LPCTSTR szOutFileName);
	
	void PrintMCSMsg(LPTSTR szMsg);

	bool CallMCS(CEasyArray<CEasyString>& SourceList, CEasyArray<CEasyString>& LibList, LPCTSTR szOutFileName, bool IsDebug, HANDLE& hMCSProcess);
};


inline CDistributedObjectManager * CDOSMainThread::GetDistributedObjectManager()
{
	return &m_PluginObjectManager;
}

