/****************************************************************************/
/*                                                                          */
/*      文件名:    USOD3DObjectCreateFilter.cpp                             */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CUSOD3DObjectCreateFilter,CUSOObjectCreateFilter);

CUSOD3DObjectCreateFilter::CUSOD3DObjectCreateFilter()
{
	m_pD3DDevice=NULL;
}

CUSOD3DObjectCreateFilter::CUSOD3DObjectCreateFilter(CD3DDevice * pDevice)
{
	m_pD3DDevice=pDevice;
}

CUSOD3DObjectCreateFilter::~CUSOD3DObjectCreateFilter(void)
{
}

void CUSOD3DObjectCreateFilter::SetDevice(CD3DDevice * pDevice)
{
	m_pD3DDevice=pDevice;
}

bool CUSOD3DObjectCreateFilter::OnObjectCreate(CNameObject * pObject)
{
	if(m_pD3DDevice)
	{
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DSubMesh)))
		{
			((CD3DSubMesh *)pObject)->SetDevice(m_pD3DDevice);			
		}
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DObject)))
		{
			((CD3DObject *)pObject)->SetDevice(m_pD3DDevice);			
		}
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DObjectResource)))
		{
			((CD3DObjectResource *)pObject)->SetManager(m_pD3DDevice->GetObjectResourceManager());
		}
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DTexture)))
		{
			((CD3DTexture *)pObject)->SetManager(m_pD3DDevice->GetTextureManager());
		}
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DFX)))
		{
			((CD3DFX *)pObject)->SetManager(m_pD3DDevice->GetFXManager());
		}
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DBaseFont)))
		{
			((CD3DBaseFont *)pObject)->SetManager(m_pD3DDevice->GetFontManager());
		}
	}
	return true;
}

CNameObject * CUSOD3DObjectCreateFilter::FindObject(CLASS_INFO * pClassInfo,LPCTSTR ObjectName)
{
	CNameObject * pObject=NULL;
	if(m_pD3DDevice)
	{
		if(pClassInfo->IsKindOf(GET_CLASS_INFO(CD3DObjectResource)))
		{
			pObject=m_pD3DDevice->GetObjectResourceManager()->GetResource(ObjectName);
		}		
		else if(pClassInfo->IsKindOf(GET_CLASS_INFO(CD3DTexture)))
		{
			pObject=m_pD3DDevice->GetTextureManager()->GetTextrue(ObjectName);
		}
		else if(pClassInfo->IsKindOf(GET_CLASS_INFO(CD3DFX)))
		{
			pObject=m_pD3DDevice->GetFXManager()->GetFX(ObjectName);
		}
		else if(pClassInfo->IsKindOf(GET_CLASS_INFO(CD3DBaseFont)))
		{
			pObject=m_pD3DDevice->GetFontManager()->GetFont(ObjectName);
		}
	}
	return pObject;
}

}