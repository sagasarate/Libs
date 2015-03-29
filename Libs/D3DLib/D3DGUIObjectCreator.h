/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUIObjectCreator.h                                    */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "D3DGUIWndRect.h"
#include "D3DGUITexture.h"
#include "D3DGUITextRect.h"

namespace D3DGUI{

class CD3DGUIObjectCreator :
	public CNameObject,public IUIObjectCreator
{
protected:
	CD3DDevice *	m_pDevice;
	CD3DUIRender *	m_pRender;

	DECLARE_CLASS_INFO_STATIC(CD3DGUIObjectCreator)
public:
	CD3DGUIObjectCreator(CD3DUIRender *	pRender);
	virtual ~CD3DGUIObjectCreator(void);

	virtual IUIWndRect * CreateWndRect(FLOAT_RECT * pRect);
	virtual IUITextRect * CreateTextRect(FLOAT_RECT * pRect,const LOGFONT* pLogFont);
	virtual IUITexture * CreateTextureFromFile(LPCTSTR FileName);
	virtual void SetTextureDir(LPCTSTR Dir);

	virtual void Release()
	{
		CNameObject::Release();
	}
};

}