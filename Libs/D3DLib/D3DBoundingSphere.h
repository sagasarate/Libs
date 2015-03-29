/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBoundingSphere.h                                      */
/*      创建日期:  2009年09月11日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DBoundingSphere
{
public:
	CD3DVector3		m_Center;
	FLOAT			m_Radius;
public:
	CD3DBoundingSphere();
	CD3DBoundingSphere(const CD3DBoundingSphere& Value);
	CD3DBoundingSphere(const CD3DVector3& Center,FLOAT Radius);
	~CD3DBoundingSphere(){}

	void ComputeFromVertex(LPVOID pVertext,int VertexCount,int VertexSize);
	void AppendFromVertex(LPVOID pVertext,int VertexCount,int VertexSize);

	CD3DBoundingSphere operator+(const CD3DBoundingSphere& Box);
	CD3DBoundingSphere& operator+=(const CD3DBoundingSphere& Box);
};

inline CD3DBoundingSphere::CD3DBoundingSphere()
{
	m_Center.x=0;
	m_Center.y=0;
	m_Center.z=0;
	m_Radius=0;	
}

inline CD3DBoundingSphere::CD3DBoundingSphere(const CD3DBoundingSphere& Value)
{
	m_Center=Value.m_Center;
	m_Radius=Value.m_Radius;
}

inline CD3DBoundingSphere::CD3DBoundingSphere(const CD3DVector3& Center,FLOAT Radius)
{
	m_Center=Center;
	m_Radius=Radius;
}

inline void CD3DBoundingSphere::ComputeFromVertex(LPVOID pVertext,int VertexCount,int VertexSize)
{
	D3DXComputeBoundingSphere((D3DXVECTOR3 *)pVertext,VertexCount,VertexSize,&m_Center,&m_Radius);
}
inline void CD3DBoundingSphere::AppendFromVertex(LPVOID pVertext,int VertexCount,int VertexSize)
{
	CD3DVector3 Center;
	FLOAT Radius;
	

	D3DXComputeBoundingSphere((D3DXVECTOR3 *)pVertext,VertexCount,VertexSize,&Center,&Radius);

	CD3DVector3 BCenter,SCenter;
	FLOAT BRadius,SRadius;
	FLOAT Len;
	if(m_Radius>Radius)
	{
		BCenter=m_Center;
		SCenter=Center;
		BRadius=m_Radius;
		SRadius=Radius;
	}
	else
	{
		BCenter=Center;
		SCenter=m_Center;
		BRadius=Radius;
		SRadius=m_Radius;
	}
	
	Len=sqrt((BCenter.x-SCenter.x)*(BCenter.x-SCenter.x)+
		(BCenter.y-SCenter.y)*(BCenter.y-SCenter.y)+
		(BCenter.z-SCenter.z)*(BCenter.z-SCenter.z));

	if(BRadius>=SRadius+Len)
	{
		m_Center=BCenter;
		m_Radius=BRadius;
	}
	else
	{
		m_Radius=(BRadius+Len+SRadius)/2;

		FLOAT s=(Len+SRadius-BRadius)/Len;
		D3DXVec3Lerp(&m_Center,&BCenter,&SCenter,s);
	}
}

inline CD3DBoundingSphere CD3DBoundingSphere::operator+(const CD3DBoundingSphere& Box)
{
	CD3DVector3 Center;
	FLOAT Radius;
	CD3DVector3 BCenter,SCenter;
	FLOAT BRadius,SRadius;
	FLOAT Len;
	if(m_Radius>Box.m_Radius)
	{
		BCenter=m_Center;
		SCenter=Box.m_Center;
		BRadius=m_Radius;
		SRadius=Box.m_Radius;
	}
	else
	{
		BCenter=Box.m_Center;
		SCenter=m_Center;
		BRadius=Box.m_Radius;
		SRadius=m_Radius;
	}

	Len=sqrt((BCenter.x-SCenter.x)*(BCenter.x-SCenter.x)+
		(BCenter.y-SCenter.y)*(BCenter.y-SCenter.y)+
		(BCenter.z-SCenter.z)*(BCenter.z-SCenter.z));

	if(BRadius>=SRadius+Len)
	{
		Center=BCenter;
		Radius=BRadius;
	}
	else
	{
		Radius=(BRadius+Len+SRadius)/2;

		FLOAT s=(Len+SRadius-BRadius)/Len;
		D3DXVec3Lerp(&Center,&BCenter,&SCenter,s);
	}
	return CD3DBoundingSphere(Center,Radius);

}

inline CD3DBoundingSphere& CD3DBoundingSphere::operator+=(const CD3DBoundingSphere& Box)
{
	CD3DVector3 BCenter,SCenter;
	FLOAT BRadius,SRadius;
	FLOAT Len;
	if(m_Radius>Box.m_Radius)
	{
		BCenter=m_Center;
		SCenter=Box.m_Center;
		BRadius=m_Radius;
		SRadius=Box.m_Radius;
	}
	else
	{
		BCenter=Box.m_Center;
		SCenter=m_Center;
		BRadius=Box.m_Radius;
		SRadius=m_Radius;
	}

	Len=sqrt((BCenter.x-SCenter.x)*(BCenter.x-SCenter.x)+
		(BCenter.y-SCenter.y)*(BCenter.y-SCenter.y)+
		(BCenter.z-SCenter.z)*(BCenter.z-SCenter.z));

	if(BRadius>=SRadius+Len)
	{
		m_Center=BCenter;
		m_Radius=BRadius;
	}
	else
	{
		m_Radius=(BRadius+Len+SRadius)/2;

		FLOAT s=(Len+SRadius-BRadius)/Len;
		D3DXVec3Lerp(&m_Center,&BCenter,&SCenter,s);
	}
	return *this;
}

}