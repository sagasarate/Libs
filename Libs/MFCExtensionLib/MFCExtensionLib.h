#pragma once

#include "PropertyGrid.h"
#include "DialogItemSorter.h"

inline int AfxMessageBoxEx(UINT nType,UINT nIDHelp,LPCTSTR szFormat,...)
{
	va_list	vl;
	va_start(vl,szFormat);
	CEasyString Msg;
	Msg.FormatVL(szFormat,vl);
	int Ret=AfxMessageBox(Msg,nType,nIDHelp);
	va_end( vl);
	return Ret;
}

extern bool ExportListViewToCSV(CListCtrl& ListView,LPCTSTR szExportFileName,int ExportCodePage);