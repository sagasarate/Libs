/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DCombo.h                                               */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "d3dedit.h"


namespace D3DGUI{

class CD3DCombo :
	public CD3DEdit
{
protected:
	CD3DButton *			m_pExpandButton;
	CD3DSimpleList *		m_pComboList;

	bool					m_IsListShow;
	UINT					m_ComboListHeight;

	DECLARE_CLASS_INFO(CD3DCombo)
public:
	CD3DCombo();
	CD3DCombo(CD3DGUI * pGUI);
	virtual ~CD3DCombo(void);
	virtual void InitWnd(CD3DGUI *  pGUI);

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void SetMultiLine(bool IsMultiLine){}
	virtual void EnableScrollBar(bool Enable){}
	virtual void SetVisible(bool IsVisible);

	virtual CEasyRect GetCenterRect();

	virtual void UpdateRects();

	CD3DButton * GetExpandButton()
	{
		return m_pExpandButton;
	}
	
	CD3DSimpleList * GetComboList()
	{
		return m_pComboList;
	}
	void SetComboListHeight(UINT Height)
	{
		m_ComboListHeight=Height;
	}
	UINT GetComboListHeight()
	{
		return m_ComboListHeight;
	}

	void SelectItem(int Index);	

	int AddItem(LPCTSTR ItemText);
	bool InsertItem(int Index,LPCTSTR ItemText);
	bool DeleteItem(int Index);
	void DeleteAllItem();

	int GetItemCount();
	bool SetItemText(int Index,LPCTSTR Text);
	bool GetItemText(int Index,CEasyString& Text);
	bool SetItemData(int Index,LPVOID pData);
	LPVOID GetItemData(int Index);

	int GetFirstSelectedItem();
	int GetNextSelectedItem(int Index);

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

protected:
	void SaveFrameToXML(xml_node& Frame);
	void LoadFrameFromXML(xml_node& Frame);
	virtual void GetMiniSize(int& Width,int& Height);
};

}