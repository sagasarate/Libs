#pragma once
#include "afxcmn.h"



// CMainControlView 窗体视图

class CMainControlView : public CFormView
{
	DECLARE_DYNCREATE(CMainControlView)

protected:
	CMainControlView();           // 动态创建所使用的受保护的构造函数
	virtual ~CMainControlView();

public:
	enum { IDD = IDD_MAINCONTROLVIEW };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
protected:
	struct CLIENT_EVENT
	{
		int		ItemIndex;
		int		Status;
	};
	CDialogItemSorter						m_ItemSorter;
	CListCtrl								m_lvClients;
	CListCtrl								m_lvInfos;
	CCycleBuffer							m_LogBuffer;
	CThreadSafeIDStorage<CLIENT_EVENT>		m_EventQueue;

	volatile UINT							m_SendCount;
	volatile UINT							m_SendBytes;
	volatile UINT							m_RecvCount;
	volatile UINT							m_RecvBytes;

	
public:
	virtual void OnInitialUpdate();
	afx_msg void OnSize(UINT nType, int cx, int cy);

	void AddClient(CClient * pClient);
	void PushEvent(int ItemIndex,int Status);
	afx_msg void OnNMDblclkListClients(NMHDR *pNMHDR, LRESULT *pResult);

	void PushLog(LPCTSTR szMsg);
	CRichEditCtrl m_reSystemLog;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnEditCut();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();

	void AddSendBytes(UINT Bytes)
	{
		AtomicInc(&m_SendCount);
		AtomicAdd(&m_SendBytes,Bytes);
	}
	void AddRecvBytes(UINT Bytes)
	{
		AtomicInc(&m_RecvCount);
		AtomicAdd(&m_RecvBytes,Bytes);
	}

protected:
	void OnEvent(CLIENT_EVENT& Event);
	void RefreshStat();
};


