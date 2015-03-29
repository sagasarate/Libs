/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBaseFont.cpp                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\d3dbasefont.h"
#include ".\d3dfontmanager.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO_STATIC(CD3DBaseFont,CNameObject);

CD3DBaseFont::CD3DBaseFont()
{
	m_pManager=NULL;	
}

CD3DBaseFont::CD3DBaseFont(CD3DFontManager * pManager)
{
	m_pManager=pManager;	
}

CD3DBaseFont::~CD3DBaseFont(void)
{
	if(m_pManager&&GetID())
	{
		m_pManager->DeleteD3DFont(GetID());
	}
}


}