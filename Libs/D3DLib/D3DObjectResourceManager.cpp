/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DObjectResourceManager.cpp                             */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO_STATIC(CD3DObjectResourceManager,CNameObject);

CD3DObjectResourceManager::CD3DObjectResourceManager(CD3DDevice * pD3DDevice,int StorageSize):CNameObject()
{
	m_pD3DDevice=pD3DDevice;
	m_ObjectStorage.Create(StorageSize);
}

CD3DObjectResourceManager::~CD3DObjectResourceManager(void)
{
#ifdef _DEBUG
	void * Pos;

	Pos=m_ObjectStorage.GetFirstObjectPos();
	while(Pos)
	{
		CEasyString Key;
		CD3DObjectResource * pResource=*(m_ObjectStorage.GetNextObject(Pos,Key));
		PrintSystemLog(0,_T("对象资源<%s>未释放！"),(LPCTSTR)pResource->GetName());

	}
	m_ObjectStorage.Destory();
#endif
}

bool CD3DObjectResourceManager::AddResource(CD3DObjectResource * pResource,LPCTSTR ResourceName)
{	
	CEasyString Key=ResourceName;
	Key.MakeUpper();
	UINT ID=m_ObjectStorage.Insert(Key,pResource);
	if(ID)
	{
		pResource->SetID(ID);
		pResource->SetName(ResourceName);
		return true;
	}
	else
	{
		PrintD3DLog(0,_T("将资源添加到对象资源管理器失败(%u,%u)"),
			m_ObjectStorage.GetObjectCount(),m_ObjectStorage.GetBufferSize());
		return false;
	}
}

CD3DObjectResource * CD3DObjectResourceManager::DeleteResource(UINT ID)
{
	CD3DObjectResource ** ppResource=m_ObjectStorage.GetObject(ID);
	if(ppResource)
	{
		m_ObjectStorage.DeleteByID(ID);		
		return *ppResource;
	}
	else
	{
		PrintD3DLog(0,_T("CD3DObjectResourceManager::DeleteResource:资源[%u]未找到"),
			ID);
	}
	return NULL;
}

CD3DObjectResource * CD3DObjectResourceManager::DeleteResource(LPCTSTR ResourceName)
{
	CEasyString Key=ResourceName;
	Key.MakeUpper();
	CD3DObjectResource ** ppResource=m_ObjectStorage.Find(Key);
	if(ppResource)
	{
		m_ObjectStorage.DeleteByID((*ppResource)->GetID());		
		return *ppResource;
	}
	else
	{
		PrintD3DLog(0,_T("CD3DObjectResourceManager::DeleteResource:资源[%s]未找到"),
			ResourceName);
	}
	return NULL;
}

int CD3DObjectResourceManager::GetCount()
{
	return m_ObjectStorage.GetObjectCount();
}

LPVOID CD3DObjectResourceManager::GetFirstPos()
{
	return m_ObjectStorage.GetFirstObjectPos();
}

LPVOID CD3DObjectResourceManager::GetLastPos()
{
	return m_ObjectStorage.GetLastObjectPos();
}

CD3DObjectResource * CD3DObjectResourceManager::GetNext(LPVOID& Pos)
{
	CEasyString Key;
	return *m_ObjectStorage.GetNextObject(Pos,Key);
}

CD3DObjectResource * CD3DObjectResourceManager::GetPrev(LPVOID& Pos)
{
	CEasyString Key;
	return *m_ObjectStorage.GetPrevObject(Pos,Key);
}

}