/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DObject.cpp                                            */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"


namespace D3DLib{

D3DMATERIAL9 CD3DObject::SELECTED_SUBMESH_MATERIAL=
{
	{1.0f,0.0f,0.0f,1.0f},
	{1.0f,0.0f,0.0f,1.0f},
	{1.0f,0.0f,0.0f,1.0f},
	{1.0f,0.0f,0.0f,1.0f},
	40.0f
};

UINT CD3DObject::m_UpdateCount=0;
UINT CD3DObject::m_RenderDataUpdateCount=0;

IMPLEMENT_CLASS_INFO(CD3DObject,CTreeObject);

CD3DObject::CD3DObject():CTreeObject()
{
	m_pD3DDevice=NULL;
	m_pRender=NULL;		
	m_pParent=NULL;
	m_LocalMatrix.SetIdentity();
	m_WorldMatrix.SetIdentity();
	m_IsMatrixChanged=true;
	m_IsRenderDataChanged=true;
	m_Flag=OBJECT_FLAG_VISIBLE;		
	m_pBoundingFrame=NULL;
}

CD3DObject::~CD3DObject(void)
{		
	Destory();
}

void CD3DObject::Destory()
{	
	if(m_pRender)
		m_pRender->DelObject(this);
	if(m_pRender)
		m_pRender->DelRootObject(this);

	m_pRender=NULL;

	CTreeObject::Destory();	

	

	//SAFE_RELEASE(m_pBoundingFrame);
	m_pBoundingFrame=NULL;

	
	
}

bool CD3DObject::Reset()
{
	for(UINT i=0;i<GetChildCount();i++)
		GetChildByIndex(i)->Reset();
	return true;
}

bool CD3DObject::Restore()
{
	for(UINT i=0;i<GetChildCount();i++)
		GetChildByIndex(i)->Restore();
	return true;
}

void CD3DObject::SetRender(CD3DBaseRender * pRender)
{
	m_pRender=pRender;
	if(m_pRender)
		SetDevice(m_pRender->GetDevice());
}

void CD3DObject::SetParent(CTreeObject* pParent)
{
	if(m_pParent==NULL)		
	{
		if(m_pRender)
			m_pRender->DelRootObject(this);
	}
	CTreeObject::SetParent(pParent);
	if(m_pParent==NULL)
	{
		if(m_pRender)
			m_pRender->AddRootObject(this);
	}
	m_IsMatrixChanged=true;
}

const CD3DMatrix CD3DObject::GetWorldMatrixDirect()
{
	if(GetParent())
	{
		return m_LocalMatrix*GetParent()->GetWorldMatrixDirect();
	}
	else
	{
		return m_LocalMatrix;
	}
}


int CD3DObject::GetSubMeshCount()
{
	return 0;
}

CD3DSubMesh *  CD3DObject::GetSubMesh(UINT index)
{
	return NULL;
}

CD3DSubMeshMaterial * CD3DObject::GetSubMeshMaterial(UINT index)
{
	CD3DSubMesh * pSubMesh=GetSubMesh(index);
	if(pSubMesh)
	{
		return &(pSubMesh->GetMaterial());
	}
	return NULL;
}

CD3DBoundingBox * CD3DObject::GetBoundingBox()
{
	return NULL;
}

CD3DBoundingSphere * CD3DObject::GetBoundingSphere()
{
	return NULL;
}

CD3DBoundingBox CD3DObject::GetWorldBoundingBox()
{
	CD3DBoundingBox BBox;
	if(GetSubMeshCount())
	{
		BBox=GetSubMesh(0)->GetBoundingBoxWithTranform(GetWorldMatrix());
		for(int i=0;i<GetSubMeshCount();i++)
		{
			GetSubMesh(i)->AppendBoundingBoxWithTranform(BBox,GetWorldMatrix());			
		}
	}
	return BBox;
}


CD3DLight * CD3DObject::GetLight(int Index)
{
	return NULL;
}

CD3DLight ** CD3DObject::GetLights()
{
	return NULL;
}

void CD3DObject::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{	
	//设置变换矩阵
	if(pCamera)
		pCamera->Apply(pRender->GetDevice(),D3DCAMERA_APPLY_ALL);
	pRender->GetDevice()->GetD3DDevice()->SetTransform(D3DTS_WORLD,&GetWorldMatrixR());
}

void CD3DObject::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	//设置材质

	//如果是被选中的使用特殊材质
	if(pSubMesh->IsSelected())
		pRender->GetDevice()->GetD3DDevice()->SetMaterial(&SELECTED_SUBMESH_MATERIAL);
	else
		pRender->GetDevice()->GetD3DDevice()->SetMaterial(&(pMaterial->GetMaterial()));

	//设置纹理
	if(pMaterial->GetFX())
	{			
		pMaterial->GetFX()->SetTexture("TexLay0",pMaterial->GetTexture(0));
	}	
}

void CD3DObject::OnPrepareRenderData()
{
	if(m_IsRenderDataChanged)
	{
		CAutoLock Lock(GetRenderLock());

		m_WorldMatrixR=m_WorldMatrix;
		for(int i=0;i<GetSubMeshCount();i++)
		{
			CD3DSubMesh * pSubMesh=GetSubMesh(i);
			if(pSubMesh)
			{
				pSubMesh->OnPrepareRenderData();
			}
		}		
		m_RenderDataUpdateCount++;
	}

	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		((CD3DObject *)m_ChildList[i])->OnPrepareRenderData();
	}

	m_IsRenderDataChanged=false;
}


void CD3DObject::Update(FLOAT Time)
{
	//更新世界矩阵
	m_UpdateCount++;
	if(GetParent())
	{
		if(m_IsMatrixChanged||GetParent()->IsMatrixChanged())
		{
			m_WorldMatrix=m_LocalMatrix*GetParent()->GetWorldMatrix();
			m_IsRenderDataChanged=true;
		}
	}
	else
	{
		if(m_IsMatrixChanged)
		{
			m_WorldMatrix=m_LocalMatrix;
			m_IsRenderDataChanged=true;
		}
	}


	//更新子对象
	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		((CD3DObject *)m_ChildList[i])->Update(Time);
	}

	m_IsMatrixChanged=false;
}

bool CD3DObject::NeedUpdateAni()
{
	return false;
}

void CD3DObject::SetVisible(bool IsVisible)
{
	if(IsVisible)
	{
		m_Flag|=OBJECT_FLAG_VISIBLE;
	}
	else
	{
		m_Flag&=~OBJECT_FLAG_VISIBLE;
	}
	
}

void CD3DObject::SetVisibleRecursive(bool IsVisible)
{
	SetVisible(IsVisible);
	for(UINT i=0;i<GetChildCount();i++)
	{
		GetChildByIndex(i)->SetVisibleRecursive(IsVisible);
	}
}

bool CD3DObject::IsVisible()
{
	return (m_Flag&OBJECT_FLAG_VISIBLE)!=0;
}

bool CD3DObject::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly)
{
	if(GetBoundingBox())
	{
		CD3DBoundingBox BBox;


		BBox=(*GetBoundingBox())*GetWorldMatrix();

		if(!BBox.RayIntersect(Point,Dir,IntersectPoint,Distance,true))
			return false;

		
	}

	bool IsIntersect=false;

	Distance=3.4E+38f;
	FLOAT Dis,DotValue;
	CD3DVector3 InterPoint;

	CD3DMatrix Mat=GetWorldMatrix().GetInverse();
	CD3DVector3 RayPos=Point*Mat;
	CD3DVector3 RayDir=Dir*Mat.GetRotation();

	RayDir.Normalize();

	for(int i=0;i<GetSubMeshCount();i++)
	{
		CD3DSubMesh * pSubMesh=GetSubMesh(i);
		if(pSubMesh)
		{
			if(pSubMesh->RayIntersect(RayPos,RayDir,InterPoint,Dis,DotValue,TestOnly))
			{
				IsIntersect=true;
				if(Dis<Distance)
				{
					Distance=Dis;
					IntersectPoint=InterPoint;
				}
				if(TestOnly)
				{
					break;
				}
			}
		}
	}
	if(IsIntersect)
	{		
		IntersectPoint*=GetWorldMatrix();
		Distance=(IntersectPoint-Point).Length();
	}
	return IsIntersect;
}

bool CD3DObject::LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance)
{
	CD3DVector3 P;
	FLOAT D;

	if(GetBoundingBox())
	{
		if(!GetBoundingBox()->LineIntersect(StartPoint,EndPoint,P,D))
			return false;
	}

	CD3DVector3 Dir=(EndPoint-StartPoint);
	FLOAT Len=Dir.Length();
	Dir.Normalize();


	if(RayIntersect(StartPoint,Dir,P,D,false))
	{
		if(D<=Len)
		{
			IntersectPoint=P;
			Distance=D;
			return true;
		}
	}
	return false;
}

CD3DObject * CD3DObject::PickObject(CD3DVector3 Point,CD3DVector3 Dir,FLOAT& Distance)
{
	Distance=MAX_HEIGHT;
	CD3DObject * pObject=NULL;
	CD3DVector3 IntersectPoint;
	FLOAT ObjDis;

	
	if(RayIntersect(Point,Dir,IntersectPoint,ObjDis,false))
	{
		Distance=ObjDis;
		pObject=this;
	}

	for(UINT i=0;i<GetChildCount();i++)
	{
		if((!GetChildByIndex(i)->IsVisible())||(GetChildByIndex(i)->CheckFlag(CD3DObject::OBJECT_FLAG_CULLED)))		
			continue;

		CD3DObject * pObj=GetChildByIndex(i)->PickObject(Point,Dir,ObjDis);		
		if(pObj&&ObjDis<Distance)
		{
			Distance=ObjDis;
			pObject=pObj;
		}
		
	}
	
	return pObject;
}

void CD3DObject::ShowBoundingFrame(int Operator)
{
	switch(Operator)
	{
	case DBFO_HIDE:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(false);
		}
		break;
	case DBFO_SHOW:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(true);
			else
			{
				if(GetRender()==NULL)
					return;
				if(GetBoundingBox()==NULL)
					return;
				m_pBoundingFrame=new CD3DBoundingFrame();
				m_pBoundingFrame->SetRender(GetRender());
				m_pBoundingFrame->CreateFromBBox(*GetBoundingBox());
				m_pBoundingFrame->SetParent(this);
				GetRender()->AddObject(m_pBoundingFrame);
			}
		}
		break;
	case DBFO_RELEASE:
		SAFE_RELEASE(m_pBoundingFrame);
		break;
	}
}

void CD3DObject::UpdateBoundingFrame()
{
	if(m_pBoundingFrame)
	{
		m_pBoundingFrame->CreateFromBBox(*GetBoundingBox());
	}
}


bool CD3DObject::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DObject)))
		return false;

	if(!CTreeObject::CloneFrom(pObject,Param))
		return false;	

	CD3DObject * pSource=(CD3DObject *)pObject;

	m_pRender=pSource->m_pRender;		
	m_LocalMatrix=pSource->m_LocalMatrix;
	m_WorldMatrix=pSource->m_WorldMatrix;		
	m_Flag=pSource->m_Flag;		
	
	return true;
}

void CD3DObject::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	for(int i=0;i<GetSubMeshCount();i++)
	{
		CD3DSubMesh * pSubMesh=GetSubMesh(i);
		if(pSubMesh)
		{
			pSubMesh->PickResource(pResourceManager,Param);
		}
	}
	for(UINT i=0;i<GetChildCount();i++)
	{
		GetChildByIndex(i)->PickResource(pResourceManager,Param);
	}
}

bool CD3DObject::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CTreeObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;	

	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DO_LOCAL_MATIRX,(char *)&m_LocalMatrix,sizeof(m_LocalMatrix)));	
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DO_IS_FLAG,m_Flag));	

	return true;
}

bool CD3DObject::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CTreeObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_D3DO_LOCAL_MATIRX:
			memcpy(&m_LocalMatrix,(LPCSTR)Value,sizeof(m_LocalMatrix));
			break;
		case SST_D3DO_IS_FLAG:
			m_Flag=Value;
			break;
		}
	}
	return true;
}

UINT CD3DObject::GetSmartStructSize(UINT Param)
{
	SAFE_RELEASE(m_pBoundingFrame);

	UINT Size=CTreeObject::GetSmartStructSize(Param);
	Size+=SMART_STRUCT_STRING_MEMBER_SIZE(sizeof(m_LocalMatrix));
	Size+=SMART_STRUCT_FIX_MEMBER_SIZE(sizeof(m_Flag));
	return Size;
}



}