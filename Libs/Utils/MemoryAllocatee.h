/****************************************************************************/
/*                                                                          */
/*      文件名:    MemoryAllocatee.h                                        */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

//#include "MemAllocInfoMap.h"
//#include "MemAllocModualMap.h"


extern TCHAR MEMORY_ALLOCATEE[];

#define PTR_HASH_SIZE	128

class CMemoryAllocatee:
	public CStaticObject2<CMemoryAllocatee, MEMORY_ALLOCATEE>
{
protected:
	struct ALLOC_INFO
	{
		size_t			AllocSize;
		LPCTSTR			FileName;
		int				Line;
		LPCTSTR			ModuleName;
		UINT			AllocCount;
	};

	struct MODULE_ALLOC_INFO
	{
		LPCTSTR			ModuleName;
		size_t			AllocSize;
		int				AllocCount;

		static int CompareWithAllocSize(const void * p1, const void * p2)
		{
			const MODULE_ALLOC_INFO * pInfo1 = *((const MODULE_ALLOC_INFO **)p1);
			const MODULE_ALLOC_INFO * pInfo2 = *((const MODULE_ALLOC_INFO **)p2);

			if (pInfo1->AllocSize < pInfo2->AllocSize)
				return 1;
			else if (pInfo1->AllocSize > pInfo2->AllocSize)
				return -1;
			else
				return 0;
		}
	};

	struct MODULE_NAME
	{
		LPCTSTR	Name;
		MODULE_NAME()
		{
			Name = _T("");
		}
		MODULE_NAME(LPCTSTR szName)
		{
			Name = szName;
		}
		bool operator<(const MODULE_NAME& ModuleName)
		{
			return _tcscmp(Name, ModuleName.Name) < 0;
		}
		bool operator>(const MODULE_NAME& ModuleName)
		{
			return _tcscmp(Name, ModuleName.Name) > 0;
		}
	};


	CStaticMapLite<void *, ALLOC_INFO>				m_AllocInfos[PTR_HASH_SIZE];
	CStaticMapLite<MODULE_NAME, MODULE_ALLOC_INFO>	m_AllocModules;
	volatile size_t									m_TotalAllocSize;
	volatile UINT									m_AllocCount;
	CEasyCriticalSection							m_AllocInfoLock[PTR_HASH_SIZE];
	CEasyCriticalSection							m_AllocModuleLock;
	TCHAR											m_LogDir[MAX_PATH];
	static volatile bool							m_Enable;
public:
	CMemoryAllocatee(void);
	~CMemoryAllocatee(void);

	void Enable(bool BeEnable)
	{
		CMemoryAllocatee::m_Enable = BeEnable;
	}
	void SetLogDir(LPCTSTR szDir)
	{
		_tcscpy_s(m_LogDir, MAX_PATH, szDir);
	}

	void AddAllocRecord(void * pMemory, size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName);
	void AddFreeRecord(void * pMemory);
	void ReportLeakStat();
	void ReportAllocStat(UINT LogChannel);
};


template<typename Type>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName)
{
	Type * pMemory = new Type();
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type,typename P1>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName,P1 p1)
{
	Type * pMemory = new Type(p1);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2)
{
	Type * pMemory = new Type(p1, p2);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3)
{
	Type * pMemory = new Type(p1, p2, p3);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4)
{
	Type * pMemory = new Type(p1, p2, p3, p4);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5)
{
	Type * pMemory = new Type(p1,p2,p3,p4,p5);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6)
{
	Type * pMemory = new Type(p1, p2, p3, p4, p5, p6);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7)
{
	Type * pMemory = new Type(p1, p2, p3, p4, p5, p6, p7);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8)
{
	Type * pMemory = new Type(p1, p2, p3, p4, p5, p6, p7, p8);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9)
{
	Type * pMemory = new Type(p1, p2, p3, p4, p5, p6, p7, p8, p9);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2, typename P3, typename P4, typename P5, typename P6, typename P7, typename P8, typename P9, typename P10>
inline Type * MonitoredNew(LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2, P3 p3, P4 p4, P5 p5, P6 p6, P7 p7, P8 p8, P9 p9, P10 p10)
{
	Type * pMemory = new Type(p1, p2, p3, p4, p5, p6, p7, p8, p9, p10);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type), FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type>
inline void MonitoredDelete(Type * pMemory)
{
	CMemoryAllocatee::GetInstance()->AddFreeRecord(pMemory);
	delete pMemory;
}

template<typename Type>
inline Type * MonitoredNewArray(size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName)
{
	Type * pMemory = new Type[Size]();
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type)*Size, FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1>
inline Type * MonitoredNewArray(size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1)
{
	Type * pMemory = new Type[Size](p1);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type)*Size, FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type, typename P1, typename P2>
inline Type * MonitoredNewArray(size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName, P1 p1, P2 p2)
{
	Type * pMemory = new Type[Size](p1, p2);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, sizeof(Type)*Size, FileName, Line, ModuleName);
	}

	return (Type *)pMemory;
}

template<typename Type>
inline void MonitoredDeleteArray(Type * pMemory)
{
	CMemoryAllocatee::GetInstance()->AddFreeRecord(pMemory);
	delete[] pMemory;
}

template<typename Type>
inline Type * MonitoredMalloc(size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName)
{
	void * pMemory = ::malloc(Size);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, Size, FileName, Line, ModuleName);
	}

	return pMemory;
}

template<typename Type>
inline Type * MonitoredRealloc(Type* pMemory, size_t Size, LPCTSTR FileName, int Line, LPCTSTR ModuleName)
{
	CMemoryAllocatee::GetInstance()->AddFreeRecord(pMemory);
	pMemory = ::realloc(pMemory, Size);
	if (pMemory)
	{
		CMemoryAllocatee::GetInstance()->AddAllocRecord(pMemory, Size, FileName, Line, ModuleName);
	}

	return pMemory;
}

template<typename Type>
inline void MonitoredFree(Type * pMemory)
{
	CMemoryAllocatee::GetInstance()->AddFreeRecord(pMemory);
	::free(pMemory);
}