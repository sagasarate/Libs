#include "StdAfx.h"

namespace D3DLib{


IMPLEMENT_CLASS_INFO(CD3DSwapChain,CNameObject);

CD3DSwapChain::CD3DSwapChain(void)
{
	m_pDevice=NULL;
	m_pD3DSwapChain=NULL;
}

CD3DSwapChain::~CD3DSwapChain(void)
{
	Destory();
}

void CD3DSwapChain::Destory()
{
	CNameObject::Destory();
	SAFE_RELEASE(m_pD3DSwapChain);
}

bool CD3DSwapChain::Create(CD3DDevice * pDevice,D3DDEVICE_PARAMS& CreateParams)
{
	Destory();

	m_pDevice=pDevice;
	if(m_pDevice->GetD3DDevice()->CreateAdditionalSwapChain(&CreateParams.PresentParams,&m_pD3DSwapChain)==D3D_OK)
	{
		return true;
	}
	return false;
}

bool CD3DSwapChain::Present(const RECT * pSourceRect,const RECT * pDestRect,HWND hDestWindowOverride,const RGNDATA * pDirtyRegion,DWORD dwFlags)
{
	if(m_pD3DSwapChain)
	{
		if(m_pD3DSwapChain->Present(pSourceRect,pDestRect,hDestWindowOverride,pDirtyRegion,dwFlags)==D3D_OK)
		{
			return true;
		}
	}
	return false;
}

LPDIRECT3DSURFACE9 CD3DSwapChain::GetBackBuffer(UINT Index)
{
	LPDIRECT3DSURFACE9 pBackBuffer=NULL;
	if(m_pD3DSwapChain->GetBackBuffer(Index,D3DBACKBUFFER_TYPE_MONO,&pBackBuffer)==D3D_OK)
	{
		return pBackBuffer;
	}
	return NULL;
}

}