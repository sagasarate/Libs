/****************************************************************************/
/*                                                                          */
/*      文件名:    VMRImageCompositor.cpp                                   */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\vmrimagecompositor.h"
#include "dvdmedia.h"

namespace DShowLib{

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
	return hr; } while(0)
CVMRImageCompositor::CVMRImageCompositor(IDirect3DDevice9* pD3DDev)
{
	m_pD3DDev=pD3DDev;
	m_pTargetTexture=NULL;
	m_fTU=1.0f;
	m_fTV=1.0f;
	m_Width=0;
	m_Height=0;
	m_OrgWidth=0;
	m_OrgHeight=0;
	m_PixelFormat=0;

}

CVMRImageCompositor::~CVMRImageCompositor(void)
{
}


HRESULT CVMRImageCompositor::InitCompositionDevice( 
	/* [in] */ IUnknown *pD3DDevice)
{
	return S_OK;
}

HRESULT CVMRImageCompositor::TermCompositionDevice( 
	/* [in] */ IUnknown *pD3DDevice)
{
	return S_OK;
}

HRESULT CVMRImageCompositor::SetStreamMediaType( 
	/* [in] */ DWORD dwStrmID,
	/* [in] */ AM_MEDIA_TYPE *pmt,
	/* [in] */ BOOL fTexture)
{
	if (pmt)
	{
		

		LPBITMAPINFOHEADER pBmiHeader=GetbmiHeader(pmt);

		if(pBmiHeader)
		{
		
			D3DCAPS9 d3dcaps;


			if(pBmiHeader->biCompression==BI_RGB||pBmiHeader->biCompression==BI_BITFIELDS)
			{
				m_PixelFormat=D3DFMT_X8R8G8B8;
			}
			else
			{			
				m_PixelFormat=pBmiHeader->biCompression;
			}


			RECT Rect=GetSourceRectFromMediaType(pmt);
			m_OrgWidth=Rect.right-Rect.left;
			m_OrgHeight=Rect.bottom-Rect.left;
			

			m_pD3DDev->GetDeviceCaps( &d3dcaps );
			if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
			{
				m_Width=1;
				m_Height=1;
				while( m_Width < m_OrgWidth )
					m_Width = m_Width << 1;
				while( m_Height < m_OrgHeight )
					m_Height = m_Height << 1;

				m_fTU = (float)(m_OrgWidth) / (float)(m_Width);
				m_fTV = (float)(m_OrgHeight) / (float)(m_Height);
			}
			else
			{
				m_Width=m_OrgWidth;
				m_Height=m_OrgHeight;
			}
		}
	}
	return S_OK;
}

HRESULT CVMRImageCompositor::CompositeImage( 
	/* [in] */ IUnknown *pD3DDevice,
	/* [in] */ IDirect3DSurface9 *pddsRenderTarget,
	/* [in] */ AM_MEDIA_TYPE *pmtRenderTarget,
	/* [in] */ REFERENCE_TIME rtStart,
	/* [in] */ REFERENCE_TIME rtEnd,
	/* [in] */ D3DCOLOR dwClrBkGnd,
	/* [in] */ VMR9VideoStreamInfo *pVideoStreamInfo,
	/* [in] */ UINT cStreams)
{
	HRESULT hr=S_OK;

	if(m_pTargetTexture)
	{

		D3DLOCKED_RECT SrcRect,DesRect;
		
		
		if(SUCCEEDED(pVideoStreamInfo->pddsVideoSurface->LockRect(&SrcRect,NULL,0)))
		{
			if(SUCCEEDED(m_pTargetTexture->LockRect(0,&DesRect,NULL,0)))
			{
				char * pSrcBits=(char *)SrcRect.pBits;
				char * pDesBits=(char *)DesRect.pBits;
				for(int i=0;i<m_OrgHeight;i++)
				{
					memcpy(pDesBits,pSrcBits,SrcRect.Pitch);
					pSrcBits+=SrcRect.Pitch;
					pDesBits+=DesRect.Pitch;
				}
				m_pTargetTexture->UnlockRect(0);				
			}
			pVideoStreamInfo->pddsVideoSurface->UnlockRect();
			
		}	

		return hr;
	}
	return S_OK;
}


HRESULT CVMRImageCompositor::QueryInterface( 
									  REFIID riid,
									  void** ppvObject)
{
	HRESULT hr = E_NOINTERFACE;

	if( ppvObject == NULL ) {
		hr = E_POINTER;
	} 
	else if( riid == IID_IVMRImageCompositor ) {
		*ppvObject = static_cast<IVMRImageCompositor9*>( this );
		AddRef();
		hr = S_OK;
	} 	
	else if( riid == IID_IUnknown ) {
		*ppvObject = 
			static_cast<IUnknown*>( 
			static_cast<IVMRImageCompositor9*>( this ) );
		AddRef();
		hr = S_OK;    
	}
	return hr;
}



ULONG CVMRImageCompositor::AddRef()
{
	return InterlockedIncrement(& m_RefCount);
}

ULONG CVMRImageCompositor::Release()
{
	ULONG ret = InterlockedDecrement(& m_RefCount);
	if( ret == 0 )
	{
		delete this;
	}

	return ret;
}

RECT CVMRImageCompositor::GetSourceRectFromMediaType( const AM_MEDIA_TYPE *pMediaType )
{
	RECT rRect = {0,0,0,0};
	if (!pMediaType)
	{
		PrintSystemLog(0xff00ff, "pMediaType is NULL");
		return rRect;
	}

	if (!(pMediaType->pbFormat))
	{
		return rRect;
	}

	if ((pMediaType->formattype == FORMAT_VideoInfo) &&
		(pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER)))
	{
		rRect = (((VIDEOINFOHEADER*)(pMediaType->pbFormat))->rcSource);
	}
	else if ((pMediaType->formattype == FORMAT_VideoInfo2) &&
		(pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER2)))
	{
		rRect = (((VIDEOINFOHEADER2*)(pMediaType->pbFormat))->rcSource);
	}

	// DirectShow filters have the habit of sometimes not setting the src and dst
	// rectangles, in this case we should imply that the these rectangles
	// should be the same width and height as the bitmap in the media format.
	if (IsRectEmpty(&rRect)) 
	{
		LPBITMAPINFOHEADER lpbi = GetbmiHeader(pMediaType);

		if (lpbi) 
		{
			SetRect(&rRect, 0, 0, abs(lpbi->biWidth), abs(lpbi->biHeight));
		}
	}

	return rRect;
}

LPBITMAPINFOHEADER CVMRImageCompositor::GetbmiHeader(const AM_MEDIA_TYPE *pMediaType)
{	

	if (!pMediaType)
	{
		PrintSystemLog(0xff00ff, "pMediaType is NULL");
		return NULL;
	}

	if (!(pMediaType->pbFormat))
	{
		PrintSystemLog(0xff00ff, "pMediaType->pbFormat is NULL");
		return NULL;
	}

	LPBITMAPINFOHEADER lpHeader = NULL;
	if ((pMediaType->formattype == FORMAT_VideoInfo) &&
		(pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER)))
	{
		lpHeader = &(((VIDEOINFOHEADER*)(pMediaType->pbFormat))->bmiHeader);
	}
	else if ((pMediaType->formattype == FORMAT_VideoInfo2) &&
		(pMediaType->cbFormat >= sizeof(VIDEOINFOHEADER2)))
	{
		lpHeader = &(((VIDEOINFOHEADER2*)(pMediaType->pbFormat))->bmiHeader);
	}

	return lpHeader;
}

}