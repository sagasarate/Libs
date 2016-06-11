/****************************************************************************/
/*                                                                          */
/*      文件名:    ExceptionParser.h                                        */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


#define MAX_SYMBOL_NAME		1024

enum EXCEPTION_HANDLE_MODE
{
	EXCEPTION_SET_DEFAULT_HANDLER=1,
	EXCEPTION_SET_TRANSLATOR=(1<<1),
	EXCEPTION_USE_API_HOOK=(1<<2),
	EXCEPTION_MAKE_DUMP=(1<<3),
	EXCEPTION_MAKE_FULL_DUMP=(1<<4),
	EXCEPTION_PRE_INIT_SYM=(1<<5),
	EXCEPTION_LOG_MODULE_SYM_STATUS = (1 << 6),
};

class CExceptionParser :
	public CStaticObject<CExceptionParser>
{
public:
	struct SYMBOL_INFO_EX:public SYMBOL_INFO
	{
		char NameBuffer[MAX_SYMBOL_NAME];
		SYMBOL_INFO_EX()
		{
			ZeroMemory(this,sizeof(*this));
			SizeOfStruct=sizeof(SYMBOL_INFO);
			MaxNameLen=MAX_SYMBOL_NAME;
		}
	};

	struct ADDRESS_INFO
	{
		DWORD64			Address;
		HINSTANCE		hInstance;
		TCHAR			ModuleName[MAX_PATH];
		SYMBOL_INFO_EX	SymbolInfo;
		TCHAR			CppFileName[MAX_PATH];
		int				LineNumber;
		ADDRESS_INFO()
		{
			Clear();
		}
		void Clear()
		{
			Address = 0;
			hInstance = NULL;
			ModuleName[0] = 0;
			CppFileName[0] = 0;
			LineNumber = -1;
		}
	};
protected:
	CEasyCriticalSection	m_CriticalSection;
	HANDLE					m_hProcess;
	int						m_ExceptionCount;
	HANDLE					m_hExceptionLog;
	UINT					m_Flag;
public:
	CExceptionParser(void);
	~CExceptionParser(void);

	void Init(UINT Flag);

	void ParseException(LPEXCEPTION_POINTERS pException);

	void OnFinishModuleLoad();

	static LONG ExceptionHander(LPEXCEPTION_POINTERS pException);
	static void ExceptionTranslator(UINT Code,LPEXCEPTION_POINTERS pException);
	static LONG ExceptionPrinter(LPEXCEPTION_POINTERS pException,UINT64 DebugInfo1,LPCTSTR szFunName);

	BOOL WriteDump(LPEXCEPTION_POINTERS pException);

	BOOL SymInit();
	BOOL SymLoadFromModule(LPCTSTR szModuleFileName);
	UINT GetCallStack(DWORD64 * pAddressBuffer,UINT Depth);

	BOOL GetAddressInfo(DWORD64 Address,ADDRESS_INFO * pAddressInfo);

protected:

	void ParseCallStack(PCONTEXT pContextRecord,UINT MaxLoopCount=16);	

	void LogException(LPCTSTR Format,...);


	static void InvalidParameterHandler(
		const WCHAR * expression,
		const WCHAR * function, 
		const WCHAR * file, 
		unsigned int line,
		uintptr_t pReserved
		);

	static void SignalHandler(int signal);	

	static BOOL GetInMemoryFileVersion(LPCTSTR szFile, LPTSTR szFileFullName, DWORD & dwMS, DWORD & dwLS);

	static void EnumModuleSymStatus(HANDLE hProcess);
	
};


inline void LogCallStack(int LogChannel)
{
	DWORD64 AddressBuff[10];
	ZeroMemory(AddressBuff,sizeof(AddressBuff));
	CExceptionParser::GetInstance()->GetCallStack(AddressBuff,10);
	for(int i=0;i<10;i++)
	{
		CExceptionParser::ADDRESS_INFO AddressInfo;
		if(CExceptionParser::GetInstance()->GetAddressInfo(AddressBuff[i],&AddressInfo))
		{
			CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,_T("%ll08X,%s:%d"),AddressInfo.Address,AddressInfo.CppFileName,AddressInfo.LineNumber);
		}
	}
}

#define ASSERT_AND_LOG(Express) if(!(Express)){LogCallStack(LOG_IMPORTANT_CHANNEL);assert(false);}
