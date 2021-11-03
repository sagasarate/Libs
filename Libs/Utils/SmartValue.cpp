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

#ifdef WIN32
#ifdef UNICODE
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UCS16;
#else
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_ANSI;
#endif
#else
CEasyString::STRING_CODE_PAGE CSmartValue::INTERNAL_STRING_CODE_PAGE = CEasyString::STRING_CODE_PAGE_UTF8;
#endif

BYTE CSmartValue::NULL_DATA = VT_NULL;

int CSmartValue::GetTypeFromData(LPCVOID pData, UINT DataSize)
{
	if (pData == NULL || DataSize < sizeof(BYTE))
		return VT_UNKNOWN;

	return *((BYTE *)pData);
}
