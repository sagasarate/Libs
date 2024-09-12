/****************************************************************************/
/*                                                                          */
/*      文件名:    USOObjectCreateFilter.cpp                                */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO(CUSOObjectCreateFilter,CNameObject);

CUSOObjectCreateFilter::CUSOObjectCreateFilter(void)
{
}

CUSOObjectCreateFilter::~CUSOObjectCreateFilter(void)
{
}

bool CUSOObjectCreateFilter::OnObjectCreate(CNameObject * pObject)
{
	return true;
}

CNameObject * CUSOObjectCreateFilter::FindObject(CLASS_INFO * pClassInfo,LPCTSTR ObjectName)
{
	return NULL;
}
