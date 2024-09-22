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

	virtual bool OnRun();

	virtual bool AddLogPrinter(ILogPrinter * pLogPrinter);
	virtual bool RemoveLogPrinter(ILogPrinter * pLogPrinter);
	UINT GetLogPrinterCount()
	{
		return (UINT)m_LogPrinterList.GetCount();
	}
};

