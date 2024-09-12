/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSubMeshMaterial.h                                     */
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

class CD3DDevice;
class CD3DTexture;
class CD3DFX;

class CD3DSubMeshMaterial :
	public CNameObject
{
protected:
	//struct STORAGE_STRUCT:public CNameObject::STORAGE_STRUCT
	//{		
	//	D3DMATERIAL9		Material;
	//	int					TextureCount;
	//	int					TextureIndex[MAX_TEXTURE_LAYER];
	//	UINT64				TextureProperty[MAX_TEXTURE_LAYER];
	//	int					FXIndex;
	//};
	enum SST_MEMBER_ID
	{
		SST_D3DSMM_MATERIAL=SST_NO_MAX,
		SST_D3DSMM_TEXTURE,
		SST_D3DSMM_FX,
		SST_D3DSMM_GLOBAL_COLOR,
		SST_D3DSMM_GLOBAL_COLOR_FX_PARAM_NAME,
		SST_D3DSMM_MAX=SST_NO_MAX+50,
	};
	enum SST_TEXTURE
	{
		SST_TEX_TEXTURE=1,
		SST_TEX_PROPERTY,
		SST_TEX_TEX_FX_PARAM_NAME,
		SST_TEX_MAT_FX_PARAM_NAME,
	};
	struct MATERIAL_TEXTURE
	{
		CD3DTexture*	pTexture;
		UINT64			Property;
		CD3DMatrix		UVTransform;
		CEasyStringA	TexFXParamName;
		CEasyStringA	MatFXParamName;
	};
	D3DMATERIAL9					m_Material;
	D3DCOLORVALUE					m_GlobalColor;
	CEasyStringA					m_GlobalColorFXParamName;
	CEasyArray<MATERIAL_TEXTURE>	m_TextureList;	
	CD3DFX*							m_pFX;

	UINT64							m_HashCode[2];
	

	DECLARE_CLASS_INFO(CD3DSubMeshMaterial)
public:
	CD3DSubMeshMaterial(void);
	~CD3DSubMeshMaterial(void);

	virtual void Destory();

	bool Reset();
	bool Restore();

	void SetMaterial(D3DMATERIAL9& Material);
	D3DMATERIAL9& GetMaterial();	

	void SetGlobalColor(D3DCOLORVALUE& Color);
	D3DCOLORVALUE& GetGlobalColor();

	void SetGlobalColorFXParamName(LPCSTR szName);
	LPCSTR GetGlobalColorFXParamName();

	void SetFX(CD3DFX * pFX);
	CD3DFX * GetFX();

	void AddTexture(CD3DTexture * pTexture,UINT64 Property,LPCSTR szTexFXParamName,LPCSTR szMatFXParamName);
	bool SetTexture(UINT Layer,CD3DTexture * pTexture);
	bool SetTextureProperty(UINT Layer,UINT64 Property);
	bool SetTextureUVTransform(UINT Layer,CD3DMatrix& TransformMat);
	CD3DTexture * GetTexture(UINT Layer);
	UINT64 GetTextureProperty(UINT Layer);
	CD3DMatrix& GetTextureUVTransform(UINT Layer);
	LPCSTR GetTextureFXParamName(UINT Layer);
	LPCSTR GetTextureMatFXParamName(UINT Layer);
	UINT GetTextureLayerCount();
	void ClearAllTexture();

	void CaculateHashCode();
	void GetHashCode(UINT64& HashCode1,UINT64& HashCode2);

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);
protected:
	

	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);	

	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	
};


inline void CD3DSubMeshMaterial::SetMaterial(D3DMATERIAL9& Material)
{
	m_Material=Material;
}

inline D3DMATERIAL9& CD3DSubMeshMaterial::GetMaterial()
{
	return m_Material;
}

inline void CD3DSubMeshMaterial::SetGlobalColor(D3DCOLORVALUE& Color)
{
	m_GlobalColor=Color;
}
inline D3DCOLORVALUE& CD3DSubMeshMaterial::GetGlobalColor()
{
	return m_GlobalColor;
}

inline void CD3DSubMeshMaterial::SetGlobalColorFXParamName(LPCSTR szName)
{
	m_GlobalColorFXParamName=szName;
}
inline LPCSTR CD3DSubMeshMaterial::GetGlobalColorFXParamName()
{
	return m_GlobalColorFXParamName;
}

inline CD3DFX * CD3DSubMeshMaterial::GetFX()
{
	return m_pFX;
}

inline void CD3DSubMeshMaterial::AddTexture(CD3DTexture * pTexture,UINT64 Property,LPCSTR szTexFXParamName,LPCSTR szMatFXParamName)
{
	MATERIAL_TEXTURE Tex;
	Tex.pTexture=pTexture;
	Tex.Property=Property;
	Tex.UVTransform.SetIdentity();
	Tex.TexFXParamName=szTexFXParamName;
	Tex.MatFXParamName=szMatFXParamName;
	m_TextureList.Add(Tex);
}

inline bool CD3DSubMeshMaterial::SetTextureProperty(UINT Layer,UINT64 Property)
{
	if(Layer<m_TextureList.GetCount())
	{
		m_TextureList[Layer].Property=Property;
		return true;
	}
	return false;
}

inline bool CD3DSubMeshMaterial::SetTextureUVTransform(UINT Layer,CD3DMatrix& TransformMat)
{
	if(Layer<m_TextureList.GetCount())
	{
		m_TextureList[Layer].UVTransform=TransformMat;
		return true;
	}
	return false;
}

inline CD3DTexture * CD3DSubMeshMaterial::GetTexture(UINT Layer)
{
	if(Layer<m_TextureList.GetCount())
	{
		return m_TextureList[Layer].pTexture;
	}
	return NULL;
}

inline UINT64 CD3DSubMeshMaterial::GetTextureProperty(UINT Layer)
{
	if(Layer<m_TextureList.GetCount())
	{
		return m_TextureList[Layer].Property;
	}
	return 0;
}

inline CD3DMatrix& CD3DSubMeshMaterial::GetTextureUVTransform(UINT Layer)
{
	static CD3DMatrix Mat;
	if(Layer<m_TextureList.GetCount())
	{
		return m_TextureList[Layer].UVTransform;
	}
	return Mat;
}

inline LPCSTR CD3DSubMeshMaterial::GetTextureFXParamName(UINT Layer)
{
	if(Layer<m_TextureList.GetCount())
	{
		return m_TextureList[Layer].TexFXParamName;
	}
	return "";
}
inline LPCSTR CD3DSubMeshMaterial::GetTextureMatFXParamName(UINT Layer)
{
	if(Layer<m_TextureList.GetCount())
	{
		return m_TextureList[Layer].MatFXParamName;
	}
	return "";
}

inline UINT CD3DSubMeshMaterial::GetTextureLayerCount()
{
	return (UINT)m_TextureList.GetCount();
}

inline void CD3DSubMeshMaterial::GetHashCode(UINT64& HashCode1,UINT64& HashCode2)
{
	HashCode1=m_HashCode[0];
	HashCode2=m_HashCode[1];
}

}