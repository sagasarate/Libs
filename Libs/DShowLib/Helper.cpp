#include "StdAfx.h"


STDAPI CreateMemoryAllocator(IMemAllocator **ppAllocator)
{
	return CoCreateInstance(CLSID_MemoryAllocator,
		0,
		CLSCTX_INPROC_SERVER,
		IID_IMemAllocator,
		(void **)ppAllocator);
}

// ------------------------------------------------------------------------
// filter registration through IFilterMapper. used if IFilterMapper is
// not found (Quartz 1.0 install)

STDAPI
AMovieSetupRegisterFilter( const AMOVIESETUP_FILTER * const psetupdata
						  , IFilterMapper *                  pIFM
						  , BOOL                             bRegister  )
{
	DbgLog((LOG_TRACE, 3, TEXT("= AMovieSetupRegisterFilter")));

	// check we've got data
	//
	if( NULL == psetupdata ) return S_FALSE;


	// unregister filter
	// (as pins are subkeys of filter's CLSID key
	// they do not need to be removed separately).
	//
	DbgLog((LOG_TRACE, 3, TEXT("= = unregister filter")));
	HRESULT hr = pIFM->UnregisterFilter( *(psetupdata->clsID) );


	if( bRegister )
	{
		// register filter
		//
		DbgLog((LOG_TRACE, 3, TEXT("= = register filter")));
		hr = pIFM->RegisterFilter( *(psetupdata->clsID)
			, psetupdata->strName
			, psetupdata->dwMerit    );
		if( SUCCEEDED(hr) )
		{
			// all its pins
			//
			DbgLog((LOG_TRACE, 3, TEXT("= = register filter pins")));
			for( UINT m1=0; m1 < psetupdata->nPins; m1++ )
			{
				hr = pIFM->RegisterPin( *(psetupdata->clsID)
					, psetupdata->lpPin[m1].strName
					, psetupdata->lpPin[m1].bRendered
					, psetupdata->lpPin[m1].bOutput
					, psetupdata->lpPin[m1].bZero
					, psetupdata->lpPin[m1].bMany
					, *(psetupdata->lpPin[m1].clsConnectsToFilter)
					, psetupdata->lpPin[m1].strConnectsToPin );

				if( SUCCEEDED(hr) )
				{
					// and each pin's media types
					//
					DbgLog((LOG_TRACE, 3, TEXT("= = register filter pin types")));
					for( UINT m2=0; m2 < psetupdata->lpPin[m1].nMediaTypes; m2++ )
					{
						hr = pIFM->RegisterPinType( *(psetupdata->clsID)
							, psetupdata->lpPin[m1].strName
							, *(psetupdata->lpPin[m1].lpMediaType[m2].clsMajorType)
							, *(psetupdata->lpPin[m1].lpMediaType[m2].clsMinorType) );
						if( FAILED(hr) ) break;
					}
					if( FAILED(hr) ) break;
				}
				if( FAILED(hr) ) break;
			}
		}
	}

	// handle one acceptable "error" - that
	// of filter not being registered!
	// (couldn't find a suitable #define'd
	// name for the error!)
	//
	if( 0x80070002 == hr)
		return NOERROR;
	else
		return hr;
}


//  Put this one here rather than in ctlutil.cpp to avoid linking
//  anything brought in by ctlutil.cpp
STDAPI CreatePosPassThru(
						 LPUNKNOWN pAgg,
						 BOOL bRenderer,
						 IPin *pPin,
						 IUnknown **ppPassThru
						 )
{
	*ppPassThru = NULL;
	IUnknown *pUnkSeek;
	HRESULT hr = CoCreateInstance(CLSID_SeekingPassThru,
		pAgg,
		CLSCTX_INPROC_SERVER,
		IID_IUnknown,
		(void **)&pUnkSeek
		);
	if (FAILED(hr)) {
		return hr;
	}

	ISeekingPassThru *pPassThru;
	hr = pUnkSeek->QueryInterface(IID_ISeekingPassThru, (void**)&pPassThru);
	if (FAILED(hr)) {
		pUnkSeek->Release();
		return hr;
	}
	hr = pPassThru->Init(bRenderer, pPin);
	pPassThru->Release();
	if (FAILED(hr)) {
		pUnkSeek->Release();
		return hr;
	}
	*ppPassThru = pUnkSeek;
	return S_OK;
}