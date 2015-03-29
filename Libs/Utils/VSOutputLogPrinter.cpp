/****************************************************************************/
/*                                                                          */
/*      文件名:    VSOutputLogPrinter.cpp                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CVSOutputLogPrinter,CNameObject);

CVSOutputLogPrinter::CVSOutputLogPrinter(void):ILogPrinter()
{
}

CVSOutputLogPrinter::~CVSOutputLogPrinter(void)
{
}

void CVSOutputLogPrinter::PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl)
{
#ifdef WIN32
	TCHAR MsgBuff[4096];
	_vstprintf_s( MsgBuff, 4096, Format, vl );
	OutputDebugString(MsgBuff);
	OutputDebugString(_T("\r\n"));
#else
	vprintf(Format,vl);
	printf(_T("\n"));
#endif
}
