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

inline void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, CEasyString& value)
{
	CString Temp = value;
	DDX_Text(pDX, nIDC, Temp);
	value = Temp;
}

inline void AFXAPI DDX_Text(CDataExchange* pDX, int nIDC, WORD& value)
{
	DDX_Text(pDX, nIDC, *((short *)&value));
}

extern bool ExportListViewToCSV(CListCtrl& ListView,LPCTSTR szExportFileName,int ExportCodePage);

extern bool AfxBrowseForFolder(LPCTSTR szTitke, CString& SelectedFolder, UINT Flag = BIF_RETURNONLYFSDIRS);