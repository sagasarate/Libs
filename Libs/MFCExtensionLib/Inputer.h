/****************************************************************************/
/*                                                                          */
/*      文件名:    Inputer.h                                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "LinkEdit.h"
#include "LinkComboBox.h"
#include "PropertyConfigDialog.h"


#define ID_FUCTION_BUTTON	3762
// CInputer
namespace PropertyGrid
{
class CInputer : public CWnd
{
	DECLARE_DYNAMIC(CInputer)

public:
	CInputer();
	virtual ~CInputer();

protected:
	DECLARE_MESSAGE_MAP()

	afx_msg void OnBnClicked();

protected:
	CLinkEdit				m_Edit;
	CLinkComboBox			m_ComboBox;
	CSpinButtonCtrl			m_Spin;
	CButton					m_Button;

	int						m_Type;
	bool					m_IsOpenFile;
	CString					m_DefaultFileName;
	CString					m_DefaultFileExt;
	CString					m_FileExtPattern;
	CPropertyConfigDialog *	m_FuctionDialog;


public:
	virtual BOOL Create(CWnd* pParentWnd,CFont * pFont);

	void StartInput(int Type,CString& Define,CString& DefaultValue,CPropertyConfigDialog * pFuctionDialog);

	void LostInputFocus(bool IsCheckMouse=true);

	void EndInput(bool IsChanged);
	

	CString GetInputText();
};

}
