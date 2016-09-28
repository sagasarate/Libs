/****************************************************************************/
/*                                                                          */
/*      文件名:    ThreadSafeIDStorage.h                                    */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once


template < class T ,int StorageMode=EDSM_STATIC>
class CThreadSafeIDStorage:public CIDStorage<T,StorageMode>
{
protected:
	CEasyCriticalSection		m_EasyCriticalSection;
public:
	CThreadSafeIDStorage()
	{
		
	}
	~CThreadSafeIDStorage()
	{
		
	}
	bool Create(UINT Size,UINT GrowSize=0,UINT GrowLimit=0)
	{	
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::Create(Size,GrowSize,GrowLimit);
	}
	bool Create(const STORAGE_POOL_SETTING& PoolSetting)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T, StorageMode>::Create(PoolSetting.StartSize, PoolSetting.GrowSize, PoolSetting.GrowLimit);
	}
	void Destory()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		CIDStorage<T,StorageMode>::Destory();
	}
	void Clear()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		CIDStorage<T,StorageMode>::Clear();
	}
	UINT NewObject(T** ppObject)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::NewObject(ppObject);
	}	

	T* NewObject()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::NewObject();
	}	
	LPVOID InsertAfter(LPVOID Pos=NULL)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::InsertAfter(Pos);
	}
	LPVOID InsertAfter(const T& Object,LPVOID Pos=NULL)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::InsertAfter(Object,Pos);
	}
	LPVOID InsertBefore(LPVOID Pos=NULL)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::InsertBefore(Pos);
	}
	LPVOID InsertBefore(const T& Object,LPVOID Pos=NULL)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::InsertBefore(Object,Pos);
	}
	LPVOID InsertSorted(const T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::InsertSorted(Object);
	}
	UINT AddObject(const T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::AddObject(Object);
	}		
	//LPVOID GetObjectPos(UINT ID)
	//{
	//	CAutoLock Lock(m_EasyCriticalSection);
	//	return CIDStorage<T,StorageMode>::GetObjectPos(ID);
	//}
	//T* GetObject(LPVOID Pos)
	//{
	//	CAutoLock Lock(m_EasyCriticalSection);
	//	return CIDStorage<T,StorageMode>::GetObject(Pos);
	//}	
	//T* GetObject(UINT ID)
	//{
	//	CAutoLock Lock(m_EasyCriticalSection);
	//	return CIDStorage<T,StorageMode>::GetObject(ID);
	//}
	//UINT GetObjectID(LPVOID Pos)
	//{
	//	CAutoLock Lock(m_EasyCriticalSection);
	//	return CIDStorage<T,StorageMode>::GetObjectID(Pos);
	//}
	BOOL DeleteObject(UINT ID)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::DeleteObject(ID);
	}	
	T* GetNextObject(LPVOID& Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::GetNextObject(Pos);
	}

	T* GetPrevObject(LPVOID& Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T, StorageMode>::GetPrevObject(Pos);
	}
	BOOL MoveBeforeTo(LPVOID Pos,LPVOID Target)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::MoveBeforeTo(Pos,Target);
	}
	BOOL MoveAfterTo(LPVOID Pos,LPVOID Target)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::MoveAfterTo(Pos,Target);
	}
	BOOL MoveSorted(LPVOID Pos)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::MoveSorted(Pos);
	}
	LPVOID PushFront()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PushFront();
	}

	LPVOID PushFront(const T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PushFront(Object);
	}

	LPVOID PushBack()
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PushBack();
	}

	LPVOID PushBack(const T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PushBack(Object);
	}

	BOOL PopFront(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PopFront(Object);
	}

	BOOL PopBack(T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::PopBack(Object);
	}
	LPVOID Find(const T& Object)
	{
		CAutoLock Lock(m_EasyCriticalSection);
		return CIDStorage<T,StorageMode>::Find(Object);		
	}

	void Verfy(UINT& UsedCount,UINT& FreeCount)
	{
		CAutoLock Lock(m_EasyCriticalSection);

		CIDStorage<T,StorageMode>::Verfy(UsedCount,FreeCount);
	}
};
