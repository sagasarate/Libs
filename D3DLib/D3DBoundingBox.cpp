/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBoundingBox.cpp                                       */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"

namespace D3DLib{

bool CD3DBoundingBox::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly)  const
{	
	CD3DVector3 p1,p2,p3;
	FLOAT U,V;
	FLOAT D;
	FLOAT Dis=3.4E+38f;
	bool IsIntersect=false;
	//顶面
	p1=CD3DVector3(m_Min.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Max.y,m_Max.z);
	p3=CD3DVector3(m_Min.x,m_Max.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}

	p1=CD3DVector3(m_Max.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Max.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Max.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	//底面
	p1=CD3DVector3(m_Min.x,m_Min.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Min.y,m_Max.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}

	p1=CD3DVector3(m_Max.x,m_Min.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Min.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	//前面
	p1=CD3DVector3(m_Min.x,m_Max.y,m_Min.z);
	p2=CD3DVector3(m_Max.x,m_Max.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	p1=CD3DVector3(m_Max.x,m_Max.y,m_Min.z);
	p2=CD3DVector3(m_Max.x,m_Min.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Min.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	//后面
	p1=CD3DVector3(m_Min.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Max.y,m_Max.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	p1=CD3DVector3(m_Max.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Min.y,m_Max.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	//左面
	p1=CD3DVector3(m_Min.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Min.x,m_Max.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	p1=CD3DVector3(m_Min.x,m_Max.y,m_Min.z);
	p2=CD3DVector3(m_Min.x,m_Min.y,m_Min.z);
	p3=CD3DVector3(m_Min.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	//右面
	p1=CD3DVector3(m_Max.x,m_Max.y,m_Max.z);
	p2=CD3DVector3(m_Max.x,m_Max.y,m_Min.z);
	p3=CD3DVector3(m_Max.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	p1=CD3DVector3(m_Max.x,m_Max.y,m_Min.z);
	p2=CD3DVector3(m_Max.x,m_Min.y,m_Min.z);
	p3=CD3DVector3(m_Max.x,m_Min.y,m_Max.z);
	if(D3DXIntersectTri(&p1,&p2,&p3,&Point,&Dir,&U,&V,&D))
	{
		if(TestOnly)
			return true;
		if(D<Dis)
		{
			IsIntersect=true;
			Dis=D;
			IntersectPoint=p1+(p2-p1)*U+(p3-p1)*V;
			Distance=Dis;
		}
	}
	return IsIntersect;
}

}