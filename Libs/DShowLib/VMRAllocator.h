/****************************************************************************/
/*                                                                          */
/*      文件名:    VMRAllocator.h                                           */
/*      创建日期:  2009年07月06日                                           */
/*      作者:      Sagasarate                                               */
/*                                                                          */
/*      本软件版权归Sagasarate(sagasarate@sina.com)所有                     */
/*      你可以将本软件用于任何商业和非商业软件开发，但                      */
/*      必须保留此版权声明                                                  */
/*                                                                          */
/****************************************************************************/
#pragma once
#include "vector"

namespace DShowLib{

class CVMRAllocator:
	public IVMRSurfaceAllocator9, 
	public IVMRImagePresenter9
{
protected:
	CEasyCriticalSection    m_ObjectLock;
	HWND					m_hWnd;
	volatile long			m_RefCount;

	IDirect3D9 *								m_pD3D;	
	IDirect3DDevice9 *							m_pD3DDev;
	CSmartPtr<IVMRSurfaceAllocatorNotify9>		m_pVMRSurfAllocNotify;
	std::vector<CSmartPtr<IDirect3DSurface9> >	m_SurfaceList;	

	LPDIRECT3DTEXTURE9							m_pTargetTexture;
	FLOAT										m_fTU;
	FLOAT										m_fTV;
	int											m_Width;
	int											m_Height;
	int											m_OrgWidth;
	int											m_OrgHeight;
	
public:
	CVMRAllocator(HWND hWnd, IDirect3D9* pD3D,
		IDirect3DDevice9* pD3DDev);
	virtual ~CVMRAllocator(void);

	void SetTargetTexture(LPDIRECT3DTEXTURE9 pTargetTexture);

	// IVMRSurfaceAllocator9
	virtual HRESULT STDMETHODCALLTYPE InitializeDevice( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ VMR9AllocationInfo *lpAllocInfo,
		/* [out][in] */ DWORD *lpNumBuffers);

	virtual HRESULT STDMETHODCALLTYPE TerminateDevice( 
		/* [in] */ DWORD_PTR dwID);

	virtual HRESULT STDMETHODCALLTYPE GetSurface( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ DWORD SurfaceIndex,
		/* [in] */ DWORD SurfaceFlags,
		/* [out] */ IDirect3DSurface9 **lplpSurface);

	virtual HRESULT STDMETHODCALLTYPE AdviseNotify( 
		/* [in] */ IVMRSurfaceAllocatorNotify9 *lpIVMRSurfAllocNotify);

		// IVMRImagePresenter9
	virtual HRESULT STDMETHODCALLTYPE StartPresenting( 
		/* [in] */ DWORD_PTR dwUserID);

	virtual HRESULT STDMETHODCALLTYPE StopPresenting( 
		/* [in] */ DWORD_PTR dwUserID);

	virtual HRESULT STDMETHODCALLTYPE PresentImage( 
		/* [in] */ DWORD_PTR dwUserID,
		/* [in] */ VMR9PresentationInfo *lpPresInfo);

		// IUnknown
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
protected:
	void DeleteSurfaces();
	bool NeedToHandleDisplayChange();

	// This function is here so we can catch the loss of surfaces.
	// All the functions are using the FAIL_RET macro so that they exit
	// with the last error code.  When this returns with the surface lost
	// error code we can restore the surfaces.
	HRESULT PresentHelper(VMR9PresentationInfo *lpPresInfo);
};

inline void CVMRAllocator::SetTargetTexture(LPDIRECT3DTEXTURE9 pTargetTexture)
{
	m_pTargetTexture=pTargetTexture;
}

}