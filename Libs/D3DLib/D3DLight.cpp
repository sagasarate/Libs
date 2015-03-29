/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DLight.cpp                                             */
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

IMPLEMENT_CLASS_INFO(CD3DLight,CD3DObject);

CD3DLight::CD3DLight(void):CD3DObject()
{
	const D3DCOLORVALUE WhiteColor={1.0f,1.0f,1.0f,1.0f};	
	const D3DCOLORVALUE GrayColor={0.5f,0.5f,0.5f,1.0f};	
	const D3DCOLORVALUE BlackColor={0.0f,0.0f,0.0f,1.0f};
	m_LightData.Type=D3DLIGHT_DIRECTIONAL;
	m_LightData.Ambient=WhiteColor;
	m_LightData.Diffuse=WhiteColor;
	m_LightData.Specular=WhiteColor;
	m_LightData.Position.x=0.0f;
	m_LightData.Position.y=0.0f;
	m_LightData.Position.z=0.0f;
	m_LightData.Direction.x=1.0f;
	m_LightData.Direction.y=1.0f;
	m_LightData.Direction.z=1.0f;
	m_LightData.Range=sqrt(FLT_MAX);
	m_LightData.Falloff=1.0f;
	m_LightData.Attenuation0=0.0f;
	m_LightData.Attenuation1=1.0f;
	m_LightData.Attenuation2=0.0f;
	m_LightData.Theta=PI/8;
	m_LightData.Phi=PI/6;	

	m_BoundingBox.m_Min.SetValue(-0.2f,-0.2f,0.0f);
	m_BoundingBox.m_Max.SetValue(0.2f,0.2f,1.0f);
}

CD3DLight::CD3DLight(const CD3DLight& Light)
{
	m_LightData=Light.m_LightData;
	m_LocalMatrix=Light.m_LocalMatrix;
	m_WorldMatrix=Light.m_WorldMatrix;

	m_BoundingBox.m_Min.SetValue(-0.5f,0.0f,-0.5f);
	m_BoundingBox.m_Max.SetValue(0.5f,1.0f,0.5f);
}

CD3DLight::CD3DLight(const D3DLIGHT9& Light)
{
	m_LightData=Light;

	m_BoundingBox.m_Min.SetValue(-0.5f,0.0f,-0.5f);
	m_BoundingBox.m_Max.SetValue(0.5f,1.0f,0.5f);
}

CD3DLight::~CD3DLight(void)
{
	Destory();
}

void CD3DLight::Destory()
{
	if(m_pRender)
		m_pRender->DelObject(this);
	if(m_pRender)
		m_pRender->DelRootObject(this);

	m_pRender=NULL;

	CD3DObject::Destory();
}

void CD3DLight::SetLight(const D3DLIGHT9& Light)
{
	m_LightData=Light;
	CD3DVector3 EndVec=Light.Direction;
	EndVec.Normalize();
	CD3DVector3 StartVec=LIGHT_START_DIR;

	CD3DVector3 RotateAix=StartVec.Cross(EndVec);
	RotateAix.Normalize();

	FLOAT RotateAngle=acosf(StartVec.Dot(EndVec));

	CD3DQuaternion Rotation=CD3DQuaternion::FromRotationAxis(RotateAix,RotateAngle);
	
	CD3DMatrix Mat=CD3DMatrix::FromRotationQuaternion(Rotation);
	Mat.SetTranslation(Light.Position);
	SetLocalMatrix(Mat);

	
	//Update(0);
}

D3DLIGHT9& CD3DLight::GetLight()
{
	return m_LightData;
}

void CD3DLight::GetCurLight(D3DLIGHT9& Light)
{
	Light=m_LightData;
}

void CD3DLight::SetDirect(FLOAT x,FLOAT y,FLOAT z)
{
	CD3DVector3 EndVec(x,y,z);
	EndVec.Normalize();
	CD3DVector3 StartVec=LIGHT_START_DIR;

	CD3DVector3 RotateAix=StartVec.Cross(EndVec);
	RotateAix.Normalize();

	FLOAT RotateAngle=acosf(StartVec.Dot(EndVec));

	CD3DQuaternion Rotation=CD3DQuaternion::FromRotationAxis(RotateAix,RotateAngle);
	
	CD3DMatrix Mat=GetLocalMatrix();
	Mat.SetRotation(CD3DMatrix::FromRotationQuaternion(Rotation));
	SetLocalMatrix(Mat);
	//Update(0);
}

void CD3DLight::SetPosition(FLOAT x,FLOAT y,FLOAT z)
{
	CD3DMatrix Mat=GetLocalMatrix();
	Mat.SetTranslation(x,y,z);
	SetLocalMatrix(Mat);
	//Update(0);
}

void CD3DLight::Apply(CD3DDevice * pDevice,int Index)
{
	if(pDevice)
	{
		D3DLIGHT9 Light;
		GetCurLight(Light);
		pDevice->GetD3DDevice()->SetLight(Index,&Light);
	}
}

void CD3DLight::Update(FLOAT Time)
{
	CD3DObject::Update(Time);
	m_LightData.Position=GetWorldMatrix().GetTranslation();
	m_LightData.Direction=LIGHT_START_DIR*GetWorldMatrix().GetRotation();
}

bool CD3DLight::RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly)
{
	if(GetBoundingBox())
	{
		CD3DBoundingBox BBox;


		BBox=(*GetBoundingBox())*GetWorldMatrix();

		return BBox.RayIntersect(Point,Dir,IntersectPoint,Distance,TestOnly);
	}
	
	return false;
}

CD3DBoundingBox * CD3DLight::GetBoundingBox()
{
	return &m_BoundingBox;
}

void CD3DLight::ShowBoundingFrame(int Operator)
{
	switch(Operator)
	{
	case DBFO_HIDE:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(false);
		}
		break;
	case DBFO_SHOW:
		{
			if(m_pBoundingFrame)
				m_pBoundingFrame->SetVisible(true);
			else
			{
				if(GetRender()==NULL)
					return;				
				m_pBoundingFrame=new CD3DBoundingFrame();
				m_pBoundingFrame->SetRender(GetRender());
				m_pBoundingFrame->CreateFromLight(this);
				m_pBoundingFrame->SetParent(this);
				GetRender()->AddObject(m_pBoundingFrame);
			}
		}
		break;
	case DBFO_RELEASE:
		SAFE_RELEASE(m_pBoundingFrame);
		break;
	}
}

void CD3DLight::UpdateBoundingFrame()
{
	if(m_pBoundingFrame)
	{
		m_pBoundingFrame->CreateFromLight(this);
	}
}

}