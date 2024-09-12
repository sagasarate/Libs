#pragma once


#define QueryFilterInfoReleaseGraph(fi) if ((fi).pGraph) (fi).pGraph->Release();

#define QueryPinInfoReleaseFilter(pi) if ((pi).pFilter) (pi).pFilter->Release();

//  Make me one from quartz.dll
STDAPI CreateMemoryAllocator(IMemAllocator **ppAllocator);

// helper used by IAMovieSetup implementation
STDAPI
AMovieSetupRegisterFilter( const AMOVIESETUP_FILTER * const psetupdata
						  , IFilterMapper *                  pIFM
						  , BOOL                             bRegister  );