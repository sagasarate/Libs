/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWM2ModelResource.h                                  */
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

class CD3DWOWM2ModelResource :
	public CD3DObjectResource
{
public:
	enum SUB_MESH_PROPERTY:UINT64
	{		
		SMP_RENDER_FLAG_SHIFT=8,
		SMP_RENDER_FLAG_MASK=(((UINT64)0xFF)<<SMP_RENDER_FLAG_SHIFT),

		SMP_BLEND_MODE_SHIFT=16,
		SMP_BLEND_MODE_MASK=(((UINT64)0xFF)<<SMP_BLEND_MODE_SHIFT),

		SMP_MESH_FLAG_SHIFT=24,
		SMP_MESH_FLAG_MASK=(((UINT64)0xFF)<<SMP_MESH_FLAG_SHIFT),

		SMP_COLOR_ANI_INDEX_SHIFT=32,
		SMP_COLOR_ANI_INDEX_MASK=(((UINT64)0xFF)<<SMP_COLOR_ANI_INDEX_SHIFT),

		SMP_TRANSPARENCY_ANI_INDEX_SHIFT=40,
		SMP_TRANSPARENCY_ANI_INDEX_MASK=(((UINT64)0xFF)<<SMP_TRANSPARENCY_ANI_INDEX_SHIFT),
	};

	enum TEXTURE_FLAG:UINT64
	{
		TF_TEXTURE_FLAG_SHIFT=0,
		TF_TEXTURE_FLAG_MASK=(((UINT64)0xFF)),

		TF_BLEND_MODE_SHIFT=8,
		TF_BLEND_MODE_MASK=(((UINT64)0xFF)<<TF_TEXTURE_FLAG_SHIFT),

		TF_SKIN_TEXTURE_TYPE_SHIFT=16,
		TF_SKIN_TEXTURE_TYPE_MASK=(((UINT64)0xFF)<<TF_SKIN_TEXTURE_TYPE_SHIFT),
		
		TF_UV_ANI_INDEX_SHIFT=24,
		TF_UV_ANI_INDEX_MASK=(((UINT64)0xFF)<<TF_UV_ANI_INDEX_SHIFT),
	};
	

	struct MODEL_VERTEXT
	{
		CD3DVector3			Pos;
		FLOAT				BoneWeight[MAX_VERTEX_BONE_BIND];
		BYTE				BoneID[MAX_VERTEX_BONE_BIND];
		CD3DVector3			Normal;		
		D3DCOLOR			Diffuse;
		CD3DVector2			TextureCoord;	
	};
	
	struct ANIMATION_SEQUENCE
	{
		UINT				AnimationID;
		CEasyString			AnimationName;		
		UINT				SubAnimationID;
		int					Index;
		UINT				Length;
		FLOAT				MovingSpeed;
		UINT				Flags;
		UINT				Flags2;		
		UINT				PlaybackSpeed;
		CD3DBoundingBox		BoundingBox;
		CD3DBoundingSphere	BoundingSphere;		
	};	
	struct COLOR_ANIMATION_INFO
	{
		ANIMATION_COLOR_BLOCK	ColorAnimations;
		ANIMATION_ALPHA_BLOCK	AlphaAnimations;
	};
	struct TRANSPARENCY_ANIMATION_INFO
	{
		ANIMATION_ALPHA_BLOCK	AlphaAnimations;
	};
	struct MODEL_BONE
	{
		int								AnimationSeq;
		UINT							Flags;
		int								ChildCount;
		short							ParentBone;
		WORD							GeosetID;				
		CD3DVector3						PivotPoint;		
		ANIMATION_TRANSLATION_BLOCK		Translations;
		ANIMATION_ROTATION_BLOCK		Rotations;
		ANIMATION_SCALING_BLOCK			Scalings;


	};
	enum MODLE_BONE_FLAG
	{
		BONE_FLAG_BILLBOARD=0x8,
		BONE_FLAG_BILLBOARD_HORIZONTAL=0x40,
		BONE_FLAG_HAVE_TRANSFORM=0x200,
	};
	enum MODEL_ATTACHMENT_TYPE
	{
		MAT_DUMMY,
		MAT_PARTICLE_EMITTER,
		MAT_RIBBON_EMITTER,
	};
	struct BONE_MATRIX
	{
		WORD			BoneID;		
		short			ParentID;
		UINT			Flags;
		CD3DVector3		PivotPoint;
		CD3DMatrix		Matrix;
		WORD			AttachmentType;
		WORD			AttachmentID;
		CD3DMatrix		AttachTransform;
		CD3DObject *	pAttachObject;
		BONE_MATRIX()
		{
			BoneID=0;
			ParentID=0;
			Flags=0;
			AttachmentType=0;
			AttachmentID=0;
			pAttachObject=NULL;
		}
	};	
	struct MODEL_ATTACHMENT
	{
		WORD						ID;
		WORD						Bone;
		CEasyString					Name;
		CD3DVector3					Position;
		//ANIMATION_TRANSLATION_BLOCK	AnimationData;
	};

	struct PARTICLE_EMITTER_BIND_INFO
	{
		CD3DParticleEmitterResource *	pModelResource;
		WORD							BindBone;
		CD3DVector3						BindPosition;
	};

	struct RIBBON_EMITTER_BIND_INFO
	{
		CD3DRibbonEmitterResource *		pModelResource;
		WORD							BindBone;
		CD3DVector3						BindPosition;
	};
		
	struct TEXTURE_UV_ANI
	{		
		ANIMATION_TRANSLATION_BLOCK		Translations;
		ANIMATION_ROTATION_BLOCK		Rotations;
		ANIMATION_SCALING_BLOCK			Scalings;
	};

	struct CAMERA_INFO
	{
		UINT						Type;
		FLOAT						FarClipping;	
		FLOAT						NearClipping;	
		CD3DVector3					Position;
		CD3DVector3					Target;
		ANIMATION_TRANSLATION_BLOCK	TranslationPos;	
		ANIMATION_TRANSLATION_BLOCK	TranslationTar;
		ANIMATION_SCALING_BLOCK		Scaling;
		ANIMATION_BLOCK<FLOAT>		FOV;
	};

protected:
	enum LOCAL_DEFINES
	{
		MAX_M2_ANI_NAME_LEN=64,
		MAX_M2_ATTACHMENT_NAME_LEN=64,
	};
	enum SST_MEMBER_ID
	{
		SST_D3DWMMR_BONE_ANIMATION_SEQUENCE=SST_D3DOR_MAX,
		SST_D3DWMMR_COLOR_ANIMATION,
		SST_D3DWMMR_TRANSPARENCY_ANIMATION,
		SST_D3DWMMR_TEXTURE_UV_ANIMATION,
		SST_D3DWMMR_BONE,
		SST_D3DWMMR_SKIN_MESH_BONE_COUNT,
		SST_D3DWMMR_KEY_BONE,
		SST_D3DWMMR_GLOBAL_SEQUENCE,
		SST_D3DWMMR_ATTACHMENT,
		SST_D3DWMMR_PARTICLE_EMITTER,
		SST_D3DWMMR_RIBBON_EMITTER,
		SST_D3DWMMR_FLAG,
		SST_D3DWMMR_MAX=SST_D3DOR_MAX+50,
	};
	enum SST_BONE_ANIMATION_SEQUENCE
	{
		SST_BAS_ANIMATION_ID=1,
		SST_BAS_ANIMATION_NAME,
		SST_BAS_SUBANIMATION_ID,
		SST_BAS_INDEX,
		SST_BAS_LENGTH,
		SST_BAS_MOVING_SPEED,
		SST_BAS_FLAGS,
		SST_BAS_FLAGS2,
		SST_BAS_PLAYBACK_SPEED,
		SST_BAS_BOUNDING_BOX,
		SST_BAS_BOUNDING_SPHERE,
	};	
	enum SST_COLOR_ANIMATION
	{
		SST_CA_COLOR_ANIMATION=1,
		SST_CA_ALPHA_ANIMATION,
	};
	enum SST_TRANSPARENCY_ANIMATION
	{
		SST_TA_ALPHA_ANIMATION=1,
	};
	enum SST_TEXTURE_UV_ANIMATION
	{
		SST_TUVA_TRANSLATIONS=1,
		SST_TUVA_ROTATIONS,
		SST_TUVA_SCALINGS,
	};
	enum SST_BONE_INFO
	{
		SST_BI_ANIMATION_SEQ=1,
		SST_BI_FLAGS,
		SST_BI_PARENT_BONE,
		SST_BI_GEOSET_ID,	
		SST_BI_PIVOT_POINT,
		SST_BI_TRANSLATIONS,
		SST_BI_ROTATIONS,
		SST_BI_SCALINGS,	
	};
	enum SST_ATTACHMENT_INFO
	{
		SST_AI_ID=1,
		SST_AI_BONE,
		SST_AI_NAME,
		SST_AI_POSITION,
	};

	enum SST_PARTICLE_EMITTER_BIND_INFO
	{
		SST_PEBI_MODEL_RESOURCE=1,
		SST_PEBI_POSITION,
		SST_PEBI_BONE,
	};

	enum SST_RIBBON_EMITTER_BIND_INFO
	{
		SST_REBI_MODEL_RESOURCE=1,
		SST_REBI_POSITION,
		SST_REBI_BONE,
	};
	
	
	
	struct BONE_ANI_CACHE_FRAME
	{
		int							BoneID;
		CEasyArray<CD3DMatrix>		Matrix;		
	};


	struct BONE_ANI_CACHE
	{
		int									AniIndex;
		CEasyArray<BONE_ANI_CACHE_FRAME>	Bones;
	};


	CEasyString									m_ModelFilePath;
	CEasyArray<CEasyString>						m_SkinFiles;
	CEasyArray<ANIMATION_SEQUENCE>				m_AnimationSequence;
	CEasyArray<COLOR_ANIMATION_INFO>			m_ColorAnimations;
	CEasyArray<TRANSPARENCY_ANIMATION_INFO>		m_TransparencyAnimations;	
	CEasyArray<TEXTURE_UV_ANI>					m_TextureUVAnimations;

	CEasyArray<MODEL_BONE>						m_Bones;
	UINT										m_SkinMeshBoneCount;
	CEasyArray<short>							m_KeyBoneIndex;

	CEasyArray<UINT>							m_GlobalSequences;

	CEasyArray<MODEL_ATTACHMENT>				m_Attachments;

	CEasyArray<PARTICLE_EMITTER_BIND_INFO>		m_ParticleEmitters;
	CEasyArray<RIBBON_EMITTER_BIND_INFO>		m_RibbonEmitters;

	bool										m_HasBoneAni;
	bool										m_HasMaterialAni;
	bool										m_HasSkinMeshAni;
	UINT										m_BoneAniCacheSize;
	UINT										m_BoneAniCacheFreq;
	CEasyList<BONE_ANI_CACHE>					m_BoneAniCache;
	CEasyArray<BONE_ANI_CACHE>					m_GlobalBoneAniCache;
	UINT										m_Flag;
	UINT										m_Version;
	CEasyArray<CAMERA_INFO>						m_CameraInfos;

	CEasyArray<MODEL_VERTEXT>					m_Vertices;
	CEasyArray<WORD>							m_Indices;

	

	
	DECLARE_CLASS_INFO(CD3DWOWM2ModelResource)
public:

	

	CD3DWOWM2ModelResource(void);
	CD3DWOWM2ModelResource(CD3DObjectResourceManager* pManager);
	~CD3DWOWM2ModelResource(void);

	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();

	bool LoadFromFile(LPCTSTR szModelFileName);

	bool LoadFromXFile(LPCTSTR szModelFileName);

	bool ChangeSkin(UINT Index);

public:	

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);

	int GetAnimationID(LPCTSTR AnimationName);
	ANIMATION_SEQUENCE * FindAnimationInfo(UINT AnimationID,UINT SubAnimationID);
	int GetAnimationIndex(UINT AnimationID,UINT SubAnimationID);
	ANIMATION_SEQUENCE * GetAnimationInfo(UINT AnimationID,UINT SubAnimationID);
	UINT GetAnimationCount();
	ANIMATION_SEQUENCE * GetAnimationInfoByIndex(UINT AnimationIndex);

	UINT GetBoneCount();
	UINT GetSkinMeshBoneCount();
	MODEL_BONE * GetBoneInfo(UINT Index);
	int GetKeyBone(UINT Key);
	UINT GetKeyBoneCount();
	bool HasBoneAni();
	bool HasMaterialAni();

	bool MakeAnimationBoneFrame(UINT AnimationIndex,UINT Time,bool IsLoop,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix=true);
	bool MakeAnimationBoneFrameByTree(UINT AnimationIndex,UINT Time,bool IsLoop,UINT StartBone,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix=true);
	bool MakeAnimationBoneFrameByTree(ANIMATION_SEQUENCE * pAniInfo,UINT Time,bool IsLoop,UINT StartBone,BONE_MATRIX * pMatrixBuffer,UINT MatrixBufferSize,bool ResetMatrix=true);

	bool MakeColorAnimationFrame(UINT AniIndex,UINT Time,bool IsLoop,D3DCOLORVALUE& Color);
	bool MakeTransparencyAnimationFrame(UINT AniIndex,UINT Time,bool IsLoop,FLOAT& Alpha);

	MODEL_ATTACHMENT * GetAttachment(UINT ID);
	UINT GetAttachmentCount();
	MODEL_ATTACHMENT * GetAttachmentByIndex(UINT Index);

	UINT GetParticleEmitterCount();
	PARTICLE_EMITTER_BIND_INFO * GetParticleEmitterInfo(UINT Index);

	

	UINT GetRibbonEmitterCount();
	RIBBON_EMITTER_BIND_INFO * GetRibbonEmitterInfo(UINT Index);


	bool MakeTextureUVAniFrame(UINT Index,UINT Time,bool IsLoop,CD3DMatrix& Frame);

	void SetBoneAniCache(UINT CacheSize,UINT CacheFreq);

	UINT CaculateDataSize();

	UINT GetFlag();
	UINT GetVersion();

	UINT GetCameraCount();
	CAMERA_INFO * GetCameraInfo(UINT Index);

	UINT GetVertexCount();
	MODEL_VERTEXT * GetVertices();
	UINT GetIndexCount();
	WORD * GetIndices();

	UINT GetSkinCount();
	LPCTSTR GetSkinFileName(UINT Index);
	

protected:
	bool LoadSkin(LPCTSTR SkinFileName,CEasyArray<CD3DSubMesh *>& SubMeshList,BYTE * pModelData);
	void MakeSubmeshMaterial(BYTE * pModelData,BYTE * pSkinData,UINT SubMeshIndex,CD3DSubMesh * pD3DSubMesh);
	void CreateBounding();
	bool LoadBones(BYTE * pModelData,LPCTSTR szModelPath);
	bool LoadAnimationFromFile(BYTE * pData,UINT AnimationID,UINT SubAnimationID,UINT AniIndex,LPCTSTR szModelPath,M2_MODEL_BONE * pOrgBoneInfo);

	bool LoadColorAnimation(BYTE * pModelData);
	bool LoadTransparencyAnimation(BYTE * pModelData);
	bool LoadRibbonEmitters(BYTE * pModelData);
	bool LoadParticleEmitters(BYTE * pModelData);
	bool LoadTextureUVAnimation(BYTE * pModelData);

	void LoadBoneAnimation(BYTE * pData,BYTE * pAniData,MODEL_BONE& BoneInfo,M2_MODEL_BONE& OrgBoneInfo,UINT AniIndex);

	



	//WORD RebuildVertexIndex(CEasyArray<WORD>& VertexIndexList,WORD VertexIndex);
	void BuildSubMeshData();

	bool LoadAttachments(BYTE * pModelData);
	bool LoadCameraInfos(BYTE * pModelData);

	void BuildFX(CD3DSubMesh * pSubMesh);

	bool MakeOneAnimationBoneFrame(ANIMATION_SEQUENCE * pAniInfo,UINT Time,bool IsLoop,UINT Bone,BONE_MATRIX * pMatrix,bool UseCache);

	bool BuildGlobalBoneAniCache();
	BONE_ANI_CACHE * BuildBoneAniCache(ANIMATION_SEQUENCE * pAniInfo);
	BONE_ANI_CACHE * FindBoneAniCache(UINT AniIndex);
	bool MakeOneAnimationBoneFrameFromCache(BONE_ANI_CACHE * pAniCache,UINT Time,bool IsLoop,UINT Bone,CD3DMatrix& TransformMatrix);

	
	void CheckAni();

	bool IsAniBone(UINT Bone);
	void CheckSkins(LPCTSTR szModelPath);

	
};


inline bool CD3DWOWM2ModelResource::HasBoneAni()
{
	return m_HasBoneAni;
}

inline bool CD3DWOWM2ModelResource::HasMaterialAni()
{
	return m_HasMaterialAni;
}

inline UINT CD3DWOWM2ModelResource::GetFlag()
{
	return m_Flag;
}

inline UINT CD3DWOWM2ModelResource::GetVersion()
{
	return m_Version;
}

inline UINT CD3DWOWM2ModelResource::GetVertexCount()
{
	return m_Vertices.GetCount();
}
inline CD3DWOWM2ModelResource::MODEL_VERTEXT * CD3DWOWM2ModelResource::GetVertices()
{
	return m_Vertices.GetBuffer();
}
inline UINT CD3DWOWM2ModelResource::GetIndexCount()
{
	return m_Indices.GetCount();
}
inline WORD * CD3DWOWM2ModelResource::GetIndices()
{
	return m_Indices.GetBuffer();
}

inline UINT CD3DWOWM2ModelResource::GetSkinCount()
{
	return m_SkinFiles.GetCount();
}
inline LPCTSTR CD3DWOWM2ModelResource::GetSkinFileName(UINT Index)
{
	if(Index<m_SkinFiles.GetCount())
		return m_SkinFiles[Index];
	return NULL;
}

}