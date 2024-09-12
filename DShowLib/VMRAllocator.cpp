/****************************************************************************/
/*                                                                          */
/*      文件名:    VMRAllocator.cpp                                         */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#include "StdAfx.h"
#include ".\vmrallocator.h"


namespace DShowLib{

#define FAIL_RET(x) do { if( FAILED( hr = ( x  ) ) ) \
	return hr; } while(0)

CVMRAllocator::CVMRAllocator(HWND hWnd, IDirect3D9* pD3D,
							 IDirect3DDevice9* pD3DDev)
{
	m_RefCount=1;
	m_hWnd=hWnd;
	m_pD3D=pD3D;
	m_pD3DDev=pD3DDev;
	m_pTargetTexture=NULL;
	m_fTU = 1.f;
	m_fTV = 1.f;
	m_Width=0;
	m_Height=0;
	m_OrgWidth=0;
	m_OrgHeight=0;

}

CVMRAllocator::~CVMRAllocator(void)
{
	DeleteSurfaces();
}




//IVMRSurfaceAllocator9
HRESULT CVMRAllocator::InitializeDevice( 
									 /* [in] */ DWORD_PTR dwUserID,
									 /* [in] */ VMR9AllocationInfo *lpAllocInfo,
									 /* [out][in] */ DWORD *lpNumBuffers)
{
	D3DCAPS9 d3dcaps;
	DWORD dwWidth = 1;
	DWORD dwHeight = 1;
	

	if( lpNumBuffers == NULL )
	{
		return E_POINTER;
	}

	if( m_pVMRSurfAllocNotify == NULL )
	{
		return E_FAIL;
	}

	HRESULT hr = S_OK;

	m_OrgWidth=lpAllocInfo->dwWidth;
	m_OrgHeight=lpAllocInfo->dwHeight;

	m_pD3DDev->GetDeviceCaps( &d3dcaps );
	if( d3dcaps.TextureCaps & D3DPTEXTURECAPS_POW2 )
	{

		while( dwWidth < lpAllocInfo->dwWidth )
			dwWidth = dwWidth << 1;
		while( dwHeight < lpAllocInfo->dwHeight )
			dwHeight = dwHeight << 1;

		m_fTU = (float)(lpAllocInfo->dwWidth) / (float)(dwWidth);
		m_fTV = (float)(lpAllocInfo->dwHeight) / (float)(dwHeight);
		
		lpAllocInfo->dwWidth = dwWidth;
		lpAllocInfo->dwHeight = dwHeight;
		
	}
	m_Width=lpAllocInfo->dwWidth;
	m_Height=lpAllocInfo->dwHeight;
	
	lpAllocInfo->dwFlags |= VMR9AllocFlag_TextureSurface;
	DeleteSurfaces(); 
	m_SurfaceList.resize(*lpNumBuffers);
	FAIL_RET( m_pVMRSurfAllocNotify->AllocateSurfaceHelper(lpAllocInfo, lpNumBuffers, & m_SurfaceList.at(0) ) );
	

	return S_OK;
}

HRESULT CVMRAllocator::TerminateDevice( 
									/* [in] */ DWORD_PTR dwID)
{
	DeleteSurfaces();
	return S_OK;
}

HRESULT CVMRAllocator::GetSurface( 
							   /* [in] */ DWORD_PTR dwUserID,
							   /* [in] */ DWORD SurfaceIndex,
							   /* [in] */ DWORD SurfaceFlags,
							   /* [out] */ IDirect3DSurface9 **lplpSurface)
{
	if( lplpSurface == NULL )
	{
		return E_POINTER;
	}

	if (SurfaceIndex >= m_SurfaceList.size() ) 
	{
		return E_FAIL;
	}

	CAutoLock Lock(m_ObjectLock);

	*lplpSurface=m_SurfaceList[SurfaceIndex];
	(*lplpSurface)->AddRef();

	return S_OK;
}

HRESULT CVMRAllocator::AdviseNotify( 
								 /* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify)
{
	CAutoLock Lock(m_ObjectLock);

	HRESULT hr=S_OK;

	m_pVMRSurfAllocNotify = lpIVMRSurfAllocNotify;

	HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );
	FAIL_RET( m_pVMRSurfAllocNotify->SetD3DDevice( m_pD3DDev, hMonitor ) );

	return hr;
}

HRESULT CVMRAllocator::StartPresenting( 
									/* [in] */ DWORD_PTR dwUserID)
{
	CAutoLock Lock(m_ObjectLock);

	ASSERT( m_pD3DDev );
	if( m_pD3DDev == NULL )
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CVMRAllocator::StopPresenting( 
								   /* [in] */ DWORD_PTR dwUserID)
{
	return S_OK;
}

HRESULT CVMRAllocator::PresentImage( 
								 /* [in] */ DWORD_PTR dwUserID,
								 /* [in] */ VMR9PresentationInfo *lpPresInfo)
{
	HRESULT hr;
	CAutoLock Lock(m_ObjectLock);

	// if we are in the middle of the display change
	if( NeedToHandleDisplayChange() )
	{
		// NOTE: this piece of code is left as a user exercise.  
		// The D3DDevice here needs to be switched
		// to the device that is using another adapter
	}

	hr = PresentHelper( lpPresInfo );

	// IMPORTANT: device can be lost when user changes the resolution
	// or when (s)he presses Ctrl + Alt + Delete.
	// We need to restore our video memory after that

	//if( hr == D3DERR_DEVICELOST)
	//{
	//	if (m_pD3DDev->TestCooperativeLevel() == D3DERR_DEVICENOTRESET) 
	//	{				

	//		HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

	//		FAIL_RET( m_pVMRSurfAllocNotify->ChangeD3DDevice( m_pD3DDev, hMonitor ) );

	//	}

	//	hr = S_OK;
	//}

	return hr;
}

HRESULT CVMRAllocator::PresentHelper(VMR9PresentationInfo *lpPresInfo)
{
	// parameter validation
	if( lpPresInfo == NULL )
	{
		return E_POINTER;
	}
	else if( lpPresInfo->lpSurf == NULL )
	{
		return E_POINTER;
	}

	CAutoLock Lock(m_ObjectLock);
	HRESULT hr;

	
	if(m_pTargetTexture)
	{
	
		CSmartPtr<IDirect3DSurface9> pSurface;
		FAIL_RET( m_pTargetTexture->GetSurfaceLevel( 0 , &pSurface) );

		// copy the full surface onto the texture's surface
		FAIL_RET( m_pD3DDev->StretchRect( lpPresInfo->lpSurf, NULL,
			pSurface, NULL,
			D3DTEXF_NONE ) );

		return hr;
	}
	return S_OK;
}

bool CVMRAllocator::NeedToHandleDisplayChange()
{
	if( ! m_pVMRSurfAllocNotify )
	{
		return false;
	}

	D3DDEVICE_CREATION_PARAMETERS Parameters;
	if( FAILED( m_pD3DDev->GetCreationParameters(&Parameters) ) )
	{
		ASSERT( false );
		return false;
	}

	HMONITOR currentMonitor = m_pD3D->GetAdapterMonitor( Parameters.AdapterOrdinal );

	HMONITOR hMonitor = m_pD3D->GetAdapterMonitor( D3DADAPTER_DEFAULT );

	return hMonitor != currentMonitor;


}


// IUnknown
HRESULT CVMRAllocator::QueryInterface( 
								   REFIID riid,
								   void** ppvObject)
{
	HRESULT hr = E_NOINTERFACE;

	if( ppvObject == NULL ) {
		hr = E_POINTER;
	} 
	else if( riid == IID_IVMRSurfaceAllocator9 ) {
		*ppvObject = static_cast<IVMRSurfaceAllocator9*>( this );
		AddRef();
		hr = S_OK;
	} 
	else if( riid == IID_IVMRImagePresenter9 ) {
		*ppvObject = static_cast<IVMRImagePresenter9*>( this );
		AddRef();
		hr = S_OK;
	} 
	else if( riid == IID_IUnknown ) {
		*ppvObject = 
			static_cast<IUnknown*>( 
			static_cast<IVMRSurfaceAllocator9*>( this ) );
		AddRef();
		hr = S_OK;    
	}

	return hr;
}

ULONG CVMRAllocator::AddRef()
{
	return InterlockedIncrement(& m_RefCount);
}

ULONG CVMRAllocator::Release()
{
	ULONG ret = InterlockedDecrement(& m_RefCount);
	if( ret == 0 )
	{
		delete this;
	}

	return ret;
}

void CVMRAllocator::DeleteSurfaces()
{
	CAutoLock Lock(m_ObjectLock);	

	for( size_t i = 0; i < m_SurfaceList.size(); ++i ) 
	{
		m_SurfaceList[i] = NULL;
	}
}

}