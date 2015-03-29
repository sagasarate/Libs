/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DVector2.h                                             */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DLib{

class CD3DVector2 :
	public D3DXVECTOR2
{
public:
	CD3DVector2(void):D3DXVECTOR2(){}
	CD3DVector2(const FLOAT * pData):D3DXVECTOR2(pData){}
	CD3DVector2(const D3DXVECTOR2& Vector):D3DXVECTOR2(Vector){}
	CD3DVector2(const D3DXFLOAT16 * pData):D3DXVECTOR2(pData){}
	CD3DVector2(FLOAT x, FLOAT y):D3DXVECTOR2(x,y){}
	CD3DVector2(FLOAT Value);

	~CD3DVector2(void){}

	void SetValue(FLOAT vx, FLOAT vy);

	//标准化
	void Normalize();
	CD3DVector2 GetNormalize();

	//长度
	FLOAT Length();
	FLOAT LengthSq();

	//点积
	FLOAT Dot(const D3DXVECTOR2& V2);	

	const CD3DVector2& operator=(FLOAT Value);

};

inline CD3DVector2::CD3DVector2(FLOAT Value)
{
	x=Value;
	y=Value;
}

inline void CD3DVector2::SetValue(FLOAT vx, FLOAT vy)
{
	x=vx;
	y=vy;
}

inline void CD3DVector2::Normalize()
{
	D3DXVec2Normalize(this,this);
}

inline CD3DVector2 CD3DVector2::GetNormalize()
{
	CD3DVector2 Vector;
	D3DXVec2Normalize(&Vector,this);
	return Vector;
}

inline FLOAT CD3DVector2::Length()
{
	return D3DXVec2Length(this);
}

inline FLOAT CD3DVector2::LengthSq()
{
	return D3DXVec2LengthSq(this);
}

inline FLOAT CD3DVector2::Dot(const D3DXVECTOR2& V2)
{
	return D3DXVec2Dot(this,&V2);
}

inline const CD3DVector2& CD3DVector2::operator=(FLOAT Value)
{
	x=Value;
	y=Value;
	return *this;
}

}