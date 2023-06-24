/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWADTModel.h                                         */
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

#pragma comment(linker,"/include:?m_CD3DWOWADTModelClassInfoRegister@CD3DWOWADTModel@D3DLib@@1VCClassInfoRegister@@A")

class CD3DWOWADTModel :
	public CD3DBaseStaticModel
{
protected:
	enum SST_MEMBER_ID
	{
		SST_D3DWAM_MODEL_RESOURCE=SST_D3DO_MAX,				
		SST_D3DWAM_MAX=SST_D3DO_MAX+50,
	};

	CD3DWOWADTModelResource *							m_pModelResource;
	CEasyArray<CD3DWOWM2Model *>						m_M2ObjectList;
	CEasyArray<CD3DWOWWMOModel *>						m_WMOObjectList;
	CD3DBoundingBox										m_BoundingBox;
	D3DCOLOR											m_WaterColorLight;
	D3DCOLOR											m_WaterColorDark;
	CEasyArray<CD3DObject *>							m_NeedUpdateChilds;

	DECLARE_FILE_CHANNEL_MANAGER
	DECLARE_CLASS_INFO(CD3DWOWADTModel)
public:
	CD3DWOWADTModel(void);
	~CD3DWOWADTModel(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadFromFile(LPCTSTR ModelFileName,bool IsBigAlphaMask,bool BeLoadObject=true);
	bool LoadFromResource(CD3DWOWADTModelResource * pModelResource,bool BeLoadObject=true);

	void SetWaterColor(D3DCOLOR ColorLight,D3DCOLOR ColorDark);

	CD3DWOWADTModelResource * GetModelResource();
public:
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
public:
	virtual void OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera);
	virtual void OnPrepareRenderData();
	virtual void Update(FLOAT Time);
	virtual int GetSubMeshCount();
	virtual CD3DSubMesh * GetSubMesh(UINT index);
	virtual CD3DSubMeshMaterial * GetSubMeshMaterial(UINT index);

	virtual CD3DBoundingBox * GetBoundingBox();
	virtual CD3DBoundingSphere * GetBoundingSphere();

	bool GetHeightByXZ(const CD3DVector3& Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight,bool IncludeChild=true);

	void BuildBoundingBox();

	UINT GetM2ObjectCount();
	UINT GetWMOObjectCount();

	CD3DWOWDoodadModel * LoadM2Object(UINT Index);
	CD3DWOWWMOModel * LoadWMOObject(UINT Index);

	bool LoadObjects();
	void CheckNeedUpdateObjects();
};

inline CD3DWOWADTModelResource * CD3DWOWADTModel::GetModelResource()
{
	return m_pModelResource;
}

inline void CD3DWOWADTModel::SetWaterColor(D3DCOLOR ColorLight,D3DCOLOR ColorDark)
{
	m_WaterColorLight=ColorLight;
	m_WaterColorDark=ColorDark;
}

inline UINT CD3DWOWADTModel::GetM2ObjectCount()
{
	return m_pModelResource->GetM2ObjectCount();
}
inline UINT CD3DWOWADTModel::GetWMOObjectCount()
{
	return m_pModelResource->GetWMOObjectCount();
}

}