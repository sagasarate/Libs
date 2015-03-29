/****************************************************************************/
/*                                                                          */
/*      文件名:    IBaseServer.h                                            */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class IBaseServer
{
public:	

	virtual CNetServer * GetServer()=0;

	virtual BOOL PrintConsoleLog(int Level,LPCTSTR szLogMsg)=0;

	virtual void ExecCommand(LPCTSTR szCommand)=0;

	virtual BOOL SetServerStatus(WORD StatusID,const CSmartValue& Value)=0;
	virtual CSmartValue GetServerStatus(WORD StatusID)=0;
	virtual CSmartStruct& GetAllServerStatus()=0;
	virtual void QueryShowDown()=0;
	virtual bool IsServerTerminated()=0;
};