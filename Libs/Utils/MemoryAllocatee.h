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




class CMemoryAllocatee
{
protected:
	struct ALLOC_INFO
	{
		int				AllocSize;
		LPCTSTR			FileName;
		int				Line;
	};

	static bool				m_IsWorking;
	static CMemoryAllocatee m_MemoryAllocatee;

	CEasyMap<void *,ALLOC_INFO>	m_AllocInfos;
public:
	CMemoryAllocatee(void);
	~CMemoryAllocatee(void);

	void * Alloc(int Size,LPCTSTR FileName,int Line);
	void * Realloc(void * pBlock,int Size,LPCTSTR FileName,int Line);
	void Free(void * pBlock);

	static CMemoryAllocatee * GetInstance();
};
