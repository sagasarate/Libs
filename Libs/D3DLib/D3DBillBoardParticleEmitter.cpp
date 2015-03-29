/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2BillBoardParticleEmitter.cpp                     */
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

IMPLEMENT_FILE_CHANNEL_MANAGER(CD3DBillBoardParticleEmitter)
IMPLEMENT_CLASS_INFO(CD3DBillBoardParticleEmitter,CD3DBaseDynamicModel);

UINT CD3DBillBoardParticleEmitter::m_BillBoardParticleEmitterUpdateCount=0;
CD3DBillBoardParticleEmitter::CD3DBillBoardParticleEmitter(void)
{
	m_pModelResource=NULL;
	m_MaxParticleCount=0;	
	m_IsPlaying=false;
	m_pParticleVertexBuffer=NULL;
	m_pParticleIndexBuffer=NULL;
	m_ParticleCount=0;
	m_StartTime=-1;
	m_RecentUpdateTime=0;
	m_PlaySpeedRate=1.0f;
	m_EmitterRate=0;
	m_EnbaleGravityTransform=true;
	m_NoTrail=false;
	m_TextureTileRotation=0;
	m_BoundingBox.m_Min.SetValue(-0.1f,-0.1f,-0.1f);
	m_BoundingBox.m_Max.SetValue(0.1f,0.1f,0.1f);
	
}

CD3DBillBoardParticleEmitter::~CD3DBillBoardParticleEmitter(void)
{
	Destory();
}


void CD3DBillBoardParticleEmitter::Destory()
{
	CD3DObject::Destory();

	SAFE_RELEASE(m_pModelResource);
	m_MaxParticleCount=0;
	m_IsPlaying=false;
	m_ParticleCount=0;
	m_StartTime=-1;
	m_RecentUpdateTime=0;
	m_EmitterRate=0;
	m_EnbaleGravityTransform=true;
	m_NoTrail=false;
	SAFE_DELETE_ARRAY(m_pParticleVertexBuffer);
	SAFE_DELETE_ARRAY(m_pParticleIndexBuffer);
	m_SubMesh.Destory();
	
}

bool CD3DBillBoardParticleEmitter::Reset()
{
	bool Ret=m_SubMesh.Reset();
	if(m_pModelResource)
		Ret=Ret&&m_pModelResource->Reset();
	return Ret;
}
bool CD3DBillBoardParticleEmitter::Restore()
{
	bool Ret=m_SubMesh.Restore();
	if(m_pModelResource)
		Ret=Ret&&m_pModelResource->Restore();
	return Ret;
}

bool CD3DBillBoardParticleEmitter::Play(bool IsLoop)
{
	m_IsPlaying=true;
	m_StartTime=-1;
	return true;
}
bool CD3DBillBoardParticleEmitter::Stop()
{
	m_IsPlaying=false;
	return true;
}
bool CD3DBillBoardParticleEmitter::IsPlaying()
{
	return m_IsPlaying;
}

void CD3DBillBoardParticleEmitter::SetPlaySpeed(FLOAT Rate)
{
	m_PlaySpeedRate=Rate;
}

int CD3DBillBoardParticleEmitter::GetSubMeshCount()
{
	return 1;
}
CD3DSubMesh * CD3DBillBoardParticleEmitter::GetSubMesh(UINT index)
{
	return &m_SubMesh;
}

CD3DBoundingBox * CD3DBillBoardParticleEmitter::GetBoundingBox()
{	
	return &m_BoundingBox;
}
CD3DBoundingSphere * CD3DBillBoardParticleEmitter::GetBoundingSphere()
{	
	return NULL;
}

bool CD3DBillBoardParticleEmitter::CanDoSubMeshViewCull()
{
	return false;
}

bool CD3DBillBoardParticleEmitter::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DBillBoardParticleEmitter)))
		return false;

	if(!CD3DObject::CloneFrom(pObject,Param))
		return false;

	CD3DBillBoardParticleEmitter * pSrcObject=(CD3DBillBoardParticleEmitter *)pObject;

	SetDevice(pSrcObject->GetDevice());

	if(!LoadFromResource(pSrcObject->m_pModelResource,pSrcObject->m_MaxParticleCount))
		return false;

	

		
	m_ParticleCount=pSrcObject->m_ParticleCount;
	m_IsPlaying=pSrcObject->m_IsPlaying;
	m_StartTime=pSrcObject->m_StartTime;
	m_RecentUpdateTime=pSrcObject->m_RecentUpdateTime;
	m_PlaySpeedRate=pSrcObject->m_PlaySpeedRate;
	m_EmitterRate=pSrcObject->m_EmitterRate;	
	
	memcpy(m_pParticleVertexBuffer,pSrcObject->m_pParticleVertexBuffer,sizeof(PARTICLE_RECT)*m_ParticleCount);

	m_SubMesh.SetVertexCount(m_ParticleCount*4);
	m_SubMesh.SetIndexCount(m_ParticleCount*6);
	m_SubMesh.SetPrimitiveCount(m_ParticleCount*2);

	return true;
}

void CD3DBillBoardParticleEmitter::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_pModelResource)
	{
		m_pModelResource->PickResource(pResourceManager,Param);
		pResourceManager->AddResource(m_pModelResource);		
	}
}

bool CD3DBillBoardParticleEmitter::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseDynamicModel::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DBBPE_MODEL_RESOURCE,m_pModelResource->GetName()));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DBBPE_MAX_PARTICLE_COUNT,m_MaxParticleCount));

	return true;
	
}

bool CD3DBillBoardParticleEmitter::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DBaseDynamicModel::FromSmartStruct(Packet,pResourceManager,Param))
		return false;
	CD3DParticleEmitterResource * pModelResource=NULL;
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{	
		case SST_D3DBBPE_MODEL_RESOURCE:
			{
				LPCTSTR szResourceName=Value;
				pModelResource=(CD3DParticleEmitterResource *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DParticleEmitterResource));
				if(pModelResource)
				{
					pModelResource->AddUseRef();
				}
			}
			break;
		case SST_D3DBBPE_MAX_PARTICLE_COUNT:
			m_MaxParticleCount=Value;
			break;
		}
	}
	return LoadFromResource(pModelResource,m_MaxParticleCount);
}
UINT CD3DBillBoardParticleEmitter::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DBaseDynamicModel::GetSmartStructSize(Param);
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(m_pModelResource->GetNameLength());
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_MaxParticleCount));

	return Size;
}

void CD3DBillBoardParticleEmitter::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
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

	if(m_NoTrail)
	{
		pFX->SetMatrix("WorldMatrix",GetWorldMatrixR());
		pFX->SetMatrix("WorldRotation",GetWorldMatrixR().GetRotation());
		pFX->SetVector("WorldScale",GetWorldMatrixR().GetScale());
		pFX->SetVector("WorldTranslation",GetWorldMatrixR().GetTranslation());			
	}
	else
	{
		CD3DMatrix Mat;				
		CD3DVector3 Vec;

		Mat.SetIdentity();
		pFX->SetMatrix("WorldMatrix",Mat);			
		pFX->SetMatrix("WorldRotation",Mat);
		Vec=1.0f;
		pFX->SetVector("WorldScale",Vec);
		Vec=0.0f;
		pFX->SetVector("WorldTranslation",Vec);			
	}

	////设置视投影矩阵
	//pFX->SetMatrix("ViewMatrix",pCamera->GetViewMatR());
	//pFX->SetMatrix("ProjMatrix",pCamera->GetProjectMatR());

	pFX->SetBool("TransformGravity",m_EnbaleGravityTransform);
	pFX->SetInt("TextureTileRotation",m_TextureTileRotation);
}

void CD3DBillBoardParticleEmitter::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
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

	if(pSubMesh->IsSelected())			
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}

}




void CD3DBillBoardParticleEmitter::Update(FLOAT Time)
{
	CD3DObject::Update(Time);	

	//Time=Time/100;

	if(m_StartTime<0)
	{
		m_StartTime=Time;
		m_RecentUpdateTime=Time;
	}
	
	UINT CurTime=(UINT)((Time-m_StartTime)*1000*m_PlaySpeedRate);
	FLOAT TimeSpan=(Time-m_RecentUpdateTime)*m_PlaySpeedRate;
	m_RecentUpdateTime=Time;

	

	int CreateCount=0;
	int DeleteCount=0;

	if(IsVisible()&&(!CheckFlag(CD3DObject::OBJECT_FLAG_CULLED)))
	{		
		m_BillBoardParticleEmitterUpdateCount++;
		if(m_pModelResource&&m_IsPlaying)
		{
			CD3DParticleEmitterResource::PARTICLE_PARAM Param;
			if(m_pModelResource->MakeParticleParam(CurTime,true,Param))
			{
				m_EmitterRate+=Param.EmissionRate*TimeSpan;
				while(m_EmitterRate>=1.0f)
				{
					m_EmitterRate-=1.0f;
					if(m_ParticleCount<m_MaxParticleCount)
					{
						BuildParticle(m_pModelResource->GetParticleEmitterInfo(),&Param);
						CreateCount++;
					}

				}
			}
		}

		for(int i=(int)m_ParticleCount-1;i>=0;i--)
		{
			for(int j=0;j<4;j++)
			{
				m_pParticleVertexBuffer[i].Vertex[j].Info.Life+=TimeSpan;
			}
			if(m_pParticleVertexBuffer[i].Vertex[0].Info.Life>=m_pParticleVertexBuffer[i].Vertex[0].Info.MaxLife)
			{			
				memmove(m_pParticleVertexBuffer+i,m_pParticleVertexBuffer+i+1,sizeof(PARTICLE_RECT)*(m_ParticleCount-i-1));
				DeleteCount++;
				m_ParticleCount--;			
			}
		}

		if(CreateCount||DeleteCount)
		{
			m_SubMesh.SetVertexCount(m_ParticleCount*4);
			m_SubMesh.SetIndexCount(m_ParticleCount*6);
			m_SubMesh.SetPrimitiveCount(m_ParticleCount*2);
			m_IsRenderDataChanged=true;

			//PrintD3DDebugLog(0,"%s粒子数%d",GetName(),m_ParticleCount);
			//m_SubMesh.CheckValid();
		}
	}
	

	//PrintSystemLog(0,"Particle=%u,%u",CreateCount,DeleteCount);

}

bool CD3DBillBoardParticleEmitter::NeedUpdateAni()
{
	return true;
}


bool CD3DBillBoardParticleEmitter::LoadFromResource(CD3DParticleEmitterResource * pModelResource,UINT MaxParticleCount)
{
	if(GetDevice()==NULL)
		return false;
	


	Destory();
	m_pModelResource=pModelResource;
	m_pModelResource->AddUseRef();
	m_MaxParticleCount=MaxParticleCount;

	SetName(m_pModelResource->GetName());

	CD3DParticleEmitterResource::PARTICLE_EMITTER_INFO * pEmitterInfo=m_pModelResource->GetParticleEmitterInfo();

	if(m_MaxParticleCount==0)
	{
		FLOAT MaxEmissionRate=0.0f;
		
		for(UINT i=0;i<pEmitterInfo->EmissionRate.Animations[0].Keys.GetCount();i++)
		{
			if(pEmitterInfo->EmissionRate.Animations[0].Keys[i]>MaxEmissionRate)
			{
				MaxEmissionRate=pEmitterInfo->EmissionRate.Animations[0].Keys[i];
			}
		}
		FLOAT MaxLifeSpan=0.0f;
		for(UINT i=0;i<pEmitterInfo->LifeSpan.Animations[0].Keys.GetCount();i++)
		{
			if(pEmitterInfo->LifeSpan.Animations[0].Keys[i]>MaxLifeSpan)
			{
				MaxLifeSpan=pEmitterInfo->LifeSpan.Animations[0].Keys[i];
			}
		}
		m_MaxParticleCount=(UINT)(MaxEmissionRate*MaxLifeSpan*2.0f);
	}
	
	if(m_MaxParticleCount>MAX_PARTICLE_COUNT)
		m_MaxParticleCount=MAX_PARTICLE_COUNT;	
	if(m_MaxParticleCount<MIN_PARTICLE_COUNT)
		m_MaxParticleCount=MIN_PARTICLE_COUNT;
	m_pParticleVertexBuffer=new PARTICLE_RECT[m_MaxParticleCount];
	m_pParticleIndexBuffer=new WORD[m_MaxParticleCount*6];

	D3DVERTEXELEMENT9 ParticleVertexElements[] = 
	{
		{0, 0,  D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_POSITION,	0},
		{0, 16, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_NORMAL,	0},
		{0, 32, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_COLOR,		0},
		{0, 48, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_COLOR,		1},		
		{0, 64, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	0},
		{0, 68, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	1},
		{0, 72, D3DDECLTYPE_D3DCOLOR, D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	2},
		{0, 76, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	3},
		{0, 92,	D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	4},
		{0, 108, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	5},
		{0, 124, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	6},
		{0, 140, D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,		D3DDECLUSAGE_TEXCOORD,	7},
		D3DDECL_END()
	};

	m_SubMesh.SetDevice(GetDevice());
	m_SubMesh.DeclareVertexFormat(ParticleVertexElements,sizeof(PARTICLE_VERTEX),sizeof(WORD));
	m_SubMesh.SetPrimitiveType(D3DPT_TRIANGLELIST);	
	m_SubMesh.SetVertices((BYTE *)m_pParticleVertexBuffer);
	m_SubMesh.SetIndices((BYTE *)m_pParticleIndexBuffer);
	m_SubMesh.SetRenderBufferUsed(CD3DSubMesh::BUFFER_USE_CUSTOM);
	m_SubMesh.SetVertexCount(m_MaxParticleCount*4);
	m_SubMesh.SetIndexCount(m_MaxParticleCount*6);
	m_SubMesh.AllocVertexBufferR();
	m_SubMesh.AllocIndexBufferR();
	m_SubMesh.SetVertexCount(0);
	m_SubMesh.SetIndexCount(0);
	m_SubMesh.AddProperty(CD3DSubMesh::SMF_IS_ANI_MESH);
	for(UINT i=0;i<m_MaxParticleCount;i++)
	{
		
		m_pParticleVertexBuffer[i].Vertex[0].Tex.x=0.0f;
		m_pParticleVertexBuffer[i].Vertex[0].Tex.y=0.0f;
		m_pParticleVertexBuffer[i].Vertex[1].Tex.x=1.0f;
		m_pParticleVertexBuffer[i].Vertex[1].Tex.y=0.0f;
		m_pParticleVertexBuffer[i].Vertex[2].Tex.x=0.0f;
		m_pParticleVertexBuffer[i].Vertex[2].Tex.y=1.0f;
		m_pParticleVertexBuffer[i].Vertex[3].Tex.x=1.0f;
		m_pParticleVertexBuffer[i].Vertex[3].Tex.y=1.0f;			
		
		m_pParticleIndexBuffer[i*6]=i*4;
		m_pParticleIndexBuffer[i*6+1]=i*4+1;
		m_pParticleIndexBuffer[i*6+2]=i*4+2;
		m_pParticleIndexBuffer[i*6+3]=i*4+2;
		m_pParticleIndexBuffer[i*6+4]=i*4+1;
		m_pParticleIndexBuffer[i*6+5]=i*4+3;
	}

	D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};
	D3DCOLORVALUE GrayColor={0.8f,0.8f,0.8f,1.0f};
	D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};

	m_SubMesh.GetMaterial().GetMaterial().Ambient=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Diffuse=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Specular=WhiteColor;
	m_SubMesh.GetMaterial().GetMaterial().Emissive=BlackColor;
	m_SubMesh.GetMaterial().GetMaterial().Power=40.0f;


	m_SubMesh.GetMaterial().AddTexture(pEmitterInfo->pTexture,0,"TexLay0","");
	if(pEmitterInfo->pTexture)
		pEmitterInfo->pTexture->AddUseRef();
	m_SubMesh.GetMaterial().SetFX(pEmitterInfo->pFX);
	if(pEmitterInfo->pFX)
		pEmitterInfo->pFX->AddUseRef();

	if(pEmitterInfo->BlendingType!=EBT_OPACITY&&pEmitterInfo->BlendingType!=EBT_ALPHA_TEST)
	{
		m_SubMesh.SetTransparent(true);
	}

	m_SubMesh.GetMaterial().CaculateHashCode();

	if(pEmitterInfo->Flags&CD3DParticleEmitterResource::PEF_GRAVITY_TRANSFORM)
	{
		m_EnbaleGravityTransform=true;
	}
	else
	{
		m_EnbaleGravityTransform=false;
	}

	m_NoTrail=((pEmitterInfo->Flags&CD3DParticleEmitterResource::PEF_NO_TRAIL)!=0);
	m_TextureTileRotation=pEmitterInfo->TextureTileRotation;

	m_ParticleCount=0;

	CreateBoundings();

	return true;
}

void CD3DBillBoardParticleEmitter::BuildParticle(CD3DParticleEmitterResource::PARTICLE_EMITTER_INFO * pParticleEmitterInfo,CD3DParticleEmitterResource::PARTICLE_PARAM * pParam)
{
	CD3DVector3 Dir(0.0f,1.0f,0.0f);
	FLOAT RotationX=0;
	FLOAT RotationZ=0;

	

	CD3DVector3 StartPos;

	if(pParticleEmitterInfo->EmitterType==CD3DParticleEmitterResource::EMITTER_PLANE)
	{
		if((pParticleEmitterInfo->Flags&CD3DParticleEmitterResource::PED_DIR_NO_HORIZONTAL_TRANS)==0)
		{
			RotationX=GetRandf(0.0f,pParam->HorizontalRange)-pParam->HorizontalRange/2;
		}

		//if(pParticleEmitterInfo->Flags&PEF_DIR_VERTICAL_TRANS)
		{
			RotationZ=GetRandf(0.0f,pParam->VerticalRange)-pParam->VerticalRange/2;
		}

		if(RotationX!=0.0f||RotationZ!=0.0f)
		{
			Dir=Dir*CD3DMatrix::FromRotationX(RotationX)*CD3DMatrix::FromRotationZ(RotationZ);	
			Dir.Normalize();
		}

		StartPos.x=GetRandf(-pParam->EmissionAreaLength/2,pParam->EmissionAreaLength/2);
		StartPos.y=0;
		StartPos.z=GetRandf(-pParam->EmissionAreaWidth/2,pParam->EmissionAreaWidth/2);
	}
	else
	{
		RotationX=GetRandf(0.0f,pParam->HorizontalRange)-pParam->HorizontalRange/2;
		RotationZ=GetRandf(0.0f,pParam->VerticalRange)-pParam->VerticalRange/2;
		Dir=Dir*CD3DMatrix::FromRotationX(RotationX)*CD3DMatrix::FromRotationZ(RotationZ);	
		Dir.Normalize();

		FLOAT S=GetRandf(0.0f,1.0f);
		FLOAT Radius1=pParam->EmissionAreaWidth*S;
		FLOAT Radius2=pParam->EmissionAreaLength*S;		
		FLOAT Angle=GetRandf(0.0f,D3DX_PI*2);

		StartPos.y=GetRandf(-Radius2,Radius2);
		
		if(Radius2>0)
		{
			Radius1=sqrt((1.0f-(StartPos.y*StartPos.y)/(Radius2*Radius2))*(Radius1*Radius1));
		}		
		StartPos.x=Radius1*cos(Angle);
		StartPos.z=Radius1*sin(Angle);
	}

	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Position=StartPos;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Life=0.0f;

	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Direction=Dir;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.MaxLife=pParam->LifeSpan;

	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Speed=pParam->Speed;	
	if(pParticleEmitterInfo->Flags&CD3DParticleEmitterResource::PEF_HAVE_SPEED_VARIATION)
	{
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SpeedVariation=pParam->SpeedVariation;
	}
	else
	{
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SpeedVariation=0;
	}	
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Gravity=pParam->Gravity;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Decelerate=pParam->Decelerate;

	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Slowdown=pParticleEmitterInfo->Slowdown;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ParticleRotation=
		GetRandf(-pParticleEmitterInfo->Rotation*D3DX_PI,pParticleEmitterInfo->Rotation*D3DX_PI);

	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorLifeMid=pParticleEmitterInfo->ParticleColor.TimeStamps[1];
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.OpacityLifeMid=pParticleEmitterInfo->ParticleOpacity.TimeStamps[1];

//	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeLifeMid=pParticleEmitterInfo->ParticleSizes.TimeStamps[1];

	BuildColor(pParticleEmitterInfo->ParticleColor,pParticleEmitterInfo->ParticleOpacity,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorStart,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorMid,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorEnd,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorLifeMid,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.OpacityLifeMid);
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorStart=
	//	D3DCOLOR_RGBA(
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[0].x,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[0].y,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[0].z,
	//	(DWORD)(pParticleEmitterInfo->ParticleOpacity.Keys[0]*255)
	//	);
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorMid=
	//	D3DCOLOR_RGBA(
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[1].x,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[1].y,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[1].z,
	//	(DWORD)(pParticleEmitterInfo->ParticleOpacity.Keys[1]*255)
	//	);
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.ColorEnd=
	//	D3DCOLOR_RGBA(
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[2].x,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[2].y,
	//	(DWORD)pParticleEmitterInfo->ParticleColor.Keys[2].z,
	//	(DWORD)(pParticleEmitterInfo->ParticleOpacity.Keys[2]*255)
	//	);

	BuildSize(pParticleEmitterInfo->ParticleSizes,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeStart,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeMid,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeEnd,
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeLifeMid);

	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeStart=pParticleEmitterInfo->ParticleSizes.Keys[0];
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeMid=pParticleEmitterInfo->ParticleSizes.Keys[1];
	//m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.SizeEnd=pParticleEmitterInfo->ParticleSizes.Keys[2];

	m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.TexColRow=pParticleEmitterInfo->TextureRows*100.0f+pParticleEmitterInfo->TextureCols;

	if(m_NoTrail)
	{
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Translation=0.0f;
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Rotation.SetIdentity();
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Scaling=1.0f;
	}
	else
	{
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Translation=GetWorldMatrix().GetTranslation();
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Rotation=GetWorldMatrix().GetRotationOnQuaternion();
		m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info.Scaling=GetWorldMatrix().GetScale();
	}


	m_pParticleVertexBuffer[m_ParticleCount].Vertex[1].Info=m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[2].Info=m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info;
	m_pParticleVertexBuffer[m_ParticleCount].Vertex[3].Info=m_pParticleVertexBuffer[m_ParticleCount].Vertex[0].Info;

	m_ParticleCount++;


}

void CD3DBillBoardParticleEmitter::BuildColor(FAKE_ANIMATION_FRAME<CD3DVector3>& ColorAni,FAKE_ANIMATION_FRAME<FLOAT>& OpacityAni,D3DCOLOR& StartColor,D3DCOLOR& MidColor,D3DCOLOR& EndColor,FLOAT& ColorMidTime,FLOAT& OpacityMidTime)
{
	UINT RedStart=0,RedMid=0,RedEnd=0;
	UINT GreenStart=0,GreenMid=0,GreenEnd=0;
	UINT BlueStart=0,BlueMid=0,BlueEnd=0;
	UINT AlphaStart=255,AlphaMid=255,AlphaEnd=255;
	ColorMidTime=0;
	OpacityMidTime=0;
	if(ColorAni.Keys.GetCount()==1)
	{
		RedStart=RedMid=RedEnd=(UINT)ColorAni.Keys[0].x;
		GreenStart=GreenMid=GreenEnd=(UINT)ColorAni.Keys[0].y;
		BlueStart=BlueMid=BlueEnd=(UINT)ColorAni.Keys[0].z;
	}
	else if(ColorAni.Keys.GetCount()==2)
	{
		RedStart=(UINT)ColorAni.Keys[0].x;
		RedEnd=(UINT)ColorAni.Keys[1].x;
		RedMid=(RedStart+RedEnd)/2;

		GreenStart=(UINT)ColorAni.Keys[0].y;
		GreenEnd=(UINT)ColorAni.Keys[1].y;
		GreenMid=(GreenStart+GreenEnd)/2;

		BlueStart=(UINT)ColorAni.Keys[0].z;
		BlueEnd=(UINT)ColorAni.Keys[1].z;
		BlueMid=(BlueStart+BlueEnd)/2;

		ColorMidTime=(ColorAni.TimeStamps[0]+ColorAni.TimeStamps[1])/2;
	}
	else if(ColorAni.Keys.GetCount()>=3)
	{
		RedStart=(UINT)ColorAni.Keys[0].x;
		RedMid=(UINT)ColorAni.Keys[1].x;
		RedEnd=(UINT)ColorAni.Keys[2].x;

		GreenStart=(UINT)ColorAni.Keys[0].y;
		GreenMid=(UINT)ColorAni.Keys[1].y;
		GreenEnd=(UINT)ColorAni.Keys[2].y;

		BlueStart=(UINT)ColorAni.Keys[0].z;
		BlueMid=(UINT)ColorAni.Keys[1].z;
		BlueEnd=(UINT)ColorAni.Keys[2].z;	

		ColorMidTime=ColorAni.TimeStamps[1];
	}

	if(OpacityAni.Keys.GetCount()==1)
	{
		AlphaStart=AlphaMid=AlphaEnd=(UINT)(OpacityAni.Keys[0]*255);
	}
	else if(OpacityAni.Keys.GetCount()==2)
	{
		AlphaStart=(UINT)(OpacityAni.Keys[0]*255);		
		AlphaEnd=(UINT)(OpacityAni.Keys[1]*255);
		AlphaMid=(AlphaStart+AlphaEnd)/2;

		OpacityMidTime=(OpacityAni.TimeStamps[0]+OpacityAni.TimeStamps[1])/2;
	}
	else if(OpacityAni.Keys.GetCount()>=3)
	{
		AlphaStart=(UINT)(OpacityAni.Keys[0]*255);
		AlphaMid=(UINT)(OpacityAni.Keys[1]*255);
		AlphaEnd=(UINT)(OpacityAni.Keys[2]*255);

		OpacityMidTime=OpacityAni.TimeStamps[1];
	}

	StartColor=D3DCOLOR_RGBA(RedStart,GreenStart,BlueStart,AlphaStart);
	MidColor=D3DCOLOR_RGBA(RedMid,GreenMid,BlueMid,AlphaMid);
	EndColor=D3DCOLOR_RGBA(RedEnd,GreenEnd,BlueEnd,AlphaEnd);
}

void CD3DBillBoardParticleEmitter::BuildSize(FAKE_ANIMATION_FRAME<CD3DVector2>& SizeAni,CD3DVector2& StartSize,CD3DVector2& MidSize,CD3DVector2& EndSize,FLOAT& MidTime)
{
	if(SizeAni.Keys.GetCount()==1)
	{
		MidTime=0;
		StartSize=SizeAni.Keys[0];
		MidSize=SizeAni.Keys[0];
		EndSize=SizeAni.Keys[0];
	}
	else if(SizeAni.Keys.GetCount()==2)
	{
		MidTime=(SizeAni.TimeStamps[0]+SizeAni.TimeStamps[1])/2;
		StartSize=SizeAni.Keys[0];		
		EndSize=SizeAni.Keys[1];
		MidSize=(StartSize+EndSize)/2;
	}
	else if(SizeAni.Keys.GetCount()>=3)
	{
		MidTime=SizeAni.TimeStamps[1];
		StartSize=SizeAni.Keys[0];
		MidSize=SizeAni.Keys[1];
		EndSize=SizeAni.Keys[2];
	}
	else
	{
		MidTime=0;
		StartSize=0;
		MidSize=0;
		EndSize=0;
	}
}

void CD3DBillBoardParticleEmitter::CreateBoundings()
{
	CD3DParticleEmitterResource::PARTICLE_EMITTER_INFO * pEmitterInfo=m_pModelResource->GetParticleEmitterInfo();
	FLOAT MaxLife=0;
	FLOAT MaxSpeed=0;
	FLOAT MaxAcc=0;
	FLOAT MaxGravity=0;
	FLOAT MaxSize=0;
	FLOAT MaxArea=0;
	for(UINT i=0;i<pEmitterInfo->LifeSpan.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->LifeSpan.Animations[i].Keys.GetCount();j++)
		{
			if(pEmitterInfo->LifeSpan.Animations[i].Keys[j]>MaxLife)
			{
				MaxLife=pEmitterInfo->LifeSpan.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->Speed.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->Speed.Animations[i].Keys.GetCount();j++)
		{
			if(abs(pEmitterInfo->Speed.Animations[i].Keys[j])>MaxSpeed)
			{
				MaxSpeed=pEmitterInfo->Speed.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->SpeedVariation.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->SpeedVariation.Animations[i].Keys.GetCount();j++)
		{
			if(abs(pEmitterInfo->SpeedVariation.Animations[i].Keys[j])>MaxAcc)
			{
				MaxAcc=pEmitterInfo->SpeedVariation.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->Gravity.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->Gravity.Animations[i].Keys.GetCount();j++)
		{
			if(abs(pEmitterInfo->Gravity.Animations[i].Keys[j])>MaxGravity)
			{
				MaxGravity=pEmitterInfo->Gravity.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->EmissionAreaWidth.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->EmissionAreaWidth.Animations[i].Keys.GetCount();j++)
		{
			if(abs(pEmitterInfo->EmissionAreaWidth.Animations[i].Keys[j])>MaxArea)
			{
				MaxArea=pEmitterInfo->EmissionAreaWidth.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->EmissionAreaLength.Animations.GetCount();i++)
	{
		for(UINT j=0;j<pEmitterInfo->EmissionAreaLength.Animations[i].Keys.GetCount();j++)
		{
			if(abs(pEmitterInfo->EmissionAreaLength.Animations[i].Keys[j])>MaxArea)
			{
				MaxArea=pEmitterInfo->EmissionAreaLength.Animations[i].Keys[j];
			}
		}
	}
	for(UINT i=0;i<pEmitterInfo->ParticleSizes.Keys.GetCount();i++)
	{
		if(MaxSize<pEmitterInfo->ParticleSizes.Keys[i].x)
		{
			MaxSize=pEmitterInfo->ParticleSizes.Keys[i].x;
		}
		if(MaxSize<pEmitterInfo->ParticleSizes.Keys[i].y)
		{
			MaxSize=pEmitterInfo->ParticleSizes.Keys[i].y;
		}
	}
	if(pEmitterInfo->EmitterType==CD3DParticleEmitterResource::EMITTER_PLANE)
	{
		MaxArea=MaxArea/2;
	}
	FLOAT Size=MaxSpeed*MaxLife+0.5f*(MaxAcc+MaxGravity)*MaxLife*MaxLife+MaxSize/2+MaxArea;
	m_BoundingBox.m_Min.SetValue(-Size,-Size,-Size);
	m_BoundingBox.m_Max.SetValue(Size,Size,Size);
}

}



