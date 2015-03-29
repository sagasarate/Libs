#include "StdAfx.h"

//=====================================================================
//=====================================================================
// Implements CBaseFilter
//=====================================================================
//=====================================================================


/* Override this to say what interfaces we support and where */

STDMETHODIMP CBaseFilter::NonDelegatingQueryInterface(REFIID riid,
													  void **ppv)
{
	/* Do we have this interface */

	if (riid == IID_IBaseFilter) {
		return GetInterface((IBaseFilter *) this, ppv);
	} else if (riid == IID_IMediaFilter) {
		return GetInterface((IMediaFilter *) this, ppv);
	} else if (riid == IID_IPersist) {
		return GetInterface((IPersist *) this, ppv);
	} else if (riid == IID_IAMovieSetup) {
		return GetInterface((IAMovieSetup *) this, ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}

#ifdef DEBUG
STDMETHODIMP_(ULONG) CBaseFilter::NonDelegatingRelease()
{
	if (m_cRef == 1) {
		KASSERT(m_pGraph == NULL);
	}
	return CUnknown::NonDelegatingRelease();
}
#endif


/* Constructor */

CBaseFilter::CBaseFilter(const TCHAR    *pName,
						 LPUNKNOWN  pUnk,
						 CCritSec   *pLock,
						 REFCLSID   clsid) :
CUnknown( pName, pUnk ),
m_pLock(pLock),
m_clsid(clsid),
m_State(State_Stopped),
m_pClock(NULL),
m_pGraph(NULL),
m_pSink(NULL),
m_pName(NULL),
m_PinVersion(1)
{

	ASSERT(pLock != NULL);
}

/* Passes in a redundant HRESULT argument */

CBaseFilter::CBaseFilter(TCHAR     *pName,
						 LPUNKNOWN  pUnk,
						 CCritSec  *pLock,
						 REFCLSID   clsid,
						 HRESULT   *phr) :
CUnknown( pName, pUnk ),
m_pLock(pLock),
m_clsid(clsid),
m_State(State_Stopped),
m_pClock(NULL),
m_pGraph(NULL),
m_pSink(NULL),
m_pName(NULL),
m_PinVersion(1)
{

	ASSERT(pLock != NULL);
	UNREFERENCED_PARAMETER(phr);
}

#ifdef UNICODE
CBaseFilter::CBaseFilter(const CHAR *pName,
						 LPUNKNOWN  pUnk,
						 CCritSec   *pLock,
						 REFCLSID   clsid) :
CUnknown( pName, pUnk ),
m_pLock(pLock),
m_clsid(clsid),
m_State(State_Stopped),
m_pClock(NULL),
m_pGraph(NULL),
m_pSink(NULL),
m_pName(NULL),
m_PinVersion(1)
{

	ASSERT(pLock != NULL);
}
CBaseFilter::CBaseFilter(CHAR     *pName,
						 LPUNKNOWN  pUnk,
						 CCritSec  *pLock,
						 REFCLSID   clsid,
						 HRESULT   *phr) :
CUnknown( pName, pUnk ),
m_pLock(pLock),
m_clsid(clsid),
m_State(State_Stopped),
m_pClock(NULL),
m_pGraph(NULL),
m_pSink(NULL),
m_pName(NULL),
m_PinVersion(1)
{

	ASSERT(pLock != NULL);
	UNREFERENCED_PARAMETER(phr);
}
#endif

/* Destructor */

CBaseFilter::~CBaseFilter()
{

	// NOTE we do NOT hold references on the filtergraph for m_pGraph or m_pSink
	// When we did we had the circular reference problem.  Nothing would go away.

	delete[] m_pName;

	// must be stopped, but can't call Stop here since
	// our critsec has been destroyed.

	/* Release any clock we were using */
	if (m_pClock) {
		m_pClock->Release();
		m_pClock = NULL;
	}
}

/* Return the filter's clsid */
STDMETHODIMP
CBaseFilter::GetClassID(CLSID *pClsID)
{
	CheckPointer(pClsID,E_POINTER);
	ValidateReadWritePtr(pClsID,sizeof(CLSID));
	*pClsID = m_clsid;
	return NOERROR;
}

/* Override this if your state changes are not done synchronously */
STDMETHODIMP
CBaseFilter::GetState(DWORD dwMSecs, FILTER_STATE *State)
{
	UNREFERENCED_PARAMETER(dwMSecs);
	CheckPointer(State,E_POINTER);
	ValidateReadWritePtr(State,sizeof(FILTER_STATE));

	*State = m_State;
	return S_OK;
}


/* Set the clock we will use for synchronisation */

STDMETHODIMP
CBaseFilter::SetSyncSource(IReferenceClock *pClock)
{
	CComAutoLock cObjectLock(m_pLock);

	// Ensure the new one does not go away - even if the same as the old
	if (pClock) {
		pClock->AddRef();
	}

	// if we have a clock, release it
	if (m_pClock) {
		m_pClock->Release();
	}

	// Set the new reference clock (might be NULL)
	// Should we query it to ensure it is a clock?  Consider for a debug build.
	m_pClock = pClock;

	return NOERROR;
}

/* Return the clock we are using for synchronisation */
STDMETHODIMP
CBaseFilter::GetSyncSource(IReferenceClock **pClock)
{
	CheckPointer(pClock,E_POINTER);
	ValidateReadWritePtr(pClock,sizeof(IReferenceClock *));
	CComAutoLock cObjectLock(m_pLock);

	if (m_pClock) {
		// returning an interface... addref it...
		m_pClock->AddRef();
	}
	*pClock = (IReferenceClock*)m_pClock;
	return NOERROR;
}



// override CBaseMediaFilter Stop method, to deactivate any pins this
// filter has.
STDMETHODIMP
CBaseFilter::Stop()
{
	CComAutoLock cObjectLock(m_pLock);
	HRESULT hr = NOERROR;

	// notify all pins of the state change
	if (m_State != State_Stopped) {
		int cPins = GetPinCount();
		for (int c = 0; c < cPins; c++) {

			CBasePin *pPin = GetPin(c);

			// Disconnected pins are not activated - this saves pins worrying
			// about this state themselves. We ignore the return code to make
			// sure everyone is inactivated regardless. The base input pin
			// class can return an error if it has no allocator but Stop can
			// be used to resync the graph state after something has gone bad

			if (pPin->IsConnected()) {
				HRESULT hrTmp = pPin->Inactive();
				if (FAILED(hrTmp) && SUCCEEDED(hr)) {
					hr = hrTmp;
				}
			}
		}
	}


	m_State = State_Stopped;
	return hr;
}


// override CBaseMediaFilter Pause method to activate any pins
// this filter has (also called from Run)

STDMETHODIMP
CBaseFilter::Pause()
{
	CComAutoLock cObjectLock(m_pLock);

	// notify all pins of the change to active state
	if (m_State == State_Stopped) {
		int cPins = GetPinCount();
		for (int c = 0; c < cPins; c++) {

			CBasePin *pPin = GetPin(c);

			// Disconnected pins are not activated - this saves pins
			// worrying about this state themselves

			if (pPin->IsConnected()) {
				HRESULT hr = pPin->Active();
				if (FAILED(hr)) {
					return hr;
				}
			}
		}
	}



	m_State = State_Paused;
	return S_OK;
}

// Put the filter into a running state.

// The time parameter is the offset to be added to the samples'
// stream time to get the reference time at which they should be presented.
//
// you can either add these two and compare it against the reference clock,
// or you can call CBaseFilter::StreamTime and compare that against
// the sample timestamp.

STDMETHODIMP
CBaseFilter::Run(REFERENCE_TIME tStart)
{
	CComAutoLock cObjectLock(m_pLock);

	// remember the stream time offset
	m_tStart = tStart;

	if (m_State == State_Stopped){
		HRESULT hr = Pause();

		if (FAILED(hr)) {
			return hr;
		}
	}
	// notify all pins of the change to active state
	if (m_State != State_Running) {
		int cPins = GetPinCount();
		for (int c = 0; c < cPins; c++) {

			CBasePin *pPin = GetPin(c);

			// Disconnected pins are not activated - this saves pins
			// worrying about this state themselves

			if (pPin->IsConnected()) {
				HRESULT hr = pPin->Run(tStart);
				if (FAILED(hr)) {
					return hr;
				}
			}
		}
	}


	m_State = State_Running;
	return S_OK;
}

//
// return the current stream time - samples with start timestamps of this
// time or before should be rendered by now
HRESULT
CBaseFilter::StreamTime(CRefTime& rtStream)
{
	// Caller must lock for synchronization
	// We can't grab the filter lock because we want to be able to call
	// this from worker threads without deadlocking

	if (m_pClock == NULL) {
		return VFW_E_NO_CLOCK;
	}

	// get the current reference time
	HRESULT hr = m_pClock->GetTime((REFERENCE_TIME*)&rtStream);
	if (FAILED(hr)) {
		return hr;
	}

	// subtract the stream offset to get stream time
	rtStream -= m_tStart;

	return S_OK;
}


/* Create an enumerator for the pins attached to this filter */

STDMETHODIMP
CBaseFilter::EnumPins(IEnumPins **ppEnum)
{
	CheckPointer(ppEnum,E_POINTER);
	ValidateReadWritePtr(ppEnum,sizeof(IEnumPins *));

	/* Create a new ref counted enumerator */

	*ppEnum = new CEnumPins(this,
		NULL);

	return *ppEnum == NULL ? E_OUTOFMEMORY : NOERROR;
}


// default behaviour of FindPin is to assume pins are named
// by their pin names
STDMETHODIMP
CBaseFilter::FindPin(
					 LPCWSTR Id,
					 IPin ** ppPin
					 )
{
	CheckPointer(ppPin,E_POINTER);
	ValidateReadWritePtr(ppPin,sizeof(IPin *));

	//  We're going to search the pin list so maintain integrity
	CComAutoLock lck(m_pLock);
	int iCount = GetPinCount();
	for (int i = 0; i < iCount; i++) {
		CBasePin *pPin = GetPin(i);
		ASSERT(pPin != NULL);

		if (0 == lstrcmpW(pPin->Name(), Id)) {
			//  Found one that matches
			//
			//  AddRef() and return it
			*ppPin = pPin;
			pPin->AddRef();
			return S_OK;
		}
	}
	*ppPin = NULL;
	return VFW_E_NOT_FOUND;
}

/* Return information about this filter */

STDMETHODIMP
CBaseFilter::QueryFilterInfo(FILTER_INFO * pInfo)
{
	CheckPointer(pInfo,E_POINTER);
	ValidateReadWritePtr(pInfo,sizeof(FILTER_INFO));

	if (m_pName) {
		lstrcpynW(pInfo->achName, m_pName, sizeof(pInfo->achName)/sizeof(WCHAR));
	} else {
		pInfo->achName[0] = L'\0';
	}
	pInfo->pGraph = m_pGraph;
	if (m_pGraph)
		m_pGraph->AddRef();
	return NOERROR;
}


/* Provide the filter with a filter graph */

STDMETHODIMP
CBaseFilter::JoinFilterGraph(
							 IFilterGraph * pGraph,
							 LPCWSTR pName)
{
	CComAutoLock cObjectLock(m_pLock);

	// NOTE: we no longer hold references on the graph (m_pGraph, m_pSink)

	m_pGraph = pGraph;
	if (m_pGraph) {
		HRESULT hr = m_pGraph->QueryInterface(IID_IMediaEventSink,
			(void**) &m_pSink);
		if (FAILED(hr)) {
			ASSERT(m_pSink == NULL);
		}
		else m_pSink->Release();        // we do NOT keep a reference on it.
	} else {
		// if graph pointer is null, then we should
		// also release the IMediaEventSink on the same object - we don't
		// refcount it, so just set it to null
		m_pSink = NULL;
	}


	if (m_pName) {
		delete[] m_pName;
		m_pName = NULL;
	}

	if (pName) {
		DWORD nameLen = lstrlenW(pName)+1;
		m_pName = new WCHAR[nameLen];
		if (m_pName) {
			CopyMemory(m_pName, pName, nameLen*sizeof(WCHAR));
		} else {
			// !!! error here?
			ASSERT(FALSE);
		}
	}


	return NOERROR;
}


// return a Vendor information string. Optional - may return E_NOTIMPL.
// memory returned should be freed using CoTaskMemFree
// default implementation returns E_NOTIMPL
STDMETHODIMP
CBaseFilter::QueryVendorInfo(
							 LPWSTR* pVendorInfo)
{
	UNREFERENCED_PARAMETER(pVendorInfo);
	return E_NOTIMPL;
}


// send an event notification to the filter graph if we know about it.
// returns S_OK if delivered, S_FALSE if the filter graph does not sink
// events, or an error otherwise.
HRESULT
CBaseFilter::NotifyEvent(
						 long EventCode,
						 LONG_PTR EventParam1,
						 LONG_PTR EventParam2)
{
	// Snapshot so we don't have to lock up
	IMediaEventSink *pSink = m_pSink;
	if (pSink) {
		if (EC_COMPLETE == EventCode) {
			EventParam2 = (LONG_PTR)(IBaseFilter*)this;
		}

		return pSink->Notify(EventCode, EventParam1, EventParam2);
	} else {
		return E_NOTIMPL;
	}
}

// Request reconnect
// pPin is the pin to reconnect
// pmt is the type to reconnect with - can be NULL
// Calls ReconnectEx on the filter graph
HRESULT
CBaseFilter::ReconnectPin(
						  IPin *pPin,
						  AM_MEDIA_TYPE const *pmt
						  )
{
	IFilterGraph2 *pGraph2;
	if (m_pGraph != NULL) {
		HRESULT hr = m_pGraph->QueryInterface(IID_IFilterGraph2, (void **)&pGraph2);
		if (SUCCEEDED(hr)) {
			hr = pGraph2->ReconnectEx(pPin, pmt);
			pGraph2->Release();
			return hr;
		} else {
			return m_pGraph->Reconnect(pPin);
		}
	} else {
		return E_NOINTERFACE;
	}
}



/* This is the same idea as the media type version does for type enumeration
on pins but for the list of pins available. So if the list of pins you
provide changes dynamically then either override this virtual function
to provide the version number, or more simply call IncrementPinVersion */

LONG CBaseFilter::GetPinVersion()
{
	return m_PinVersion;
}


/* Increment the current pin version cookie */

void CBaseFilter::IncrementPinVersion()
{
	InterlockedIncrement(&m_PinVersion);
}

/* register filter */

STDMETHODIMP CBaseFilter::Register()
{
	// get setup data, if it exists
	//
	LPAMOVIESETUP_FILTER psetupdata = GetSetupData();

	// check we've got data
	//
	if( NULL == psetupdata ) return S_FALSE;

	// init is ref counted so call just in case
	// we're being called cold.
	//
	HRESULT hr = CoInitialize( (LPVOID)NULL );
	ASSERT( SUCCEEDED(hr) );

	// get hold of IFilterMapper
	//
	IFilterMapper *pIFM;
	hr = CoCreateInstance( CLSID_FilterMapper
		, NULL
		, CLSCTX_INPROC_SERVER
		, IID_IFilterMapper
		, (void **)&pIFM       );
	if( SUCCEEDED(hr) )
	{
		hr = AMovieSetupRegisterFilter( psetupdata, pIFM, TRUE );
		pIFM->Release();
	}

	// and clear up
	//
	CoFreeUnusedLibraries();
	CoUninitialize();

	return NOERROR;
}


/* unregister filter */

STDMETHODIMP CBaseFilter::Unregister()
{
	// get setup data, if it exists
	//
	LPAMOVIESETUP_FILTER psetupdata = GetSetupData();

	// check we've got data
	//
	if( NULL == psetupdata ) return S_FALSE;

	// OLE init is ref counted so call
	// just in case we're being called cold.
	//
	HRESULT hr = CoInitialize( (LPVOID)NULL );
	ASSERT( SUCCEEDED(hr) );

	// get hold of IFilterMapper
	//
	IFilterMapper *pIFM;
	hr = CoCreateInstance( CLSID_FilterMapper
		, NULL
		, CLSCTX_INPROC_SERVER
		, IID_IFilterMapper
		, (void **)&pIFM       );
	if( SUCCEEDED(hr) )
	{
		hr = AMovieSetupRegisterFilter( psetupdata, pIFM, FALSE );

		// release interface
		//
		pIFM->Release();
	}

	// clear up
	//
	CoFreeUnusedLibraries();
	CoUninitialize();

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
