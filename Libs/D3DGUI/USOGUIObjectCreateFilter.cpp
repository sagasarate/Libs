/****************************************************************************/
/*                                                                          */
/*      文件名:    USOGUIObjectCreateFilter.cpp                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\usoguiobjectcreatefilter.h"

namespace D3DGUI{

IMPLEMENT_CLASS_INFO(CUSOGUIObjectCreateFilter,CUSOObjectCreateFilter);

CUSOGUIObjectCreateFilter::CUSOGUIObjectCreateFilter(void)
{
	m_pGUI=NULL;
}

CUSOGUIObjectCreateFilter::CUSOGUIObjectCreateFilter(CD3DGUI * pGUI)
{
	m_pGUI=pGUI;
}


CUSOGUIObjectCreateFilter::~CUSOGUIObjectCreateFilter(void)
{
}

void CUSOGUIObjectCreateFilter::SetGUIManager(CD3DGUI * pGUI)
{
	m_pGUI=pGUI;
}

bool CUSOGUIObjectCreateFilter::OnObjectCreate(CNameObject * pObject)
{
	if(m_pGUI)
	{
		if(pObject->IsKindOf(GET_CLASS_INFO(CD3DWnd)))
		{
			((CD3DWnd *)pObject)->InitWnd(m_pGUI);			
		}		
	}
	return true;
}

CNameObject * CUSOGUIObjectCreateFilter::FindObject(CLASS_INFO * pClassInfo,LPCTSTR ObjectName)
{
	CNameObject * pObject=NULL;
	if(m_pGUI)
	{
		pObject=m_pGUI->GetWndByName(ObjectName);		
	}
	return pObject;
}

}