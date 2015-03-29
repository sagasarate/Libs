#pragma once

#define SERVER_UPDATE_LIMIT			16
#define SERVER_LINK_UPDATE_LIMIT	128
#define CLIENT_UPDATE_LIMIT			128		
#define MS_UDP_UPDATE_LIMIT			16
#define AS_UDP_UPDATE_LIMIT			64
#define CHAT_UDP_UPDATE_LIMIT		16

class CMainThread :
	public CDOSServerThread,public CStaticObject<CMainThread>
{
protected:		
	struct PLUGIN_INFO
	{
		UINT					ID;
		CEasyString				PluginFileName;
		HMODULE					hModule;
		PLUGIN_INIT_FN			pInitFN;
		PLUGIN_QUERY_RELEASE_FN	pQueryReleaseFN;
		PLUGIN_RELEASE_FN		pReleaseFN;
	};
	//CLoginPlane		m_LoginPlane;
	//CMapArea		m_MapArea;
	CDistributedObjectManager			m_PluginObjectManager;
	CEasyArray<PLUGIN_INFO>				m_PluginList;
	CThreadSafeIDStorage<UINT>			m_PluginUnloadQueue;

	DECLARE_CLASS_INFO_STATIC(CMainThread)

public:
	CMainThread(void);
	virtual ~CMainThread(void);

	virtual BOOL OnStart();

	virtual int Update(int ProcessPacketLimit=DEFAULT_SERVER_PROCESS_PACKET_LIMIT);
	
	virtual void OnBeginTerminate();
	virtual BOOL OnTerminating();	

	virtual void OnTerminate();

	

	virtual int GetClientCount();
	virtual LPCTSTR GetConfigFileName();

	CDistributedObjectManager * GetDistributedObjectManager();

	bool QueryFreePlugin(UINT PluginID);
	
protected:
	BOOL LoadPlugins();
	void FreePlugins();
	bool FreePlugin(UINT PluginID);

	int ShowObjectCount(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ShowGroupInfo(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ListPlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
	int ReleasePlugin(CESThread * pESThread,ES_BOLAN* pResult,ES_BOLAN* pParams,int ParamCount);
};


inline CDistributedObjectManager * CMainThread::GetDistributedObjectManager()
{
	return &m_PluginObjectManager;
}