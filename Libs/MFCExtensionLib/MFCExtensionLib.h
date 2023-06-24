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



inline void DDX_Text(CDataExchange* pDX, int nIDC, WORD& value)
{
	DDX_Text(pDX, nIDC, *((short *)&value));
}



extern bool ExportListViewToCSV(CListCtrl& ListView,LPCTSTR szExportFileName,int ExportCodePage);

extern bool AfxBrowseForFolder(LPCTSTR szTitke, CString& SelectedFolder, UINT Flag = BIF_RETURNONLYFSDIRS);

extern void DDX_Text(CDataExchange* pDX, int nIDC, CEasyString& value);
extern void DDX_Check(CDataExchange* pDX, int nIDC, bool& value);

template<typename T1,typename T2>
inline void DDX_Check(CDataExchange* pDX, int nIDC, T1& Flag, T2 TargetFlag)
{
	bool IsCheck = (Flag & TargetFlag) != 0;
	DDX_Check(pDX, nIDC, IsCheck);
	if (IsCheck)
		Flag = (T1)(Flag | TargetFlag);
	else
		Flag = (T1)(Flag & (~TargetFlag));
}


inline void DDX_DateTimeCtrl(CDataExchange* pDX, int nIDC, CEasyTime& value)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CDateTimeCtrl* pWnd = (CDateTimeCtrl*)CWnd::FromHandle(hWndCtrl);

	ENSURE(pWnd);
	if (pDX->m_bSaveAndValidate)
	{
		SYSTEMTIME Time;
		if (pWnd->GetTime(&Time) == GDT_VALID)
		{
			if ((pWnd->GetStyle()&DTS_TIMEFORMAT) == DTS_TIMEFORMAT)
			{
				value.Hour() = Time.wHour;
				value.Minute() = Time.wMinute;
				value.Second() = Time.wSecond;
				value.Milliseconds() = Time.wMilliseconds;
			}
			else
			{
				value.Year() = Time.wYear;
				value.Month() = Time.wMonth;
				value.Day() = Time.wDay;
			}
		}
	}
	else
	{
		SYSTEMTIME Time;
		value.GetTime(Time);
		pWnd->SetTime(&Time);
	}
}

inline void ListCtrlColAutoFit(CListCtrl& lv)
{
	lv.SetRedraw(FALSE);
	int nColumnCount = lv.GetHeaderCtrl()->GetItemCount();

	for (int i = 0; i < nColumnCount; i++)
	{
		lv.SetColumnWidth(i, LVSCW_AUTOSIZE);
		int nColumnWidth = lv.GetColumnWidth(i);
		lv.SetColumnWidth(i, LVSCW_AUTOSIZE_USEHEADER);
		int nHeaderWidth = lv.GetColumnWidth(i);
		lv.SetColumnWidth(i, max(nColumnWidth, nHeaderWidth));
	}
	lv.SetRedraw(TRUE);
}

inline void ListCtrlClearSelections(CListCtrl& lv)
{
	POSITION Pos = lv.GetFirstSelectedItemPosition();
	while (Pos)
	{
		int Item = lv.GetNextSelectedItem(Pos);
		lv.SetItemState(Item, 0, LVIS_SELECTED);
	}
}