/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DDevice.h                                              */
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

class CD3DTextureManager;
class CD3DFXManager;
class CD3DFontManager;
class CD3DTexture;
class CD3DObjectResourceManager;
class CD3DObjectManager;
class CD3DSwapChain;

struct D3DDEVICE_PARAMS
{
	UINT					Adapter;
	D3DDEVTYPE				DeviceType;
	HWND					hFocusWindow;
	DWORD					BehaviorFlags;
	D3DPRESENT_PARAMETERS	PresentParams;
	bool					IsUseMultiThreadRender;
	D3DDEVICE_PARAMS()
	{
		Adapter=D3DADAPTER_DEFAULT;
		DeviceType=D3DDEVTYPE_HAL;
		hFocusWindow=NULL;
		BehaviorFlags=D3DCREATE_MIXED_VERTEXPROCESSING;
		ZeroMemory(&PresentParams,sizeof(PresentParams));
		PresentParams.Windowed = TRUE;	
		PresentParams.SwapEffect =D3DSWAPEFFECT_DISCARD;
		PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
		PresentParams.EnableAutoDepthStencil = TRUE;
		PresentParams.AutoDepthStencilFormat = D3DFMT_D16;
		PresentParams.PresentationInterval=D3DPRESENT_INTERVAL_IMMEDIATE;
		IsUseMultiThreadRender=false;
	}

};

class CD3DDevice :
	public CNameObject
{
protected:
	LPDIRECT3D9						m_pD3D;
	LPDIRECT3DDEVICE9				m_pd3dDevice;

	CEasyArray<LPDIRECT3DSURFACE9>	m_DefaultRenderTargetList;
	LPDIRECT3DSURFACE9				m_pDefaultDepthStencilBuffer;
	
	D3DDEVICE_PARAMS				m_D3DParams;
	D3DCAPS9						m_D3DCaps;

	CD3DTextureManager *			m_pTextureManager;	
	CD3DFXManager *					m_pFXManager;
	CD3DFontManager	*				m_pFontManager;
	CD3DObjectResourceManager * 	m_pObjectResourceManager;

	static bool						m_IsUseMultiThreadRender;

	DECLARE_CLASS_INFO(CD3DDevice)
public:
	CD3DDevice();
	virtual ~CD3DDevice(void);

	bool Create(D3DDEVICE_PARAMS& Params);
	bool Reset();

	LPDIRECT3D9 GetD3D()
	{
		return m_pD3D;
	}

	LPDIRECT3DDEVICE9 GetD3DDevice()
	{return m_pd3dDevice;}

	HWND GetBindHwnd()
	{return m_D3DParams.hFocusWindow;}

	D3DCAPS9& GetDeviceCaps()
	{
		return m_D3DCaps;
	}
	D3DDEVICE_PARAMS& GetCreateParam()
	{
		return m_D3DParams;
	}

	virtual bool RestoreObject();	
	virtual bool ReleaseObject();
	
	
	inline CD3DTextureManager * GetTextureManager()
	{
		return m_pTextureManager;
	}

	inline CD3DFXManager * GetFXManager()
	{
		return m_pFXManager;
	}

	inline CD3DFontManager * GetFontManager()
	{
		return m_pFontManager;
	}
	inline CD3DObjectResourceManager * GetObjectResourceManager()
	{
		return m_pObjectResourceManager;
	}	

	bool Clear(D3DCOLOR ClearColor,DWORD Flag=D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);

	bool StartRender();
	bool StartRender(D3DCOLOR ClearColor,UINT ClearFlag=D3DCLEAR_TARGET|D3DCLEAR_ZBUFFER);
	void EndRender(bool WantPresent=true);

	UINT GetRenderTargetCount()
	{
		return m_DefaultRenderTargetList.GetCount();
	}
	bool SetRenderTarget(UINT Index,LPDIRECT3DSURFACE9 pRenderTarget);
	bool SetRenderTarget(UINT Index,CD3DTexture * pTexture,UINT SurfaceLevel);
	bool SetRenderTarget(UINT Index,CD3DSwapChain * pSwapChain);
	LPDIRECT3DSURFACE9 GetRenderTarget(UINT Index);
	bool RestoreRenderTargetToDefault(UINT Index);

	bool SetDepthStencilBuffer(LPDIRECT3DSURFACE9 pDepthStencilBuffer);
	bool RestoreDepthStencilBufferToDefault();

	bool SetViewPort(RECT& Rect);

	bool IsCreated()
	{
		return m_pd3dDevice!=NULL;
	}

	bool CheckDeviceFormat(DWORD CheckFormat,DWORD Usage=0,DWORD RType=D3DRTYPE_TEXTURE);

	static bool IsUseMultiThreadRender()
	{
		return m_IsUseMultiThreadRender;
	}

	int Update(FLOAT Time);

};

}