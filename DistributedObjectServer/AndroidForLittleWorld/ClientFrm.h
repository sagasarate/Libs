// ChildFrm.h : CChildFrame 类的接口
//


#pragma once



class CClientFrame : public CMDIChildWnd
{
	DECLARE_DYNCREATE(CClientFrame)
public:
	CClientFrame();

// 属性
public:

// 操作
public:

// 重写
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	
	CClientInfoView * GetView()
	{
		return m_pView;
	}

	

// 实现
public:
	// 框架工作区的视图。
	CClientInfoView *		m_pView;
	
	virtual ~CClientFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// 生成的消息映射函数
protected:
	afx_msg void OnFileClose();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
};
