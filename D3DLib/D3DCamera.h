/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DCamera.h                                              */
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

enum D3DCAMERA_APPLY_MODE
{
	D3DCAMERA_APPLY_VIEW=1,
	D3DCAMERA_APPLY_PROJECT=2,
	D3DCAMERA_APPLY_ALL=3,
};

class CD3DCamera :
	public CD3DObject
{
protected:	
	CD3DMatrix		m_ViewMatrix;
	CD3DMatrix		m_ProjectMatrix;
	FLOAT			m_Near;
	FLOAT			m_Far;
	CD3DFrustum		m_Frustum;

	CD3DMatrix		m_ViewMatrixR;
	CD3DMatrix		m_ProjectMatrixR;
	CD3DFrustum		m_FrustumR;
	
	//struct STORAGE_STRUCT:public CD3DObject::STORAGE_STRUCT
	//{
	//	CD3DMatrix ProjMatrix;
	//};

	DECLARE_CLASS_INFO(CD3DCamera)

public:
	CD3DCamera();
	virtual ~CD3DCamera(void);

	virtual void Destory();


	virtual bool CanRender();

	void SetViewMat(const CD3DMatrix& Mat);
	void SetProjectMat(const CD3DMatrix& Mat);

	CD3DMatrix& GetViewMat();
	CD3DMatrix& GetProjectMat();	
	CD3DFrustum& GetFrustum();

	CD3DMatrix& GetViewMatR();
	CD3DMatrix& GetProjectMatR();	
	CD3DFrustum& GetFrustumR();

	void LookAt(const CD3DVector3& Eye,const CD3DVector3& At,const CD3DVector3& Up);

	void SetProjectPerspective(FLOAT Fov,FLOAT Aspect,FLOAT Near,FLOAT Far);
	void SetProjectOrtho(FLOAT Width,FLOAT Height,FLOAT Near,FLOAT Far);

	
	void SetPos(const CD3DVector3& Pos);
	void SetRotation(FLOAT Yaw,FLOAT Pitch,FLOAT Roll);

	void Apply(CD3DDevice* pDevice,int Mode);

	void GetPickRay(FLOAT ScreenX,FLOAT ScreenY,CD3DVector3& Point,CD3DVector3& Dir);
	void GetPickRay(int ScreenX,int ScreenY,int ScreenWidth,int ScreenHeight,CD3DVector3& Point,CD3DVector3& Dir);

	FLOAT GetNear();
	FLOAT GetFar();

	virtual void OnPrepareRenderData();
	virtual void Update(FLOAT Time);


protected:
	//virtual CNameObject::STORAGE_STRUCT * USOCreateHead(UINT Param=0);
	//virtual int USOWriteHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	//virtual int USOReadHead(CNameObject::STORAGE_STRUCT * pHead,CUSOFile * pResourceManager,UINT Param=0);
	
};

inline bool CD3DCamera::CanRender()
{
	return false;
}

inline void CD3DCamera::SetViewMat(const CD3DMatrix& Mat)
{
	SetLocalMatrix(Mat.GetInverse());
}
inline void CD3DCamera::SetProjectMat(const CD3DMatrix& Mat)
{
	m_ProjectMatrix=Mat;
	CD3DMatrix Invert=Mat;
	Invert.Invert();
	CD3DVector3 Near(0,0,0);
	CD3DVector3 Far(0,0,1);
	Near=Near*Invert;
	Far=Far*Invert;
	m_Near=Near.z;
	m_Far=Far.z;
}

inline CD3DMatrix& CD3DCamera::GetViewMat()
{
	return m_ViewMatrix;
}
inline CD3DMatrix& CD3DCamera::GetProjectMat()
{
	return m_ProjectMatrix;
}
inline CD3DFrustum& CD3DCamera::GetFrustum()
{
	return m_Frustum;
}

inline CD3DMatrix& CD3DCamera::GetViewMatR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_ViewMatrixR;
	else
		return m_ViewMatrix;
}
inline CD3DMatrix& CD3DCamera::GetProjectMatR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_ProjectMatrixR;
	else
		return m_ProjectMatrix;
}
inline CD3DFrustum& CD3DCamera::GetFrustumR()
{
	if(CD3DDevice::IsUseMultiThreadRender())
		return m_FrustumR;
	else
		return m_Frustum;
}

inline void CD3DCamera::LookAt(const CD3DVector3& Eye,const CD3DVector3& At,const CD3DVector3& Up)
{
	//视矩阵由一个标准视矩阵和一个变换矩阵组成

	m_ViewMatrix=CD3DMatrix::FromLookAtLH(Eye,At,Up);
	m_LocalMatrix=m_ViewMatrix.GetInverse();
}

inline void CD3DCamera::SetProjectPerspective(FLOAT Fov,FLOAT Aspect,FLOAT Near,FLOAT Far)
{
	m_ProjectMatrix=CD3DMatrix::FromPerspectiveFovLH(Fov,Aspect,Near,Far);
	m_Near=Near;
	m_Far=Far;
}
inline void CD3DCamera::SetProjectOrtho(FLOAT Width,FLOAT Height,FLOAT Near,FLOAT Far)
{
	m_ProjectMatrix=CD3DMatrix::FromOrthoLH(Width,Height,Near,Far);
}


inline void CD3DCamera::SetPos(const CD3DVector3& Pos)
{
	m_LocalMatrix.SetTranslation(Pos);
}
inline void CD3DCamera::SetRotation(FLOAT Yaw,FLOAT Pitch,FLOAT Roll)
{
	m_LocalMatrix.SetRotation(CD3DMatrix::FromRotationYawPitchRoll(Yaw,Pitch,Roll));
}

inline void CD3DCamera::Apply(CD3DDevice* pDevice,int Mode)
{
	if(pDevice)
	{
		if(CD3DDevice::IsUseMultiThreadRender())
		{
			if(Mode&D3DCAMERA_APPLY_VIEW)
				pDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW,&m_ViewMatrixR);
			if(Mode&D3DCAMERA_APPLY_PROJECT)
				pDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&m_ProjectMatrixR);
		}
		else
		{
			if(Mode&D3DCAMERA_APPLY_VIEW)
				pDevice->GetD3DDevice()->SetTransform(D3DTS_VIEW,&m_ViewMatrix);
			if(Mode&D3DCAMERA_APPLY_PROJECT)
				pDevice->GetD3DDevice()->SetTransform(D3DTS_PROJECTION,&m_ProjectMatrix);
		}
	}

}

inline FLOAT CD3DCamera::GetNear()
{
	return m_Near;
}
inline FLOAT CD3DCamera::GetFar()
{
	return m_Far;
}

}