/****************************************************************************/
/*                                                                          */
/*      文件名:    LinkComboBox.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace PropertyGrid
{
class CLinkComboBox : public CComboBox
{
	DECLARE_DYNAMIC(CLinkComboBox)
protected:
	
public:
	CLinkComboBox();
	virtual ~CLinkComboBox();	
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
};
}