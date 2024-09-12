/****************************************************************************/
/*                                                                          */
/*      文件名:    Inputer.cpp                                              */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// Inputer.cpp : 实现文件
//

#include "stdafx.h"
#include "Inputer.h"
#include "PropertyGrid.h"

// CInputer
namespace PropertyGrid
{

IMPLEMENT_DYNAMIC(CInputer, CWnd)
CInputer::CInputer()
{
	m_Type=0;
	m_IsOpenFile = true;
	m_DefaultFileName = _T("*.*");
	m_DefaultFileExt = _T("*.*");
	m_FileExtPattern = _T("All File(*.*)|*.*||");
	m_FuctionDialog=NULL;
}

CInputer::~CInputer()
{
}


BEGIN_MESSAGE_MAP(CInputer, CWnd)
	ON_BN_CLICKED(ID_FUCTION_BUTTON, OnBnClicked)
END_MESSAGE_MAP()



// CInputer 消息处理程序
BOOL CInputer::Create( CWnd* pParentWnd,CFont * pFont)
{
	// TODO: 在此添加专用代码和/或调用基类

	if(!CWnd::Create(NULL,_T(""), WS_CHILD, CRect(0,0,0,0), pParentWnd,3384, NULL))
		return false;

	m_Edit.Create(WS_VISIBLE|WS_CHILD|ES_AUTOHSCROLL,CRect(0,0,0,0),this,11001);
	m_Edit.SetFont(pFont);
	m_Edit.ShowWindow(SW_HIDE);

	m_ComboBox.Create(WS_VISIBLE|WS_CHILD|CBS_DROPDOWNLIST,CRect(0,0,0,0),this,11002);
	m_ComboBox.SetFont(pFont);
	m_ComboBox.ShowWindow(SW_HIDE);

	m_Spin.Create(WS_VISIBLE|WS_CHILD|UDS_ALIGNRIGHT|UDS_SETBUDDYINT|UDS_NOTHOUSANDS,CRect(0,0,20,20),this,103883);
	m_Spin.ShowWindow(SW_HIDE);
	
	m_Button.Create(_T("…"),WS_VISIBLE|WS_CHILD|BS_NOTIFY|BS_PUSHBUTTON,CRect(0,0,20,20),this,ID_FUCTION_BUTTON);
	m_Button.ShowWindow(SW_HIDE);

	return true;
}

void CInputer::StartInput(int Type,CString& Define,CString& DefaultValue,CPropertyConfigDialog * pFuctionDialog)
{
	CRect Rect;

	m_Type=Type;
	m_FuctionDialog=pFuctionDialog;	
	switch(m_Type)
	{
	case PROT_NONE:
		break;
	case PROT_EDIT:
		{
			//设置编辑框大小
			GetWindowRect(&Rect);			
			ScreenToClient(&Rect);
			m_Edit.MoveWindow(&Rect);
			m_Edit.ShowWindow(SW_SHOW);
			m_Edit.SetWindowText((LPCTSTR)DefaultValue);
			m_Edit.SetFocus();
		}
		break;
	case PROT_NUMBER:
		{
			//设置编辑框大小
			GetWindowRect(&Rect);			
			ScreenToClient(&Rect);
			m_Edit.MoveWindow(&Rect);	
			m_Edit.ModifyStyle(0,ES_NUMBER);
			m_Edit.ShowWindow(SW_SHOW);
			m_Edit.SetWindowText((LPCTSTR)DefaultValue);
			m_Edit.SetFocus();
		}
		break;
	case PROT_SPIN_NUMBER:
		{
			//设置编辑框大小
			GetWindowRect(&Rect);			
			ScreenToClient(&Rect);
			m_Edit.MoveWindow(&Rect);	
			m_Edit.ModifyStyle(0,ES_NUMBER);
			m_Edit.ShowWindow(SW_SHOW);
			m_Edit.SetWindowText((LPCTSTR)DefaultValue);
			m_Edit.SetFocus();

			CSettingFile SettingStr;
			SettingStr.Load(Define);
			int Min=SettingStr.GetInteger(_T("Define"),_T("Min"),0);
			int Max=SettingStr.GetInteger(_T("Define"),_T("Max"),99999);
			m_Spin.ShowWindow(SW_SHOW);	
			m_Spin.SetBuddy(&m_Edit);
			m_Spin.SetRange32(Min,Max);	

			
		}
		break;
	case PROT_COMBO:
		{
		
			//设置组合框大小
			GetWindowRect(&Rect);			
			ScreenToClient(&Rect);
			Rect.bottom=Rect.top+100;
			m_ComboBox.MoveWindow(&Rect);			

			m_ComboBox.ResetContent();
			CStringSplitter Splitter(Define,';');
			for (UINT i = 0; i < Splitter.GetCount(); i++)
			{
				if (_tcslen(Splitter[i]) > 0)
				{
					m_ComboBox.InsertString(i, Splitter[i]);
					if (DefaultValue == Splitter[i])
						m_ComboBox.SetCurSel(i);
				}
			}
			Rect.bottom = Rect.top + 20 * Splitter.GetCount() + 50;
			m_ComboBox.MoveWindow(&Rect);
			m_ComboBox.ShowWindow(SW_SHOW);
			m_ComboBox.SetFocus();
		}

		break;
	case PROT_BOOL:
		{
			//设置组合框大小
			GetWindowRect(&Rect);			
			ScreenToClient(&Rect);
			Rect.bottom=Rect.top+100;
			m_ComboBox.MoveWindow(&Rect);			

			m_ComboBox.ResetContent();
			m_ComboBox.InsertString(0,_T("是"));
			m_ComboBox.InsertString(1,_T("否"));
			if(DefaultValue==_T("是"))
				m_ComboBox.SetCurSel(0);
			else
				m_ComboBox.SetCurSel(1);
			m_ComboBox.ShowWindow(SW_SHOW);
			m_ComboBox.SetFocus();
		}
		break;
	case PROT_COLOR:
		break;
	case PROT_FONT:
		break;
	case PROT_COLOREX:
		break;
	case PROT_DIRECT:
		break;
	case PROT_PATH:
		{			
			//设置编辑框大小
			GetWindowRect(&Rect);
			Rect.right-=Rect.Height();
			ScreenToClient(&Rect);
			m_Edit.MoveWindow(&Rect);	
			m_Edit.ShowWindow(SW_SHOW);
			m_Edit.SetWindowText((LPCTSTR)DefaultValue);
			m_Edit.SetFocus();

			//设置按钮大小
			GetWindowRect(&Rect);
			Rect.left=Rect.right-Rect.Height();
			ScreenToClient(&Rect);
			m_Button.MoveWindow(&Rect);
			m_Button.ShowWindow(SW_SHOW);
			if (Define == _T("Save"))
				m_IsOpenFile = false;
			CStringSplitter Splitter(Define, ';');
			if (Splitter.GetCount() >= 1)
			{
				if(_tcscmp(Splitter[0], _T("Save"))==0)
					m_IsOpenFile = false;
				else
					m_IsOpenFile = true;
			}
			else
			{
				m_IsOpenFile = true;
			}
			if (Splitter.GetCount() >= 2)
			{
				m_DefaultFileExt = Splitter[1];
			}
			else
			{
				m_DefaultFileExt = _T("*.*");
			}
			if (Splitter.GetCount() >= 3)
			{
				m_DefaultFileName = Splitter[2];
			}
			else
			{
				m_DefaultFileName = _T("*.*");
			}
			if (Splitter.GetCount() >= 4)
			{
				m_FileExtPattern = Splitter[3];
			}
			else
			{
				m_FileExtPattern = _T("All File(*.*)|*.*||");
			}
		}
		break;
	case PROT_CUSTOM:
		{
			//设置编辑框大小
			GetWindowRect(&Rect);
			Rect.right-=Rect.Height();
			ScreenToClient(&Rect);
			m_Edit.MoveWindow(&Rect);	
			m_Edit.ShowWindow(SW_SHOW);
			m_Edit.SetWindowText((LPCTSTR)DefaultValue);
			m_Edit.EnableInput(false);
			m_Edit.SetFocus();

			//设置按钮大小
			GetWindowRect(&Rect);
			Rect.left=Rect.right-Rect.Height();
			ScreenToClient(&Rect);
			m_Button.MoveWindow(&Rect);
			m_Button.ShowWindow(SW_SHOW);			
		}
		break;
	}	
	
	ShowWindow(SW_SHOW);
}

void CInputer::LostInputFocus(bool IsCheckMouse)
{
	if(IsCheckMouse)
	{	
		if(m_Spin.IsWindowVisible())
		{
			CPoint Point;
			GetCursorPos(&Point);
			CRect Rect;
			m_Spin.GetWindowRect(&Rect);
			if(Rect.PtInRect(Point))
				return;
		}
		if(m_Button.IsWindowVisible())
		{
			CPoint Point;
			GetCursorPos(&Point);
			CRect Rect;
			m_Button.GetWindowRect(&Rect);
			if(Rect.PtInRect(Point))
				return;
		}
		
		if(m_ComboBox.IsWindowVisible())
		{
			CPoint Point;
			GetCursorPos(&Point);
			CRect Rect;
			m_ComboBox.GetWindowRect(&Rect);
			if(Rect.PtInRect(Point))
				return;
		}

		if(m_Edit.IsWindowVisible())
		{
			CPoint Point;
			GetCursorPos(&Point);
			CRect Rect;
			m_Edit.GetWindowRect(&Rect);
			if(Rect.PtInRect(Point))
				return;
		}
	}
	if(m_Type==PROT_CUSTOM)
		EndInput(false);
	else
		EndInput(true);
}

void CInputer::EndInput(bool IsChanged)
{
	((CPropertyGrid *)GetParent())->OnFinishEdit(IsChanged);
	ShowWindow(SW_HIDE);
	m_Edit.ShowWindow(SW_HIDE);
	m_ComboBox.ShowWindow(SW_HIDE);
	m_Button.ShowWindow(SW_HIDE);
	m_Spin.ShowWindow(SW_HIDE);
	m_Edit.ModifyStyle(ES_NUMBER,0);
	m_Edit.EnableInput(true);

}

void CInputer::OnBnClicked()
{
	switch(m_Type)
	{
	case PROT_NONE:
		break;
	case PROT_EDIT:
		break;
	case PROT_NUMBER:
		break;
	case PROT_SPIN_NUMBER:
		break;
	case PROT_COMBO:
		break;
	case PROT_BOOL:
		break;
	case PROT_COLOR:
		break;
	case PROT_FONT:
		break;
	case PROT_COLOREX:
		break;
	case PROT_DIRECT:
		break;
	case PROT_PATH:
		{
			CFileDialog dlg(m_IsOpenFile, m_DefaultFileExt, m_DefaultFileName, OFN_OVERWRITEPROMPT, m_FileExtPattern);

			if(dlg.DoModal()==IDOK)
			{
				m_Edit.SetWindowText((LPCTSTR)dlg.GetPathName());				
			}
			
		}
		break;
	case PROT_CUSTOM:
		{
			if(m_FuctionDialog)
			{
				if(m_FuctionDialog->IsModual())
				{
					if(m_FuctionDialog->DoModal()==IDOK)
						EndInput(true);
					else
						EndInput(false);
				}
				else
				{
					//m_FuctionDialog->OnInitDialog();
					m_FuctionDialog->ShowWindow(SW_SHOW);
					EndInput(false);
				}
			}
		}
		break;
	}
	m_Edit.SetFocus();
}

CString CInputer::GetInputText()
{
	CString Text;
	switch(m_Type)
	{
	case PROT_NONE:
		break;
	case PROT_EDIT:		
	case PROT_NUMBER:		
	case PROT_SPIN_NUMBER:
	case PROT_COLOR:		
	case PROT_FONT:		
	case PROT_COLOREX:		
	case PROT_DIRECT:		
	case PROT_PATH:		
	case PROT_CUSTOM:
		m_Edit.GetWindowText(Text);
		break;
	case PROT_COMBO:		
	case PROT_BOOL:
		m_ComboBox.GetWindowText(Text);
		break;
	
		
	}

	
	
	return Text;
}

}

