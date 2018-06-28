/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBoundingFrame.cpp                                     */
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



	



IMPLEMENT_CLASS_INFO(CD3DBoundingFrame,CD3DObject);

CD3DBoundingFrame::CD3DBoundingFrame(void):CD3DObject()
{
	m_pSubMesh=NULL;
}

CD3DBoundingFrame::~CD3DBoundingFrame(void)
{
	Destory();
}

void CD3DBoundingFrame::Destory()
{
	CD3DObject::Destory();

	SAFE_RELEASE(m_pSubMesh);	
	
}

bool CD3DBoundingFrame::Reset()
{		
	return m_pSubMesh->Reset()&&CD3DObject::Reset();
	
}

bool CD3DBoundingFrame::Restore()
{		
	return m_pSubMesh->Restore()&&CD3DObject::Restore();	
}

bool CD3DBoundingFrame::CreateFromBBox(const CD3DBoundingBox& BBox,D3DCOLOR Color)
{
	if(GetDevice()==NULL)
		return false;

	Destory();

	m_BoundingBox=BBox;

	m_pSubMesh=new CD3DSubMesh(GetDevice());

	m_pSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
	m_pSubMesh->GetVertexFormat().VertexSize=sizeof(BF_VERTEX);
	m_pSubMesh->SetVertexCount(12*2);
	m_pSubMesh->SetPrimitiveCount(12);
	m_pSubMesh->SetPrimitiveType(D3DPT_LINELIST);
	m_pSubMesh->GetBoundingBox()=BBox;

	m_pSubMesh->AllocDXVertexBuffer();	

	
	m_pSubMesh->GetMaterial().SetFX(
		GetDevice()->GetFXManager()->LoadFXFromMemory(_T("DEFAULT_BOUND_FRAME_FX_NT"),(void *)DEFAULT_BOUND_FRAME_FX_NT,(int)strlen(DEFAULT_BOUND_FRAME_FX_NT)));

	
	BF_VERTEX * pBuff;



	
	m_pSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pBuff,0);


	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Min.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Min.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Min.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Min.z),Color);
	pBuff+=2;

	//
	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Min.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Max.z),Color);
	pBuff+=2;
	//
	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Max.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Min.y,BBox.m_Max.z),
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Max.x,BBox.m_Max.y,BBox.m_Max.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Max.z),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(BBox.m_Min.x,BBox.m_Max.y,BBox.m_Max.z),
		CD3DVector3(BBox.m_Min.x,BBox.m_Min.y,BBox.m_Max.z),Color);
	pBuff+=2;

	m_pSubMesh->GetDXVertexBuffer()->Unlock();


	return true;
}

bool CD3DBoundingFrame::CreateFromLight(CD3DLight * pLight,D3DCOLOR Color)
{
	if(GetDevice()==NULL)
		return false;

	Destory();

	m_BoundingBox=*(pLight->GetBoundingBox());
	
	

	m_pSubMesh=new CD3DSubMesh(GetDevice());

	m_pSubMesh->GetVertexFormat().FVF=D3DFVF_XYZ|D3DFVF_DIFFUSE;
	m_pSubMesh->GetVertexFormat().VertexSize=sizeof(BF_VERTEX);
	m_pSubMesh->SetVertexCount(8*2);
	m_pSubMesh->SetPrimitiveCount(8);
	m_pSubMesh->SetPrimitiveType(D3DPT_LINELIST);
	m_pSubMesh->GetBoundingBox()=m_BoundingBox;

	m_pSubMesh->AllocDXVertexBuffer();	


	m_pSubMesh->GetMaterial().SetFX(
		GetDevice()->GetFXManager()->LoadFXFromMemory(_T("DEFAULT_BOUND_FRAME_FX_NT"),(void *)DEFAULT_BOUND_FRAME_FX_NT,(int)+strlen(DEFAULT_BOUND_FRAME_FX_NT)));


	BF_VERTEX * pBuff;




	m_pSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pBuff,0);


	BuildLine(pBuff,
		CD3DVector3( 0.0f, 0.0f, 0.0f),
		CD3DVector3(-0.2f,-0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3( 0.0f, 0.0f, 0.0f),
		CD3DVector3(-0.2f, 0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3( 0.0f, 0.0f, 0.0f),
		CD3DVector3( 0.2f,-0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3( 0.0f, 0.0f, 0.0f),
		CD3DVector3( 0.2f, 0.2f, 1.0f),Color);
	pBuff+=2;


	BuildLine(pBuff,
		CD3DVector3( 0.2f, 0.2f, 1.0f),
		CD3DVector3( 0.2f,-0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3( 0.2f, 0.2f, 1.0f),
		CD3DVector3(-0.2f, 0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(-0.2f,-0.2f, 1.0f),
		CD3DVector3( 0.2f,-0.2f, 1.0f),Color);
	pBuff+=2;

	BuildLine(pBuff,
		CD3DVector3(-0.2f,-0.2f, 1.0f),
		CD3DVector3(-0.2f, 0.2f, 1.0f),Color);
	pBuff+=2;

	
	

	m_pSubMesh->GetDXVertexBuffer()->Unlock();


	return true;
}

void CD3DBoundingFrame::SetColor(D3DCOLOR Color)
{
	if(m_pSubMesh->GetDXVertexBuffer())
	{
		BF_VERTEX * pBuff;
		m_pSubMesh->GetDXVertexBuffer()->Lock(0,0,(LPVOID *)&pBuff,0);
		for(UINT i=0;i<m_pSubMesh->GetVertexCount();i++)
		{
			pBuff[i].Color=Color;
		}
		m_pSubMesh->GetDXVertexBuffer()->Unlock();
	}
}

int CD3DBoundingFrame::GetSubMeshCount()
{
	return 1;
}

CD3DSubMesh * CD3DBoundingFrame::GetSubMesh(UINT index)
{
	return m_pSubMesh;
}

CD3DBoundingBox * CD3DBoundingFrame::GetBoundingBox()
{
	return &m_BoundingBox;
}

bool CD3DBoundingFrame::CanDoSubMeshViewCull()
{
	return false;
}

bool CD3DBoundingFrame::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly)
{
	return false;
}
bool CD3DBoundingFrame::GetHeightByXZ(FLOAT x,FLOAT z,FLOAT& y)
{
	return false;
}


void CD3DBoundingFrame::BuildLine(BF_VERTEX * pBuff,CD3DVector3 p1,CD3DVector3 p2,D3DCOLOR Color)
{
	pBuff[0].Pos=p1;
	pBuff[1].Pos=p2;
	for(int i=0;i<2;i++)
	{
		pBuff[i].Color=Color;
	}
}

bool CD3DBoundingFrame::ToSmartStruct(CSmartStruct& Packet,CUSOResourceManager * pResourceManager,UINT Param)
{
	PrintD3DLog( _T("CD3DBoundingFrame被保存"));
	return CD3DObject::ToSmartStruct(Packet,pResourceManager,Param);
}

UINT CD3DBoundingFrame::GetSmartStructSize(UINT Param)
{
	PrintD3DLog(_T("CD3DBoundingFrame被保存"));
	return CD3DObject::GetSmartStructSize(Param);
}

}