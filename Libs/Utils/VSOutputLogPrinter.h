/****************************************************************************/
/*                                                                          */
/*      文件名:    VSOutputLogPrinter.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


class CVSOutputLogPrinter :
	public ILogPrinter
{
protected:
	

	DECLARE_CLASS_INFO_STATIC(CVSOutputLogPrinter);
public:
	CVSOutputLogPrinter(void);
	virtual ~CVSOutputLogPrinter(void);

	virtual void PrintLogDirect(int Level, DWORD Color, LPCTSTR Msg);
	virtual void PrintLogVL(int Level,DWORD Color,LPCTSTR Format,va_list vl);
};
