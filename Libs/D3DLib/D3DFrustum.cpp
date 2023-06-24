/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DFrustum.cpp                                           */
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

CD3DFrustum CD3DFrustum::FromMatrixs(CD3DMatrix ViewMat,CD3DMatrix PrjMat)
{
	CD3DFrustum Frustum;
	CD3DVector3 CornerVertexs[8];
	ViewMat.Invert();
	PrjMat.Invert();

	CornerVertexs[0] = CD3DVector3(-1.0f, -1.0f,  0.0f); // left-bottom
	CornerVertexs[1] = CD3DVector3( 1.0f, -1.0f,  0.0f); // right-bottom
	CornerVertexs[2] = CD3DVector3(-1.0f,  1.0f,  0.0f); // left-top
	CornerVertexs[3] = CD3DVector3( 1.0f,  1.0f,  0.0f); // right-top

	CornerVertexs[4] = CD3DVector3(-1.0f, -1.0f,  1.0f); // left-bottom
	CornerVertexs[5] = CD3DVector3( 1.0f, -1.0f,  1.0f); // right-bottom
	CornerVertexs[6] = CD3DVector3(-1.0f,  1.0f,  1.0f); // left-top
	CornerVertexs[7] = CD3DVector3( 1.0f,  1.0f,  1.0f); // right-top

	for(int i=0;i<8;i++)
	{
		CornerVertexs[i] =CornerVertexs[i]*PrjMat;
	}

	for(int i=0;i<8;i++)
	{
		CornerVertexs[i] =CornerVertexs[i]*ViewMat;
	}

	

	Frustum.m_Planes[0] = CD3DPlane::FromPoints(CornerVertexs[0], CornerVertexs[1], CornerVertexs[2]);
	Frustum.m_Planes[1] = CD3DPlane::FromPoints(CornerVertexs[6], CornerVertexs[5], CornerVertexs[4]);
	Frustum.m_Planes[2] = CD3DPlane::FromPoints(CornerVertexs[2], CornerVertexs[3], CornerVertexs[6]);
	Frustum.m_Planes[3] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[1], CornerVertexs[0]);
	Frustum.m_Planes[4] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[0], CornerVertexs[2]);
	Frustum.m_Planes[5] = CD3DPlane::FromPoints(CornerVertexs[5], CornerVertexs[3], CornerVertexs[1]);

	return Frustum;
}

CD3DFrustum CD3DFrustum::FromCombinedMatrix(CD3DMatrix TransMat)
{
	CD3DFrustum Frustum;
	CD3DVector3 CornerVertexs[8];
	TransMat.Invert();

	CornerVertexs[0] = CD3DVector3(-1.0f, -1.0f,  0.0f); // left-bottom
	CornerVertexs[1] = CD3DVector3( 1.0f, -1.0f,  0.0f); // right-bottom
	CornerVertexs[2] = CD3DVector3(-1.0f,  1.0f,  0.0f); // left-top
	CornerVertexs[3] = CD3DVector3( 1.0f,  1.0f,  0.0f); // right-top

	CornerVertexs[4] = CD3DVector3(-1.0f, -1.0f,  1.0f); // left-bottom
	CornerVertexs[5] = CD3DVector3( 1.0f, -1.0f,  1.0f); // right-bottom
	CornerVertexs[6] = CD3DVector3(-1.0f,  1.0f,  1.0f); // left-top
	CornerVertexs[7] = CD3DVector3( 1.0f,  1.0f,  1.0f); // right-top

	for(int i=0;i<8;i++)
	{
		CornerVertexs[i] =CornerVertexs[i]*TransMat;
	}

	Frustum.m_Planes[0] = CD3DPlane::FromPoints(CornerVertexs[0], CornerVertexs[1], CornerVertexs[2]);
	Frustum.m_Planes[1] = CD3DPlane::FromPoints(CornerVertexs[6], CornerVertexs[5], CornerVertexs[4]);
	Frustum.m_Planes[2] = CD3DPlane::FromPoints(CornerVertexs[2], CornerVertexs[3], CornerVertexs[6]);
	Frustum.m_Planes[3] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[1], CornerVertexs[0]);
	Frustum.m_Planes[4] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[0], CornerVertexs[2]);
	Frustum.m_Planes[5] = CD3DPlane::FromPoints(CornerVertexs[5], CornerVertexs[3], CornerVertexs[1]);

	return Frustum;
}

CD3DFrustum CD3DFrustum::FromCustomViewFace(CD3DMatrix ViewMat,CD3DMatrix PrjMat,FLOAT MaxX,FLOAT MaxY,FLOAT MaxZ,FLOAT MinX,FLOAT MinY,FLOAT MinZ)
{
	CD3DFrustum Frustum;
	CD3DVector3 CornerVertexs[8];
	ViewMat.Invert();
	PrjMat.Invert();

	CornerVertexs[0] = CD3DVector3(MinX, MinY, MinZ); // left-bottom
	CornerVertexs[1] = CD3DVector3(MaxX, MinY, MinZ); // right-bottom
	CornerVertexs[2] = CD3DVector3(MinX, MaxY, MinZ); // left-top
	CornerVertexs[3] = CD3DVector3(MaxX, MaxY, MinZ); // right-top

	CornerVertexs[4] = CD3DVector3(MinX, MinY, MaxZ); // left-bottom
	CornerVertexs[5] = CD3DVector3(MaxX, MinY, MaxZ); // right-bottom
	CornerVertexs[6] = CD3DVector3(MinX, MaxY, MaxZ); // left-top
	CornerVertexs[7] = CD3DVector3(MaxX, MaxY, MaxZ); // right-top

	for(int i=0;i<8;i++)
	{
		CornerVertexs[i] =CornerVertexs[i]*PrjMat;
	}

	for(int i=0;i<8;i++)
	{
		CornerVertexs[i] =CornerVertexs[i]*ViewMat;
	}

	Frustum.m_Planes[0] = CD3DPlane::FromPoints(CornerVertexs[0], CornerVertexs[1], CornerVertexs[2]);//near face
	Frustum.m_Planes[1] = CD3DPlane::FromPoints(CornerVertexs[6], CornerVertexs[5], CornerVertexs[4]);//far face
	Frustum.m_Planes[2] = CD3DPlane::FromPoints(CornerVertexs[2], CornerVertexs[3], CornerVertexs[6]);//top face
	Frustum.m_Planes[3] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[1], CornerVertexs[0]);//bottom face
	Frustum.m_Planes[4] = CD3DPlane::FromPoints(CornerVertexs[4], CornerVertexs[0], CornerVertexs[2]);//left face
	Frustum.m_Planes[5] = CD3DPlane::FromPoints(CornerVertexs[5], CornerVertexs[3], CornerVertexs[1]);//right face

	return Frustum;
}

int CD3DFrustum::BoxLocation(const D3DXVECTOR3& Min,const D3DXVECTOR3& Max) const
{
	D3DXVECTOR3     m       = Min + (Max-Min)*0.5;  // center of AABB
	D3DXVECTOR3     d       = Max - m;				 // half-diagonal

	int					i = 0;	
	int					incount = 0;
	while (i<6) 
	{		
		float NP = (float)(d.x*fabs(m_Planes[i].a)+d.y*fabs(m_Planes[i].b)+d.z*fabs(m_Planes[i].c));
		float MP = m.x*m_Planes[i].a+m.y*m_Planes[i].b+m.z*m_Planes[i].c+m_Planes[i].d;
		if ((MP+NP) < 0.0f) 	
			return 0;					// behind clip plane		
		if ((MP-NP) < 0.0f) 
			incount++;
		i++;									// next plane
	}
	if (incount == 0)
		return 2;                             // AABB intersects frustum
	else 
		return 1;
}

int CD3DFrustum::BoxLocation(const CD3DBoundingBox& Box) const
{
	return BoxLocation(Box.m_Min,Box.m_Max);
}

int CD3DFrustum::BoxLocation(D3DXVECTOR3 * pVertices) const
{
	int vec_in_cnt;
	int plane_in_cnt = 0;

	for( int p = 0; p < 6; p++ )
	{
		vec_in_cnt = 0;

		for( int v=0; v<8; v++)
		{
			if( m_Planes[p].a * pVertices[v].x +
				m_Planes[p].b * pVertices[v].y +
				m_Planes[p].c * pVertices[v].z +
				m_Planes[p].d > 0)
				vec_in_cnt ++;
		}

		if (vec_in_cnt == 0)
			return 0;

		if (vec_in_cnt == 8)
			plane_in_cnt++;
	}
	if (plane_in_cnt == 6)
		return 2;
	else 
		return 1;
}

int CD3DFrustum::PointLocation(const D3DXVECTOR3 & Point) const
{
	for( int p = 0; p < 6; p++ )
	{
		if( m_Planes[p].a * Point.x +
			m_Planes[p].b * Point.y +
			m_Planes[p].c * Point.z +
			m_Planes[p].d < 0)
		{
			return 0;
		}
	}
	return 2;
}

int CD3DFrustum::SphereLocation(const D3DXVECTOR3 & Center, FLOAT Radius) const
{
	int iCount = 0;
	float fDistance;
	for (UINT iPlane=0; iPlane<6; iPlane++)
	{
		fDistance = m_Planes[iPlane].a * Center.x +
			m_Planes[iPlane].b * Center.y +
			m_Planes[iPlane].c * Center.z +
			m_Planes[iPlane].d;
		if (fDistance <= -Radius)
			return false;
		if (fDistance > Radius)
			iCount++;
	}
	return (iCount == 6) ? 2 : 1;
}

}
