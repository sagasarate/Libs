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

static void DumpBinData(const BYTE * pData, UINT DataLen, UINT MaxDumpSize)
{
	PrintImportantLog(_T("DumpDataLen=%u"), DataLen);
	if (DataLen > MaxDumpSize)
		DataLen = MaxDumpSize;
	for (UINT i = 0; i < DataLen; i += 8)
	{
		PrintImportantLog(_T("%02X %02X %02X %02X %02X %02X %02X %02X"),
			((i < DataLen) ? pData[i] : 0),
			((i + 1 < DataLen) ? pData[i + 1] : 0),
			((i + 2 < DataLen) ? pData[i + 2] : 0),
			((i + 3 < DataLen) ? pData[i + 3] : 0),
			((i + 4 < DataLen) ? pData[i + 4] : 0),
			((i + 5 < DataLen) ? pData[i + 5] : 0),
			((i + 6 < DataLen) ? pData[i + 6] : 0),
			((i + 7 < DataLen) ? pData[i + 7] : 0)
		);
	}
}

IMPLEMENT_CLASS_INFO_STATIC(CFastMemoryPool, CNameObject)

CFastMemoryPool::CFastMemoryPool(void) :CNameObject()
{
	m_pBlockLevels = NULL;
	m_BlockLevelCount = 0;
	m_BlockSize = 0;
	m_LevelSize = 0;
	m_BufferSize = 0;
	m_pBuffer = NULL;
	m_AllocCount = 0;
	m_FreeCount = 0;
	m_SystemAllocCount = 0;
	m_SystemFreeCount = 0;
}

CFastMemoryPool::~CFastMemoryPool(void)
{
	Destory();
}

BOOL CFastMemoryPool::Create(UINT BlockSize, UINT LevelSize, UINT MaxLevel, bool IsThreadLock)
{
	Destory();

	m_BlockSize = BlockSize;
	m_LevelSize = LevelSize;
	m_IsThreadLock = IsThreadLock;

	UINT NodeAditionSize = sizeof(BlockNode) + sizeof(UINT);

	//计算最多可划分多少个级别
	UINT Block = BlockSize;
	m_BlockLevelCount = 0;
	while (Block + NodeAditionSize < LevelSize&&MaxLevel)
	{
		m_BlockLevelCount++;
		Block = Block << 1;
		MaxLevel--;
	}

	m_BufferSize = m_LevelSize * m_BlockLevelCount;
	m_pBuffer = MONITORED_NEW_ARRAY(_T("CFastMemoryPool"), BYTE, m_BufferSize);
	m_pBlockLevels = MONITORED_NEW_ARRAY(_T("CFastMemoryPool"), BlockList, m_BlockLevelCount);

	PrintImportantLog(_T("内存池已初始化,开始地址0x%p,结束地址0x%p,总大小%u,层大小%u,层数%u"),
		m_pBuffer, m_pBuffer + m_BufferSize, m_BufferSize, m_LevelSize, m_BlockLevelCount);

	for (UINT i = 0; i < m_BlockLevelCount; i++)
	{
		BYTE * pLevelBuffer = m_pBuffer + i * m_LevelSize;
		m_pBlockLevels[i].AvailableSize = m_BlockSize << i;
		m_pBlockLevels[i].BlockSize = m_pBlockLevels[i].AvailableSize + NodeAditionSize;
		m_pBlockLevels[i].BlockCount = m_LevelSize / m_pBlockLevels[i].BlockSize;
		m_pBlockLevels[i].UsedCount = 0;
		m_pBlockLevels[i].pBlocks = (BlockNode *)pLevelBuffer;
		m_pBlockLevels[i].pFreeList = m_pBlockLevels[i].pBlocks;

		PrintImportantLog(_T("第%u层:地址范围0x%p-0x%p,总大小%u,总块数%u,块大小%u,可用块大小%u"),
			i, pLevelBuffer, pLevelBuffer + m_LevelSize - 1, m_LevelSize, m_pBlockLevels[i].BlockCount, m_pBlockLevels[i].BlockSize, m_pBlockLevels[i].AvailableSize);

		for (UINT j = 0; j < m_pBlockLevels[i].BlockCount; j++)
		{
			BlockNode * pBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + j * m_pBlockLevels[i].BlockSize);
			BlockNode * pPrevBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + (j - 1)*m_pBlockLevels[i].BlockSize);
			BlockNode * pNextBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + (j + 1)*m_pBlockLevels[i].BlockSize);

			pBlocks->pBlockList = m_pBlockLevels + i;
			pBlocks->AllocSize = 0;
			pBlocks->Flag = BF_FREE;

			if (j == 0)
			{
				pBlocks->pPrev = NULL;
			}
			else
			{
				pBlocks->pPrev = pPrevBlocks;
			}
			if (j == m_pBlockLevels[i].BlockCount - 1)
			{
				pBlocks->pNext = NULL;
			}
			else
			{
				pBlocks->pNext = pNextBlocks;
			}

		}
	}

	
	return TRUE;
}

void CFastMemoryPool::Destory()
{
#ifdef _DEBUG
	for (UINT i = 0; i < m_BlockLevelCount; i++)
	{
		if (m_pBlockLevels[i].UsedCount)
			PrintImportantLog(_T("还有%d个内存块未释放！"), m_pBlockLevels[i].UsedCount);
	}
#endif
	SAFE_DELETE_ARRAY(m_pBlockLevels);
	SAFE_DELETE_ARRAY(m_pBuffer);
	m_BlockLevelCount = 0;
	m_BlockSize = 0;
	m_LevelSize = 0;
	m_BufferSize = 0;
}

void CFastMemoryPool::Clear()
{
	for (UINT i = 0; i < m_BlockLevelCount; i++)
	{
		m_pBlockLevels[i].UsedCount = 0;
		m_pBlockLevels[i].pFreeList = m_pBlockLevels[i].pBlocks;

		for (UINT j = 0; j < m_pBlockLevels[i].BlockCount; j++)
		{
			BlockNode * pBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + j * m_pBlockLevels[i].BlockSize);
			BlockNode * pPrevBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + (j - 1)*m_pBlockLevels[i].BlockSize);
			BlockNode * pNextBlocks = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + (j + 1)*m_pBlockLevels[i].BlockSize);

			pBlocks->Flag = BF_FREE;

			if (j == 0)
			{
				pBlocks->pPrev = NULL;
			}
			else
			{
				pBlocks->pPrev = pPrevBlocks;
			}
			if (j == m_pBlockLevels[i].BlockCount - 1)
			{
				pBlocks->pNext = NULL;
			}
			else
			{
				pBlocks->pNext = pNextBlocks;
			}
		}
	}
}

void CFastMemoryPool::Verfy(int LogChannel)
{
	CAutoLockEx ThreadLock;
	if (m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0, _T("开始内存池校验：PoolAlloc=%u/%u,SystemAlloc=%u/%u"),
		m_AllocCount, m_FreeCount, m_SystemAllocCount, m_SystemFreeCount);
	for (UINT i = 0; i < m_BlockLevelCount; i++)
	{
		for (UINT j = 0; j < m_pBlockLevels[i].BlockCount; j++)
		{
			BlockNode * pNode = (BlockNode *)((char *)m_pBlockLevels[i].pBlocks + j * m_pBlockLevels[i].BlockSize);
			BYTE * pData = (BYTE *)pNode + sizeof(BlockNode);

			if ((pNode->Flag == BF_USED) || (pNode->Flag != BF_FREE))
			{
				PrintImportantLog(_T("内存块%p头部已被破坏"), pNode);
				DumpBlock(pNode);
				assert(false);
			}
			if (pNode->Flag == BF_USED)
			{
				if (*((UINT *)(((BYTE *)pNode) + sizeof(BlockNode) + pNode->AllocSize)) != BF_TAIL)
				{
					PrintImportantLog(_T("内存块%p尾部已被破坏"), pNode);
					DumpBlock(pNode);
					assert(false);
				}
			}
		}
	}
	CLogManager::GetInstance()->PrintLog(LogChannel, ILogPrinter::LOG_LEVEL_NORMAL, 0, _T("内存池校验结束"));
}

LPVOID CFastMemoryPool::Alloc(UINT Size)
{
	if (m_pBuffer&&Size)
	{
		UINT Level = 0;
		for (; Level < m_BlockLevelCount; Level++)
		{
			if (m_pBlockLevels[Level].AvailableSize >= Size)
				break;
		}
		if (Level < m_BlockLevelCount)
		{
			LPVOID pMem = AllocBlock(m_pBlockLevels + Level, Size);
			if (pMem)
			{
				AtomicInc(&m_AllocCount);
				return pMem;
			}
		}

		AtomicInc(&m_SystemAllocCount);
		return MONITORED_NEW_ARRAY(_T("CFastMemoryPool"), char, Size);
	}
	else
		return NULL;
}

BOOL CFastMemoryPool::Free(LPVOID pMem)
{
	if (pMem)
	{
		if (m_pBuffer)
		{
			if (((size_t)(((BYTE *)pMem) - m_pBuffer)) < m_BufferSize)
			{
				AtomicInc(&m_FreeCount);
				return FreeBlock((BlockNode *)(((BYTE *)pMem) - sizeof(BlockNode)));
			}
			AtomicInc(&m_SystemFreeCount);
			MONITORED_DELETE_ARRAY((char *)pMem);
			return TRUE;
		}
		else
			return FALSE;
	}
	else
	{
		PrintImportantLog(_T("错误，释放了一个空指针"));
		return FALSE;
	}
}



LPVOID CFastMemoryPool::AllocBlock(BlockList * pBlockList, UINT AllocSize)
{
	CAutoLockEx ThreadLock;
	if (m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	if (pBlockList)
	{
		if (AllocSize <= pBlockList->AvailableSize)
		{
			if (pBlockList->pFreeList)
			{
				volatile BlockNode * pNode;

				pNode = pBlockList->pFreeList;
				pBlockList->pFreeList = pNode->pNext;
				if (pBlockList->pFreeList)
					pBlockList->pFreeList->pPrev = NULL;
				pNode->Flag = BF_USED;
				*((UINT *)(((BYTE *)pNode) + sizeof(BlockNode) + AllocSize)) = BF_TAIL;
				pNode->AllocSize = AllocSize;
				pBlockList->UsedCount++;

				return (char *)pNode + sizeof(BlockNode);
			}
		}
		else
		{
			PrintImportantLog(_T("错误，请求分配大小%u大于块大小%u"), AllocSize, pBlockList->AvailableSize);
		}
	}
	return NULL;
}

BOOL CFastMemoryPool::FreeBlock(BlockNode * pNode)
{
	CAutoLockEx ThreadLock;
	if (m_IsThreadLock)
		ThreadLock.Lock(m_EasyCriticalSection);

	if (pNode)
	{
		if (pNode->Flag != BF_USED)
		{
			PrintImportantLog(_T("内存块头部已被破坏[0x%X],大小%u"), pNode->Flag, pNode->AllocSize);
			DumpBlock(pNode);
			assert(false);

		}
		BlockList * pBlockList = pNode->pBlockList;

		if (*((UINT *)(((BYTE *)pNode) + sizeof(BlockNode) + pNode->AllocSize)) != BF_TAIL)
		{
			PrintImportantLog(_T("内存块尾部已被破坏,大小%u"), pNode->AllocSize);
			DumpBlock(pNode);
			assert(false);
		}
		

		pNode->AllocSize = 0;
		pNode->Flag = BF_FREE;
		pNode->pPrev = NULL;
		pNode->pNext = pBlockList->pFreeList;
		if (pBlockList->pFreeList)
			pBlockList->pFreeList->pPrev = pNode;
		pBlockList->pFreeList = pNode;
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
	for (UINT i = 0; i < m_BlockLevelCount; i++)
	{
		BlockList * pLevel = m_pBlockLevels + i;
		if ((((BYTE *)pNode) >= ((BYTE *)pLevel->pBlocks)) && (((BYTE *)pNode) < ((BYTE *)pLevel->pBlocks) + pLevel->BlockSize*pLevel->BlockCount))
		{
			UINT BlockIndex = (UINT)((((BYTE *)pNode) - ((BYTE *)pLevel->pBlocks)) / pLevel->BlockSize);
			PrintImportantLog(_T("块0x%p位于第%u层的第%u块,层开始地址0x%p,块大小%u,总块数%u"),
				pNode, i, BlockIndex, pLevel->pBlocks, pLevel->BlockSize, pLevel->BlockCount);
			PrintImportantLog(_T("出错块%u:"), BlockIndex);
			DumpBinData((BYTE *)pNode, pLevel->BlockSize, 1024);
			if (BlockIndex)
			{
				PrintImportantLog(_T("前一块%u:"), BlockIndex-1);
				BYTE * pPrev = ((BYTE *)pNode) - pLevel->BlockSize;
				DumpBinData(pPrev, pLevel->BlockSize, 1024);
			}
			if (BlockIndex < pLevel->BlockCount - 1)
			{
				PrintImportantLog(_T("后一块%u:"), BlockIndex + 1);
				BYTE * pNext = ((BYTE *)pNode) + pLevel->BlockSize;
				DumpBinData(pNext, pLevel->BlockSize, 1024);
			}
			return;
		}
	}
	PrintImportantLog(_T("未能找到出错内存块0x%p的位置"), pNode);
	DumpBinData((BYTE *)pNode, pNode->AllocSize + sizeof(BlockNode) + sizeof(UINT) * 2, 1024);
}




void CFastMemoryPool::DoStat()
{

}
