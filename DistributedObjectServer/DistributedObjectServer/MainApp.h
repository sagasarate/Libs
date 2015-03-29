// DBProxy.h : PROJECT_NAME 应用程序的主头文件
//

#pragma once



// CMainApp:
// 有关此类的实现，请参阅 DBProxy.cpp
//

class CMainApp : public CServerApp
{
protected:
	bool		m_IsConfigLoaded;
public:
	CMainApp();

// 重写
	public:
	//virtual BOOL InitInstance();
	//virtual int ExitInstance();

	virtual BOOL OnStartUp();
	virtual void OnShutDown();

// 实现
	virtual void OnSetServiceName();
	
};

extern CMainApp theApp;
