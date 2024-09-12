/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DGUITexture.h                                          */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace D3DGUI{
class CD3DGUITexture :
	public CD3DTexture,public IUITexture
{
protected:
	DECLARE_CLASS_INFO(CD3DGUITexture)
public:
	CD3DGUITexture();
	CD3DGUITexture(CD3DTextureManager * pManager);
	virtual ~CD3DGUITexture(void);

	virtual CEasyString GetName()
	{
		return CD3DTexture::GetName();
	}
	virtual int GetWidth()
	{
		return CD3DTexture::GetWidth();
	}
	virtual int GetHeight()
	{
		return CD3DTexture::GetHeight();
	}
	virtual FLOAT X2U(int X)
	{
		return ((FLOAT)X+0.5f)/(m_TextureSurfaceInfo.Width);
	}
	virtual FLOAT Y2V(int Y)
	{
		return ((FLOAT)Y+0.5f)/(m_TextureSurfaceInfo.Height);
	}
	virtual int U2X(FLOAT U)
	{
		return (int)(U*m_TextureSurfaceInfo.Width);
	}
	virtual int V2X(FLOAT V)
	{
		return (int)(V*m_TextureSurfaceInfo.Height);
	}

	
	virtual void PickResource(CUSOResourceManager * pResourceManager,UINT Param=0)
	{
		pResourceManager->AddResource((CD3DTexture *)this);
	}

	void AddUseRef()
	{
		CD3DTexture::AddUseRef();
	}

	virtual void Release()
	{
		CD3DTexture::Release();
	}

};

}