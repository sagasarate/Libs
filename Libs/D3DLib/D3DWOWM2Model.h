/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2Model.h                                          */
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

#pragma comment(linker,"/include:?m_CD3DWOWM2ModelClassInfoRegister@CD3DWOWM2Model@D3DLib@@1VCClassInfoRegister@@A")

class CD3DWOWM2Model :
	public CD3DBaseDynamicModel
{
protected:	

	enum SST_MEMBER_ID
	{
		SST_D3DWMM_MODEL_RESOURCE=SST_D3DO_MAX,		
		SST_D3DWMM_CUR_ANI_ID,
		SST_D3DWMM_CUR_SUB_ANI_ID,
		SST_D3DWMM_ATCIVE_ATTACHMENT,
		SST_D3DWMM_MAX=SST_D3DO_MAX+50,
	};
	enum SST_ACTIVE_ATTACHMENT_INFO
	{
		SST_AAI_ATTACHMENT_ID=1,
		SST_AAI_DUMMY_STORAGE_ID,
	};
	
	enum INTERIM_STATUS
	{
		IS_NONE,
		IS_INIT,
		IS_DOING,
	};

	struct ADDITIONAL_BONE_MATRIX
	{
		UINT		BoneID;
		CD3DMatrix	Matrix;
	};
	

	CD3DWOWM2ModelResource *							m_pModelResource;
	CEasyArray<CD3DWOWM2ModelResource::BONE_MATRIX>		m_BoneMatrices;
	CEasyArray<CD3DMatrix>								m_BoneMatList;
	//CEasyArray<CD3DMatrix>								m_BoneMatListR;
	bool												m_UseSoftSkinMesh;
	CEasyArray<CD3DSubMesh *>							m_SubMeshListForSoftSkinMesh;
	CEasyArray<CD3DWOWM2ModelResource::MODEL_VERTEXT>	m_Vertices;
	CEasyArray<CD3DWOWM2ModelResource::MODEL_VERTEXT>	m_VerticesR;

	CEasyArray<CD3DMatrix>								m_InterimBoneMatStart;
	CEasyArray<CD3DMatrix>								m_InterimBoneMatEnd;
	int													m_InterimStatus;
	FLOAT												m_InterimTime;

	CEasyArray<ADDITIONAL_BONE_MATRIX>					m_AdditionalBoneMatrix;

	int													m_CurAnimationID;
	int													m_CurSubAnimationID;
	int													m_CurAnimationIndex;
	FLOAT												m_CurAnimationLength;

	bool												m_IsPlaying;
	bool												m_IsLoop;
	FLOAT												m_PlayStartTime;
	FLOAT												m_CurPlayTime;
	FLOAT												m_PlaySpeedRate;
	

	CD3DBoundingBox 									m_CurBoundingBox;
	


	DECLARE_FILE_CHANNEL_MANAGER
	DECLARE_CLASS_INFO(CD3DWOWM2Model)
public:
	CD3DWOWM2Model(void);
	~CD3DWOWM2Model(void);

	virtual void Destory();
	virtual void DestoryModel();

	virtual bool Reset();
	virtual bool Restore();

	bool Play(int AnimationID,int SubAnimationID,FLOAT InterimTime,bool IsLoop);
	virtual bool Play(bool IsLoop);	
	virtual bool Stop();
	virtual bool IsPlaying();
	virtual void SetPlaySpeed(FLOAT Rate);

	bool AddBoneTrans(int KeyBone,CD3DMatrix& Matrix);
	void ClearAdditionalBoneTrans();
	

	bool LoadFromFile(LPCTSTR szModelFileName);
	bool LoadFromResource(CD3DWOWM2ModelResource * pModelResource);
	bool LoadFromXFile(LPCTSTR szModelFileName);

	bool ChangeSkin(UINT Index);

	
	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	CD3DWOWM2ModelResource * GetModelResource();

	int GetCurAnimationID();
	int GetCurSubAnimationID();

	CD3DDummy * EnableAttachment(UINT ID);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

	virtual void OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);
	virtual CD3DSubMeshMaterial  *GetSubMeshMaterial(UINT index);
	virtual CD3DSubMesh * GetOriginSubMesh(UINT index);

	virtual CD3DBoundingBox * GetBoundingBox();
	virtual CD3DBoundingSphere * GetBoundingSphere();

	virtual bool CanDoSubMeshViewCull();

	

	virtual void Update(FLOAT Time);
	virtual bool NeedUpdateAni();
protected:
	void PrepareSoftSkinMesh();
	void CaculateSoftSkinMesh();

	virtual void FetchAnimationFrames(UINT Time);
	virtual void OnSubMeshChanged();

};



inline int CD3DWOWM2Model::GetCurAnimationID()
{
	return m_CurAnimationID;
}
inline int CD3DWOWM2Model::GetCurSubAnimationID()
{
	return m_CurSubAnimationID;
}

}