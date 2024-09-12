/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DLight.h                                               */
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

class CD3DLight :
	public CD3DObject
{
protected:
	
	D3DLIGHT9		m_LightData;
	CD3DBoundingBox m_BoundingBox;

	DECLARE_CLASS_INFO(CD3DLight)
public:
	

	CD3DLight(void);
	CD3DLight(const CD3DLight& Light);
	CD3DLight(const D3DLIGHT9& Light);

	virtual ~CD3DLight(void);

	virtual void Destory();

	bool CanRender();

	void SetLight(const D3DLIGHT9& Light);
	D3DLIGHT9& GetLight();	
	void GetCurLight(D3DLIGHT9& Light);

	void SetDirect(FLOAT x,FLOAT y,FLOAT z);
	void SetPosition(FLOAT x,FLOAT y,FLOAT z);
	void SetAmbient(FLOAT r,FLOAT g,FLOAT b,FLOAT a);
	void SetDiffuse(FLOAT r,FLOAT g,FLOAT b,FLOAT a);
	void SetSpecular(FLOAT r,FLOAT g,FLOAT b,FLOAT a);
	void SetAmbient(D3DCOLOR Color);
	void SetDiffuse(D3DCOLOR Color);
	void SetSpecular(D3DCOLOR Color);

	void Apply(CD3DDevice * pDevice,int Index);
	virtual void Update(FLOAT Time);

	virtual bool RayIntersect(const CD3DVector3& Point,const CD3DVector3& Dir,CD3DVector3& IntersectPoint,FLOAT& Distance,bool TestOnly);
	virtual CD3DBoundingBox * GetBoundingBox();
	virtual void ShowBoundingFrame(int Operator);
	virtual void UpdateBoundingFrame();
};

inline bool CD3DLight::CanRender()
{
	return false;
}




inline void CD3DLight::SetAmbient(FLOAT r,FLOAT g,FLOAT b,FLOAT a)
{
	m_LightData.Ambient.r=r;
	m_LightData.Ambient.g=g;
	m_LightData.Ambient.b=b;
	m_LightData.Ambient.a=a;
}

inline void CD3DLight::SetDiffuse(FLOAT r,FLOAT g,FLOAT b,FLOAT a)
{
	m_LightData.Diffuse.r=r;
	m_LightData.Diffuse.g=g;
	m_LightData.Diffuse.b=b;
	m_LightData.Diffuse.a=a;
}

inline void CD3DLight::SetSpecular(FLOAT r,FLOAT g,FLOAT b,FLOAT a)
{
	m_LightData.Specular.r=r;
	m_LightData.Specular.g=g;
	m_LightData.Specular.b=b;
	m_LightData.Specular.a=a;
}

inline void CD3DLight::SetAmbient(D3DCOLOR Color)
{
	m_LightData.Ambient.a=((FLOAT)((Color>>24)&0xFF))/255.0f;
	m_LightData.Ambient.r=((FLOAT)((Color>>16)&0xFF))/255.0f;
	m_LightData.Ambient.g=((FLOAT)((Color>>8)&0xFF))/255.0f;
	m_LightData.Ambient.b=((FLOAT)((Color)&0xFF))/255.0f;
}
inline void CD3DLight::SetDiffuse(D3DCOLOR Color)
{
	m_LightData.Diffuse.a=((FLOAT)((Color>>24)&0xFF))/255.0f;
	m_LightData.Diffuse.r=((FLOAT)((Color>>16)&0xFF))/255.0f;
	m_LightData.Diffuse.g=((FLOAT)((Color>>8)&0xFF))/255.0f;
	m_LightData.Diffuse.b=((FLOAT)((Color)&0xFF))/255.0f;
}
inline void CD3DLight::SetSpecular(D3DCOLOR Color)
{
	m_LightData.Specular.a=((FLOAT)((Color>>24)&0xFF))/255.0f;
	m_LightData.Specular.r=((FLOAT)((Color>>16)&0xFF))/255.0f;
	m_LightData.Specular.g=((FLOAT)((Color>>8)&0xFF))/255.0f;
	m_LightData.Specular.b=((FLOAT)((Color)&0xFF))/255.0f;
}
}