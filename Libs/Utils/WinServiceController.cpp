/****************************************************************************/
/*                                                                          */
/*      文件名:    WinServiceController.cpp                                 */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

IMPLEMENT_CLASS_INFO_STATIC(CWinServiceController,CNameObject)

CWinServiceController::CWinServiceController(void)
{
	m_hSC=NULL;
	m_hService=NULL;
}

CWinServiceController::~CWinServiceController(void)
{
	CloseService();
}

bool CWinServiceController::OpenService(LPCTSTR szServiceName)
{
	CloseService();

	m_hSC=OpenSCManager(NULL,NULL,SC_MANAGER_ALL_ACCESS);
	if(m_hSC==NULL)
		return false;

	m_ServiceName=szServiceName;

	m_hService=::OpenService(m_hSC,m_ServiceName,SC_MANAGER_ALL_ACCESS);
	if(m_hService==NULL)
		return false;

	return true;
}

void CWinServiceController::CloseService()
{
	m_ServiceName.Clear();
	if(m_hService)
	{
		CloseServiceHandle(m_hService);
		m_hService=NULL;
	}
	if(m_hSC)
	{
		CloseServiceHandle(m_hSC);
		m_hSC=NULL;
	}
}

bool CWinServiceController::StartupService(LPCTSTR * pStartArgs,UINT StartArgCount,UINT WaitTimeOut)
{
	if(m_hService)
	{
		if(GetServiceStatus()==SS_RUNNING)
			return true;
		if(::StartService(m_hService,StartArgCount,pStartArgs))
		{
			if(WaitTimeOut)
			{
				return WaitForStatus(SS_RUNNING,WaitTimeOut);
			}
			return true;
		}
	}
	return false;
}
bool CWinServiceController::ShutdownService(UINT WaitTimeOut)
{
	if(m_hService)
	{
		if(GetServiceStatus()==SERVICE_STOPPED)
			return true;
		SERVICE_STATUS ServiceInfo;
		if(::ControlService(m_hService,SERVICE_CONTROL_STOP,&ServiceInfo))
		{
			if(WaitTimeOut)
			{
				return WaitForStatus(SERVICE_STOPPED,WaitTimeOut);
			}
			return true;
		}
	}
	return false;
}
bool CWinServiceController::PauseService(UINT WaitTimeOut)
{
	if(m_hService)
	{
		if(GetServiceStatus()==SS_PAUSED)
			return true;
		if(GetServiceStatus()!=SS_RUNNING)
			return false;
		SERVICE_STATUS ServiceInfo;
		if(::ControlService(m_hService,SERVICE_CONTROL_PAUSE,&ServiceInfo))
		{
			if(WaitTimeOut)
			{
				return WaitForStatus(SS_PAUSED,WaitTimeOut);
			}
			return true;
		}
	}
	return false;
}
bool CWinServiceController::ResumeService(UINT WaitTimeOut)
{
	if(m_hService)
	{
		if(GetServiceStatus()==SS_RUNNING)
			return true;
		if(GetServiceStatus()!=SS_PAUSED)
			return false;
		SERVICE_STATUS ServiceInfo;
		if(::ControlService(m_hService,SERVICE_CONTROL_CONTINUE,&ServiceInfo))
		{
			if(WaitTimeOut)
			{
				return WaitForStatus(SS_RUNNING,WaitTimeOut);
			}
			return true;
		}
	}
	return false;
}

bool CWinServiceController::WaitForStatus(int Status,UINT WaitTimeOut)
{
	CEasyTimer WaitTimer;
	WaitTimer.SetTimeOut(WaitTimeOut);
	do 
	{
		int CurStatus=GetServiceStatus();
		if(CurStatus==SS_NONE)
			return false;
		if(CurStatus==Status)
			return true;
		Sleep(100);
	} while(!WaitTimer.IsTimeOut());
	return false;
}

int CWinServiceController::GetServiceStatus()
{
	if(m_hService)
	{
		SERVICE_STATUS_PROCESS ServiceInfo;
		DWORD InfoSize;
		if(QueryServiceStatusEx(m_hService,SC_STATUS_PROCESS_INFO,(BYTE *)&ServiceInfo,sizeof(ServiceInfo),&InfoSize))
		{
			return ServiceInfo.dwCurrentState;
		}
	}
	return SS_NONE;
}

bool CWinServiceController::GetServiceImageFilePath(LPTSTR pBuffer,int BufferSize)
{
	if(m_hService)
	{		
		DWORD InfoSize;
		if(!QueryServiceConfig(m_hService,NULL,0,&InfoSize))
		{
			DWORD dwError = GetLastError();
			if( ERROR_INSUFFICIENT_BUFFER == dwError )
			{
				BYTE * pInfoBuffer=new BYTE[InfoSize];
				QUERY_SERVICE_CONFIG * pConfigInfo=(QUERY_SERVICE_CONFIG *)pInfoBuffer;
				if(QueryServiceConfig(m_hService,pConfigInfo,InfoSize,&InfoSize))
				{
					strncpy_0(pBuffer,BufferSize,pConfigInfo->lpBinaryPathName,BufferSize);
					delete[] pInfoBuffer;
					return true;
				}
				delete[] pInfoBuffer;
			}				
		}
	}
	return false;
}