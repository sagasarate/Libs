#pragma once
#include "afxcmn.h"
#include "mapbox.h"


// CDlgMapView 对话框

class CDlgMapView : public CDialog
{
	DECLARE_DYNAMIC(CDlgMapView)
protected:
	CDialogItemSorter	m_ItemSorter;
	CClient *			m_pClient;	
	CListCtrl			m_lvCharList;
	CMapBox				m_stMapBox;
public:
	CDlgMapView(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CDlgMapView();

// 对话框数据
	enum { IDD = IDD_DIALOG_MAP_VIEW };

	void SetClient(CClient * pClient)
	{
		m_pClient=pClient;
	}

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	void DrawMap(CPaintDC * pDC);

	void FillCharList();
	void OnClickMap(CPoint Point);
};
