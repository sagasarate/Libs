// AndroidForLittleWorld.cpp : 定义应用程序的类行为。
//

#include "stdafx.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMainApp

BEGIN_MESSAGE_MAP(CMainApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, &CMainApp::OnAppAbout)
END_MESSAGE_MAP()


// CMainApp 构造

CMainApp::CMainApp()
{
	// TODO: 在此处添加构造代码，
	// 将所有重要的初始化放置在 InitInstance 中
}


// 唯一的一个 CMainApp 对象

CMainApp theApp;

CMainApp * CMainApp::GetInstance()
{
	return &theApp;
}
// CMainApp 初始化

BOOL CMainApp::InitInstance()
{
	// 如果一个运行在 Windows XP 上的应用程序清单指定要
	// 使用 ComCtl32.dll 版本 6 或更高版本来启用可视化方式，
	//则需要 InitCommonControlsEx()。否则，将无法创建窗口。

	CExceptionParser::GetInstance()->Init(EXCEPTION_SET_DEFAULT_HANDLER|EXCEPTION_USE_API_HOOK);


	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// 将它设置为包括所有要在应用程序中使用的
	// 公共控件类。
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	// 初始化 OLE 库
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();
	AfxInitRichEdit2();
	// 标准初始化
	// 如果未使用这些功能并希望减小
	// 最终可执行文件的大小，则应移除下列
	// 不需要的特定初始化例程
	// 更改用于存储设置的注册表项
	// TODO: 应适当修改该字符串，
	// 例如修改为公司或组织名
	SetRegistryKey(_T("应用程序向导生成的本地应用程序"));
	// 若要创建主窗口，此代码将创建新的框架窗口
	// 对象，然后将其设置为应用程序的主窗口对象
	CMDIFrameWnd* pFrame = new CMainFrame;
	if (!pFrame)
		return FALSE;
	m_pMainWnd = pFrame;
	// 创建主 MDI 框架窗口
	if (!pFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	// 试图加载共享 MDI 菜单和快捷键表
	//TODO: 添加附加成员变量，并加载对
	//	应用程序可能需要的附加菜单类型的调用
	HINSTANCE hInst = AfxGetResourceHandle();
	m_hMDIMenu  = ::LoadMenu(hInst, MAKEINTRESOURCE(IDR_AndroidForLittlTYPE));
	m_hMDIAccel = ::LoadAccelerators(hInst, MAKEINTRESOURCE(IDR_AndroidForLittlTYPE));


	


	// 主窗口已初始化，因此显示它并对其进行更新
	pFrame->ShowWindow(SW_SHOWMAXIMIZED);
	pFrame->UpdateWindow();

	m_pMainControlFrame=(CMainControlFrame *)pFrame->CreateNewChild(
		RUNTIME_CLASS(CMainControlFrame), IDR_AndroidForLittlTYPE, m_hMDIMenu, m_hMDIAccel);

	if(m_pMainControlFrame)
		m_pMainControlFrame->SetWindowText("MainControl");
	else
		return FALSE;

	m_Server.StartUp();

	if(!LoadConfig(CONFIG_FILE_NAME))
		return FALSE;

	return TRUE;
}


// CMainApp 消息处理程序

int CMainApp::ExitInstance() 
{
	//TODO: 处理可能已添加的附加资源
	for(UINT i=0;i<m_GroupList.GetCount();i++)
	{
		m_GroupList[i]->SafeTerminate();
		SAFE_RELEASE(m_GroupList[i]);
	}

	m_GroupList.Clear();

	m_Server.ShutDown();

	CLogManager::ReleaseInstance();

	if (m_hMDIMenu != NULL)
		FreeResource(m_hMDIMenu);
	if (m_hMDIAccel != NULL)
		FreeResource(m_hMDIAccel);

	return CWinApp::ExitInstance();
}




// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()

// 用于运行对话框的应用程序命令
void CMainApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// CMainApp 消息处理程序

bool CMainApp::LoadConfig(LPCTSTR szConfigName)
{
	xml_parser Parser;

	if(Parser.parse_file(szConfigName,pug::parse_trim_attribute))
	{
		xml_node Config=Parser.document();
		if(Config.moveto_child("Main"))
		{
			xml_node Group=Config;
			if(Group.moveto_child("Group"))
			{
				CLIENT_GROUP_CONFIG GroupConfig;
				GroupConfig.ServerAddress.SetIP(Group.attribute("ServerIP").getvalue());
				GroupConfig.ServerAddress.SetPort((WORD)Group.attribute("ServerPort"));
				GroupConfig.AccountName=Group.attribute("AccountName").getvalue();
				GroupConfig.Password=Group.attribute("Password").getvalue();
				GroupConfig.FirstConnectDelay=(UINT)Group.attribute("FirstConnectDelay");
				GroupConfig.ReconnectDelay = (UINT)Group.attribute("ReconnectDelay");
				GroupConfig.AutoReconect=Group.attribute("AutoReconect");
				GroupConfig.ScriptFileName=Group.attribute("ScriptFile").getvalue();
				GroupConfig.ClientCount = (UINT)Group.attribute("ClientCount");
				GroupConfig.StartIndex = (UINT)Group.attribute("StartIndex");

				CClientGroup * pGroup=new CClientGroup();
				pGroup->SetIndex(m_GroupList.GetCount());
				if(pGroup->Init(&m_Server,m_pMainControlFrame->GetView(),GroupConfig))
				{
					m_GroupList.Add(pGroup);
					pGroup->Start();
				}
				else
				{
					SAFE_RELEASE(pGroup);
				}
			}
			return true;
		}
		else
		{
			AfxMessageBoxEx(MB_OK,0,"配置文件[%s]不合法",szConfigName);
			return false;
		}

	}
	else
	{
		AfxMessageBoxEx(MB_OK,0,"无法打开配置文件[%s]",szConfigName);
		return false;
	}
}

UINT CMainApp::AddCommandHistory(LPCTSTR szCommand)
{
	m_CommandHistory.Add(szCommand);
	if(m_CommandHistory.GetCount()>MAX_COMMAND_HISTORY)
		m_CommandHistory.Delete(0);
	return m_CommandHistory.GetCount()+1;
}
LPCTSTR CMainApp::GetCommandHistory(int& Index)
{
	if(Index<0)
		Index=m_CommandHistory.GetCount()-1;
	if(Index>=m_CommandHistory.GetCount())
		Index=0;

	if(Index>=0&&Index<m_CommandHistory.GetCount())
		return m_CommandHistory[Index];

	return NULL;
}

void CMainApp::OnMainControlViewClose()
{
	for(UINT i=0;i<m_GroupList.GetCount();i++)
	{
		m_GroupList[i]->SafeTerminate();
		SAFE_RELEASE(m_GroupList[i]);
	}

	m_GroupList.Clear();
}