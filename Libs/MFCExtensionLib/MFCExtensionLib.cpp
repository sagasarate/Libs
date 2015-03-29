#include "stdafx.h"

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