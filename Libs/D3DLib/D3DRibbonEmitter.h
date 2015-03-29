/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2RibbonEmitter.h                                  */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DRibbonEmitter :
	public CD3DBaseDynamicModel
{
protected:
	enum SST_MEMBER_ID
	{
		SST_D3DRE_MODEL_RESOURCE=SST_D3DO_MAX,		
		SST_D3DRE_MAX=SST_D3DO_MAX+50,
	};
	enum
	{
		MAX_RIBBON_COUNT=50000,
	};
	struct RIBBON_VERTEX
	{
		CD3DVector3	Pos;
		FLOAT		CreateTime;
		D3DCOLOR	Color;
		CD3DVector2	Tex;
	};
	struct RIBBON_PAIR
	{
		RIBBON_VERTEX	Vertex[2];
	};
	CD3DRibbonEmitterResource *	m_pModelResource;
	CD3DSubMesh					m_SubMesh;	
	UINT						m_MaxRibbonCount;	
	RIBBON_PAIR *				m_pRibbonVertexBuffer;
	UINT						m_RibbonCount;
	bool						m_IsPlaying;
	FLOAT						m_StartTime;	
	FLOAT						m_ClipLength;
	FLOAT						m_RecentCreateTime;
	FLOAT						m_PlaySpeedRate;
	CD3DBoundingBox				m_BoundingBox;

	DECLARE_FILE_CHANNEL_MANAGER
	DECLARE_CLASS_INFO(CD3DRibbonEmitter)
public:
	CD3DRibbonEmitter(void);
	~CD3DRibbonEmitter(void);

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

	bool LoadFromResource(CD3DRibbonEmitterResource * pModelResource);

	CD3DRibbonEmitterResource * GetModelResource();
protected:
	void BuildRibbon(CD3DRibbonEmitterResource::RIBBON_EMITTER_INFO * pRibbonEmitterInfo,CD3DRibbonEmitterResource::RIBBON_PARAM * pParam,FLOAT Time);
	UINT DelRibbon(FLOAT Time,FLOAT Length);
};
inline CD3DRibbonEmitterResource * CD3DRibbonEmitter::GetModelResource()
{
	return m_pModelResource;
}
}