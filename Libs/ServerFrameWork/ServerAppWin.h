/****************************************************************************/
/*                                                                          */
/*      文件名:    ServerAppWin.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once



class CServerApp :
	public CWinApp,public CNTService
{
protected:
	CBaseServer *		m_pServer;
public:
	CServerApp(void);
	~CServerApp(void);

	virtual BOOL InitInstance();

	virtual int ExitInstance();

	virtual BOOL OnIdle(LONG lCount);

	virtual void Run(DWORD argc, LPTSTR * argv);
	
	virtual void Stop();
public:

	virtual int Run();
	virtual BOOL OnStartUp();
	virtual void OnShutDown();

protected:
	void GetProgamVersion();
	//DECLARE_MESSAGE_MAP()
	
};
