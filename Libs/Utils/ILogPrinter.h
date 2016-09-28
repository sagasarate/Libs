/****************************************************************************/
/*                                                                          */
/*      文件名:    ILogPrinter.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class ILogPrinter:public CNameObject
{
public:
	enum LOG_LEVEL
	{
		LOG_LEVEL_NORMAL=(1),
		LOG_LEVEL_DEBUG=(1<<1),
	};
	ILogPrinter(void):CNameObject(){};
	virtual ~ILogPrinter(void){};
	virtual void PrintLogDirect(int Level, LPCTSTR Tag, LPCTSTR Msg) = 0;
	virtual void PrintLogVL(int Level, LPCTSTR Tag, LPCTSTR Format, va_list vl) = 0;
	virtual bool NeedAsyncUpdate()
	{
		return false;
	}
	virtual int Update()
	{
		return 0;
	}
};