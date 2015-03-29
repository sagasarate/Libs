/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DButton.h                                              */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "d3dwnd.h"

namespace D3DGUI{

enum BUTTON_STATUS
{
	BUTTON_STATUS_NORMAL,
	BUTTON_STATUS_ACTIVE,
	BUTTON_STATUS_PRESS,
	BUTTON_STATUS_DISBALE,
	BUTTON_STATUS_MAX
};

struct BUTTON_STATUS_INFO
{
	IUITexture*	pTexture;
	CEasyRect		TextRect;	
	FLOAT_RECT	UVRect[RECT_TEXT];
	FLOAT_POINT	TextOffset;
	BUTTON_STATUS_INFO()
	{
		pTexture=NULL;
		TextOffset.x=0;
		TextOffset.y=0;
	}
};

class CD3DButton :
	public CD3DWnd
{
protected:
	BUTTON_STATUS_INFO	m_pStatusInfo[BUTTON_STATUS_MAX];
	int					m_CurStatus;
	
	DECLARE_CLASS_INFO(CD3DButton)
	
public:
	CD3DButton();
	CD3DButton(CD3DGUI * pGUI);
	virtual ~CD3DButton(void);

	virtual void Destory();

	virtual BOOL OnMessage(CD3DWnd * pWnd,UINT msg, WPARAM wParam, LPARAM lParam);

	virtual bool SetTexture(int Status,LPCTSTR TextureFileName,RECT& TextureRect,FLOAT_POINT TextOffset);
	virtual bool GetTexture(int Status,CEasyString& TextureFileName,RECT& TextureRect,FLOAT_POINT& TextOffset);
	
	virtual void SetBorders(WIN_BORDERS& Borders);
	
	void SetButtonStatus(int Status);
	int GetButtonStatus()
	{
		return m_CurStatus;
	}
	
	virtual void EnableWindow(bool IsEnable);
	
	virtual bool UpdateTextureRect();	
	virtual void UpdateText();

	virtual void SaveToXml(xml_node * pXMLNode);
	virtual bool LoadFromXml(xml_node * pXMLNode);

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

protected:
	void CaculateUV(int Status);
	void SaveTextureToXML(xml_node& Texture);
	void LoadTextureFromXML(xml_node& Texture);
};

}