#pragma once

namespace D3DLib{

class CD3DRibbonEmitterResource :
	public CD3DObjectResource
{
public:
	struct RIBBON_EMITTER_INFO
	{			
		UINT							ID;
		CEasyArray<CD3DTexture *>		Textures;	
		CD3DFX *						pFX;		
		bool							IsTransparent;
		FLOAT							Resolution;		
		FLOAT							Length;			
		FLOAT							Emissionangle;	
		ANIMATION_BLOCK<CD3DVector3>	Color;			
		ANIMATION_BLOCK<FLOAT>			Opacity;		
		ANIMATION_BLOCK<FLOAT>			Above;			
		ANIMATION_BLOCK<FLOAT>			Below;
		void Clear()
		{

		}
	};

	struct RIBBON_PARAM
	{
		D3DCOLOR	Color;	
		FLOAT		Above;	
		FLOAT		Below;			
	};

protected:

	enum SST_MEMBER_ID
	{			
		SST_D3DRER_ID=SST_D3DOR_MAX,		
		SST_D3DRER_TEXTURE,
		SST_D3DRER_FX,
		SST_D3DRER_IS_TRANSPARENT,
		SST_D3DRER_RESOLUTION,
		SST_D3DRER_LENGTH,
		SST_D3DRER_EMISSION_ANGLE,
		SST_D3DRER_COLOR,
		SST_D3DRER_OPACITY,
		SST_D3DRER_ABOVE,
		SST_D3DRER_BELOW,
		SST_D3DRER_MAX=SST_D3DOR_MAX+50,
	};

	RIBBON_EMITTER_INFO					m_RibbonEmitterInfo;
	CEasyArray<UINT>					m_GlobalSequences;

	DECLARE_CLASS_INFO(CD3DRibbonEmitterResource)
public:
	CD3DRibbonEmitterResource(void);
	CD3DRibbonEmitterResource(CD3DObjectResourceManager* pManager);
	~CD3DRibbonEmitterResource(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadM2RibbonEmitter(UINT ID,M2_RIBBON_EMITTER * pRibbonEmitter,BYTE * pModelData,CEasyArray<UINT>& GlobalSequences);

	bool MakeRibbonParam(UINT Time,bool IsLoop,RIBBON_PARAM& Param);

	RIBBON_EMITTER_INFO * GetRibbonEmitterInfo();

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
protected:
	CD3DFX * BuildRibbonFX(UINT RenderFlag,UINT BlendingMode);
};

inline CD3DRibbonEmitterResource::RIBBON_EMITTER_INFO * CD3DRibbonEmitterResource::GetRibbonEmitterInfo()
{
	return &m_RibbonEmitterInfo;
}

}