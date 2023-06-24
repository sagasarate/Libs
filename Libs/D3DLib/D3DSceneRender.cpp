/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSceneRender.cpp                                       */
/*      创建日期:  2010年07月19日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/

#include "StdAfx.h"


namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DSceneRender,CD3DBaseRender);


CD3DSceneRender::CD3DSceneRender(void)
{
	m_pCamera=NULL;
	m_pBackGroundCamera=NULL;
	m_pSharedParamFX=NULL;
	m_pGlobalLight=NULL;
	m_FogColor=0xFFFFFFFF;
	m_FogNear=10000.0f;
	m_FogFar=10000.0f;
	m_WaterColorLight=0xFF666666;
	m_WaterColorDark=0xFF191919;
	m_ShadowColor=0x80808080;

	m_TreeMode=TREE_MODE_QD;
	m_pSceneRoot=NULL;
	m_pSelectedSceneTree=NULL;
	m_MaxTreeDepth=0;
	m_TreeDepthLimit=0;
	m_pDepthTexture=NULL;
	m_ShowNodeFrame=false;
	m_SizeCullSize=0;
}

CD3DSceneRender::~CD3DSceneRender(void)
{
	Destory();
}

bool CD3DSceneRender::Create(CD3DDevice * pDevice,UINT TreeMode,CD3DBoundingBox SceneBox,int StartDepth,int DepthLimit)
{
	Destory();

	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	m_pDevice=pDevice;	
	m_TreeMode=TreeMode;
	m_ObjectList.Create(1024,1024);
	m_SceneRenderUnitList.Create(1024,1024);
	m_SceneObjectList.Create(1024,1024);
	m_RenderBatchList.Create(1024,1024);
	m_TransparentSubMeshList.Create(1024,1024);

	m_pSharedParamFX=m_pDevice->GetFXManager()->LoadFXFromMemory(_T("SHARED_PARAM_FX"),SHARED_PARAM_FX,strlen(SHARED_PARAM_FX));

	m_pSharedParamFX->SetBool("IsGlobalLightEnable",true);

	m_MaxTreeDepth=0;
	m_TreeDepthLimit=DepthLimit;

	m_pSceneRoot=new TREE_NODE;
	m_pSceneRoot->NodeBox=SceneBox;
	m_pSceneRoot->IsFixed=true;


	BuildSceneTree(m_pSceneRoot,StartDepth);

	//UpdateSceneTree();
	return true;
}

void CD3DSceneRender::Destory()
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	RemoveAllObject();

	SAFE_RELEASE(m_pSharedParamFX);	
	SAFE_RELEASE(m_pDepthTexture);	
	if(m_pSceneRoot)
	{
		DeleteSceneNode(m_pSceneRoot);
		m_pSceneRoot=NULL;
	}	
	
	CD3DBaseRender::Destory();
}

bool CD3DSceneRender::Reset()
{		
	m_pDepthTexture->Reset();
	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->Reset();
	}
	for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
	{
		m_BackGroundObjectList[i]->Reset();
	}
	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		m_SceneObjectList[i]->Reset();
	}	
	return true;
}

bool CD3DSceneRender::Restore() 
{		
	if(m_pDepthTexture)
	{
		CreateDepthTexture();
	}
	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->Restore();
	}
	for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
	{
		m_BackGroundObjectList[i]->Restore();
	}
	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		m_SceneObjectList[i]->Restore();
	}	
	return true;
}


bool CD3DSceneRender::DelObject(CD3DObject * pObj,bool IsRecursive)
{
	{
		CAutoLockEx Lock;

		if(CD3DDevice::IsUseMultiThreadRender())
		{
			Lock.Lock(m_RenderLock);
		}

		if(pObj->IsKindOf(GET_CLASS_INFO(CD3DWOWWMOModel)))
			DelWMOObject((CD3DWOWWMOModel *)pObj);

		if(pObj->GetParent()==NULL)
			DelRootObject(pObj);

		pObj->SetRender(NULL);

		if(pObj->IsKindOf(GET_CLASS_INFO(CD3DLight)))
		{
			DeleteLight((CD3DLight *)pObj);
		}
		else if(pObj->CanRender())
		{
			bool IsDeleted=false;
			for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
			{
				if(m_BackGroundObjectList[i]==pObj)
				{
					m_BackGroundObjectList.Delete(i);
					IsDeleted=true;
					break;
				}
			}
			if(!IsDeleted)
			{
				for(int i=(int)m_ObjectList.GetCount()-1;i>=0;i--)
				{
					if(m_ObjectList[i]==pObj)
					{
						m_ObjectList.Delete(i);					
						IsDeleted=true;
						break;
					}
				}
			}

			if(!IsDeleted)
			{
				if(!DeleteSceneObject(pObj))				
				{
					PrintD3DLog(_T("从场景树中删除对象[%s,%s]失败"),
						pObj->GetName(),pObj->GetClassInfo().ClassName);
				}
			}
		}
	}

	if(IsRecursive)
	{
		for(UINT i=0;i<pObj->GetChildCount();i++)
		{
			DelObject(pObj->GetChildByIndex(i));
		}
	}

	return true;
}

bool CD3DSceneRender::AddSceneObject(CD3DObject * pObject,bool IsRecursive)
{
	
	
	if((!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWADTModel)))&&
		(!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWWMOModel)))&&
		(!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWWMOGroupModel)))&&
		(!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWM2Model)))&&
		(!pObject->IsKindOf(GET_CLASS_INFO(CD3DBillBoardParticleEmitter)))&&
		(!pObject->IsKindOf(GET_CLASS_INFO(CD3DScene))))
	{
		return AddObject(pObject,IsRecursive);
	}
	
	pObject->Update(0);		
	CD3DBoundingBox BBox=(*(pObject->GetBoundingBox()))*pObject->GetWorldMatrix();

	//CD3DVector3 BoxSize=BBox.GetSize();
	//if(BoxSize.x<=0||BoxSize.y<=0||BoxSize.z<=0)
	//{
	//	PrintD3DDebugLog("对象[%s]包围和大小异常，无法加入场景树，当作非场景对象处理",pObject->GetName());
	//	if(!AddObject(pObject,false))
	//		return false;
	//}
	//else
	{
		int Relation=m_pSceneRoot->NodeBox.CheckRelation(BBox);
		if((!BBox.IsValid())||Relation==CD3DBoundingBox::RELATION_TYPE_OUT||Relation==CD3DBoundingBox::RELATION_TYPE_BE_INCLUDE)
		{
			PrintD3DLog(_T("对象[%s]因为超出场景范围，无法加入场景树"),pObject->GetName());
			if(!AddObject(pObject,false))
				return false;
		}
		
		if(pObject->CanRender())
		{
			CAutoLockEx Lock;

			if(CD3DDevice::IsUseMultiThreadRender())
			{
				Lock.Lock(m_RenderLock);
			}

			pObject->SetRender(this);

			if(pObject->GetParent()==NULL)
				AddRootObject(pObject);
		

			//WMO本身不加入场景树
			if(pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWWMOModel)))
			{
				if(!AddObject(pObject,false))
					return false;
			}			
			else if(!AddObjectToTree(m_pSceneRoot,pObject,true))
			{
				return false;			
			}
		}
		else
		{
			PrintD3DLog(_T("对象[%s]不是可渲染对象，不加入场景树"));
			return AddObject(pObject,IsRecursive);
		}
	}
		
	

	if(IsRecursive)
	{
		for(UINT i=0;i<pObject->GetChildCount();i++)
		{
			AddSceneObject(pObject->GetChildByIndex(i),true);
		}
	}		

	
	return true;
}



void CD3DSceneRender::SetCamera(CD3DCamera * pCamera)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	SAFE_RELEASE(m_pCamera);
	m_pCamera=pCamera;
	if(m_pCamera)
	{
		m_pCamera->AddUseRef();
		if(m_pCamera->GetParent()==NULL)
			AddRootObject(m_pCamera);
	}
}

void CD3DSceneRender::SetBackGroundCamera(CD3DCamera * pCamera)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	SAFE_RELEASE(m_pBackGroundCamera);
	m_pBackGroundCamera=pCamera;
	if(m_pBackGroundCamera)
	{
		m_pBackGroundCamera->AddUseRef();
		if(m_pBackGroundCamera->GetParent()==NULL)
			AddRootObject(m_pBackGroundCamera);
	}
}

void CD3DSceneRender::ShowNodeFrame(TREE_NODE * pTree,bool IsShow)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	if(IsShow)
	{
		if(pTree->pBoundingFrame==NULL)
		{
			pTree->pBoundingFrame=new CD3DBoundingFrame();
			pTree->pBoundingFrame->SetDevice(m_pDevice);
			pTree->pBoundingFrame->CreateFromBBox(pTree->NodeBox,0xFF0000FF);
			pTree->pBoundingFrame->SetName(_T("NodeBox"));
			AddObject(pTree->pBoundingFrame);			
		}
	}	
}

void CD3DSceneRender::SetSelectTree(TREE_NODE * pTree)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);

	}
	m_pSelectedSceneTree=pTree;
}

void CD3DSceneRender::HideCutObject(TREE_NODE * pTree)
{
	for(UINT i=0;i<pTree->ObjectList.GetCount();i++)
	{
		if(!pTree->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED))
		{
			pTree->ObjectList[i]->pObject->SetVisible(false);
		}
	}
	if(pTree->ChildCount)
	{
		for(UINT i=0;i<m_TreeMode;i++)
		{
			HideCutObject(pTree->Childs[i]);
		}
	}
}

void CD3DSceneRender::FindTreeByObject(TREE_NODE * pRoot,CD3DObject * pObject,CEasyArray<TREE_NODE *>& TreeList)
{
	for(UINT i=0;i<pRoot->ObjectList.GetCount();i++)
	{
		if(pRoot->ObjectList[i]->pObject==pObject)
		{
			TreeList.Add(pRoot);
			break;
		}
	}
	if(pRoot->ChildCount)
	{
		for(UINT i=0;i<m_TreeMode;i++)
		{
			FindTreeByObject(pRoot->Childs[i],pObject,TreeList);
		}
	}
}

void CD3DSceneRender::FindTreeBySubMesh(TREE_NODE * pRoot,CD3DObject * pObject,CD3DSubMesh * pSubMesh,CEasyArray<TREE_NODE *>& TreeList)
{
	for(UINT i=0;i<pRoot->ObjectList.GetCount();i++)
	{
		if(pRoot->ObjectList[i]->pObject==pObject&&pRoot->ObjectList[i]->pSubMesh==pSubMesh)
		{
			TreeList.Add(pRoot);
			break;
		}
	}
	if(pRoot->ChildCount)
	{
		for(UINT i=0;i<m_TreeMode;i++)
		{
			FindTreeBySubMesh(pRoot->Childs[i],pObject,pSubMesh,TreeList);
		}
	}
}

CD3DSceneRender::TREE_NODE * CD3DSceneRender::FindTreeByBBox(TREE_NODE * pRoot,const CD3DBoundingBox& BBox)
{
	if(CheckBBoxRelation(pRoot->NodeBox,BBox)==CD3DBoundingBox::RELATION_TYPE_INCLUDE)
	{
		if(pRoot->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				TREE_NODE * pNode=FindTreeByBBox(pRoot->Childs[i],BBox);
				if(pNode)
					return pNode;
			}
		}
		return pRoot;
	}
	return NULL;
}

bool CD3DSceneRender::GetHeightByXZ(const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight)
{
	bool HaveHeight=false;
	FLOAT FinalHeight=-1E38f,FinalWaterHeight=-1E38f;
	FLOAT H,WH;

	m_HeightTestCount=0;

	MinHeight=m_pSceneRoot->NodeBox.m_Min.y;

	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		m_WMOObjectList[i]->RemoveFlagRecursive(CD3DObject::OBJECT_FLAG_CHECKED);
	}

	if(HeightTest(m_pSceneRoot,Pos,MinHeight,MaxHeight,H,WH))
	{
		HaveHeight=true;
		if(H>FinalHeight)
			FinalHeight=H;
		if(WH>FinalWaterHeight)
			FinalWaterHeight=WH;
	}
	if(HaveHeight)
	{
		Height=FinalHeight;
		WaterHeight=FinalWaterHeight;
	}
	//PrintD3DDebugLog("HeightTest(%u)",m_HeightTestCount);
	return HaveHeight;
}

bool CD3DSceneRender::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode)
{
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		m_WMOObjectList[i]->RemoveFlagRecursive(CD3DObject::OBJECT_FLAG_CHECKED);
	}
	m_RayIntersectCount=0;
	if(RayIntersect(m_pSceneRoot,Point,Dir,IntersectPoint,Distance,DotValue,ppIntersectObject,TestMode))
	{
		//PrintD3DDebugLog("RayIntersect(%u)",m_RayIntersectCount);
		return true;
	}
	return false;
}

bool CD3DSceneRender::LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode)
{
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		m_WMOObjectList[i]->RemoveFlagRecursive(CD3DObject::OBJECT_FLAG_CHECKED);
	}
	m_LineIntersectCount=0;
	if(LineIntersect(m_pSceneRoot,StartPoint,EndPoint,IntersectPoint,Distance,DotValue,ppIntersectObject,TestMode))
	{
		//PrintD3DDebugLog("LineIntersect(%u)",m_LineIntersectCount);
		return true;
	}
	return false;
}

void CD3DSceneRender::BuildSceneTree(TREE_NODE * pRoot,int StartDepth)
{
	if(StartDepth<=0)
		return;

	StartDepth--;

	CD3DVector3 CutCenter=GetBoxCutCenter(pRoot);
	CreateChilds(pRoot,CutCenter,true);
	for(UINT i=0;i<m_TreeMode;i++)
	{
		pRoot->Childs[i]->IsFixed=true;
		BuildSceneTree(pRoot->Childs[i],StartDepth);
	}
}

bool CD3DSceneRender::AddWMOObject(CD3DWOWWMOModel * pWMOObject)
{
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		if(m_WMOObjectList[i]==pWMOObject)
		{
			PrintD3DLog(_T("CD3DSceneRender::AddWMOObject:WMO对象[%s]已经存在了"),
				pWMOObject->GetName());
			return false;
		}
	}
	m_WMOObjectList.Add(pWMOObject);
	return true;
}
bool CD3DSceneRender::DelWMOObject(CD3DWOWWMOModel * pWMOObject)
{
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		if(m_WMOObjectList[i]==pWMOObject)
		{
			m_WMOObjectList.Delete(i);
			return true;
		}
	}
	PrintD3DLog(_T("CD3DSceneRender::DelWMOObject:WMO对象[%s]不存在"),
		pWMOObject->GetName());
	return false;
}

void CD3DSceneRender::SetGlobalLight(CD3DLight * pLight)
{
	SAFE_RELEASE(m_pGlobalLight);
	m_pGlobalLight=pLight;
	if(m_pGlobalLight)
	{
		m_pGlobalLight->AddUseRef();
		if(m_pGlobalLight->GetParent()==NULL)
			AddRootObject(m_pGlobalLight);
	}
}

void CD3DSceneRender::AddLight(CD3DLight * pLight)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	for(UINT i=0;i<m_LightList.GetCount();i++)
	{
		if(m_LightList[i]==pLight)
		{
			return;
		}
	}
	pLight->SetRender(this);	
	pLight->AddUseRef();
	m_LightList.Add(pLight);	
}

bool CD3DSceneRender::DeleteLight(UINT Index)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	if(Index<m_LightList.GetCount())
	{
		SAFE_RELEASE(m_LightList[Index]);
		m_LightList.Delete(Index);
		return true;
	}
	return false;
}

bool CD3DSceneRender::DeleteLight(CD3DLight * pLight)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	for(UINT i=0;i<m_LightList.GetCount();i++)
	{
		if(m_LightList[i]==pLight)
		{
			return DeleteLight(i);
		}
	}
	return false;
}

bool CD3DSceneRender::DeleteLight(LPCTSTR LightName)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	for(UINT i=0;i<m_LightList.GetCount();i++)
	{
		if(_tcscmp(m_LightList[i]->GetName(),LightName)==0)
		{
			return DeleteLight(i);
		}
	}	
	return false;
}

void CD3DSceneRender::RemoveAllObject()
{
	CAutoLockEx Lock;
	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}

	CD3DBaseRender::RemoveAllObject();

	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		m_ObjectList[i]->SetRender(NULL);		
	}
	m_ObjectList.Clear();

	for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
	{
		m_BackGroundObjectList[i]->SetRender(NULL);		
	}
	m_BackGroundObjectList.Clear();

	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		m_SceneObjectList[i]->SetRender(NULL);		
	}
	m_SceneObjectList.Clear();


	if(m_pSceneRoot)
	{
		if(m_pSceneRoot->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				DeleteSceneNode(m_pSceneRoot->Childs[i]);
				m_pSceneRoot->Childs[i]=NULL;
			}
			m_pSceneRoot->ChildCount=0;
		}
		m_pSceneRoot->ObjectList.Clear();
	}


	for(UINT i=0;i<m_SceneRenderUnitList.GetCount();i++)
	{
		SAFE_DELETE(m_SceneRenderUnitList[i]);
	}
	m_SceneRenderUnitList.Clear();

	for(UINT i=0;i<m_LightList.GetCount();i++)
	{
		SAFE_RELEASE(m_LightList[i]);
	}

	m_WMOObjectList.Clear();

	SAFE_RELEASE(m_pGlobalLight);
	SAFE_RELEASE(m_pCamera);
	SAFE_RELEASE(m_pBackGroundCamera);

	m_MaxTreeDepth=0;

}

//void CD3DSceneRender::UpdateSceneTree()
//{
//	CAutoLockEx Lock;
//
//	if(CD3DDevice::IsUseMultiThreadRender())
//	{
//		Lock.Lock(m_RenderLock);
//	}
//
//
//	UpdateTree(m_pSceneRoot);
//}

void CD3DSceneRender::Render()
{	

	LPDIRECT3DDEVICE9 pD3DDevice=m_pDevice->GetD3DDevice();
	if(pD3DDevice==NULL)
		return ;	
	if(!m_pCamera)
		return;


	CAutoLockEx RenderLock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		RenderLock.Lock(m_RenderLock);		
	}

	//设置共享参数
	
	m_pSharedParamFX->SetColor("FogColor",GetFogColor());
	m_pSharedParamFX->SetFloat("FogNear",GetFogNear());
	m_pSharedParamFX->SetFloat("FogFar",GetFogFar());

	//设置全局光
	if(m_pGlobalLight)
	{
		m_pSharedParamFX->SetValue("GlobalLight",&(m_pGlobalLight->GetLight()),sizeof(D3DLIGHT9));
	}


	////设置摄像机

	//m_pCamera->Apply(m_pDevice,D3DCAMERA_APPLY_ALL);


	////设置灯光
	//for(UINT i=0;i<MAX_LIGHT;i++)
	//{
	//	m_pDevice->GetD3DDevice()->LightEnable(i,false);
	//}
	//for(UINT i=0;i<m_LightList.GetCount();i++)
	//{
	//	if(i<MAX_LIGHT)
	//	{
	//		m_pDevice->GetD3DDevice()->LightEnable(i,true);			
	//		m_LightList[i]->Apply(m_pDevice,i);
	//	}
	//}			


	//渲染背景对象
	if(m_pBackGroundCamera)
	{
		m_RenderBatchList.Empty();
		m_TransparentSubMeshList.Empty();

		m_pSharedParamFX->SetMatrix("PrjMatrix",m_pBackGroundCamera->GetProjectMatR());
		m_pSharedParamFX->SetMatrix("ViewMatrix",m_pBackGroundCamera->GetViewMatR());

		m_pSharedParamFX->SetVector("CameraPos",m_pBackGroundCamera->GetWorldMatrixR().GetTranslation());
		m_pSharedParamFX->SetFloat("CameraNear",m_pBackGroundCamera->GetNear());
		m_pSharedParamFX->SetFloat("CameraFar",m_pBackGroundCamera->GetFar());

		CD3DFrustum Frustum=m_pBackGroundCamera->GetFrustumR();

		for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
		{
			CD3DObject * pObject=m_BackGroundObjectList[i];

			if(!pObject->CheckFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED))
				RenderObject(pObject,Frustum,true);
		}

		qsort(m_RenderBatchList.GetBuffer(),m_RenderBatchList.GetCount(),sizeof(RENDER_SUBMESH_INFO),SubMeshCompare);
		qsort(m_TransparentSubMeshList.GetBuffer(),m_TransparentSubMeshList.GetCount(),sizeof(RENDER_SUBMESH_INFO),SubMeshCompare);

		DoBatchRender();

		//清除ZBuffer
		m_pDevice->Clear(0,D3DCLEAR_ZBUFFER);
	}


	m_pSharedParamFX->SetMatrix("PrjMatrix",m_pCamera->GetProjectMatR());
	m_pSharedParamFX->SetMatrix("ViewMatrix",m_pCamera->GetViewMatR());

	m_pSharedParamFX->SetVector("CameraPos",m_pCamera->GetWorldMatrixR().GetTranslation());
	m_pSharedParamFX->SetFloat("CameraNear",m_pCamera->GetNear());
	m_pSharedParamFX->SetFloat("CameraFar",m_pCamera->GetFar());


	CD3DFrustum Frustum=m_pCamera->GetFrustumR();


	//清空计数
	m_ObjectCount=0;
	m_SubMeshCount=0;
	m_FaceCount=0;
	m_VertexCount=0;

	m_MergeRenderSubMeshCount=0;
	m_FXApplyCount=0;
	m_MaterialSetCount=0;
	m_TreeCheckCount=0;
	m_ObjectCheckCount=0;
	m_DirectRenderCount=0;
	m_BranchObjectCheckCount=0;
	m_TreeCutCount=0;
	m_ObjectCutCount=0;

	m_RenderBatchList.Empty();
	m_TransparentSubMeshList.Empty();


	//进行WMO对象的视口裁剪
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		CD3DBoundingBox BBox=(*m_WMOObjectList[i]->GetBoundingBox())*m_WMOObjectList[i]->GetWorldMatrixR();
		if(Frustum.BoxLocation(BBox))
		{
			m_WMOObjectList[i]->DoPortalCull(m_pCamera);
		}		
	}
	
	//渲染非场景对象

	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		CD3DObject * pObject=m_ObjectList[i];

		if(!pObject->CheckFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED))
			RenderObject(pObject,Frustum,true);
	}
	

	//渲染场景对象

	ClearTreeRenderFlag(m_pSceneRoot);

	CD3DBoundingBox CameraBBox;

	CameraBBox.m_Min=m_pCamera->GetWorldMatrixR().GetTranslation();
	CameraBBox.m_Max=CameraBBox.m_Min;
	CameraBBox.m_Min.x-=m_pCamera->GetFar();
	CameraBBox.m_Min.y-=m_pCamera->GetFar();
	CameraBBox.m_Min.z-=m_pCamera->GetFar();
	CameraBBox.m_Max.x+=m_pCamera->GetFar();
	CameraBBox.m_Max.y+=m_pCamera->GetFar();
	CameraBBox.m_Max.z+=m_pCamera->GetFar();

	TREE_NODE * pRenderTree=FindTreeByBBox(m_pSceneRoot,CameraBBox);
	if(pRenderTree==NULL)
		pRenderTree=m_pSceneRoot;

	if(m_pSelectedSceneTree)
		RenderScene(m_pSelectedSceneTree,Frustum,true);
	else
		RenderScene(pRenderTree,Frustum,true);

	qsort(m_RenderBatchList.GetBuffer(),m_RenderBatchList.GetCount(),sizeof(RENDER_SUBMESH_INFO),SubMeshCompare);
	qsort(m_TransparentSubMeshList.GetBuffer(),m_TransparentSubMeshList.GetCount(),sizeof(RENDER_SUBMESH_INFO),SubMeshCompare);

	DoBatchRender();

	//PrintImportantLog(0,"Face=%d",m_FaceCount);
	
	//清理所有临时渲染标记
	for(UINT i=0;i<m_SceneRenderUnitList.GetCount();i++)
	{
		m_SceneRenderUnitList[i]->IsPreRenderd=false;
		m_SceneRenderUnitList[i]->IsRenderd=false;
	}

	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		if(m_SceneObjectList[i]->CheckFlag(CD3DObject::OBJECT_FLAG_RENDERED))
			m_SceneObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_CULLED);
		else
			m_SceneObjectList[i]->AddFlag(CD3DObject::OBJECT_FLAG_CULLED);
		m_SceneObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED|CD3DObject::OBJECT_FLAG_RENDERED);
	}

	for(UINT i=0;i<m_ObjectList.GetCount();i++)
	{
		if(m_ObjectList[i]->CheckFlag(CD3DObject::OBJECT_FLAG_RENDERED))
			m_ObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_CULLED);
		else
			m_ObjectList[i]->AddFlag(CD3DObject::OBJECT_FLAG_CULLED);
		m_ObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED|CD3DObject::OBJECT_FLAG_RENDERED);
	}

	for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
	{
		if(m_BackGroundObjectList[i]->CheckFlag(CD3DObject::OBJECT_FLAG_RENDERED))
			m_BackGroundObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_CULLED);
		else
			m_BackGroundObjectList[i]->AddFlag(CD3DObject::OBJECT_FLAG_CULLED);
		m_BackGroundObjectList[i]->RemoveFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED|CD3DObject::OBJECT_FLAG_RENDERED);
	}

	
}


void CD3DSceneRender::Update(FLOAT Time)
{
	CD3DObject::m_UpdateCount=0;
	CD3DObject::m_RenderDataUpdateCount=0;
	CD3DBillBoardParticleEmitter::m_BillBoardParticleEmitterUpdateCount=0;

	for(int i=0;i<(int)m_RootObjectList.GetCount();i++)
	{		
		m_RootObjectList[i]->Update(Time);
	}
	if(CD3DDevice::IsUseMultiThreadRender())
	{		
		CAutoLockEx Lock;
		if(m_PrepareRenderDataFailCount<MAX_PREPARE_RENDER_DATA_FAIL_COUNT)
		{
			if(Lock.TryLock(m_RenderLock))
			{
				for(int i=0;i<(int)m_RootObjectList.GetCount();i++)
				{
					m_RootObjectList[i]->OnPrepareRenderData();
				}
				m_PrepareRenderDataFailCount=0;
			}
			else
			{
				m_PrepareRenderDataFailCount++;
			}
		}
		else
		{
			Lock.Lock(m_RenderLock);
			for(int i=0;i<(int)m_RootObjectList.GetCount();i++)
			{
				m_RootObjectList[i]->OnPrepareRenderData();
			}
			m_PrepareRenderDataFailCount=0;
		}

	}
}


bool CD3DSceneRender::CreateDepthTexture()
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(m_RenderLock);
	}
	SAFE_RELEASE(m_pDepthTexture);
	m_pDepthTexture=new CD3DTexture(m_pDevice->GetTextureManager());
	if(m_pDepthTexture->CreateTexture(m_pDevice->GetCreateParam().PresentParams.BackBufferWidth,
		m_pDevice->GetCreateParam().PresentParams.BackBufferHeight,
		D3DFMT_R32F,D3DUSAGE_RENDERTARGET,D3DPOOL_DEFAULT))
	{
		return true;
	}
	SAFE_RELEASE(m_pDepthTexture);		
	return false;
}

bool CD3DSceneRender::ClearDepthTexture()
{
	LPDIRECT3DSURFACE9 pRenderTarget=NULL;
	if(m_pDepthTexture->GetD3DTexture()->GetSurfaceLevel(0, &pRenderTarget)==D3D_OK)
	{
		HRESULT Ret=m_pDevice->GetD3DDevice()->ColorFill(pRenderTarget,NULL,0xFFFFFFFF);
		SAFE_RELEASE(pRenderTarget);
		return Ret==D3D_OK;
	}
	return false;
	
}

bool CD3DSceneRender::DeleteSceneObject(CD3DObject * pObject)
{
	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		if(m_SceneObjectList[i]==pObject)
		{
			m_SceneObjectList.Delete(i);
			break;
		}
	}

	bool IsDeleted=false;
	for(int i=(int)m_SceneRenderUnitList.GetCount()-1;i>=0;i--)
	{
		if(m_SceneRenderUnitList[i]->pObject==pObject)
		{
			for(int j=(int)m_SceneRenderUnitList[i]->TreeNodes.GetCount()-1;j>=0;j--)
			{
				TREE_NODE * pTree=m_SceneRenderUnitList[i]->TreeNodes[j];
				pTree->DelObj(m_SceneRenderUnitList[i]);
				UpdateTree(pTree);
			}
			if(m_SceneRenderUnitList[i]->TreeNodes.GetCount())
			{
				PrintD3DLog(_T("未能从所有树节点中清除此对象"));
			}
			//DeleteObjectFromTree(m_pSceneRoot,m_SceneRenderUnitList[i]);
			SAFE_DELETE(m_SceneRenderUnitList[i]);
			m_SceneRenderUnitList.Delete(i);		
			IsDeleted=true;
		}
		else
		{
			if(IsDeleted)
			{
				return true;
			}
		}
		
	}
	return false;
}

//int CD3DSceneRender::DeleteObjectFromTree(TREE_NODE * pNode,RENDER_OBJECT_INFO * pObjectInfo)
//{	
//	int DeleteCount=0;
//	for(UINT i=0;i<pNode->ObjectList.GetCount();i++)
//	{
//		if(pNode->ObjectList[i]==pObjectInfo)
//		{
//			pNode->ObjectList.Delete(i);
//			DeleteCount++;
//			break;
//		}
//	}
//	if(pNode->ChildCount)
//	{
//		for(UINT i=0;i<m_TreeMode;i++)
//		{
//			DeleteCount+=DeleteObject(pNode->Childs[i],pObjectInfo);
//		}
//	}
//	return DeleteCount;
//}

void CD3DSceneRender::DeleteSceneNode(TREE_NODE * pNode)
{
	if(pNode->ChildCount)
	{
		for(UINT i=0;i<m_TreeMode;i++)
		{
			DeleteSceneNode(pNode->Childs[i]);
			pNode->Childs[i]=NULL;
		}
	}
	SAFE_RELEASE(pNode->pBoundingFrame);
	if(pNode->pParent)
	{
		for(UINT i=0;i<m_TreeMode;i++)
		{
			if(pNode->pParent->Childs[i]==pNode)
			{
				pNode->pParent->Childs[i]=NULL;
				pNode->pParent->ChildCount--;
			}
		}
	}
	delete pNode;
}

//void CD3DSceneRender::DeleteNodeChilds(TREE_NODE * pNode,TREE_NODE * pParent)
//{
//	if(pNode!=pParent)
//	{
//		SAFE_RELEASE(pNode->pBoundingFrame);
//	}
//	if(pNode->ChildCount)
//	{
//		for(UINT i=0;i<m_TreeMode;i++)
//		{
//			DeleteNodeChilds(pNode->Childs[i],pParent);			
//		}
//		for(UINT i=0;i<m_TreeMode;i++)
//		{
//			SAFE_DELETE(pNode->Childs[i]);
//		}
//		pNode->ChildCount=0;
//		pNode->ChildCount=false;
//	}
//	else
//	{
//		if(pNode!=pParent)
//		{
//			for(UINT i=0;i<pNode->ObjectList.GetCount();i++)
//			{
//				pNode->ObjectList[i]->pObject->CheckFlag(0);
//				pParent->ObjectList.Add(pNode->ObjectList[i]);
//			}
//			pNode->ObjectList.Clear();
//			
//		}
//	}	
//}

bool CD3DSceneRender::AddObjectToTree(TREE_NODE * pTree,CD3DObject * pObject,bool BeWalkTree)
{
	CEasyTimerEx CostTimer;
	static UINT64 MaxCost=0;

	CostTimer.SaveTime();

	m_AddObjectSubMeshCount=0;
	m_AddObjectTreeWalkCount=0;
	m_AddObjectTreeAddCount=0;
	m_AddObjectTreeCutCount=0;
	m_AddObjectTreeUpdateCount=0;
	m_AddObjectObjectTestCount=0;

	for(UINT i=0;i<m_SceneObjectList.GetCount();i++)
	{
		if(m_SceneObjectList[i]==pObject)
		{
			PrintD3DLog(_T("对象[%s]已经存在于场景中"),
				pObject->GetName());
			return false;
		}
	}	

	pObject->OnPrepareRenderData();

	m_SceneObjectList.Add(pObject);
	

	for(int i=0;i<pObject->GetSubMeshCount();i++)
	{
		CD3DSubMesh * pSubMesh=pObject->GetSubMesh(i);
		CD3DSubMeshMaterial * pMaterial=pObject->GetSubMeshMaterial(i);

		if(pSubMesh&&pMaterial)
		{
			//pSubMesh->CheckValid();
			if(pMaterial->GetFX())
			{
				RENDER_OBJECT_INFO * pObjectInfo=new RENDER_OBJECT_INFO;
				pObjectInfo->ObjectType=RBT_OBJECT;
				if(pSubMesh->CheckProperty(CD3DSubMesh::SMF_IS_ANI_MESH)||(!pObject->CanDoSubMeshViewCull()))
				{
					//对于SkinMesh,使用对象包围盒作为SubMesh包围盒
					pObjectInfo->WorldBBox=(*pObject->GetBoundingBox())*pObject->GetWorldMatrix();
				}
				else
				{
					pObjectInfo->WorldBBox=pSubMesh->GetBoundingBoxWithTranform(pObject->GetWorldMatrix());
				}
				m_AddObjectSubMeshCount++;
				pObjectInfo->pObject=pObject;
				pObjectInfo->pSubMesh=pSubMesh;
				pObjectInfo->pMaterial=pMaterial;
				pObjectInfo->IsPreRenderd=false;
				pObjectInfo->IsRenderd=false;
				pObjectInfo->MergeRender=pObject->CheckFlag(CD3DObject::OBJECT_FLAG_CAN_MERGE_RENDER);
				m_SceneRenderUnitList.Add(pObjectInfo);
				if(BeWalkTree)
				{
					AddObjectToTree(pTree,pObjectInfo);
				}
				else
				{
					pTree->AddObj(pObjectInfo);						
				}
			}
			else
			{
				PrintD3DLog(_T("对象[%s]的SubMesh[%u]没有FX,无法加入渲染"),
					pObject->GetName(),i);
			}
		}
	}

	UINT64 CostTime=CostTimer.GetPastTime();
	if(CostTime>MaxCost)
	{
		MaxCost=CostTime;
		PrintD3DDebugLog(_T("CD3DSceneRender::AddSceneObject:最大耗时:%g[%u,%u,%u,%u,%u,%u]"),
			(double)MaxCost/CEasyTimerEx::TIME_UNIT_PER_SECOND,
			m_AddObjectSubMeshCount,m_AddObjectTreeWalkCount,m_AddObjectTreeAddCount,
			m_AddObjectTreeCutCount,m_AddObjectTreeUpdateCount,m_AddObjectObjectTestCount);
	}
	
	return true;
}

void CD3DSceneRender::AddObjectToTree(TREE_NODE * pTree,RENDER_OBJECT_INFO * pObjectInfo)
{		
	m_AddObjectTreeWalkCount++;
	if(pTree->ChildCount)
	{
		CD3DBoundingBox BBox=pObjectInfo->WorldBBox;
		bool IsCut=false;
		UINT IntersectCount=0;
		bool IntersectWitch[OTC_MAX]={0};

		for(UINT j=0;j<m_TreeMode;j++)
		{
			int Relation=CheckBBoxRelation(pTree->Childs[j]->NodeBox,BBox);
			if(Relation!=CD3DBoundingBox::RELATION_TYPE_OUT)
			{
				IntersectCount++;
				IntersectWitch[j]=true;
				if(IntersectCount>=m_TreeMode)
					break;
			}
		}
		if(IntersectCount)
		{
			if(IntersectCount<=m_TreeMode/2)
			{
				for(UINT j=0;j<OTC_MAX;j++)
				{
					if(IntersectWitch[j])
					{
						AddObjectToTree(pTree->Childs[j],pObjectInfo);
					}
				}		
				return;
			}					
		}
		else
		{
			PrintD3DLog(_T("CD3DSceneRender::AddObjectToTree:异常，加入节点的对象和节点的任何一个子结点都不相交"));
		}
	}	
	//未能加入子树，就加入当前节点
	
	m_AddObjectTreeAddCount++;
	pTree->AddObj(pObjectInfo);
	UpdateTree(pTree);
}

void CD3DSceneRender::UpdateTree(TREE_NODE * pNode)
{	
	if(pNode->Depth>m_MaxTreeDepth)
	{
		m_MaxTreeDepth=pNode->Depth;
		CD3DVector3 Size=pNode->NodeBox.GetSize();
		PrintD3DLog(_T("Depth=%u,XSize=%g,ZSize=%g"), m_MaxTreeDepth, Size.x, Size.z);
	}

	m_AddObjectTreeUpdateCount++;
	
	if(pNode->UpdateFlag==TNUF_ADD_OBJECT)
	{
		pNode->UpdateFlag=TNUF_NONE;
		if(pNode->ObjectList.GetCount()>=MINI_CUT_COUNT&&pNode->ChildCount==0)
		{		
			//尝试进行空间分割
			CD3DVector3 CutCenter=GetBoxCutCenter(pNode);
			if(CanBeCut(pNode,CutCenter))
			{
				m_AddObjectTreeCutCount++;
				CreateChilds(pNode,CutCenter,false);

				//将对象分散到子树中
				for(int i=(int)pNode->ObjectList.GetCount()-1;i>=0;i--)
				{
					CD3DBoundingBox BBox=pNode->ObjectList[i]->WorldBBox;
					bool IsCut=false;
					UINT IntersectCount=0;
					bool IntersectWitch[OTC_MAX]={0};

					for(UINT j=0;j<m_TreeMode;j++)
					{
						m_AddObjectObjectTestCount++;
						int Relation=CheckBBoxRelation(pNode->Childs[j]->NodeBox,BBox);
						if(Relation==CD3DBoundingBox::RELATION_TYPE_INCLUDE)
						{
							pNode->Childs[j]->AddObj(pNode->ObjectList[i]);
							IsCut=true;
							break;
						}
						else if(Relation==CD3DBoundingBox::RELATION_TYPE_INTERSECT||Relation==CD3DBoundingBox::RELATION_TYPE_BE_INCLUDE)
						{
							IntersectCount++;
							IntersectWitch[j]=true;
							if(IntersectCount>=m_TreeMode)
								break;
						}
					}
					if(IntersectCount)
					{
						if(IntersectCount<m_TreeMode)
						{
							for(UINT j=0;j<OTC_MAX;j++)
							{
								if(IntersectWitch[j])
								{
									pNode->Childs[j]->AddObj(pNode->ObjectList[i]);
									IsCut=true;
								}
							}						
						}					
					}
					if(IsCut)
					{
						pNode->DelObj(i);
					}
				}
			}

			//更新子结点
			if(pNode->ChildCount)
			{
				for(UINT i=0;i<m_TreeMode;i++)
				{
					UpdateTree(pNode->Childs[i]);
				}		
			}
		}

		
	}

	if(pNode->UpdateFlag==TNUF_DEL_OBJECT)
	{
		pNode->UpdateFlag=TNUF_NONE;
	
		//检查子节点是否可删除
		if(pNode->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				if(pNode->Childs[i]->IsFixed||pNode->Childs[i]->ChildCount||pNode->Childs[i]->ObjectList.GetCount())
				{
					return;
				}
			}	
			for(UINT i=0;i<m_TreeMode;i++)
			{
				DeleteSceneNode(pNode->Childs[i]);
			}
		}

		if(pNode->ObjectList.GetCount()==0&&pNode->ChildCount==0&&pNode->pParent)
		{
			pNode->pParent->UpdateFlag=TNUF_DEL_OBJECT;
			UpdateTree(pNode->pParent);
		}
		
	}
}

void CD3DSceneRender::CreateChilds(TREE_NODE * pNode,CD3DVector3 CutCenter,bool IsFixed)
{		
	CD3DBoundingBox ChildBBoxs[OTC_MAX];
	CutBBox(pNode,CutCenter,ChildBBoxs);
	for(UINT i=0;i<m_TreeMode;i++)
	{
		pNode->Childs[i]=new TREE_NODE;
		pNode->Childs[i]->pParent=pNode;
		pNode->Childs[i]->Depth=pNode->Depth+1;		
		pNode->Childs[i]->NodeBox=ChildBBoxs[i];		
		pNode->Childs[i]->ObjectList.Create(8,128);
		pNode->Childs[i]->IsFixed=IsFixed;

		//节点有变动，更新相关数据
		if(m_ShowNodeFrame)
		{
			pNode->Childs[i]->pBoundingFrame=new CD3DBoundingFrame();
			pNode->Childs[i]->pBoundingFrame->SetDevice(m_pDevice);
			if(pNode->Childs[i]->IsFixed)
				pNode->Childs[i]->pBoundingFrame->CreateFromBBox(pNode->Childs[i]->NodeBox,0xFF00FF00);
			else
				pNode->Childs[i]->pBoundingFrame->CreateFromBBox(pNode->Childs[i]->NodeBox,0xFF0000FF);

			AddObject(pNode->Childs[i]->pBoundingFrame);			
		}
	}
	pNode->ChildCount=m_TreeMode;
}


void CD3DSceneRender::CutBBox(TREE_NODE * pNode,CD3DVector3 CutCenter,CD3DBoundingBox * pChildBBoxs)
{	
	if(m_TreeMode==QTC_MAX)
	{		
		for(UINT i=0;i<QTC_MAX;i++)
		{
			switch(i)
			{
			case QTC_FRONT_LEFT:		
				pChildBBoxs[i].m_Max.SetValue(CutCenter.x,				pNode->NodeBox.m_Max.y,		pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,	pNode->NodeBox.m_Min.y,		CutCenter.z);
				break;
			case QTC_FRONT_RIGHT:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,	pNode->NodeBox.m_Max.y,		pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min.SetValue(CutCenter.x,				pNode->NodeBox.m_Min.y,		CutCenter.z);
				break;
			case QTC_BACK_LEFT:
				pChildBBoxs[i].m_Max.SetValue(CutCenter.x,				pNode->NodeBox.m_Max.y,		CutCenter.z);
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,	pNode->NodeBox.m_Min.y,		pNode->NodeBox.m_Min.z);			
				break;
			case QTC_BACK_RIGHT:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,	pNode->NodeBox.m_Max.y,		CutCenter.z);
				pChildBBoxs[i].m_Min.SetValue(CutCenter.x,				pNode->NodeBox.m_Min.y,		pNode->NodeBox.m_Min.z);
				break;			
			}
		}
	}
	else
	{
		
		for(UINT i=0;i<OTC_MAX;i++)
		{
			switch(i)
			{
			case OTC_FRONT_LEFT_BOTTOM:		
				pChildBBoxs[i].m_Max.SetValue(CutCenter.x,CutCenter.y,pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,pNode->NodeBox.m_Min.y,CutCenter.z);
				break;
			case OTC_FRONT_RIGHT_BOTTOM:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,CutCenter.y,pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min.SetValue(CutCenter.x,pNode->NodeBox.m_Min.y,CutCenter.z);
				break;
			case OTC_BACK_LEFT_BOTTOM:
				pChildBBoxs[i].m_Max=CutCenter;
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,pNode->NodeBox.m_Min.y,pNode->NodeBox.m_Min.z);			
				break;
			case OTC_BACK_RIGHT_BOTTOM:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,CutCenter.y,CutCenter.z);
				pChildBBoxs[i].m_Min.SetValue(CutCenter.x,pNode->NodeBox.m_Min.y,pNode->NodeBox.m_Min.z);
				break;
			case OTC_FRONT_LEFT_TOP:	
				pChildBBoxs[i].m_Max.SetValue(CutCenter.x,pNode->NodeBox.m_Max.y,pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,CutCenter.y,CutCenter.z);
				break;
			case OTC_FRONT_RIGHT_TOP:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,pNode->NodeBox.m_Max.y,pNode->NodeBox.m_Max.z);
				pChildBBoxs[i].m_Min=CutCenter;
				break;
			case OTC_BACK_LEFT_TOP:
				pChildBBoxs[i].m_Max.SetValue(CutCenter.x,pNode->NodeBox.m_Max.y,CutCenter.z);
				pChildBBoxs[i].m_Min.SetValue(pNode->NodeBox.m_Min.x,CutCenter.y,pNode->NodeBox.m_Min.z);
				break;
			case OTC_BACK_RIGHT_TOP:
				pChildBBoxs[i].m_Max.SetValue(pNode->NodeBox.m_Max.x,pNode->NodeBox.m_Max.y,CutCenter.z);
				pChildBBoxs[i].m_Min.SetValue(CutCenter.x,CutCenter.y,pNode->NodeBox.m_Min.z);
				break;
			}
		}
	}
}

bool CD3DSceneRender::AddObject(CD3DObject * pObj,bool IsRecursive)
{
	{
		CAutoLockEx Lock;

		if(CD3DDevice::IsUseMultiThreadRender())
		{
			Lock.Lock(m_RenderLock);
		}

		pObj->Update(0);		
		pObj->OnPrepareRenderData();
		

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
			{
				m_ObjectList.Add(pObj);

				for(int i=0;i<pObj->GetSubMeshCount();i++)
				{
					CD3DSubMesh * pSubMesh=pObj->GetSubMesh(i);
				
					//if(pSubMesh)
					//{
					//	pSubMesh->CheckValid();
					//}
				}
			}
		}
		else if(pObj->IsKindOf(GET_CLASS_INFO(CD3DLight)))
		{
			AddLight((CD3DLight *)pObj);
		}

		if(pObj->IsKindOfFast(GET_CLASS_INFO(CD3DWOWWMOModel)))
		{
			AddWMOObject((CD3DWOWWMOModel *)pObj);
		}
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

bool CD3DSceneRender::AddBackGroundObject(CD3DObject * pObj,bool IsRecursive)
{
	{
		CAutoLockEx Lock;

		if(CD3DDevice::IsUseMultiThreadRender())
		{
			Lock.Lock(m_RenderLock);
		}

		pObj->Update(0);		
		pObj->OnPrepareRenderData();


		if(pObj->GetParent()==NULL)
			AddRootObject(pObj);

		pObj->SetRender(this);
		if(pObj->CanRender())
		{
			bool IsExist=false;
			for(UINT i=0;i<m_BackGroundObjectList.GetCount();i++)
			{
				if(m_BackGroundObjectList[i]==pObj)
				{
					IsExist=true;
					break;
				}
			}	
			if(!IsExist)
			{
				m_BackGroundObjectList.Add(pObj);

				for(int i=0;i<pObj->GetSubMeshCount();i++)
				{
					CD3DSubMesh * pSubMesh=pObj->GetSubMesh(i);

					//if(pSubMesh)
					//{
					//	pSubMesh->CheckValid();
					//}
				}
			}
		}
	}

	if(IsRecursive)
	{
		for(UINT i=0;i<pObj->GetChildCount();i++)
		{
			AddBackGroundObject(pObj->GetChildByIndex(i));
		}
	}

	return true;
}

bool CD3DSceneRender::CanBeCut(TREE_NODE * pNode,CD3DVector3 CutCenter)
{
	if(m_TreeDepthLimit>0)
	{
		if(pNode->Depth>=m_TreeDepthLimit)
			return false;
	}

	if(pNode->ObjectList.GetCount()>=MINI_CUT_COUNT)
	{
		CD3DBoundingBox ChildBBoxs[OTC_MAX];
		CutBBox(pNode,CutCenter,ChildBBoxs);

		int ChildCount=0;
		
		for(int i=(int)pNode->ObjectList.GetCount()-1;i>=0;i--)
		{
			for(UINT j=0;j<m_TreeMode;j++)
			{
				int Relation=CheckBBoxRelation(ChildBBoxs[j],pNode->ObjectList[i]->WorldBBox);
				if(Relation==CD3DBoundingBox::RELATION_TYPE_INCLUDE)
				{		
					ChildCount++;
					break;
				}
			}
		}
		return ChildCount>1;		
	}
	return false;
}

void CD3DSceneRender::RenderObject(CD3DObject * pObject,const CD3DFrustum& Frustum,bool NeedFrustumCheck)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(pObject->GetRenderLock());
	}
	
	pObject->AddFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED);	
	

	if(pObject->IsVisible())
	{

		//视锥裁减
	
		int BoxCut=2;

		CD3DBoundingBox BBox;
		CD3DBoundingBox * pBBox=NULL;

		
		if(NeedFrustumCheck)
		{
			m_ObjectCheckCount++;
			pBBox=pObject->GetBoundingBox();
			if(pBBox)
			{
				BBox=(*pBBox)*pObject->GetWorldMatrixR();
				BoxCut=Frustum.BoxLocation(BBox);
				if(BoxCut==0)
				{
					m_ObjectCutCount++;
					return;				
				}
			}
			else
			{
				m_ObjectCutCount++;
				return;	
			}
		}
		else
		{
			m_DirectRenderCount++;			
		}


		if(m_SizeCullSize>0)
		{
			if(pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWM2Model)))
			{
				if(pBBox==NULL)
				{
					pBBox=pObject->GetBoundingBox();
					BBox=(*pBBox)*pObject->GetWorldMatrixR();
				}
				CD3DVector3 Len;
				Len=BBox.m_Max-BBox.m_Min;
				CD3DVector3 Pos=(BBox.m_Max+BBox.m_Min)/2;
				Pos=Pos*m_pCamera->GetViewMatR();
				Len.z=Pos.z;
				Len*=m_pCamera->GetProjectMatR();

				if(Len.x<m_SizeCullSize&&Len.y<m_SizeCullSize)
				{
					return;
				}
			}
		}

		m_ObjectCount++;
		

		pObject->AddFlag(CD3DObject::OBJECT_FLAG_RENDERED);
		
		
		//pObject->OnPrepareRender(m_pDevice,NULL,NULL,m_LightList,m_pCamera);

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
					if(BoxCut==1&&pObject->CanDoSubMeshViewCull()&&(!pSubMesh->CheckProperty(CD3DSubMesh::SMF_IS_ANI_MESH)))
					{
						//对于对象部分可见的，进行SubMesh级裁减,SkinMesh除外
						CD3DBoundingBox SubMeshBBox=pSubMesh->GetBoundingBox();
						SubMeshBBox*=pObject->GetWorldMatrixR();
						if(!Frustum.BoxLocation(SubMeshBBox))
						{
							pSubMesh->SetCulled(true);
							continue;
						}
					}
					
					pSubMesh->SetCulled(false);
					if(pObject->CheckFlag(CD3DObject::OBJECT_FLAG_CAN_MERGE_RENDER))
					{
						m_MergeRenderSubMeshCount++;
					}
					if(pSubMesh->IsTransparent())
					{
						RENDER_SUBMESH_INFO BatchInfo;
						BatchInfo.pObj=pObject;
						BatchInfo.pSubMesh=pSubMesh;
						BatchInfo.pMaterial=pMaterial;									
						m_TransparentSubMeshList.Add(BatchInfo);
						continue;
					}					
					RENDER_SUBMESH_INFO BatchInfo;
					BatchInfo.pObj=pObject;
					BatchInfo.pSubMesh=pSubMesh;
					BatchInfo.pMaterial=pMaterial;							
					m_RenderBatchList.Add(BatchInfo);
					//pObject->OnPrepareRender(m_pDevice,pSubMesh,pMaterial,m_LightList,m_pCamera);
					//RenderSubMesh(pSubMesh,pMaterial->GetFX());				
				}					
			}
		}
	}
}

void CD3DSceneRender::RenderSubMesh(RENDER_OBJECT_INFO * pObjectInfo,const CD3DFrustum& Frustum,bool NeedFrustumCheck)
{
	CAutoLockEx Lock;

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		Lock.Lock(pObjectInfo->pObject->GetRenderLock());
	}

	pObjectInfo->IsPreRenderd=true;

	if(pObjectInfo->pObject->IsVisible()&&pObjectInfo->pSubMesh->IsVisible())
	{	
		if(NeedFrustumCheck)
		{
			//if(pObjectInfo->pObject->CanDoSubMeshViewCull())
			{
				m_ObjectCheckCount++;
				//进行SubMesh级裁减
				if(!Frustum.BoxLocation(pObjectInfo->WorldBBox))
				{				
					m_ObjectCutCount++;
					return;
				}
			}
		}
		else
		{
			m_DirectRenderCount++;
		}

		if(m_SizeCullSize>0)
		{
			if(pObjectInfo->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWM2Model)))
			{				
				CD3DVector3 Len;
				Len=pObjectInfo->WorldBBox.m_Max-pObjectInfo->WorldBBox.m_Min;
				CD3DVector3 Pos=(pObjectInfo->WorldBBox.m_Max+pObjectInfo->WorldBBox.m_Min)/2;
				Pos=Pos*m_pCamera->GetViewMatR();
				Len.z=Pos.z;
				Len*=m_pCamera->GetProjectMatR();

				if(Len.x<m_SizeCullSize&&Len.y<m_SizeCullSize)
				{
					return;
				}
			}
		}
		
		pObjectInfo->IsRenderd=true;
		if(pObjectInfo->MergeRender)
		{
			m_MergeRenderSubMeshCount++;
		}
		RENDER_SUBMESH_INFO BatchInfo;
		BatchInfo.pObj=pObjectInfo->pObject;
		BatchInfo.pSubMesh=pObjectInfo->pSubMesh;
		BatchInfo.pMaterial=pObjectInfo->pMaterial;
		if(pObjectInfo->pSubMesh->IsTransparent())
		{
			m_TransparentSubMeshList.Add(BatchInfo);			
		}					
		else
		{
			m_RenderBatchList.Add(BatchInfo);
		}
	}					
}

void CD3DSceneRender::RenderScene(TREE_NODE * pNode,const CD3DFrustum& Frustum,bool NeedCheck)
{
	if(NeedCheck)
	{
		m_TreeCheckCount++;

		for(UINT i=0;i<pNode->ObjectList.GetCount();i++)
		{			
			if((!pNode->ObjectList[i]->IsPreRenderd)&&
				(!pNode->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED)))
			{				
				if(pNode->ChildCount)
					m_BranchObjectCheckCount++;
				RenderSubMesh(pNode->ObjectList[i],Frustum,true);
			}
		}

		if(pNode->ChildCount)
		{
			int BoxCut=Frustum.BoxLocation(pNode->NodeBox);
			switch(BoxCut)
			{
			case CD3DFrustum::LR_OUT:
				m_TreeCutCount++;
				break;
			case CD3DFrustum::LR_INTERSECT:
				pNode->IsRendered=true;
				for(UINT i=0;i<m_TreeMode;i++)
				{
					RenderScene(pNode->Childs[i],Frustum,true);
				}
				break;
			case CD3DFrustum::LR_INCLUDE:
				pNode->IsRendered=true;
				for(UINT i=0;i<m_TreeMode;i++)
				{
					RenderScene(pNode->Childs[i],Frustum,false);
				}
				break;
			}
		}
	}
	else
	{
		pNode->IsRendered=true;

		for(UINT i=0;i<pNode->ObjectList.GetCount();i++)
		{
			if((!pNode->ObjectList[i]->IsPreRenderd)&&
				(!pNode->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_PREPARE_RENDERED)))
			{				
				RenderSubMesh(pNode->ObjectList[i],Frustum,false);
			}


		}
		if(pNode->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				RenderScene(pNode->Childs[i],Frustum,false);
			}
		}
	}
}

void CD3DSceneRender::DoBatchRender()
{
	CD3DObject * pObject=NULL;

	CD3DFX * pFX=NULL;

	UINT64 HashCode1,HashCode2;
	UINT64 OldHashCode1,OldHashCode2;
	

	OldHashCode1=0;
	OldHashCode2=0;

	for(UINT i=0;i<m_RenderBatchList.GetCount();i++)
	{
		RENDER_SUBMESH_INFO& BatchInfo=m_RenderBatchList[i];
		bool NeedSetObjectFXParam=false;

		BatchInfo.pMaterial->GetHashCode(HashCode1,HashCode2);

		

		if(pFX!=BatchInfo.pMaterial->GetFX())
		{
			if(pFX)
			{
				pFX->EndPass(0);
				pFX->End();
			}
			pFX=BatchInfo.pMaterial->GetFX();
			pFX->Begin();
			pFX->BeginPass(0);
			NeedSetObjectFXParam=true;
			m_FXApplyCount++;
		}

		if(HashCode1!=OldHashCode1||HashCode2!=OldHashCode2)
		{
			OldHashCode1=HashCode1;
			OldHashCode2=HashCode2;

			SetSubMeshMaterialFXParams(BatchInfo.pMaterial);
		}

		if(pObject!=BatchInfo.pObj)
		{

			if(CD3DDevice::IsUseMultiThreadRender())
			{
				if(pObject)
					pObject->GetRenderLock().Unlock();
			}
			pObject=BatchInfo.pObj;			
			pObject->AddFlag(CD3DObject::OBJECT_FLAG_RENDERED);
			if(CD3DDevice::IsUseMultiThreadRender())
			{

				pObject->GetRenderLock().Lock();
			}
			NeedSetObjectFXParam=true;
			m_ObjectCount++;
		}
		if(NeedSetObjectFXParam)
		{
			pObject->OnPrepareRender(this,pFX,m_LightList,m_pCamera);
		}

		BatchInfo.pObj->OnPrepareRenderSubMesh(this,pFX,BatchInfo.pSubMesh,BatchInfo.pMaterial,m_LightList,m_pCamera);
		pFX->CommitChanges();
		RenderSubMeshEx(BatchInfo.pSubMesh);
	}

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		if(pObject)
			pObject->GetRenderLock().Unlock();
	}

	if(pFX)
	{
		pFX->EndPass(0);
		pFX->End();
	}

	pObject=NULL;
	pFX=NULL;

	//CD3DObject * pObject=NULL;

	//for(UINT i=0;i<m_RenderBatchList.GetCount();i++)
	//{
	//	RENDER_BATCH_INFO& BatchInfo=m_RenderBatchList[i];

	//	if(pObject!=BatchInfo.pObj)
	//	{

	//		if(CD3DDevice::IsUseMultiThreadRender())
	//		{
	//			if(pObject)
	//				pObject->GetRenderLock().Unlock();
	//		}
	//		pObject=BatchInfo.pObj;			
	//		pObject->AddFlag(CD3DObject::OBJECT_FLAG_RENDERED);
	//		if(CD3DDevice::IsUseMultiThreadRender())
	//		{

	//			pObject->GetRenderLock().Lock();
	//		}

	//		pObject->OnPrepareRender(m_pDevice,NULL,NULL,m_LightList,m_pCamera);
	//	}

	//	BatchInfo.pObj->OnPrepareRender(m_pDevice,BatchInfo.pSubMesh,BatchInfo.pMaterial,m_LightList,m_pCamera);
	//	CD3DBaseRender::RenderSubMesh(BatchInfo.pSubMesh,BatchInfo.pMaterial->GetFX(),NULL);
	//}

	//if(CD3DDevice::IsUseMultiThreadRender())
	//{
	//	if(pObject)
	//		pObject->GetRenderLock().Unlock();
	//}

	//pObject=NULL;

	//再绘制透明的	

	OldHashCode1=0;
	OldHashCode2=0;

	for(UINT i=0;i<m_TransparentSubMeshList.GetCount();i++)
	{
		RENDER_SUBMESH_INFO& BatchInfo=m_TransparentSubMeshList[i];
		bool NeedSetObjectFXParam=false;

		BatchInfo.pMaterial->GetHashCode(HashCode1,HashCode2);

		assert(BatchInfo.pObj);
		assert(BatchInfo.pMaterial);
		assert(BatchInfo.pMaterial->GetFX());

		if(pFX!=BatchInfo.pMaterial->GetFX())
		{
			if(pFX)
			{
				pFX->EndPass(0);
				pFX->End();
			}
			pFX=BatchInfo.pMaterial->GetFX();
			pFX->Begin();
			pFX->BeginPass(0);
			NeedSetObjectFXParam=true;
			m_FXApplyCount++;
		}

		if(HashCode1!=OldHashCode1||HashCode2!=OldHashCode2)
		{
			OldHashCode1=HashCode1;
			OldHashCode2=HashCode2;

			SetSubMeshMaterialFXParams(BatchInfo.pMaterial);
		}

		if(pObject!=BatchInfo.pObj)
		{

			if(CD3DDevice::IsUseMultiThreadRender())
			{
				if(pObject)
					pObject->GetRenderLock().Unlock();
			}
			pObject=BatchInfo.pObj;
			pObject->AddFlag(CD3DObject::OBJECT_FLAG_RENDERED);
			if(CD3DDevice::IsUseMultiThreadRender())
			{

				pObject->GetRenderLock().Lock();
			}
			NeedSetObjectFXParam=true;
			m_ObjectCount++;
		}
		if(NeedSetObjectFXParam)
		{
			pObject->OnPrepareRender(this,pFX,m_LightList,m_pCamera);
		}
		BatchInfo.pObj->OnPrepareRenderSubMesh(this,pFX,BatchInfo.pSubMesh,BatchInfo.pMaterial,m_LightList,m_pCamera);
		pFX->CommitChanges();
		RenderSubMeshEx(BatchInfo.pSubMesh);
	}

	if(CD3DDevice::IsUseMultiThreadRender())
	{
		if(pObject)
			pObject->GetRenderLock().Unlock();		
	}

	if(pFX)
	{
		pFX->EndPass(0);
		pFX->End();
	}

	pObject=NULL;
	pFX=NULL;

	//for(UINT i=0;i<m_TransparentSubMeshList.GetCount();i++)
	//{
	//	RENDER_BATCH_INFO& BatchInfo=m_TransparentSubMeshList[i];

	//	if(pObject!=BatchInfo.pObj)
	//	{

	//		if(CD3DDevice::IsUseMultiThreadRender())
	//		{
	//			if(pObject)
	//				pObject->GetRenderLock().Unlock();
	//		}
	//		pObject=BatchInfo.pObj;
	//		pObject->AddFlag(CD3DObject::OBJECT_FLAG_RENDERED);
	//		if(CD3DDevice::IsUseMultiThreadRender())
	//		{

	//			pObject->GetRenderLock().Lock();
	//		}

	//		pObject->OnPrepareRender(m_pDevice,NULL,NULL,m_LightList,m_pCamera);
	//	}
	//	BatchInfo.pObj->OnPrepareRender(m_pDevice,BatchInfo.pSubMesh,BatchInfo.pMaterial,m_LightList,m_pCamera);
	//	CD3DBaseRender::RenderSubMesh(BatchInfo.pSubMesh,BatchInfo.pMaterial->GetFX(),NULL);
	//}

	//if(CD3DDevice::IsUseMultiThreadRender())
	//{
	//	if(pObject)
	//		pObject->GetRenderLock().Unlock();		
	//}

}


void CD3DSceneRender::ClearTreeRenderFlag(TREE_NODE * pNode)
{
	pNode->IsRendered=false;
	for(UINT i=0;i<m_TreeMode;i++)
	{
		if(pNode->Childs[i])
			ClearTreeRenderFlag(pNode->Childs[i]);
	}
}

int CD3DSceneRender::CheckBBoxRelation(const CD3DBoundingBox& BBox1,const CD3DBoundingBox& BBox2)
{
	if(m_TreeMode==TREE_MODE_QD)
	{
		//四叉树模式不考虑上方向的Y轴
		if(BBox1.m_Max.x<BBox2.m_Min.x||
			BBox1.m_Max.z<BBox2.m_Min.z||
			BBox1.m_Min.x>BBox2.m_Max.x||
			BBox1.m_Min.z>BBox2.m_Max.z)
		{
			return CD3DBoundingBox::RELATION_TYPE_OUT;
		}

		if(BBox1.m_Max.x>=BBox2.m_Max.x&&
			BBox1.m_Max.z>=BBox2.m_Max.z&&
			BBox1.m_Min.x<=BBox2.m_Min.x&&
			BBox1.m_Min.z<=BBox2.m_Min.z)
		{
			return CD3DBoundingBox::RELATION_TYPE_INCLUDE;
		}

		if(BBox1.m_Max.x<=BBox2.m_Max.x&&
			BBox1.m_Max.z<=BBox2.m_Max.z&&
			BBox1.m_Min.x>=BBox2.m_Min.x&&
			BBox1.m_Min.z>=BBox2.m_Min.z)
		{
			return CD3DBoundingBox::RELATION_TYPE_BE_INCLUDE;
		}

		return CD3DBoundingBox::RELATION_TYPE_INTERSECT;
	}
	else
	{
		return BBox1.CheckRelation(BBox2);
	}
}

int BBoxRangeCompare(const void * b1,const void * b2)
{
	float Center1=(((CD3DSceneRender::BBOX_RANGE_INFO *)b1)->Min+((CD3DSceneRender::BBOX_RANGE_INFO *)b1)->Max)/2;
	float Center2=(((CD3DSceneRender::BBOX_RANGE_INFO *)b2)->Min+((CD3DSceneRender::BBOX_RANGE_INFO *)b2)->Max)/2;
	if(Center1==Center2)
		return 0;
	else if(Center1>Center2)
		return 1;
	else
		return -1;

}

int BBoxRangeCompareWithWeight(const void * b1,const void * b2)
{
	CD3DSceneRender::BBOX_RANGE_INFO * pb1=(CD3DSceneRender::BBOX_RANGE_INFO *)b1;
	CD3DSceneRender::BBOX_RANGE_INFO * pb2=(CD3DSceneRender::BBOX_RANGE_INFO *)b2;
	
	if(pb1->Weight==pb2->Weight)
		return 0;
	else if(pb1->Weight>pb2->Weight)
		return 1;
	else
		return -1;
	
}

CD3DVector3 CD3DSceneRender::GetBoxCutCenter(TREE_NODE * pNode)
{
	CD3DVector3 Center=(pNode->NodeBox.m_Max+pNode->NodeBox.m_Min)/2; 

	return Center;
	
	//CEasyArray<BBOX_RANGE_INFO> BBoxsX;
	//CEasyArray<BBOX_RANGE_INFO> BBoxsZ;
	//CEasyArray<BBOX_RANGE_INFO> BBoxsY;

	//BBoxsX.Create(pNode->ObjectList.GetCount(),pNode->ObjectList.GetCount());
	//BBoxsZ.Create(pNode->ObjectList.GetCount(),pNode->ObjectList.GetCount());
	//if(m_TreeMode==QTC_MAX)
	//{
	//	BBoxsY.Create(pNode->ObjectList.GetCount(),pNode->ObjectList.GetCount());
	//}

	//for(UINT i=0;i<pNode->ObjectList.GetCount();i++)
	//{
	//	CD3DBoundingBox BBox=pNode->ObjectList[i]->WorldBBox;
	//	BBOX_RANGE_INFO Range;
	//	Range.Min=BBox.m_Min.x;
	//	Range.Max=BBox.m_Max.x;
	//	Range.MinCount=0;
	//	Range.MaxCount=0;
	//	Range.Weight=0;
	//	if(Range.Min>pNode->NodeBox.m_Min.x&&
	//		Range.Max<pNode->NodeBox.m_Max.x)
	//		BBoxsX.Add(Range);
	//	Range.Min=BBox.m_Min.z;
	//	Range.Max=BBox.m_Max.z;
	//	if(Range.Min>pNode->NodeBox.m_Min.z&&
	//		Range.Max<pNode->NodeBox.m_Max.z)
	//		BBoxsZ.Add(Range);
	//	if(m_TreeMode==QTC_MAX)
	//	{
	//		Range.Min=BBox.m_Min.y;
	//		Range.Max=BBox.m_Max.y;
	//		if(Range.Min>pNode->NodeBox.m_Min.y&&
	//			Range.Max<pNode->NodeBox.m_Max.y)
	//			BBoxsY.Add(Range);
	//	}
	//}

	//if(BBoxsX.GetCount()>1)
	//{
	//	qsort(BBoxsX.GetBuffer(),BBoxsX.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompare);

	//	for(UINT i=0;i<BBoxsX.GetCount()-1;i++)
	//	{
	//		for(UINT j=i+1;j<BBoxsX.GetCount();j++)
	//		{
	//			if(BBoxsX[i].Max>BBoxsX[j].Min)
	//			{
	//				BBoxsX[i].MaxCount++;
	//				BBoxsX[j].MinCount++;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}			
	//	}	
	//	UINT MaxConverCount=1;
	//	for(int i=BBoxsX.GetCount()-1;i>0;i--)
	//	{
	//		for(int j=i-1;j>=0;j--)
	//		{
	//			if(BBoxsX[i].Min<BBoxsX[j].Max)
	//			{
	//				BBoxsX[i].MinCount++;
	//				BBoxsX[j].MaxCount++;
	//				if(BBoxsX[i].MinCount>MaxConverCount)
	//					MaxConverCount=BBoxsX[i].MinCount;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}
	//	}

	//	for(UINT i=1;i<BBoxsX.GetCount();i++)
	//	{
	//		BBoxsX[i].Weight=((float)abs((int)BBoxsX.GetCount()/2-(int)i)/(BBoxsX.GetCount()/2))+
	//			((float)BBoxsX[i].MinCount/MaxConverCount);
	//	}

	//	BBoxsX[0].Weight=1000.0f;

	//	qsort(BBoxsX.GetBuffer(),BBoxsX.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompareWithWeight);
	//	Center.x=BBoxsX[0].Min;
	//}


	//if(BBoxsZ.GetCount()>1)
	//{
	//	qsort(BBoxsZ.GetBuffer(),BBoxsZ.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompare);



	//	for(UINT i=0;i<BBoxsZ.GetCount()-1;i++)
	//	{
	//		for(UINT j=i+1;j<BBoxsZ.GetCount();j++)
	//		{
	//			if(BBoxsZ[i].Max>=BBoxsZ[j].Min)
	//			{
	//				BBoxsZ[i].MaxCount++;
	//				BBoxsZ[j].MinCount++;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}			
	//	}
	//	UINT MaxConverCount=1;
	//	for(int i=BBoxsZ.GetCount()-1;i>0;i--)
	//	{
	//		for(int j=i-1;j>=0;j--)
	//		{
	//			if(BBoxsZ[i].Min<BBoxsZ[j].Max)
	//			{
	//				BBoxsZ[i].MinCount++;
	//				BBoxsZ[j].MaxCount++;
	//				if(BBoxsZ[i].MinCount>MaxConverCount)
	//					MaxConverCount=BBoxsZ[i].MinCount;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}
	//	}

	//	for(UINT i=1;i<BBoxsZ.GetCount();i++)
	//	{
	//		BBoxsZ[i].Weight=((float)abs((int)BBoxsZ.GetCount()/2-(int)i)/(BBoxsZ.GetCount()/2))+
	//			((float)BBoxsZ[i].MinCount/MaxConverCount);
	//	}

	//	BBoxsZ[0].Weight=1000.0f;

	//	qsort(BBoxsZ.GetBuffer(),BBoxsZ.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompareWithWeight);

	//	Center.z=BBoxsZ[0].Min;
	//}

	//if(BBoxsY.GetCount()>1)
	//{
	//	qsort(BBoxsY.GetBuffer(),BBoxsY.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompare);



	//	for(UINT i=0;i<BBoxsY.GetCount()-1;i++)
	//	{
	//		for(UINT j=i+1;j<BBoxsY.GetCount();j++)
	//		{
	//			if(BBoxsY[i].Max>=BBoxsY[j].Min)
	//			{
	//				BBoxsY[i].MaxCount++;
	//				BBoxsY[j].MinCount++;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}			
	//	}
	//	UINT MaxConverCount=1;
	//	for(int i=BBoxsY.GetCount()-1;i>0;i--)
	//	{
	//		for(int j=i-1;j>=0;j--)
	//		{
	//			if(BBoxsY[i].Min<BBoxsY[j].Max)
	//			{
	//				BBoxsY[i].MinCount++;
	//				BBoxsY[j].MaxCount++;
	//				if(BBoxsY[i].MinCount>MaxConverCount)
	//					MaxConverCount=BBoxsY[i].MinCount;
	//			}
	//			else
	//			{
	//				break;
	//			}
	//		}
	//	}

	//	for(UINT i=1;i<BBoxsY.GetCount();i++)
	//	{
	//		BBoxsY[i].Weight=((float)abs((int)BBoxsY.GetCount()/2-(int)i)/(BBoxsY.GetCount()/2))+
	//			((float)BBoxsY[i].MinCount/MaxConverCount);
	//	}

	//	BBoxsY[0].Weight=1000.0f;

	//	qsort(BBoxsY.GetBuffer(),BBoxsY.GetCount(),sizeof(BBOX_RANGE_INFO),BBoxRangeCompareWithWeight);

	//	Center.y=BBoxsY[0].Min;
	//}
	//return Center;
	
}

bool CD3DSceneRender::HeightTest(TREE_NODE * pRoot,const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight)
{
	if(pRoot->NodeBox.CheckRelation(Pos)!=CD3DBoundingBox::RELATION_TYPE_OUT)
	{
		bool HaveHeight=false;
		bool IsADTTested=false;
		FLOAT FinalHeight=-1E38f,FinalWaterHeight=-1E38f;
		
		for(UINT i=0;i<pRoot->ObjectList.GetCount();i++)
		{
			if(pRoot->ObjectList[i]->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWWMOGroupModel))&&
				(!pRoot->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_CHECKED)))
			{
				CD3DWOWWMOGroupModel * pModel=(CD3DWOWWMOGroupModel *)pRoot->ObjectList[i]->pObject;
				FLOAT H,WH;
				m_HeightTestCount++;
				if(pModel->GetHeightByXZ(Pos,MinHeight,MaxHeight,H,WH))
				{
					HaveHeight=true;
					if(H>FinalHeight)
						FinalHeight=H;
					if(WH>FinalWaterHeight)
						FinalWaterHeight=WH;
				}
			}
			else if((!IsADTTested)&&pRoot->ObjectList[i]->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWADTModel)))
			{
				CD3DWOWADTModel * pModel=(CD3DWOWADTModel *)pRoot->ObjectList[i]->pObject;
				FLOAT H,WH;
				m_HeightTestCount++;
				if(pModel->GetHeightByXZ(Pos,MinHeight,MaxHeight,H,WH,false))
				{
					IsADTTested=true;
					HaveHeight=true;
					if(H>FinalHeight)
						FinalHeight=H;
					if(WH>FinalWaterHeight)
						FinalWaterHeight=WH;
				}
			}
			else if(pRoot->ObjectList[i]->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWM2Model))&&
				pRoot->ObjectList[i]->pSubMesh->CheckProperty(CD3DSubMesh::SMF_HAVE_COLLIDE))
			{
				FLOAT H,WH;
				m_HeightTestCount++;
				CD3DMatrix Mat=pRoot->ObjectList[i]->pObject->GetWorldMatrix();
				if(pRoot->ObjectList[i]->pSubMesh->GetHeightByXZ(Mat,Pos,MinHeight,MaxHeight,H,WH))
				{
					HaveHeight=true;						
					if(H>FinalHeight)
						FinalHeight=H;					
					if(WH>FinalWaterHeight)
						FinalWaterHeight=WH;
				}
			}
		}
		if(pRoot->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				FLOAT H,WH;
				if(HeightTest(pRoot->Childs[i],Pos,MinHeight,MaxHeight,H,WH))
				{				
					HaveHeight=true;
					if(H>FinalHeight)
						FinalHeight=H;
					if(WH>FinalWaterHeight)
						FinalWaterHeight=WH;
				}
			}	
		}

		if(HaveHeight)
		{
			Height=FinalHeight;
			WaterHeight=FinalWaterHeight;
		}
		return HaveHeight;
	}
	return false;
}

bool CD3DSceneRender::RayIntersect(TREE_NODE * pRoot,const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode)
{
	if(pRoot->NodeBox.CheckRelationRay(Point,Dir)==CD3DBoundingBox::RELATION_TYPE_INTERSECT)
	{
		bool IsIntersect=false;
		FLOAT FinalDis;
		CD3DVector3 P;
		FLOAT D=0,A;
		CD3DObject * pObj=NULL;

		for(UINT i=0;i<pRoot->ObjectList.GetCount();i++)
		{
			if(pRoot->ObjectList[i]->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWWMOGroupModel))&&
				(!pRoot->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_CHECKED)))
			{
				pRoot->ObjectList[i]->pObject->AddFlag(CD3DObject::OBJECT_FLAG_CHECKED);
				CD3DWOWWMOGroupModel * pModel=(CD3DWOWWMOGroupModel *)pRoot->ObjectList[i]->pObject;
				m_RayIntersectCount++;
				if(pModel->RayIntersect(Point,Dir,P,D,A,TestMode,false))
				{
					if(IsIntersect)
					{
						if(D<FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pModel;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pModel;
						FinalDis=D;
					}
				}				
			}			
			else if(CanTestCollide(pRoot->ObjectList[i]->pSubMesh->GetProperty(),TestMode))
			{
				CD3DMatrix Mat=pRoot->ObjectList[i]->pObject->GetWorldMatrix();
				CD3DMatrix MatInv=pRoot->ObjectList[i]->pObject->GetWorldMatrix().GetInverse();
				CD3DVector3 RayPos=Point*MatInv;
				CD3DVector3 RayDir=Dir*MatInv.GetRotation();
				RayDir.Normalize();
				
				m_RayIntersectCount++;
				if(pRoot->ObjectList[i]->pSubMesh->RayIntersect(RayPos,RayDir,P,D,A,false))
				{
					P=P*Mat;
					D=(P-Point).Length();
					if(IsIntersect)
					{
						if(D<FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pRoot->ObjectList[i]->pObject;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pRoot->ObjectList[i]->pObject;
						FinalDis=D;
					}
				}
			}
		}
		if(pRoot->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				if(RayIntersect(pRoot->Childs[i],Point,Dir,P,D,A,&pObj,TestMode))
				{				
					if(IsIntersect)
					{
						if(D<FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pObj;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pObj;
						FinalDis=D;
					}
				}
			}	
		}
		
		return IsIntersect;
	}
	return false;
}

bool CD3DSceneRender::LineIntersect(TREE_NODE * pRoot,const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode)
{
	
	FLOAT FinalDis;
	bool IsIntersect=false;
	CD3DVector3 P;
	FLOAT D=0,A;
	CD3DObject * pObj=NULL;

	if(pRoot->NodeBox.LineIntersect(StartPoint,EndPoint,P,D))
	{
		for(UINT i=0;i<pRoot->ObjectList.GetCount();i++)
		{
			if(pRoot->ObjectList[i]->pObject->IsKindOfFast(GET_CLASS_INFO(CD3DWOWWMOGroupModel))&&
				(!pRoot->ObjectList[i]->pObject->CheckFlag(CD3DObject::OBJECT_FLAG_CHECKED)))
			{
				pRoot->ObjectList[i]->pObject->AddFlag(CD3DObject::OBJECT_FLAG_CHECKED);
				CD3DWOWWMOGroupModel * pModel=(CD3DWOWWMOGroupModel *)pRoot->ObjectList[i]->pObject;
				m_LineIntersectCount++;
				if(pModel->LineIntersect(StartPoint,EndPoint,P,D,A,TestMode))
				{
					if(IsIntersect)
					{
						if(D<=FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pModel;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pModel;
						FinalDis=D;
					}
					
				}
			}			
			else if(CanTestCollide(pRoot->ObjectList[i]->pSubMesh->GetProperty(),TestMode))
			{
				CD3DMatrix Mat=pRoot->ObjectList[i]->pObject->GetWorldMatrix();
				CD3DMatrix MatInv=pRoot->ObjectList[i]->pObject->GetWorldMatrix().GetInverse();
				CD3DVector3 Pos1=StartPoint*MatInv;
				CD3DVector3 Pos2=EndPoint*MatInv;				

				m_LineIntersectCount++;
				if(pRoot->ObjectList[i]->pSubMesh->LineIntersect(Pos1,Pos2,P,D,A))
				{
					P=P*Mat;
					D=(P-StartPoint).Length();
					if(IsIntersect)
					{
						if(D<=FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pRoot->ObjectList[i]->pObject;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pRoot->ObjectList[i]->pObject;
						FinalDis=D;
					}
					
				}
			}
		}
		if(pRoot->ChildCount)
		{
			for(UINT i=0;i<m_TreeMode;i++)
			{
				if(LineIntersect(pRoot->Childs[i],StartPoint,EndPoint,P,D,A,&pObj,TestMode))
				{				
					if(IsIntersect)
					{
						if(D<=FinalDis)
						{
							IntersectPoint=P;
							Distance=D;
							DotValue=A;
							if(ppIntersectObject)
								*ppIntersectObject=pObj;
							FinalDis=D;
						}
					}
					else
					{
						IsIntersect=true;
						IntersectPoint=P;
						Distance=D;
						DotValue=A;
						if(ppIntersectObject)
							*ppIntersectObject=pObj;
						FinalDis=D;
					}					
				}
			}	
		}

		return IsIntersect;
		
	}
	return false;
}

int CD3DSceneRender::SubMeshCompare(const void * s1,const void * s2)
{
	RENDER_SUBMESH_INFO * pBatchInfo1=(RENDER_SUBMESH_INFO *)s1;
	RENDER_SUBMESH_INFO * pBatchInfo2=(RENDER_SUBMESH_INFO *)s2;

	UINT64 HashCode11,HashCode12;
	UINT64 HashCode21,HashCode22;
	
	if(pBatchInfo1->pMaterial->GetFX()==pBatchInfo2->pMaterial->GetFX())
	{
		pBatchInfo1->pMaterial->GetHashCode(HashCode11,HashCode12);
		pBatchInfo2->pMaterial->GetHashCode(HashCode21,HashCode22);
		
		if(HashCode11==HashCode21&&HashCode12==HashCode22)
		{
			if(pBatchInfo1->pObj==pBatchInfo2->pObj)
			{
				return 0;
			}
			else if(pBatchInfo1->pObj>pBatchInfo2->pObj)
				return 1;
			else
				return -1;
		}
		else if(HashCode11>HashCode21||(HashCode11==HashCode21&&HashCode12>HashCode22))
			return 1;
		else
			return -1;
	}
	else if(pBatchInfo1->pMaterial->GetFX()>pBatchInfo2->pMaterial->GetFX())
		return 1;
	else
		return -1;
	}


void CD3DSceneRender::RenderSubMeshEx(CD3DSubMesh * pSubMesh)
{	
	m_SubMeshCount++;
	m_FaceCount+=pSubMesh->GetPrimitiveCountR();
	m_VertexCount+=pSubMesh->GetVertexCountR();

	if(pSubMesh->GetPrimitiveCountR()<=0)
		return;

	//pSubMesh->CheckValidR();

	//设置顶点结构定义
	if(pSubMesh->GetVertexFormat().pVertexDeclaration)
	{
		m_pDevice->GetD3DDevice()->SetVertexDeclaration(pSubMesh->GetVertexFormat().pVertexDeclaration);
	}
	else
	{
		m_pDevice->GetD3DDevice()->SetFVF(pSubMesh->GetVertexFormat().FVF);
	}

	D3DFORMAT IndexFormat=D3DFMT_INDEX16;
	if(pSubMesh->GetVertexFormat().IndexSize==sizeof(DWORD))
		IndexFormat=D3DFMT_INDEX32;


	//填充顶点数据
	if(pSubMesh->GetRenderBufferUsed()==CD3DSubMesh::BUFFER_USE_DX)
	{
		m_pDevice->GetD3DDevice()->SetStreamSource( 0, pSubMesh->GetDXVertexBuffer(), 0, pSubMesh->GetVertexFormat().VertexSize );
		if(pSubMesh->GetIndexCountR())
			m_pDevice->GetD3DDevice()->SetIndices(pSubMesh->GetDXIndexBuffer());
	}


	HRESULT hr;
	//提交渲染	

	
	
	if(pSubMesh->GetIndexCountR())
	{
		if(pSubMesh->GetRenderBufferUsed()==CD3DSubMesh::BUFFER_USE_DX)
		{
			hr=m_pDevice->GetD3DDevice()->DrawIndexedPrimitive((D3DPRIMITIVETYPE)pSubMesh->GetPrimitiveType(),
				pSubMesh->GetVertexStartR(),pSubMesh->GetVertexStartR(),pSubMesh->GetVertexCountR(),
				pSubMesh->GetIndexStartR(),pSubMesh->GetPrimitiveCountR());
		}
		else
		{
			//PrintD3DLog("%d",pSubMesh->GetIndexStartR());

			hr=m_pDevice->GetD3DDevice()->DrawIndexedPrimitiveUP((D3DPRIMITIVETYPE)pSubMesh->GetPrimitiveType(),
				pSubMesh->GetVertexStartR(),
				pSubMesh->GetVertexCountR()+pSubMesh->GetVertexStartR(),
				pSubMesh->GetPrimitiveCountR(),
				pSubMesh->GetIndexBufferR()+pSubMesh->GetVertexFormat().IndexSize*pSubMesh->GetIndexStartR(),
				IndexFormat,
				pSubMesh->GetVertexBufferR(),
				pSubMesh->GetVertexFormat().VertexSize);
		}
	}
	else
	{
		if(pSubMesh->GetRenderBufferUsed()==CD3DSubMesh::BUFFER_USE_DX)
		{
			hr=m_pDevice->GetD3DDevice()->DrawPrimitive( (D3DPRIMITIVETYPE)pSubMesh->GetPrimitiveType(), 
				pSubMesh->GetVertexStartR(), pSubMesh->GetPrimitiveCountR() );
		}
		else
		{
			hr=m_pDevice->GetD3DDevice()->DrawPrimitiveUP( (D3DPRIMITIVETYPE)pSubMesh->GetPrimitiveType(), 
				pSubMesh->GetPrimitiveCountR(), 
				pSubMesh->GetVertexBufferR()+pSubMesh->GetVertexFormat().VertexSize*pSubMesh->GetVertexStartR(),
				pSubMesh->GetVertexFormat().VertexSize );
		}
	}
				
}

bool CD3DSceneRender::CanTestCollide(UINT64 SubMeshProperty,UINT TestMode)
{
	return (SubMeshProperty&CD3DSubMesh::SMF_HAVE_COLLIDE)||(TestMode&RITM_INCLUDE_NO_COLLIDE_FACE);
}

void CD3DSceneRender::SetSubMeshMaterialFXParams(CD3DSubMeshMaterial * pMaterial)
{
	if(pMaterial->GetFX())
	{
		pMaterial->GetFX()->SetColor(pMaterial->GetGlobalColorFXParamName(),pMaterial->GetGlobalColor());

		for(UINT i=0;i<pMaterial->GetTextureLayerCount();i++)
		{
			if(pMaterial->GetTextureProperty(i)&D3D_TEX_FLAG_DEPTH_TEX)
			{
				pMaterial->GetFX()->SetTexture(pMaterial->GetTextureFXParamName(i),m_pDepthTexture);
			}
			else
			{
				pMaterial->GetFX()->SetTexture(pMaterial->GetTextureFXParamName(i),pMaterial->GetTexture(i));
			}
			if(pMaterial->GetTextureProperty(i)&D3D_TEX_FLAG_UV_ANI)
			{
				pMaterial->GetFX()->SetMatrix(pMaterial->GetTextureMatFXParamName(i),pMaterial->GetTextureUVTransform(i));
			}
		}
		m_MaterialSetCount++;
	}
}

}