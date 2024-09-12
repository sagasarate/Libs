/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2RibbonEmitter.cpp                                */
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

IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DRibbonEmitter)
IMPLEMENT_CLASS_INFO(CD3DRibbonEmitter,CD3DBaseDynamicModel);

CD3DRibbonEmitter::CD3DRibbonEmitter(void)
{
	m_pModelResource=NULL;
	m_MaxRibbonCount=0;
	m_pRibbonVertexBuffer=NULL;
	m_RibbonCount=0;
	m_IsPlaying=false;
	m_StartTime=-1;
	m_RecentCreateTime=0;
	m_PlaySpeedRate=1.0f;

	m_BoundingBox.m_Min.SetValue(-1.0f,-1.0f,-1.0f);
	m_BoundingBox.m_Max.SetValue(1.0f,1.0f,1.0f);
}

CD3DRibbonEmitter::~CD3DRibbonEmitter(void)
{
	Destory();
}

void CD3DRibbonEmitter::Destory()
{
	CD3DObject::Destory();

	SAFE_RELEASE(m_pModelResource);
	m_MaxRibbonCount=0;
	m_RibbonCount=0;
	m_IsPlaying=false;
	m_StartTime=-1;
	m_RecentCreateTime=0;
	m_PlaySpeedRate=1.0f;
	SAFE_DELETE_ARRAY(m_pRibbonVertexBuffer);
	m_SubMesh.Destory();
	
}

bool CD3DRibbonEmitter::Reset()
{
	return m_SubMesh.Reset();
}
bool CD3DRibbonEmitter::Restore()
{
	return m_SubMesh.Restore();
}

bool CD3DRibbonEmitter::Play(bool IsLoop)
{
	m_IsPlaying=true;
	m_StartTime=-1;
	return true;
}
bool CD3DRibbonEmitter::Stop()
{
	m_IsPlaying=false;
	return true;
}
bool CD3DRibbonEmitter::IsPlaying()
{
	return m_IsPlaying;
}

void CD3DRibbonEmitter::SetPlaySpeed(FLOAT Rate)
{
	m_PlaySpeedRate=Rate;
}

int CD3DRibbonEmitter::GetSubMeshCount()
{
	return 1;
}
CD3DSubMesh * CD3DRibbonEmitter::GetSubMesh(UINT index)
{
	return &m_SubMesh;
}

CD3DBoundingBox * CD3DRibbonEmitter::GetBoundingBox()
{
	return &m_BoundingBox;
}
CD3DBoundingSphere * CD3DRibbonEmitter::GetBoundingSphere()
{	
	return NULL;
}

bool CD3DRibbonEmitter::CanDoSubMeshViewCull()
{
	return false;
}

bool CD3DRibbonEmitter::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DRibbonEmitter)))
		return false;

	if(!CD3DObject::CloneFrom(pObject,Param))
		return false;

	CD3DRibbonEmitter * pSrcObject=(CD3DRibbonEmitter *)pObject;

	SetDevice(pSrcObject->GetDevice());

	if(!LoadFromResource(pSrcObject->m_pModelResource))
		return false;

	

	m_RibbonCount=pSrcObject->m_RibbonCount;
	m_IsPlaying=pSrcObject->m_IsPlaying;
	m_StartTime=pSrcObject->m_StartTime;
	m_RecentCreateTime=pSrcObject->m_RecentCreateTime;
	m_ClipLength=pSrcObject->m_ClipLength;	
	m_PlaySpeedRate=pSrcObject->m_PlaySpeedRate;

	memcpy(m_pRibbonVertexBuffer,pSrcObject->m_pRibbonVertexBuffer,sizeof(RIBBON_PAIR)*m_RibbonCount);

	m_SubMesh.SetVertexCount(m_RibbonCount*2);		
	m_SubMesh.SetPrimitiveCount((m_RibbonCount-1)*2);

	return true;
}

void CD3DRibbonEmitter::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_pModelResource)
	{
		m_pModelResource->PickResource(pResourceManager,Param);
		pResourceManager->AddResource(m_pModelResource);		
	}
}
bool CD3DRibbonEmitter::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseDynamicModel::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRE_MODEL_RESOURCE,m_pModelResource->GetName()));

	return true;
}
bool CD3DRibbonEmitter::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseDynamicModel::FromSmartStruct(Packet,pResourceManager,Param))
		return false;
	CD3DRibbonEmitterResource * pModelResource=NULL;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DRE_MODEL_RESOURCE:
			{
				LPCTSTR szResourceName=Value;
				pModelResource=(CD3DRibbonEmitterResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DRibbonEmitterResource));
				if(pModelResource)
				{
					pModelResource->AddUseRef();
				}
			}
			break;	
		}
	}
	return LoadFromResource(pModelResource);
}
UINT CD3DRibbonEmitter::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DBaseDynamicModel::GetSmartStructSize(Param);
	Size += CSmartStruct::GetStringMemberSize(m_pModelResource->GetNameLength());

	return Size;
}

void CD3DRibbonEmitter::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
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
	//pFX->SetColor("FogColor",pRender->GetFogColor());
	//pFX->SetFloat("FogNear",pRender->GetFogNear());
	//pFX->SetFloat("FogFar",pRender->GetFogFar());
}
void CD3DRibbonEmitter::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	////设置材质
	//D3DMATERIAL9 * pD3DMaterial;
	//if(pSubMesh->IsSelected())
	//	pD3DMaterial=&SELECTED_SUBMESH_MATERIAL;
	//else
	//	pD3DMaterial=&(pMaterial->GetMaterial());
	//pFX->SetColor("MaterialAmbient",pD3DMaterial->Ambient);
	//pFX->SetColor("MaterialDiffuse",pD3DMaterial->Diffuse);
	//pFX->SetColor("MaterialSpecular",pD3DMaterial->Specular);
	//pFX->SetColor("MaterialEmissive",pD3DMaterial->Emissive);
	//pFX->SetFloat("MaterialPower",pD3DMaterial->Power);

	//设置纹理
	//pFX->SetTexture("TexLay0",pMaterial->GetTexture(0));

	//设置视投影矩阵
	//pFX->SetMatrix("ViewMatrix",pCamera->GetViewMatR());
	//pFX->SetMatrix("ProjMatrix",pCamera->GetProjectMatR());

	if(pSubMesh->IsSelected())			
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}
}




void CD3DRibbonEmitter::Update(FLOAT Time)
{
	CD3DObject::Update(Time);

	//Time=Time/100;
	
	

	int CreateCount=0;
		
	if(m_pModelResource)
	{
		CD3DRibbonEmitterResource::RIBBON_EMITTER_INFO * pRibbonEmitterInfo=m_pModelResource->GetRibbonEmitterInfo();

		
		if(IsVisible()&&(!CheckFlag(CD3DObject::OBJECT_FLAG_CULLED)))
		{	
			if(m_IsPlaying)
			{
				if(m_StartTime<0)
				{
					m_StartTime=Time;		
					m_RibbonCount=0;
					m_RecentCreateTime=Time;
					CD3DRibbonEmitterResource::RIBBON_PARAM Param;
					if(m_pModelResource->MakeRibbonParam(0,true,Param))
					{
						BuildRibbon(pRibbonEmitterInfo,&Param,Time);				
						CreateCount++;
					}
				}

				UINT CurTime=(UINT)((Time-m_StartTime)*1000*m_PlaySpeedRate);

				CD3DVector3 Pos(0,0,0);
				Pos=Pos*GetWorldMatrix();
				FLOAT ClipTime=(Time-m_RecentCreateTime)*m_PlaySpeedRate;
				if(ClipTime>m_ClipLength)
				{			
					m_RecentCreateTime=Time;
					CD3DRibbonEmitterResource::RIBBON_PARAM Param;
					if(m_pModelResource->MakeRibbonParam(CurTime,true,Param))
					{
						BuildRibbon(pRibbonEmitterInfo,&Param,Time);					
						CreateCount++;
					}			
				}	
			}
			
			if(m_RibbonCount)
			{			
				CreateCount+=DelRibbon(Time,pRibbonEmitterInfo->Length);
			}
		}
		
	}
	
	if(CreateCount)
	{
		m_SubMesh.SetVertexCount(m_RibbonCount*2);		
		m_SubMesh.SetPrimitiveCount((m_RibbonCount-1)*2);
		m_IsRenderDataChanged=true;
	}

	

	//PrintD3DLog("Particle=%u,%u",CreateCount,DeleteCount);
}

bool CD3DRibbonEmitter::NeedUpdateAni()
{
	return true;
}

bool CD3DRibbonEmitter::LoadFromResource(CD3DRibbonEmitterResource * pModelResource)
{
	if(GetDevice()==NULL)
		return false;
	
	Destory();
	m_pModelResource=pModelResource;
	m_pModelResource->AddUseRef();
	

	CD3DRibbonEmitterResource::RIBBON_EMITTER_INFO * pRibbonEmitterInfo=m_pModelResource->GetRibbonEmitterInfo();
	if(pRibbonEmitterInfo==NULL)
		return false;

	m_ClipLength=(pRibbonEmitterInfo->Length/pRibbonEmitterInfo->Resolution)*GetWorldMatrix().GetScale().x;
	//m_ClipLength=(pRibbonEmitterInfo->Length)*GetWorldMatrix().GetScale().x;

	m_MaxRibbonCount=(UINT)pRibbonEmitterInfo->Resolution+1;

	if(m_MaxRibbonCount>MAX_RIBBON_COUNT)
		m_MaxRibbonCount=MAX_RIBBON_COUNT;	
	m_pRibbonVertexBuffer=new RIBBON_PAIR[m_MaxRibbonCount];

	D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};
	D3DCOLORVALUE GrayColor={0.8f,0.8f,0.8f,1.0f};
	D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};

	m_SubMesh.GetMaterial().GetMaterial().Ambient=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Diffuse=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Specular=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Emissive=BlackColor;
	m_SubMesh.GetMaterial().GetMaterial().Power=40.0f;


	m_SubMesh.GetVertexFormat().FVF=D3DFVF_XYZB1|D3DFVF_DIFFUSE|D3DFVF_TEX1;
	m_SubMesh.GetVertexFormat().VertexSize=sizeof(RIBBON_VERTEX);
	m_SubMesh.GetVertexFormat().IndexSize=0;
	m_SubMesh.SetPrimitiveType(D3DPT_TRIANGLESTRIP);	
	m_SubMesh.SetVertices((BYTE *)m_pRibbonVertexBuffer);
	m_SubMesh.SetVertexCount(m_MaxRibbonCount*2);
	m_SubMesh.AllocVertexBufferR();
	m_SubMesh.SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
	m_SubMesh.SetVertexCount(0);
	m_SubMesh.AddProperty(CD3DSubMesh::SMF_IS_ANI_MESH);
	
	for(UINT i=0;i<m_MaxRibbonCount;i++)
	{
		m_pRibbonVertexBuffer[i].Vertex[0].Tex.x=0.0f;
		m_pRibbonVertexBuffer[i].Vertex[0].Tex.y=0.0f;
		m_pRibbonVertexBuffer[i].Vertex[1].Tex.x=0.0f;
		m_pRibbonVertexBuffer[i].Vertex[1].Tex.y=1.0f;		
	}
	for(UINT i=0;i<pRibbonEmitterInfo->Textures.GetCount();i++)
	{
		m_SubMesh.GetMaterial().AddTexture(pRibbonEmitterInfo->Textures[i],0,"TexLay0","");
		if(pRibbonEmitterInfo->Textures[i])
			pRibbonEmitterInfo->Textures[i]->AddUseRef();
	}
	

	m_SubMesh.GetMaterial().SetFX(pRibbonEmitterInfo->pFX);
	if(pRibbonEmitterInfo->pFX)
		pRibbonEmitterInfo->pFX->AddUseRef();
	
	m_SubMesh.SetTransparent(pRibbonEmitterInfo->IsTransparent);

	m_SubMesh.GetMaterial().CaculateHashCode();

	m_RibbonCount=0;
	return true;
}

void CD3DRibbonEmitter::BuildRibbon(CD3DRibbonEmitterResource::RIBBON_EMITTER_INFO * pRibbonEmitterInfo,CD3DRibbonEmitterResource::RIBBON_PARAM * pParam,FLOAT Time)
{		
	if(m_RibbonCount>=m_MaxRibbonCount)//||RibbonLen>=m_ClipLength*pRibbonEmitterInfo->Resolution)
	{
		for(UINT i=0;i<m_RibbonCount-1;i++)
		{
			m_pRibbonVertexBuffer[i].Vertex[0]=m_pRibbonVertexBuffer[i+1].Vertex[0];
			m_pRibbonVertexBuffer[i].Vertex[1]=m_pRibbonVertexBuffer[i+1].Vertex[1];
		}
		
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[0].Pos=CD3DVector3(pParam->Above,0,0)*GetWorldMatrix();
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[0].Color=pParam->Color;
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[0].CreateTime=Time;
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[1].Pos=CD3DVector3(-pParam->Below,0,0)*GetWorldMatrix();
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[1].Color=pParam->Color;	
		m_pRibbonVertexBuffer[m_RibbonCount-1].Vertex[1].CreateTime=Time;
	}
	else
	{
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[0].Pos=CD3DVector3(pParam->Above,0,0)*GetWorldMatrix();
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[0].Color=pParam->Color;
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[0].CreateTime=Time;
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[1].Pos=CD3DVector3(-pParam->Below,0,0)*GetWorldMatrix();
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[1].Color=pParam->Color;	
		m_pRibbonVertexBuffer[m_RibbonCount].Vertex[1].CreateTime=Time;
		m_RibbonCount++;

		
	}


	for(UINT i=0;i<m_RibbonCount;i++)
	{
		m_pRibbonVertexBuffer[i].Vertex[0].Tex.x=i*1.0f/(m_RibbonCount-1);
		m_pRibbonVertexBuffer[i].Vertex[1].Tex.x=i*1.0f/(m_RibbonCount-1);
	}
	
}


UINT CD3DRibbonEmitter::DelRibbon(FLOAT Time,FLOAT Length)
{		
	UINT DeleteCount=0;

	for(UINT i=0;i<m_RibbonCount;i++)
	{
		if((Time-m_pRibbonVertexBuffer[i].Vertex[0].CreateTime)*m_PlaySpeedRate<Length)
		{
			break;
		}
		DeleteCount++;
	}

	if(DeleteCount)
	{
		m_RibbonCount-=DeleteCount;
		for(UINT i=0;i<m_RibbonCount;i++)
		{
			m_pRibbonVertexBuffer[i].Vertex[0]=m_pRibbonVertexBuffer[i+DeleteCount].Vertex[0];
			m_pRibbonVertexBuffer[i].Vertex[1]=m_pRibbonVertexBuffer[i+DeleteCount].Vertex[1];
			m_pRibbonVertexBuffer[i].Vertex[0].Tex.x=i*1.0f/(m_RibbonCount-1);
			m_pRibbonVertexBuffer[i].Vertex[1].Tex.x=i*1.0f/(m_RibbonCount-1);
		}
	}
	
	return DeleteCount;
}

}