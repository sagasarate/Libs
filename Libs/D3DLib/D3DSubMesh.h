/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSubMesh.h                                             */
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

enum SUBMESH_CLONE_PARAM
{
	SUBMESH_CLONE_USE_SYSTEM_MEM=(1<<1),
	SUBMESH_CLONE_NOT_CLONE_MATERIAL=(1<<2),
};
	
class CD3DDevice;
	
class CD3DSubMesh:public CNameObject
{
	DECLARE_CLASS_INFO(CD3DSubMesh)
public:
	struct VERTEX_FORMAT
	{
		DWORD			FVF;
		WORD			VertexSize;
		WORD			IndexSize;
		LPDIRECT3DVERTEXDECLARATION9	pVertexDeclaration;
	};	
	enum SUB_MESH_PROPERTYS
	{
		SMF_IS_VISIBLE=1,
		SMF_IS_TRANSPARENT=(1<<1),		
		SMF_IS_ANI_MESH=(1<<2),
		SMF_HAVE_COLLIDE=(1<<3),
	};
	enum SUB_MESH_FLAGS
	{		
		SMF_IS_CULLED=1,
		SMF_IS_SELECTED=(1<<1),	
	};
	enum BUFFER_USED
	{
		BUFFER_USE_DX,
		BUFFER_USE_CUSTOM,
	};
	
	enum SST_MEMBER_ID
	{
		SST_D3DSM_FVF=SST_NO_MAX,
		SST_D3DSM_VERTEX_DECLARATION,
		SST_D3DSM_VERTEX_SIZE,
		SST_D3DSM_INDEX_SIZE,
		SST_D3DSM_PRIMITIVE_TYPE,
		SST_D3DSM_PRIMITIVE_COUNT,
		SST_D3DSM_VERTEX_COUNT,	
		SST_D3DSM_INDEX_COUNT,	
		SST_D3DSM_BOUNDING_BOX,
		SST_D3DSM_BOUNDING_SPHERE,
		SST_D3DSM_PROPERTY,
		SST_D3DSM_VERTEX,
		SST_D3DSM_INDEX,
		SST_D3DSM_MATERIAL,
		SST_D3DSM_MAX=SST_NO_MAX+50,
	};
	
protected:
	CD3DDevice *			m_pDevice;
	VERTEX_FORMAT			m_VertexFormat;
	UINT					m_PrimitiveType;
	UINT					m_PrimitiveCount;
	BYTE*					m_pVertexBuffer;	
	LPDIRECT3DVERTEXBUFFER9 m_pDXVertexBuffer;
	bool					m_IsVertexsSelfDelete;
	bool					m_IsDXVertexBufferSelfRelease;
	UINT					m_VertexCount;
	UINT					m_StartVertex;
	BYTE*					m_pIndexBuffer;
	LPDIRECT3DINDEXBUFFER9	m_pDXIndexBuffer;
	bool					m_IsIndexsSelfDelete;
	bool					m_IsDXIndexBufferSelfRelease;
	UINT					m_IndexCount;	
	UINT					m_StartIndex;
	CD3DSubMeshMaterial		m_Material;
	CD3DBoundingBox			m_BoundingBox;
	CD3DBoundingSphere		m_BoundingSphere;
	UINT64					m_Property;
	UINT64					m_Flag;
	UINT					m_RenderBufferUsed;
	UINT					m_OrginDataBufferUsed;

	BYTE*					m_pVertexBufferR;
	bool					m_IsVertexBufferRSelfDelete;

	BYTE*					m_pIndexBufferR;
	bool					m_IsIndexBufferRSelfDelete;

	UINT					m_PrimitiveCountR;
	UINT					m_VertexCountR;
	UINT					m_StartVertexR;
	UINT					m_IndexCountR;	
	UINT					m_StartIndexR;

	
public:

	CD3DSubMesh(void);
	CD3DSubMesh(CD3DDevice * pDevice);
	virtual ~CD3DSubMesh(void);

	virtual void Destory();
	

	bool Reset();
	bool Restore();

	void SetDevice(CD3DDevice * pDevice);
	CD3DDevice * GetDevice();

	void CreateBounding();
	void CreateBoundingBox();
	void CreateBoundingSphere();
	CD3DBoundingBox GetBoundingBoxWithTranform(const CD3DMatrix& TransformMat);
	void AppendBoundingBoxWithTranform(CD3DBoundingBox& BBox,const CD3DMatrix& TransformMat);

	bool RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,bool TestOnly);
	bool LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue);
	bool GetHeightByXZ(const CD3DMatrix& WorldMatrix,const CD3DVector3& Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight);

	VERTEX_FORMAT& GetVertexFormat();
	bool DeclareVertexFormat(D3DVERTEXELEMENT9* pVertexElements,WORD VertexSize,WORD IndexSize);

	UINT GetPrimitiveType();
	void SetPrimitiveType(UINT Type);
	UINT GetPrimitiveCount();
	void SetPrimitiveCount(UINT Count);

	BYTE * GetVertexBuffer();
	void SetVertices(BYTE * pVertexBuffer);
	void AllocVertexBuffer();
	LPDIRECT3DVERTEXBUFFER9 GetDXVertexBuffer();
	void SetDXVertexBuffer(LPDIRECT3DVERTEXBUFFER9 pDXVertexBuffer,bool IsSelfRelease=false);
	bool AllocDXVertexBuffer(DWORD Usage=0,D3DPOOL Pool=D3DPOOL_MANAGED);	
	UINT GetVertexCount();
	void SetVertexCount(UINT Count);
	UINT GetVertexStart();
	void SetVertexStart(UINT Start);

	BYTE * GetIndexBuffer();
	void SetIndices(BYTE * pIndexBuffer);
	void AllocIndexBuffer();
	LPDIRECT3DINDEXBUFFER9 GetDXIndexBuffer();
	void SetDXIndexBuffer(LPDIRECT3DINDEXBUFFER9 pDXIndexBuffer,bool IsSelfRelease=false);
	bool AllocDXIndexBuffer(DWORD Usage=0,D3DPOOL Pool=D3DPOOL_MANAGED);
	UINT GetIndexCount();
	void SetIndexCount(UINT Count);
	UINT GetIndexStart();
	void SetIndexStart(UINT Start);

	CD3DSubMeshMaterial& GetMaterial();
	CD3DBoundingBox& GetBoundingBox();
	void SetBoundingBox(const CD3DBoundingBox& BoundingBox);
	CD3DBoundingSphere& GetBoundingSphere();
	void SetBoundingSphere(const CD3DBoundingSphere& BoundingSphere);

	UINT64 GetProperty();
	void SetProperty(UINT64 Property);
	void AddProperty(UINT64 Property);
	void RemoveProperty(UINT64 Property);
	bool CheckProperty(UINT64 Property);

	UINT64 GetFlag();
	void SetFlag(UINT64 Flag);
	void AddFlag(UINT64 Flag);
	void RemoveFlag(UINT64 Flag);
	bool CheckFlag(UINT64 Flag);

	bool IsTransparent();
	void SetTransparent(bool Enable);
	bool IsCulled();
	void SetCulled(bool Enable);
	bool IsSelected();
	void SetSelected(bool Enable);
	bool IsVisible();
	void SetVisible(bool Enable);

	UINT GetRenderBufferUsed();
	void SetRenderBufferUsed(UINT Which);
	UINT GetOrginDataBufferUsed();
	void SetOrginDataBufferUsed(UINT Which);

	void SetVertexBufferR(BYTE * pVertexBuffer,bool IsSelfDelete=false);
	void AllocVertexBufferR();
	BYTE * GetVertexBufferR();

	void SetIndexBufferR(BYTE * pIndexBuffer,bool IsSelfDelete=false);
	void AllocIndexBufferR();
	BYTE * GetIndexBufferR();

	UINT GetPrimitiveCountR();
	UINT GetVertexCountR();
	UINT GetVertexStartR();
	UINT GetIndexCountR();
	UINT GetIndexStartR();

	void OnPrepareRenderData();


	static bool SortByName(CD3DSubMesh * pSubMesh1,CD3DSubMesh * pSubMesh2);
	static bool SortByRender(CD3DSubMesh * pSubMesh1,CD3DSubMesh * pSubMesh2);

	virtual bool CloneFrom(CNameObject * pObject,UINT Param=0);

	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0);

	virtual bool ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual bool FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param=0);
	virtual UINT GetSmartStructSize(UINT Param=0);


	bool CheckValid();
	bool CheckValidR();
protected:
	
};

inline void CD3DSubMesh::SetDevice(CD3DDevice * pDevice)
{
	m_pDevice=pDevice;
}
inline CD3DDevice * CD3DSubMesh::GetDevice()
{
	return m_pDevice;
}


inline CD3DSubMesh::VERTEX_FORMAT& CD3DSubMesh::GetVertexFormat()
{
	return m_VertexFormat;
}

inline UINT CD3DSubMesh::GetPrimitiveType()
{
	return m_PrimitiveType;
}
inline void CD3DSubMesh::SetPrimitiveType(UINT Type)
{
	m_PrimitiveType=Type;
}
inline UINT CD3DSubMesh::GetPrimitiveCount()
{
	return m_PrimitiveCount;
}
inline void CD3DSubMesh::SetPrimitiveCount(UINT Count)
{
	m_PrimitiveCount=Count;
}

inline BYTE * CD3DSubMesh::GetVertexBuffer()
{
	return m_pVertexBuffer;
}

inline LPDIRECT3DVERTEXBUFFER9 CD3DSubMesh::GetDXVertexBuffer()
{
	return m_pDXVertexBuffer;
}
inline void CD3DSubMesh::SetDXVertexBuffer(LPDIRECT3DVERTEXBUFFER9 pDXVertexBuffer,bool IsSelfRelease)
{
	m_pDXVertexBuffer=pDXVertexBuffer;
	m_IsDXVertexBufferSelfRelease=IsSelfRelease;
}
inline UINT CD3DSubMesh::GetVertexCount()
{
	return m_VertexCount;
}
inline void CD3DSubMesh::SetVertexCount(UINT Count)
{
	m_VertexCount=Count;
}
inline UINT CD3DSubMesh::GetVertexStart()
{
	return m_StartVertex;
}
inline void CD3DSubMesh::SetVertexStart(UINT Start)
{
	m_StartVertex=Start;
}


inline BYTE * CD3DSubMesh::GetIndexBuffer()
{
	return m_pIndexBuffer;
}

inline LPDIRECT3DINDEXBUFFER9 CD3DSubMesh::GetDXIndexBuffer()
{
	return m_pDXIndexBuffer;
}
inline void CD3DSubMesh::SetDXIndexBuffer(LPDIRECT3DINDEXBUFFER9 pDXIndexBuffer,bool IsSelfRelease)
{
	m_pDXIndexBuffer=pDXIndexBuffer;
	m_IsDXIndexBufferSelfRelease=IsSelfRelease;
}
inline UINT CD3DSubMesh::GetIndexCount()
{
	return m_IndexCount;
}
inline void CD3DSubMesh::SetIndexCount(UINT Count)
{
	m_IndexCount=Count;
}
inline UINT CD3DSubMesh::GetIndexStart()
{
	return m_StartIndex;
}
inline void CD3DSubMesh::SetIndexStart(UINT Start)
{
	m_StartIndex=Start;
}


inline CD3DSubMeshMaterial& CD3DSubMesh::GetMaterial()
{
	return m_Material;
}
inline CD3DBoundingBox& CD3DSubMesh::GetBoundingBox()
{
	return m_BoundingBox;
}
inline void CD3DSubMesh::SetBoundingBox(const CD3DBoundingBox& BoundingBox)
{
	m_BoundingBox=BoundingBox;
}
inline CD3DBoundingSphere& CD3DSubMesh::GetBoundingSphere()
{
	return m_BoundingSphere;
}
inline void CD3DSubMesh::SetBoundingSphere(const CD3DBoundingSphere& BoundingSphere)
{
	m_BoundingSphere=BoundingSphere;
}

inline UINT64 CD3DSubMesh::GetProperty()
{
	return m_Property;
}
inline void CD3DSubMesh::SetProperty(UINT64 Property)
{
	m_Property=Property;
}
inline void CD3DSubMesh::AddProperty(UINT64 Property)
{
	m_Property|=Property;
}
inline void CD3DSubMesh::RemoveProperty(UINT64 Property)
{
	m_Property&=~Property;
}
inline bool CD3DSubMesh::CheckProperty(UINT64 Property)
{
	return (m_Property&Property)!=0;
}
inline UINT64 CD3DSubMesh::GetFlag()
{
	return m_Flag;
}
inline void CD3DSubMesh::SetFlag(UINT64 Flag)
{
	m_Flag=Flag;
}
inline void CD3DSubMesh::AddFlag(UINT64 Flag)
{
	m_Flag|=Flag;
}
inline void CD3DSubMesh::RemoveFlag(UINT64 Flag)
{
	m_Flag&=~Flag;
}
inline bool CD3DSubMesh::CheckFlag(UINT64 Flag)
{
	return (m_Flag&Flag)!=0;
}


inline bool CD3DSubMesh::IsTransparent()
{
	return (m_Property&SMF_IS_TRANSPARENT)!=0;
}
inline void CD3DSubMesh::SetTransparent(bool Enable)
{
	if(Enable)
		m_Property=m_Property|SMF_IS_TRANSPARENT;
	else
		m_Property=m_Property&(~SMF_IS_TRANSPARENT);
}
inline bool CD3DSubMesh::IsCulled()
{
	return (m_Flag&SMF_IS_CULLED)!=0;
}
inline void CD3DSubMesh::SetCulled(bool Enable)
{
	if(Enable)
		m_Flag=m_Flag|SMF_IS_CULLED;
	else
		m_Flag=m_Flag&(~SMF_IS_CULLED);
}

inline bool CD3DSubMesh::IsSelected()
{
	return (m_Flag&SMF_IS_SELECTED)!=0;
}

inline void CD3DSubMesh::SetSelected(bool Enable)
{
	if(Enable)
		m_Flag=m_Flag|SMF_IS_SELECTED;
	else
		m_Flag=m_Flag&(~SMF_IS_SELECTED);
}
inline bool CD3DSubMesh::IsVisible()
{
	return (m_Property&SMF_IS_VISIBLE)!=0;
}
inline void CD3DSubMesh::SetVisible(bool Enable)
{
	if(Enable)
		m_Property=m_Property|SMF_IS_VISIBLE;
	else
		m_Property=m_Property&(~SMF_IS_VISIBLE);
}

inline UINT CD3DSubMesh::GetRenderBufferUsed()
{
	return m_RenderBufferUsed;
}
inline void CD3DSubMesh::SetRenderBufferUsed(UINT Which)
{
	m_RenderBufferUsed=Which;
}
inline UINT CD3DSubMesh::GetOrginDataBufferUsed()
{
	return m_OrginDataBufferUsed;
}
inline void CD3DSubMesh::SetOrginDataBufferUsed(UINT Which)
{
	m_OrginDataBufferUsed=Which;
}

inline BYTE * CD3DSubMesh::GetVertexBufferR()
{
	if(CD3DDevice::IsUseMultiThreadRender()&&m_pVertexBufferR)
		return m_pVertexBufferR;
	else
		return m_pVertexBuffer;
}
inline BYTE * CD3DSubMesh::GetIndexBufferR()
{
	if(CD3DDevice::IsUseMultiThreadRender()&&m_pIndexBufferR)
		return m_pIndexBufferR;
	else
		return m_pIndexBuffer;
}

inline UINT CD3DSubMesh::GetPrimitiveCountR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_PrimitiveCountR;
	else
		return m_PrimitiveCount;
}
inline UINT CD3DSubMesh::GetVertexCountR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_VertexCountR;
	else
		return m_VertexCount;
}
inline UINT CD3DSubMesh::GetVertexStartR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_StartVertexR;
	else
		return m_StartVertex;
}
inline UINT CD3DSubMesh::GetIndexCountR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_IndexCountR;
	else
		return m_IndexCount;
}
inline UINT CD3DSubMesh::GetIndexStartR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_StartIndexR;
	else
		return m_StartIndex;
}

}