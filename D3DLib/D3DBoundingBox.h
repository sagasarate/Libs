/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DBoundingBox.h                                         */
/*      创建日期:  2010年02月09日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DBoundingBox
{
public:
	CD3DVector3		m_Min;
	CD3DVector3		m_Max;

	enum RELATION_TYPE
	{
		RELATION_TYPE_OUT,					//分离
		RELATION_TYPE_INCLUDE,				//包含
		RELATION_TYPE_BE_INCLUDE,			//被包含
		RELATION_TYPE_INTERSECT,			//相交
	};
public:
	CD3DBoundingBox();
	CD3DBoundingBox(const CD3DBoundingBox& Box);
	CD3DBoundingBox(FLOAT MinX,FLOAT MinY,FLOAT MinZ,FLOAT MaxX,FLOAT MaxY,FLOAT MaxZ);
	CD3DBoundingBox(D3DXVECTOR3 Min,D3DXVECTOR3 Max);
	~CD3DBoundingBox(){}

	void ComputeFromVertex(LPVOID pVertext,UINT VertexCount,UINT VertexSize);
	void ComputeFromVertexWithTransform(LPVOID pVertext,UINT VertexCount,UINT VertexSize,const CD3DMatrix& TransformMat);
	void AppendFromVertex(LPVOID pVertext,UINT VertexCount,UINT VertexSize);
	void AppendFromVertexWithTransform(LPVOID pVertext,UINT VertexCount,UINT VertexSize,const CD3DMatrix& TransformMat);

	CD3DBoundingBox operator+(const CD3DBoundingBox& Box);
	CD3DBoundingBox& operator+=(const CD3DBoundingBox& Box);

	CD3DVector3 GetBoxPoint(int Index) const;
	CD3DVector3 GetSize() const;
	CD3DVector3 GetCenter() const;

	CD3DBoundingBox operator*(const CD3DMatrix& Mat);
	void operator*=(const CD3DMatrix& Mat);

	bool RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly) const;
	bool LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance) const;
	void Rebuild();
	int CheckRelation(const CD3DBoundingBox& BBox) const;
	int CheckRelation(const CD3DVector3& Point) const;
	int CheckRelationRay(const CD3DVector3& Point,const CD3DVector3& Dir) const;
	int CheckRelationLine(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint) const;
	void Merge(const CD3DBoundingBox& BBox);
	bool IsValid();
};

inline CD3DBoundingBox::CD3DBoundingBox()
{
	m_Min.x=0;
	m_Min.y=0;
	m_Min.z=0;
	m_Max.x=0;
	m_Max.y=0;
	m_Max.z=0;
}

inline CD3DBoundingBox::CD3DBoundingBox(const CD3DBoundingBox& Box)
{
	m_Min=Box.m_Min;
	m_Max=Box.m_Max;
}

inline CD3DBoundingBox::CD3DBoundingBox(FLOAT MinX,FLOAT MinY,FLOAT MinZ,FLOAT MaxX,FLOAT MaxY,FLOAT MaxZ)
{
	m_Min.x=MinX;
	m_Min.y=MinY;
	m_Min.z=MinZ;
	m_Max.x=MaxX;
	m_Max.y=MaxY;
	m_Max.z=MaxZ;
}

inline CD3DBoundingBox::CD3DBoundingBox(D3DXVECTOR3 Min,D3DXVECTOR3 Max)
{
	m_Min=Min;
	m_Max=Max;
}

inline void CD3DBoundingBox::ComputeFromVertex(LPVOID pVertext,UINT VertexCount,UINT VertexSize)
{
	D3DXComputeBoundingBox((D3DXVECTOR3 *)pVertext,VertexCount,VertexSize,&m_Min,&m_Max);
}
inline void CD3DBoundingBox::ComputeFromVertexWithTransform(LPVOID pVertext,UINT VertexCount,UINT VertexSize,const CD3DMatrix& TransformMat)
{
	CD3DVector3 * pVertex=(CD3DVector3 *)pVertext;
	m_Min=(*pVertex)*TransformMat;
	m_Max=m_Min;
	for(UINT i=1;i<VertexCount;i++)
	{
		pVertex=(CD3DVector3 *)(((BYTE *)pVertext)+VertexSize*i);
		CD3DVector3 WorldVertex=(*pVertex)*TransformMat;
		if(WorldVertex.x<m_Min.x)
			m_Min.x=WorldVertex.x;
		if(WorldVertex.y<m_Min.y)
			m_Min.y=WorldVertex.y;
		if(WorldVertex.z<m_Min.z)
			m_Min.z=WorldVertex.z;

		if(WorldVertex.x>m_Max.x)
			m_Max.x=WorldVertex.x;
		if(WorldVertex.y>m_Max.y)
			m_Max.y=WorldVertex.y;
		if(WorldVertex.z>m_Max.z)
			m_Max.z=WorldVertex.z;
	}
}
inline void CD3DBoundingBox::AppendFromVertex(LPVOID pVertext,UINT VertexCount,UINT VertexSize)
{
	CD3DVector3 Min,Max;
	D3DXComputeBoundingBox((D3DXVECTOR3 *)pVertext,VertexCount,VertexSize,&Min,&Max);
	if(Min.x<m_Min.x)
		m_Min.x=Min.x;
	if(Min.y<m_Min.y)
		m_Min.y=Min.y;
	if(Min.z<m_Min.z)
		m_Min.z=Min.z;

	if(Max.x>m_Max.x)
		m_Max.x=Max.x;
	if(Max.y>m_Max.y)
		m_Max.y=Max.y;
	if(Max.z>m_Max.z)
		m_Max.z=Max.z;
}

inline void CD3DBoundingBox::AppendFromVertexWithTransform(LPVOID pVertext,UINT VertexCount,UINT VertexSize,const CD3DMatrix& TransformMat)
{
	for(UINT i=0;i<VertexCount;i++)
	{
		CD3DVector3 * pVertex=(CD3DVector3 *)(((BYTE *)pVertext)+VertexSize*i);
		CD3DVector3 WorldVertex=(*pVertex)*TransformMat;
		if(WorldVertex.x<m_Min.x)
			m_Min.x=WorldVertex.x;
		if(WorldVertex.y<m_Min.y)
			m_Min.y=WorldVertex.y;
		if(WorldVertex.z<m_Min.z)
			m_Min.z=WorldVertex.z;

		if(WorldVertex.x>m_Max.x)
			m_Max.x=WorldVertex.x;
		if(WorldVertex.y>m_Max.y)
			m_Max.y=WorldVertex.y;
		if(WorldVertex.z>m_Max.z)
			m_Max.z=WorldVertex.z;
	}
}

inline CD3DBoundingBox CD3DBoundingBox::operator+(const CD3DBoundingBox& Box)
{
	return CD3DBoundingBox(
		Box.m_Min.x<m_Min.x?Box.m_Min.x:m_Min.x,
		Box.m_Min.y<m_Min.y?Box.m_Min.y:m_Min.y,
		Box.m_Min.z<m_Min.z?Box.m_Min.z:m_Min.z,
		Box.m_Max.x>m_Max.x?Box.m_Max.x:m_Max.x,
		Box.m_Max.y>m_Max.y?Box.m_Max.y:m_Max.y,
		Box.m_Max.z>m_Max.z?Box.m_Max.z:m_Max.z
		);

}

inline CD3DBoundingBox& CD3DBoundingBox::operator+=(const CD3DBoundingBox& Box)
{
	if(Box.m_Min.x<m_Min.x)
		m_Min.x=Box.m_Min.x;
	if(Box.m_Min.y<m_Min.y)
		m_Min.y=Box.m_Min.y;
	if(Box.m_Min.z<m_Min.z)
		m_Min.z=Box.m_Min.z;

	if(Box.m_Max.x>m_Max.x)
		m_Max.x=Box.m_Max.x;
	if(Box.m_Max.y>m_Max.y)
		m_Max.y=Box.m_Max.y;
	if(Box.m_Max.z>m_Max.z)
		m_Max.z=Box.m_Max.z;

	return *this;
}

inline CD3DVector3 CD3DBoundingBox::GetBoxPoint(int Index)  const
{
	CD3DVector3 Point;

	Point.x=(Index&1)?m_Min.x:m_Max.x;
	Point.y=(Index&2)?m_Min.y:m_Max.y;
	Point.z=(Index&4)?m_Min.z:m_Max.z;
	return Point;
}

inline CD3DVector3 CD3DBoundingBox::GetSize()  const
{
	return m_Max-m_Min;
}

inline CD3DVector3 CD3DBoundingBox::GetCenter()  const
{
	return (m_Max+m_Min)/2;
}

inline CD3DBoundingBox CD3DBoundingBox::operator*(const CD3DMatrix& Mat)
{
	CD3DVector3	Points[8];

	for(int i=0;i<8;i++)
	{
		Points[i]=GetBoxPoint(i)*Mat;

	}
	CD3DBoundingBox BBox;
	BBox.ComputeFromVertex(Points,8,sizeof(CD3DVector3));
	return BBox;

	//CD3DBoundingBox BBox;
	//BBox.m_Min=m_Min*Mat;
	//BBox.m_Max=m_Max*Mat;
	//if(BBox.m_Min.x>BBox.m_Max.x)
	//	Swap(BBox.m_Min.x,BBox.m_Max.x);
	//if(BBox.m_Min.y>BBox.m_Max.y)
	//	Swap(BBox.m_Min.y,BBox.m_Max.y);
	//if(BBox.m_Min.z>BBox.m_Max.z)
	//	Swap(BBox.m_Min.z,BBox.m_Max.z);
	//return BBox;
}

inline void CD3DBoundingBox::operator*=(const CD3DMatrix& Mat)
{
	CD3DVector3	Points[8];

	for(int i=0;i<8;i++)
	{
		Points[i]=GetBoxPoint(i)*Mat;

	}	
	ComputeFromVertex(Points,8,sizeof(CD3DVector3));

	//m_Min*=Mat;
	//m_Max*=Mat;
	//if(m_Min.x>m_Max.x)
	//	Swap(m_Min.x,m_Max.x);
	//if(m_Min.y>m_Max.y)
	//	Swap(m_Min.y,m_Max.y);
	//if(m_Min.z>m_Max.z)
	//	Swap(m_Min.z,m_Max.z);
}

inline void CD3DBoundingBox::Rebuild()
{
	CD3DVector3	Points[8];

	for(int i=0;i<8;i++)
	{
		Points[i]=GetBoxPoint(i);

	}	
	ComputeFromVertex(Points,8,sizeof(CD3DVector3));
}

inline bool CD3DBoundingBox::LineIntersect(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint,CD3DVector3& IntersectPoint,FLOAT& Distance) const
{
	int R1=CheckRelation(StartPoint);
	int R2=CheckRelation(EndPoint);

	if(R1==RELATION_TYPE_INCLUDE&&R2==RELATION_TYPE_INCLUDE)
		return true;

	CD3DVector3 Dir=(EndPoint-StartPoint);
	FLOAT Len=Dir.Length();
	Dir.Normalize();

	CD3DVector3 P;
	FLOAT D;
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
inline int CD3DBoundingBox::CheckRelation(const CD3DBoundingBox& BBox)  const
{
	if(m_Max.x<BBox.m_Min.x||
		m_Max.y<BBox.m_Min.y||
		m_Max.z<BBox.m_Min.z||
		m_Min.x>BBox.m_Max.x||
		m_Min.y>BBox.m_Max.y||
		m_Min.z>BBox.m_Max.z)
	{
		return RELATION_TYPE_OUT;
	}

	if(m_Max.x>=BBox.m_Max.x&&
		m_Max.y>=BBox.m_Max.y&&
		m_Max.z>=BBox.m_Max.z&&
		m_Min.x<=BBox.m_Min.x&&
		m_Min.y<=BBox.m_Min.y&&
		m_Min.z<=BBox.m_Min.z)
	{
		return RELATION_TYPE_INCLUDE;
	}

	if(m_Max.x<=BBox.m_Max.x&&
		m_Max.y<=BBox.m_Max.y&&
		m_Max.z<=BBox.m_Max.z&&
		m_Min.x>=BBox.m_Min.x&&
		m_Min.y>=BBox.m_Min.y&&
		m_Min.z>=BBox.m_Min.z)
	{
		return RELATION_TYPE_BE_INCLUDE;
	}

	return RELATION_TYPE_INTERSECT;
}

inline int CD3DBoundingBox::CheckRelation(const CD3DVector3& Point)  const
{
	if(Point.x<=m_Max.x&&Point.x>=m_Min.x&&
		Point.y<=m_Max.y&&Point.y>=m_Min.y&&
		Point.z<=m_Max.z&&Point.z>=m_Min.z)
	{
		return RELATION_TYPE_INCLUDE;
	}
	return RELATION_TYPE_OUT;
}

inline int CD3DBoundingBox::CheckRelationRay(const CD3DVector3& Point,const CD3DVector3& Dir) const
{	
	CD3DVector3 IntersectPoint;
	FLOAT Distance;
	if(RayIntersect(Point,Dir,IntersectPoint,Distance,true))
	{		
		return RELATION_TYPE_INTERSECT;
	}
	return RELATION_TYPE_OUT;
}

inline int CD3DBoundingBox::CheckRelationLine(const CD3DVector3& StartPoint,const CD3DVector3& EndPoint) const
{
	if(CheckRelation(StartPoint)==RELATION_TYPE_INCLUDE&&CheckRelation(EndPoint)==RELATION_TYPE_INCLUDE)
	{
		return RELATION_TYPE_INCLUDE;
	}

	CD3DVector3 Dir=EndPoint-StartPoint;
	FLOAT Len=Dir.Length();
	Dir.Normalize();
	CD3DVector3 IntersectPoint;
	FLOAT Distance;
	if(RayIntersect(StartPoint,Dir,IntersectPoint,Distance,false))
	{
		if(Distance<=Len)
			return RELATION_TYPE_INTERSECT;
	}
	return RELATION_TYPE_OUT;
}

inline void CD3DBoundingBox::Merge(const CD3DBoundingBox& BBox)
{
	m_Max.x=max(m_Max.x,BBox.m_Max.x);
	m_Max.y=max(m_Max.y,BBox.m_Max.y);
	m_Max.z=max(m_Max.z,BBox.m_Max.z);
	m_Min.x=min(m_Min.x,BBox.m_Min.x);
	m_Min.y=min(m_Min.y,BBox.m_Min.y);
	m_Min.z=min(m_Min.z,BBox.m_Min.z);
}

inline bool CD3DBoundingBox::IsValid()
{
	return m_Max.IsValid()&&m_Min.IsValid();
}

}