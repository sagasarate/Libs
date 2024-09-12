/****************************************************************************/
/*                                                                          */
/*      文件名:    FastMemoryPool.h                                         */
/*      创建日期:  2009年10月23日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

class CFastMemoryPool :
	public CNameObject
{
protected:
	enum BLOCK_FLAG
	{
		BF_FREE=0x12345678,
		BF_USED=0x9ABCDEF0,
		BF_TAIL=0xAABBCCDD,
	};

	enum
	{
		MAX_CALL_STACK=10,
		MAX_CALL_INFO=10,
	};
	struct CALL_INFO
	{
		DWORD64							CallStack[MAX_CALL_STACK];		
		UINT							StackDepth;
		int								RefCount;		
	};


	struct BlockList;
	struct BlockNode
	{	
		UINT					Flag;
		UINT					AllocSize;
		BlockList *				pBlockList;
		volatile BlockNode *	pPrev;
		volatile BlockNode *	pNext;		
	};
	struct BlockList
	{	
		UINT					AvailableSize;
		UINT					BlockSize;		
		UINT					BlockCount;
		volatile UINT			UsedCount;
		BlockNode *				pBlocks;
		volatile BlockNode *	pFreeList;		
	};
	

	BlockList *			m_pBlockLevels;
	UINT				m_BlockSize;
	UINT				m_LevelSize;
	UINT				m_BlockLevelCount;
	BYTE *				m_pBuffer;
	size_t				m_BufferSize;
	bool				m_IsThreadLock;

	volatile UINT		m_AllocCount;
	volatile UINT		m_FreeCount;
	volatile UINT		m_SystemAllocCount;
	volatile UINT		m_SystemFreeCount;


	CEasyCriticalSection	m_EasyCriticalSection;	

	DECLARE_CLASS_INFO_STATIC(CFastMemoryPool)
public:
	CFastMemoryPool(void);
	virtual ~CFastMemoryPool(void);

	BOOL Create(UINT BlockSize,UINT LevelSize,UINT MaxLevel=0,bool IsThreadLock=false);
	virtual void Destory();
	void Clear();
	void Verfy(int LogChannel);

	LPVOID Alloc(UINT Size);
	BOOL Free(LPVOID pMem);

protected:
	LPVOID AllocBlock(BlockList * pBlockList,UINT AllocSize);
	BOOL FreeBlock(BlockNode * pNode);
	void DumpBlock(BlockNode * pNode);
public:
	UINT GetAllocCount();
	UINT GetFreeCount();
	UINT GetSystemAllocCount();
	UINT GetSystemFreeCount();
	void DoStat();
	
};

inline UINT CFastMemoryPool::GetAllocCount()
{
	return m_AllocCount;
}
inline UINT CFastMemoryPool::GetFreeCount()
{
	return m_FreeCount;
}
inline UINT CFastMemoryPool::GetSystemAllocCount()
{
	return m_SystemAllocCount;
}
inline UINT CFastMemoryPool::GetSystemFreeCount()
{
	return m_SystemFreeCount;
}