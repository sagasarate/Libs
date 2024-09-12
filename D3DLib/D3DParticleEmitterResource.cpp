#include "StdAfx.h"

namespace D3DLib{


IMPLEMENT_CLASS_INFO(CD3DParticleEmitterResource,CD3DObjectResource);

CD3DParticleEmitterResource::CD3DParticleEmitterResource(void)
{
}

CD3DParticleEmitterResource::CD3DParticleEmitterResource(CD3DObjectResourceManager* pManager)
	:CD3DObjectResource(pManager)
{

}

CD3DParticleEmitterResource::~CD3DParticleEmitterResource(void)
{
	Destory();
}

void CD3DParticleEmitterResource::Destory()
{
	SAFE_RELEASE(m_ParticleEmitterInfo.pTexture);
	SAFE_RELEASE(m_ParticleEmitterInfo.pFX);
	m_ParticleEmitterInfo.Clear();
	m_GlobalSequences.Clear();
}

bool CD3DParticleEmitterResource::Reset()
{
	bool Ret=true;
	if(m_ParticleEmitterInfo.pTexture)
	{
		Ret=Ret||m_ParticleEmitterInfo.pTexture->Reset();
	}
	if(m_ParticleEmitterInfo.pFX)
	{
		Ret=Ret||m_ParticleEmitterInfo.pFX->Reset();
	}
	return Ret;
}
bool CD3DParticleEmitterResource::Restore()
{
	bool Ret=true;
	if(m_ParticleEmitterInfo.pTexture)
	{
		Ret=Ret||m_ParticleEmitterInfo.pTexture->Restore();
	}
	if(m_ParticleEmitterInfo.pFX)
	{
		Ret=Ret||m_ParticleEmitterInfo.pFX->Restore();
	}
	return Ret;
}

bool CD3DParticleEmitterResource::LoadM2ParticleEmitter(UINT ID,M2_PARTICLE_EMITTER * pParticleEmitter,BYTE * pModelData,CEasyArray<UINT>& GlobalSequences)
{
	if(m_pManager==NULL||pParticleEmitter==NULL||pModelData==NULL)
		return false;

	BLZ_M2_HEADER * pHeader=(BLZ_M2_HEADER *)pModelData;


	WORD * pTexturesLookup=(WORD *)(pModelData+pHeader->TexLookupOffset);

	M2_MODEL_TEXTURE * pTextures=(M2_MODEL_TEXTURE *)(pModelData+pHeader->TexturesOffset);

	m_GlobalSequences=GlobalSequences;

	//PrintD3DLog("Particle%u,Flag=0x%X",i,pParticleEmitter->Flags);
	m_ParticleEmitterInfo.ID=ID;
	m_ParticleEmitterInfo.Flags=pParticleEmitter->Flags;
	m_ParticleEmitterInfo.BlendingType=pParticleEmitter->BlendingType;
	m_ParticleEmitterInfo.EmitterType=pParticleEmitter->EmitterType;		
	m_ParticleEmitterInfo.GlobalColor=pParticleEmitter->ParticleColorRef;
	m_ParticleEmitterInfo.ParticleType=pParticleEmitter->ParticleType;
	m_ParticleEmitterInfo.HeadOrTail=pParticleEmitter->HeadOrTail;
	m_ParticleEmitterInfo.TextureTileRotation=pParticleEmitter->TextureTileRotation;
	m_ParticleEmitterInfo.TextureRows=pParticleEmitter->TextureRows;
	m_ParticleEmitterInfo.TextureCols=pParticleEmitter->TextureCols;
	m_ParticleEmitterInfo.Scale=pParticleEmitter->Scale;
	m_ParticleEmitterInfo.Slowdown=pParticleEmitter->Slowdown;
	m_ParticleEmitterInfo.Rotation=pParticleEmitter->Rotation;
	m_ParticleEmitterInfo.Trans=BLZTranslationToD3D(pParticleEmitter->Trans);

	if(pTextures[pParticleEmitter->Texture].Type==0)
	{
		CEasyString szTextureName=(char *)pModelData+pTextures[pParticleEmitter->Texture].FileNameOffset;
		m_ParticleEmitterInfo.pTexture=m_pManager->GetDevice()->GetTextureManager()->LoadTexture(szTextureName);
	}		
	m_ParticleEmitterInfo.pFX=BuildParticleFX(m_ParticleEmitterInfo.BlendingType);

	LoadAniBlockFloat(pModelData,pParticleEmitter->EmissionSpeed,m_ParticleEmitterInfo.Speed);
	LoadAniBlockFloat(pModelData,pParticleEmitter->SpeedVariation,m_ParticleEmitterInfo.SpeedVariation);
	LoadAniBlockFloat(pModelData,pParticleEmitter->VerticalRange,m_ParticleEmitterInfo.VerticalRange);
	LoadAniBlockFloat(pModelData,pParticleEmitter->HorizontalRange,m_ParticleEmitterInfo.HorizontalRange);
	LoadAniBlockFloat(pModelData,pParticleEmitter->Gravity,m_ParticleEmitterInfo.Gravity);
	LoadAniBlockFloat(pModelData,pParticleEmitter->LifeSpan,m_ParticleEmitterInfo.LifeSpan);
	LoadAniBlockFloat(pModelData,pParticleEmitter->EmissionRate,m_ParticleEmitterInfo.EmissionRate);
	LoadAniBlockFloat(pModelData,pParticleEmitter->EmissionAreaLength,m_ParticleEmitterInfo.EmissionAreaLength);
	LoadAniBlockFloat(pModelData,pParticleEmitter->EmissionAreaWidth,m_ParticleEmitterInfo.EmissionAreaWidth);
	LoadAniBlockFloat(pModelData,pParticleEmitter->Decelerate,m_ParticleEmitterInfo.Decelerate);
	//LoadAniBlockBool(pModelData,pParticleEmitter->EnabledIn,m_ParticleEmitterInfo.EnabledIn);

	LoadFakeAniBlockColor(pModelData,pParticleEmitter->ParticleColor,m_ParticleEmitterInfo.ParticleColor);
	LoadFakeAniBlockAlpha(pModelData,pParticleEmitter->ParticleOpacity,m_ParticleEmitterInfo.ParticleOpacity);
	LoadFakeAniBlockSize(pModelData,pParticleEmitter->ParticleSizes,m_ParticleEmitterInfo.ParticleSizes);
	LoadFakeAniBlockShort(pModelData,pParticleEmitter->Intensity,m_ParticleEmitterInfo.Intensity);
	//LoadFakeAniBlockShort(pModelData,pParticleEmitter->UnknownReferences1,m_ParticleEmitterInfo.UnknowBlock);

	//m_ParticleEmitterInfo.ParticleColor;		
	//m_ParticleEmitterInfo.ParticleOpacity;	
	//m_ParticleEmitterInfo.ParticleSizes;		
	//m_ParticleEmitterInfo.Intensity;	

	return true;
}

bool CD3DParticleEmitterResource::MakeParticleParam(UINT Time,bool IsLoop,PARTICLE_PARAM& Param)
{
	
	UINT AniLength=0;
	if(m_ParticleEmitterInfo.Speed.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.Speed.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.Speed.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.Speed.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.Speed.Animations[0],
		Param.Speed);

	AniLength=0;
	if(m_ParticleEmitterInfo.SpeedVariation.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.SpeedVariation.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.SpeedVariation.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.SpeedVariation.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.SpeedVariation.Animations[0],
		Param.SpeedVariation);

	AniLength=0;
	if(m_ParticleEmitterInfo.VerticalRange.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.VerticalRange.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.VerticalRange.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.VerticalRange.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.VerticalRange.Animations[0],
		Param.VerticalRange);

	AniLength=0;
	if(m_ParticleEmitterInfo.HorizontalRange.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.HorizontalRange.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.HorizontalRange.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.HorizontalRange.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.HorizontalRange.Animations[0],
		Param.HorizontalRange);

	AniLength=0;
	if(m_ParticleEmitterInfo.Gravity.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.Gravity.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.Gravity.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.Gravity.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.Gravity.Animations[0],
		Param.Gravity);

	AniLength=0;
	if(m_ParticleEmitterInfo.LifeSpan.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.LifeSpan.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.LifeSpan.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.LifeSpan.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.LifeSpan.Animations[0],
		Param.LifeSpan);

	AniLength=0;
	if(m_ParticleEmitterInfo.EmissionRate.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.EmissionRate.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.EmissionRate.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.EmissionRate.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.EmissionRate.Animations[0],
		Param.EmissionRate);

	AniLength=0;
	if(m_ParticleEmitterInfo.EmissionAreaLength.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.EmissionAreaLength.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.EmissionAreaLength.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.EmissionAreaLength.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.EmissionAreaLength.Animations[0],
		Param.EmissionAreaLength);

	AniLength=0;
	if(m_ParticleEmitterInfo.EmissionAreaWidth.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.EmissionAreaWidth.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.EmissionAreaWidth.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.EmissionAreaWidth.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.EmissionAreaWidth.Animations[0],
		Param.EmissionAreaWidth);

	AniLength=0;
	if(m_ParticleEmitterInfo.Decelerate.GlobalSequenceID>=0&&
		m_ParticleEmitterInfo.Decelerate.GlobalSequenceID<(int)m_GlobalSequences.GetCount())
	{
		AniLength=m_GlobalSequences[m_ParticleEmitterInfo.Decelerate.GlobalSequenceID];
	}
	GetInterpolationValue(Time,IsLoop,
		m_ParticleEmitterInfo.Decelerate.InterpolationType,
		AniLength,
		m_ParticleEmitterInfo.Decelerate.Animations[0],
		Param.Decelerate);
	return true;
	
}

void CD3DParticleEmitterResource::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_ParticleEmitterInfo.pTexture)
	{
		pResourceManager->AddResource(m_ParticleEmitterInfo.pTexture);
	}
	if(m_ParticleEmitterInfo.pFX)
	{
		pResourceManager->AddResource(m_ParticleEmitterInfo.pFX);
	}
}

bool CD3DParticleEmitterResource::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CD3DObjectResource::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_ID,m_ParticleEmitterInfo.ID));	
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_FLAGS,m_ParticleEmitterInfo.Flags));	
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_TEXTURE,m_ParticleEmitterInfo.pTexture->GetName()));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_BLENDING_TYPE,m_ParticleEmitterInfo.BlendingType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_EMITTER_TYPE,m_ParticleEmitterInfo.EmitterType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_GLOBAL_COLOR, (UINT)m_ParticleEmitterInfo.GlobalColor));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_PARTICLE_TYPE,m_ParticleEmitterInfo.ParticleType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_HEAD_OR_TAIL,m_ParticleEmitterInfo.HeadOrTail));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_TEXTURE_TILE_ROTATION,m_ParticleEmitterInfo.TextureTileRotation));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_TEXTURE_ROWS,m_ParticleEmitterInfo.TextureRows));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_TEXTURE_COLS,m_ParticleEmitterInfo.TextureCols));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_SCALE,
		(char *)&(m_ParticleEmitterInfo.Scale),sizeof(m_ParticleEmitterInfo.Scale)));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_SLOWDOWN,m_ParticleEmitterInfo.Slowdown));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_ROTATION,m_ParticleEmitterInfo.Rotation));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_TRANS,
		(char *)&(m_ParticleEmitterInfo.Trans),sizeof(m_ParticleEmitterInfo.Trans)));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_FX,m_ParticleEmitterInfo.pFX->GetName()));


	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Speed,Packet,SST_D3DPER_SPEED))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.SpeedVariation,Packet,SST_D3DPER_SPEED_VARIATION))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.VerticalRange,Packet,SST_D3DPER_VERTICAL_RANGE))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.HorizontalRange,Packet,SST_D3DPER_HORIZONTAL_RANGE))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Gravity,Packet,SST_D3DPER_GRAVITY))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.LifeSpan,Packet,SST_D3DPER_LIFE_SPAN))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionRate,Packet,SST_D3DPER_EMISSION_RATE))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionAreaLength,Packet,SST_D3DPER_EMISSION_AREA_LENGTH))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionAreaWidth,Packet,SST_D3DPER_EMISSION_AREA_WIDTH))
		return false;
	if(!PackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Decelerate,Packet,SST_D3DPER_DECELERATE))
		return false;

	if(!PackFakeAnimationBlock<CD3DVector3>(m_ParticleEmitterInfo.ParticleColor,Packet,SST_D3DPER_PARTICLE_COLOR))
		return false;
	if(!PackFakeAnimationBlock<FLOAT>(m_ParticleEmitterInfo.ParticleOpacity,Packet,SST_D3DPER_PARTICLE_OPACITY))
		return false;
	if(!PackFakeAnimationBlock<CD3DVector2>(m_ParticleEmitterInfo.ParticleSizes,Packet,SST_D3DPER_PARTICLE_SIZES))
		return false;
	if(!PackFakeAnimationBlock<short>(m_ParticleEmitterInfo.Intensity,Packet,SST_D3DPER_INTENSITY))
		return false;

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DPER_GLOBAL_SEQUENCE,
		(char *)m_GlobalSequences.GetBuffer(),m_GlobalSequences.GetCount()*sizeof(UINT)));

	return true;
}

bool CD3DParticleEmitterResource::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
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
		case SST_D3DPER_ID:
			m_ParticleEmitterInfo.ID=Value;
			break;
		case SST_D3DPER_FLAGS:
			m_ParticleEmitterInfo.Flags=Value;
			break;
		case SST_D3DPER_TEXTURE:
			{
				LPCTSTR szResourceName=Value;
				m_ParticleEmitterInfo.pTexture=
					(CD3DTexture *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DTexture));
				if(m_ParticleEmitterInfo.pTexture)
				{
					m_ParticleEmitterInfo.pTexture->AddUseRef();
				}
			}
			break;
		case SST_D3DPER_BLENDING_TYPE:
			m_ParticleEmitterInfo.BlendingType=Value;
			break;
		case SST_D3DPER_EMITTER_TYPE:
			m_ParticleEmitterInfo.EmitterType=Value;
			break;
		case SST_D3DPER_GLOBAL_COLOR:
			m_ParticleEmitterInfo.GlobalColor = (UINT)Value;
			break;
		case SST_D3DPER_PARTICLE_TYPE:
			m_ParticleEmitterInfo.ParticleType=Value;
			break;
		case SST_D3DPER_HEAD_OR_TAIL:
			m_ParticleEmitterInfo.HeadOrTail=Value;
			break;
		case SST_D3DPER_TEXTURE_TILE_ROTATION:
			m_ParticleEmitterInfo.TextureTileRotation=Value;
			break;
		case SST_D3DPER_TEXTURE_ROWS:
			m_ParticleEmitterInfo.TextureRows=Value;
			break;
		case SST_D3DPER_TEXTURE_COLS:
			m_ParticleEmitterInfo.TextureCols=Value;
			break;
		case SST_D3DPER_SCALE:
			memcpy(&m_ParticleEmitterInfo.Scale,(LPCSTR)Value,
				sizeof(m_ParticleEmitterInfo.Scale));
			break;
		case SST_D3DPER_SLOWDOWN:
			m_ParticleEmitterInfo.Slowdown=Value;
			break;
		case SST_D3DPER_ROTATION:
			m_ParticleEmitterInfo.Rotation=Value;
			break;
		case SST_D3DPER_TRANS:
			memcpy(&m_ParticleEmitterInfo.Trans,(LPCSTR)Value,
				sizeof(m_ParticleEmitterInfo.Trans));
			break;
		case SST_D3DPER_FX:
			{
				LPCTSTR szResourceName=Value;
				m_ParticleEmitterInfo.pFX=
					(CD3DFX *)pResourceManager->FindResource(szResourceName,GET_CLASS_INFO(CD3DFX));
				if(m_ParticleEmitterInfo.pFX)
				{
					m_ParticleEmitterInfo.pFX->AddUseRef();
				}
			}
			break;
		case SST_D3DPER_SPEED:			
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Speed,Value,0))
				return false;
			break;
		case SST_D3DPER_SPEED_VARIATION:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.SpeedVariation,Value,0))
				return false;
			break;
		case SST_D3DPER_VERTICAL_RANGE:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.VerticalRange,Value,0))
				return false;
			break;
		case SST_D3DPER_HORIZONTAL_RANGE:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.HorizontalRange,Value,0))
				return false;
			break;
		case SST_D3DPER_GRAVITY:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Gravity,Value,0))
				return false;
			break;
		case SST_D3DPER_LIFE_SPAN:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.LifeSpan,Value,0))
				return false;
			break;
		case SST_D3DPER_EMISSION_RATE:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionRate,Value,0))
				return false;
			break;
		case SST_D3DPER_EMISSION_AREA_LENGTH:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionAreaLength,Value,0))
				return false;
			break;
		case SST_D3DPER_EMISSION_AREA_WIDTH:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.EmissionAreaWidth,Value,0))
				return false;
			break;
		case SST_D3DPER_DECELERATE:
			if(!UnpackAnimationBlock<FLOAT>(m_ParticleEmitterInfo.Decelerate,Value,0))
				return false;
			break;
		case SST_D3DPER_PARTICLE_COLOR:
			if(!UnpackFakeAnimationBlock<CD3DVector3>(m_ParticleEmitterInfo.ParticleColor,Value,0))
				return false;
			break;
		case SST_D3DPER_PARTICLE_OPACITY:
			if(!UnpackFakeAnimationBlock<FLOAT>(m_ParticleEmitterInfo.ParticleOpacity,Value,0))
				return false;
			break;
		case SST_D3DPER_PARTICLE_SIZES:
			if(!UnpackFakeAnimationBlock<CD3DVector2>(m_ParticleEmitterInfo.ParticleSizes,Value,0))
				return false;
			break;
		case SST_D3DPER_INTENSITY:
			if(!UnpackFakeAnimationBlock<short>(m_ParticleEmitterInfo.Intensity,Value,0))
				return false;
			break;
		case SST_D3DPER_GLOBAL_SEQUENCE:
			m_GlobalSequences.Resize(Value.GetLength()/sizeof(UINT));
			memcpy(m_GlobalSequences.GetBuffer(),(LPCSTR)Value,
				sizeof(UINT)*m_GlobalSequences.GetCount());
			break;
		}
	}
	return true;
}

UINT CD3DParticleEmitterResource::GetSmartStructSize(UINT Param)
{
	UINT Size=CD3DObjectResource::GetSmartStructSize(Param);

	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.ID));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.Flags));
	Size += CSmartStruct::GetStringMemberSize(m_ParticleEmitterInfo.pTexture->GetNameLength());
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.BlendingType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.EmitterType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.GlobalColor));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.ParticleType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.HeadOrTail));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.TextureTileRotation));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.TextureRows));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.TextureCols));
	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_ParticleEmitterInfo.Scale));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.Slowdown));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_ParticleEmitterInfo.Rotation));
	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_ParticleEmitterInfo.Trans));
	Size += CSmartStruct::GetStringMemberSize(m_ParticleEmitterInfo.pFX->GetNameLength());
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.Speed);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.SpeedVariation);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.VerticalRange);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.HorizontalRange);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.Gravity);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.LifeSpan);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.EmissionRate);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.EmissionAreaLength);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.EmissionAreaWidth);
	Size+=GetAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.Decelerate);

	Size+=GetFakeAnimationBlockSmartStructSize<CD3DVector3>(m_ParticleEmitterInfo.ParticleColor);
	Size+=GetFakeAnimationBlockSmartStructSize<FLOAT>(m_ParticleEmitterInfo.ParticleOpacity);
	Size+=GetFakeAnimationBlockSmartStructSize<CD3DVector2>(m_ParticleEmitterInfo.ParticleSizes);
	Size+=GetFakeAnimationBlockSmartStructSize<short>(m_ParticleEmitterInfo.Intensity);

	Size += CSmartStruct::GetStringMemberSizeA(m_GlobalSequences.GetCount()*sizeof(UINT));

	return Size;
}

CD3DFX * CD3DParticleEmitterResource::BuildParticleFX(UINT BlendingType)
{
	CEasyString  FXName;	
	CEasyStringA EnableZWrite;	
	CEasyStringA EnableAlphaBlend;
	CEasyStringA BlendOp;
	CEasyStringA SrcBlend;
	CEasyStringA DestBlend;
	CEasyStringA EnableAlphaTest;
	CEasyStringA DiffuseFunction;


	FXName.Format(_T("M2Particle\\0x%X"),BlendingType);

	DiffuseFunction="CaculateDiffuse(Pos,float3(0,0,0))";

	switch(BlendingType)
	{
	case EBT_OPACITY:			//glDisable(GL_BLEND); glDisable(GL_ALPHA_TEST); 
		EnableZWrite="True";
		EnableAlphaBlend="False";
		BlendOp="Add";
		SrcBlend="SrcAlpha";
		DestBlend="InvSrcAlpha";
		EnableAlphaTest="False";
		break;
	case EBT_ADD:				//glBlendFunc(GL_SRC_COLOR, GL_ONE);
		EnableZWrite="False";
		EnableAlphaBlend="True";
		BlendOp="Add";
		SrcBlend="SrcColor";
		DestBlend="One";
		EnableAlphaTest="False";
		DiffuseFunction="1.0f";
		break;
	case EBT_ALPHA_BLEND:		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  
		EnableZWrite="False";
		EnableAlphaBlend="True";
		BlendOp="Add";
		SrcBlend="SrcAlpha";
		DestBlend="InvSrcAlpha";
		EnableAlphaTest="False";
		break;
	case EBT_ALPHA_TEST:		//glDisable(GL_BLEND); glEnable(GL_ALPHA_TEST);  
		EnableZWrite="True";
		EnableAlphaBlend="False";
		BlendOp="Add";
		SrcBlend="SrcAlpha";
		DestBlend="InvSrcAlpha";
		EnableAlphaTest="True";
		break;
	case EBT_ALPHA_ADD:			//glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
		EnableZWrite="False";
		EnableAlphaBlend="True";
		BlendOp="Add";
		SrcBlend="SrcAlpha";
		DestBlend="One";
		EnableAlphaTest="False";
		DiffuseFunction="1.0f";
		break;
	case EBT_ALPHA_MUL:			//glBlendFunc(GL_SRC_ALPHA, GL_ONE); 
		EnableZWrite="False";
		EnableAlphaBlend="True";
		BlendOp="Add";
		SrcBlend="Zero";
		DestBlend="SrcColor";
		EnableAlphaTest="False";
		break;
	default:
		PrintD3DLog( _T("CD3DWOWM2ModelResource::BuildParticleFX:未知混合模式%d"), BlendingType);
		EnableZWrite="True";
		EnableAlphaBlend="False";
		BlendOp="Add";
		SrcBlend="SrcAlpha";
		DestBlend="InvSrcAlpha";
		EnableAlphaTest="False";
	}	



	CEasyStringA FxContent=M2_PARTICLE_FX;
	
	FxContent.Replace("<EnableZWrite>",EnableZWrite);
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