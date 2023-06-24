/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DQuaternion.h                                          */
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

class CD3DQuaternion :
	public D3DXQUATERNION
{
public:
	CD3DQuaternion():D3DXQUATERNION(){}
	CD3DQuaternion(const FLOAT * pData):D3DXQUATERNION(pData){}
	CD3DQuaternion(const D3DXFLOAT16 * pData):D3DXQUATERNION(pData){}
	CD3DQuaternion(FLOAT x, FLOAT y, FLOAT z, FLOAT w):D3DXQUATERNION(x,y,z,w){}
	CD3DQuaternion(const D3DXQUATERNION& Quaternion);
	
	
	~CD3DQuaternion(){}

	//单位化
	void SetIdentity();
	CD3DQuaternion GetIdentity();
	BOOL IsIdentity();

	//反转
	void Inverse();
	CD3DQuaternion GetInverse();

	//规格化
	void Normalize();
	CD3DQuaternion GetNormalize();

	//长度
	FLOAT Length();
	FLOAT LengthSq();

	//共轭
	void Conjugate();
	CD3DQuaternion GetConjugate();

	//获取欧拉角
	void GetYawPitchRoll(FLOAT& Yaw,FLOAT& Pitch,FLOAT& Roll,bool ReprojectAxis=true);
	//获取旋转轴旋转角
	void GetAxisAngle(D3DXVECTOR3& Axis,FLOAT& Angle);

	static CD3DQuaternion FromRotationAxis(D3DXVECTOR3& Axis,FLOAT Angle);
	static CD3DQuaternion FromRotationMatrix(D3DXMATRIX& Mat);
	static CD3DQuaternion FromRotationYawPitchRoll(FLOAT Yaw,FLOAT Pitch,FLOAT Roll);

	//插值
	static CD3DQuaternion SLerp(const D3DXQUATERNION& q1,const D3DXQUATERNION& q2,FLOAT s);
	

};

inline CD3DQuaternion::CD3DQuaternion(const D3DXQUATERNION& Quaternion)
{
	x=Quaternion.x;
	y=Quaternion.y;
	z=Quaternion.z;
	w=Quaternion.w;
}


inline void CD3DQuaternion::SetIdentity()
{
	x=0.0f;
	y=0.0f;
	z=0.0f;
	w=1.0f;
}

inline CD3DQuaternion CD3DQuaternion::GetIdentity()
{
	CD3DQuaternion Quaternion;
	Quaternion.SetIdentity();	
	return Quaternion;
}

inline BOOL CD3DQuaternion::IsIdentity()
{
	return (x==0.0f&&y==0.0f&&z==0.0f&&w==1.0f);		
}


inline void CD3DQuaternion::Inverse()
{
	D3DXQuaternionInverse(this,this);
}

inline CD3DQuaternion CD3DQuaternion::GetInverse()
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionInverse(&Quaternion,this);
	return Quaternion;
}


inline void CD3DQuaternion::Normalize()
{
	D3DXQuaternionNormalize(this,this);
}

inline CD3DQuaternion CD3DQuaternion::GetNormalize()
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionNormalize(&Quaternion,this);
	return Quaternion;
}


inline FLOAT CD3DQuaternion::Length()
{
	return D3DXQuaternionLength(this);
}

inline FLOAT CD3DQuaternion::LengthSq()
{
	return D3DXQuaternionLengthSq(this);
}


inline void CD3DQuaternion::Conjugate()
{
	D3DXQuaternionConjugate(this,this);
}

inline CD3DQuaternion CD3DQuaternion::GetConjugate()
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionConjugate(&Quaternion,this);
	return Quaternion;
}


inline void CD3DQuaternion::GetYawPitchRoll(FLOAT& Yaw,FLOAT& Pitch,FLOAT& Roll,bool ReprojectAxis)
{
	if (ReprojectAxis)
	{
		// roll = atan2(localx.y, localx.x)
		// pick parts of xAxis() implementation that we need
		FLOAT fTx  = 2.0f*x;
		FLOAT fTy  = 2.0f*y;
		FLOAT fTz  = 2.0f*z;
		FLOAT fTwz = fTz*w;
		FLOAT fTxy = fTy*x;
		FLOAT fTyy = fTy*y;
		FLOAT fTzz = fTz*z;

		FLOAT fTwx = fTx*w;
		FLOAT fTxx = fTx*x;
		FLOAT fTyz = fTz*y;
		FLOAT fTwy = fTy*w;
		FLOAT fTxz = fTz*x;

		// Vector3(1.0-(fTyy+fTzz), fTxy+fTwz, fTxz-fTwy);
		Yaw=atan2(fTxz+fTwy, 1.0f-(fTxx+fTyy));
		Pitch=atan2(fTyz+fTwx, 1.0f-(fTxx+fTzz));
		Roll=atan2(fTxy+fTwz, 1.0f-(fTyy+fTzz));

	}
	else
	{
		Yaw=asin(-2*(x*z - w*y));
		Pitch=atan2(2*(y*z + w*x), w*w - x*x - y*y + z*z);
		Roll=atan2(2*(x*y + w*z), w*w + x*x - y*y - z*z);
	}

	


	//float matrix[3][3];
	//float cx,sx;
	//float cy,sy,yr;
	//float cz,sz;
	/////////////////////////////////////////////////////////////////////////////////
	//// CONVERT QUATERNION TO MATRIX - I DON'T REALLY NEED ALL OF IT
	//matrix[0][0] = 1.0f - (2.0f * this->y * this->y) - (2.0f * this->z * this->z);
	////	matrix[0][1] = (2.0f * this->x * this->y) - (2.0f * this->w * this->z);
	////	matrix[0][2] = (2.0f * this->x * this->z) + (2.0f * this->w * this->y);
	//matrix[1][0] = (2.0f * this->x * this->y) + (2.0f * this->w * this->z);
	////	matrix[1][1] = 1.0f - (2.0f * this->x * this->x) - (2.0f * this->z * this->z);
	////	matrix[1][2] = (2.0f * this->y * this->z) - (2.0f * this->w * this->x);
	//matrix[2][0] = (2.0f * this->x * this->z) - (2.0f * this->w * this->y);
	//matrix[2][1] = (2.0f * this->y * this->z) + (2.0f * this->w * this->x);
	//matrix[2][2] = 1.0f - (2.0f * this->x * this->x) - (2.0f * this->y * this->y);

	//sy = -matrix[2][0];
	//cy = sqrt(1 - (sy * sy));
	//yr = (float)atan2(sy,cy);
	//Yaw = yr;////(yr * 180.0f) / (float)D3DX_PI;

	//// AVOID DIVIDE BY ZERO ERROR ONLY WHERE Y= +-90 or +-270 
	//// NOT CHECKING cy BECAUSE OF PRECISION ERRORS
	//if (sy != 1.0f && sy != -1.0f)	
	//{
	//	cx = matrix[2][2] / cy;
	//	sx = matrix[2][1] / cy;
	//	Pitch = (float)atan2(sx,cx);///((float)atan2(sx,cx) * 180.0f) / (float)D3DX_PI;	// RAD TO DEG

	//	cz = matrix[0][0] / cy;
	//	sz = matrix[1][0] / cy;
	//	Roll = (float)atan2(sz,cz);///((float)atan2(sz,cz) * 180.0f) / (float)D3DX_PI;	// RAD TO DEG
	//}
	//else
	//{
	//	// SINCE Cos(Y) IS 0, I AM SCREWED.  ADOPT THE STANDARD Z = 0
	//	// I THINK THERE IS A WAY TO FIX THIS BUT I AM NOT SURE.  EULERS SUCK
	//	// NEED SOME MORE OF THE MATRIX TERMS NOW
	//	matrix[1][1] = 1.0f - (2.0f * this->x * this->x) - (2.0f * this->z * this->z);
	//	matrix[1][2] = (2.0f * this->y * this->z) - (2.0f * this->w * this->x);
	//	cx = matrix[1][1];
	//	sx = -matrix[1][2];
	//	Pitch = (float)atan2(sx,cx);////((float)atan2(sx,cx) * 180.0f) / (float)D3DX_PI;	// RAD TO DEG

	//	cz = 1.0f;
	//	sz = 0.0f;
	//	Roll = (float)atan2(sz,cz);///((float)atan2(sz,cz) * 180.0f) / (float)D3DX_PI;	// RAD TO DEG
	//}
	//
}

inline void CD3DQuaternion::GetAxisAngle(D3DXVECTOR3& Axis,FLOAT& Angle)
{
	D3DXQuaternionToAxisAngle(this,&Axis,&Angle);

	//angle = acos(w)*2
	//axis.x = x
	//axis.y = y
	//axis.z = z

}

inline CD3DQuaternion CD3DQuaternion::FromRotationAxis(D3DXVECTOR3& Axis,FLOAT Angle)
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionRotationAxis(&Quaternion,&Axis,Angle);
	return Quaternion;

	//Vector3 u = axis;
	//u.normalize();
	//Real s = Math::rSin(angle/2.f); 
	//x = s*u.x;
	//y = s*u.y;
	//z = s*u.z;
	//w = Math::rCos(angle/2.f); 

}

inline CD3DQuaternion CD3DQuaternion::FromRotationMatrix(D3DXMATRIX& Mat)
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionRotationMatrix(&Quaternion,&Mat);
	return Quaternion;
}

inline CD3DQuaternion CD3DQuaternion::FromRotationYawPitchRoll(FLOAT Yaw,FLOAT Pitch,FLOAT Roll)
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionRotationYawPitchRoll(&Quaternion,Yaw,Pitch,Roll);
	return Quaternion;
}

inline CD3DQuaternion CD3DQuaternion::SLerp(const D3DXQUATERNION& q1,const D3DXQUATERNION& q2,FLOAT s)
{
	CD3DQuaternion Quaternion;
	D3DXQuaternionSlerp(&Quaternion,&q1,&q2,s);
	return Quaternion;
}	

}