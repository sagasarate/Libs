/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWADTModelResource.h                                 */
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

class CD3DWOWADTModelResource :
	public CD3DObjectResource
{

protected:
	struct MODEL_VERTEXT
	{
		CD3DVector3			Pos;
		CD3DVector3			Normal;
		D3DCOLOR			Color;
		CD3DVector2			Tex;		
	};

	struct MODEL_LIQUID_VERTEXT
	{
		CD3DVector3			Pos;
		D3DCOLOR			Color;
		CD3DVector2			Tex;		
	};

	struct MODEL_NORMAL_VERTEXT
	{
		CD3DVector3			Pos;
		D3DCOLOR			Color;
	};

	struct TEXTURE_INFO
	{
		BYTE						TextureIndex;
		UINT						Flag;
		CSmartPtr<CD3DTexture>		pAlphaMap;
	};

	enum ALPHA_DATA_TYPE
	{
		ADT_UNCOMPRESS_2048,
		ADT_UNCOMPRESS_4096,
		ADT_COMPRESS,
	};

	enum
	{		
		TERRAIN_BLOCK_HEIGHT_COUNT=145,
		TERRAIN_BLOCK_WATER_HEIGHT_COUNT=81,
	};

	struct TEXTURE_LAYER_INFO
	{
		BYTE						LayerCount;
		TEXTURE_INFO				Layers[BLZ_ADT_TEXTURE_LAYER_COUNT];
		CSmartPtr<CD3DTexture>		pShadowMap;
		TEXTURE_LAYER_INFO()
		{
			LayerCount=0;
			pShadowMap=NULL;
		}
	};

	struct TERRAIN_BLOCK_HEIGHT_INFO
	{
		bool	HaveWater;
		FLOAT	TerrainHeight[TERRAIN_BLOCK_HEIGHT_COUNT];
		FLOAT	WaterHeight[TERRAIN_BLOCK_WATER_HEIGHT_COUNT];
	};
public:
	enum SUB_MESH_PROPERTY:UINT64
	{
		SMP_IS_WATER=(1<<4),
		SMP_RENDER_TO_DEPTH=(1<<5),
	};
	enum TEXTURE_PROPERTY:UINT64
	{
		TP_ALPHA_MAP=1,
		TP_SHADOW_MAP=(1<<1),
	};
	enum MODEL_FX_TYPE
	{
		MODEL_FX_TYPE_NORMAL,
		MODEL_FX_TYPE_LIQUID,
	};
	enum DOODAD_FLAG:UINT64
	{
		DOODAD_FLAG_SHIFT=8,
		DOODAD_FLAG_MASK=(((UINT64)0xFFFF)<<DOODAD_FLAG_SHIFT),
	};
	struct M2_OBJECT_INFO
	{
		UINT						ID;
		CEasyString					ModelFilePath;
		CD3DWOWM2ModelResource *	pModelResource;
		CD3DVector3					Position;
		CD3DQuaternion				Orientation;
		FLOAT						Scale;
		WORD						Flag;
	};
	struct WMO_OBJECT_INFO
	{
		UINT						ID;
		CEasyString					ModelFilePath;
		CD3DWOWWMOModelResource *	pModelResource;
		CD3DVector3					Position;
		CD3DQuaternion				Orientation;
		UINT						DoodadSet;
		WORD						Flag;
	};

	

protected:

	enum SST_MEMBER_ID
	{
		SST_D3DWAMR_M2_OBJECT_LIST=SST_D3DOR_MAX,
		SST_D3DWAMR_WMO_OBJECT_LIST,
		SST_D3DWAMR_HEIGHT_INFO_LIST,
		SST_D3DWAMR_POSITION,
		SST_D3DWAMR_MAX=SST_D3DOR_MAX+50,
	};

	enum SST_M2_OBJECT_LIST
	{
		SST_MOL_M2_OBJECT_INFO=1,
	};

	enum SST_M2_OBJECT_INFO
	{
		SST_MOI_ID=1,
		SST_MOI_MODEL_RESOURCE,
		SST_MOI_POSITION,
		SST_MOI_ORIENTATION,
		SST_MOI_SCALE,
	};

	enum SST_WMO_OBJECT_LIST
	{
		SST_WOL_WMO_OBJECT_INFO=1,
	};

	enum SST_WMO_OBJECT_INFO
	{
		SST_WOI_ID=1,
		SST_WOI_MODEL_RESOURCE,
		SST_WOI_POSITION,
		SST_WOI_ORIENTATION,
		SST_WOI_DOODAD_SET,
	};

	enum SST_HEIGHT_INFO_LIST
	{
		SST_HIL_HEIGHT_INFO=1,
	};

	enum SST_HEIGHT_INFO
	{
		SST_HI_HAVE_WATER=1,
		SST_HI_TERRAIN_HEIGHT,
		SST_HI_WATER_HEIGHT,
	};

	

	TERRAIN_BLOCK_HEIGHT_INFO		m_TerrainHeightInfo[BLZ_ADT_MAP_TILE_COUNT];
	UINT							m_TerrainHoleInfo[BLZ_ADT_MAP_TILE_COUNT];
	CD3DVector3						m_Position;
	CEasyArray<M2_OBJECT_INFO>		m_M2ObjectList;
	CEasyArray<WMO_OBJECT_INFO>		m_WMOObjectList;
	
	DECLARE_CLASS_INFO(CD3DWOWADTModelResource)
public:
	CD3DWOWADTModelResource(void);
	CD3DWOWADTModelResource(CD3DObjectResourceManager* pManager);
	~CD3DWOWADTModelResource(void);


	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadFromFile(LPCTSTR ModelFileName,bool IsBigAlphaMask,bool BeLoadObject);

	UINT GetM2ObjectCount();
	M2_OBJECT_INFO * GetM2ObjectInfo(UINT Index);

	UINT GetWMOObjectCount();
	WMO_OBJECT_INFO * GetWMOObjectInfo(UINT Index);

	bool GetHeightByXZ(FLOAT x,FLOAT z,FLOAT& Height,FLOAT& WaterHeight);
public:	

	bool LoadTerrain(LPCTSTR ModelFileName);
	bool LoadObjectsInfo(LPCTSTR ModelFileName,bool BeLoadObject);
	bool LoadTextureInfo(LPCTSTR ModelFileName,bool IsBigAlphaMask);

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
protected:
	CD3DFX * BuildFX(UINT Index,UINT LayCount,bool HaveShadowMap,bool UseNewShader);
	CD3DFX * BuildLiquidFX();
	bool LoadAlphaLayer(TEXTURE_LAYER_INFO& LayInfo,UINT LayerCount,BLZ_CHUNK_MCLY * pMCLY,BLZ_CHUNK_MCAL * pMCAL,UINT ID,bool IsBigAlphaMask);
	CD3DTexture * CreateAlphaMap(int DataType,LPBYTE pData,UINT DataSize,UINT& ProcessSize,UINT ID,UINT Layer);
	bool LoadShadowMap(TEXTURE_LAYER_INFO& LayInfo,BLZ_CHUNK_MCSH * pMCSH,UINT ID);
	CD3DTexture * LoadLiquidTexture(int LiquidType);
	bool WantRenderWater(BYTE * pRenderMask,int Pos);
};


}