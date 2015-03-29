/****************************************************************************/
/*                                                                          */
/*      文件名:    LinkComboBox.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"
#include "LinkComboBox.h"
#include "Inputer.h"

namespace PropertyGrid
{
IMPLEMENT_DYNAMIC(CLinkComboBox, CComboBox)
CLinkComboBox::CLinkComboBox()
{
}

CLinkComboBox::~CLinkComboBox()
{
}


BEGIN_MESSAGE_MAP(CLinkComboBox, CComboBox)
	ON_WM_KILLFOCUS()	
END_MESSAGE_MAP()


void CLinkComboBox::OnKillFocus(CWnd* pNewWnd)
{
	//CLinkComboBox::OnKillFocus(pNewWnd);
	if(IsWindowVisible())
		((CInputer *)GetParent())->LostInputFocus();
	
	//ShowWindow(SW_HIDE);
	

	// TODO: 在此处添加消息处理程序代码
}


// CLinkComboBox 消息处理程序
}