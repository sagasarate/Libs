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
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CVSOutputLogPrinter,CNameObject);

CVSOutputLogPrinter::CVSOutputLogPrinter(void):ILogPrinter()
{
}

CVSOutputLogPrinter::~CVSOutputLogPrinter(void)
{
}

void CVSOutputLogPrinter::PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg)
{
#ifdef WIN32	
	if (Tag)
	{
		OutputDebugString(_T("["));
		OutputDebugString(Tag);
		OutputDebugString(_T("]"));
	}
	OutputDebugString(Msg);
	OutputDebugString(_T("\r\n"));
#else
	if (Tag)
	{
		printf(_T("[%s]"), Tag);
	}
	printf(_T("%s\n"),Msg);
#endif
}

void CVSOutputLogPrinter::PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl)
{
#ifdef WIN32
	if (Tag)
	{
		OutputDebugString(_T("["));
		OutputDebugString(Tag);
		OutputDebugString(_T("]"));
	}
	TCHAR MsgBuff[4096];
	_vstprintf_s( MsgBuff, 4096, Format, vl );
	OutputDebugString(MsgBuff);
	OutputDebugString(_T("\r\n"));
#else
	if (Tag)
	{
		printf(_T("[%s]"), Tag);
	}
	vprintf(Format,vl);
	printf(_T("\n"));
#endif
}
