/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DUIRender.cpp                                          */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


namespace D3DLib{


IMPLEMENT_CLASS_INFO(CD3DUIRender,CD3DBaseRender);

CD3DUIRender::CD3DUIRender()
{
	m_ObjectList.Create(100,10);
	m_RootObjectList.Create(100,10);
}

CD3DUIRender::~CD3DUIRender(void)
{	
	Destory();	
}

bool CD3DUIRender::Create(CD3DDevice * pDevice)
{
	m_pDevice=pDevice;	
	return true;	
}

void CD3DUIRender::Destory()
{
	CAutoLockEx Lock;
	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	RemoveAllObject();
	CD3DBaseRender::Destory();
}

bool CD3DUIRender::Reset()
{		
	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->Reset();
	}
	return true;
}

bool CD3DUIRender::Restore() 
{		
	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->Restore();
	}
    return true;
}

void CD3DUIRender::Render()
{	
	CAutoLockEx RenderLock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		RenderLock.Lock(m_RenderLock);
		
	}

	LPDIRECT3DDEVICE9 pD3DDevice=m_pDevice->GetD3DDevice();
	if(pD3DDevice==NULL)
		return ;	


	//清空计数
	m_ObjectCount=0;
	m_SubMeshCount=0;
	m_FaceCount=0;
	m_VertexCount=0;

	bool IsCurDefaultLight=false;

	CEasyArray<CD3DLight *>	EmptyLightList;


	for(UINT j=0;j<m_ObjectList.GetCount();j++)
	{
		CD3DObject * pObject=m_ObjectList[j];
		if(pObject->IsVisible())
		{
			m_ObjectCount++;
			
			CAutoLockEx ObjectLock;

			if(CD3DDevice::IsUseMultiThreadRender())
			{
				ObjectLock.Lock(pObject->GetRenderLock());
			}
			
			//pObject->OnPrepareRender(this,NULL,EmptyLightList,NULL);
			int SubMeshCount=pObject->GetSubMeshCount();
			for(int i=0;i<SubMeshCount;i++)
			{
				CD3DSubMesh * pSubMesh;
				CD3DSubMeshMaterial * pMaterial;

				pSubMesh=pObject->GetSubMesh(i);
				pMaterial=pObject->GetSubMeshMaterial(i);
				if(pSubMesh)
				{
					if(pSubMesh->IsVisible())
					{
						pObject->OnPrepareRenderSubMesh(this,pMaterial->GetFX(),pSubMesh,pMaterial,EmptyLightList,NULL);
						RenderSubMesh(pSubMesh,pMaterial->GetFX(),NULL);
					}					
				}
					
			}
		}
		
	}	
}

void CD3DUIRender::RenderDirectly(CD3DObject * pObject)
{
	LPDIRECT3DDEVICE9 pD3DDevice=m_pDevice->GetD3DDevice();
	if(pD3DDevice==NULL)
		return ;	

	if(pObject->IsVisible())
	{
		CAutoLockEx Lock;

		if(CD3DDevice::IsUseMultiThreadRender())
		{
			Lock.Lock(pObject->GetRenderLock());
		}

		if(CD3DDevice::IsUseMultiThreadRender())
		{
			pObject->OnPrepareRenderData();
		}

		CEasyArray<CD3DLight *>	EmptyLightList;

		int SubMeshCount=pObject->GetSubMeshCount();
		//pObject->OnPrepareRender(this,NULL,EmptyLightList,NULL);
		for(int i=0;i<SubMeshCount;i++)
		{
			CD3DSubMesh * pSubMesh;
			CD3DSubMeshMaterial * pMaterial;

			pSubMesh=pObject->GetSubMesh(i);
			pMaterial=pObject->GetSubMeshMaterial(i);
			if(pSubMesh)
			{
				pObject->OnPrepareRenderSubMesh(this,pMaterial->GetFX(),pSubMesh,pMaterial,EmptyLightList,NULL);
				RenderSubMesh(pSubMesh,pMaterial->GetFX(),NULL);
			}

		}
	}

}


bool CD3DUIRender::AddObject(CD3DObject * pObj,bool IsRecursive)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	if(pObj->GetParent()==NULL)
		AddRootObject(pObj);

	pObj->SetRender(this);
	if(pObj->CanRender())
	{
		bool IsExist=false;
		for(UINT i=0;i<m_ObjectList.GetCount();i++)
		{
			if(m_ObjectList[i]==pObj)
			{
				IsExist=true;
				break;
			}
		}	
		if(!IsExist)
			m_ObjectList.Add(pObj);
	}	

	if(IsRecursive)
	{
		for(UINT i=0;i<pObj->GetChildCount();i++)
		{
			AddObject(pObj->GetChildByIndex(i));
		}
	}
	
	return true;
}


bool CD3DUIRender::DelObject(CD3DObject * pObj,bool IsRecursive)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	for(int i=(int)m_ObjectList.GetCount()-1;i>=0;i--)
	{
		if(m_ObjectList[i]==pObj)
		{
			m_ObjectList.Delete(i);
			
			if(pObj->GetParent()==NULL)
				DelRootObject(pObj);

			pObj->SetRender(NULL);
			return true;
		}
	}

	if(IsRecursive)
	{
		for(UINT i=0;i<pObj->GetChildCount();i++)
		{
			DelObject(pObj->GetChildByIndex(i));
		}
	}
	return false;
}

void CD3DUIRender::RemoveAllObject()
{
	CD3DBaseRender::RemoveAllObject();
	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->SetRender(NULL);		
	}
	m_ObjectList.Clear();
}

bool CD3DUIRender::MoveToTop(CD3DObject * pObj,CD3DObject *pBefore)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	bool IsExist=false;
	for(int i=(int)m_ObjectList.GetCount()-1;i>=0;i--)
	{
		if(m_ObjectList[i]==pObj)
		{
			m_ObjectList.Delete(i);
			IsExist=true;
		}
	}

	if(IsExist)
	{
		if(pBefore)
		{
			for(UINT i=0;i<m_ObjectList.GetCount();i++)
			{		
				if(m_ObjectList[i]==pBefore)
				{				
					m_ObjectList.Insert(i+1,pObj);
					return true;				
				}
			}	
		}
		m_ObjectList.Add(pObj);
		return true;
	}	
	return false;
}

bool CD3DUIRender::MoveToTop(CD3DObject ** ppObj,UINT ObjectCount,CD3DObject *pBefore)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	CEasyArray<CD3DObject *> WantTopList(64,64);
	for(UINT i=0;i<ObjectCount;i++)
	{
		for(int j=(int)m_ObjectList.GetCount()-1;j>=0;j--)
		{
			if(m_ObjectList[j]==ppObj[i])
			{
				m_ObjectList.Delete(j);
				WantTopList.Add(ppObj[i]);
			}
		}
	}

	if(pBefore)
	{
		for(int i=0;i<(int)m_ObjectList.GetCount();i++)
		{	
			if(m_ObjectList[i]==pBefore)
			{	
				m_ObjectList.Insert(i+1,WantTopList);
				return true;;
			}
		}	
	}
	m_ObjectList.AddArray(WantTopList);
	return true;
}

}
