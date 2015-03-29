#pragma once


// CMainControlFrame 框架

class CMainControlFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CMainControlFrame)
public:
	CMainControlFrame();           // 动态创建所使用的受保护的构造函数
	virtual ~CMainControlFrame();

protected:
	DECLARE_MESSAGE_MAP()

protected:
	CMainControlView *		m_pView;
public:
	afx_msg void OnClose();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	CMainControlView * GetView()
	{
		return m_pView;
	}
};


