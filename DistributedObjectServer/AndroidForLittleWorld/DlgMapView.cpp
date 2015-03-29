// DlgMapView.cpp : 实现文件
//

#include "stdafx.h"
#include "DlgMapView.h"

#define UPDATE_MAP_TIMER		200
#define UPDATE_CHAR_LIST_TIMER	1000
#define MAP_DRAW_RATE			4

// CDlgMapView 对话框

IMPLEMENT_DYNAMIC(CDlgMapView, CDialog)

CDlgMapView::CDlgMapView(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgMapView::IDD, pParent)
{
	m_pClient=NULL;
}

CDlgMapView::~CDlgMapView()
{
}

void CDlgMapView::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_CHAR, m_lvCharList);
	DDX_Control(pDX, IDC_STATIC_MAP, m_stMapBox);
}


BEGIN_MESSAGE_MAP(CDlgMapView, CDialog)
	ON_WM_SIZE()
	ON_WM_TIMER()
END_MESSAGE_MAP()


// CDlgMapView 消息处理程序

BOOL CDlgMapView::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_ItemSorter.SetParent(this);

	m_ItemSorter.SetSortType(0,CDialogItemSorter::SORT_HORIZONTAL|
		CDialogItemSorter::PARAM_ALIGN_SPACE|CDialogItemSorter::ENABLE_RESIZE_BAR,3,0);

	m_ItemSorter.AddDialogItem(0,1,
		CDialogItemSorter::RESIZE_FIT_PARENT_WIDTH|
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT,
		IDC_STATIC_MAP,CRect(0,0,0,0),0,0);
	m_ItemSorter.AddDialogItem(0,2,
		CDialogItemSorter::RESIZE_FIT_PARENT_HEIGHT,
		IDC_LIST_CHAR,CRect(0,0,0,0),0,0);


	m_lvCharList.SetExtendedStyle(LVS_EX_GRIDLINES|LVS_EX_FULLROWSELECT);

	m_lvCharList.InsertColumn(0,"ID",LVCFMT_LEFT,60);
	m_lvCharList.InsertColumn(1,"坐标",LVCFMT_CENTER,100);
	m_lvCharList.InsertColumn(2,"角色名",LVCFMT_CENTER,100);

	m_ItemSorter.DoSort();

	m_stMapBox.SetParent(this);

	SetTimer(UPDATE_MAP_TIMER,UPDATE_MAP_TIMER,NULL);
	SetTimer(UPDATE_CHAR_LIST_TIMER,UPDATE_CHAR_LIST_TIMER,NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgMapView::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	m_ItemSorter.DoSort();
}

void CDlgMapView::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	if(nIDEvent==UPDATE_MAP_TIMER)
	{
		m_stMapBox.Invalidate();
	}
	if(nIDEvent==UPDATE_CHAR_LIST_TIMER)
	{
		FillCharList();
	}

	CDialog::OnTimer(nIDEvent);
}


void CDlgMapView::DrawMap(CPaintDC * pDC)
{
	if(m_pClient)
	{
		
		UINT64 SelfCharID=m_pClient->GetSelfCharID();
		MAP_CHAR_INFO * pSelfCharInfo=m_pClient->GetChar(SelfCharID);
		if(pSelfCharInfo)
		{
			CRect MapViewRect;
			m_stMapBox.GetClientRect(&MapViewRect);

			float CenterX=pSelfCharInfo->MoveStatus.GetPosX();
			float CenterY=pSelfCharInfo->MoveStatus.GetPosY();
			void * Pos=m_pClient->GetFirstCharPos();
			while(Pos)
			{
				MAP_CHAR_INFO * pCharInfo=m_pClient->GetNextChar(Pos);
				if(pCharInfo)
				{
					float PosX=pCharInfo->MoveStatus.GetPosX();
					float PosY=pCharInfo->MoveStatus.GetPosY();

					int DrawX=(PosX-CenterX)*MAP_DRAW_RATE+MapViewRect.Width()/2-MAP_DRAW_RATE/2;
					int DrawY=(PosY-CenterY)*MAP_DRAW_RATE+MapViewRect.Height()/2-MAP_DRAW_RATE/2;

					CRect DrawBox=MapViewRect;
					DrawBox.right-=MAP_DRAW_RATE;
					DrawBox.bottom-=MAP_DRAW_RATE;
					if(DrawBox.PtInRect(CPoint(DrawX,DrawY)))
					{

						COLORREF CharColor=RGB(255,0,0);
						if(pCharInfo->CharID<NPC_ID_START)
							CharColor=RGB(0,255,255);
						if(pCharInfo->CharID==SelfCharID)
							CharColor=RGB(255,255,255);

						pDC->FillSolidRect(DrawX,DrawY,MAP_DRAW_RATE,MAP_DRAW_RATE,CharColor);
					}
				}
			}
		}
	}
}


void CDlgMapView::FillCharList()
{
	m_lvCharList.SetRedraw(false);
	m_lvCharList.DeleteAllItems();
	if(m_pClient)
	{
		void * Pos=m_pClient->GetFirstCharPos();
		while(Pos)
		{
			MAP_CHAR_INFO * pCharInfo=m_pClient->GetNextChar(Pos);
			if(pCharInfo)
			{
				CEasyString Temp;
				Temp.Format("%llu",pCharInfo->CharID);
				int Item=m_lvCharList.InsertItem(m_lvCharList.GetItemCount(),Temp);
				Temp.Format("%g,%g",
					pCharInfo->MoveStatus.GetPosX(),
					pCharInfo->MoveStatus.GetPosY());
				m_lvCharList.SetItemText(Item,1,Temp);
				m_lvCharList.SetItemText(Item,2,pCharInfo->BaseCharInfo.GetCharName());
			}
		}
	}
	m_lvCharList.SetRedraw(true);
}	
void CDlgMapView::OnClickMap(CPoint Point)
{
	// TODO: 在此添加控件通知处理程序代码
	if(m_pClient)
	{
		UINT64 SelfCharID=m_pClient->GetSelfCharID();
		MAP_CHAR_INFO * pSelfCharInfo=m_pClient->GetChar(SelfCharID);
		if(pSelfCharInfo)
		{
			CRect MapViewRect;
			m_stMapBox.GetClientRect(&MapViewRect);

			float PosX=(float)(Point.x-MapViewRect.Width()/2)/MAP_DRAW_RATE;
			float PosY=(float)(Point.y-MapViewRect.Height()/2)/MAP_DRAW_RATE;

			PosX+=pSelfCharInfo->MoveStatus.GetPosX();
			PosY+=pSelfCharInfo->MoveStatus.GetPosY();

			CEasyString Command;
			Command.Format("Move(%g,%g)",PosX,PosY);

			m_pClient->PushCommand(Command);
		}
	}
}
