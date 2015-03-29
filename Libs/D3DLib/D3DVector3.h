/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DVector3.h                                             */
/*      创建日期:  2009年09月25日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DVector3 :
	public D3DXVECTOR3
{
public:
	CD3DVector3(void):D3DXVECTOR3(){}
	CD3DVector3(const FLOAT * pData):D3DXVECTOR3(pData){}
	CD3DVector3(const D3DVECTOR& Vector):D3DXVECTOR3(Vector){}
	CD3DVector3(const D3DXFLOAT16 * pData):D3DXVECTOR3(pData){}
	CD3DVector3(FLOAT x, FLOAT y, FLOAT z):D3DXVECTOR3(x,y,z){}
	CD3DVector3(FLOAT Value);

	~CD3DVector3(void){}

	void SetValue(FLOAT vx, FLOAT vy, FLOAT vz);

	//标准化
	void Normalize();
	CD3DVector3 GetNormalize() const;

	//长度
	FLOAT Length() const;
	FLOAT LengthSq() const;

	//点积
	FLOAT Dot(const D3DXVECTOR3& V2) const;

	//叉积
	CD3DVector3 Cross(const D3DXVECTOR3& V2) const;

	//矩阵变换
	CD3DVector3 operator*(const D3DXMATRIX& Mat) const;
	void operator*=(const D3DXMATRIX& Mat);

	CD3DVector3 operator*(FLOAT Len) const;
	void operator*=(FLOAT Len);
	void operator=(FLOAT Value);

	CD3DVector3 operator+(const CD3DVector3& V2) const;
	void operator+=(const CD3DVector3& V2);

	CD3DVector3 operator+(FLOAT Value) const;
	void operator+=(FLOAT Value);

	CD3DVector3 operator-(const CD3DVector3& V2) const;
	void operator-=(const CD3DVector3& V2);

	CD3DVector3 operator-(FLOAT Value) const;
	void operator-=(FLOAT Value);

	CD3DVector3 operator*(const D3DXVECTOR3& V2) const;

	bool IsValid() const;

	//插值
	static CD3DVector3 Lerp(const D3DXVECTOR3& v1,const D3DXVECTOR3& v2,FLOAT s);
};

inline CD3DVector3::CD3DVector3(FLOAT Value)
{
	x=Value;
	y=Value;
	z=Value;
}


inline void CD3DVector3::SetValue(FLOAT vx, FLOAT vy, FLOAT vz)
{
	x=vx;
	y=vy;
	z=vz;
}

inline void CD3DVector3::Normalize()
{
	D3DXVec3Normalize(this,this);
}

inline CD3DVector3 CD3DVector3::GetNormalize() const
{
	CD3DVector3 Vector;
	D3DXVec3Normalize(&Vector,this);
	return Vector;
}

inline FLOAT CD3DVector3::Length() const
{
	return D3DXVec3Length(this);
}

inline FLOAT CD3DVector3::LengthSq() const
{
	return D3DXVec3LengthSq(this);
}

inline FLOAT CD3DVector3::Dot(const D3DXVECTOR3& V2) const
{
	return D3DXVec3Dot(this,&V2);
}


inline CD3DVector3 CD3DVector3::Cross(const D3DXVECTOR3& V2) const
{
	CD3DVector3 RetVec;

	D3DXVec3Cross(&RetVec,this,&V2);

	return RetVec;
}

inline CD3DVector3 CD3DVector3::operator*(const D3DXMATRIX& Mat) const
{
	D3DXVECTOR4 RetVec;
	D3DXVec3Transform(&RetVec,this,&Mat);
	return CD3DVector3(RetVec.x/RetVec.w,RetVec.y/RetVec.w,RetVec.z/RetVec.w);
}

inline void CD3DVector3::operator*=(const D3DXMATRIX& Mat)
{
	*this=*this*Mat;
}

inline CD3DVector3 CD3DVector3::operator*(FLOAT Len) const
{
	return CD3DVector3(x*Len,y*Len,z*Len);
}

inline void CD3DVector3::operator*=(FLOAT Len)
{
	x*=Len;
	y*=Len;
	z*=Len;
}

inline void CD3DVector3::operator=(FLOAT Value)
{
	x=Value;
	y=Value;
	z=Value;
}

inline CD3DVector3 CD3DVector3::operator+(const CD3DVector3& V2) const
{
	CD3DVector3 RetVec;

	D3DXVec3Add(&RetVec,this,&V2);

	return RetVec;
}

inline void CD3DVector3::operator+=(const CD3DVector3& V2)
{
	D3DXVec3Add(this,this,&V2);
}

inline CD3DVector3 CD3DVector3::operator*(const D3DXVECTOR3& V2) const
{
	CD3DVector3 RetVec;
	RetVec.x=x*V2.x;
	RetVec.y=y*V2.y;
	RetVec.z=z*V2.z;

	return RetVec;
}

inline CD3DVector3 CD3DVector3::operator+(FLOAT Value) const
{
	CD3DVector3 RetVec;
	RetVec.x=x+Value;
	RetVec.y=y+Value;
	RetVec.z=z+Value;
	return RetVec;
}
inline void CD3DVector3::operator+=(FLOAT Value)
{
	x+=Value;
	y+=Value;
	z+=Value;
}

inline CD3DVector3 CD3DVector3::operator-(const CD3DVector3& V2) const
{
	CD3DVector3 RetVec;
	RetVec.x=x-V2.x;
	RetVec.y=y-V2.y;
	RetVec.z=z-V2.z;
	return RetVec;
}

inline void CD3DVector3::operator-=(const CD3DVector3& V2)
{
	x-=V2.x;
	y-=V2.y;
	z-=V2.z;
}

inline CD3DVector3 CD3DVector3::operator-(FLOAT Value) const
{
	CD3DVector3 RetVec;
	RetVec.x=x-Value;
	RetVec.y=y-Value;
	RetVec.z=z-Value;
	return RetVec;
}

inline void CD3DVector3::operator-=(FLOAT Value)
{
	x-=Value;
	y-=Value;
	z-=Value;
}

inline bool CD3DVector3::IsValid() const
{
	return _finite(x)&&_finite(y)&&_finite(x);
}

inline CD3DVector3 CD3DVector3::Lerp(const D3DXVECTOR3& v1,const D3DXVECTOR3& v2,FLOAT s)
{
	CD3DVector3 RetVec;

	D3DXVec3Lerp(&RetVec,&v1,&v2,s);

	return RetVec;
}



}