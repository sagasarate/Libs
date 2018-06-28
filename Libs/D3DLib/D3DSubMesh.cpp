/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DSubMesh.cpp                                           */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DSubMesh,CNameObject);
CD3DSubMesh::CD3DSubMesh(void)
{
	m_pDevice=NULL;
	ZeroMemory(&m_VertexFormat,sizeof(m_VertexFormat));
	m_PrimitiveType=D3DPT_TRIANGLELIST;
	m_PrimitiveCount=0;
	m_pVertexBuffer=NULL;
	m_pDXVertexBuffer=NULL;
	m_IsVertexsSelfDelete=false;
	m_IsDXVertexBufferSelfRelease=false;
	m_VertexCount=0;
	m_StartVertex=0;
	m_pIndexBuffer=NULL;
	m_pDXIndexBuffer=NULL;
	m_IsIndexsSelfDelete=false;
	m_IsDXIndexBufferSelfRelease=false;
	m_IndexCount=0;	
	m_StartIndex=0;
	m_Property=0;
	m_Flag=0;
	m_RenderBufferUsed=BUFFER_USE_DX;
	m_OrginDataBufferUsed=BUFFER_USE_DX;
	
	SetVisible(true);


	m_pVertexBufferR=NULL;
	m_IsVertexBufferRSelfDelete=false;
	m_pIndexBufferR=NULL;
	m_IsIndexBufferRSelfDelete=false;

	m_PrimitiveCountR=0;
	m_VertexCountR=0;
	m_StartVertexR=0;
	m_IndexCountR=0;	
	m_StartIndexR=0;

}

CD3DSubMesh::CD3DSubMesh(CD3DDevice * pDevice)
{
	m_pDevice=pDevice;
	ZeroMemory(&m_VertexFormat,sizeof(m_VertexFormat));
	m_PrimitiveType=D3DPT_TRIANGLELIST;
	m_PrimitiveCount=0;
	m_pVertexBuffer=NULL;
	m_pDXVertexBuffer=NULL;
	m_IsVertexsSelfDelete=false;
	m_IsDXVertexBufferSelfRelease=false;
	m_VertexCount=0;
	m_StartVertex=0;
	m_pIndexBuffer=NULL;
	m_pDXIndexBuffer=NULL;
	m_IsIndexsSelfDelete=false;
	m_IsDXIndexBufferSelfRelease=false;
	m_IndexCount=0;	
	m_StartIndex=0;
	m_Property=0;
	m_Flag=0;
	m_RenderBufferUsed=BUFFER_USE_DX;
	m_OrginDataBufferUsed=BUFFER_USE_DX;
	
	SetVisible(true);

	m_pVertexBufferR=NULL;
	m_IsVertexBufferRSelfDelete=false;
	m_pIndexBufferR=NULL;
	m_IsIndexBufferRSelfDelete=false;

	m_PrimitiveCountR=0;
	m_VertexCountR=0;
	m_StartVertexR=0;
	m_IndexCountR=0;	
	m_StartIndexR=0;
}

CD3DSubMesh::~CD3DSubMesh(void)
{
	Destory();
}


void CD3DSubMesh::Destory()
{
	if(m_IsVertexsSelfDelete)
	{
		SAFE_DELETE_ARRAY(m_pVertexBuffer);
	}
	if(m_IsDXVertexBufferSelfRelease)
	{
		SAFE_RELEASE(m_pDXVertexBuffer);
	}
	if(m_IsIndexsSelfDelete)
	{
		SAFE_DELETE_ARRAY(m_pIndexBuffer);
	}
	if(m_IsDXIndexBufferSelfRelease)
	{
		SAFE_RELEASE(m_pDXIndexBuffer);
	}
	SAFE_RELEASE(m_VertexFormat.pVertexDeclaration);
	if(m_IsVertexBufferRSelfDelete)
	{
		SAFE_DELETE_ARRAY(m_pVertexBufferR);
	}	
	if(m_IsIndexBufferRSelfDelete)
	{
		SAFE_DELETE_ARRAY(m_pIndexBufferR);
	}
	
	m_Material.Destory();
	CNameObject::Destory();
}

bool CD3DSubMesh::Reset()
{
	return m_Material.Reset();
}
bool CD3DSubMesh::Restore()
{
	return m_Material.Restore();
}

void CD3DSubMesh::CreateBounding()
{
	if(m_VertexCount)
	{
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{				
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);
			m_BoundingBox.ComputeFromVertex(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);
			m_BoundingSphere.ComputeFromVertex(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);			
			m_pDXVertexBuffer->Unlock();
		}
		else
		{
			m_BoundingBox.ComputeFromVertex(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);
			m_BoundingSphere.ComputeFromVertex(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);			
		}
	}
	
}

void CD3DSubMesh::CreateBoundingBox()
{
	if(m_VertexCount)
	{
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{				
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);
			m_BoundingBox.ComputeFromVertex(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);		
			m_pDXVertexBuffer->Unlock();
		}
		else
		{
			m_BoundingBox.ComputeFromVertex(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);		
		}
	}
}
void CD3DSubMesh::CreateBoundingSphere()
{
	if(m_VertexCount)
	{
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{				
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);		
			m_BoundingSphere.ComputeFromVertex(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);			
			m_pDXVertexBuffer->Unlock();
		}
		else
		{			
			m_BoundingSphere.ComputeFromVertex(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize);			
		}
	}
}

CD3DBoundingBox CD3DSubMesh::GetBoundingBoxWithTranform(const CD3DMatrix& TransformMat)
{
	CD3DBoundingBox BBox;
	if(m_VertexCount)
	{
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{				
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);
			BBox.ComputeFromVertexWithTransform(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize,TransformMat);		
			m_pDXVertexBuffer->Unlock();
		}
		else
		{
			BBox.ComputeFromVertexWithTransform(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize,TransformMat);		
		}
	}
	return BBox;
}

void CD3DSubMesh::AppendBoundingBoxWithTranform(CD3DBoundingBox& BBox,const CD3DMatrix& TransformMat)
{
	if(m_VertexCount)
	{
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{				
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);
			BBox.AppendFromVertexWithTransform(
				pBuff+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize,TransformMat);		
			m_pDXVertexBuffer->Unlock();
		}
		else
		{
			BBox.AppendFromVertexWithTransform(
				m_pVertexBuffer+m_StartVertex*m_VertexFormat.VertexSize,
				m_VertexCount,
				m_VertexFormat.VertexSize,TransformMat);		
		}
	}
}

bool CD3DSubMesh::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue,bool TestOnly)
{
	BYTE * pVertexBuff=NULL;
	BYTE * pIndexBuff=NULL;
	bool IsIntersect=false;

	if(!m_BoundingBox.RayIntersect(Point,Dir,IntersectPoint,Distance,true))
		return IsIntersect;

	if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
	{
		m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pVertexBuff,D3DLOCK_READONLY);
		if(m_pDXIndexBuffer)
			m_pDXIndexBuffer->Lock(0,0,(LPVOID *)&pIndexBuff,D3DLOCK_READONLY);
	}
	else
	{				
		pVertexBuff=m_pVertexBuffer;
		pIndexBuff=m_pIndexBuffer;
	}
	
	for(UINT t=0;t<m_PrimitiveCount;t++)
	{
		CD3DVector3 p1;
		CD3DVector3 p2;
		CD3DVector3 p3;

		if(pIndexBuff)
		{
			if(m_VertexFormat.IndexSize==2)
			{
				WORD * IndexList=(WORD *)pIndexBuff+m_StartIndex;
				p1=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3]*m_VertexFormat.VertexSize);
				p2=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3+1]*m_VertexFormat.VertexSize);
				p3=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3+2]*m_VertexFormat.VertexSize);
			}
			else
			{
				DWORD * IndexList=(DWORD *)pIndexBuff+m_StartIndex;
				p1=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3]*m_VertexFormat.VertexSize);
				p2=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3+1]*m_VertexFormat.VertexSize);
				p3=*(CD3DVector3 *)(pVertexBuff+IndexList[t*3+2]*m_VertexFormat.VertexSize);
			}
		}
		else
		{
			p1=*(CD3DVector3 *)(pVertexBuff+(m_StartVertex+t*3)*m_VertexFormat.VertexSize);
			p2=*(CD3DVector3 *)(pVertexBuff+(m_StartVertex+t*3+1)*m_VertexFormat.VertexSize);
			p3=*(CD3DVector3 *)(pVertexBuff+(m_StartVertex+t*3+2)*m_VertexFormat.VertexSize);
		}
		

		FLOAT U,V;
		FLOAT Dis;

		if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&Dis))
		{			
			if(IsIntersect)
			{
				if(Dis<Distance)
				{
					Distance=Dis;
					IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
					DotValue=CD3DPlane::FromPoints(p1,p2,p3).GetNormalize().DotNormal(Dir);
				}
			}
			else
			{
				IsIntersect=true;
				if(TestOnly)
				{
					break;
				}
				Distance=Dis;
				IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
				DotValue=CD3DPlane::FromPoints(p1,p2,p3).GetNormalize().DotNormal(Dir);
			}
		}
	}
	if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
	{
		m_pDXVertexBuffer->Unlock();
		if(m_pDXIndexBuffer)
			m_pDXIndexBuffer->Unlock();
	}
	return IsIntersect;
}

bool CD3DSubMesh::LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance,FLOAT& DotValue)
{
	CD3DVector3 P;
	FLOAT D,A;

	if(!m_BoundingBox.LineIntersect(StartPoint,EndPoint,P,D))
		return false;

	CD3DVector3 Dir=(EndPoint-StartPoint);
	FLOAT Len=Dir.Length();
	Dir.Normalize();

	
	if(RayIntersect(StartPoint,Dir,P,D,A,false))
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

bool CD3DSubMesh::GetHeightByXZ(const CD3DMatrix& WorldMatrix,const CD3DVector3& Pos,FLOAT MinHeight,FLOAT MaxHeight,FLOAT& Height,FLOAT& WaterHeight)
{
	CD3DMatrix InvMat=WorldMatrix.GetInverse();
	CD3DVector3 RayPos=Pos;
	CD3DVector3 RayDir(0,-1,0);
	CD3DVector3 IntersectPoint;
	FLOAT Distance,DotValue;
	RayPos.y=MaxHeight;
	RayPos*=InvMat;
	RayDir*=InvMat.GetScaleRotation();
	RayDir.Normalize();
	if(RayIntersect(RayPos,RayDir,IntersectPoint,Distance,DotValue,false))
	{
		IntersectPoint*=WorldMatrix;
		if(IntersectPoint.y>=MinHeight)
		{
			Height=IntersectPoint.y;
			WaterHeight=Height;
			return true;
		}
		//else
		//{
		//	PrintSystemLog(0,"得到的高%g度不在高度范围[%g,%g]内",
		//		IntersectPoint.y,MinHeight,MaxHeight);
		//}
	}
	return false;
}

bool CD3DSubMesh::DeclareVertexFormat(D3DVERTEXELEMENT9* pVertexElements,WORD VertexSize,WORD IndexSize)
{
	if(m_pDevice==NULL)
		return false;
	SAFE_RELEASE(m_VertexFormat.pVertexDeclaration);
	if(m_pDevice->GetD3DDevice()->CreateVertexDeclaration(pVertexElements,&(m_VertexFormat.pVertexDeclaration))==D3D_OK)
	{
		m_VertexFormat.VertexSize=VertexSize;
		m_VertexFormat.IndexSize=IndexSize;
		return true;
	}
	return false;
}

void CD3DSubMesh::SetVertices(BYTE * pVertexBuffer)
{
	m_pVertexBuffer=pVertexBuffer;
	m_IsVertexsSelfDelete=false;	
}

void CD3DSubMesh::AllocVertexBuffer()
{
	m_pVertexBuffer=new BYTE[m_VertexFormat.VertexSize*m_VertexCount];	
	m_IsVertexsSelfDelete=true;	
}
bool CD3DSubMesh::AllocDXVertexBuffer(DWORD Usage,D3DPOOL Pool)
{
	if(m_pDevice==NULL)
		return false;
	SAFE_RELEASE(m_pDXVertexBuffer);
	HRESULT hResult=m_pDevice->GetD3DDevice()->CreateVertexBuffer( 
		m_VertexFormat.VertexSize*m_VertexCount,
		Usage, m_VertexFormat.FVF,
		Pool, &m_pDXVertexBuffer, NULL );
	if(hResult==D3D_OK)
	{
		m_IsDXVertexBufferSelfRelease=true;
		return true;
	}
	else
	{
		PrintD3DLog( _T("申请顶点缓冲失败%X"), hResult);
	}
	return false;	
}

void CD3DSubMesh::SetIndices(BYTE * pIndexBuffer)
{
	m_pIndexBuffer=pIndexBuffer;
	m_IsIndexsSelfDelete=false;	
}


void CD3DSubMesh::AllocIndexBuffer()
{
	m_pIndexBuffer=new BYTE[m_VertexFormat.IndexSize*m_IndexCount];	
	m_IsIndexsSelfDelete=true;	
}

bool CD3DSubMesh::AllocDXIndexBuffer(DWORD Usage,D3DPOOL Pool)
{
	if(m_pDevice==NULL)
		return false;
	SAFE_RELEASE(m_pDXIndexBuffer);
	D3DFORMAT IndexType=D3DFMT_INDEX16;
	if(m_VertexFormat.IndexSize==sizeof(DWORD))
		IndexType=D3DFMT_INDEX32;
	HRESULT hResult=m_pDevice->GetD3DDevice()->CreateIndexBuffer(
		m_VertexFormat.IndexSize*m_IndexCount,
		Usage,			
		IndexType,
		Pool,
		&m_pDXIndexBuffer,
		NULL);
	if(hResult==D3D_OK)
	{
		m_IsDXIndexBufferSelfRelease=true;
		return true;
	}
	else
	{
		PrintD3DLog(_T("申请索引缓冲失败%X"), hResult);
	}
	return false;
}


void CD3DSubMesh::SetVertexBufferR(BYTE * pVertexBuffer,bool IsSelfDelete)
{
	m_pVertexBufferR=pVertexBuffer;
	m_IsVertexBufferRSelfDelete=IsSelfDelete;
}
void CD3DSubMesh::AllocVertexBufferR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
	{
		m_pVertexBufferR=new BYTE[m_VertexFormat.VertexSize*m_VertexCount];	
		//ZeroMemory(m_pVertexBufferR,m_VertexFormat.VertexSize*m_VertexCount);
		m_IsVertexBufferRSelfDelete=true;	
	}
}

void CD3DSubMesh::SetIndexBufferR(BYTE * pIndexBuffer,bool IsSelfDelete)
{
	m_pIndexBufferR=pIndexBuffer;
	m_IsIndexBufferRSelfDelete=IsSelfDelete;
}
void CD3DSubMesh::AllocIndexBufferR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
	{
		m_pIndexBufferR=new BYTE[m_VertexFormat.IndexSize*m_IndexCount];	
		//ZeroMemory(m_pIndexBufferR,m_VertexFormat.IndexSize*m_IndexCount);
		m_IsIndexBufferRSelfDelete=true;	
	}
}

void CD3DSubMesh::OnPrepareRenderData()
{
	if(m_pVertexBufferR)
	{		
		memcpy(m_pVertexBufferR+m_VertexFormat.VertexSize*m_StartVertex,
			m_pVertexBuffer+m_VertexFormat.VertexSize*m_StartVertex,
			m_VertexFormat.VertexSize*m_VertexCount);
	}
	if(m_pIndexBufferR)
	{		
		memcpy(m_pIndexBufferR+m_VertexFormat.IndexSize*m_StartIndex,
			m_pIndexBuffer+m_VertexFormat.IndexSize*m_StartIndex,
			m_VertexFormat.IndexSize*m_IndexCount);
	}

	m_PrimitiveCountR=m_PrimitiveCount;
	m_VertexCountR=m_VertexCount;
	m_StartVertexR=m_StartVertex;
	m_IndexCountR=m_IndexCount;	
	m_StartIndexR=m_StartIndex;
}

bool CD3DSubMesh::SortByName(CD3DSubMesh * pSubMesh1,CD3DSubMesh * pSubMesh2)
{
	if(_tcscmp(pSubMesh1->GetName(),pSubMesh2->GetName())<0)
		return true;
	else
		return false;
}

bool CD3DSubMesh::SortByRender(CD3DSubMesh * pSubMesh1,CD3DSubMesh * pSubMesh2)
{
	BYTE RenderOrder1=pSubMesh1->GetProperty()>>56;
	BYTE RenderOrder2=pSubMesh2->GetProperty()>>56;

	return RenderOrder1<=RenderOrder2;
}

bool CD3DSubMesh::CloneFrom(CNameObject * pObject,UINT Param)
{
	if(!pObject->IsKindOf(GET_CLASS_INFO(CD3DSubMesh)))
		return false;
	CD3DSubMesh * pSrcSubMesh=(CD3DSubMesh *)pObject;

	Destory();

	m_pDevice=pSrcSubMesh->m_pDevice;
	m_VertexFormat=pSrcSubMesh->m_VertexFormat;
	m_PrimitiveType=pSrcSubMesh->m_PrimitiveType;
	m_PrimitiveCount=pSrcSubMesh->m_PrimitiveCount;
	
	m_VertexCount=pSrcSubMesh->m_VertexCount;
	m_StartVertex=pSrcSubMesh->m_StartVertex;
	
	m_IndexCount=pSrcSubMesh->m_IndexCount;
	m_StartIndex=pSrcSubMesh->m_StartIndex;
	
	m_BoundingBox=pSrcSubMesh->m_BoundingBox;
	m_BoundingSphere=pSrcSubMesh->m_BoundingSphere;
	m_Property=pSrcSubMesh->m_Property;
	m_Flag=pSrcSubMesh->m_Flag;

	if(Param&SUBMESH_CLONE_USE_SYSTEM_MEM)
	{
		m_RenderBufferUsed=BUFFER_USE_CUSTOM;
		m_OrginDataBufferUsed=BUFFER_USE_CUSTOM;
	}
	else
	{
		m_RenderBufferUsed=pSrcSubMesh->m_OrginDataBufferUsed;
		m_OrginDataBufferUsed=pSrcSubMesh->m_OrginDataBufferUsed;
	}

	if((Param&SUBMESH_CLONE_NOT_CLONE_MATERIAL)==0)
	{
		m_Material.CloneFrom(&(pSrcSubMesh->m_Material),Param);
	}

	{
		void * pSrcVertexData=NULL;
		void * pDestVertexData=NULL;
		if(pSrcSubMesh->m_OrginDataBufferUsed==BUFFER_USE_DX&&pSrcSubMesh->GetDXVertexBuffer())
		{
			pSrcSubMesh->GetDXVertexBuffer()->Lock(0,0,&pSrcVertexData,0);
		}
		else
		{
			pSrcVertexData=pSrcSubMesh->GetVertexBuffer();
		}

		if(m_OrginDataBufferUsed==BUFFER_USE_DX)
		{
			AllocDXVertexBuffer();
			GetDXVertexBuffer()->Lock(0,0,&pDestVertexData,0);
		}
		else
		{
			if(m_IsVertexsSelfDelete)
			{
				AllocVertexBuffer();
				pDestVertexData=GetVertexBuffer();
			}
		}
		if(pDestVertexData&&pSrcVertexData)
		{
			memcpy(pDestVertexData,pSrcVertexData,m_VertexFormat.VertexSize*m_VertexCount);
		}		
		if(pSrcSubMesh->m_OrginDataBufferUsed==BUFFER_USE_DX&&pSrcSubMesh->GetDXVertexBuffer())
		{
			pSrcSubMesh->GetDXVertexBuffer()->Unlock();
		}
		if(m_OrginDataBufferUsed==BUFFER_USE_DX)
		{
			GetDXVertexBuffer()->Unlock();
		}
	}


	{
		void * pSrcIndexData=NULL;
		void * pDestIndexData=NULL;
		if(pSrcSubMesh->m_OrginDataBufferUsed==BUFFER_USE_DX&&pSrcSubMesh->GetDXIndexBuffer())
		{
			pSrcSubMesh->GetDXIndexBuffer()->Lock(0,0,&pSrcIndexData,0);
		}
		else
		{
			pSrcIndexData=pSrcSubMesh->GetIndexBuffer();
		}

		if(m_OrginDataBufferUsed==BUFFER_USE_DX)
		{
			AllocDXIndexBuffer();
			GetDXIndexBuffer()->Lock(0,0,&pDestIndexData,0);
		}
		else
		{
			if(m_IsIndexsSelfDelete)
			{
				AllocIndexBuffer();
				pDestIndexData=GetIndexBuffer();
			}
		}
		if(pDestIndexData&&pSrcIndexData)
		{
			memcpy(pDestIndexData,pSrcIndexData,m_VertexFormat.IndexSize*m_IndexCount);
		}		
		if(pSrcSubMesh->m_OrginDataBufferUsed==BUFFER_USE_DX&&pSrcSubMesh->GetDXIndexBuffer())
		{
			pSrcSubMesh->GetDXIndexBuffer()->Unlock();
		}
		if(m_OrginDataBufferUsed==BUFFER_USE_DX)
		{
			GetDXIndexBuffer()->Unlock();
		}
	}

	return true;
}


void CD3DSubMesh::PickResource(CUSOResourceManager * pResourceManager,UINT Param)
{
	m_Material.PickResource(pResourceManager,Param);
}

bool CD3DSubMesh::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(!CNameObject::ToSmartStruct(Packet,pResourceManager,Param))
		return false;
	if(m_VertexFormat.pVertexDeclaration)
	{
		UINT ElementCount;
		m_VertexFormat.pVertexDeclaration->GetDeclaration(NULL,&ElementCount);
		D3DVERTEXELEMENT9 * pElements=new D3DVERTEXELEMENT9[ElementCount];
		m_VertexFormat.pVertexDeclaration->GetDeclaration(pElements,&ElementCount);
		bool Ret=Packet.AddMember(SST_D3DSM_VERTEX_DECLARATION,(char *)pElements,sizeof(D3DVERTEXELEMENT9)*ElementCount);
		delete[] pElements;
		if(!Ret)
			return false;
	}
	else
	{
		CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_FVF, (UINT)m_VertexFormat.FVF));
	}
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_VERTEX_SIZE,m_VertexFormat.VertexSize));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_INDEX_SIZE,m_VertexFormat.IndexSize));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_PRIMITIVE_TYPE,m_PrimitiveType));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_PRIMITIVE_COUNT,m_PrimitiveCount));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_VERTEX_COUNT,m_VertexCount));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_INDEX_COUNT,m_IndexCount));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_BOUNDING_BOX,(char *)&m_BoundingBox,sizeof(m_BoundingBox)));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_BOUNDING_SPHERE,(char *)&m_BoundingSphere,sizeof(m_BoundingSphere)));
	CHECK_SMART_STRUCT_ADD_AND_RETURN(Packet.AddMember(SST_D3DSM_PROPERTY,m_Property));

	if(m_VertexCount)
	{
		UINT DataSize=m_VertexCount*m_VertexFormat.VertexSize;
		bool Ret=false;
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{
			BYTE *pBuff;
			m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);			
			Ret=Packet.AddMember(SST_D3DSM_VERTEX,(char *)pBuff,DataSize);
			m_pDXVertexBuffer->Unlock();
			
		}
		else if(m_IsVertexsSelfDelete)
		{
			Ret=Packet.AddMember(SST_D3DSM_VERTEX,(char *)m_pVertexBuffer,DataSize);
		}
		if(!Ret)
			return false;
	}
	if(m_IndexCount)
	{
		UINT DataSize=m_IndexCount*m_VertexFormat.IndexSize;
		bool Ret=false;
		if(GetOrginDataBufferUsed()==BUFFER_USE_DX)
		{
			BYTE *pBuff;
			m_pDXIndexBuffer->Lock(0,0,(LPVOID *)&pBuff,0);			
			Ret=Packet.AddMember(SST_D3DSM_INDEX,(char *)pBuff,DataSize);
			m_pDXIndexBuffer->Unlock();
			
		}
		else if(m_IsIndexsSelfDelete)
		{
			Ret=Packet.AddMember(SST_D3DSM_VERTEX,(char *)m_pIndexBuffer,DataSize);
		}
		if(!Ret)
			return false;
	}

	{
		UINT BufferSize;
		void * pBuffer=Packet.PrepareMember(BufferSize);
		CSmartStruct SubPacket(pBuffer,BufferSize,true);
		if(!m_Material.ToSmartStruct(SubPacket,pResourceManager,Param))
			return false;
		if(!Packet.FinishMember(SST_D3DSM_MATERIAL,SubPacket.GetDataLen()))
			return false;
	}
	return true;
}
bool CD3DSubMesh::FromSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	if(m_pDevice==NULL)
		return false;

	if(!CNameObject::FromSmartStruct(Packet,pResourceManager,Param))
		return false;

	void * Pos=Packet.GetFirstMemberPosition();
	while(Pos)
	{
		WORD MemberID;
		CSmartValue Value=Packet.GetNextMember(Pos,MemberID);
		switch(MemberID)
		{
		case SST_D3DSM_FVF:
			m_VertexFormat.FVF=(UINT)Value;
			break;
		case SST_D3DSM_VERTEX_DECLARATION:
			{
				D3DVERTEXELEMENT9* pVertexElements=(D3DVERTEXELEMENT9*)((LPCTSTR)Value);				
				if(m_pDevice->GetD3DDevice()->CreateVertexDeclaration(pVertexElements,&(m_VertexFormat.pVertexDeclaration))!=D3D_OK)
					return false;
			}
			break;
		case SST_D3DSM_VERTEX_SIZE:
			m_VertexFormat.VertexSize=Value;
			break;
		case SST_D3DSM_INDEX_SIZE:
			m_VertexFormat.IndexSize=Value;
			break;
		case SST_D3DSM_PRIMITIVE_TYPE:
			m_PrimitiveType=Value;
			break;
		case SST_D3DSM_PRIMITIVE_COUNT:
			m_PrimitiveCount=Value;
			break;
		case SST_D3DSM_VERTEX_COUNT:
			m_VertexCount=Value;
			break;
		case SST_D3DSM_INDEX_COUNT:
			m_IndexCount=Value;
			break;
		case SST_D3DSM_BOUNDING_BOX:
			memcpy(&m_BoundingBox,(LPCSTR)Value,sizeof(m_BoundingBox));
			break;
		case SST_D3DSM_BOUNDING_SPHERE:
			memcpy(&m_BoundingSphere,(LPCSTR)Value,sizeof(m_BoundingSphere));
			break;
		case SST_D3DSM_PROPERTY:
			m_Property=Value;
			break;
		case SST_D3DSM_VERTEX:
			if(m_VertexCount)
			{
				AllocDXVertexBuffer();

				int DataSize=m_VertexCount*m_VertexFormat.VertexSize;		
				BYTE *pDataBuff;
				m_pDXVertexBuffer->Lock(0,0,(LPVOID *)&pDataBuff,0);
				memcpy(	pDataBuff,(LPCSTR)Value,DataSize);
				m_pDXVertexBuffer->Unlock();				
			}		
			break;
		case SST_D3DSM_INDEX:
			if(m_IndexCount)
			{
				AllocDXIndexBuffer();
				int DataSize=m_IndexCount*m_VertexFormat.IndexSize;
				BYTE *pDataBuff;
				m_pDXIndexBuffer->Lock(0,0,(LPVOID *)&pDataBuff,0);
				memcpy(	pDataBuff,(LPCSTR)Value,DataSize);
				m_pDXIndexBuffer->Unlock();				
			}
			break;
		case SST_D3DSM_MATERIAL:
			{
				CSmartStruct SubPacket=Value;
				if(!m_Material.FromSmartStruct(SubPacket,pResourceManager,Param))
					return false;
			}
			break;
		}
	}	
	return true;
}

UINT CD3DSubMesh::GetSmartStructSize(UINT Param)
{
	UINT Size=CNameObject::GetSmartStructSize(Param);

	if(m_VertexFormat.pVertexDeclaration)
	{
		UINT ElementCount;
		m_VertexFormat.pVertexDeclaration->GetDeclaration(NULL,&ElementCount);
		Size += CSmartStruct::GetStringMemberSizeA(sizeof(D3DVERTEXELEMENT9)*ElementCount);
	}
	else
	{
		Size+=CSmartStruct::GetFixMemberSize(sizeof(m_VertexFormat.FVF));
	}
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_VertexFormat.VertexSize));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_VertexFormat.IndexSize));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_PrimitiveType));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_PrimitiveCount));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_VertexCount));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_IndexCount));
	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_BoundingBox));
	Size += CSmartStruct::GetStringMemberSizeA(sizeof(m_BoundingSphere));
	Size+=CSmartStruct::GetFixMemberSize(sizeof(m_Property));
	Size += CSmartStruct::GetStringMemberSizeA(m_VertexCount*m_VertexFormat.VertexSize);
	Size += CSmartStruct::GetStringMemberSizeA(m_IndexCount*m_VertexFormat.IndexSize);

	Size+=CSmartStruct::GetStructMemberSize(m_Material.GetSmartStructSize(Param));
	
	return Size;
}

bool CD3DSubMesh::CheckValid()
{
	if(GetRenderBufferUsed()==BUFFER_USE_CUSTOM)
	{
		for(UINT i=m_StartIndex;i<m_StartIndex+m_IndexCount;i++)
		{
			UINT Index;
			if(m_VertexFormat.IndexSize==2)
			{
				Index=((WORD *)m_pIndexBuffer)[i];
			}
			else
			{
				Index=((DWORD *)m_pIndexBuffer)[i];
			}
			if(Index<m_StartVertex||Index>=m_StartVertex+m_VertexCount)
			{
				PrintD3DLog(_T("CD3DSubMesh::CheckValid:索引值超出顶点范围"));
				return false;
			}
		}
		for(UINT i=m_StartVertex;i<m_StartVertex+m_VertexCount;i++)
		{
			float VertexValue=*((float *)(m_pVertexBuffer+i*m_VertexFormat.VertexSize));
			if(VertexValue<-1e10f&&VertexValue>1e10f)
			{
				PrintD3DLog(_T("CD3DSubMesh::CheckValid:顶点数值异常"));
				return false;
			}
			BYTE LastByte=(m_pVertexBuffer+i*m_VertexFormat.VertexSize)[m_VertexFormat.VertexSize-1];
		}
	}
	return true;
}

bool CD3DSubMesh::CheckValidR()
{
	if(GetRenderBufferUsed()==BUFFER_USE_CUSTOM)
	{
		for(UINT i=m_StartIndexR;i<m_StartIndexR+m_IndexCountR;i++)
		{
			UINT Index;
			if(m_VertexFormat.IndexSize==2)
			{
				Index=((WORD *)GetIndexBufferR())[i];
			}
			else
			{
				Index=((DWORD *)GetIndexBufferR())[i];
			}
			if(Index<m_StartVertexR||Index>=m_StartVertexR+m_VertexCountR)
			{
				PrintD3DLog(_T("CD3DSubMesh::CheckValidR:索引值超出顶点范围"));
				return false;
			}
		}
		for(UINT i=m_StartVertexR;i<m_StartVertexR+m_VertexCountR;i++)
		{
			float VertexValue=*((float *)(GetVertexBufferR()+i*m_VertexFormat.VertexSize));
			if(VertexValue<-1e10f&&VertexValue>1e10f)
			{
				PrintD3DLog(_T("CD3DSubMesh::CheckValidR:顶点数值异常"));
				return false;
			}
			BYTE LastByte=(GetVertexBufferR()+i*m_VertexFormat.VertexSize)[m_VertexFormat.VertexSize-1];
		}
	}
	return true;
}

}