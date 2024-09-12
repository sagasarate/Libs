/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2CharacterModel.cpp                               */
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

IMPLEMENT_CLASS_INFO(CD3DWOWM2CharacterModel,CD3DWOWM2Model);
CD3DWOWM2CharacterModel::CD3DWOWM2CharacterModel(void)
{
	m_CreatureDisplayID=0;
	m_NeedRebuildSubMesh=false;
	m_CharRace=-1;
	m_CharSex=-1;
	m_CharSexMax=0;
	m_CharSkinColor=0;
	m_CharSkinColorMax=0;
	m_CharHairColor=0;
	m_CharHairColorMax=0;
	m_CharFaceType=0;
	m_CharFaceTypeMax=0;
	m_CharHairType=0;
	m_CharHairTypeMax=0;
	m_CharBeardType=0;
	m_CharBeardTypeMax=0;
	m_IsCharBald=false;
	ZeroMemory(m_Equipments,sizeof(m_Equipments));
	m_pHelmetModel=NULL;
	m_pLeftShoulderModel=NULL;
	m_pRightShoulderModel=NULL;
	m_pLeftWeaponModel=NULL;
	m_pRightWeaponModel=NULL;
	m_pRightShieldModel=NULL;

	m_CloseHandAnimationIndex=-1;



}

CD3DWOWM2CharacterModel::~CD3DWOWM2CharacterModel(void)
{
	Destory();
}


void CD3DWOWM2CharacterModel::Destory()
{
	DestoryModel();
	CD3DWOWM2Model::Destory();
}

void CD3DWOWM2CharacterModel::DestoryModel()
{
	CleanCharModel();
	CD3DWOWM2Model::DestoryModel();
}


bool CD3DWOWM2CharacterModel::Reset()
{
	bool Ret=true;
	for(UINT i=0;i<m_SubMeshMaterialList.GetCount();i++)
	{		
		Ret=Ret&&m_SubMeshMaterialList[i].Reset();
	}
	return Ret&&CD3DWOWM2Model::Reset();
}
bool CD3DWOWM2CharacterModel::Restore()
{
	bool Ret=true;
	for(UINT i=0;i<m_SubMeshMaterialList.GetCount();i++)
	{		
		Ret=Ret&&m_SubMeshMaterialList[i].Restore();
	}
	return Ret&&CD3DWOWM2Model::Restore();
}

bool CD3DWOWM2CharacterModel::SetCreatureDisplayID(UINT ID)
{
	CBLZWOWDatabase::BLZ_DB_CREATURE_DISPLAY_INFO * pDisplayInfo=CBLZWOWDatabase::GetInstance()->FindCreatureDisplayInfo(ID);
	if(pDisplayInfo)
	{
		m_CreatureDisplayID=ID;
		return true;
	}
	return false;
}

bool CD3DWOWM2CharacterModel::SetCharSkinColor(int Value)
{
	if(Value<0||Value>=m_CharSkinColorMax)
		return false;
	m_CharSkinColor=Value;
	return true;
}
bool CD3DWOWM2CharacterModel::SetCharHairColor(int Value)
{
	if(Value<0||Value>=m_CharHairColorMax)
		return false;
	m_CharHairColor=Value;
	return true;
}
bool CD3DWOWM2CharacterModel::SetCharFaceType(int Value)
{
	if(Value<0||Value>=m_CharFaceTypeMax)
		return false;
	m_CharFaceType=Value;
	return true;
}
bool CD3DWOWM2CharacterModel::SetCharHairType(int Value)
{
	if(Value<0||Value>=m_CharHairTypeMax)
		return false;
	m_CharHairType=Value;
	return true;
}
bool CD3DWOWM2CharacterModel::SetCharBeardType(int Value)
{
	if(Value<0||Value>=m_CharBeardTypeMax)
		return false;
	m_CharBeardType=Value;
	return true;
}		

bool CD3DWOWM2CharacterModel::SetEquipment(UINT Slot,UINT ItemID)
{
	if(Slot<CES_MAX)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DATA * pItemInfo=CBLZWOWDatabase::GetInstance()->GetItemData(ItemID);
		if(pItemInfo)
		{			
			CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
				CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(pItemInfo->ItemDisplayInfo);
			if(pItemDisplayInfo)
			{
				m_Equipments[Slot].ItemID=ItemID;
				m_Equipments[Slot].ItemDisplayID=pItemInfo->ItemDisplayInfo;
				return true;
			}				
		}
		else
		{
			m_Equipments[Slot].ItemID=0;
			m_Equipments[Slot].ItemDisplayID=0;
		}
		
	}
	return false;
}

bool CD3DWOWM2CharacterModel::LoadCharacter(UINT Race,UINT Sex)
{
	if(Race>CBLZWOWDatabase::GetInstance()->GetMaxCharRace())
		return false;
	m_CharRace=Race;	
	m_CharSexMax=CBLZWOWDatabase::GetInstance()->GetMaxCharSex(m_CharRace);

	if(Sex>=m_CharSexMax)
		return false;

	m_CharSex=Sex;
	CBLZWOWDatabase::GetInstance()->GetCharMaxInfo(m_CharRace,m_CharSex,
		m_CharSkinColorMax,m_CharHairColorMax,m_CharFaceTypeMax,m_CharHairTypeMax,m_CharBeardTypeMax);


	CBLZWOWDatabase::BLZ_DB_CHAR_RACE_INFO * pRaceInfo=CBLZWOWDatabase::GetInstance()->FindRaceInfo(m_CharRace);
	if(pRaceInfo)
	{
		CBLZWOWDatabase::BLZ_DB_CREATURE_DISPLAY_INFO * pDisplayInfo=NULL;
		if(m_CharSex==0)
			pDisplayInfo=CBLZWOWDatabase::GetInstance()->FindCreatureDisplayInfo(pRaceInfo->MaleModelID);
		else
			pDisplayInfo=CBLZWOWDatabase::GetInstance()->FindCreatureDisplayInfo(pRaceInfo->FemaleModelID);
		if(pDisplayInfo)
		{

			CBLZWOWDatabase::BLZ_DB_CREATURE_MODEL_INFO * pModelInfo=CBLZWOWDatabase::GetInstance()->
				FindCreatureModelInfo(pDisplayInfo->ModelID);
			if(pModelInfo)
			{
				CEasyString ModelFileName=pModelInfo->ModelPath;
				int Pos=ModelFileName.ReverseFind('.');
				if(Pos>=0)
				{
					ModelFileName=ModelFileName.Left(Pos);
				}
				ModelFileName=ModelFileName+".m2";
				if(LoadFromFile(ModelFileName))
				{
					CEasyString Temp;
					Temp.Format(_T("%s_%s_%u"),
						(LPCTSTR)pRaceInfo->RaceName,
						m_CharSex==0?_T("男"):_T("女"),
						pDisplayInfo->ID);
					SetName(Temp);
					m_CreatureDisplayID=pDisplayInfo->ID;
					m_NeedRebuildSubMesh=true;
					if(BuildModel())
					{
						CD3DMatrix Mat=CD3DMatrix::FromScale(pDisplayInfo->Scale,pDisplayInfo->Scale,pDisplayInfo->Scale);
						SetLocalMatrix(Mat);
						return true;
					}
				}
			}
		}
	}	
	return false;
}

bool CD3DWOWM2CharacterModel::LoadCreature(UINT CreatureDisplayID)
{
	CBLZWOWDatabase::BLZ_DB_CREATURE_DISPLAY_INFO * pDisplayInfo=CBLZWOWDatabase::GetInstance()->FindCreatureDisplayInfo(CreatureDisplayID);
	if(pDisplayInfo)
	{
		CBLZWOWDatabase::BLZ_DB_CREATURE_MODEL_INFO * pModelInfo=CBLZWOWDatabase::GetInstance()->FindCreatureModelInfo(pDisplayInfo->ModelID);
		if(pModelInfo)
		{
			CEasyString ModelFileName=pModelInfo->ModelPath;
			int Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";
			if(LoadFromFile(ModelFileName))
			{	
				CEasyString Temp;
				Temp.Format(_T("%u_%s"),CreatureDisplayID,(LPCTSTR)pDisplayInfo->Name);
				SetName(Temp);
				m_CreatureDisplayID=CreatureDisplayID;
				FetchCreatureExtraInfo(pDisplayInfo->ExtraDisplayInformationID);
				m_NeedRebuildSubMesh=pDisplayInfo->ExtraDisplayInformationID!=0;
				if(BuildModel())
				{
					CD3DMatrix Mat=CD3DMatrix::FromScale(pDisplayInfo->Scale,pDisplayInfo->Scale,pDisplayInfo->Scale);
					SetLocalMatrix(Mat);
					return true;
				}				
			}
			
		}
	}
	return false;
}

bool CD3DWOWM2CharacterModel::BuildModel()
{
	D3DLOCK_FOR_OBJECT_ADD;

	if(GetDevice()==NULL||m_pModelResource==NULL)
		return false;

	CleanCharModel();

	m_CloseHandAnimationIndex=m_pModelResource->GetAnimationIndex(15,0);

	bool	HairVisible=true;				
	bool	Facial1Visible=true;			
	bool	Facial2Visible=true;			
	bool	Facial3Visible=true;			
	bool	EarsVisible=true;
	bool	HaveSleeve=false;


	BuildEquipmentModel(HairVisible,Facial1Visible,Facial2Visible,Facial3Visible,EarsVisible);

	if(m_NeedRebuildSubMesh)
	{
		RebuildSubMesh(HairVisible,Facial1Visible,Facial2Visible,Facial3Visible,EarsVisible,HaveSleeve);
	}
	else
	{
		for(int i=0;i<m_pModelResource->GetSubMeshCount();i++)
		{
			CD3DSubMesh * pD3DSubMesh=m_pModelResource->GetSubMesh(i);
			m_SubMeshList.Add(pD3DSubMesh);
		}
	}


	m_SubMeshMaterialList.Resize(m_SubMeshList.GetCount());

	CD3DTexture * pCharSkinTexture=NULL;
	CD3DTexture * pCharSkinExtraTexture=NULL;
	CD3DTexture * pCharHairTexture=NULL;	
	CD3DTexture * pCapeTexture=NULL;
	CD3DTexture * pSkinTexture1=NULL;
	CD3DTexture * pSkinTexture2=NULL;
	CD3DTexture * pSkinTexture3=NULL;

	MakeCharSkinTexture(GetDevice(),pCharSkinTexture,pCharSkinExtraTexture,pCharHairTexture,pCapeTexture,
		pSkinTexture1,pSkinTexture2,pSkinTexture3,HaveSleeve);
	
	CEasyStringA TexFXName;
	CEasyStringA MatFXName;

	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{

		//m_SubMeshList[i]->SetVisible(true);
		m_SubMeshMaterialList[i].SetMaterial(m_SubMeshList[i]->GetMaterial().GetMaterial());

		int Part=m_SubMeshList[i]->GetID()/100;
		int Type=m_SubMeshList[i]->GetID()%100;		

		for(UINT j=0;j<m_SubMeshList[i]->GetMaterial().GetTextureLayerCount();j++)
		{
			UINT64 TextureProperty=m_SubMeshList[i]->GetMaterial().GetTextureProperty(j);
			int TextureType=(TextureProperty&CD3DWOWM2ModelResource::TF_SKIN_TEXTURE_TYPE_MASK)>>
				CD3DWOWM2ModelResource::TF_SKIN_TEXTURE_TYPE_SHIFT;
			TexFXName.Format("TexLay%d",m_SubMeshMaterialList[i].GetTextureLayerCount());
			MatFXName.Format("UVMatrix%d",m_SubMeshMaterialList[i].GetTextureLayerCount());
			switch(TextureType)
			{
			case D3D_TEXTURE_TYPE_DIRECT:
				{
					CD3DTexture * pTexture=m_SubMeshList[i]->GetMaterial().GetTexture(j);					
					m_SubMeshMaterialList[i].AddTexture(pTexture,TextureProperty,TexFXName,MatFXName);
					pTexture->AddUseRef();
				}
				break;			
			case D3D_TEXTURE_TYPE_BODY:
				if(pCharSkinTexture)
				{				
					m_SubMeshMaterialList[i].AddTexture(pCharSkinTexture,TextureProperty,TexFXName,MatFXName);
					pCharSkinTexture->AddUseRef();
				}
				break;
			case D3D_TEXTURE_TYPE_CAPE:
				if(pCapeTexture)
				{
					m_SubMeshMaterialList[i].AddTexture(pCapeTexture,TextureProperty,TexFXName,MatFXName);
					pCapeTexture->AddUseRef();
				}
				break;
			case D3D_TEXTURE_TYPE_HAIR:
				if(pCharHairTexture)
				{

					m_SubMeshMaterialList[i].AddTexture(pCharHairTexture,TextureProperty,TexFXName,MatFXName);
					pCharHairTexture->AddUseRef();
				}
				break;
			case D3D_TEXTURE_TYPE_FUR:
				if(pCharSkinExtraTexture)
				{

					m_SubMeshMaterialList[i].AddTexture(pCharSkinExtraTexture,TextureProperty,TexFXName,MatFXName);
					pCharSkinExtraTexture->AddUseRef();
				}
				break;	
			case D3D_TEXTURE_TYPE_CREATURE_SKIN1:
				if(pSkinTexture1)
				{
					m_SubMeshMaterialList[i].AddTexture(pSkinTexture1,TextureProperty,TexFXName,MatFXName);
					pSkinTexture1->AddUseRef();
				}
				break;
			case D3D_TEXTURE_TYPE_CREATURE_SKIN2:
				if(pSkinTexture2)
				{
					m_SubMeshMaterialList[i].AddTexture(pSkinTexture2,TextureProperty,TexFXName,MatFXName);
					pSkinTexture2->AddUseRef();
				}
				break;
			case D3D_TEXTURE_TYPE_CREATURE_SKIN3:
				if(pSkinTexture3)
				{
					m_SubMeshMaterialList[i].AddTexture(pSkinTexture3,TextureProperty,TexFXName,MatFXName);
					pSkinTexture3->AddUseRef();
				}
				break;
			default:
				PrintD3DLog(_T("SubMesh[%s] has no Texture"),m_SubMeshList[i]->GetName());
				break;
			}
		}


		m_SubMeshMaterialList[i].SetFX(m_SubMeshList[i]->GetMaterial().GetFX());
		if(m_SubMeshList[i]->GetMaterial().GetFX())
			m_SubMeshList[i]->GetMaterial().GetFX()->AddUseRef();

		m_SubMeshMaterialList[i].CaculateHashCode();

	}
	SAFE_RELEASE(pCharSkinTexture);
	SAFE_RELEASE(pCharSkinExtraTexture);
	SAFE_RELEASE(pCharHairTexture);
	SAFE_RELEASE(pCapeTexture);
	SAFE_RELEASE(pSkinTexture1);
	SAFE_RELEASE(pSkinTexture2);
	SAFE_RELEASE(pSkinTexture3);

	OnSubMeshChanged();
	Update(m_PlayStartTime+m_CurPlayTime);
	OnPrepareRenderData();
	return true;
}



int CD3DWOWM2CharacterModel::GetSubMeshCount()
{
	//return m_pModelResource->GetSubMeshCount();
	return (int)m_SubMeshList.GetCount();
}
CD3DSubMesh * CD3DWOWM2CharacterModel::GetOriginSubMesh(UINT index)
{
	//return m_pModelResource->GetSubMesh(index);
	return m_SubMeshList[index];
}
CD3DSubMeshMaterial * CD3DWOWM2CharacterModel::GetSubMeshMaterial(UINT index)
{
	//return &(m_pModelResource->GetSubMesh(index)->GetMaterial());
	return &(m_SubMeshMaterialList[index]);
}

bool CD3DWOWM2CharacterModel::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DWOWM2CharacterModel)))
		return false;

	Destory();

	if(!CD3DWOWM2Model::CloneFrom(pObject,Param))
		return false;

	CD3DWOWM2CharacterModel * pSrcObject=(CD3DWOWM2CharacterModel *)pObject;

	m_SubMeshList=pSrcObject->m_SubMeshList;

	m_SubMeshMaterialList.Resize(pSrcObject->m_SubMeshMaterialList.GetCount());
	for(UINT i=0;i<pSrcObject->m_SubMeshMaterialList.GetCount();i++)
	{
		m_SubMeshMaterialList[i].SetMaterial(pSrcObject->m_SubMeshMaterialList[i].GetMaterial());
		for(UINT j=0;j<pSrcObject->m_SubMeshMaterialList[i].GetTextureLayerCount();j++)
		{
			m_SubMeshMaterialList[i].AddTexture(pSrcObject->m_SubMeshMaterialList[i].GetTexture(j),
				pSrcObject->m_SubMeshMaterialList[i].GetTextureProperty(j),
				pSrcObject->m_SubMeshMaterialList[i].GetTextureFXParamName(j),
				pSrcObject->m_SubMeshMaterialList[i].GetTextureMatFXParamName(j));
			if(pSrcObject->m_SubMeshMaterialList[i].GetTexture(j))
				pSrcObject->m_SubMeshMaterialList[i].GetTexture(j)->AddUseRef();
		}	
		m_SubMeshMaterialList[i].SetFX(pSrcObject->m_SubMeshMaterialList[i].GetFX());
		if(pSrcObject->m_SubMeshMaterialList[i].GetFX())
			pSrcObject->m_SubMeshMaterialList[i].GetFX()->AddUseRef();
	}

	m_CharRace=pSrcObject->m_CharRace;
	m_CharSex=pSrcObject->m_CharSex;
	m_CharSexMax=pSrcObject->m_CharSexMax;
	m_CharSkinColor=pSrcObject->m_CharSkinColor;
	m_CharSkinColorMax=pSrcObject->m_CharSkinColorMax;
	m_CharHairColor=pSrcObject->m_CharHairColor;
	m_CharHairColorMax=pSrcObject->m_CharHairColorMax;
	m_CharFaceType=pSrcObject->m_CharFaceType;
	m_CharFaceTypeMax=pSrcObject->m_CharFaceTypeMax;
	m_CharHairType=pSrcObject->m_CharHairType;
	m_CharHairTypeMax=pSrcObject->m_CharHairTypeMax;
	m_CharBeardType=pSrcObject->m_CharBeardType;
	m_CharBeardTypeMax=pSrcObject->m_CharBeardTypeMax;
	m_IsCharBald=pSrcObject->m_IsCharBald;

	memcpy(m_Equipments,pSrcObject->m_Equipments,sizeof(m_Equipments));

	if(pSrcObject->m_pHelmetModel)
		m_pHelmetModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pHelmetModel->GetStorageID());
	if(pSrcObject->m_pLeftShoulderModel)
		m_pLeftShoulderModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pLeftShoulderModel->GetStorageID());
	if(pSrcObject->m_pRightShoulderModel)
		m_pRightShoulderModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pRightShoulderModel->GetStorageID());
	if(pSrcObject->m_pLeftWeaponModel)
		m_pLeftWeaponModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pLeftWeaponModel->GetStorageID());
	if(pSrcObject->m_pRightWeaponModel)
		m_pRightWeaponModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pRightWeaponModel->GetStorageID());
	if(pSrcObject->m_pRightShieldModel)
		m_pRightShieldModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(pSrcObject->m_pRightShieldModel->GetStorageID());

	OnSubMeshChanged();
	return true;

}

void CD3DWOWM2CharacterModel::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	CD3DWOWM2Model::PickResource(pResourceManager,Param);
	for(UINT i=0;i<m_SubMeshMaterialList.GetCount();i++)
	{
		if(m_SubMeshMaterialList[i].GetFX())
		{			
			pResourceManager->AddResource(m_SubMeshMaterialList[i].GetFX());
		}
		for(UINT j=0;j<m_SubMeshMaterialList[i].GetTextureLayerCount();j++)
		{
			if(m_SubMeshMaterialList[i].GetTexture(j))
			{
				pResourceManager->AddResource(m_SubMeshMaterialList[i].GetTexture(j));
			}
		}
	}
}

bool CD3DWOWM2CharacterModel::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	SAFE_RELEASE(m_pBoundingFrame);

	if(!CD3DWOWM2Model::ToSmartStruct(Packet,pResourceManager,Param))
		return false;

	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_ACITVE_SUB_MESH,
			m_SubMeshList[i]->GetStorageID()));
	}
	for(UINT i=0;i<m_SubMeshMaterialList.GetCount();i++)
	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);

		if(!m_SubMeshMaterialList[i].ToSmartStruct(SubPacket,pResourceManager,Param))
			return false;

		if(!Packet.FinishMember(SST_D3DWMCM_CHAR_MATERIAL,SubPacket.GetDataLen()))
			return false;
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_RACE,m_CharRace));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_SEX,m_CharSex));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_SKIN_COLOR,m_CharSkinColor));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_HAIR_COLOR,m_CharHairColor));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_FACE_TYPE,m_CharFaceType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_HAIR_TYPE,m_CharHairType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_WHISKER_TYPE,m_CharBeardType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_IS_BALD,(char)m_IsCharBald));

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_EQUIPMENTS,(char *)m_Equipments,sizeof(m_Equipments)));

	if(m_pHelmetModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_HELMET_MODEL,m_pHelmetModel->GetStorageID()));
	}
	if(m_pLeftShoulderModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_LEFT_SHOULDER_MODEL,m_pLeftShoulderModel->GetStorageID()));
	}
	if(m_pRightShoulderModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_RIGHT_SHOULDER_MODEL,m_pRightShoulderModel->GetStorageID()));
	}
	if(m_pLeftWeaponModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_LEFT_WEAPON_MODEL,m_pLeftWeaponModel->GetStorageID()));
	}
	if(m_pRightWeaponModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_RIGHT_WEAPON_MODEL,m_pRightWeaponModel->GetStorageID()));
	}
	if(m_pRightShieldModel)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CHAR_SHIELD_MODEL,m_pRightShieldModel->GetStorageID()));
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_CREATURE_DISPLAY_ID,m_CreatureDisplayID));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DWMCM_NEED_REBUILD_SUBMESH,(BYTE)m_NeedRebuildSubMesh));
	
	return true;
}
bool CD3DWOWM2CharacterModel::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DWOWM2Model::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	UINT SubMeshCount=0;
	UINT MaterialCount=0;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWMCM_CHAR_ACITVE_SUB_MESH:
			SubMeshCount++;	
			break;
		case SST_D3DWMCM_CHAR_MATERIAL:
			MaterialCount++;
			break;
		}
	}
	m_SubMeshList.Resize(SubMeshCount);
	m_SubMeshMaterialList.Resize(MaterialCount);

	SubMeshCount=0;
	MaterialCount=0;

	Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DWMCM_CHAR_ACITVE_SUB_MESH:
			{
				UINT StorageID=Value;
				for(int i=0;i<m_pModelResource->GetSubMeshCount();i++)
				{
					if(m_pModelResource->GetSubMesh(i)->GetStorageID()==StorageID)
					{
						m_SubMeshList[SubMeshCount]=m_pModelResource->GetSubMesh(i);
						break;
					}
				}
			}	
			SubMeshCount++;
			break;
		case SST_D3DWMCM_CHAR_MATERIAL:
			{
				CSmartStruct MaterialPacket=Value;
				if(!m_SubMeshMaterialList[MaterialCount].FromSmartStruct(MaterialPacket,pResourceManager,Param))
					return false;
			}
			MaterialCount++;
			break;
		case SST_D3DWMCM_CHAR_RACE:
			m_CharRace=Value;
			break;
		case SST_D3DWMCM_CHAR_SEX:
			m_CharSex=Value;
			break;
		case SST_D3DWMCM_CHAR_SKIN_COLOR:
			m_CharSkinColor=Value;
			break;
		case SST_D3DWMCM_CHAR_HAIR_COLOR:
			m_CharHairColor=Value;
			break;
		case SST_D3DWMCM_CHAR_FACE_TYPE:
			m_CharFaceType=Value;
			break;
		case SST_D3DWMCM_CHAR_HAIR_TYPE:
			m_CharHairType=Value;
			break;
		case SST_D3DWMCM_CHAR_WHISKER_TYPE:
			m_CharBeardType=Value;
			break;
		case SST_D3DWMCM_CHAR_IS_BALD:
			m_IsCharBald=(BYTE)Value!=0;
			break;
		case SST_D3DWMCM_CHAR_EQUIPMENTS:
			memcpy(m_Equipments,(LPCSTR)Value,sizeof(m_Equipments));
			break;
		case SST_D3DWMCM_CHAR_HELMET_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pHelmetModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pHelmetModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pHelmetModel=NULL;
					}
				}
			}
			break;
		case SST_D3DWMCM_CHAR_LEFT_SHOULDER_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pLeftShoulderModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pLeftShoulderModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pLeftShoulderModel=NULL;
					}
				}
			}
			break;
		case SST_D3DWMCM_CHAR_RIGHT_SHOULDER_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pRightShoulderModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pRightShoulderModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pRightShoulderModel=NULL;
					}
				}
			}
			break;
		case SST_D3DWMCM_CHAR_LEFT_WEAPON_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pLeftWeaponModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pLeftWeaponModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pLeftWeaponModel=NULL;
					}
				}
			}
			break;
		case SST_D3DWMCM_CHAR_RIGHT_WEAPON_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pRightWeaponModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pRightWeaponModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pRightWeaponModel=NULL;
					}
				}
			}
			break;
		case SST_D3DWMCM_CHAR_SHIELD_MODEL:
			{
				UINT StorageID=Value;
				if(StorageID)
				{
					m_pRightShieldModel=(CD3DWOWM2ItemModel *)GetChildByStorageIDRecursive(StorageID);
					if(!m_pRightShieldModel->IsKindOf(GET_CLASS_INFO(CD3DWOWM2ItemModel)))
					{
						m_pRightShieldModel=NULL;
					}
				}
			}
			break;
			
		case SST_D3DWMCM_CREATURE_DISPLAY_ID:
			m_CreatureDisplayID=Value;
			break;
		case SST_D3DWMCM_NEED_REBUILD_SUBMESH:
			m_NeedRebuildSubMesh=Value;
			break;
		}
	}
	m_CharSexMax=CBLZWOWDatabase::GetInstance()->GetMaxCharSex(m_CharRace);
	CBLZWOWDatabase::GetInstance()->GetCharMaxInfo(m_CharRace,m_CharSex,
		m_CharSkinColorMax,m_CharHairColorMax,m_CharFaceTypeMax,m_CharHairTypeMax,m_CharBeardTypeMax);
	OnSubMeshChanged();
	return true;
}
UINT CD3DWOWM2CharacterModel::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DWOWM2Model::GetSmartStructSize(Param);

	for(UINT i=0;i<m_SubMeshList.GetCount();i++)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_SubMeshList[i]->GetStorageID()));
	}
	for(UINT i=0;i<m_SubMeshMaterialList.GetCount();i++)
	{
		Size+=m_SubMeshMaterialList[i].GetSmartStructSize(Param);
		Size+=CSmartStruct::GetStructMemberSize(0);
	}
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharRace));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharSex));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharSkinColor));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharHairColor));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharFaceType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharHairType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CharBeardType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_IsCharBald));
	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_Equipments));

	if(m_pHelmetModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pHelmetModel->GetStorageID()));
	}
	if(m_pLeftShoulderModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pLeftShoulderModel->GetStorageID()));
	}
	if(m_pRightShoulderModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pRightShoulderModel->GetStorageID()));
	}
	if(m_pLeftWeaponModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pLeftWeaponModel->GetStorageID()));
	}
	if(m_pRightWeaponModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pRightWeaponModel->GetStorageID()));
	}
	if(m_pRightShieldModel)
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_pRightShieldModel->GetStorageID()));
	}

	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_CreatureDisplayID));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_NeedRebuildSubMesh));

	return Size;
}


void CD3DWOWM2CharacterModel::CleanCharModel()
{
	m_CloseHandAnimationIndex=-1;

	if(m_pHelmetModel)
		m_pHelmetModel->SetParent(NULL);
	if(m_pLeftShoulderModel)
		m_pLeftShoulderModel->SetParent(NULL);
	if(m_pRightShoulderModel)
		m_pRightShoulderModel->SetParent(NULL);
	if(m_pLeftWeaponModel)
		m_pLeftWeaponModel->SetParent(NULL);
	if(m_pRightWeaponModel)
		m_pRightWeaponModel->SetParent(NULL);
	if(m_pRightShieldModel)
		m_pRightShieldModel->SetParent(NULL);
	SAFE_RELEASE(m_pHelmetModel);
	SAFE_RELEASE(m_pLeftShoulderModel);
	SAFE_RELEASE(m_pRightShoulderModel);
	SAFE_RELEASE(m_pLeftWeaponModel);
	SAFE_RELEASE(m_pRightWeaponModel);
	SAFE_RELEASE(m_pRightShieldModel);
	m_SubMeshMaterialList.Clear();
	m_SubMeshList.Clear();
}

bool CD3DWOWM2CharacterModel::FetchCreatureExtraInfo(UINT ExtraInfoID)
{
	CBLZWOWDatabase::BLZ_DB_CREATURE_EXTRA_DISPLAY_INFO * pExtraInfo=CBLZWOWDatabase::GetInstance()->FindCreatureExtraDisplayInfo(ExtraInfoID);
	if(pExtraInfo)
	{
		m_CharRace=pExtraInfo->Race;
		m_CharSex=pExtraInfo->Sex;
		m_CharSkinColor=pExtraInfo->SkinColor;
		m_CharFaceType=pExtraInfo->FaceType;
		m_CharHairType=pExtraInfo->HairType;
		m_CharHairColor=pExtraInfo->HairColor;
		m_CharBeardType=pExtraInfo->BeardType;
		m_Equipments[CES_HEAD].ItemID=0;
		m_Equipments[CES_HEAD].ItemDisplayID=pExtraInfo->HeadEuipment;
		m_Equipments[CES_SHOULDER].ItemID=0;
		m_Equipments[CES_SHOULDER].ItemDisplayID=pExtraInfo->ShoulderEuipment;
		m_Equipments[CES_CAPE].ItemID=0;
		m_Equipments[CES_CAPE].ItemDisplayID=pExtraInfo->CapeEuipment;
		m_Equipments[CES_SHIRT].ItemID=0;
		m_Equipments[CES_SHIRT].ItemDisplayID=pExtraInfo->ShirtEuipment;
		m_Equipments[CES_LEG].ItemID=0;
		m_Equipments[CES_LEG].ItemDisplayID=pExtraInfo->LegEuipment;
		m_Equipments[CES_BOOT].ItemID=0;
		m_Equipments[CES_BOOT].ItemDisplayID=pExtraInfo->BootEuipment;
		m_Equipments[CES_BUST].ItemID=0;
		m_Equipments[CES_BUST].ItemDisplayID=pExtraInfo->BustEuipment;
		m_Equipments[CES_WRIST].ItemID=0;
		m_Equipments[CES_WRIST].ItemDisplayID=pExtraInfo->WristEuipment;
		m_Equipments[CES_HAND].ItemID=0;
		m_Equipments[CES_HAND].ItemDisplayID=pExtraInfo->HandEuipment;
		m_Equipments[CES_BELT].ItemID=0;
		m_Equipments[CES_BELT].ItemDisplayID=pExtraInfo->BeltEuipment;
		if(m_Equipments[CES_BUST].ItemDisplayID==0)
			m_Equipments[CES_BUST].ItemDisplayID=m_Equipments[CES_WRIST].ItemDisplayID;
		return true;
	}
	

	return false;

}

bool CD3DWOWM2CharacterModel::BuildEquipmentModel(bool& HairVisible,bool& Facial1Visible,bool& Facial2Visible,bool& Facial3Visible,bool& EarsVisible)
{
	if(m_Equipments[CES_HEAD].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_HEAD].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CBLZWOWDatabase::BLZ_DB_CHAR_RACE_INFO * pRaceInfo=CBLZWOWDatabase::GetInstance()->FindRaceInfo(m_CharRace);
			if(pRaceInfo)
			{
				CEasyString ModelFileName=ITEM_PATH_BY_SLOT[IISI_HEAD];
				ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->LeftModel;
				int Pos=ModelFileName.ReverseFind('.');
				if(Pos>=0)
				{
					ModelFileName=ModelFileName.Left(Pos);
				}				
				ModelFileName=ModelFileName+"_"+pRaceInfo->Abbrev;
				if(m_CharSex==0)
					ModelFileName+="M";
				else
					ModelFileName+="F";
				ModelFileName=ModelFileName+".m2";

				CD3DDummy * pDummy=EnableAttachment(CAI_HELMET);
				if(pDummy)
				{
					m_pHelmetModel=new CD3DWOWM2ItemModel();
					m_pHelmetModel->SetDevice(GetDevice());
					if(m_pHelmetModel->LoadFromFile(ModelFileName))
					{
						m_pHelmetModel->Play(0,0,0,true);
						m_pHelmetModel->SetParent(pDummy);
						//GetRender()->AddObject(m_pHelmetModel);
						m_pHelmetModel->SetItemDisplayID(m_Equipments[CES_HEAD].ItemDisplayID);
						m_pHelmetModel->BuildModel();

						CBLZWOWDatabase::BLZ_DB_HELMET_GEOSET_VISIBLE_INFO * pVisibleInfo=NULL;

						pVisibleInfo=CBLZWOWDatabase::GetInstance()->GetHelmetGeosetVisibleInfo(pItemDisplayInfo->HelmetGeosetVisData1);

						//pVisibleInfo=CBLZWOWDatabase::GetInstance()->GetHelmetGeosetVisibleInfo(pItemDisplayInfo->HelmetGeosetVisData2);

						if(pVisibleInfo)
						{
							HairVisible=((pVisibleInfo->HairVisible>>m_CharRace)&1)==0;
							Facial1Visible=((pVisibleInfo->Facial1Visible>>m_CharRace)&1)==0;
							Facial2Visible=((pVisibleInfo->Facial2Visible>>m_CharRace)&1)==0;
							Facial3Visible=((pVisibleInfo->Facial3Visible>>m_CharRace)&1)==0;
							EarsVisible=((pVisibleInfo->EarsVisible>>m_CharRace)&1)==0;
						}

					}
					else
					{
						SAFE_RELEASE(m_pHelmetModel);
					}
				}
			}
		}		
	}

	if(m_Equipments[CES_SHOULDER].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_SHOULDER].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CEasyString ModelFileName;
			CEasyString SkinFileName;
			int Pos;
			CD3DDummy * pDummy=NULL;

			ModelFileName=ITEM_PATH_BY_SLOT[IISI_SHOULDERS];
			ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->LeftModel;
			Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";

			pDummy=EnableAttachment(CAI_LEFT_SHOULDER);
			if(pDummy)
			{
				m_pLeftShoulderModel=new CD3DWOWM2ItemModel();
				m_pLeftShoulderModel->SetDevice(GetDevice());

				if(m_pLeftShoulderModel->LoadFromFile(ModelFileName))
				{
					m_pLeftShoulderModel->Play(0,0,0,true);
					m_pLeftShoulderModel->SetParent(pDummy);
					//GetRender()->AddObject(m_pLeftShoulderModel);
					m_pLeftShoulderModel->SetItemDisplayID(m_Equipments[CES_SHOULDER].ItemDisplayID);
					m_pLeftShoulderModel->SetItemHandType(CD3DWOWM2ItemModel::IHT_LEFT);
					m_pLeftShoulderModel->BuildModel();
				}
				else
				{
					SAFE_RELEASE(m_pLeftShoulderModel);
				}
			}

			ModelFileName=ITEM_PATH_BY_SLOT[IISI_SHOULDERS];
			ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->RightModel;
			Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";

			pDummy=EnableAttachment(CAI_RIGHT_SHOULDER);
			if(pDummy)
			{
				m_pRightShoulderModel=new CD3DWOWM2ItemModel();
				m_pRightShoulderModel->SetDevice(GetDevice());
				if(m_pRightShoulderModel->LoadFromFile(ModelFileName))
				{
					m_pRightShoulderModel->Play(0,0,0,true);
					m_pRightShoulderModel->SetParent(pDummy);
					//GetRender()->AddObject(m_pRightShoulderModel);
					m_pRightShoulderModel->SetItemDisplayID(m_Equipments[CES_SHOULDER].ItemDisplayID);
					m_pRightShoulderModel->SetItemHandType(CD3DWOWM2ItemModel::IHT_RIGHT);						
					m_pRightShoulderModel->BuildModel();
				}
				else
				{
					SAFE_RELEASE(m_pRightShoulderModel);
				}
			}

		}		
	}


	if(m_Equipments[CES_LEFT_HAND].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_LEFT_HAND].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CEasyString ModelFileName;
			CEasyString SkinFileName;
			int Pos;
			CD3DDummy * pDummy=NULL;

			ModelFileName=ITEM_PATH_BY_SLOT[IISI_ONE_HAND];
			ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->LeftModel;
			Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";

			pDummy=EnableAttachment(CAI_LEFT_PALM2);

			if(pDummy)
			{
				m_pLeftWeaponModel=new CD3DWOWM2ItemModel();
				m_pLeftWeaponModel->SetDevice(GetDevice());
				if(m_pLeftWeaponModel->LoadFromFile(ModelFileName))
				{
					m_pLeftWeaponModel->Play(0,0,0,true);
					m_pLeftWeaponModel->SetParent(pDummy);
					//GetRender()->AddObject(m_pLeftWeaponModel);
					m_pLeftWeaponModel->SetItemDisplayID(m_Equipments[CES_LEFT_HAND].ItemDisplayID);						
					m_pLeftWeaponModel->BuildModel();
				}
				else
				{
					SAFE_RELEASE(m_pLeftWeaponModel);
				}
			}
		}

	}

	if(m_Equipments[CES_RIGHT_HAND].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_RIGHT_HAND].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CEasyString ModelFileName;
			CEasyString SkinFileName;
			int Pos;
			CD3DDummy * pDummy=NULL;

			ModelFileName=ITEM_PATH_BY_SLOT[IISI_ONE_HAND];
			ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->LeftModel;
			Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";

			pDummy=EnableAttachment(CAI_RIGHT_PALM2);
			if(pDummy)
			{
				m_pRightWeaponModel=new CD3DWOWM2ItemModel();
				m_pRightWeaponModel->SetDevice(GetDevice());					
				if(m_pRightWeaponModel->LoadFromFile(ModelFileName))
				{
					m_pRightWeaponModel->Play(0,0,0,true);
					m_pRightWeaponModel->SetParent(pDummy);
					//GetRender()->AddObject(m_pRightWeaponModel);
					m_pRightWeaponModel->SetItemDisplayID(m_Equipments[CES_RIGHT_HAND].ItemDisplayID);						
					m_pRightWeaponModel->BuildModel();
				}
				else
				{
					SAFE_RELEASE(m_pRightWeaponModel);
				}
			}
		}

	}

	if(m_Equipments[CES_SHIELD].ItemDisplayID)
	{

		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_SHIELD].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CEasyString ModelFileName;
			CEasyString SkinFileName;
			int Pos;
			CD3DDummy * pDummy=NULL;

			ModelFileName=ITEM_PATH_BY_SLOT[IISI_SHIELD];
			ModelFileName=ModelFileName+"\\"+pItemDisplayInfo->LeftModel;
			Pos=ModelFileName.ReverseFind('.');
			if(Pos>=0)
			{
				ModelFileName=ModelFileName.Left(Pos);
			}
			ModelFileName=ModelFileName+".m2";

			pDummy=EnableAttachment(CAI_LEFT_WRIST);

			if(pDummy)
			{
				m_pRightShieldModel=new CD3DWOWM2ItemModel();
				m_pRightShieldModel->SetDevice(GetDevice());
				if(m_pRightShieldModel->LoadFromFile(ModelFileName))
				{
					m_pRightShieldModel->Play(0,0,0,true);
					m_pRightShieldModel->SetParent(pDummy);
					m_pRightShieldModel->SetItemDisplayID(m_Equipments[CES_SHIELD].ItemDisplayID);						
					m_pRightShieldModel->BuildModel();
				}
				else
				{
					SAFE_RELEASE(m_pRightShieldModel);
				}
			}
		}

	}
	return true;
}

bool CD3DWOWM2CharacterModel::RebuildSubMesh(bool HairVisible,bool Facial1Visible,bool Facial2Visible,bool Facial3Visible,bool EarsVisible,bool& HaveSleeve)
{
	int CharHairSubMesh=CBLZWOWDatabase::GetInstance()->FindCharHairSubMesh(m_CharRace,m_CharSex,m_CharHairType,m_IsCharBald);

	int CharWhiskerSubMeshID1=0;
	int CharWhiskerSubMeshID2=0;
	int CharWhiskerSubMeshID3=0;
	CBLZWOWDatabase::GetInstance()->FindCharWhiskerSubMesh(m_CharRace,m_CharSex,m_CharBeardType,
		CharWhiskerSubMeshID1,CharWhiskerSubMeshID2,CharWhiskerSubMeshID3);

	int CapeSubMeshType=1;
	
	int GloveType=1;
	int PantsType=1;
	int RobeType=1;
	int BootsType=1;
	int SleeveType=1;
	int EnsignType=1;

	if(m_Equipments[CES_CAPE].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_CAPE].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			CapeSubMeshType=pItemDisplayInfo->GloveGeosetFlags+1;
		}		
	}

	if(m_Equipments[CES_HAND].ItemDisplayID)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_HAND].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			GloveType=pItemDisplayInfo->GloveGeosetFlags+1;
		}
	}


	if(m_Equipments[CES_LEG].ItemDisplayID)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_LEG].ItemDisplayID);
		if(pItemDisplayInfo)
		{
			PantsType=pItemDisplayInfo->PantsGeosetFlags+1;
			RobeType=pItemDisplayInfo->RobeGeosetFlags+1;
		}
	}

	if(m_Equipments[CES_BOOT].ItemDisplayID)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_BOOT].ItemDisplayID);
		if(pItemDisplayInfo)
		{				
			BootsType=pItemDisplayInfo->GloveGeosetFlags+1;					
		}
	}

	if(m_Equipments[CES_BUST].ItemDisplayID)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_BUST].ItemDisplayID);
		if(pItemDisplayInfo)
		{				
			SleeveType=pItemDisplayInfo->GloveGeosetFlags+1;
			if(pItemDisplayInfo->RobeGeosetFlags+1>1)
				RobeType=pItemDisplayInfo->RobeGeosetFlags+1;
		}
	}
	if(m_Equipments[CES_ENSIGN].ItemDisplayID)
	{
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_ENSIGN].ItemDisplayID);
		if(pItemDisplayInfo)
		{				
			EnsignType=pItemDisplayInfo->GloveGeosetFlags+1;
		}
	}

	if(RobeType>1)
	{		
		PantsType=0;
		BootsType=0;
		EnsignType=0;
	}
	if(GloveType>1)
	{
		SleeveType=1;
	}
	if(BootsType>1)
	{
		if(PantsType==2)
			PantsType=1;
	}

	HaveSleeve=SleeveType>1;

	for(int i=0;i<m_pModelResource->GetSubMeshCount();i++)
	{
		CD3DSubMesh * pD3DSubMesh=m_pModelResource->GetSubMesh(i);

		//pD3DSubMesh->SetVisible(false);

		int Index=pD3DSubMesh->GetID()/10000;
		int Part=(pD3DSubMesh->GetID()%10000)/100;
		int Type=pD3DSubMesh->GetID()%100;

		if(m_CharRace==RACE_WORGEN&&(Index==3||Index==4||Index==73||Index==82))
			continue;
		if(m_CharRace==RACE_GOBLIN&&Index==3)
			continue;

		switch(Part)
		{
		case CSP_HAIR:
			if(Type==0)
			{
				m_SubMeshList.Add(pD3DSubMesh);
				
			}
			else
			{
				if(Type==CharHairSubMesh&&HairVisible)
				{
					m_SubMeshList.Add(pD3DSubMesh);
				}				
			}
			break;
		case CSP_WHISKER1:
			if(Type==CharWhiskerSubMeshID1&&Facial1Visible)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_WHISKER2:
			if(Type==CharWhiskerSubMeshID2&&Facial2Visible)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_WHISKER3:
			if(Type==CharWhiskerSubMeshID3&&Facial2Visible)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_GLOVE:
			if(Type==GloveType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_FOOT:
			if(Type==BootsType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_EAR:
			if(EarsVisible)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_SLEEVE:
			if(Type==SleeveType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_PAINTS:
			if(Type==PantsType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_ENSIGN:
			if(Type==EnsignType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_ROBE:
			if(Type==RobeType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
			break;
		case CSP_BACK:			
			if(Type==CapeSubMeshType)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}					
			break;
		default:
			if(Type==1)
			{
				m_SubMeshList.Add(pD3DSubMesh);
			}	
		}	
	}

	return true;
}

bool CD3DWOWM2CharacterModel::MakeCharSkinTexture(CD3DDevice * pD3DDevice,CD3DTexture *& pCharSkinTexture,CD3DTexture *& pCharSkinExtraTexture,
												  CD3DTexture *& pCharHairTexture,CD3DTexture *& pCapeTexture,
												  CD3DTexture *& pSkinTexture1,CD3DTexture *& pSkinTexture2,CD3DTexture *& pSkinTexture3,
												  bool HaveSleeve)
{
		
	CBLZWOWDatabase::BLZ_DB_CREATURE_DISPLAY_INFO * pDisplayInfo=CBLZWOWDatabase::GetInstance()->FindCreatureDisplayInfo(m_CreatureDisplayID);
	if(pDisplayInfo)
	{
		CBLZWOWDatabase::BLZ_DB_CREATURE_MODEL_INFO * pModelInfo=CBLZWOWDatabase::GetInstance()->FindCreatureModelInfo(pDisplayInfo->ModelID);
		if(pModelInfo)
		{
			if(!pDisplayInfo->Skin1.IsEmpty())
			{
				CEasyString SkinTextureFileName = CFileTools::GetPathDirectory(pModelInfo->ModelPath);
				SkinTextureFileName+=pDisplayInfo->Skin1;
				SkinTextureFileName+=".blp";
				pSkinTexture1=GetDevice()->GetTextureManager()->LoadTexture(SkinTextureFileName);
			}
			if(!pDisplayInfo->Skin2.IsEmpty())
			{
				CEasyString SkinTextureFileName = CFileTools::GetPathDirectory(pModelInfo->ModelPath);
				SkinTextureFileName+=pDisplayInfo->Skin2;
				SkinTextureFileName+=".blp";
				pSkinTexture2=GetDevice()->GetTextureManager()->LoadTexture(SkinTextureFileName);
			}
			if(!pDisplayInfo->Skin3.IsEmpty())
			{
				CEasyString SkinTextureFileName = CFileTools::GetPathDirectory(pModelInfo->ModelPath);
				SkinTextureFileName+=pDisplayInfo->Skin3;
				SkinTextureFileName+=".blp";
				pSkinTexture3=GetDevice()->GetTextureManager()->LoadTexture(SkinTextureFileName);
			}
		}
	}

	if(m_Equipments[CES_CAPE].ItemDisplayID)
	{		
		CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
			CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[CES_CAPE].ItemDisplayID);
		if(pItemDisplayInfo)
		{			
			CEasyString CapeTextureFileName=ITEM_PATH_BY_SLOT[IISI_BACK];
			CapeTextureFileName=CapeTextureFileName+"\\"+pItemDisplayInfo->LeftModelTexture+".blp";
			pCapeTexture=pD3DDevice->GetTextureManager()->LoadTexture(CapeTextureFileName);
		}		
	}
	
	if(m_CharRace>=0&&m_CharSex>=0)
	{
		CSmartPtr<CD3DTexture> pSkinTexture=NULL;
		CSmartPtr<CD3DTexture> pSkinExtraTexture=NULL;	
		CSmartPtr<CD3DTexture> pHairTexture=NULL;

		CBLZWOWDatabase::BLZ_DB_CHAR_SECTION * pSkinSection=CBLZWOWDatabase::GetInstance()->FindCharSection(0,m_CharRace,m_CharSex,0,m_CharSkinColor);
		if(pSkinSection==NULL)
			return false;
		pSkinTexture=new CD3DTexture(pD3DDevice->GetTextureManager());
		CEasyString Name;
		Name.Format(_T("%s\\SKINTEX"),m_pModelResource->GetName());
		pSkinTexture->SetName(Name);
		if(!pSkinTexture->LoadTexture(pSkinSection->Texture1))
		{			
			return false;
		}

		int PosRate=1;

		if(pSkinTexture->GetTextureInfo().Width==256&&pSkinTexture->GetTextureInfo().Height==256)
		{
			PosRate=1;
		}
		else if(pSkinTexture->GetTextureInfo().Width==512&&pSkinTexture->GetTextureInfo().Height==512)
		{
			PosRate=2;
		}
		else
		{
			return false;
		}

		if(!pSkinSection->Texture2.IsEmpty())
		{
			pSkinExtraTexture=pD3DDevice->GetTextureManager()->LoadTexture(pSkinSection->Texture2);	
		}		

		CBLZWOWDatabase::BLZ_DB_CHAR_SECTION * pFaceSection=CBLZWOWDatabase::GetInstance()->FindCharSection(1,m_CharRace,m_CharSex,m_CharFaceType,m_CharSkinColor);
		if(pFaceSection)
		{		
			if(!pFaceSection->Texture1.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pFaceSection->Texture1);
				AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,0,192*PosRate,false);
			}
			if(!pFaceSection->Texture2.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pFaceSection->Texture2);	
				AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,0,160*PosRate,false);
			}
		}



		CBLZWOWDatabase::BLZ_DB_CHAR_SECTION * pWhiskerSection=CBLZWOWDatabase::GetInstance()->FindCharSection(2,m_CharRace,m_CharSex,m_CharBeardType,m_CharHairColor);
		if(pWhiskerSection)
		{		
			if(!pWhiskerSection->Texture1.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pWhiskerSection->Texture1);
				AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,0,192*PosRate,true);
			}
			if(!pWhiskerSection->Texture2.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pWhiskerSection->Texture2);	
				AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,0,160*PosRate,true);
			}
		}

		CBLZWOWDatabase::BLZ_DB_CHAR_SECTION * pHairSection=CBLZWOWDatabase::GetInstance()->FindCharSection(3,m_CharRace,m_CharSex,m_CharHairType,m_CharHairColor);
		if(pHairSection)
		{		
			if(!pHairSection->Texture1.IsEmpty())
			{
				pHairTexture=pD3DDevice->GetTextureManager()->LoadTexture(pHairSection->Texture1);			
			}
			if(!pHairSection->Texture2.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pHairSection->Texture2);
				AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,0,192*PosRate,true);
			}
			if(!pHairSection->Texture3.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pHairSection->Texture3);
				AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,0,160*PosRate,true);
			}
		}

		CBLZWOWDatabase::BLZ_DB_CHAR_SECTION * pUnderwearSection=CBLZWOWDatabase::GetInstance()->FindCharSection(4,m_CharRace,m_CharSex,0,m_CharSkinColor);
		if(pUnderwearSection)
		{
			if(!pUnderwearSection->Texture1.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pUnderwearSection->Texture1);
				AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,128*PosRate,96*PosRate,false);
			}
			if(!pUnderwearSection->Texture2.IsEmpty())
			{
				CSmartPtr<CD3DTexture> pTexture=pD3DDevice->GetTextureManager()->LoadTexture(pUnderwearSection->Texture2);
				AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,128*PosRate,0,false);
			}
		}	

		for(int i=0;i<CES_MAX;i++)
		{
			if(m_Equipments[i].ItemDisplayID)
			{
				CBLZWOWDatabase::BLZ_DB_ITEM_DISPLAY_INFO * pItemDisplayInfo=
					CBLZWOWDatabase::GetInstance()->GetItemDisplayInfo(m_Equipments[i].ItemDisplayID);
				if(pItemDisplayInfo)
				{		

					if(!pItemDisplayInfo->UpperArmTexture.IsEmpty()&&(i==CES_SHIRT||i==CES_BUST))
					{
						CEasyString TextureFileName=EQUIPMENT_ARM_UPPER_TEXTURE_PATH;						
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->UpperArmTexture;						
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,0,0,false);
					}
					if((!HaveSleeve)||i==CES_BUST||i==CES_SHIRT)
					{
						if(!pItemDisplayInfo->LowerArmTexture.IsEmpty()&&(i==CES_WRIST||i==CES_SHIRT||i==CES_BUST||i==CES_HAND))
						{
							CEasyString TextureFileName=EQUIPMENT_ARM_LOWER_TEXTURE_PATH;
							TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->LowerArmTexture;
							CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
							AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,0,64*PosRate,false);
						}
					}

					if(!pItemDisplayInfo->HandsTexture.IsEmpty()&&(i==CES_HAND))
					{
						CEasyString TextureFileName=EQUIPMENT_HAND_TEXTURE_PATH;
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->HandsTexture;
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,0,128*PosRate,false);
					}

					if(!pItemDisplayInfo->UpperTorsoTexture.IsEmpty()&&(i==CES_BUST||i==CES_SHIRT||i==CES_ENSIGN))
					{
						CEasyString TextureFileName=EQUIPMENT_TORSO_UPPER_TEXTURE_PATH;						
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->UpperTorsoTexture;						
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,128*PosRate,0,false);
					}

					if(!pItemDisplayInfo->LowerTorsoTexture.IsEmpty()&&(i==CES_BUST||i==CES_SHIRT||i==CES_ENSIGN))
					{
						CEasyString TextureFileName=EQUIPMENT_TORSO_LOWER_TEXTURE_PATH;						
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->LowerTorsoTexture;						
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,128*PosRate,64*PosRate,false);
					}


					if(!pItemDisplayInfo->UpperLegTexture.IsEmpty()&&(i==CES_BUST||i==CES_BELT||i==CES_LEG))
					{
						CEasyString TextureFileName=EQUIPMENT_LEG_UPPER_TEXTURE_PATH;
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->UpperLegTexture;
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,128*PosRate,96*PosRate,false);
					}

					if(!pItemDisplayInfo->LowerLegTexture.IsEmpty()&&(i==CES_BUST||i==CES_LEG||i==CES_BOOT))
					{
						CEasyString TextureFileName=EQUIPMENT_LEG_LOWER_TEXTURE_PATH;
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->LowerLegTexture;
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,64*PosRate,128*PosRate,160*PosRate,false);
					}


					if(!pItemDisplayInfo->FootTexture.IsEmpty()&&(i==CES_LEG||i==CES_BOOT))
					{
						CEasyString TextureFileName=EQUIPMENT_FOOT_TEXTURE_PATH;
						TextureFileName=TextureFileName+"\\"+pItemDisplayInfo->FootTexture;
						CSmartPtr<CD3DTexture> pTexture=LoadTextureBySex(pD3DDevice,TextureFileName,m_CharSex);
						AddTexture(pSkinTexture,pTexture,128*PosRate,32*PosRate,128*PosRate,224*PosRate,false);
					}

				}
			}	
		}

		pCharSkinTexture=pSkinTexture;
		pSkinTexture.Detach();
		pCharSkinExtraTexture=pSkinExtraTexture;
		pSkinExtraTexture.Detach();
		pCharHairTexture=pHairTexture;
		pHairTexture.Detach();
	}
	return true;

}

void CD3DWOWM2CharacterModel::AlphaMix(D3D_A8B8G8R8_PIXEL& DestPixel,D3D_A8B8G8R8_PIXEL& SrcPixel,bool UseAlphaBlend)
{
	if(UseAlphaBlend)
	{
		DestPixel.Red=(DestPixel.Red*(255-SrcPixel.Alpha)+SrcPixel.Red*SrcPixel.Alpha)/255;
		DestPixel.Green=(DestPixel.Green*(255-SrcPixel.Alpha)+SrcPixel.Green*SrcPixel.Alpha)/255;
		DestPixel.Blue=(DestPixel.Blue*(255-SrcPixel.Alpha)+SrcPixel.Blue*SrcPixel.Alpha)/255;	
	}
	else
	{
		if(SrcPixel.Alpha>=128)
		{
			DestPixel.Red=SrcPixel.Red;
			DestPixel.Green=SrcPixel.Green;
			DestPixel.Blue=SrcPixel.Blue;	
		}
	}
}

bool CD3DWOWM2CharacterModel::AddTexture(CD3DTexture * pDestTexture,CD3DTexture * pSrcTexture,UINT SrcWifth,UINT SrcHeight,UINT DestOffsetX,UINT DestOffsetY,bool UseAlphaBlend)
{
	if((pDestTexture->GetTextureInfo().Format==D3DFMT_X8B8G8R8||
		pDestTexture->GetTextureInfo().Format==D3DFMT_A8B8G8R8))
	{
		int MipLevels=pDestTexture->GetTextureInfo().MipLevels;
		for(int i=0;i<MipLevels;i++)
		{
			D3DLOCKED_RECT DestLockRect;			
			if(pDestTexture->GetD3DTexture()->LockRect(i,&DestLockRect,NULL,0)==D3D_OK)
			{			

				MixTexture(pSrcTexture,i,SrcWifth,SrcHeight,(BYTE *)DestLockRect.pBits,DestLockRect.Pitch,DestOffsetX,DestOffsetY,UseAlphaBlend);

				pDestTexture->GetD3DTexture()->UnlockRect(i);
			}

		}

		return true;
	}	
	return false;
}

bool CD3DWOWM2CharacterModel::MixTexture(CD3DTexture * pSrcTexture,int MipLevel,UINT SrcWidth,UINT SrcHeight,BYTE * pDestPixels,UINT DestPitch,UINT DestOffsetX,UINT DestOffsetY,bool UseAlphaBlend)
{
	if(pSrcTexture!=NULL)
	{
		if(pSrcTexture->GetTextureInfo().Width<=SrcWidth&&
			pSrcTexture->GetTextureInfo().Height<=SrcHeight)
		{
			int Rate=SrcWidth/pSrcTexture->GetTextureInfo().Width;
			if(pSrcTexture->GetTextureInfo().Width*Rate!=SrcWidth||
				pSrcTexture->GetTextureInfo().Height*Rate!=SrcHeight)
			{
				return false;
			}
			UINT OffsetX=(DestOffsetX>>MipLevel);
			UINT OffsetY=(DestOffsetY>>MipLevel);
			UINT Width=(SrcWidth>>MipLevel);
			UINT Height=(SrcHeight>>MipLevel);
			if(Width==0)
				Width=1;
			if(Height==0)
				Height=1;
			D3DLOCKED_RECT SrcLockRect;
			if(pSrcTexture->GetD3DTexture()->LockRect(MipLevel,&SrcLockRect,NULL,0)==D3D_OK)
			{								
				pDestPixels=pDestPixels+DestPitch*OffsetY;
				BYTE * pSrcPixels=(BYTE *)SrcLockRect.pBits;
				for(UINT y=0;y<Height;y++)
				{
					D3D_A8B8G8R8_PIXEL * pDestLine=(D3D_A8B8G8R8_PIXEL *)(pDestPixels+sizeof(D3D_A8B8G8R8_PIXEL)*OffsetX);
					D3D_A8B8G8R8_PIXEL * pSrcLine=(D3D_A8B8G8R8_PIXEL *)(pSrcPixels+SrcLockRect.Pitch*(y/Rate));
					if(pSrcTexture->GetTextureInfo().Format==D3DFMT_A8B8G8R8)
					{
						for(UINT x=0;x<Width;x++)
						{
							AlphaMix(pDestLine[x],pSrcLine[x/Rate],UseAlphaBlend);
						}
					}	
					else
					{
						for(UINT x=0;x<Width;x++)
						{
							pDestLine[x]=pSrcLine[x/Rate];
						}
					}
												
					pDestPixels+=DestPitch;
				}							
				pSrcTexture->GetD3DTexture()->UnlockRect(MipLevel);
				return true;
			}
		}
	}
	return false;
}

CD3DTexture * CD3DWOWM2CharacterModel::LoadTextureBySex(CD3DDevice * pD3DDevice,LPCTSTR TextureFileName,int Sex)
{
	CEasyString FileName=TextureFileName;
	if(Sex==0)
		FileName+="_M.blp";
	else
		FileName+="_F.blp";
	CD3DTexture * pTexture=pD3DDevice->GetTextureManager()->LoadTexture(FileName);
	if(pTexture==NULL)
	{
		FileName=TextureFileName;
		FileName+="_U.blp";
		pTexture=pD3DDevice->GetTextureManager()->LoadTexture(FileName);
	}
	return pTexture;
}

void CD3DWOWM2CharacterModel::FetchAnimationFrames(UINT Time)
{
	CD3DWOWM2Model::FetchAnimationFrames(Time);
	if(m_Equipments[CES_LEFT_HAND].ItemDisplayID)
	{
		for(UINT i=BONE_LFINGER1;i<=BONE_LTHUMB;i++)
		{
			int Bone=m_pModelResource->GetKeyBone(i);
			if(Bone>=0)
			{					
				m_pModelResource->MakeAnimationBoneFrameByTree(m_CloseHandAnimationIndex,32,false,Bone,					
					m_BoneMatrices.GetBuffer(),m_BoneMatrices.GetCount());
			}
		}		
	}
	if(m_Equipments[CES_RIGHT_HAND].ItemDisplayID)
	{
		for(UINT i=BONE_RFINGER1;i<=BONE_RTHUMB;i++)
		{
			int Bone=m_pModelResource->GetKeyBone(i);
			if(Bone>=0)
			{
				m_pModelResource->MakeAnimationBoneFrameByTree(m_CloseHandAnimationIndex,32,false,Bone,				
					m_BoneMatrices.GetBuffer(),m_BoneMatrices.GetCount());
			}
		}		
	}
}


}