#pragma once

class CAsyncLogWorkThread :
	public CEasyThread
{
protected:
	CEasyArray<ILogPrinter *>	m_LogPrinterList;
	CEasyCriticalSection		m_EasyCriticalSection;
public:
	CAsyncLogWorkThread();
	~CAsyncLogWorkThread();

	virtual BOOL OnRun();

	bool AddLogPrinter(ILogPrinter * pLogPrinter);
	bool RemoveLogPrinter(ILogPrinter * pLogPrinter);
	UINT GetLogPrinterCount()
	{
		return (UINT)m_LogPrinterList.GetCount();
	}
};

