/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DCamera.cpp                                            */
/*      创建日期:  2009年10月15日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

IMPLEMENT_CLASS_INFO(CD3DCamera,CD3DObject);

CD3DCamera::CD3DCamera():CD3DObject()
{	
	m_ProjectMatrix.SetIdentity();
	m_ViewMatrix.SetIdentity();	
	m_Near=0.0f;
	m_Far=1.0f;
}

CD3DCamera::~CD3DCamera(void)
{
	Destory();
}

void CD3DCamera::Destory()
{
	CD3DObject::Destory();
}

void CD3DCamera::GetPickRay(FLOAT ScreenX,FLOAT ScreenY,CD3DVector3& Point,CD3DVector3& Dir)
{
	CD3DVector3 v;

	v.x =  ScreenX / m_ProjectMatrix._11;
	v.y =  ScreenY / m_ProjectMatrix._22;
	v.z = 1.0f;
	CD3DMatrix Mat=GetWorldMatrix();
	

	Dir.x  = v.x*Mat._11 + v.y*Mat._21 + v.z*Mat._31;//+ Mat._41;
	Dir.y  = v.x*Mat._12 + v.y*Mat._22 + v.z*Mat._32;//+ Mat._42;
	Dir.z  = v.x*Mat._13 + v.y*Mat._23 + v.z*Mat._33;//+ Mat._43;
	Dir.Normalize();

	Point.x = Mat._41;
	Point.y = Mat._42;
	Point.z = Mat._43;
}

void CD3DCamera::GetPickRay(int ScreenX,int ScreenY,int ScreenWidth,int ScreenHeight,CD3DVector3& Point,CD3DVector3& Dir)
{
	GetPickRay((FLOAT)((2.0f*ScreenX/ScreenWidth)-1),
		(FLOAT)-((2.0f*ScreenY/ScreenHeight)-1),
		Point,Dir);
}

void CD3DCamera::OnPrepareRenderData()
{
	{
		CAutoLock Lock(GetRenderLock());

		m_WorldMatrixR=m_WorldMatrix;
		m_ViewMatrixR=m_ViewMatrix;
		m_ProjectMatrixR=m_ProjectMatrix;
		m_FrustumR=m_Frustum;	

	}

	for(UINT i=0;i<m_ChildList.GetCount();i++)
	{
		((CD3DObject *)m_ChildList[i])->OnPrepareRenderData();
	}
}

void CD3DCamera::Update(FLOAT Time)
{
	//更新世界矩阵
	
	m_WorldMatrix=m_LocalMatrix;
	if(GetParent())
	{
		m_WorldMatrix*=GetParent()->GetWorldMatrix().GetRotation();
		m_WorldMatrix*=CD3DMatrix::FromTranslation(GetParent()->GetWorldMatrix().GetTranslation());
	}
	m_ViewMatrix=GetWorldMatrix().GetInverse();
	m_Frustum=CD3DFrustum::FromMatrixs(m_ViewMatrix,m_ProjectMatrix);
	
	//更新子对象
	for(UINT i=0;i<GetChildCount();i++)
		GetChildByIndex(i)->Update(Time);
}


}