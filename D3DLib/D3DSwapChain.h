#pragma once

namespace D3DLib{


class CD3DSwapChain :
	public CNameObject
{
protected:
	CD3DDevice *			m_pDevice;
	IDirect3DSwapChain9 *	m_pD3DSwapChain;

	DECLARE_CLASS_INFO(CD3DSwapChain)
public:
	CD3DSwapChain(void);
	~CD3DSwapChain(void);

	virtual void Destory();

	bool Create(CD3DDevice * pDevice,D3DDEVICE_PARAMS& CreateParams);

	bool Present(const RECT * pSourceRect=NULL,const RECT * pDestRect=NULL,HWND hDestWindowOverride=NULL,const RGNDATA * pDirtyRegion=NULL,DWORD dwFlags=0);

	LPDIRECT3DSURFACE9 GetBackBuffer(UINT Index);
};

}