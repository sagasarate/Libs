#pragma once
#include "afxcmn.h"


// CDlgServerStatus 对话框

class CDlgServerStatus : public CDialog
{
	DECLARE_DYNAMIC(CDlgServerStatus)

public:
	CDlgServerStatus(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgServerStatus();

// 对话框数据
	enum { IDD = IDD_SERVER_STATUS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	CListCtrl m_lvServerStatus;
	virtual BOOL OnInitDialog();

	void FlushStatus(CSmartStruct& ServerStatus);
	afx_msg void OnSize(UINT nType, int cx, int cy);
};
