/****************************************************************************/
/*                                                                          */
/*      文件名:    SystemConfig.h                                           */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CSystemConfig :
	public CStaticObject4<CSystemConfig>
{
	struct MALLOC_CONFIG
	{
		bool	bSetTrimThreshold;
		int		TrimThreshold;
		bool	bSetMMapThreshold;
		int		MMapThreshold;
		bool	bSetMMapMax;
		int		MMapMax;

		MALLOC_CONFIG()
		{
			bSetTrimThreshold = false;
			TrimThreshold = 1024 * 128;
			bSetMMapThreshold = false;
			MMapThreshold = 1024 * 128;
			bSetMMapMax = false;
			MMapMax = 1024 * 1024 * 64;
		}
	};
protected:
	UINT			m_MainThreadProcessLimit;
	CIPAddress		m_UDPControlAddress;
	CEasyString		m_ControlPipeName;
	UINT			m_ControlPipeAssembleBufferSize;
	bool			m_LogServerObjectUse;	
	UINT			m_LogLevel;
	UINT			m_ConsoleLogLevel;
	UINT			m_LogCacheSize;
	bool			m_EnableGuardThread;
	UINT			m_GuardThreadKeepAliveTime;
	UINT			m_GuardThreadKeepAliveCount;
	bool			m_PreLoadModuleSym;
	bool			m_MakeFullDump;
	bool			m_LogModuleSymStatus;
	MALLOC_CONFIG	m_MallocConfig;
public:
	CSystemConfig(void);
	~CSystemConfig(void);

	bool LoadConfig(LPCTSTR ConfigFileName);

	UINT GetMainThreadProcessLimit()
	{
		return m_MainThreadProcessLimit;
	}
	CIPAddress& GetUDPControlAddress()
	{
		return m_UDPControlAddress;
	}
	bool IsControlPipeEnable()
	{
		return (!m_ControlPipeName.IsEmpty()) && m_ControlPipeAssembleBufferSize > 0;
	}
	LPCTSTR GetControlPipeName()
	{
		return m_ControlPipeName;
	}
	UINT GetControlPipeAssembleBufferSize()
	{
		return m_ControlPipeAssembleBufferSize;
	}
	bool IsLogServerObjectUse()
	{
		return m_LogServerObjectUse;
	}
	bool IsPreLoadModuleSym()
	{
		return m_PreLoadModuleSym;
	}
	bool IsMakeFullDump()
	{
		return m_MakeFullDump;
	}
	bool IsLogModuleSymStatus()
	{
		return m_LogModuleSymStatus;
	}
	UINT GetLogLevel()
	{
		return m_LogLevel;
	}
	UINT GetConsoleLogLevel()
	{
		return m_ConsoleLogLevel;
	}
	bool GetEnableGuardThread()
	{
		return m_EnableGuardThread;
	}
	UINT GetLogCacheSize()
	{
		return m_LogCacheSize;
	}
	UINT GetGuardThreadKeepAliveTime()
	{
		return m_GuardThreadKeepAliveTime;
	}
	UINT GetGuardThreadKeepAliveCount()
	{
		return m_GuardThreadKeepAliveCount;
	}
	const MALLOC_CONFIG&	GetMallocConfig()
	{
		return m_MallocConfig;
	}
};
