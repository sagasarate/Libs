/****************************************************************************/
/*                                                                          */
/*      文件名:    PropertyConfigDialog.h                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


// CPropertyConfigDialog 对话框
namespace PropertyGrid
{
class CPropertyConfigDialog : public CDialog
{
	DECLARE_DYNAMIC(CPropertyConfigDialog)

public:
	CPropertyConfigDialog(UINT nIDTemplate, CWnd* pParentWnd = NULL);   // 标准构造函数
	virtual ~CPropertyConfigDialog();

// 对话框数据
	bool IsModual()
	{
		return m_IsModual;
	}

protected:
	bool	m_IsModual;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
};
}