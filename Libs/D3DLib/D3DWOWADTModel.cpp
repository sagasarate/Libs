/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWADTModel.cpp                                       */
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


IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DWOWADTModel)

IMPLEMENT_CLASS_INFO(CD3DWOWADTModel,CD3DBaseStaticModel);

CD3DWOWADTModel::CD3DWOWADTModel(void)
{
	m_pModelResource=NULL;
	m_WaterColorLight=0xFF666666;
	m_WaterColorDark=0xFF191919;

}

CD3DWOWADTModel::~CD3DWOWADTModel(void)
{
	Destory();
}

void CD3DWOWADTModel::Destory()
{	
	CD3DBaseStaticModel::Destory();

	m_NeedUpdateChilds.Clear();
	m_M2ObjectList.Clear();
	m_WMOObjectList.Clear();
	SAFE_RELEASE(m_pModelResource);
	
}

bool CD3DWOWADTModel::Reset()
{
	return true;
}

bool CD3DWOWADTModel::Restore()
{
	return true;
}

bool CD3DWOWADTModel::LoadFromFile(LPCTSTR ModelFileName,bool IsBigAlphaMask,bool BeLoadObject)
{
	


	CD3DWOWADTModelResource* pResource=
		dynamic_cast<CD3DWOWADTModelResource*>(GetDevice()->GetObjectResourceManager()->GetResource(ModelFileName));
	if(!pResource)
	{
		pResource=new CD3DWOWADTModelResource(GetDevice()->GetObjectResourceManager());
		if(!pResource->LoadFromFile(ModelFileName,IsBigAlphaMask,BeLoadObject))
		{
			pResource->Release();
			return false;
		}
		if(!GetDevice()->GetObjectResourceManager()->AddResource(pResource,ModelFileName))
		{
			pResource->Release();
			return false;
		}
	}
	else
	{
		pResource->AddUseRef();
	}	


	SetName(ModelFileName);

	bool Ret=LoadFromResource(pResource,BeLoadObject);
	SAFE_RELEASE(pResource);
	return Ret;
}

bool CD3DWOWADTModel::LoadFromResource(CD3DWOWADTModelResource * pModelResource,bool BeLoadObject)
{
	if(GetDevice()==NULL)
		return false;

	Destory();

	m_pModelResource=pModelResource;	
	pModelResource->AddUseRef();
	

	if(BeLoadObject)
	{
		if(!LoadObjects())
			return false;

		CheckNeedUpdateObjects();
	}
	else
	{
		BuildBoundingBox();
	}

	m_IsRenderDataChanged=true;
	return true;
}

void CD3DWOWADTModel::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DBaseStaticModel::PickResource(pResourceManager,Param);
	if(m_pModelResource)
	{
		m_pModelResource->PickResource(pResourceManager);
		pResourceManager->AddResource(m_pModelResource);
	}
}

bool CD3DWOWADTModel::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	SAFE_RELEASE(m_pBoundingFrame);

	if(!CD3DBaseStaticModel::ToSmartStruct(Packet,pResourceManager,Param|OPP_WITHOUT_CHILD))
		return false;	

	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			if((!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DWOWWMOModel)))&&
				(!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DWOWDoodadModel))))
			{
				UINT BufferSize;
				void * pBuffer=Packet.PrepareMember(BufferSize);
				CSmartStruct ChildPacket(pBuffer,BufferSize,true);
				if(!m_ChildList[i]->ToSmartStruct(ChildPacket,pResourceManager,Param))
					return false;
				if(!Packet.FinishMember(SST_TO_CHILD,ChildPacket.GetDataLen()))
					return false;
			}
		}
	}
	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWAM_MODEL_RESOURCE,m_pModelResource->GetName()));	

	return true;
}
bool CD3DWOWADTModel::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseStaticModel::FromSmartStruct(Packet,pResourceManager,Param))
		return false;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWAM_MODEL_RESOURCE:
			{
				LPCTSTR szResourceName=Value;
				CD3DWOWADTModelResource * pModelResource=(CD3DWOWADTModelResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DWOWADTModelResource));
				if(pModelResource)
				{
					if(!LoadFromResource(pModelResource))
						return false;
				}
			}
			break;	
		}
	}

	return true;
}

UINT CD3DWOWADTModel::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DBaseStaticModel::GetSmartStructSize(Param|OPP_WITHOUT_CHILD);

	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			if((!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DWOWWMOModel)))&&
				(!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DWOWDoodadModel))))
			{
				Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(m_ChildList[i]->GetSmartStructSize(Param));
			}
		}
	}

	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(m_pModelResource->GetNameLength());

	return Size;
}

void CD3DWOWADTModel::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	CD3DMatrix Mat=GetWorldMatrixR();
	pFX->SetMatrix("WorldMatrix",Mat);


	Mat=GetWorldMatrixR()*pCamera->GetViewMatR();
	pFX->SetMatrix("WorldViewMatrix",Mat);

	if(pRender->IsKindOfFast(GET_CLASS_INFO(CD3DSceneRender)))
	{
		pFX->SetColor("WaterColorLight",((CD3DSceneRender *)pRender)->GetWaterColorLight());
		pFX->SetColor("WaterColorDark",((CD3DSceneRender *)pRender)->GetWaterColorDark());
		pFX->SetColor("ShadowColor",((CD3DSceneRender *)pRender)->GetShadowColor());
	}
	else
	{
		pFX->SetColor("WaterColorLight",m_WaterColorLight);
		pFX->SetColor("WaterColorDark",m_WaterColorDark);
	}
	
}
void CD3DWOWADTModel::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	if(pSubMesh->IsSelected())			
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}

	//if(pSubMesh->GetProperty()&CD3DWOWADTModelResource::SMP_IS_WATER)
	//{
	//	pFX->SetTexture("TexLay0",pMaterial->GetTexture(0));
	//	pFX->SetTexture("TexLay1",((CD3DSceneRender *)pRender)->GetDepthTexture());
	//}
	//else
	//{
	//	for(UINT i=0;i<pMaterial->GetTextureLayerCount();i++)
	//	{
	//		char szTexName[64];
	//		sprintf_s(szTexName,64,"TexLay%d",i);
	//		if(pMaterial->GetTextureProperty(i)&CD3DWOWADTModelResource::TP_SHADOW_MAP)
	//		{
	//			pFX->SetTexture("TexShadow",pMaterial->GetTexture(i));
	//		}
	//		else
	//		{
	//			pFX->SetTexture(szTexName,pMaterial->GetTexture(i));
	//		}					
	//	}
	//}
	//if(pSubMesh->IsSelected())
	//{
	//	pFX->SetColor("GlobalColor",0xFFFF0000);
	//}
	//else
	//{
	//	pFX->SetColor("GlobalColor",0xFFFFFFFF);
	//}
}

void CD3DWOWADTModel::OnPrepareRenderData()
{
	if(m_IsRenderDataChanged)
	{
		{
			CAutoLock Lock(GetRenderLock());

			m_WorldMatrixR=m_WorldMatrix;
			for(int i=0;i<GetSubMeshCount();i++)
			{
				CD3DSubMesh * pSubMesh=GetSubMesh(i);
				if(pSubMesh)
				{
					pSubMesh->OnPrepareRenderData();
				}
			}		

			m_RenderDataUpdateCount++;
		}

		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			((CD3DObject *)m_ChildList[i])->OnPrepareRenderData();
		}
		
	}
	else
	{
		for(UINT i=0;i<m_NeedUpdateChilds.GetCount();i++)
		{
			m_NeedUpdateChilds[i]->OnPrepareRenderData();
		}
	}

	m_IsRenderDataChanged=false;
}

void CD3DWOWADTModel::Update(FLOAT Time)
{
	//CD3DBaseStaticModel::Update(Time);
	m_UpdateCount++;
	bool NeedUpdateAll=false;
	if(GetParent())
	{
		if(m_IsMatrixChanged||GetParent()->IsMatrixChanged())
		{
			m_WorldMatrix=m_LocalMatrix*GetParent()->GetWorldMatrix();
			NeedUpdateAll=true;
		}
	}
	else
	{
		if(m_IsMatrixChanged)
		{
			m_WorldMatrix=m_LocalMatrix;
			NeedUpdateAll=true;
		}
	}

	if(NeedUpdateAll)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			((CD3DObject *)m_ChildList[i])->Update(Time);
		}
	}
	else
	{
		for(UINT i=0;i<m_NeedUpdateChilds.GetCount();i++)
		{
			m_NeedUpdateChilds[i]->Update(Time);
		}
	}
		
	m_IsMatrixChanged=false;
}

int CD3DWOWADTModel::GetSubMeshCount()
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMeshCount();
	return 0;
}
CD3DSubMesh * CD3DWOWADTModel::GetSubMesh(UINT index)
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMesh(index);
	return NULL;
}
CD3DSubMeshMaterial * CD3DWOWADTModel::GetSubMeshMaterial(UINT index)
{
	if(m_pModelResource)
		return &(m_pModelResource->GetSubMesh(index)->GetMaterial());
	return NULL;
}

CD3DBoundingBox * CD3DWOWADTModel::GetBoundingBox()
{	
	return &m_BoundingBox;
}
CD3DBoundingSphere * CD3DWOWADTModel::GetBoundingSphere()
{
	if(m_pModelResource)
		return m_pModelResource->GetBoundingSphere();
	return NULL;
}

bool CD3DWOWADTModel::GetHeightByXZ(const CD3DVector3& Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight,bool IncludeChild)
{
	if(GetBoundingBox())
	{		
		CD3DBoundingBox WMOBBox=(*GetBoundingBox())*GetWorldMatrix();
		if(WMOBBox.m_Min.x<=Pos.x&&Pos.x<=WMOBBox.m_Max.x&&
			WMOBBox.m_Min.z<=Pos.z&&Pos.z<=WMOBBox.m_Max.z)
		{
			FLOAT FinalHeight=WMOBBox.m_Min.y,FinalWaterHeight=WMOBBox.m_Min.y;
			bool HaveHeight=false;
			if(m_pModelResource)
			{
				FLOAT ADTHeight,ADTWaterHeight;
				CD3DVector3 LocalPos=Pos*GetWorldMatrix().GetInverse();
				if(m_pModelResource->GetHeightByXZ(LocalPos.x,LocalPos.z,ADTHeight,ADTWaterHeight))
				{
					//PrintSystemLog(0,"%g,%s",ADTHeight,m_pModelResource->GetName());
					if(ADTHeight<=MaxHeight)
					{
						HaveHeight=true;
						if(ADTHeight>FinalHeight)
							FinalHeight=ADTHeight;
						if(ADTWaterHeight>FinalWaterHeight)
							FinalWaterHeight=ADTWaterHeight;
					}
				}
			}
			if(IncludeChild)
			{
				MinHeight=WMOBBox.m_Min.y;
				if(HaveHeight)
				{
					MinHeight=FinalHeight;
				}

				for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
				{
					FLOAT WMOHeight,WMOWaterHeight;
					if(m_WMOObjectList[i]->GetHeightByXZ(Pos,WMOBBox.m_Min.y,MaxHeight,WMOHeight,WMOWaterHeight))
					{
						//PrintSystemLog(0,"%g,%s",WMOHeight,m_WMOObjectList[i]->GetModelResource()->GetName());
						HaveHeight=true;
						if(WMOHeight>FinalHeight)
							FinalHeight=WMOHeight;
						if(WMOWaterHeight>FinalWaterHeight)
							FinalWaterHeight=WMOWaterHeight;
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
	}
	return false;
}


bool CD3DWOWADTModel::LoadObjects()
{
	for(UINT i=0;i<m_pModelResource->GetM2ObjectCount();i++)
	{
		CD3DWOWADTModelResource::M2_OBJECT_INFO * pInfo=m_pModelResource->GetM2ObjectInfo(i);
		if(pInfo)
		{
			CD3DWOWDoodadModel * pObject=new CD3DWOWDoodadModel();
			pObject->SetDevice(GetDevice());
			if(pInfo->pModelResource)
			{
				if(!pObject->LoadFromResource(pInfo->pModelResource))
				{
					PrintSystemLog(0,_T("地图对象装载失败"));
					SAFE_RELEASE(pObject);
					return false;
				}		
			}
			else
			{
				if(!pObject->LoadFromFile(pInfo->ModelFilePath))
				{
					PrintSystemLog(0,_T("地图对象装载失败"));
					SAFE_RELEASE(pObject);
					return false;
				}
				pInfo->pModelResource=pObject->GetModelResource();
				pInfo->pModelResource->AddUseRef();
			}
			CD3DMatrix Mat=CD3DMatrix::FromScale(pInfo->Scale,pInfo->Scale,pInfo->Scale)*
				CD3DMatrix::FromRotationQuaternion(pInfo->Orientation)*
				CD3DMatrix::FromTranslation(pInfo->Position);
			pObject->SetLocalMatrix(Mat);
			pObject->SetParent(this);
			pObject->GetModelResource()->SetBoneAniCache(5,30);
			pObject->Play(0,0,0,true);
			CEasyString ObjectName;
			ObjectName.Format(_T("%u"),
				pInfo->ID);
			pObject->SetName(ObjectName);
			pObject->AddFlag((pInfo->Flag>>CD3DWOWADTModelResource::DOODAD_FLAG_SHIFT)&CD3DWOWADTModelResource::DOODAD_FLAG_MASK);
			m_M2ObjectList.Add(pObject);			
		}
	}

	for(UINT i=0;i<m_pModelResource->GetWMOObjectCount();i++)
	{
		CD3DWOWADTModelResource::WMO_OBJECT_INFO * pInfo=m_pModelResource->GetWMOObjectInfo(i);
		if(pInfo)
		{
			CD3DWOWWMOModel * pObject=new CD3DWOWWMOModel();
			pObject->SetDevice(GetDevice());
			if(pInfo->pModelResource)
			{
				if(!pObject->LoadFromResource(pInfo->pModelResource))
				{
					PrintSystemLog(0,_T("地图对象装载失败"));
					SAFE_RELEASE(pObject);
					return false;
				}
			}
			else
			{
				if(!pObject->LoadFromFile(pInfo->ModelFilePath))
				{
					PrintSystemLog(0,_T("地图对象装载失败"));
					SAFE_RELEASE(pObject);
					return false;
				}
				pInfo->pModelResource=pObject->GetModelResource();
				pInfo->pModelResource->AddUseRef();
			}
			CD3DMatrix Mat=CD3DMatrix::FromRotationQuaternion(pInfo->Orientation)*
				CD3DMatrix::FromTranslation(pInfo->Position);
			pObject->SetLocalMatrix(Mat);
			pObject->SetParent(this);			
			pObject->ShowDoodads(pInfo->DoodadSet);
			CEasyString ObjectName;
			ObjectName.Format(_T("%u"),
				pInfo->ID);
			pObject->SetName(ObjectName);
			pObject->AddFlag((pInfo->Flag>>CD3DWOWADTModelResource::DOODAD_FLAG_SHIFT)&CD3DWOWADTModelResource::DOODAD_FLAG_MASK);
			
			m_WMOObjectList.Add(pObject);			
		}
	}	

	BuildBoundingBox();

	return true;
}

void CD3DWOWADTModel::BuildBoundingBox()
{
	m_BoundingBox=*m_pModelResource->GetBoundingBox();
	for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
	{
		if(m_M2ObjectList[i]->GetBoundingBox()->IsValid())
			m_BoundingBox.Merge((*m_M2ObjectList[i]->GetBoundingBox())*m_M2ObjectList[i]->GetLocalMatrix());		
	}	
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		if(m_WMOObjectList[i]->GetBoundingBox()->IsValid())
			m_BoundingBox.Merge((*m_WMOObjectList[i]->GetBoundingBox())*m_WMOObjectList[i]->GetLocalMatrix());
	}
}

void CD3DWOWADTModel::CheckNeedUpdateObjects()
{
	m_NeedUpdateChilds.Clear();
	for(UINT i=0;i<m_M2ObjectList.GetCount();i++)
	{
		if(m_M2ObjectList[i]->NeedUpdateAni())
		{
			m_NeedUpdateChilds.Add(m_M2ObjectList[i]);
		}
	}
	for(UINT i=0;i<m_WMOObjectList.GetCount();i++)
	{
		m_NeedUpdateChilds.Add(m_WMOObjectList[i]);
	}
}

}