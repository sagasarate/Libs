/****************************************************************************/
/*                                                                          */
/*      文件名:    LinkEdit.h                                               */
/*      创建日期:  2009年07月23日                                           */
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
class CLinkEdit : public CEdit
{
	DECLARE_DYNAMIC(CLinkEdit)
protected:
	bool					m_EnableInput;
	
	
public:
	CLinkEdit();
	virtual ~CLinkEdit();	
	

	void EnableInput(bool IsEnable)
	{
		m_EnableInput=IsEnable;
	}

protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnKillFocus(CWnd* pNewWnd);		
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);	
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

};

}