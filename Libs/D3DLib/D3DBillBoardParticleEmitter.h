/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2BillBoardParticleEmitter.h                       */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DBillBoardParticleEmitter :
	public CD3DBaseDynamicModel
{
public:
	static UINT										m_BillBoardParticleEmitterUpdateCount;
protected:
	enum SST_MEMBER_ID
	{
		SST_D3DBBPE_MODEL_RESOURCE=SST_D3DO_MAX,		
		SST_D3DBBPE_MAX_PARTICLE_COUNT,
		SST_D3DBBPE_MAX=SST_D3DO_MAX+50,
	};
	enum
	{
		MAX_PARTICLE_COUNT=10000,
		MIN_PARTICLE_COUNT=2,
	};
	struct PARTICLE_VERTEX
	{
		struct
		{
			CD3DVector3		Position;
			FLOAT			Life;

			CD3DVector3		Direction;
			FLOAT			MaxLife;

			FLOAT			Speed;
			FLOAT			SpeedVariation;
			FLOAT			Gravity;		
			FLOAT			Decelerate;

			FLOAT			Slowdown;				
			FLOAT			ParticleRotation;
			FLOAT			ColorLifeMid;
			FLOAT			OpacityLifeMid;

			

			D3DCOLOR		ColorStart;

			D3DCOLOR		ColorMid;

			D3DCOLOR		ColorEnd;

			CD3DVector3		Translation;
			FLOAT			TexColRow;

			CD3DQuaternion	Rotation;
			

			CD3DVector3		Scaling;			
			FLOAT			SizeLifeMid;

			CD3DVector2		SizeStart;
			CD3DVector2		SizeMid;

			CD3DVector2		SizeEnd;
		} Info;
		CD3DVector2	Tex;
	};
	struct PARTICLE_RECT
	{
		PARTICLE_VERTEX		Vertex[4];
	};
	
	CD3DParticleEmitterResource *					m_pModelResource;	
	CD3DSubMesh										m_SubMesh;
	UINT											m_MaxParticleCount;	
	bool											m_IsPlaying;
	PARTICLE_RECT *									m_pParticleVertexBuffer;
	WORD *											m_pParticleIndexBuffer;
	UINT											m_ParticleCount;	
	FLOAT											m_StartTime;
	FLOAT											m_RecentUpdateTime;
	FLOAT											m_PlaySpeedRate;
	FLOAT											m_EmitterRate;	
	bool											m_EnbaleGravityTransform;
	bool											m_NoTrail;
	int												m_TextureTileRotation;
	CD3DBoundingBox									m_BoundingBox;

	

	DECLARE_FILE_CHANNEL_MANAGER
	DECLARE_CLASS_INFO(CD3DBillBoardParticleEmitter)

public:
	CD3DBillBoardParticleEmitter(void);
	virtual ~CD3DBillBoardParticleEmitter(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	virtual bool Play(bool IsLoop);	
	virtual bool Stop();
	virtual bool IsPlaying();
	virtual void SetPlaySpeed(FLOAT Rate);

	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);

	virtual CD3DBoundingBox * GetBoundingBox();
	virtual CD3DBoundingSphere * GetBoundingSphere();

	virtual bool CanDoSubMeshViewCull();

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

	virtual void OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);


	virtual void Update(FLOAT Time);
	virtual bool NeedUpdateAni();

	bool LoadFromResource(CD3DParticleEmitterResource * pModelResource,UINT MaxParticleCount);

	CD3DParticleEmitterResource * GetModelResource();
protected:
	void BuildParticle(CD3DParticleEmitterResource::PARTICLE_EMITTER_INFO * pParticleEmitterInfo,CD3DParticleEmitterResource::PARTICLE_PARAM * pParam);
	void BuildColor(FAKE_ANIMATION_FRAME<CD3DVector3>& ColorAni,FAKE_ANIMATION_FRAME<FLOAT>& OpacityAni,D3DCOLOR& StartColor,D3DCOLOR& MidColor,D3DCOLOR& EndColor,FLOAT& ColorMidTime,FLOAT& OpacityMidTime);
	void BuildSize(FAKE_ANIMATION_FRAME<CD3DVector2>& SizeAni,CD3DVector2& StartSize,CD3DVector2& MidSize,CD3DVector2& EndSize,FLOAT& MidTime);
	void CreateBoundings();
};

inline CD3DParticleEmitterResource * CD3DBillBoardParticleEmitter::GetModelResource()
{
	return m_pModelResource;
}

}