/****************************************************************************/
/*                                                                          */
/*      文件名:    PropertyConfigDialog.cpp                                 */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
// PropertyConfigDialog.cpp : 实现文件
//

#include "stdafx.h"
#include "PropertyConfigDialog.h"

namespace PropertyGrid
{
// CPropertyConfigDialog 对话框

IMPLEMENT_DYNAMIC(CPropertyConfigDialog, CDialog)
CPropertyConfigDialog::CPropertyConfigDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
	m_IsModual=true;
}

CPropertyConfigDialog::~CPropertyConfigDialog()
{
}

void CPropertyConfigDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(CPropertyConfigDialog, CDialog)
END_MESSAGE_MAP()


// CPropertyConfigDialog 消息处理程序
}