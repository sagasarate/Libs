#pragma once

namespace D3DLib{

class CD3DParticleEmitterResource :
	public CD3DObjectResource
{
public:
	enum PARTICLE_EMITTER_TYPE
	{
		EMITTER_NONE,
		EMITTER_PLANE,
		EMITTER_SPHERE,
		EMITTER_SPLINE,
		EMITTER_MAX,
	};

	enum PARTICLE_EMITTER_FLAG
	{
		PEF_GRAVITY_TRANSFORM=(0x10),
		//PEF_DIR_VERTICAL_TRANS=0x20,//0x1,
		PEF_HAVE_SPEED_VARIATION=0x02,
		PED_DIR_NO_HORIZONTAL_TRANS=0x20000,//0x400,
		PEF_NO_TRAIL=(0x10),	
		PEF_ACITVE_ON_TIME=(0x400),
		PEF_NOT_BILLBOARD=0x1000,
		//PEF_NO_SPEED_VARIATION=0x20000,
	};

	struct PARTICLE_EMITTER_INFO
	{
		UINT								ID;
		UINT								Flags;					
		CD3DTexture *						pTexture;				
		BYTE								BlendingType;			
		BYTE								EmitterType;			
		D3DCOLOR							GlobalColor;
		BYTE								ParticleType;			
		BYTE								HeadOrTail;				
		short								TextureTileRotation;	
		WORD								TextureRows;			
		WORD								TextureCols;	
		CD3DVector3							Scale;					
		FLOAT								Slowdown;				
		FLOAT								Rotation;				
		CD3DVector3							Trans;
		CD3DFX *							pFX;
		ANIMATION_BLOCK<FLOAT>				Speed;			
		ANIMATION_BLOCK<FLOAT>				SpeedVariation;			
		ANIMATION_BLOCK<FLOAT>				VerticalRange;			
		ANIMATION_BLOCK<FLOAT>				HorizontalRange;		
		ANIMATION_BLOCK<FLOAT>				Gravity;				
		ANIMATION_BLOCK<FLOAT>				LifeSpan;				
		ANIMATION_BLOCK<FLOAT>				EmissionRate;			
		ANIMATION_BLOCK<FLOAT>				EmissionAreaLength;		
		ANIMATION_BLOCK<FLOAT>				EmissionAreaWidth;		
		ANIMATION_BLOCK<FLOAT>				Decelerate;		
		//ANIMATION_BLOCK<bool>				EnabledIn;
		FAKE_ANIMATION_FRAME<CD3DVector3>	ParticleColor;			
		FAKE_ANIMATION_FRAME<FLOAT>			ParticleOpacity;		
		FAKE_ANIMATION_FRAME<CD3DVector2>	ParticleSizes;			
		FAKE_ANIMATION_FRAME<short>			Intensity;
		//FAKE_ANIMATION_FRAME<short>			UnknowBlock;
		PARTICLE_EMITTER_INFO()
		{
			Clear();
		}
		void Clear()
		{
			ID=0;
			Flags=0;
			pTexture=NULL;
			BlendingType=0;
			EmitterType=0;
			GlobalColor=0;
			ParticleType=0;
			HeadOrTail=0;
			TextureTileRotation=0;
			TextureRows=0;
			TextureCols=0;
			Slowdown=0;
			Rotation=0;
			pFX=NULL;
			Speed.Clear();
			SpeedVariation.Clear();
			VerticalRange.Clear();
			HorizontalRange.Clear();
			Gravity.Clear();
			LifeSpan.Clear();
			EmissionRate.Clear();
			EmissionAreaLength.Clear();
			EmissionAreaWidth.Clear();
			Decelerate.Clear();
			ParticleColor.Clear();
			ParticleOpacity.Clear();
			ParticleSizes.Clear();
			Intensity.Clear();


		}
	};
	struct PARTICLE_PARAM
	{
		FLOAT								Speed;
		FLOAT								SpeedVariation;
		FLOAT								VerticalRange;
		FLOAT								HorizontalRange;
		FLOAT								Gravity;
		FLOAT								LifeSpan;
		FLOAT								EmissionRate;
		FLOAT								EmissionAreaLength;
		FLOAT								EmissionAreaWidth;
		FLOAT								Decelerate;
	};

protected:
	enum SST_MEMBER_ID
	{
		SST_D3DPER_ID=SST_D3DOR_MAX,
		SST_D3DPER_FLAGS,
		SST_D3DPER_TEXTURE,
		SST_D3DPER_BLENDING_TYPE,
		SST_D3DPER_EMITTER_TYPE,
		SST_D3DPER_GLOBAL_COLOR,
		SST_D3DPER_PARTICLE_TYPE,
		SST_D3DPER_HEAD_OR_TAIL,
		SST_D3DPER_TEXTURE_TILE_ROTATION,
		SST_D3DPER_TEXTURE_ROWS,
		SST_D3DPER_TEXTURE_COLS,
		SST_D3DPER_SCALE,
		SST_D3DPER_SLOWDOWN,
		SST_D3DPER_ROTATION,
		SST_D3DPER_TRANS,
		SST_D3DPER_FX,
		SST_D3DPER_SPEED,
		SST_D3DPER_SPEED_VARIATION,
		SST_D3DPER_VERTICAL_RANGE,
		SST_D3DPER_HORIZONTAL_RANGE,
		SST_D3DPER_GRAVITY,
		SST_D3DPER_LIFE_SPAN,
		SST_D3DPER_EMISSION_RATE,
		SST_D3DPER_EMISSION_AREA_LENGTH,
		SST_D3DPER_EMISSION_AREA_WIDTH,
		SST_D3DPER_DECELERATE,
		SST_D3DPER_PARTICLE_COLOR,
		SST_D3DPER_PARTICLE_OPACITY,
		SST_D3DPER_PARTICLE_SIZES,
		SST_D3DPER_INTENSITY,
		SST_D3DPER_GLOBAL_SEQUENCE,
		SST_D3DPER_MAX=SST_D3DOR_MAX+50,
	};

	
	PARTICLE_EMITTER_INFO					m_ParticleEmitterInfo;
	CEasyArray<UINT>						m_GlobalSequences;

	DECLARE_CLASS_INFO(CD3DParticleEmitterResource)
public:
	CD3DParticleEmitterResource(void);
	CD3DParticleEmitterResource(CD3DObjectResourceManager* pManager);
	~CD3DParticleEmitterResource(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadM2ParticleEmitter(UINT ID,M2_PARTICLE_EMITTER * pParticleEmitter,BYTE * pModelData,CEasyArray<UINT>& GlobalSequences);

	bool MakeParticleParam(UINT Time,bool IsLoop,PARTICLE_PARAM& Param);

	PARTICLE_EMITTER_INFO * GetParticleEmitterInfo();

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
protected:
	CD3DFX * BuildParticleFX(UINT BlendingType);
};

inline CD3DParticleEmitterResource::PARTICLE_EMITTER_INFO * CD3DParticleEmitterResource::GetParticleEmitterInfo()
{
	return &m_ParticleEmitterInfo;
}
}