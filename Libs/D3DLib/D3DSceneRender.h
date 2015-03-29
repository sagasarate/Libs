/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSceneRender.h                                         */
/*      创建日期:  2010年07月19日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DBoundingFrame;
class CD3DWOWWMOModel;

class CD3DSceneRender :
	public CD3DBaseRender
{
public:
	enum RENDER_MODE
	{
		RENDER_MODE_NORMAL,
		RENDER_MODE_DEPTH,
	};

	enum OCTREE_CHILD
	{
		OTC_FRONT_LEFT_BOTTOM,
		OTC_FRONT_RIGHT_BOTTOM,
		OTC_BACK_LEFT_BOTTOM,
		OTC_BACK_RIGHT_BOTTOM,
		OTC_FRONT_LEFT_TOP,
		OTC_FRONT_RIGHT_TOP,
		OTC_BACK_LEFT_TOP,
		OTC_BACK_RIGHT_TOP,
		OTC_MAX,
	};	

	enum QDTREE_CHILD
	{
		QTC_FRONT_LEFT,
		QTC_FRONT_RIGHT,
		QTC_BACK_LEFT,
		QTC_BACK_RIGHT,		
		QTC_MAX,
	};

	enum eMINI_CUT_COUNT
	{
		MINI_CUT_COUNT=2,
	};

	enum RENDER_OBJECT_TYPE
	{
		RBT_OBJECT,
		RBT_SUBMESH,
	};
	
	struct RENDER_SUBMESH_INFO
	{			
		CD3DObject *			pObj;
		CD3DSubMesh *			pSubMesh;
		CD3DSubMeshMaterial *	pMaterial;
	};

	struct TREE_NODE;

	struct RENDER_OBJECT_INFO
	{
		BYTE					ObjectType;
		CD3DBoundingBox			WorldBBox;
		CD3DObject *			pObject;
		CD3DSubMesh *			pSubMesh;
		CD3DSubMeshMaterial *	pMaterial;
		bool					IsPreRenderd;
		bool					IsRenderd;
		bool					MergeRender;
		CEasyArray<TREE_NODE *>	TreeNodes;

	};

	enum TREE_NODE_UPDATE_FLAG
	{
		TNUF_NONE,
		TNUF_ADD_OBJECT,
		TNUF_DEL_OBJECT,
	};
	
	struct TREE_NODE
	{
		CEasyArray<RENDER_OBJECT_INFO *>	ObjectList;
		TREE_NODE *							Childs[OTC_MAX];
		TREE_NODE *							pParent;
		UINT								Depth;
		CD3DBoundingBox						NodeBox;
		UINT								ChildCount;
		CD3DBoundingFrame *					pBoundingFrame;
		bool								IsRendered;
		bool								IsFixed;
		int									UpdateFlag;
		TREE_NODE()
		{
			ZeroMemory(Childs,sizeof(Childs));
			pParent=NULL;
			Depth=0;
			ChildCount=0;
			pBoundingFrame=NULL;
			IsRendered=false;			
			IsFixed=false;
			UpdateFlag=TNUF_NONE;
		}
		bool AddObj(RENDER_OBJECT_INFO * pObj)
		{
			ObjectList.Add(pObj);
			pObj->TreeNodes.Add(this);
			UpdateFlag=TNUF_ADD_OBJECT;
			return true;
		}
		bool DelObj(RENDER_OBJECT_INFO * pObj)
		{
			for(UINT i=0;i<ObjectList.GetCount();i++)
			{
				if(ObjectList[i]==pObj)
				{				
					return DelObj(i);
				}
			}
			return false;
		}
		bool DelObj(UINT Index)
		{
			if(Index<ObjectList.GetCount())
			{
				RENDER_OBJECT_INFO * pObjectInfo=ObjectList[Index];
				ObjectList.Delete(Index);
				for(UINT i=0;i<pObjectInfo->TreeNodes.GetCount();i++)
				{
					if(pObjectInfo->TreeNodes[i]==this)
					{
						pObjectInfo->TreeNodes.Delete(i);
						UpdateFlag=TNUF_DEL_OBJECT;
						return true;
					}
				}
				PrintD3DLog(0,_T("未能删除树节点记录"));
				UpdateFlag=TNUF_DEL_OBJECT;
				return true;
			}
			return false;
		}
	};	

	struct BBOX_RANGE_INFO
	{
		float	Weight;
		UINT	MinCount;
		UINT	MaxCount;
		float	Min;
		float	Max;
	};

	
protected:

	CD3DCamera *							m_pCamera;
	CD3DCamera *							m_pBackGroundCamera;
	CD3DFX *								m_pSharedParamFX;
	CD3DLight *								m_pGlobalLight;
	CEasyArray<CD3DLight *>					m_LightList;

	D3DCOLOR								m_FogColor;
	FLOAT									m_FogNear;
	FLOAT									m_FogFar;	

	D3DCOLOR								m_WaterColorLight;
	D3DCOLOR								m_WaterColorDark;

	D3DCOLOR								m_ShadowColor;

	CEasyArray<CD3DObject *>				m_ObjectList;
	CEasyArray<CD3DObject *>				m_BackGroundObjectList;
	CEasyArray<CD3DWOWWMOModel *>			m_WMOObjectList;

	UINT									m_TreeMode;
	TREE_NODE *								m_pSceneRoot;
	TREE_NODE *								m_pSelectedSceneTree;
	CEasyArray<RENDER_OBJECT_INFO *>		m_SceneRenderUnitList;
	CEasyArray<CD3DObject *>				m_SceneObjectList;
	UINT									m_TreeDepthLimit;
	UINT									m_MaxTreeDepth;

	CEasyArray<RENDER_SUBMESH_INFO>			m_RenderBatchList;
	CEasyArray<RENDER_SUBMESH_INFO>			m_TransparentSubMeshList;
	CD3DTexture *							m_pDepthTexture;
	
	UINT									m_MergeRenderSubMeshCount;
	UINT									m_FXApplyCount;
	UINT									m_MaterialSetCount;
	UINT									m_TreeCheckCount;
	UINT									m_ObjectCheckCount;
	UINT									m_DirectRenderCount;
	UINT									m_BranchObjectCheckCount;
	UINT									m_TreeCutCount;
	UINT									m_ObjectCutCount;
	UINT									m_HeightTestCount;
	UINT									m_RayIntersectCount;
	UINT									m_LineIntersectCount;

	UINT									m_AddObjectSubMeshCount;
	UINT									m_AddObjectTreeWalkCount;
	UINT									m_AddObjectTreeAddCount;
	UINT									m_AddObjectTreeCutCount;
	UINT									m_AddObjectTreeUpdateCount;
	UINT									m_AddObjectObjectTestCount;
	

	bool									m_ShowNodeFrame;

	float									m_SizeCullSize;

	DECLARE_CLASS_INFO(CD3DSceneRender)
public:
public:
	enum TREE_MODE
	{
		TREE_MODE_QD=QTC_MAX,
		TREE_MODE_OC=OTC_MAX,
	};


	CD3DSceneRender(void);
	~CD3DSceneRender(void);



	bool Create(CD3DDevice * pDevice,UINT TreeMode,CD3DBoundingBox SceneBox,int StartDepth,int DepthLimit);
	virtual void Destory();

	virtual bool Reset();
	virtual bool Restore();	


	virtual bool AddObject(CD3DObject * pObj,bool IsRecursive=true);	
	virtual bool DelObject(CD3DObject * pObj,bool IsRecursive=true);


	bool AddBackGroundObject(CD3DObject * pObj,bool IsRecursive=true);
	
	bool AddSceneObject(CD3DObject * pObject,bool IsRecursive=true);

	void SetGlobalLight(CD3DLight * pLight);
	CD3DLight * GetGlobalLight();

	void AddLight(CD3DLight * pLight);
	bool DeleteLight(UINT Index);
	bool DeleteLight(CD3DLight * pLight);
	bool DeleteLight(LPCTSTR LightName);
	
	virtual void RemoveAllObject();

	//void UpdateSceneTree();

	virtual void SetCamera(CD3DCamera * pCamera);
	virtual CD3DCamera * GetCamera();

	void SetBackGroundCamera(CD3DCamera * pCamera);
	CD3DCamera * GetBackGroundCamera();

	virtual UINT GetLightCount();
	virtual CD3DLight * GetLight(UINT Index);

	D3DCOLOR GetFogColor();
	FLOAT GetFogNear();
	FLOAT GetFogFar();

	void SetFog(D3DCOLOR FogColor,FLOAT FogNear,FLOAT FogFar);
	void SetFogColor(D3DCOLOR Color);


	D3DCOLOR GetWaterColorLight();
	D3DCOLOR GetWaterColorDark();

	void SetWaterColorLight(D3DCOLOR Color);
	void SetWaterColorDark(D3DCOLOR Color);
	
	D3DCOLOR GetShadowColor();
	void SetShadowColor(D3DCOLOR Color);

	virtual void Render();
	virtual void Update(FLOAT Time);

	bool CreateDepthTexture();
	bool ClearDepthTexture();

	virtual CD3DTexture * GetDepthTexture();

	void SetSizeCullSize(float MiniSize);

	UINT GetMergeSubMeshRenderCount();
	UINT GetFXApplyCount();
	UINT GetMaterialSetCount();
	UINT GetTreeCheckCount();
	UINT GetObjectCheckCount();
	UINT GetDirectRenderCount();
	UINT GetBranchObjectCheckCount();
	UINT GetTreeCutCount();
	UINT GetObjectCutCount();

	void ShowNodeFrame(bool IsShow);
	void ShowNodeFrame(TREE_NODE * pTree,bool IsShow);

	TREE_NODE * GetSceneRoot();
	void SetSelectTree(TREE_NODE * pTree);
	UINT GetSceneTreeMode();

	void HideCutObject(TREE_NODE * pTree);

	void FindTreeByObject(TREE_NODE * pRoot,CD3DObject * pObject,CEasyArray<TREE_NODE *>& TreeList);
	void FindTreeBySubMesh(TREE_NODE * pRoot,CD3DObject * pObject,CD3DSubMesh * pSubMesh,CEasyArray<TREE_NODE *>& TreeList);
	TREE_NODE * FindTreeByBBox(TREE_NODE * pRoot,const CD3DBoundingBox& BBox);

	bool GetHeightByXZ(const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight);
	bool RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode);
	bool LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode);

protected:

	void BuildSceneTree(TREE_NODE * pRoot,int StartDepth);

	bool AddWMOObject(CD3DWOWWMOModel * pWMOObject);
	bool DelWMOObject(CD3DWOWWMOModel * pWMOObject);

	
	
	bool DeleteSceneObject(CD3DObject * pObject);
	//int DeleteObjectFromTree(TREE_NODE * pNode,RENDER_OBJECT_INFO * pObjectInfo);
	void DeleteSceneNode(TREE_NODE * pNode);
	//void DeleteNodeChilds(TREE_NODE * pNode,TREE_NODE * pParent);
	bool AddObjectToTree(TREE_NODE * pTree,CD3DObject * pObject,bool BeWalkTree);
	void AddObjectToTree(TREE_NODE * pTree,RENDER_OBJECT_INFO * pObjectInfo);
	void UpdateTree(TREE_NODE * pNode);
	void CreateChilds(TREE_NODE * pNode,CD3DVector3 CutCenter,bool IsFixed);	
	void CutBBox(TREE_NODE * pNode,CD3DVector3 CutCenter,CD3DBoundingBox * ChildBBoxs);
	
	bool CanBeCut(TREE_NODE * pNode,CD3DVector3 CutCenter);
	void RenderObject(CD3DObject * pObject,const CD3DFrustum& Frustum,bool NeedFrustumCheck);
	void RenderSubMesh(RENDER_OBJECT_INFO * pObjectInfo,const CD3DFrustum& Frustum,bool NeedFrustumCheck);
	void RenderScene(TREE_NODE * pNode,const CD3DFrustum& Frustum,bool NeedCheck);
	void DoBatchRender();

	void ClearTreeRenderFlag(TREE_NODE * pNode);

	int CheckBBoxRelation(const CD3DBoundingBox& BBox1,const CD3DBoundingBox& BBox2);
	CD3DVector3 GetBoxCutCenter(TREE_NODE * pNode);
	bool HeightTest(TREE_NODE * pRoot,const CD3DVector3 Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight);
	bool RayIntersect(TREE_NODE * pRoot,const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode);
	bool LineIntersect(TREE_NODE * pRoot,const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,CD3DObject ** ppIntersectObject,UINT TestMode);
	
	static int SubMeshCompare(const void * s1,const void * s2);

	void RenderSubMeshEx(CD3DSubMesh * pSubMesh);

	bool CanTestCollide(UINT64 SubMeshProperty,UINT TestMode);

	void SetSubMeshMaterialFXParams(CD3DSubMeshMaterial * pMaterial);
};


inline CD3DLight * CD3DSceneRender::GetGlobalLight()
{
	return m_pGlobalLight;
}

inline CD3DCamera * CD3DSceneRender::GetCamera()
{
	return m_pCamera;
}

inline CD3DCamera * CD3DSceneRender::GetBackGroundCamera()
{
	return m_pBackGroundCamera;
}

inline UINT CD3DSceneRender::GetLightCount()
{
	return m_LightList.GetCount();
}

inline CD3DLight * CD3DSceneRender::GetLight(UINT Index)
{
	if(Index<m_LightList.GetCount())
	{
		return m_LightList[Index];
	}
	return NULL;
}


inline D3DCOLOR CD3DSceneRender::GetFogColor()
{
	return m_FogColor;
}
inline FLOAT CD3DSceneRender::GetFogNear()
{
	return m_FogNear;
}
inline FLOAT CD3DSceneRender::GetFogFar()
{
	return m_FogFar;
}

inline void CD3DSceneRender::SetFog(D3DCOLOR FogColor,FLOAT FogNear,FLOAT FogFar)
{
	m_FogColor=FogColor;
	m_FogNear=FogNear;
	m_FogFar=FogFar;
}

inline void CD3DSceneRender::SetFogColor(D3DCOLOR Color)
{
	m_FogColor=Color;
}


inline D3DCOLOR CD3DSceneRender::GetWaterColorLight()
{
	return m_WaterColorLight;
}
inline D3DCOLOR CD3DSceneRender::GetWaterColorDark()
{
	return m_WaterColorDark;
}

inline void CD3DSceneRender::SetWaterColorLight(D3DCOLOR Color)
{
	m_WaterColorLight=Color;
}
inline void CD3DSceneRender::SetWaterColorDark(D3DCOLOR Color)
{
	m_WaterColorDark=Color;
}

inline D3DCOLOR CD3DSceneRender::GetShadowColor()
{
	return m_ShadowColor;
}

inline void CD3DSceneRender::SetShadowColor(D3DCOLOR Color)
{
	m_ShadowColor=Color;
}

inline CD3DTexture * CD3DSceneRender::GetDepthTexture()
{
	return m_pDepthTexture;
}

inline void CD3DSceneRender::SetSizeCullSize(float MiniSize)
{
	m_SizeCullSize=MiniSize;
}

inline UINT CD3DSceneRender::GetMergeSubMeshRenderCount()
{
	return m_MergeRenderSubMeshCount;
}



inline UINT CD3DSceneRender::GetFXApplyCount()
{
	return m_FXApplyCount;
}
inline UINT CD3DSceneRender::GetMaterialSetCount()
{
	return m_MaterialSetCount;
}
inline UINT CD3DSceneRender::GetTreeCheckCount()
{
	return m_TreeCheckCount;
}
inline UINT CD3DSceneRender::GetObjectCheckCount()
{
	return m_ObjectCheckCount;
}
inline UINT CD3DSceneRender::GetDirectRenderCount()
{
	return m_DirectRenderCount;
}
inline UINT CD3DSceneRender::GetBranchObjectCheckCount()
{
	return m_BranchObjectCheckCount;
}
inline UINT CD3DSceneRender::GetTreeCutCount()
{
	return m_TreeCutCount;
}
inline UINT CD3DSceneRender::GetObjectCutCount()
{
	return m_ObjectCutCount;
}

inline void CD3DSceneRender::ShowNodeFrame(bool IsShow)
{
	m_ShowNodeFrame=IsShow;
}

inline CD3DSceneRender::TREE_NODE * CD3DSceneRender::GetSceneRoot()
{
	return m_pSceneRoot;
}

inline UINT CD3DSceneRender::GetSceneTreeMode()
{
	return m_TreeMode;
}

}