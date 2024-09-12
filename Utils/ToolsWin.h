/****************************************************************************/
/*                                                                          */
/*      文件名:    ToolsWin32.h                                             */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once






#define CHECK_HRESULT(hr) \
	if(FAILED(hr))\
{\
	PrintSystemLog(_T("HResult Error:<%s><%d>[%x]"),__FILE__,__LINE__,hr);\
	return false;\
}


inline void DoSleep(UINT nMilliseconds)
{
	Sleep(nMilliseconds);
}




extern CEasyString GetFontNameEN(LOGFONT * pFont);
extern CEasyString GetFontNameCN(LOGFONT * pFont);

