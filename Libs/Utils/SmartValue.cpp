/****************************************************************************/
/*                                                                          */
/*      文件名:    SmartValue.cpp                                           */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

bool CSmartValue::m_ConvertWideCharToUTF8=false;

int CSmartValue::GetTypeFromData(LPCVOID pData,UINT DataSize)
{
	if(pData==NULL||DataSize<sizeof(BYTE))
		return VT_UNKNOWN;

	return *((BYTE *)pData);
}
void CSmartValue::EnableConvertWideCharToUTF8(bool Enable)
{
	m_ConvertWideCharToUTF8=Enable;
}
bool CSmartValue::IsConvertWideCharToUTF8()
{
	return m_ConvertWideCharToUTF8;
}
