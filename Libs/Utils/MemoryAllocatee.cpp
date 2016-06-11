/****************************************************************************/
/*                                                                          */
/*      文件名:    MemoryAllocatee.cpp                                      */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"



CMemoryAllocatee::CMemoryAllocatee(void)
{
	m_IsWorking=true;
}

CMemoryAllocatee::~CMemoryAllocatee(void)
{
	m_IsWorking=false;


	if(m_AllocInfos.GetObjectCount())
	{
		void * Pos=m_AllocInfos.GetFirstObjectPos();
		while(Pos)
		{
			void * Key;
			ALLOC_INFO * pInfo=m_AllocInfos.GetNextObject(Pos,Key);

			if(pInfo->FileName)
			{
				PrintImportantLog(0,_T("Memory Leak Size=%u,Position=%s:%d"),
					pInfo->AllocSize,pInfo->FileName,pInfo->Line);
			}
			else
			{
				PrintImportantLog(0,_T("Memory Leak Size=%u"),
					pInfo->AllocSize);
			}
		}
	}
}

void * CMemoryAllocatee::Alloc(int Size,LPCTSTR FileName,int Line)
{
	void * pMemBlock=::malloc(Size);
	if(m_IsWorking)
	{
		ALLOC_INFO& AllocInfo=m_AllocInfos[pMemBlock];
		AllocInfo.AllocSize=Size;
		AllocInfo.FileName=FileName;
		AllocInfo.Line=Line;
	}
	return pMemBlock;
}

void * CMemoryAllocatee::Realloc(void * pBlock,int Size,LPCTSTR FileName,int Line)
{
	if(m_IsWorking)
	{
		m_AllocInfos.Delete(pBlock);
	}
	void * pMemBlock=::realloc(pBlock,Size);
	if(m_IsWorking)
	{
		ALLOC_INFO& AllocInfo=m_AllocInfos[pMemBlock];
		AllocInfo.AllocSize=Size;
		AllocInfo.FileName=FileName;
		AllocInfo.Line=Line;
	}
	return pMemBlock;
}

void CMemoryAllocatee::Free(void * pBlock)
{
	if(m_IsWorking)
	{
		m_AllocInfos.Delete(pBlock);
	}
	free(pBlock);
}

CMemoryAllocatee * CMemoryAllocatee::GetInstance()
{
	return &m_MemoryAllocatee;
}


#pragma init_seg(lib)

bool CMemoryAllocatee::m_IsWorking=false;
CMemoryAllocatee CMemoryAllocatee::m_MemoryAllocatee;
