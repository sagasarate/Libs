#include "StdAfx.h"

namespace D3DLib{



IMPLEMENT_CLASS_INFO(CD3DRibbonEmitterResource,CD3DObjectResource);

CD3DRibbonEmitterResource::CD3DRibbonEmitterResource(void)
{
}

CD3DRibbonEmitterResource::CD3DRibbonEmitterResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{

}

CD3DRibbonEmitterResource::~CD3DRibbonEmitterResource(void)
{
	Destory();
}

void CD3DRibbonEmitterResource::Destory()
{
	for(UINT i=0;i<m_RibbonEmitterInfo.Textures.GetCount();i++)
	{
		SAFE_RELEASE(m_RibbonEmitterInfo.Textures[i]);
	}	
	SAFE_RELEASE(m_RibbonEmitterInfo.pFX);
	m_RibbonEmitterInfo.Clear();
	m_GlobalSequences.Clear();
}

bool CD3DRibbonEmitterResource::Reset()
{
	bool Ret=true;
	for(UINT i=0;i<m_RibbonEmitterInfo.Textures.GetCount();i++)
	{
		Ret=Ret||m_RibbonEmitterInfo.Textures[i]->Reset();
	}	
	if(m_RibbonEmitterInfo.pFX)
	{
		Ret=Ret||m_RibbonEmitterInfo.pFX->Reset();
	}
	return Ret;
}
bool CD3DRibbonEmitterResource::Restore()
{
	bool Ret=true;
	for(UINT i=0;i<m_RibbonEmitterInfo.Textures.GetCount();i++)
	{
		Ret=Ret||m_RibbonEmitterInfo.Textures[i]->Restore();
	}	
	if(m_RibbonEmitterInfo.pFX)
	{
		Ret=Ret||m_RibbonEmitterInfo.pFX->Restore();
	}
	return Ret;
}

bool CD3DRibbonEmitterResource::LoadM2RibbonEmitter(UINT ID,M2_RIBBON_EMITTER * pRibbonEmitter,BYTE * pModelData,CEasyArray<UINT>& GlobalSequences)
{
	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;
	WORD * pTexturesLookup=(WORD *)(pModelData+pHeader->TexLookupOffset);
	M2_MODEL_TEXTURE * pTextures=(M2_MODEL_TEXTURE *)(pModelData+pHeader->TexturesOffset);
	M2_MODEL_RENDER_FLAG * pRenderFlags=(M2_MODEL_RENDER_FLAG *)(pModelData+pHeader->RenderFlagsOffset);

	m_RibbonEmitterInfo.ID=ID;
	m_RibbonEmitterInfo.Resolution=pRibbonEmitter->Resolution;
	m_RibbonEmitterInfo.Length=pRibbonEmitter->Length;
	m_RibbonEmitterInfo.Emissionangle=pRibbonEmitter->Emissionangle;

	UINT * pRibbonTextures=(UINT *)(pModelData+pRibbonEmitter->TextureOffset);
	m_RibbonEmitterInfo.Textures.Resize(pRibbonEmitter->TextureCount);
	for(UINT j=0;j<pRibbonEmitter->TextureCount;j++)
	{
		CEasyString szTextureName=(char *)pModelData+pTextures[pRibbonTextures[j]].FileNameOffset;
		CD3DTexture * pTexture=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(szTextureName);
		m_RibbonEmitterInfo.Textures[j]=pTexture;
	}
	UINT * pRibbonBlends=(UINT *)(pModelData+pRibbonEmitter->BlendRefOffset);
	m_RibbonEmitterInfo.pFX=BuildRibbonFX(pRenderFlags[pRibbonBlends[0]].Flags,
		pRenderFlags[pRibbonBlends[0]].BlendingMode);
	m_RibbonEmitterInfo.IsTransparent=(pRenderFlags[pRibbonBlends[0]].BlendingMode!=BLZ_M2_BLENDING_MODE_OPAQUE&&
		pRenderFlags[pRibbonBlends[0]].BlendingMode!=BLZ_M2_BLENDING_MODE_ALPHA_TEST);		

	LoadAniBlockVector3(pModelData,pRibbonEmitter->Color,m_RibbonEmitterInfo.Color);
	LoadAniBlockAlpha(pModelData,pRibbonEmitter->Opacity,m_RibbonEmitterInfo.Opacity);
	LoadAniBlockFloat(pModelData,pRibbonEmitter->Above,m_RibbonEmitterInfo.Above);
	LoadAniBlockFloat(pModelData,pRibbonEmitter->Below,m_RibbonEmitterInfo.Below);

	return true;
}

bool CD3DRibbonEmitterResource::MakeRibbonParam(UINT Time,bool IsLoop,RIBBON_PARAM& Param)
{
	
	UINT AniLength=0;
	if(m_RibbonEmitterInfo.Color.GlobalSequenceID>=0&&
		m_RibbonEmitterInfo.Color.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_RibbonEmitterInfo.Color.GlobalSequenceID];
	}
	CD3DVector3 Color=1.0f;
	GetInterpolationValue(Time,IsLoop,
		m_RibbonEmitterInfo.Color.InterpolationType,
		AniLength,
		m_RibbonEmitterInfo.Color.Animations[0],
		Color);

	AniLength=0;
	if(m_RibbonEmitterInfo.Opacity.GlobalSequenceID>=0&&
		m_RibbonEmitterInfo.Opacity.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_RibbonEmitterInfo.Opacity.GlobalSequenceID];
	}
	FLOAT Alpha=1.0f;
	GetInterpolationValue(Time,IsLoop,
		m_RibbonEmitterInfo.Opacity.InterpolationType,
		AniLength,
		m_RibbonEmitterInfo.Opacity.Animations[0],
		Alpha);
	Param.Color=D3DCOLOR_COLORVALUE(Color.x,Color.y,Color.z,Alpha);

	AniLength=0;
	if(m_RibbonEmitterInfo.Above.GlobalSequenceID>=0&&
		m_RibbonEmitterInfo.Above.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_RibbonEmitterInfo.Above.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_RibbonEmitterInfo.Above.InterpolationType,
		AniLength,
		m_RibbonEmitterInfo.Above.Animations[0],
		Param.Above);

	AniLength=0;
	if(m_RibbonEmitterInfo.Below.GlobalSequenceID>=0&&
		m_RibbonEmitterInfo.Below.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_RibbonEmitterInfo.Below.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_RibbonEmitterInfo.Below.InterpolationType,
		AniLength,
		m_RibbonEmitterInfo.Below.Animations[0],
		Param.Below);
	return true;
	
}

void CD3DRibbonEmitterResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	for(UINT i=0;i<m_RibbonEmitterInfo.Textures.GetCount();i++)
	{
		if(m_RibbonEmitterInfo.Textures[i])
		{
			pResourceManager->AddResource(m_RibbonEmitterInfo.Textures[i]);
		}
	}	
	if(m_RibbonEmitterInfo.pFX)
	{
		pResourceManager->AddResource(m_RibbonEmitterInfo.pFX);
	}
}

bool CD3DRibbonEmitterResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_ID,m_RibbonEmitterInfo.ID));
	

	for(UINT j=0;j<m_RibbonEmitterInfo.Textures.GetCount();j++)
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_TEXTURE,m_RibbonEmitterInfo.Textures[j]->GetName()));
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_FX,m_RibbonEmitterInfo.pFX->GetName()));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_IS_TRANSPARENT,(BYTE)m_RibbonEmitterInfo.IsTransparent));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_RESOLUTION,m_RibbonEmitterInfo.Resolution));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_LENGTH,m_RibbonEmitterInfo.Length));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DRER_EMISSION_ANGLE,m_RibbonEmitterInfo.Emissionangle));
	if(!PackAnimationBlock<CD3DVector3>(m_RibbonEmitterInfo.Color,Packet,SST_D3DRER_COLOR))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Opacity,Packet,SST_D3DRER_OPACITY))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Above,Packet,SST_D3DRER_ABOVE))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Below,Packet,SST_D3DRER_BELOW))
		return false;

	return true;
}

bool CD3DRibbonEmitterResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::FromSmartStruct(Packet,pResourceManager,Param))
		return false;
	
	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_D3DRER_ID:
			m_RibbonEmitterInfo.ID=Value;
			break;		
		case SST_D3DRER_TEXTURE:
			{
				LPCTSTR szResourceName=Value;
				CD3DTexture * pTexture=
					(CD3DTexture *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DTexture));
				if(pTexture)
				{
					pTexture->AddUseRef();
					m_RibbonEmitterInfo.Textures.Add(pTexture);
				}
			}
			break;
		case SST_D3DRER_FX:
			{
				LPCTSTR szResourceName=Value;
				m_RibbonEmitterInfo.pFX=
					(CD3DFX *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DFX));
				if(m_RibbonEmitterInfo.pFX)
				{
					m_RibbonEmitterInfo.pFX->AddUseRef();
				}
			}
			break;
		case SST_D3DRER_IS_TRANSPARENT:
			m_RibbonEmitterInfo.IsTransparent=(BYTE)Value!=0;
			break;
		case SST_D3DRER_RESOLUTION:
			m_RibbonEmitterInfo.Resolution=Value;
			break;
		case SST_D3DRER_LENGTH:
			m_RibbonEmitterInfo.Length=Value;
			break;
		case SST_D3DRER_EMISSION_ANGLE:
			m_RibbonEmitterInfo.Emissionangle=Value;
			break;
		case SST_D3DRER_COLOR:
			if(!UnpackAnimationBlock<CD3DVector3>(m_RibbonEmitterInfo.Color,Value,0))
				return false;
			break;
		case SST_D3DRER_OPACITY:
			if(!UnpackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Opacity,Value,0))
				return false;
			break;
		case SST_D3DRER_ABOVE:
			if(!UnpackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Above,Value,0))
				return false;
			break;
		case SST_D3DRER_BELOW:
			if(!UnpackAnimationBlock<FLOAT>(m_RibbonEmitterInfo.Below,Value,0))
				return false;
			break;
		}
	}
	
	return true;
}

UINT CD3DRibbonEmitterResource::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DObjectResource::GetSmartStructSize(Param);

	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_RibbonEmitterInfo.ID));
	for(UINT j=0;j<m_RibbonEmitterInfo.Textures.GetCount();j++)
	{
		Size+=SMART_STRUCT_STRING_MEMBER_SIZE(m_RibbonEmitterInfo.Textures[j]->GetNameLength());
	}
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(m_RibbonEmitterInfo.pFX->GetNameLength());
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_RibbonEmitterInfo.IsTransparent));
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_RibbonEmitterInfo.Resolution));
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_RibbonEmitterInfo.Length));
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_RibbonEmitterInfo.Emissionangle));

	Size+=GetAnimationBlockSmartStructSize<CD3DVector3>(m_RibbonEmitterInfo.Color);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_RibbonEmitterInfo.Opacity);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_RibbonEmitterInfo.Above);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_RibbonEmitterInfo.Below);
	Size+=SMART_STRUCT_STRUCT_MEMBER_SIZE(0);

	return Size;
}

CD3DFX * CD3DRibbonEmitterResource::BuildRibbonFX(UINT RenderFlag,UINT BlendingMode)
{
	CEasyString  FXName;	
	CEasyStringA EnableZWrite;
	CEasyStringA EnableFog;
	CEasyStringA CullMode;
	CEasyStringA EnableAlphaBlend;
	CEasyStringA BlendOp;
	CEasyStringA SrcBlend;
	CEasyStringA DestBlend;
	CEasyStringA EnableAlphaTest;	
	CEasyStringA DiffuseFunction;

	FXName.Format(_T("M2Ribbon\\0x%X_0x%X"),
		RenderFlag,BlendingMode);


	if(RenderFlag&BLZ_M2_RENDER_FLAG_NO_CULL)
	{
		CullMode="None";
	}
	else
	{
		CullMode="CCW";
	}

	if(RenderFlag&BLZ_M2_RENDER_FLAG_DISABLE_ZBUFFER)
	{		
		EnableZWrite="False";
	}
	else
	{		
		EnableZWrite="True";
	}

	EnableAlphaBlend="False";	
	EnableAlphaTest="False";
	BlendOp="Add";
	SrcBlend="SrcAlpha";
	DestBlend="InvSrcAlpha";
	DiffuseFunction="CaculateDiffuse(Pos,float3(0,0,0))";

	switch(BlendingMode)
	{	
	case BLZ_M2_BLENDING_MODE_ALPHA_TEST:
		EnableAlphaTest="True";
		break;
	case BLZ_M2_BLENDING_MODE_ALPHA_BLENDING:
		EnableAlphaBlend="True";
		break;
	case BLZ_M2_BLENDING_MODE_ADDITIVE:
		EnableAlphaBlend="True";
		SrcBlend="One";
		DestBlend="One";
		DiffuseFunction="1.0f";
		break;
	case BLZ_M2_BLENDING_MODE_ADDITIVE_ALPHA:
		EnableAlphaBlend="True";
		SrcBlend="DestColor";
		DestBlend="One";
		DiffuseFunction="1.0f";
		break;
	case BLZ_M2_BLENDING_MODE_MODULATE:
		break;
	case BLZ_M2_BLENDING_MODE_MUL:
		break;
	}		

	CEasyStringA FxContent=M2_RIBBON_FX;

	FxContent.Replace("<EnableZWrite>",EnableZWrite);
	FxContent.Replace("<EnableFog>",EnableFog);
	FxContent.Replace("<CullMode>",CullMode);
	FxContent.Replace("<EnableAlphaBlend>",EnableAlphaBlend);
	FxContent.Replace("<BlendOp>",BlendOp);
	FxContent.Replace("<SrcBlend>",SrcBlend);
	FxContent.Replace("<DestBlend>",DestBlend);
	FxContent.Replace("<EnableAlphaTest>",EnableAlphaTest);
	FxContent.Replace("<DiffuseFunction>",DiffuseFunction);

	CD3DFX * pFX=m_pManager->GetDevice()->GetFXManager()->LoadFXFromMemory(FXName,FxContent.GetBuffer(),FxContent.GetLength());	
	return pFX;
}

}