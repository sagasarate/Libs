/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWMOModelResource.h                                 */
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

class CD3DWOWWMOModelResource :
	public CD3DObjectResource
{
public:
	enum SUB_MESH_PROPERTY:UINT64
	{
		SMP_INSIDE=(1<<4),
		SMP_OUTSIDE=(1<<5),
	};
	enum BSP_PLANE_TYPE
	{
		BPT_XY=0,
		BPT_YZ=1,
		BPT_XZ=2,
		BPT_LEAF=4,
	};
	enum FACE_FLAGS
	{
		FF_NO_CAMERA_COLLIDE=0x2,
		FF_NO_COLLIDE=0x4,
		FF_COLLIDE_ONLY=0x8,
	};
	struct MODEL_VERTEXT
	{
		CD3DVector3			Pos;
		CD3DVector3			Normal;		
		D3DCOLOR			Diffuse;
		CD3DVector2			TextureCoord;	
	};

	struct DOODAD_INFO
	{
		CD3DWOWM2ModelResource *	pDoodadModel;
		CD3DVector3					Translation;
		CD3DQuaternion				Rotation;
		FLOAT						Scaling;
		D3DCOLOR					Color;
		WORD						GroupIndex;
	};

	struct DOODAD_SET_INFO
	{
		CEasyString		Name;
		UINT			StartDoodad;
		UINT			DoodadCount;
	};

	struct PORTAL_INFO
	{
		WORD						GroupIndex;
		WORD						Filler;
		CD3DVector3					Normal;
		CD3DVector3					Center;
		FLOAT						Factor;
		CEasyArray<CD3DVector3>		Vertices;		
	};

	struct BSP_NODE
	{
		WORD			PlaneType;
		short			RightChildIndex;
		short			LeftChildIndex;
		WORD			FaceCount;
		WORD			FirstFace;
		float			Distance;
		CD3DBoard *		pFaceBoard;
	};

	struct RENDER_BATCH_INFO
	{
		UINT 				StartIndex;
		WORD 				IndexCount;
		WORD 				StartVertex;
		WORD				VertexCount;
		CD3DTexture *		pTexture1;
		CD3DTexture *		pTexture2;
		CD3DFX *			pFX;
	};

	struct GROUP_INFO
	{
		UINT							Index;
		CEasyString						Name;
		UINT							Flags;
		CD3DBoundingBox					BoundingBox;
		CEasyArray<CD3DSubMesh *>		GroupSubMeshList;
		CEasyArray<PORTAL_INFO>			PortalList;
		CEasyArray<WORD>				IndexList;
		CEasyArray<MODEL_VERTEXT>		VertexList;
		CEasyArray<BSP_NODE>			BSPTree;
		CEasyArray<WORD>				BSPFaceList;
		CEasyArray<RENDER_BATCH_INFO>	RenderBatchs;
		CEasyArray<BYTE>				FaceFlags;
		
	};
	
protected:
	enum SST_MEMBER_ID
	{
		SST_D3DWWMR_DOODAD_INFO_LIST=SST_D3DOR_MAX,
		SST_D3DWWMR_DOODAD_SET_LIST,
		SST_D3DWWMR_GROUP_LIST,		
		SST_D3DWWMR_MAX=SST_D3DOR_MAX+50,
	};
	enum SST_DOODAD_INFO_LIST
	{
		SST_DIL_DOODAD_INFO=1,
	};
	enum SST_DOODAD_INFO
	{
		SST_DI_MODEL_RESOURCE=1,
		SST_DI_TRANSLATION,
		SST_DI_ROTATION,
		SST_DI_SCALING,
		SST_DI_COLOR,
		SST_DI_GROUP_INDEX,
	};
	enum SST_DOODAD_SET_LIST
	{
		SST_DSL_DOODAD_SET=1,
	};
	enum SST_DOODAD_SET
	{
		SST_DS_NAME=1,
		SST_DS_START_DOODAD,
		SST_DS_DOODAD_COUNT,
	};
	enum SST_GROUP_LIST
	{
		SST_GL_GROUP_INFO=1,
	};
	enum SST_PORTAL_LIST
	{
		SST_PL_PORTAL_INFO=1,
	};
	enum SST_PORTAL_INFO
	{
		SST_PI_GROUP_INDEX=1,
		SST_PI_FILLER,
		SST_PI_NORMAL,
		SST_PI_CENTER,
		SST_PI_FACTOR,
		SST_PI_VERTICES,
	};	
	enum SST_BSP_TREE
	{
		SST_BT_BSP_NODE_INFO=1,
	};
	enum SST_BSP_NODE
	{
		SST_BN_PLANE_TYPE=1,
		SST_BN_RIGHT_CHILD_INDEX,
		SST_BN_LEFT_CHILD_INDEX,
		SST_BN_FACE_COUNT,
		SST_BN_FIRST_FACE,
		SST_BN_DISTANCE,
	};
	enum SST_RENDER_BATCH_LIST
	{
		SST_BRL_RENDER_BATCH_INFO=1,
	};
	enum SST_RENDER_BATCH_INFO
	{
		SST_RBI_START_INDEX=1,
		SST_RBI_INDEX_COUNT,
		SST_RBI_START_VERTEX,
		SST_RBI_VERTEX_COUNT,
		SST_RBI_TEXTURE1,
		SST_RBI_TEXTURE2,
		SST_RBI_FX,
	};

	enum SST_GROUP_INFO
	{
		SST_GI_INDEX=1,
		SST_GI_NAME,
		SST_GI_FLAG,
		SST_GI_BOUNDING_BOX,
		SST_GI_PORTAL_LIST,
		SST_GI_FACE_INDEX_LIST,
		SST_GI_FACE_VERTEX_LIST,
		SST_GI_BSP_TREE,
		SST_GI_BSP_FACE_LIST,
		SST_GI_RENER_BATCH_LIST,
		SST_GI_FACE_FLAGS,
	};

	CEasyArray<DOODAD_INFO>			m_DoodadInfos;
	CEasyArray<DOODAD_SET_INFO>		m_DoodadSets;
	CEasyArray<GROUP_INFO>			m_Groups;

	DECLARE_CLASS_INFO(CD3DWOWWMOModelResource)
public:
	CD3DWOWWMOModelResource(void);
	CD3DWOWWMOModelResource(CD3DObjectResourceManager* pManager);
	~CD3DWOWWMOModelResource(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadFromFile(LPCTSTR ModelFileName);

	UINT GetDoodadSetCount();
	DOODAD_SET_INFO * GetDoodadSetInfo(UINT Index);

	UINT GetDoodadCount();
	DOODAD_INFO * GetDoodadInfo(UINT Index);

	UINT GetGroupCount();
	GROUP_INFO * GetGroupInfo(UINT Index);



public:	

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

protected:
	bool LoadGroup(GROUP_INFO& GroupInfo,LPCTSTR ModelFileName,BLZ_CHUNK_MOMT * pMaterials,UINT MaterialCount,BLZ_CHUNK_MOTX * pTextureNames,BLZ_CHUNK_MOGN * pGroupNames);

	bool LoadDoodads(UINT DoodadCount,UINT DoodadSetCount,BLZ_CHUNK_MODS * pDoodadSets,BLZ_CHUNK_MODN * pDoodadFileNames,BLZ_CHUNK_MODD * pDoodads);
	bool LoadGroups(LPCTSTR ModelFileName,UINT GroupCount,BLZ_CHUNK_MOGI * pGroups,BLZ_CHUNK_MOGN * pGroupNames,BLZ_CHUNK_MOMT * pMaterials,UINT MaterialCount,BLZ_CHUNK_MOTX * pTextureNames);

	
	bool MakeMaterial(RENDER_BATCH_INFO& BatchInfo,WMOMaterial& MaterialInfo,BLZ_CHUNK_MOTX * pTextureNames);

	CD3DFX * BuildFX(UINT BlendMode,UINT TextureFlag);

	
	void BuildSubMeshs();

	
};

}