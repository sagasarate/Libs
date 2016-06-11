/****************************************************************************/
/*                                                                          */
/*      文件名:    ExceptionParser.cpp                                      */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\exceptionparser.h"
#include <signal.h>


#ifdef UNICODE
#define SymInitializeT			SymInitializeW
#define SymGetLineFromAddr64T	SymGetLineFromAddrW64
#define IMAGEHLP_LINE64T		IMAGEHLP_LINEW64
#define SymLoadModuleExT		SymLoadModuleExW
#else
#define SymInitializeT			SymInitialize
#define SymGetLineFromAddr64T	SymGetLineFromAddr64
#define IMAGEHLP_LINE64T		IMAGEHLP_LINE64
#define SymLoadModuleExT		SymLoadModuleEx
#endif



void DisableSetUnhandledExceptionFilter()

{
#ifdef _M_IX86
	void *addr = (void*)GetProcAddress(LoadLibrary(_T("kernel32.dll")),"SetUnhandledExceptionFilter");

	if (addr) 

	{

		unsigned char code[16];

		int size = 0;

		//xor eax,eax;

		code[size++] = 0x33;

		code[size++] = 0xC0;

		//ret 4

		code[size++] = 0xC2;

		code[size++] = 0x04;

		code[size++] = 0x00;



		DWORD dwOldFlag, dwTempFlag;

		if(VirtualProtect(addr, size, PAGE_READWRITE, &dwOldFlag))
		{
			if(WriteProcessMemory(GetCurrentProcess(), addr, code, size, NULL))
			{
				PrintImportantLog(_T("Exception"), _T("屏蔽SetUnhandledExceptionFilter成功"));
			}
			VirtualProtect(addr, size, dwOldFlag, &dwTempFlag);
		}

		

	}
#endif
}



LONG CExceptionParser::ExceptionHander(LPEXCEPTION_POINTERS pException)
{
	PrintImportantLog(_T("Exception"), _T("收到未处理的异常"));
	CExceptionParser::GetInstance()->ParseException(pException);
	return 0;
}


void CExceptionParser::ExceptionTranslator(UINT Code,LPEXCEPTION_POINTERS pException)
{
	PrintImportantLog(_T("Exception"), _T("捕捉到异常,Code=%u"), Code);
	CExceptionParser::GetInstance()->ParseException(pException);	
}

LONG CExceptionParser::ExceptionPrinter(LPEXCEPTION_POINTERS pException,UINT64 DebugInfo1,LPCTSTR szFunName)
{
	PrintImportantLog(_T("Exception"), _T("开始输出异常,DebugInfo1=0x%llX,在函数%s"), DebugInfo1, szFunName);
	CExceptionParser::GetInstance()->ParseException(pException);
	return EXCEPTION_EXECUTE_HANDLER;
}

CExceptionParser::CExceptionParser(void)
{
	m_hProcess=NULL;
	m_hExceptionLog=INVALID_HANDLE_VALUE;
	m_ExceptionCount=0;
}

CExceptionParser::~CExceptionParser(void)
{
	if(m_hProcess)
		SymCleanup(m_hProcess);
}

void CExceptionParser::Init(UINT Flag)
{
	m_Flag=Flag;

	m_hProcess = GetCurrentProcess();

	if(m_Flag&EXCEPTION_SET_DEFAULT_HANDLER)
	{
		PrintImportantLog(_T("Exception"), _T("启用未处理异常捕捉"));
		SetUnhandledExceptionFilter((LPTOP_LEVEL_EXCEPTION_FILTER)ExceptionHander);

		

		_set_abort_behavior(0, _WRITE_ABORT_MSG | _CALL_REPORTFAULT);
		signal(SIGABRT,SignalHandler);
		_set_invalid_parameter_handler(InvalidParameterHandler);

	}
	if(m_Flag&EXCEPTION_USE_API_HOOK)
	{
		PrintImportantLog(_T("Exception"),_T("屏蔽系统异常捕捉"));
		DisableSetUnhandledExceptionFilter();
	}
	if(m_Flag&EXCEPTION_SET_TRANSLATOR)
	{
		PrintImportantLog(_T("Exception"),_T("启用异常处理过滤"));
		_set_se_translator(ExceptionTranslator);
	}

	if (m_Flag&EXCEPTION_PRE_INIT_SYM)
	{
		PrintImportantLog(_T("Exception"), _T("启用符号预加载"));
		SymInit();
	}

	if (m_Flag&EXCEPTION_MAKE_DUMP)
	{
		PrintImportantLog(_T("Exception"), _T("启用Dump输出"));
	}

	if (m_Flag&EXCEPTION_MAKE_FULL_DUMP)
	{
		PrintImportantLog(_T("Exception"), _T("启用FullDump输出"));
	}

	if (m_Flag&EXCEPTION_LOG_MODULE_SYM_STATUS)
	{
		PrintImportantLog(_T("Exception"), _T("输出模块符号加载状态"));
	}
}

void CExceptionParser::ParseException(LPEXCEPTION_POINTERS pException)
{
	CAutoLock Lock(m_CriticalSection);

	m_hProcess=GetCurrentProcess();	

	PrintImportantLog(_T("Exception"),_T("进行异常处理"));

	PROCESS_MEMORY_COUNTERS MemoryInfo;
	if(GetProcessMemoryInfo(m_hProcess,&MemoryInfo,sizeof(MemoryInfo)))
	{
#ifdef _WIN64
		PrintImportantLog(_T("Exception"),_T("进程内存占用:%llu,虚拟内存占用:%llu"),MemoryInfo.WorkingSetSize,MemoryInfo.PagefileUsage);
#else
		PrintImportantLog(_T("Exception"),_T("进程内存占用:%u,虚拟内存占用:%u"),MemoryInfo.WorkingSetSize,MemoryInfo.PagefileUsage);
#endif		
	}
	else
	{
		PrintImportantLog(_T("Exception"),_T("获取进程内存使用情况失败%d"),GetLastError());
	}

	if(m_ExceptionCount>0)
	{
		PrintImportantLog(_T("Exception"),_T("发生多次异常捕捉%d"),m_ExceptionCount);
		return;
	}
	m_ExceptionCount++;


	
	if((m_Flag&EXCEPTION_PRE_INIT_SYM)==0)
		SymInit();

	if (m_Flag&EXCEPTION_LOG_MODULE_SYM_STATUS)
		EnumModuleSymStatus(m_hProcess);

	TCHAR szModulePath[MAX_PATH];

	GetModuleFileName(NULL,szModulePath,MAX_PATH);	

	TCHAR szExceptionLogFileName[MAX_PATH];
	SYSTEMTIME CurTime;

	GetLocalTime(&CurTime);

	_stprintf_s(szExceptionLogFileName,MAX_PATH,_T("%s.Exception%u-%02u-%02u %02u-%02u-%02u.log"),
		szModulePath,
		CurTime.wYear,CurTime.wMonth,CurTime.wDay,
		CurTime.wHour,CurTime.wMinute,CurTime.wSecond);

	PrintImportantLog(0,_T("开始输出异常Log文件:%s.Log"),
		szExceptionLogFileName);

	m_hExceptionLog=CreateFile(szExceptionLogFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL|FILE_FLAG_WRITE_THROUGH,NULL);
	if(m_hExceptionLog==INVALID_HANDLE_VALUE)
	{
		PrintImportantLog(0,_T("无法创建异常Log文件:%s.Log"),
			szExceptionLogFileName);
	}
	
	
	LogException(_T("-----------------------------------------------------------------"));

	LogException(_T("程序发生异常:类型: 0x%x  地址: 0x%x"), 
		pException->ExceptionRecord->ExceptionCode ,pException->ExceptionRecord->ExceptionAddress);

	switch(pException->ExceptionRecord->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		{
			LogException(_T("地址:0x%x%s"),
				pException->ExceptionRecord->ExceptionInformation[1],
				pException->ExceptionRecord->ExceptionInformation[0]?_T("不可写"):_T("不可读"));
		}
		break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
		LogException(_T("EXCEPTION_DATATYPE_MISALIGNMENT"));
		break;
	case EXCEPTION_BREAKPOINT:
		LogException(_T("EXCEPTION_BREAKPOINT"));
		break;
	case EXCEPTION_SINGLE_STEP:
		LogException(_T("EXCEPTION_SINGLE_STEP"));
		break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		LogException(_T("EXCEPTION_ARRAY_BOUNDS_EXCEEDED"));
		break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
		LogException(_T("EXCEPTION_FLT_DENORMAL_OPERAND"));
		break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
		LogException(_T("EXCEPTION_FLT_DIVIDE_BY_ZERO"));
		break;
	case EXCEPTION_FLT_INEXACT_RESULT:
		LogException(_T("EXCEPTION_FLT_INEXACT_RESULT"));
		break;
	case EXCEPTION_FLT_INVALID_OPERATION:
		LogException(_T("EXCEPTION_FLT_INVALID_OPERATION"));
		break;
	case EXCEPTION_FLT_OVERFLOW:
		LogException(_T("EXCEPTION_FLT_OVERFLOW"));
		break;
	case EXCEPTION_FLT_STACK_CHECK:
		LogException(_T("EXCEPTION_FLT_STACK_CHECK"));
		break;
	case EXCEPTION_FLT_UNDERFLOW:
		LogException(_T("EXCEPTION_FLT_UNDERFLOW"));
		break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		LogException(_T("EXCEPTION_INT_DIVIDE_BY_ZERO"));
		break;
	case EXCEPTION_INT_OVERFLOW:
		LogException(_T("EXCEPTION_INT_OVERFLOW"));
		break;
	case EXCEPTION_PRIV_INSTRUCTION:
		LogException(_T("EXCEPTION_PRIV_INSTRUCTION"));
		break;
	case EXCEPTION_IN_PAGE_ERROR:
		LogException(_T("EXCEPTION_IN_PAGE_ERROR"));
		break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
		LogException(_T("EXCEPTION_ILLEGAL_INSTRUCTION"));
		break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
		LogException(_T("EXCEPTION_NONCONTINUABLE_EXCEPTION"));
		break;
	case EXCEPTION_STACK_OVERFLOW:
		LogException(_T("栈溢出"));
		break;
	case EXCEPTION_INVALID_DISPOSITION:
		LogException(_T("EXCEPTION_INVALID_DISPOSITION"));
		break;
	case EXCEPTION_GUARD_PAGE:
		LogException(_T("EXCEPTION_GUARD_PAGE"));
		break;
	case EXCEPTION_INVALID_HANDLE:
		LogException(_T("EXCEPTION_INVALID_HANDLE"));
		break;	
	}

	static ADDRESS_INFO AddressInfo;

	GetAddressInfo((DWORD64)pException->ExceptionRecord->ExceptionAddress,&AddressInfo);
	LogException(_T("地址描述:函数(%s),文件(%s)(%d)"),AddressInfo.SymbolInfo.Name,AddressInfo.CppFileName,AddressInfo.LineNumber);

	LogException(_T("调用堆栈:"));

	ParseCallStack(pException->ContextRecord);

	LogException(_T("-----------------------------------------------------------------"));

	if(m_Flag&EXCEPTION_MAKE_DUMP)
		WriteDump(pException);
	
}

void CExceptionParser::OnFinishModuleLoad()
{
	if (m_Flag&EXCEPTION_LOG_MODULE_SYM_STATUS)
		EnumModuleSymStatus(m_hProcess);
}

void CExceptionParser::ParseCallStack(PCONTEXT pContextRecord,UINT MaxLoopCount)
{
	if(m_hProcess==NULL)
		return;


	static HINSTANCE LastInstance=NULL;

	static ADDRESS_INFO AddressInfo;

	static STACKFRAME64 StackFrame;

	ZeroMemory(&StackFrame,sizeof(StackFrame));

	DWORD MachineType;

#ifdef _M_IX86
	MachineType=IMAGE_FILE_MACHINE_I386;
	StackFrame.AddrPC.Offset=pContextRecord->Eip;
	StackFrame.AddrPC.Mode=AddrModeFlat;
	StackFrame.AddrStack.Offset=pContextRecord->Esp;
	StackFrame.AddrStack.Mode=AddrModeFlat;
	StackFrame.AddrFrame.Offset=pContextRecord->Ebp;
	StackFrame.AddrFrame.Mode=AddrModeFlat;
#else
	MachineType=IMAGE_FILE_MACHINE_AMD64;
	StackFrame.AddrPC.Offset=pContextRecord->Rip;
	StackFrame.AddrPC.Mode=AddrModeFlat;
	StackFrame.AddrStack.Offset=pContextRecord->Rsp;
	StackFrame.AddrStack.Mode=AddrModeFlat;
	StackFrame.AddrFrame.Offset=pContextRecord->Rbp;
	StackFrame.AddrFrame.Mode=AddrModeFlat;
#endif

	while(MaxLoopCount&&StackWalk64(MachineType,m_hProcess,GetCurrentThread(),
		&StackFrame,pContextRecord,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL))
	{
		if(StackFrame.AddrFrame.Offset==0)
			break;
		BOOL Ret=GetAddressInfo(StackFrame.AddrPC.Offset,&AddressInfo);
		if(LastInstance!=AddressInfo.hInstance)
		{	
			LastInstance=AddressInfo.hInstance;
			LogException(_T("调用模块:[0x%llX]%s"),(UINT64)LastInstance,AddressInfo.ModuleName);
		}
		LogException(_T("调用地址:0x%llX"),(DWORD64)StackFrame.AddrPC.Offset);
		if(Ret)
			LogException(_T("地址描述:函数(%s),文件(%s)(%d)"),AddressInfo.SymbolInfo.Name,AddressInfo.CppFileName,AddressInfo.LineNumber);
		
		
		MaxLoopCount--;
	}

}

BOOL CExceptionParser::GetAddressInfo(DWORD64 Address,ADDRESS_INFO * pAddressInfo)
{
	if(m_hProcess==NULL)
		return false;
	
	pAddressInfo->Clear();

	pAddressInfo->Address=Address;

	//获取地址对应的模块信息
	MEMORY_BASIC_INFORMATION mbi ;
	ZeroMemory(&mbi,sizeof(mbi));
	VirtualQuery((LPCVOID)Address, &mbi, sizeof( mbi ) ) ;			
	HINSTANCE hInstance = (HINSTANCE)mbi.AllocationBase ;

	if(hInstance)
	{	
		if(pAddressInfo->hInstance!=hInstance)
		{
			pAddressInfo->hInstance=hInstance;			
			GetModuleFileName( hInstance, pAddressInfo->ModuleName, MAX_PATH ) ;			
		}			
	}		

	
	
	IMAGEHLP_LINE64T LineInfo;
	ZeroMemory(&LineInfo, sizeof(LineInfo));
	LineInfo.SizeOfStruct = sizeof(LineInfo);

	DWORD LineDisplacement = 0;

	if(SymGetLineFromAddr64T(m_hProcess, Address, &LineDisplacement, &LineInfo))
	{
		PrintImportantLog(0, _T("SymGetLineFromAddr64(0x%X,0x%llX) OK"),
			m_hProcess, Address); 
		if (LineInfo.FileName)
		{
			strncpy_0(pAddressInfo->CppFileName, MAX_PATH, LineInfo.FileName, MAX_PATH);
			pAddressInfo->LineNumber = LineInfo.LineNumber;
		}
	}
	else
	{
		PrintImportantLog(0, _T("SymGetLineFromAddr64(0x%X,0x%llX):%d"),
			m_hProcess, Address, GetLastError());

		DWORD64 Displacement = 0;

		if (SymFromAddr(m_hProcess, Address, &Displacement, &(pAddressInfo->SymbolInfo)))
		{
			PrintImportantLog(0, _T("SymFromAddr(0x%X,0x%llX,%d) OK"),
				m_hProcess, Address, pAddressInfo->SymbolInfo.MaxNameLen);
		}
		else
		{
			PrintImportantLog(0, _T("SymFromAddr(0x%X,0x%llX,%d):%d"),
				m_hProcess, Address, pAddressInfo->SymbolInfo.MaxNameLen, GetLastError());
		}
	}
	return TRUE;
}

BOOL CExceptionParser::WriteDump(LPEXCEPTION_POINTERS pException)
{
	HANDLE hDumpFile;

	TCHAR szModuleFileName[MAX_PATH];
	GetModuleFileName(NULL,szModuleFileName,MAX_PATH);	

	TCHAR szDumpFileName[MAX_PATH];

	SYSTEMTIME CurTime;

	GetLocalTime(&CurTime);

	_stprintf_s(szDumpFileName,MAX_PATH,_T("%s.Dump%u-%02u-%02u %02u-%02u-%02u.dmp"),
		szModuleFileName,
		CurTime.wYear,CurTime.wMonth,CurTime.wDay,
		CurTime.wHour,CurTime.wMinute,CurTime.wSecond);	

	PrintImportantLog(_T("Exception"),_T("开始写入%sDump文件%s"),
		m_Flag&EXCEPTION_MAKE_FULL_DUMP?_T("完整"):_T("最小"),
		szDumpFileName);
	hDumpFile=CreateFile(szDumpFileName,GENERIC_WRITE,0,NULL,CREATE_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
	if(hDumpFile!=INVALID_HANDLE_VALUE)
	{		
		
		static DWORD ProcessID=GetCurrentProcessId();

		static MINIDUMP_EXCEPTION_INFORMATION ExceptionInfo;
		static MINIDUMP_EXCEPTION_INFORMATION * pExceptionInfo=NULL;

		if(pException)
		{
			ExceptionInfo.ThreadId=GetCurrentThreadId();
			ExceptionInfo.ExceptionPointers=pException;
			ExceptionInfo.ClientPointers=true;
			pExceptionInfo=&ExceptionInfo;
		}
		
		PrintImportantLog(_T("Exception"),_T("创建Dump文件成功"));

		if(MiniDumpWriteDump(m_hProcess,ProcessID,hDumpFile,
			m_Flag&EXCEPTION_MAKE_FULL_DUMP?MiniDumpWithFullMemory:MiniDumpNormal,
			pExceptionInfo,NULL,NULL))
		{
			PrintImportantLog(_T("Exception"),_T("写入Dump文件成功%s"),szDumpFileName);
			CloseHandle(hDumpFile);
			return TRUE;
		}
		else
		{
			PrintImportantLog(_T("Exception"),_T("写入Dump文件失败%s"),szDumpFileName);
		}
		CloseHandle(hDumpFile);
		
	}
	else
	{
		PrintImportantLog(_T("Exception"),_T("打开Dump文件失败%s"),szDumpFileName);
	}

	return FALSE;
}


BOOL CExceptionParser::SymInit()
{
	TCHAR szModulePath[MAX_PATH];
	szModulePath[0] = 0;

	PrintImportantLog(_T("Exception"),_T("开始初始化符号"));

	DWORD dwMS, dwLS;

	dwMS = 0;
	dwLS = 0;
	GetInMemoryFileVersion(_T("dbghelp.dll"), szModulePath,dwMS, dwLS);
	PrintImportantLog(_T("Exception"), _T("dbghelp.dll版本%d.%d.%d.%d,路径:%s"),
		HIWORD(dwMS), LOWORD(dwMS), HIWORD(dwLS), LOWORD(dwLS), szModulePath);

	m_hProcess=GetCurrentProcess();	

	SymCleanup(m_hProcess);

	
	
	if(GetModuleFileName(NULL,szModulePath,MAX_PATH))
	{
		TCHAR * pTail = _tcsrchr(szModulePath, '\\');
		if (pTail)
		{
			pTail++;
			*pTail = 0;
		}
	}


	PrintImportantLog(_T("Exception"), _T("搜索符号文件在:%s"), szModulePath);
	if (!SymInitializeT(m_hProcess, szModulePath, TRUE))
	{
		PrintImportantLog(_T("Exception"),_T("无法在当前模块路径找到PDB文件，尝试进行目录搜索"));
		if(!SymInitialize(m_hProcess,NULL,TRUE))
		{
			PrintImportantLog(_T("Exception"),_T("无法到PDB文件"));
			return FALSE;
		}
	}
	PrintImportantLog(_T("Exception"),_T("初始化符号完毕"));
	
	return TRUE;
}



BOOL CExceptionParser::SymLoadFromModule(LPCTSTR szModuleFileName)
{
	PrintImportantLog(_T("Exception"),_T("开始从%s加载符号"),szModuleFileName);
	HMODULE hModule=GetModuleHandle(szModuleFileName);
	if(hModule)
	{
		MODULEINFO ModuleInfo;
		ZeroMemory(&ModuleInfo,sizeof(ModuleInfo));
		if(GetModuleInformation(m_hProcess,hModule,&ModuleInfo,sizeof(ModuleInfo)))
		{
			PrintImportantLog(_T("Exception"),_T("模块起始地址:0x%llX"),(DWORD64)ModuleInfo.lpBaseOfDll);
			if(SymLoadModuleExT(m_hProcess,NULL,szModuleFileName,NULL,(DWORD64)ModuleInfo.lpBaseOfDll,0,NULL,0))
			{
				PrintImportantLog(_T("Exception"),_T("加载符号成功"));
				return TRUE;
			}
			else
			{
				PrintImportantLog(_T("Exception"),_T("加载符号失败%d"),GetLastError());			
			}
		}
		else
		{
			PrintImportantLog(_T("Exception"),_T("获取模块信息失败%d"),GetLastError());
		}
	}
	else
	{
		PrintImportantLog(_T("Exception"),_T("获取模块句柄失败%d"),GetLastError());
	}
	return FALSE;
}

UINT CExceptionParser::GetCallStack(DWORD64 * pAddressBuffer,UINT Depth)
{

	HINSTANCE LastInstance=NULL;
	STACKFRAME64 StackFrame;
	ADDRESS_INFO AddressInfo;

	ZeroMemory(&StackFrame,sizeof(StackFrame));

	
	StackFrame.AddrPC.Mode=AddrModeFlat;	
	StackFrame.AddrStack.Mode=AddrModeFlat;	
	StackFrame.AddrFrame.Mode=AddrModeFlat;

#ifdef _M_IX86
	DWORD REIP=0,RESP=0,REBP=0;
	_asm
	{
		mov RESP,esp;
		mov REBP,ebp;
	}

	StackFrame.AddrPC.Offset=REIP;
	StackFrame.AddrStack.Offset=RESP;
	StackFrame.AddrFrame.Offset=REBP;
#else

#endif
	UINT Count=0;

	CONTEXT Context;

	ZeroMemory(&Context,sizeof(Context));

	while(Depth&&StackWalk64(IMAGE_FILE_MACHINE_I386,GetCurrentProcess(),GetCurrentThread(),
		&StackFrame,&Context,NULL,SymFunctionTableAccess64,SymGetModuleBase64,NULL))
	{
		if(StackFrame.AddrFrame.Offset==0)
			break;
		pAddressBuffer[Count]=StackFrame.AddrPC.Offset;				
		Count++;
		Depth--;
	}
	return Count;
}


void CExceptionParser::LogException(LPCTSTR Format,...)
{
	if(m_hExceptionLog!=INVALID_HANDLE_VALUE)
	{
		va_list	vl;
		va_start(vl,Format);
		TCHAR szBuff[4096];
		_vstprintf_s(szBuff,4096,Format,vl);
		_tcscat_s(szBuff,4096,_T("\r\n"));
		DWORD Len=(DWORD)_tcslen(szBuff);
		WriteFile(m_hExceptionLog,szBuff,Len,&Len,NULL);
		va_end( vl);
	}
}

void CExceptionParser::InvalidParameterHandler(const WCHAR * expression,const WCHAR * function,const WCHAR * file,unsigned int line,uintptr_t pReserved)
{
#ifdef UNICODE
	PrintImportantLog(_T("Exception"),_T("非法的调用参数[%s][%s][%s][%d]"),
		expression,
		function,
		file,
		line);
#else
	char szExpression[1024];
	char szFunction[1024];
	char szFile[MAX_PATH];

	UnicodeToAnsi(expression,wcslen(expression),szExpression,1024);
	UnicodeToAnsi(function,wcslen(function),szFunction,1024);
	UnicodeToAnsi(file,wcslen(file),szFile,1024);

	szExpression[1023]=0;
	szFunction[1023]=0;
	szFile[MAX_PATH-1]=0;

	PrintImportantLog(_T("Exception"),_T("非法的调用参数[%s][%s][%s][%d]"),
		szExpression,
		szFunction,
		szFile,
		line);
#endif
}

void CExceptionParser::SignalHandler(int signal)
{
	PrintImportantLog(_T("Exception"),_T("系统提示%d"),signal);
}

BOOL CExceptionParser::GetInMemoryFileVersion(LPCTSTR szFile, LPTSTR szFileFullName, DWORD & dwMS, DWORD & dwLS)
{
	HMODULE hInstIH = GetModuleHandle(szFile);

	// Get the full filename of the loaded version.
	TCHAR szImageHlp[MAX_PATH];
	szImageHlp[0] = 0;

	if(GetModuleFileName(hInstIH, szImageHlp, MAX_PATH))

	{
		if (szFileFullName)
		{
			_tcscpy_s(szFileFullName, MAX_PATH, szImageHlp);
		}

		dwMS = 0;
		dwLS = 0;

		// Get the version information size.
		DWORD dwVerInfoHandle;
		DWORD dwVerSize;

		dwVerSize = GetFileVersionInfoSize(szImageHlp,
			&dwVerInfoHandle);
		if (0 == dwVerSize)
		{
			return (FALSE);
		}

		// Got the version size, now get the version information.
		LPVOID lpData = (LPVOID)new TCHAR[dwVerSize];
		if (FALSE == GetFileVersionInfo(szImageHlp,
			dwVerInfoHandle,
			dwVerSize,
			lpData))
		{
			delete[] lpData;
			return (FALSE);
		}

		VS_FIXEDFILEINFO * lpVerInfo;
		UINT uiLen;
		BOOL bRet = VerQueryValue(lpData,
			_T("\\"),
			(LPVOID*)&lpVerInfo,
			&uiLen);
		if (TRUE == bRet)
		{
			dwMS = lpVerInfo->dwFileVersionMS;
			dwLS = lpVerInfo->dwFileVersionLS;
		}

		delete[] lpData;

		return (bRet);
	}
	return FALSE;
}

BOOL CALLBACK EnumModulesCallback(LPCSTR   ModuleName, DWORD64 BaseOfDll, PVOID   UserContext)
{

	IMAGEHLP_MODULE64 ModuleInfo;

	ModuleInfo.SizeOfStruct = sizeof(ModuleInfo);

	if (!SymGetModuleInfo64((HANDLE)UserContext, BaseOfDll, &ModuleInfo))
	{
		PrintImportantLog(_T("Exception"), _T("获取模块信息失败%d"), GetLastError());
	}

	PrintImportantLog(_T("Exception"), _T("%08llX %s SymType=%d NumSyms=%d CVSig=0x%X PDBSig=0x%X Age=%d PDBUnmatched=%d Image=%s PDB=%s"),
		BaseOfDll, ModuleName, ModuleInfo.SymType, ModuleInfo.NumSyms, ModuleInfo.CVSig, ModuleInfo.PdbSig, ModuleInfo.PdbAge, 
		ModuleInfo.PdbUnmatched, ModuleInfo.ImageName, ModuleInfo.LoadedPdbName);
	return TRUE;
}

void CExceptionParser::EnumModuleSymStatus(HANDLE hProcess)
{
	PrintImportantLog(_T("Exception"), _T("开始枚举模块符号加载状态"));
	if (!SymEnumerateModules64(hProcess, EnumModulesCallback, hProcess))
	{
		PrintImportantLog(_T("Exception"), _T("枚举模块失败%d"), GetLastError());
	}
	PrintImportantLog(_T("Exception"), _T("枚举模块符号加载状态完毕"));
}