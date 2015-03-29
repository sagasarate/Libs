/****************************************************************************/
/*                                                                          */
/*      文件名:    D3DDevice.cpp                                            */
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

IMPLEMENT_CLASS_INFO(CD3DDevice,CNameObject);


bool CD3DDevice::m_IsUseMultiThreadRender=false;

CD3DDevice::CD3DDevice():CNameObject()
{
	m_pD3D=NULL;
	m_pd3dDevice=NULL;	
	m_pDefaultDepthStencilBuffer=NULL;
	m_pTextureManager=NULL;
	m_pFXManager=NULL;
	m_pFontManager=NULL;
	m_pObjectResourceManager=NULL;	
	ZeroMemory( &m_D3DParams, sizeof(m_D3DParams));
	ZeroMemory(&m_D3DCaps,sizeof(m_D3DCaps));
}

CD3DDevice::~CD3DDevice(void)
{
	ReleaseObject();
}

bool CD3DDevice::Create(D3DDEVICE_PARAMS& Params)
{
	ReleaseObject();

	HRESULT hr;
	
	m_D3DParams=Params;

	if(m_D3DParams.PresentParams.hDeviceWindow==NULL)
		m_D3DParams.PresentParams.hDeviceWindow=m_D3DParams.hFocusWindow;
	

    if( NULL == ( m_pD3D = Direct3DCreate9( D3D_SDK_VERSION ) ) )
        return false;     
	
	if(m_D3DParams.IsUseMultiThreadRender)
	{
		m_IsUseMultiThreadRender=true;
		m_D3DParams.BehaviorFlags|=D3DCREATE_MULTITHREADED;
	}

	hr=m_pD3D->CreateDevice(m_D3DParams.Adapter,m_D3DParams.DeviceType,m_D3DParams.hFocusWindow,m_D3DParams.BehaviorFlags,&m_D3DParams.PresentParams,&m_pd3dDevice);
    if( FAILED( hr ))
    {
        return false;
    }

	m_pd3dDevice->GetDeviceCaps(&m_D3DCaps);

	if(m_D3DCaps.NumSimultaneousRTs==0)
	{
		PrintD3DLog(0,_T("异常，RenderTarget数量为0"));
		return false;
	}

	m_DefaultRenderTargetList.Resize(m_D3DCaps.NumSimultaneousRTs);

	for(UINT i=0;i<m_D3DCaps.NumSimultaneousRTs;i++)
	{
		m_pd3dDevice->GetRenderTarget(i,&m_DefaultRenderTargetList[i]);
	}	
	m_pd3dDevice->GetDepthStencilSurface(&m_pDefaultDepthStencilBuffer);

	m_pTextureManager=new CD3DTextureManager(this,MAX_TEXTURE_STORAGE);	
	m_pFXManager=new CD3DFXManager(this,MAX_FX_STORAGE);
	m_pFontManager=new CD3DFontManager(this,MAX_FONT_STORAGE);
	m_pObjectResourceManager=new CD3DObjectResourceManager(this,MAX_FONT_STORAGE);
	return true;
}

bool CD3DDevice::Reset()
{
	for(UINT i=0;i<m_DefaultRenderTargetList.GetCount();i++)
	{
		SAFE_RELEASE(m_DefaultRenderTargetList[i]);
	}	
	SAFE_RELEASE(m_pDefaultDepthStencilBuffer);
	if(m_D3DParams.PresentParams.Windowed)
	{
		 m_D3DParams.PresentParams.BackBufferFormat = D3DFMT_UNKNOWN;
		 m_D3DParams.PresentParams.BackBufferWidth = 0;
		 m_D3DParams.PresentParams.BackBufferHeight = 0;
	}	
	HRESULT hr=m_pd3dDevice->Reset(&m_D3DParams.PresentParams);

	for(UINT i=0;i<m_D3DCaps.NumSimultaneousRTs;i++)
	{
		m_pd3dDevice->GetRenderTarget(i,&m_DefaultRenderTargetList[i]);
	}	
	m_pd3dDevice->GetDepthStencilSurface(&m_pDefaultDepthStencilBuffer);
	return SUCCEEDED(hr);
}

bool CD3DDevice::RestoreObject()
{
	return true;
}

bool CD3DDevice::ReleaseObject()
{	
	SAFE_RELEASE(m_pObjectResourceManager);
	SAFE_RELEASE(m_pTextureManager);	
	SAFE_RELEASE(m_pFontManager);		
	SAFE_RELEASE(m_pFXManager);	
	for(UINT i=0;i<m_DefaultRenderTargetList.GetCount();i++)
	{
		SAFE_RELEASE(m_DefaultRenderTargetList[i]);
	}	
	SAFE_RELEASE(m_pDefaultDepthStencilBuffer);
	SAFE_RELEASE(m_pd3dDevice);
	SAFE_RELEASE(m_pD3D);
	
	return true;
}

bool CD3DDevice::Clear(D3DCOLOR ClearColor,DWORD Flag)
{
	if(m_pd3dDevice)
	{
		if(SUCCEEDED(m_pd3dDevice->Clear( 0, NULL, Flag,
			ClearColor, 1.0f, 0 )))
		{
			return true;
		}
	}
	return false;

}

bool CD3DDevice::StartRender()
{
	if(m_pd3dDevice)
	{		
		if(SUCCEEDED(m_pd3dDevice->BeginScene()))
			return true;
	}
	return false;
}

bool CD3DDevice::StartRender(D3DCOLOR ClearColor,UINT ClearFlag)
{
	if(m_pd3dDevice)
	{
		if(m_pd3dDevice->Clear( 0, NULL, ClearFlag, ClearColor, 1.0f, 0 )!=D3D_OK)
			return false;
		if(SUCCEEDED(m_pd3dDevice->BeginScene()))
			return true;
	}
	return false;
}

void CD3DDevice::EndRender(bool WantPresent)
{
	if(m_pd3dDevice)
	{
		m_pd3dDevice->EndScene();
		if(WantPresent)
			m_pd3dDevice->Present( NULL, NULL, NULL, NULL );
	}
}


bool CD3DDevice::SetRenderTarget(UINT Index,LPDIRECT3DSURFACE9 pRenderTarget)
{
	if(Index<m_D3DCaps.NumSimultaneousRTs)
	{
		LPDIRECT3DSURFACE9 pOldRenderTarget=NULL;
		m_pd3dDevice->GetRenderTarget(Index, &pOldRenderTarget);
		if(m_pd3dDevice->SetRenderTarget(Index, pRenderTarget)==D3D_OK)
		{
			SAFE_RELEASE(pOldRenderTarget);
			return true;
		}
		else
		{
			if(pOldRenderTarget)
				m_pd3dDevice->SetRenderTarget(Index, pOldRenderTarget);
		}
	}
	return false;
}

bool CD3DDevice::SetRenderTarget(UINT Index,CD3DTexture * pTexture,UINT SurfaceLevel)
{
	LPDIRECT3DSURFACE9 pRenderTarget=NULL;
	if(pTexture->GetD3DTexture()->GetSurfaceLevel(SurfaceLevel, &pRenderTarget)==D3D_OK)
	{
		bool Ret=SetRenderTarget(Index,pRenderTarget);
		SAFE_RELEASE(pRenderTarget);
		return Ret;
	}
	return false;
}

bool CD3DDevice::SetRenderTarget(UINT Index,CD3DSwapChain * pSwapChain)
{
	LPDIRECT3DSURFACE9 pBackBuffer=pSwapChain->GetBackBuffer(0);
	if(pBackBuffer)
	{
		bool Ret=SetRenderTarget(Index,pBackBuffer);
		SAFE_RELEASE(pBackBuffer);
		return Ret;
	}
	return false;	
}

LPDIRECT3DSURFACE9 CD3DDevice::GetRenderTarget(UINT Index)
{
	LPDIRECT3DSURFACE9 pRenderTarget=NULL;
	if(Index<m_D3DCaps.NumSimultaneousRTs)
	{
		m_pd3dDevice->GetRenderTarget(Index, &pRenderTarget);
	}
	return pRenderTarget;
}

bool CD3DDevice::RestoreRenderTargetToDefault(UINT Index)
{
	return SetRenderTarget(Index,m_DefaultRenderTargetList[Index]);
}

bool CD3DDevice::SetDepthStencilBuffer(LPDIRECT3DSURFACE9 pDepthStencilBuffer)
{
	LPDIRECT3DSURFACE9 pOldDepthStencilBuffer=NULL;
	if(m_pd3dDevice->GetDepthStencilSurface(&pOldDepthStencilBuffer)==D3D_OK)
	{
		if(m_pd3dDevice->SetDepthStencilSurface(pDepthStencilBuffer)==D3D_OK)
		{
			SAFE_RELEASE(pOldDepthStencilBuffer);
			return true;
		}
		else
		{
			m_pd3dDevice->SetDepthStencilSurface(pOldDepthStencilBuffer);
		}
	}
	return false;
}
bool CD3DDevice::RestoreDepthStencilBufferToDefault()
{
	return SetDepthStencilBuffer(m_pDefaultDepthStencilBuffer);
}


bool CD3DDevice::SetViewPort(RECT& Rect)
{
	if(m_pd3dDevice)
	{
		D3DVIEWPORT9 vp;
		vp.X      = Rect.left;
		vp.Y      = Rect.top;
		vp.Width  = Rect.right-Rect.left;
		vp.Height = Rect.bottom-Rect.top;
		vp.MinZ   = 0.0f;
		vp.MaxZ   = 1.0f;


		if(SUCCEEDED(m_pd3dDevice->SetViewport(&vp)))
			return true;			
	}	
	return false;
}

bool CD3DDevice::CheckDeviceFormat(DWORD CheckFormat,DWORD Usage,DWORD RType)
{
	return SUCCEEDED(m_pD3D->CheckDeviceFormat(m_D3DParams.Adapter,m_D3DParams.DeviceType,
		m_D3DParams.PresentParams.BackBufferFormat,
		Usage,(D3DRESOURCETYPE)RType,(D3DFORMAT)CheckFormat));
}


int CD3DDevice::Update(FLOAT Time)
{
	int ProcessCount=0;

	ProcessCount+=m_pTextureManager->Update(Time);
	ProcessCount+=m_pFXManager->Update(Time);

	return ProcessCount;
}

}