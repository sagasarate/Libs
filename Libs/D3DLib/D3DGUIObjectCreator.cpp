/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUIObjectCreator.cpp                                  */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include "D3DGUIObjectCreator.h"
namespace D3DGUI{

IMPLEMENT_CLASS_INFO_STATIC(CD3DGUIObjectCreator,CNameObject);

CD3DGUIObjectCreator::CD3DGUIObjectCreator(CD3DUIRender *	pRender)
{
	m_pRender=pRender;
	m_pDevice=pRender->GetDevice();
}

CD3DGUIObjectCreator::~CD3DGUIObjectCreator(void)
{
}


IUIWndRect * CD3DGUIObjectCreator::CreateWndRect(FLOAT_RECT * pRect)
{
	FLOAT_RECT InitRect(0,0,100,100);
	if(pRect)
		InitRect=*pRect;
	CD3DGUIWndRect * pWndRect=new CD3DGUIWndRect(InitRect);
	//pWndRect->SetRender(m_pRender);
	m_pRender->AddObject(pWndRect);

	return dynamic_cast<IUIWndRect *>(pWndRect);
}
IUITextRect * CD3DGUIObjectCreator::CreateTextRect(FLOAT_RECT * pRect,const LOGFONT* pLogFont)
{
	FLOAT_RECT InitRect(0,0,100,100);
	if(pRect)
		InitRect=*pRect;
	CD3DGUITextRect * pTextRect=new CD3DGUITextRect(InitRect);	
	//pTextRect->SetRender(m_pRender);	
	m_pRender->AddObject(pTextRect);
	if(pLogFont)
		pTextRect->SetFont(pLogFont);
	

	return dynamic_cast<IUITextRect *>(pTextRect);
}
IUITexture * CD3DGUIObjectCreator::CreateTextureFromFile(LPCTSTR FileName)
{
	CD3DGUITexture * pTexture;
	pTexture=dynamic_cast<CD3DGUITexture *>(m_pDevice->GetTextureManager()->GetTextrue(FileName));

	if(pTexture==NULL)
	{	
		pTexture=new CD3DGUITexture(m_pDevice->GetTextureManager());
		if(pTexture)
		{
			if(!pTexture->LoadTexture(FileName,1,false))
			{
				delete pTexture;
				return NULL;
			}
			m_pDevice->GetTextureManager()->AddTexture(pTexture,FileName);
		}
	}
	else
	{
		pTexture->AddUseRef();
	}
	return pTexture;
}

void CD3DGUIObjectCreator::SetTextureDir(LPCTSTR Dir)
{
	CD3DTexture::ClearPath();
	CD3DTexture::AddPath(Dir);
}

}