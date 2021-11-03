#include "stdafx.h"
#include <afxpriv.h>

bool ExportListViewToCSV(CListCtrl& ListView,LPCTSTR szExportFileName,int ExportCodePage)
{
	CCSVReader CSVFile;

	CSVFile.SetSaveCodePage(ExportCodePage);

	CSVFile.Create(65536,65536);

	CHeaderCtrl * pHeadCtrl=ListView.GetHeaderCtrl();

	

	if(pHeadCtrl)
	{
		TCHAR szBuff[1024];
		UINT ColCount=pHeadCtrl->GetItemCount();
		for(UINT i=0;i<ColCount;i++)
		{
			HDITEM hdi;

			hdi.mask = HDI_TEXT;
			hdi.pszText = szBuff;
			hdi.cchTextMax = 1024;

			if(pHeadCtrl->GetItem(i,&hdi))
			{
				CSVFile.AddColumn(hdi.pszText);
			}
			else
			{
				return false;
			}
		}

		for(int i=0;i<ListView.GetItemCount();i++)
		{
			CSVFile.AddRow();
			for(UINT j=0;j<ColCount;j++)
			{
				CSVFile.AddDataString(ListView.GetItemText(i,j));
			}
		}

		return CSVFile.Save(szExportFileName);
	}
	return false;
}

static CString s_DefaultBrowseFolder;
int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
	if (uMsg == BFFM_INITIALIZED)
	{
		if (!s_DefaultBrowseFolder.IsEmpty())
		{
			SendMessage(hwnd, BFFM_SETSELECTION,
				TRUE, (LPARAM)((LPCTSTR)s_DefaultBrowseFolder));
		}
	}
	return 0;
}

bool AfxBrowseForFolder(LPCTSTR szTitke, CString& SelectedFolder, UINT Flag)
{
	BROWSEINFO bi;
	TCHAR FilePath[MAX_PATH];
	LPITEMIDLIST pidl;

	ZeroMemory(&bi, sizeof(bi));

	//bi.pszDisplayName=FilePath;
	s_DefaultBrowseFolder = SelectedFolder;
	bi.lpszTitle = szTitke;
	bi.ulFlags = Flag;
	bi.lpfn = BrowseCallbackProc;
	pidl = ::SHBrowseForFolder(&bi);
	if (pidl != NULL)
	{
		::SHGetPathFromIDList(pidl, FilePath);
		IMalloc *pMalloc = NULL;
		::SHGetMalloc(&pMalloc);
		pMalloc->Free(pidl);
		pMalloc->Release();
		pMalloc = NULL;

		SelectedFolder = FilePath;
		return true;
	}
	return false;
}

void DDX_Text(CDataExchange* pDX, int nIDC, CEasyString& value)
{
	HWND hWndCtrl = pDX->PrepareEditCtrl(nIDC);
	if (pDX->m_bSaveAndValidate)
	{
		int nLen = ::GetWindowTextLength(hWndCtrl);
		value.Resize(nLen);
		::GetWindowText(hWndCtrl, (LPTSTR)value.GetBuffer(), nLen + 1);
		value.TrimBuffer(nLen);
	}
	else
	{
		AfxSetWindowText(hWndCtrl, value);
	}
}



void DDX_Check(CDataExchange* pDX, int nIDC, bool& value)
{
	pDX->PrepareCtrl(nIDC);
	HWND hWndCtrl;
	pDX->m_pDlgWnd->GetDlgItem(nIDC, &hWndCtrl);
	if (pDX->m_bSaveAndValidate)
	{
		value = (int)::SendMessage(hWndCtrl, BM_GETCHECK, 0, 0L) != 0;
	}
	else
	{
		::SendMessage(hWndCtrl, BM_SETCHECK, (WPARAM)(value ? TRUE : FALSE), 0L);
	}
}