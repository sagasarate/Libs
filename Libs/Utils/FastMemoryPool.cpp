/****************************************************************************/
/*                                                                          */
/*      文件名:    FastMemoryPool.cpp                                       */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "stdafx.h"

IMPLEMENT_CLASS_INFO_STATIC(CFastMemoryPool,CNameObject);

CFastMemoryPool::CFastMemoryPool(void):CNameObject()
{
	m_pBlockLevels=NULL;
	m_BlockLevelCount=0;
	m_BlockSize=0;
	m_LevelSize=0;
	m_BufferSize=0;
	m_pBuffer=NULL;
	m_AllocCount=0;
	m_FreeCount=0;
	m_SystemAllocCount=0;
	m_SystemFreeCount=0;
}

CFastMemoryPool::~CFastMemoryPool(void)
{
	Destory();
}

BOOL CFastMemoryPool::Create(UINT BlockSize,UINT LevelSize,UINT MaxLevel,bool IsThreadLock)
{
	Destory();

	m_BlockSize=BlockSize;
	m_LevelSize=LevelSize;
	m_IsThreadLock=IsThreadLock;

	UINT NodeAditionSize=sizeof(BlockNode)+sizeof(UINT);

	//计算最多可划分多少个级别
	UINT Block=BlockSize;
	m_BlockLevelCount=0;
	while(Block+NodeAditionSize<LevelSize&&MaxLevel)
	{
		m_BlockLevelCount++;
		Block=Block<<1;
		MaxLevel--;
	}

	m_BufferSize=m_LevelSize*m_BlockLevelCount;
	m_pBuffer=new char[m_BufferSize];
	m_pBlockLevels=new BlockList[m_BlockLevelCount];

	for(UINT i=0;i<m_BlockLevelCount;i++)
	{
		char * pLevelBuffer=m_pBuffer+i*m_LevelSize;
		m_pBlockLevels[i].AvailableSize=m_BlockSize<<i;
		m_pBlockLevels[i].BlockSize=m_pBlockLevels[i].AvailableSize+NodeAditionSize;
		m_pBlockLevels[i].BlockCount=m_LevelSize/m_pBlockLevels[i].BlockSize;
		m_pBlockLevels[i].UsedCount=0;
		m_pBlockLevels[i].pBlocks=(BlockNode *)pLevelBuffer;
		m_pBlockLevels[i].pFreeList=m_pBlockLevels[i].pBlocks;

		for(UINT j=0;j<m_pBlockLevels[i].BlockCount;j++)
		{
			BlockNode * pBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+j*m_pBlockLevels[i].BlockSize);
			BlockNode * pPrevBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+(j-1)*m_pBlockLevels[i].BlockSize);
			BlockNode * pNextBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+(j+1)*m_pBlockLevels[i].BlockSize);

			pBlocks->pBlockList=m_pBlockLevels+i;
			pBlocks->AllocSize=0;
			pBlocks->Flag=BF_FREE;
#ifdef LOG_MEM_CALL_STACK
			pBlocks->RecentCallInfo=0;
			ZeroMemory(pBlocks->CallInfo,sizeof(pBlocks->CallInfo));
#endif
			if(j==0)
			{
				pBlocks->pPrev=NULL;
			}
			else
			{
				pBlocks->pPrev=pPrevBlocks;
			}
			if(j==m_pBlockLevels[i].BlockCount-1)
			{
				pBlocks->pNext=NULL;
			}
			else
			{
				pBlocks->pNext=pNextBlocks;
			}

		}
	}
	return TRUE;
}

void CFastMemoryPool::Destory()
{
#ifdef _DEBUG
	for(UINT i=0;i<m_BlockLevelCount;i++)
	{
		if(m_pBlockLevels[i].UsedCount)
			PrintImportantLog(_T("还有%d个内存块未释放！"),m_pBlockLevels[i].UsedCount);
	}
#endif
	SAFE_DELETE_ARRAY(m_pBlockLevels);
	SAFE_DELETE_ARRAY(m_pBuffer);
	m_BlockLevelCount=0;
	m_BlockSize=0;
	m_LevelSize=0;
	m_BufferSize=0;
}

void CFastMemoryPool::Clear()
{
	for(UINT i=0;i<m_BlockLevelCount;i++)
	{
		m_pBlockLevels[i].UsedCount=0;
		m_pBlockLevels[i].pFreeList=m_pBlockLevels[i].pBlocks;

		for(UINT j=0;j<m_pBlockLevels[i].BlockCount;j++)
		{
			BlockNode * pBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+j*m_pBlockLevels[i].BlockSize);
			BlockNode * pPrevBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+(j-1)*m_pBlockLevels[i].BlockSize);
			BlockNode * pNextBlocks=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+(j+1)*m_pBlockLevels[i].BlockSize);

			pBlocks->Flag=BF_FREE;

			if(j==0)
			{
				pBlocks->pPrev=NULL;
			}
			else
			{
				pBlocks->pPrev=pPrevBlocks;
			}
			if(j==m_pBlockLevels[i].BlockCount-1)
			{
				pBlocks->pNext=NULL;
			}
			else
			{
				pBlocks->pNext=pNextBlocks;
			}
		}
	}
}

void CFastMemoryPool::Verfy(int LogChannel)
{
	CAutoLockEx ThreadLock;
	if(m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0, _T("开始内存池校验：PoolAlloc=%u/%u,SystemAlloc=%u/%u"), 
		m_AllocCount, m_FreeCount, m_SystemAllocCount, m_SystemFreeCount);
	for(UINT i=0;i<m_BlockLevelCount;i++)
	{
		for(UINT j=0;j<m_pBlockLevels[i].BlockCount;j++)
		{
			BlockNode * pNode=(BlockNode *)((char *)m_pBlockLevels[i].pBlocks+j*m_pBlockLevels[i].BlockSize);
			BYTE * pData = (BYTE *)pNode + sizeof(BlockNode);

			if(pNode->Flag==BF_USED)
			{
				if(*((UINT *)(((BYTE *)pNode)+sizeof(BlockNode)+pNode->AllocSize))!=BF_TAIL)
				{
					PrintImportantLog(_T("内存块%p尾部已被破坏"), pNode);
					assert(false);
				}
			}
			else if(pNode->Flag!=BF_FREE)
			{
				PrintImportantLog(_T("内存块%p头部已被破坏"), pNode);
				assert(false);

			}
			if(pNode->Flag==BF_USED)
			{
				CLogManager::GetInstance()->PrintLog(LogChannel,ILogPrinter::LOG_LEVEL_NORMAL,0,
					_T("AllocedMem:%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,")
					_T("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X"),
					pData[0],pData[1],pData[2],pData[3],pData[4],pData[5],pData[6],pData[7],
					pData[8],pData[9],pData[10],pData[11],pData[12],pData[13],pData[14],pData[15],
					pData[16],pData[17],pData[18],pData[19],pData[20],pData[21],pData[22],pData[23],
					pData[24],pData[25],pData[26],pData[27],pData[28],pData[29],pData[30],pData[31]);
			}

		}
	}
	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0, _T("内存池校验结束"));
}

LPVOID CFastMemoryPool::Alloc(UINT Size)
{
	if(m_pBuffer&&Size)
	{
		UINT Level=0;
		for(;Level<m_BlockLevelCount;Level++)
		{
			if(m_pBlockLevels[Level].AvailableSize>=Size)
				break;
		}
		if(Level<m_BlockLevelCount)
		{
			LPVOID pMem=AllocBlock(m_pBlockLevels+Level,Size);
			if(pMem)
			{
				AtomicInc(&m_AllocCount);
				return pMem;
			}
		}

		AtomicInc(&m_SystemAllocCount);
		return new char[Size];
	}
	else
		return NULL;
}

BOOL CFastMemoryPool::Free(LPVOID pMem)
{
	if(m_pBuffer)
	{
		if((UINT)((char *)pMem-m_pBuffer)<m_BufferSize)
		{
			AtomicInc(&m_FreeCount);
			return FreeBlock((BlockNode *)((char *)pMem-sizeof(BlockNode)));
		}
		AtomicInc(&m_SystemFreeCount);
		delete[] (char *)pMem;
		return TRUE;
	}
	else
		return FALSE;
}

#ifdef LOG_MEM_CALL_STACK
void CFastMemoryPool::LogMemUse(LPVOID pMem,int RefCount)
{
	if(m_pBuffer)
	{
		if((UINT)((char *)pMem-m_pBuffer)<m_BufferSize)
		{
			BlockNode * pNode=(BlockNode *)((char *)pMem-sizeof(BlockNode));

			pNode->CallInfo[pNode->RecentCallInfo].StackDepth=CExceptionParser::GetInstance()->GetCallStack((DWORD64 *)pNode->CallInfo[pNode->RecentCallInfo].CallStack,MAX_CALL_STACK);
			pNode->CallInfo[pNode->RecentCallInfo].RefCount=RefCount;
			pNode->RecentCallInfo++;
			if(pNode->RecentCallInfo>=MAX_CALL_INFO)
				pNode->RecentCallInfo=0;

		}
	}
}
#endif

LPVOID CFastMemoryPool::AllocBlock(BlockList * pBlockList,UINT AllocSize)
{
	CAutoLockEx ThreadLock;
	if(m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	if(pBlockList)
	{
		if(pBlockList->pFreeList)
		{
			volatile BlockNode * pNode;

			pNode=pBlockList->pFreeList;
			pBlockList->pFreeList=pNode->pNext;
			if(pBlockList->pFreeList)
				pBlockList->pFreeList->pPrev=NULL;
			pNode->Flag=BF_USED;
			pNode->AllocSize=AllocSize;
			*((UINT *)(((BYTE *)pNode)+sizeof(BlockNode)+pNode->AllocSize))=BF_TAIL;
			pBlockList->UsedCount++;
//#ifdef _DEBUG
//			memset((BYTE *)pNode + sizeof(BlockNode), 0xCD, pNode->AllocSize);
//#endif

			return (char *)pNode+sizeof(BlockNode);
		}
	}
	return NULL;
}

BOOL CFastMemoryPool::FreeBlock(BlockNode * pNode)
{
	CAutoLockEx ThreadLock;
	if(m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	if(pNode)
	{
		if(pNode->Flag!=BF_USED)
		{
			PrintImportantLog(_T("内存块头部已被破坏[0x%X],大小%u"), pNode->Flag, pNode->AllocSize);
			DumpBlock(pNode);
#ifdef LOG_MEM_CALL_STACK
			PrintCallStackLog(pNode);
#endif
			assert(false);

		}
		if(*((UINT *)(((BYTE *)pNode)+sizeof(BlockNode)+pNode->AllocSize))!=BF_TAIL)
		{
			PrintImportantLog(_T("内存块尾部已被破坏,大小%u"), pNode->AllocSize);
			DumpBlock(pNode);
#ifdef LOG_MEM_CALL_STACK
			PrintCallStackLog(pNode);
#endif
			assert(false);
		}
		BlockList * pBlockList=pNode->pBlockList;

		pNode->AllocSize=0;
		pNode->Flag=BF_FREE;
		pNode->pPrev=NULL;
		pNode->pNext=pBlockList->pFreeList;
		if(pBlockList->pFreeList)
			pBlockList->pFreeList->pPrev=pNode;
		pBlockList->pFreeList=pNode;
		pBlockList->UsedCount--;

//#ifdef _DEBUG
//		memset((BYTE *)pNode + sizeof(BlockNode), 0xFE, pBlockList->AvailableSize);
//#endif
		return TRUE;

	}
	return FALSE;
}

void CFastMemoryPool::DumpBlock(BlockNode * pNode)
{
	BlockList * pBlockList = pNode->pBlockList;
	UINT BlockSize = pBlockList->BlockSize;
	BYTE * pBlockData = (BYTE *)pNode;
	for (UINT i = 0; i < BlockSize; i += 32)
	{
		PrintImportantLog(_T("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,")
			_T("%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X,%02X"),
			pBlockData[i], pBlockData[i + 1], pBlockData[i + 2], pBlockData[i + 3],
			pBlockData[i + 4], pBlockData[i + 5], pBlockData[i + 6], pBlockData[i + 7],
			pBlockData[i + 8], pBlockData[i + 9], pBlockData[i + 10], pBlockData[i + 11],
			pBlockData[i + 12], pBlockData[i + 13], pBlockData[i + 14], pBlockData[i + 15],
			pBlockData[i + 16], pBlockData[i + 17], pBlockData[i + 18], pBlockData[i + 19],
			pBlockData[i + 20], pBlockData[i + 21], pBlockData[i + 22], pBlockData[i + 23],
			pBlockData[i + 24], pBlockData[i + 25], pBlockData[i + 26], pBlockData[i + 27],
			pBlockData[i + 28], pBlockData[i + 29], pBlockData[i + 30], pBlockData[i + 31]);
	}
}

#ifdef LOG_MEM_CALL_STACK
void CFastMemoryPool::PrintCallStackLog(BlockNode * pNode)
{
	PrintImportantLog(_T("当前记录号:%u"),pNode->RecentCallInfo);
	CExceptionParser::ADDRESS_INFO AddressInfo;
	CExceptionParser::GetInstance()->SymInit();
	for(UINT j=0;j<MAX_CALL_INFO;j++)
	{
		PrintImportantLog(_T("记录号"),j);
		for(UINT i=0;i<pNode->CallInfo[j].StackDepth;i++)
		{
			CExceptionParser::GetInstance()->GetAddressInfo(pNode->CallInfo[j].CallStack[i],&AddressInfo);
			PrintImportantLog(_T("%ll08X,%s:%d"),AddressInfo.Address,AddressInfo.CppFileName,AddressInfo.LineNumber);
		}
	}
}
#endif


void CFastMemoryPool::DoStat()
{

}
