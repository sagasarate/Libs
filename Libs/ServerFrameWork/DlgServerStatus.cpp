// DlgServerStatus.cpp : 实现文件
//

#include "stdafx.h"




// CDlgServerStatus 对话框

IMPLEMENT_DYNAMIC(CDlgServerStatus, CDialog)

CDlgServerStatus::CDlgServerStatus(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgServerStatus::IDD, pParent)
{

}

CDlgServerStatus::~CDlgServerStatus()
{
}

void CDlgServerStatus::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_STATUS, m_lvServerStatus);
}


BEGIN_MESSAGE_MAP(CDlgServerStatus, CDialog)
	ON_WM_SIZE()
END_MESSAGE_MAP()


// CDlgServerStatus 消息处理程序

BOOL CDlgServerStatus::OnInitDialog()
{
	CDialog::OnInitDialog();

	// TODO:  在此添加额外的初始化

	m_lvServerStatus.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_GRIDLINES);

	m_lvServerStatus.InsertColumn(0,"项",LVCFMT_LEFT,200);
	m_lvServerStatus.InsertColumn(1,"值",LVCFMT_LEFT,150);

	CRect ClientRect;
	GetClientRect(&ClientRect);

	m_lvServerStatus.MoveWindow(&ClientRect);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常: OCX 属性页应返回 FALSE
}

void CDlgServerStatus::FlushStatus(CSmartStruct& ServerStatus, CEasyArray<SERVER_STATUS_FORMAT_INFO>& FormatInfoList)
{
	m_lvServerStatus.DeleteAllItems();
	LPVOID Pos=ServerStatus.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=ServerStatus.GetNextMember(Pos,MemberID);
		CEasyString MemberIDStr("未知属性");
		int FormatType = SSFT_DEFAULT;
		for (UINT i = 0; i < FormatInfoList.GetCount(); i++)
		{
			if (FormatInfoList[i].StatusID == MemberID)
			{
				FormatType = FormatInfoList[i].FormatType;
				MemberIDStr = FormatInfoList[i].szName;
				break;
			}
		}
		
		CEasyString ValueStr;
		switch(Value.GetType())
		{
		case CSmartValue::VT_CHAR:
			ValueStr.Format("%d",(char)Value);
			break;
		case CSmartValue::VT_UCHAR:
			ValueStr.Format("%u",(BYTE)Value);
			break;
		case CSmartValue::VT_SHORT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((short)Value,true);
			else
				ValueStr.Format("%d",(short)Value);
			break;
		case CSmartValue::VT_USHORT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((WORD)Value,true);
			else
				ValueStr.Format("%u",(WORD)Value);
			break;
		case CSmartValue::VT_INT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((int)Value,true);
			else
				ValueStr.Format("%d",(int)Value);
			break;
		case CSmartValue::VT_UINT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((UINT)Value,true);
			else
				ValueStr.Format("%u",(UINT)Value);
			break;
		case CSmartValue::VT_BIGINT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((INT64)Value,true);
			else if(FormatType==SSFT_VERSION)
			{
				ULONG64_CONVERTER Version;
				Version.QuadPart=Value;
				ValueStr.Format("%d.%d.%d.%d",
					Version.Words[3],Version.Words[2],Version.Words[1],Version.Words[0]);
			}
			else
				ValueStr.Format("%lld",(INT64)Value);
			break;
		case CSmartValue::VT_UBIGINT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWords((UINT64)Value,true);
			else if(FormatType==SSFT_VERSION)
			{
				ULONG64_CONVERTER Version;
				Version.QuadPart=Value;
				ValueStr.Format("%d.%d.%d.%d",
					Version.Words[3],Version.Words[2],Version.Words[1],Version.Words[0]);
			}
			else
				ValueStr.Format("%llu",(UINT64)Value);
			break;
		case CSmartValue::VT_FLOAT:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWordsFloat((float)Value,true);
			else if(FormatType==SSFT_PERCENT)
				ValueStr.Format("%.2f%%",(float)Value*100);
			else
				ValueStr.Format("%g",(float)Value);
			break;
		case CSmartValue::VT_DOUBLE:
			if(FormatType==SSFT_FLOW)
				ValueStr=FormatNumberWordsFloat((float)Value,true);
			else if(FormatType==SSFT_PERCENT)
				ValueStr.Format("%.2f%%",(double)Value*100);
			else
				ValueStr.Format("%g",(double)Value);
			break;
		case CSmartValue::VT_STRING:
			ValueStr=(LPCTSTR)Value;
			break;
		case CSmartValue::VT_USTRING:
			ValueStr=(LPCWSTR)Value;
			break;
		default:
			ValueStr="未知格式数据";
		}
		int Item=m_lvServerStatus.InsertItem(m_lvServerStatus.GetItemCount(),MemberIDStr);
		m_lvServerStatus.SetItemText(Item,1,ValueStr);
	}
}
void CDlgServerStatus::OnSize(UINT nType, int cx, int cy)
{
	CDialog::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if(::IsWindow(m_lvServerStatus.GetSafeHwnd()))
	{
		CRect ClientRect;
		GetClientRect(&ClientRect);

		m_lvServerStatus.MoveWindow(&ClientRect);
	}
}
