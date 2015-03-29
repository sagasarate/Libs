/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2Model.cpp                                        */
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
	

IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DWOWM2Model)

IMPLEMENT_CLASS_INFO(CD3DWOWM2Model,CD3DBaseDynamicModel);
CD3DWOWM2Model::CD3DWOWM2Model(void)
{
	m_pModelResource=NULL;
	m_CurAnimationID=-1;
	m_CurSubAnimationID=-1;
	m_CurAnimationIndex=-1;
	m_CurAnimationLength=0;
	m_UseSoftSkinMesh=false;

	m_InterimStatus=IS_NONE;
	m_InterimTime=0;
	m_IsPlaying=false;
	m_IsLoop=false;
	m_PlayStartTime=-1;
	m_CurPlayTime=0;
	m_PlaySpeedRate=1.0f;


	//m_pActivedAttachments=NULL;	
}

CD3DWOWM2Model::~CD3DWOWM2Model(void)
{
	Destory();	
}


void CD3DWOWM2Model::Destory()
{
	DestoryModel();
	CD3DBaseDynamicModel::Destory();
}

void CD3DWOWM2Model::DestoryModel()
{
	for(UINT i=0;i<m_SubMeshListForSoftSkinMesh.GetCount();i++)
	{
		SAFE_RELEASE(m_SubMeshListForSoftSkinMesh[i]);
	}
	m_SubMeshListForSoftSkinMesh.Clear();
	//for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	//{
	//	if(m_BoneMatrices[i].pAttachObject)
	//	{	
	//		SAFE_RELEASE(m_BoneMatrices[i].pAttachObject);
	//	}
	//}
	m_BoneMatrices.Clear();
	m_AdditionalBoneMatrix.Clear();
	m_InterimBoneMatStart.Clear();
	m_InterimBoneMatEnd.Clear();	
	SAFE_RELEASE(m_pModelResource);
	m_CurAnimationID=-1;
	m_CurSubAnimationID=-1;
	m_CurAnimationIndex=-1;
	m_CurAnimationLength=0;
	m_UseSoftSkinMesh=false;
	m_InterimStatus=IS_NONE;
	m_InterimTime=0;
	m_IsPlaying=false;
	m_IsLoop=false;
	m_PlayStartTime=-1;
	m_CurPlayTime=0;
	m_PlaySpeedRate=1.0f;
}

bool CD3DWOWM2Model::Reset()
{
	return true;
}

bool CD3DWOWM2Model::Restore()
{
	return true;
}

bool CD3DWOWM2Model::Play(int AnimationID,int SubAnimationID,FLOAT InterimTime,bool IsLoop)
{
	if(m_pModelResource)
	{
		m_CurAnimationIndex=m_pModelResource->GetAnimationIndex(AnimationID,SubAnimationID);
		CD3DWOWM2ModelResource::ANIMATION_SEQUENCE *pAniInfo=m_pModelResource->GetAnimationInfoByIndex(m_CurAnimationIndex);
		if(pAniInfo)
		{
			m_CurAnimationID=AnimationID;
			m_CurSubAnimationID=SubAnimationID;
			
			m_CurAnimationLength=pAniInfo->Length/1000.0f;
			if(m_pModelResource->GetSubMeshCount())
				m_CurBoundingBox=pAniInfo->BoundingBox;
			else
			{
				m_CurBoundingBox.m_Min.SetValue(0,0,0);
				m_CurBoundingBox.m_Max.SetValue(1,1,1);
			}
			m_InterimTime=InterimTime;			
			return Play(IsLoop);
		}
	}
	return false;
}

bool CD3DWOWM2Model::Play(bool IsLoop)
{
	if(m_CurAnimationIndex>=0)
	{
		m_IsLoop=IsLoop;
		m_IsPlaying=true;
		m_PlayStartTime=-1.0f;
		m_CurPlayTime=0;		
		if(m_InterimTime>0.000001f)
		{
			m_InterimStatus=IS_INIT;
			//m_InterimBoneMatStart.Resize(m_BoneMatrices.GetCount());
			//for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
			//{
			//	m_InterimBoneMatStart[i]=m_BoneMatrices[i].Matrix;
			//}
		}
		else
		{
			m_InterimStatus=IS_NONE;
			//FetchAnimationFrames(0);
		}
		for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
		{
			if(m_BoneMatrices[i].pAttachObject)
			{
				if(m_BoneMatrices[i].pAttachObject->IsKindOf(GET_CLASS_INFO(CD3DBaseDynamicModel)))
				{
					((CD3DBaseDynamicModel *)m_BoneMatrices[i].pAttachObject)->Play(IsLoop);
				}
			}
		}
	}
	return false;
}
bool CD3DWOWM2Model::Stop()
{
	m_IsPlaying=false;
	for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	{
		if(m_BoneMatrices[i].pAttachObject)
		{
			if(m_BoneMatrices[i].pAttachObject->IsKindOf(GET_CLASS_INFO(CD3DBaseDynamicModel)))
			{
				//if(!m_BoneMatrices[i].HaveGlobalSequence)
					((CD3DBaseDynamicModel *)m_BoneMatrices[i].pAttachObject)->Stop();
			}
		}
	}
	return true;
}
bool CD3DWOWM2Model::IsPlaying()
{
	return m_IsPlaying;
}
void CD3DWOWM2Model::SetPlaySpeed(FLOAT Rate)
{
	m_PlaySpeedRate=Rate;
	for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	{
		if(m_BoneMatrices[i].pAttachObject)
		{
			if(m_BoneMatrices[i].pAttachObject->IsKindOf(GET_CLASS_INFO(CD3DBaseDynamicModel)))
			{				
				((CD3DBaseDynamicModel *)m_BoneMatrices[i].pAttachObject)->SetPlaySpeed(Rate);
			}			
		}
	}
}

bool CD3DWOWM2Model::AddBoneTrans(int KeyBone,CD3DMatrix& Matrix)
{
	int Bone=m_pModelResource->GetKeyBone(KeyBone);
	if(Bone>=0)
	{
		ADDITIONAL_BONE_MATRIX Info;
		Info.BoneID=Bone;
		Info.Matrix=Matrix;
		m_AdditionalBoneMatrix.Add(Info);
		return true;
	}
	return false;
}

void CD3DWOWM2Model::ClearAdditionalBoneTrans()
{
	m_AdditionalBoneMatrix.Clear();
}

bool CD3DWOWM2Model::LoadFromFile(LPCTSTR szModelFileName)
{
	if(GetDevice()==NULL)
		return false;

	CD3DWOWM2ModelResource* pResource=
		dynamic_cast<CD3DWOWM2ModelResource*>(GetDevice()->GetObjectResourceManager()->GetResource(szModelFileName));
	if(!pResource)
	{
		pResource=new CD3DWOWM2ModelResource(GetDevice()->GetObjectResourceManager());
		if(!pResource->LoadFromFile(szModelFileName))
		{
			pResource->Release();
			return false;
		}
		if(!GetDevice()->GetObjectResourceManager()->AddResource(pResource,szModelFileName))
		{
			pResource->Release();
			return false;
		}
	}
	else
	{
		pResource->AddUseRef();
	}
	bool Ret=LoadFromResource(pResource);
	SAFE_RELEASE(pResource);
	return Ret;
	
}

bool CD3DWOWM2Model::LoadFromResource(CD3DWOWM2ModelResource * pModelResource)
{
	if(pModelResource==NULL)
		return false;

	DestoryModel();

	m_pModelResource=pModelResource;	
	m_pModelResource->AddUseRef();

	m_BoneMatrices.Resize(m_pModelResource->GetBoneCount());
	m_BoneMatList.Resize(m_pModelResource->GetBoneCount());
	for(UINT i=0;i<m_pModelResource->GetBoneCount();i++)
	{
		m_BoneMatrices[i].Matrix.SetIdentity();
		m_BoneMatList[i].SetIdentity();
	}

	if(m_pModelResource->GetSkinMeshBoneCount())
	{
		m_UseSoftSkinMesh=true;
		PrepareSoftSkinMesh();
	}	
	else
	{
		m_UseSoftSkinMesh=false;
	}

	if(m_pModelResource->HasBoneAni()||m_pModelResource->HasMaterialAni())
	{
		AddFlag(CD3DObject::OBJECT_FLAG_CAN_MERGE_RENDER);
	}

	for(UINT i=0;i<m_pModelResource->GetParticleEmitterCount();i++)
	{
		CD3DWOWM2ModelResource::PARTICLE_EMITTER_BIND_INFO * pParticleEmitterInfo=m_pModelResource->GetParticleEmitterInfo(i);
		if(pParticleEmitterInfo)
		{
			m_BoneMatrices[pParticleEmitterInfo->BindBone].AttachmentType=CD3DWOWM2ModelResource::MAT_PARTICLE_EMITTER;
			m_BoneMatrices[pParticleEmitterInfo->BindBone].AttachmentID=i;			
			m_BoneMatrices[pParticleEmitterInfo->BindBone].AttachTransform=
				CD3DMatrix::FromTranslation(pParticleEmitterInfo->BindPosition);			
			CD3DBillBoardParticleEmitter * pParticleEmitter=new CD3DBillBoardParticleEmitter();

			pParticleEmitter->SetDevice(GetDevice());
			pParticleEmitter->LoadFromResource(pParticleEmitterInfo->pModelResource,0);
			pParticleEmitter->SetParent(this);
			pParticleEmitter->SetLocalMatrix(m_BoneMatrices[pParticleEmitterInfo->BindBone].AttachTransform);

			m_BoneMatrices[pParticleEmitterInfo->BindBone].pAttachObject=pParticleEmitter;			
		}
	}

	for(UINT i=0;i<m_pModelResource->GetRibbonEmitterCount();i++)
	{
		CD3DWOWM2ModelResource::RIBBON_EMITTER_BIND_INFO * pRibbonEmitterInfo=m_pModelResource->GetRibbonEmitterInfo(i);
		if(pRibbonEmitterInfo)
		{
			m_BoneMatrices[pRibbonEmitterInfo->BindBone].AttachmentType=CD3DWOWM2ModelResource::MAT_RIBBON_EMITTER;
			m_BoneMatrices[pRibbonEmitterInfo->BindBone].AttachmentID=i;
			m_BoneMatrices[pRibbonEmitterInfo->BindBone].AttachTransform=
				CD3DMatrix::FromTranslation(pRibbonEmitterInfo->BindPosition);
			CD3DRibbonEmitter * pRibbonEmitter=new CD3DRibbonEmitter();

			pRibbonEmitter->SetDevice(GetDevice());
			pRibbonEmitter->LoadFromResource(pRibbonEmitterInfo->pModelResource);
			pRibbonEmitter->SetParent(this);
			pRibbonEmitter->SetLocalMatrix(m_BoneMatrices[pRibbonEmitterInfo->BindBone].AttachTransform);

			m_BoneMatrices[pRibbonEmitterInfo->BindBone].pAttachObject=pRibbonEmitter;			
		}
	}

	m_CurBoundingBox=*m_pModelResource->GetBoundingBox();
	m_IsRenderDataChanged=true;
	return true;
}

bool CD3DWOWM2Model::LoadFromXFile(LPCTSTR szModelFileName)
{
	if(GetDevice()==NULL)
		return false;

	CD3DWOWM2ModelResource* pResource=
		dynamic_cast<CD3DWOWM2ModelResource*>(GetDevice()->GetObjectResourceManager()->GetResource(szModelFileName));
	if(!pResource)
	{
		pResource=new CD3DWOWM2ModelResource(GetDevice()->GetObjectResourceManager());
		if(!pResource->LoadFromXFile(szModelFileName))
		{
			pResource->Release();
			return false;
		}
		if(!GetDevice()->GetObjectResourceManager()->AddResource(pResource,szModelFileName))
		{
			pResource->Release();
			return false;
		}
	}
	else
	{
		pResource->AddUseRef();
	}	
	bool Ret=LoadFromResource(pResource);
	SAFE_RELEASE(pResource);
	return Ret;
}

bool CD3DWOWM2Model::ChangeSkin(UINT Index)
{
	D3DLOCK_FOR_OBJECT_ADD;
	return m_pModelResource->ChangeSkin(Index);
}


bool CD3DWOWM2Model::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWM2Model)))
		return false;

	Destory();
	

	if(!CD3DBaseDynamicModel::CloneFrom(pObject,Param))
		return false;
	

	CD3DWOWM2Model * pSrcObject=(CD3DWOWM2Model *)pObject;
	m_pModelResource=pSrcObject->m_pModelResource;
	if(m_pModelResource)
		m_pModelResource->AddUseRef();

	m_BoneMatrices=pSrcObject->m_BoneMatrices;	
	m_BoneMatList=pSrcObject->m_BoneMatList;
	m_InterimBoneMatStart=pSrcObject->m_InterimBoneMatStart;
	m_InterimBoneMatEnd=pSrcObject->m_InterimBoneMatEnd;
	m_InterimStatus=pSrcObject->m_InterimStatus;
	m_InterimTime=pSrcObject->m_InterimTime;
	m_UseSoftSkinMesh=pSrcObject->m_UseSoftSkinMesh;
	m_CurAnimationID=pSrcObject->m_CurAnimationID;
	m_CurSubAnimationID=pSrcObject->m_CurSubAnimationID;
	m_CurAnimationIndex=pSrcObject->m_CurAnimationIndex;
	m_CurAnimationLength=pSrcObject->m_CurAnimationLength;
	m_IsPlaying=pSrcObject->m_IsPlaying;
	m_IsLoop=pSrcObject->m_IsLoop;
	m_PlayStartTime=pSrcObject->m_PlayStartTime;
	m_CurPlayTime=pSrcObject->m_CurPlayTime;
	m_PlaySpeedRate=pSrcObject->m_PlaySpeedRate;
	m_CurBoundingBox=pSrcObject->m_CurBoundingBox;

	for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	{
		if(m_BoneMatrices[i].pAttachObject)
		{
			m_BoneMatrices[i].pAttachObject=(CD3DDummy *)GetChildByStorageIDRecursive(m_BoneMatrices[i].pAttachObject->GetStorageID());
		}
	}

	if(m_UseSoftSkinMesh)
		PrepareSoftSkinMesh();

	return true;
}

void CD3DWOWM2Model::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_pModelResource)
	{
		m_pModelResource->PickResource(pResourceManager);
		pResourceManager->AddResource(m_pModelResource);
	}
	for(UINT i=0;i<GetChildCount();i++)
	{
		if((!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DBillBoardParticleEmitter)))&&
			(!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DRibbonEmitter))))
		{
			GetChildByIndex(i)->PickResource(pResourceManager,Param);
		}
	}
}

CD3DWOWM2ModelResource * CD3DWOWM2Model::GetModelResource()
{
	return m_pModelResource;
}

CD3DDummy * CD3DWOWM2Model::EnableAttachment(UINT ID)
{
	CD3DWOWM2ModelResource::MODEL_ATTACHMENT * pAttachment=m_pModelResource->GetAttachment(ID);
	if(pAttachment)
	{
		if(m_BoneMatrices[pAttachment->Bone].pAttachObject==NULL)
		{
			CD3DBoundingBox BoundingBox;
			BoundingBox.m_Min=-0.1f;
			BoundingBox.m_Max=0.1f;
			m_BoneMatrices[pAttachment->Bone].AttachmentID=pAttachment->ID;
			m_BoneMatrices[pAttachment->Bone].AttachTransform=CD3DMatrix::FromTranslation(pAttachment->Position);
			m_BoneMatrices[pAttachment->Bone].AttachmentType=CD3DWOWM2ModelResource::MAT_DUMMY;
			CD3DDummy * pDummy=new CD3DDummy;			
			pDummy->SetName(pAttachment->Name);
			pDummy->SetDevice(GetDevice());			
			pDummy->SetBoundingBox(BoundingBox);
			pDummy->SetParent(this);
			pDummy->SetLocalMatrix(m_BoneMatrices[pAttachment->Bone].AttachTransform);
			m_BoneMatrices[pAttachment->Bone].pAttachObject=pDummy;
			if(GetRender())
			{
				GetRender()->AddObject(pDummy);
			}
		}
		return (CD3DDummy *)m_BoneMatrices[pAttachment->Bone].pAttachObject;
	}
	return NULL;
}

bool CD3DWOWM2Model::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	SAFE_RELEASE(m_pBoundingFrame);

	if(!CD3DBaseDynamicModel::ToSmartStruct(Packet,pResourceManager,Param|OPP_WITHOUT_CHILD))
		return false;	

	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			if((!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DBillBoardParticleEmitter)))&&
				(!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DRibbonEmitter))))
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

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMM_MODEL_RESOURCE,m_pModelResource->GetName()));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMM_CUR_ANI_ID,m_CurAnimationID));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMM_CUR_SUB_ANI_ID,m_CurSubAnimationID));
	for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	{
		if(m_BoneMatrices[i].pAttachObject&&m_BoneMatrices[i].AttachmentType==CD3DWOWM2ModelResource::MAT_DUMMY)
		{
			UINT BufferSize;
			void * pBuffer=Packet.PrepareMember(BufferSize);
			CSmartStruct SubPacket(pBuffer,BufferSize,true);

			CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AAI_ATTACHMENT_ID,m_BoneMatrices[i].AttachmentID));
			CHECK_SMART_STRUCT_ADD_AND_RETURN(SubPacket.AddMember(SST_AAI_DUMMY_STORAGE_ID,
				m_BoneMatrices[i].pAttachObject->GetStorageID()));

			if(!Packet.FinishMember(SST_D3DWMM_ATCIVE_ATTACHMENT,SubPacket.GetDataLen()))
				return false;
		}
	}

	
	return true;
}
bool CD3DWOWM2Model::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseDynamicModel::FromSmartStruct(Packet,pResourceManager,Param))
		return false;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWMM_MODEL_RESOURCE:
			{
				LPCTSTR szResourceName=Value;
				CD3DWOWM2ModelResource * pModelResource=(CD3DWOWM2ModelResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DWOWM2ModelResource));
				if(pModelResource)
				{
					LoadFromResource(pModelResource);					
				}
			}
			break;
		case SST_D3DWMM_CUR_ANI_ID:
			m_CurAnimationID=Value;
			break;
		case SST_D3DWMM_CUR_SUB_ANI_ID:
			m_CurSubAnimationID=Value;
			break;
		case SST_D3DWMM_ATCIVE_ATTACHMENT:
			{
				CSmartStruct AttachmentInfo=Value;
				UINT AttachmentID=AttachmentInfo.GetMember(SST_AAI_ATTACHMENT_ID);
				UINT StorageID=AttachmentInfo.GetMember(SST_AAI_DUMMY_STORAGE_ID);
				CD3DWOWM2ModelResource::MODEL_ATTACHMENT * pAttachment=m_pModelResource->GetAttachment(AttachmentID);
				if(pAttachment)
				{
					CD3DDummy * pDummy=(CD3DDummy *)GetChildByStorageIDRecursive(StorageID);
					if(pDummy)
					{
						CD3DBoundingBox BoundingBox;
						BoundingBox.m_Min=-0.1f;
						BoundingBox.m_Max=0.1f;
						pDummy->SetBoundingBox(BoundingBox);
						m_BoneMatrices[pAttachment->Bone].AttachmentID=pAttachment->ID;
						m_BoneMatrices[pAttachment->Bone].AttachTransform=
							CD3DMatrix::FromTranslation(pAttachment->Position);
						m_BoneMatrices[pAttachment->Bone].pAttachObject=pDummy;				
					}			
				}
			}
			break;
		}
	}
	
	
	Play(m_CurAnimationID,m_CurSubAnimationID,0,true);
	return true;
}

UINT CD3DWOWM2Model::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DBaseDynamicModel::GetSmartStructSize(Param|OPP_WITHOUT_CHILD);

	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(m_pModelResource->GetNameLength());
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_CurAnimationID));
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_CurSubAnimationID));
	
	for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
	{
		if(m_BoneMatrices[i].pAttachObject&&m_BoneMatrices[i].AttachmentType==CD3DWOWM2ModelResource::MAT_DUMMY)
		{
			Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_BoneMatrices[i].AttachmentID));
			Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_BoneMatrices[i].pAttachObject->GetStorageID()));
			Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(0);
		}
	}

	if((Param&OPP_WITHOUT_CHILD)==0)
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			if((!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DBillBoardParticleEmitter)))&&
				(!m_ChildList[i]->IsKindOf(GET_CLASS_INFO(CD3DRibbonEmitter))))
			{
				Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(m_ChildList[i]->GetSmartStructSize(Param));
			}
		}
	}
	return Size;
}

void CD3DWOWM2Model::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	////设置灯光
	//if(LightList.GetCount())
	//{		
	//	D3DLIGHT9	Light;
	//	char		szParamName[32];
	//	pFX->SetInt("LightCount",LightList.GetCount());
	//	for(UINT i=0;i<LightList.GetCount();i++)
	//	{
	//		LightList[i]->GetCurLight(Light);
	//		sprintf_s(szParamName,32,"LightType[%d]",i);
	//		pFX->SetInt(szParamName,Light.Type);
	//		sprintf_s(szParamName,32,"LightPos[%d]",i);
	//		pFX->SetVector(szParamName,CD3DVector3(Light.Position));
	//		sprintf_s(szParamName,32,"LightDir[%d]",i);
	//		pFX->SetVector(szParamName,CD3DVector3(Light.Direction));
	//		sprintf_s(szParamName,32,"LightAmbient[%d]",i);
	//		pFX->SetColor(szParamName,Light.Ambient);
	//		sprintf_s(szParamName,32,"LightDiffuse[%d]",i);
	//		pFX->SetColor(szParamName,Light.Diffuse);
	//		sprintf_s(szParamName,32,"LightSpecular[%d]",i);
	//		pFX->SetColor(szParamName,Light.Specular);
	//		sprintf_s(szParamName,32,"LightRange[%d]",i);
	//		pFX->SetFloat(szParamName,Light.Range);
	//		sprintf_s(szParamName,32,"LightAtn0[%d]",i);
	//		pFX->SetFloat(szParamName,Light.Attenuation0);
	//		sprintf_s(szParamName,32,"LightAtn1[%d]",i);
	//		pFX->SetFloat(szParamName,Light.Attenuation1);
	//		sprintf_s(szParamName,32,"LightAtn2[%d]",i);
	//		pFX->SetFloat(szParamName,Light.Attenuation2);
	//		//sprintf_s(szParamName,32,"LightFalloff[%d]",i);
	//		//pFX->SetFloat(szParamName,Light.Falloff);
	//		//sprintf_s(szParamName,32,"LightTheta[%d]",i);
	//		//pFX->SetFloat(szParamName,Light.Theta);
	//		//sprintf_s(szParamName,32,"LightPhi[%d]",i);
	//		//pFX->SetFloat(szParamName,Light.Phi);
	//	}

	//}
	////设置雾
	//CD3DSceneRender * pRender=(CD3DSceneRender *)GetRender();
	//if(pRender)
	//{
	//	pFX->SetColor("FogColor",pRender->GetFogColor());
	//	pFX->SetFloat("FogNear",pRender->GetFogNear());
	//	pFX->SetFloat("FogFar",pRender->GetFogFar());
	//}



	//pFX->SetMatrix("PrjMatrix",pCamera->GetProjectMatR());

	//pFX->SetVector("CameraPos",pCamera->GetWorldMatrixR().GetTranslation());
	//pFX->SetFloat("CameraNear",pCamera->GetNear());
	//pFX->SetFloat("CameraFar",pCamera->GetFar());




					

	//if(CD3DDevice::IsUseMultiThreadRender())
	//{
	//	UINT BoneNum=m_pModelResource->GetSkinMeshBoneCount();
	//	if(BoneNum>m_BoneMatListR.GetCount())
	//		BoneNum=m_BoneMatListR.GetCount();
	//	if((!m_UseSoftSkinMesh)&&BoneNum)
	//	{
	//		//设置骨骼矩阵
	//		pFX->SetMatrixArray("BoneMatrixArray",&(m_BoneMatListR[0]),BoneNum);
	//	}
	//}
	//else
	//{
	//	UINT BoneNum=m_pModelResource->GetSkinMeshBoneCount();
	//	if(BoneNum>m_BoneMatList.GetCount())
	//		BoneNum=m_BoneMatList.GetCount();
	//	if((!m_UseSoftSkinMesh)&&BoneNum)
	//	{
	//		//设置骨骼矩阵
	//		pFX->SetMatrixArray("BoneMatrixArray",&(m_BoneMatList[0]),BoneNum);
	//	}
	//}
}
void CD3DWOWM2Model::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	////设置材质
	//D3DMATERIAL9 * pD3DMaterial=&(pMaterial->GetMaterial());			
	//pFX->SetColor("MaterialAmbient",pD3DMaterial->Ambient);
	//pFX->SetColor("MaterialDiffuse",pD3DMaterial->Diffuse);
	//pFX->SetColor("MaterialSpecular",pD3DMaterial->Specular);
	//pFX->SetColor("MaterialEmissive",pD3DMaterial->Emissive);
	//pFX->SetFloat("MaterialPower",pD3DMaterial->Power);

	////设置纹理
	//for(UINT i=0;i<pMaterial->GetTextureLayerCount();i++)
	//{
	//	char szTexName[64];
	//	sprintf_s(szTexName,64,"TexLay%d",i);
	//	pFX->SetTexture(szTexName,pMaterial->GetTexture(i));
	//}			
	//UINT64 TextureFlag=pMaterial->GetTextureProperty(0)>>CD3DWOWM2ModelResource::TF_TEXTURE_FLAG_SHIFT;
	//if(TextureFlag&D3D_TEX_FLAG_UV_ANI)
	//{
	//	pFX->SetMatrix("UVMatrix0",pMaterial->GetTextureUVTransform(0));
	//}


	//设置全局色
	
	if(pSubMesh->IsSelected())			
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}

	CD3DMatrix WorldMat,WorldViewMat;			
	WorldMat=GetWorldMatrixR();
	WorldViewMat=WorldMat*pCamera->GetViewMatR();

	UINT64 RenderFlag=pSubMesh->GetProperty()>>CD3DWOWM2ModelResource::SMP_RENDER_FLAG_SHIFT;
	if(RenderFlag&D3D_RENDER_FLAG_BILL_BOARD)
	{
		WorldMat=WorldMat.GetScaleTranslation();
		WorldViewMat=WorldViewMat.GetScaleTranslation();
	}
	pFX->SetMatrix("WorldMatrix",WorldMat);
	pFX->SetMatrix("WorldViewMatrix",WorldViewMat);
}

int CD3DWOWM2Model::GetSubMeshCount()
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMeshCount();
	else
		return NULL;
	
}

CD3DSubMesh * CD3DWOWM2Model::GetSubMesh(UINT index)
{
	if(m_UseSoftSkinMesh)
	{
		if(index<m_SubMeshListForSoftSkinMesh.GetCount())
			return m_SubMeshListForSoftSkinMesh[index];
		else
			return NULL;
	}
	else
	{
		return GetOriginSubMesh(index);
	}
}

CD3DSubMeshMaterial  * CD3DWOWM2Model::GetSubMeshMaterial(UINT index)
{
	if(m_pModelResource)
	{
		CD3DSubMesh * pSubMesh=GetOriginSubMesh(index);
		if(pSubMesh)
		{
			return &(pSubMesh->GetMaterial());
		}
	}
	return NULL;
}

CD3DSubMesh * CD3DWOWM2Model::GetOriginSubMesh(UINT index)
{
	if(m_pModelResource)
		return m_pModelResource->GetSubMesh(index);
	else
		return NULL;	
}

CD3DBoundingBox * CD3DWOWM2Model::GetBoundingBox()
{	
	if(m_pModelResource)
	{			
		return &m_CurBoundingBox;
	}
	return NULL;
}
CD3DBoundingSphere * CD3DWOWM2Model::GetBoundingSphere()
{
	if(m_pModelResource)
		return m_pModelResource->GetBoundingSphere();
	return NULL;
}

bool CD3DWOWM2Model::CanDoSubMeshViewCull()
{
	return true;
}

void CD3DWOWM2Model::Update(FLOAT Time)
{

	//更新世界矩阵	

	m_UpdateCount++;
	if(GetParent())
	{
		if(m_IsMatrixChanged||GetParent()->IsMatrixChanged())
		{
			m_WorldMatrix=m_LocalMatrix*GetParent()->GetWorldMatrix();
			m_IsRenderDataChanged=true;
		}
	}
	else
	{
		if(m_IsMatrixChanged)
		{
			m_WorldMatrix=m_LocalMatrix;
			m_IsRenderDataChanged=true;
		}
	}
	
	if(m_IsPlaying&&m_PlayStartTime<0)	
	{
		m_PlayStartTime=Time;
	}	
	if(m_PlayStartTime>=0)
	{
		m_CurPlayTime=(Time-m_PlayStartTime)*m_PlaySpeedRate;
	}

	if(!m_IsLoop&&m_IsPlaying&&m_CurPlayTime>=m_CurAnimationLength)
	{
		Stop();
	}	
	if(m_IsPlaying&&IsVisible()&&(!CheckFlag(CD3DObject::OBJECT_FLAG_CULLED)))
	{
		UINT CurTime=(UINT)(m_CurPlayTime*1000.0f);

		if(m_pModelResource->HasBoneAni())
		{
			if(m_InterimStatus==IS_INIT)
			{		
				m_InterimBoneMatStart.Resize(m_BoneMatrices.GetCount());
				for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
				{
					m_InterimBoneMatStart[i]=m_BoneMatrices[i].Matrix;
				}
			}

			if(m_InterimStatus!=IS_DOING)
			{
				FetchAnimationFrames(CurTime);
				for(UINT i=0;i<m_AdditionalBoneMatrix.GetCount();i++)
				{
					m_BoneMatrices[m_AdditionalBoneMatrix[i].BoneID].Matrix=m_AdditionalBoneMatrix[i].Matrix*m_BoneMatrices[m_AdditionalBoneMatrix[i].BoneID].Matrix;
				}
			}

			if(m_InterimStatus==IS_INIT)
			{		
				m_InterimStatus=IS_DOING;
				m_InterimBoneMatEnd.Resize(m_BoneMatrices.GetCount());
				for(UINT i=0;i<m_BoneMatrices.GetCount();i++)
				{
					m_InterimBoneMatEnd[i]=m_BoneMatrices[i].Matrix;
				}
			}


			if(m_InterimStatus==IS_DOING)
			{
				for(UINT i=0;i<m_BoneMatList.GetCount();i++)
				{
					FLOAT S=0.0f;					
					S=m_CurPlayTime/m_InterimTime;
					if(S>1.0f)
						S=1.0f;
					if(S<0.0f)
						S=0.0f;
	

					CD3DVector3 ScalingStart,ScalingEnd;
					CD3DQuaternion RotationStart,RotationEnd;
					CD3DVector3 TranslationStart,TranslationEnd;

					m_InterimBoneMatStart[i].GetTransformation(&ScalingStart,&RotationStart,&TranslationStart);
					m_InterimBoneMatEnd[i].GetTransformation(&ScalingEnd,&RotationEnd,&TranslationEnd);					
					
					CD3DVector3 Scaling=CD3DVector3::Lerp(ScalingStart,ScalingEnd,S);
					CD3DQuaternion Rotation=CD3DQuaternion::SLerp(RotationStart,RotationEnd,S);
					CD3DVector3 Translation=CD3DVector3::Lerp(TranslationStart,TranslationEnd,S);
				

					m_BoneMatList[i].SetTransformation(&Scaling,&Rotation,&Translation);						
					m_BoneMatList[i]=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
						m_BoneMatList[i]*
						CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);
				}					
				
				if(m_CurPlayTime>m_InterimTime)
				{
					m_InterimStatus=IS_NONE;
					m_PlayStartTime=Time;
					m_CurPlayTime=0.0f;
				}
			}
			else
			{
				for(UINT i=0;i<m_BoneMatList.GetCount();i++)
				{			
					m_BoneMatList[i]=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
						m_BoneMatrices[i].Matrix*
						CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);			
				}
			}			

			
			for(UINT i=0;i<m_BoneMatList.GetCount();i++)
			{
				if(m_BoneMatrices[i].ParentID>=0)		
				{
					m_BoneMatList[i]=m_BoneMatList[i]*m_BoneMatList[m_BoneMatrices[i].ParentID];

					m_BoneMatrices[i].Flags|=m_BoneMatrices[m_BoneMatrices[i].ParentID].Flags&
						(CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD|CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD_HORIZONTAL);
				}
			}	
			

			for(UINT i=0;i<m_BoneMatList.GetCount();i++)
			{				
				CD3DSceneRender * pRender=dynamic_cast<CD3DSceneRender *>(GetRender());
				if(pRender&&pRender->GetCamera())
				{
					if(m_BoneMatrices[i].Flags&CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD)
					{
						CD3DMatrix BillBoardMat=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
							pRender->GetCamera()->GetWorldMatrixDirect().GetRotation()*
							GetWorldMatrix().GetRotation().GetInverse()*m_BoneMatList[i].GetRotation().GetInverse()*
							CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);

						m_BoneMatList[i]=BillBoardMat*m_BoneMatList[i];//*GetWorldMatrix().GetRotation().GetInverse();
					}
					else if(m_BoneMatrices[i].Flags&CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD_HORIZONTAL)
					{
						CD3DMatrix BillBoardMat=pRender->GetCamera()->GetWorldMatrixDirect().GetRotation()*
							GetWorldMatrix().GetRotation().GetInverse();

						BillBoardMat._12=0.0f;
						BillBoardMat._21=0.0f;
						BillBoardMat._22=1.0f;
						BillBoardMat._23=0.0f;
						BillBoardMat._32=0.0f;						

						BillBoardMat=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
							BillBoardMat*
							CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);

						m_BoneMatList[i]=BillBoardMat*m_BoneMatList[i];
					}					
				}
				if(m_BoneMatrices[i].pAttachObject)
				{	
					CD3DMatrix Mat=m_BoneMatrices[i].AttachTransform*m_BoneMatList[i];
					m_BoneMatrices[i].pAttachObject->SetLocalMatrix(Mat);				
				}

			}

			if(m_UseSoftSkinMesh)
			{
				CaculateSoftSkinMesh();
			}

		}
		else
		{
			for(UINT i=0;i<m_BoneMatList.GetCount();i++)
			{			
				m_BoneMatList[i].SetIdentity();
				CD3DSceneRender * pRender=dynamic_cast<CD3DSceneRender *>(GetRender());
				if(pRender&&pRender->GetCamera())
				{
					if(m_BoneMatrices[i].Flags&CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD)
					{
						CD3DMatrix BillBoardMat=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
							pRender->GetCamera()->GetWorldMatrixDirect().GetRotation()*
							GetWorldMatrix().GetRotation().GetInverse()*m_BoneMatList[i].GetRotation().GetInverse()*
							CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);		
						m_BoneMatList[i]=BillBoardMat*m_BoneMatList[i];
					}	
					else if(m_BoneMatrices[i].Flags&CD3DWOWM2ModelResource::BONE_FLAG_BILLBOARD_HORIZONTAL)
					{
						CD3DMatrix BillBoardMat=pRender->GetCamera()->GetWorldMatrixDirect().GetRotation()*
							GetWorldMatrix().GetRotation().GetInverse()*m_BoneMatList[i].GetRotation().GetInverse();

						BillBoardMat._12=0.0f;
						BillBoardMat._21=0.0f;
						BillBoardMat._22=1.0f;
						BillBoardMat._23=0.0f;
						BillBoardMat._32=0.0f;		

						BillBoardMat=CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint*(-1.0f))*
							BillBoardMat*
							CD3DMatrix::FromTranslation(m_BoneMatrices[i].PivotPoint);

						m_BoneMatList[i]=BillBoardMat*m_BoneMatList[i];
					}

					if(m_BoneMatrices[i].pAttachObject)
					{	
						CD3DMatrix Mat=m_BoneMatrices[i].AttachTransform*m_BoneMatList[i];
						m_BoneMatrices[i].pAttachObject->SetLocalMatrix(Mat);				
					}
				}
			}
		}

		if(m_pModelResource->HasMaterialAni())
		{
			//设置颜色动画和纹理动画
			for(int i=0;i<GetSubMeshCount();i++)		
			{			
				CD3DSubMesh * pSubMesh=GetSubMesh(i);
				CD3DSubMeshMaterial * pMaterial=GetSubMeshMaterial(i);
				if(pSubMesh&&pMaterial)
				{
					D3DCOLORVALUE GlobalColor={1.0f,1.0f,1.0f,1.0f};

					UINT64 SubMeshProperty=pSubMesh->GetProperty();
					BYTE ColorAniIndex=(SubMeshProperty&CD3DWOWM2ModelResource::SMP_COLOR_ANI_INDEX_MASK)>>
						CD3DWOWM2ModelResource::SMP_COLOR_ANI_INDEX_SHIFT;
					BYTE TransparencyAniIndex=(SubMeshProperty&CD3DWOWM2ModelResource::SMP_TRANSPARENCY_ANI_INDEX_MASK)>>
						CD3DWOWM2ModelResource::SMP_TRANSPARENCY_ANI_INDEX_SHIFT;

					if(ColorAniIndex)
					{
						m_pModelResource->MakeColorAnimationFrame(ColorAniIndex-1,CurTime,m_IsLoop,GlobalColor);
					}
					if(TransparencyAniIndex)
					{
						FLOAT Alpha=1.0f;
						if(m_pModelResource->MakeTransparencyAnimationFrame(TransparencyAniIndex-1,CurTime,m_IsLoop,Alpha))
						{
							GlobalColor.a*=Alpha;
						}
					}
					pMaterial->SetGlobalColor(GlobalColor);

					for(UINT j=0;j<pMaterial->GetTextureLayerCount();j++)
					{
						UINT64 TextureProperty=pMaterial->GetTextureProperty(j);
						BYTE UVAniIndex=(TextureProperty&CD3DWOWM2ModelResource::TF_UV_ANI_INDEX_MASK)>>
							CD3DWOWM2ModelResource::TF_UV_ANI_INDEX_SHIFT;
						if(UVAniIndex)
						{
							CD3DMatrix UVTransform;
							if(m_pModelResource->MakeTextureUVAniFrame(UVAniIndex-1,CurTime,m_IsLoop,UVTransform))
							{								
								pMaterial->SetTextureUVTransform(j,UVTransform);
							}
						}
					}
					pMaterial->CaculateHashCode();
				}			
			}
		}
	}
		
	//更新子对象
	
	if(m_ChildList.GetCount())
	{
		for(UINT i=0;i<m_ChildList.GetCount();i++)
		{
			((CD3DObject *)m_ChildList[i])->Update(Time);
		}
	}
	

	m_IsMatrixChanged=false;


}

bool CD3DWOWM2Model::NeedUpdateAni()
{
	if(m_pModelResource)
	{
		return m_pModelResource->HasBoneAni()||m_pModelResource->HasMaterialAni()||
			m_pModelResource->GetParticleEmitterCount()!=0||
			m_pModelResource->GetRibbonEmitterCount();
	}
	return false;
}

void CD3DWOWM2Model::PrepareSoftSkinMesh()
{	
	for(UINT i=0;i<m_SubMeshListForSoftSkinMesh.GetCount();i++)
	{
		SAFE_RELEASE(m_SubMeshListForSoftSkinMesh[i]);
	}
	m_SubMeshListForSoftSkinMesh.Clear();

	m_Vertices.Resize(m_pModelResource->GetVertexCount());
	m_VerticesR.Resize(m_pModelResource->GetVertexCount());

	memcpy(m_Vertices.GetBuffer(),m_pModelResource->GetVertices(),m_pModelResource->GetVertexCount()*sizeof(CD3DWOWM2ModelResource::MODEL_VERTEXT));

	for(int i=0;i<GetSubMeshCount();i++)
	{
		CD3DSubMesh * pSubMesh=GetOriginSubMesh(i);
		if(pSubMesh)
		{
			CD3DSubMesh * pNewSubMesh=new CD3DSubMesh(GetDevice());
			
			pNewSubMesh->CloneFrom(pSubMesh,SUBMESH_CLONE_USE_SYSTEM_MEM|SUBMESH_CLONE_NOT_CLONE_MATERIAL);
			pNewSubMesh->SetID(pSubMesh->GetID());
			pNewSubMesh->SetName(pSubMesh->GetName());
			
			pNewSubMesh->SetVertices((BYTE *)m_Vertices.GetBuffer());
			pNewSubMesh->SetVertexBufferR((BYTE *)m_VerticesR.GetBuffer());
			pNewSubMesh->SetIndices((BYTE *)m_pModelResource->GetIndices());
			pNewSubMesh->OnPrepareRenderData();

			m_SubMeshListForSoftSkinMesh.Add(pNewSubMesh);
		}		
	}
	
}

void CD3DWOWM2Model::CaculateSoftSkinMesh()
{
	m_IsRenderDataChanged=true;
	for(UINT s=0;s<m_SubMeshListForSoftSkinMesh.GetCount();s++)
	{
		
		CD3DSubMesh * pSubMesh=m_SubMeshListForSoftSkinMesh[s];
		if(pSubMesh->CheckProperty(CD3DSubMesh::SMF_IS_ANI_MESH))
		{
			CD3DWOWM2ModelResource::MODEL_VERTEXT * pDestVertices=m_Vertices.GetBuffer()+pSubMesh->GetVertexStart();

			CD3DWOWM2ModelResource::MODEL_VERTEXT * pSrcVertices=m_pModelResource->GetVertices()+pSubMesh->GetVertexStart();


			for(UINT i=0;i<pSubMesh->GetVertexCount();i++)
			{
				pDestVertices[i].Pos=0.0f;
				pDestVertices[i].Normal=0.0f;
				for(int b=0;b<MAX_VERTEX_BONE_BIND;b++)
				{
					if(pSrcVertices[i].BoneWeight[b]>0.000001f)
					{
						pDestVertices[i].Pos+=pSrcVertices[i].Pos*
							m_BoneMatList[pSrcVertices[i].BoneID[b]]*pSrcVertices[i].BoneWeight[b];
						pDestVertices[i].Normal+=pSrcVertices[i].Normal*
							m_BoneMatList[pSrcVertices[i].BoneID[b]].GetScaleRotation()*pSrcVertices[i].BoneWeight[b];
					}							
				}			
			}
		}
	}		
	
}


void CD3DWOWM2Model::FetchAnimationFrames(UINT Time)
{
	if(m_pModelResource)
	{		
		m_pModelResource->MakeAnimationBoneFrame(m_CurAnimationIndex,Time,m_IsLoop,
			m_BoneMatrices.GetBuffer(),m_BoneMatrices.GetCount());
	}
}

void CD3DWOWM2Model::OnSubMeshChanged()
{
	if(m_UseSoftSkinMesh)
		PrepareSoftSkinMesh();
}

}