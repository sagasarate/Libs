/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DWOWWMOGroupModel.cpp	                                */
/*      创建日期:  2010年08月04日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DWOWWMOGroupModel,CD3DBaseStaticModel);

CD3DWOWWMOGroupModel::CD3DWOWWMOGroupModel(void)
{
	m_pModelResource=NULL;
	m_pGroupInfo=NULL;
	m_GroupIndex=0;

	//m_IsSelected=false;
	//m_pHeightTestLine=NULL;

}

CD3DWOWWMOGroupModel::~CD3DWOWWMOGroupModel(void)
{
	Destory();
}

void CD3DWOWWMOGroupModel::Destory()
{
	CD3DBaseStaticModel::Destory();

	//m_pHeightTestLine=NULL;
	SAFE_RELEASE(m_pModelResource);
	m_pGroupInfo=NULL;
	m_GroupIndex=0;
	
}

bool CD3DWOWWMOGroupModel::LoadFromResource(CD3DWOWWMOModelResource * pModelResource,UINT GroupIndex)
{
	Destory();


	m_pGroupInfo=pModelResource->GetGroupInfo(GroupIndex);
	if(m_pGroupInfo)
	{
		m_pModelResource=pModelResource;
		m_pModelResource->AddUseRef();
		m_GroupIndex=GroupIndex;
		SetName(m_pGroupInfo->Name);
		//CreatePortalBoard();
		//CreateBSPBoard();

		m_IsRenderDataChanged=true;
		return true;
	}
	return false;
}

void CD3DWOWWMOGroupModel::OnPrepareRender(CD3DBaseRender * pRender,CD3DFX * pFX,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	

	CD3DMatrix Mat=GetWorldMatrixR()*pCamera->GetViewMatR();

	pFX->SetMatrix("WorldMatrix",GetWorldMatrixR());
	pFX->SetMatrix("WorldViewMatrix",Mat);
	
}
void CD3DWOWWMOGroupModel::OnPrepareRenderSubMesh(CD3DBaseRender * pRender,CD3DFX * pFX,CD3DSubMesh * pSubMesh,CD3DSubMeshMaterial * pMaterial,CEasyArray<CD3DLight *>& LightList,CD3DCamera * pCamera)
{
	////设置材质
	//D3DMATERIAL9 * pD3DMaterial;
	//if(pSubMesh->IsSelected())
	//	pD3DMaterial=&SELECTED_SUBMESH_MATERIAL;
	//else
	//	pD3DMaterial=&(pMaterial->GetMaterial());
	//pMaterial->GetFX()->SetColor("MaterialAmbient",pD3DMaterial->Ambient);
	//pMaterial->GetFX()->SetColor("MaterialDiffuse",pD3DMaterial->Diffuse);
	//pMaterial->GetFX()->SetColor("MaterialSpecular",pD3DMaterial->Specular);
	//pMaterial->GetFX()->SetColor("MaterialEmissive",pD3DMaterial->Emissive);
	//pMaterial->GetFX()->SetFloat("MaterialPower",pD3DMaterial->Power);

	//设置纹理
	//pFX->SetTexture("TexLay0",pMaterial->GetTexture(0));
	//if(pSubMesh->IsSelected())
	//{
	//	pFX->SetColor("GlobalColor",0xFFFF0000);
	//}
	//else
	//{
	//	pFX->SetColor("GlobalColor",0xFFFFFFFF);
	//}

	if(pSubMesh->IsSelected())			
	{
		pFX->SetColor("GlobalColor",0xFFFF0000);
	}
}


int CD3DWOWWMOGroupModel::GetSubMeshCount()
{
	if(m_pGroupInfo)
	{
		return m_pGroupInfo->GroupSubMeshList.GetCount();
	}
	return 0;
}

CD3DSubMesh * CD3DWOWWMOGroupModel::GetSubMesh(UINT index)
{
	if(m_pGroupInfo)
	{
		if(index<m_pGroupInfo->GroupSubMeshList.GetCount())
		{
			return m_pGroupInfo->GroupSubMeshList[index];
		}
	}
	return NULL;
}

CD3DSubMeshMaterial * CD3DWOWWMOGroupModel::GetSubMeshMaterial(UINT index)
{
	if(m_pGroupInfo)
	{
		if(index<m_pGroupInfo->GroupSubMeshList.GetCount())
		{
			return &(m_pGroupInfo->GroupSubMeshList[index]->GetMaterial());
		}
	}
	return NULL;
}

CD3DBoundingBox * CD3DWOWWMOGroupModel::GetBoundingBox()
{
	if(m_pGroupInfo)
	{		
		return &(m_pGroupInfo->BoundingBox);
	}
	return NULL;
}

CD3DBoundingSphere * CD3DWOWWMOGroupModel::GetBoundingSphere()
{
	if(m_pModelResource)
	{
		m_pModelResource->GetBoundingSphere();
	}
	return NULL;
}
bool CD3DWOWWMOGroupModel::GetHeightByXZ(const CD3DVector3& Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight)
{	
	CD3DVector3 P1=Pos;	
	P1.y=MaxHeight;
	CD3DVector3 Dir(0,-1,0);
	

	CD3DVector3 IntersectPoint;
	FLOAT Distance,DotValue;

	if(RayIntersect(P1,Dir,IntersectPoint,Distance,DotValue,RITM_INCLUDE_COLLIDE_ONLY_FACE,false))
	{
		if(IntersectPoint.y>=MinHeight&&MinHeight<=MaxHeight)
		{
			Height=IntersectPoint.y;
			WaterHeight=Height;
			return true;
		}		
	}
	return false;	
}

bool CD3DWOWWMOGroupModel::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly)
{
	FLOAT DotValue;
	return RayIntersect(Point,Dir,IntersectPoint,Distance,DotValue,0,TestOnly);
}

bool CD3DWOWWMOGroupModel::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,UINT TestMode,bool TestOnly)
{
	if(GetBoundingBox())
	{
		

		bool IsIntersect=false;
		FLOAT FinalDis;
		CD3DVector3 P;
		FLOAT D=0;

		CD3DMatrix Mat=GetWorldMatrix().GetInverse();
		CD3DVector3 RayPos=Point*Mat;
		CD3DVector3 RayDir=Dir*Mat.GetRotation();
		RayDir.Normalize();
	
		CD3DBoundingBox WMOBBox=*GetBoundingBox();

		if(WMOBBox.RayIntersect(RayPos,RayDir,P,D,true))
		{
			CD3DWOWWMOModelResource::BSP_NODE * pRoot=m_pGroupInfo->BSPTree.GetObject(0);
			if(pRoot)
			{				
				//WalkBSPNode(pRoot,WMOBBox);

				CEasyArray<CD3DWOWWMOModelResource::BSP_NODE *> NodeList;
				FindBSPNode(pRoot,WMOBBox,RayPos,RayDir,NodeList);
				for(UINT j=0;j<NodeList.GetCount();j++)
				{
					for(UINT i=0;i<NodeList[j]->FaceCount;i++)
					{
						WORD Face=m_pGroupInfo->BSPFaceList[i+NodeList[j]->FirstFace];
						if(CanTestCollide(m_pGroupInfo->FaceFlags[Face],TestMode))
						{
							WORD Index1=m_pGroupInfo->IndexList[Face*3];
							WORD Index2=m_pGroupInfo->IndexList[Face*3+1];						
							WORD Index3=m_pGroupInfo->IndexList[Face*3+2];
							CD3DVector3& P1=m_pGroupInfo->VertexList[Index1].Pos;
							CD3DVector3& P2=m_pGroupInfo->VertexList[Index2].Pos;
							CD3DVector3& P3=m_pGroupInfo->VertexList[Index3].Pos;
							FLOAT U,V;						
							if(D3DXIntersectTri(
								&P1,&P2,&P3,
								&RayPos,&RayDir,&U,&V,&D))
							{					
								if(IsIntersect)
								{
									if(D<FinalDis)
									{
										Distance=D;
										IntersectPoint=P1+(P2-P1)*U+(P3-P1)*V;
										DotValue=CD3DPlane::FromPoints(P1,P2,P3).GetNormalize().DotNormal(RayDir);
										FinalDis=D;
									}
								}
								else
								{
									IsIntersect=true;
									Distance=D;
									IntersectPoint=P1+(P2-P1)*U+(P3-P1)*V;
									DotValue=CD3DPlane::FromPoints(P1,P2,P3).GetNormalize().DotNormal(RayDir);
									FinalDis=D;
								}								
							}
							//m_pGroupInfo->VertexList[Index1].Diffuse=0xFFFF0000;
							//m_pGroupInfo->VertexList[Index2].Diffuse=0xFFFF0000;
							//m_pGroupInfo->VertexList[Index3].Diffuse=0xFFFF0000;
						}
					}					
				}
				if(IsIntersect)
				{
					IntersectPoint=IntersectPoint*GetWorldMatrix();
					Distance=(IntersectPoint-Point).Length();
				}
				return IsIntersect;
			}
		}
	}
	return false;
}

bool CD3DWOWWMOGroupModel::LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,UINT TestMode)
{
	CD3DVector3 P;
	FLOAT D,A;

	if(GetBoundingBox())
	{
		CD3DBoundingBox BBox=(*GetBoundingBox())*GetWorldMatrix();
		if(!BBox.LineIntersect(StartPoint,EndPoint,P,D))
			return false;
	}

	CD3DVector3 Dir=(EndPoint-StartPoint);
	FLOAT Len=Dir.Length();
	Dir.Normalize();


	if(RayIntersect(StartPoint,Dir,P,D,A,TestMode,false))
	{
		if(D<=Len)
		{
			IntersectPoint=P;
			Distance=D;
			DotValue=A;
			return true;
		}
	}
	return false;
}

void CD3DWOWWMOGroupModel::CreatePortalBoard()
{
	if(m_pGroupInfo)
	{
		for(UINT i=0;i<m_pGroupInfo->PortalList.GetCount();i++)
		{
			if(m_pGroupInfo->PortalList[i].Vertices.GetCount()>=4)
			{
				CD3DBoard * pBoard=new CD3DBoard();
				pBoard->SetDevice(GetDevice());
				if(pBoard->Create(m_pGroupInfo->PortalList[i].Vertices[0],
					m_pGroupInfo->PortalList[i].Vertices[3],
					m_pGroupInfo->PortalList[i].Vertices[1],
					m_pGroupInfo->PortalList[i].Vertices[2],
					m_pGroupInfo->PortalList[i].Normal,
					0x300000FF))
				{
					CEasyString Name;
					Name.Format(_T("%s%d"),
						GetName(),i);
					pBoard->SetParent(this);
					pBoard->SetName(Name);
					m_PortalBoards.Add(pBoard);
				}
				else
				{
					SAFE_RELEASE(pBoard);
				}
			}
		}
	}
}

void CD3DWOWWMOGroupModel::CreateBSPBoard(CD3DWOWWMOModelResource::BSP_NODE * pNode)
{
	
	CD3DBoundingBox& BBox=m_pGroupInfo->BoundingBox;
	switch(pNode->PlaneType)
	{
	case CD3DWOWWMOModelResource::BPT_XY:
		{
			pNode->pFaceBoard=new CD3DBoard();
			pNode->pFaceBoard->SetDevice(GetDevice());
			pNode->pFaceBoard->Create(
				CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,pNode->Distance),
				CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,pNode->Distance),
				CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,pNode->Distance),
				CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,pNode->Distance),
				CD3DVector3(0,0,1),D3DCOLOR_ARGB(0x80,0xff,0,0));

			pNode->pFaceBoard->SetParent(this);
			if(GetRender())
				GetRender()->AddObject(pNode->pFaceBoard);
		}
		break;
	case CD3DWOWWMOModelResource::BPT_YZ:
		{
			pNode->pFaceBoard=new CD3DBoard();
			pNode->pFaceBoard->SetDevice(GetDevice());
			pNode->pFaceBoard->Create(
				CD3DVector3(pNode->Distance,BBox.m_Min.y,BBox.m_Max.z),
				CD3DVector3(pNode->Distance,BBox.m_Max.y,BBox.m_Max.z),
				CD3DVector3(pNode->Distance,BBox.m_Min.y,BBox.m_Min.z),
				CD3DVector3(pNode->Distance,BBox.m_Max.y,BBox.m_Min.z),
				CD3DVector3(1,0,0),D3DCOLOR_ARGB(0x80,0xff,0,0));

			pNode->pFaceBoard->SetParent(this);
			if(GetRender())
				GetRender()->AddObject(pNode->pFaceBoard);
		}
		break;
	case CD3DWOWWMOModelResource::BPT_XZ:
		{
			pNode->pFaceBoard=new CD3DBoard();
			pNode->pFaceBoard->SetDevice(GetDevice());
			pNode->pFaceBoard->Create(
				CD3DVector3(BBox.m_Min.x,pNode->Distance,BBox.m_Max.z),
				CD3DVector3(BBox.m_Max.x,pNode->Distance,BBox.m_Max.z),
				CD3DVector3(BBox.m_Min.x,pNode->Distance,BBox.m_Min.z),
				CD3DVector3(BBox.m_Max.x,pNode->Distance,BBox.m_Min.z),
				CD3DVector3(0,1,0),D3DCOLOR_ARGB(0x80,0xff,0,0));

			pNode->pFaceBoard->SetParent(this);
			if(GetRender())
				GetRender()->AddObject(pNode->pFaceBoard);
		}
		break;
	case CD3DWOWWMOModelResource::BPT_LEAF:
		break;
	}
	
		
}

void CD3DWOWWMOGroupModel::CreateBSPBox(CD3DBoundingBox Box)
{
	CD3DBoundingFrame * pBox=new CD3DBoundingFrame();
	pBox->SetDevice(GetDevice());
	pBox->CreateFromBBox(Box,0xFF0000FF);
	pBox->SetParent(this);
	GetRender()->AddObject(pBox);
}

void CD3DWOWWMOGroupModel::SetBSPColor(CD3DWOWWMOModelResource::BSP_NODE * pTree,D3DCOLOR Color)
{
	if(pTree->FaceCount)
	{
		for(UINT i=0;i<pTree->FaceCount;i++)
		{
			WORD Face=m_pGroupInfo->BSPFaceList[i+pTree->FirstFace];
			WORD Index=m_pGroupInfo->IndexList[Face*3];
			m_pGroupInfo->VertexList[Index].Diffuse=Color;
			Index=m_pGroupInfo->IndexList[Face*3+1];
			m_pGroupInfo->VertexList[Index].Diffuse=Color;
			Index=m_pGroupInfo->IndexList[Face*3+2];
			m_pGroupInfo->VertexList[Index].Diffuse=Color;
		}
	}
	if(pTree->LeftChildIndex>=0)
	{
		SetBSPColor(m_pGroupInfo->BSPTree.GetObject(pTree->LeftChildIndex),Color);
	}
	if(pTree->RightChildIndex>=0)
	{
		SetBSPColor(m_pGroupInfo->BSPTree.GetObject(pTree->RightChildIndex),Color);
	}
}

CD3DWOWWMOModelResource::BSP_NODE * CD3DWOWWMOGroupModel::FindBSPNode(CD3DWOWWMOModelResource::BSP_NODE * pRoot,CD3DVector3 Pos)
{
	if(pRoot==NULL)
		return NULL;
	//CreateBSPBoard(pRoot);
	switch(pRoot->PlaneType)
	{
	case CD3DWOWWMOModelResource::BPT_XY:
		if(Pos.z<pRoot->Distance)
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->RightChildIndex),Pos);
		else
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->LeftChildIndex),Pos);
		break;
	case CD3DWOWWMOModelResource::BPT_YZ:
		if(Pos.x<pRoot->Distance)
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->RightChildIndex),Pos);
		else
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->LeftChildIndex),Pos);
		break;
	case CD3DWOWWMOModelResource::BPT_XZ:
		if(Pos.y<pRoot->Distance)
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->RightChildIndex),Pos);
		else
			return FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->LeftChildIndex),Pos);		
		break;
	case CD3DWOWWMOModelResource::BPT_LEAF:
		return pRoot;
		break;
	}
	return NULL;
}

void CD3DWOWWMOGroupModel::FindBSPNode(CD3DWOWWMOModelResource::BSP_NODE * pRoot,CD3DBoundingBox SpaceBox,CD3DVector3 Point,CD3DVector3 Dir,CEasyArray<CD3DWOWWMOModelResource::BSP_NODE *>& NodeList)
{
	if(pRoot==NULL)
		return;
	//CreateBSPBoard(pRoot);
	CD3DBoundingBox MinBox=SpaceBox;
	CD3DBoundingBox MaxBox=SpaceBox;
	CD3DVector3 P;
	FLOAT D;

	switch(pRoot->PlaneType)
	{
	case CD3DWOWWMOModelResource::BPT_XY:
		MinBox.m_Max.z=pRoot->Distance;
		MaxBox.m_Min.z=pRoot->Distance;
		break;
	case CD3DWOWWMOModelResource::BPT_YZ:
		MinBox.m_Max.x=pRoot->Distance;
		MaxBox.m_Min.x=pRoot->Distance;		
		break;
	case CD3DWOWWMOModelResource::BPT_XZ:
		MinBox.m_Max.y=pRoot->Distance;
		MaxBox.m_Min.y=pRoot->Distance;
		break;
	case CD3DWOWWMOModelResource::BPT_LEAF:
		//if(pRoot->FirstFace>=90&&pRoot->FirstFace<100)
		//	CreateBSPBox(SpaceBox);
		NodeList.Add(pRoot);
		return;
	}
	if(MinBox.RayIntersect(Point,Dir,P,D,true))
		FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->RightChildIndex),MinBox,Point,Dir,NodeList);
	if(MaxBox.RayIntersect(Point,Dir,P,D,true))
		FindBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->LeftChildIndex),MaxBox,Point,Dir,NodeList);
}

bool CD3DWOWWMOGroupModel::CanTestCollide(UINT FaceFlag,UINT TestMode)
{
	if(TestMode&RITM_CAMERA)
	{
		return ((FaceFlag&CD3DWOWWMOModelResource::FF_NO_COLLIDE)==0&&(FaceFlag&CD3DWOWWMOModelResource::FF_COLLIDE_ONLY)==0&&(FaceFlag&CD3DWOWWMOModelResource::FF_NO_CAMERA_COLLIDE)==0)||		
			((TestMode&RITM_INCLUDE_COLLIDE_ONLY_FACE)&&(FaceFlag&CD3DWOWWMOModelResource::FF_COLLIDE_ONLY)&&(FaceFlag&CD3DWOWWMOModelResource::FF_NO_CAMERA_COLLIDE)==0);
	}
	else
	{
		return ((FaceFlag&CD3DWOWWMOModelResource::FF_NO_COLLIDE)==0&&(FaceFlag&CD3DWOWWMOModelResource::FF_COLLIDE_ONLY)==0)||		
			((TestMode&RITM_INCLUDE_COLLIDE_ONLY_FACE)&&(FaceFlag&CD3DWOWWMOModelResource::FF_COLLIDE_ONLY));
	}
	

}

void CD3DWOWWMOGroupModel::WalkBSPNode(CD3DWOWWMOModelResource::BSP_NODE * pRoot,CD3DBoundingBox SpaceBox)
{
	if(pRoot==NULL)
		return;	
	CD3DBoundingBox MinBox=SpaceBox;
	CD3DBoundingBox MaxBox=SpaceBox;
	

	switch(pRoot->PlaneType)
	{
	case CD3DWOWWMOModelResource::BPT_XY:
		MinBox.m_Max.z=pRoot->Distance;
		MaxBox.m_Min.z=pRoot->Distance;
		break;
	case CD3DWOWWMOModelResource::BPT_YZ:
		MinBox.m_Max.x=pRoot->Distance;
		MaxBox.m_Min.x=pRoot->Distance;		
		break;
	case CD3DWOWWMOModelResource::BPT_XZ:
		MinBox.m_Max.y=pRoot->Distance;
		MaxBox.m_Min.y=pRoot->Distance;
		break;
	case CD3DWOWWMOModelResource::BPT_LEAF:
		if(pRoot->FirstFace>=90&&pRoot->FirstFace<100)
			CreateBSPBox(SpaceBox);		
		return;
	}
	
	WalkBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->RightChildIndex),MinBox);	
	WalkBSPNode(m_pGroupInfo->BSPTree.GetObject(pRoot->LeftChildIndex),MaxBox);
}

}