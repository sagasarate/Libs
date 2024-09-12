/****************************************************************************/
/*                                                                          */
/*      文件名:    VMRImageCompositor.h                                     */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once

namespace DShowLib{

class CVMRImageCompositor :
	public IVMRImageCompositor9
{
protected:
	IDirect3DDevice9 *			m_pD3DDev;
	LPDIRECT3DTEXTURE9			m_pTargetTexture;
	FLOAT						m_fTU;
	FLOAT						m_fTV;
	int							m_Width;
	int							m_Height;
	int							m_OrgWidth;
	int							m_OrgHeight;
	DWORD						m_PixelFormat;

	volatile long				m_RefCount;
public:
	CVMRImageCompositor(IDirect3DDevice9* pD3DDev);
	virtual ~CVMRImageCompositor(void);

	virtual HRESULT STDMETHODCALLTYPE InitCompositionDevice( 
		/* [in] */ IUnknown *pD3DDevice);

	virtual HRESULT STDMETHODCALLTYPE TermCompositionDevice( 
		/* [in] */ IUnknown *pD3DDevice);

	virtual HRESULT STDMETHODCALLTYPE SetStreamMediaType( 
		/* [in] */ DWORD dwStrmID,
		/* [in] */ AM_MEDIA_TYPE *pmt,
		/* [in] */ BOOL fTexture);

	virtual HRESULT STDMETHODCALLTYPE CompositeImage( 
		/* [in] */ IUnknown *pD3DDevice,
		/* [in] */ IDirect3DSurface9 *pddsRenderTarget,
		/* [in] */ AM_MEDIA_TYPE *pmtRenderTarget,
		/* [in] */ REFERENCE_TIME rtStart,
		/* [in] */ REFERENCE_TIME rtEnd,
		/* [in] */ D3DCOLOR dwClrBkGnd,
		/* [in] */ VMR9VideoStreamInfo *pVideoStreamInfo,
		/* [in] */ UINT cStreams);

	virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
		REFIID riid,
		void** ppvObject);

	virtual ULONG STDMETHODCALLTYPE AddRef();
	virtual ULONG STDMETHODCALLTYPE Release();

	FLOAT GetUS()
	{
		return m_fTU;
	}
	FLOAT GetVS()
	{
		return m_fTV;
	}

	void GetSize(int& Width,int& Height)
	{
		Width=m_Width;
		Height=m_Height;
	}
	void GetOrgSize(int& Width,int& Height)
	{
		Width=m_OrgWidth;
		Height=m_OrgHeight;
	}
	DWORD GetPixelFormat()
	{
		return m_PixelFormat;
	}
	void SetTargetTexture(LPDIRECT3DTEXTURE9 pTargetTexture)
	{
		m_pTargetTexture=pTargetTexture;
	}
	
protected:
	RECT GetSourceRectFromMediaType(const AM_MEDIA_TYPE *pMediaType);
	LPBITMAPINFOHEADER GetbmiHeader(const AM_MEDIA_TYPE *pMediaType);

};
}
