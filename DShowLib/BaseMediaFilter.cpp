#include "StdAfx.h"

//=====================================================================
//=====================================================================
// Implements CBaseMediaFilter
//=====================================================================
//=====================================================================


/* Constructor */

CBaseMediaFilter::CBaseMediaFilter(const TCHAR  *pName,
								   LPUNKNOWN    pUnk,
								   CCritSec *pLock,
								   REFCLSID clsid) :
CUnknown(pName, pUnk),
m_pLock(pLock),
m_clsid(clsid),
m_State(State_Stopped),
m_pClock(NULL)
{
}


/* Destructor */

CBaseMediaFilter::~CBaseMediaFilter()
{
	// must be stopped, but can't call Stop here since
	// our critsec has been destroyed.

	/* Release any clock we were using */

	if (m_pClock) {
		m_pClock->Release();
		m_pClock = NULL;
	}
}


/* Override this to say what interfaces we support and where */

STDMETHODIMP
CBaseMediaFilter::NonDelegatingQueryInterface(
	REFIID riid,
	void ** ppv)
{
	if (riid == IID_IMediaFilter) {
		return GetInterface((IMediaFilter *) this, ppv);
	} else if (riid == IID_IPersist) {
		return GetInterface((IPersist *) this, ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}

/* Return the filter's clsid */
STDMETHODIMP
CBaseMediaFilter::GetClassID(CLSID *pClsID)
{
	CheckPointer(pClsID,E_POINTER);
	ValidateReadWritePtr(pClsID,sizeof(CLSID));
	*pClsID = m_clsid;
	return NOERROR;
}

/* Override this if your state changes are not done synchronously */

STDMETHODIMP
CBaseMediaFilter::GetState(DWORD dwMSecs, FILTER_STATE *State)
{
	UNREFERENCED_PARAMETER(dwMSecs);
	CheckPointer(State,E_POINTER);
	ValidateReadWritePtr(State,sizeof(FILTER_STATE));

	*State = m_State;
	return S_OK;
}


/* Set the clock we will use for synchronisation */

STDMETHODIMP
CBaseMediaFilter::SetSyncSource(IReferenceClock *pClock)
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
CBaseMediaFilter::GetSyncSource(IReferenceClock **pClock)
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


/* Put the filter into a stopped state */

STDMETHODIMP
CBaseMediaFilter::Stop()
{
	CComAutoLock cObjectLock(m_pLock);

	m_State = State_Stopped;
	return S_OK;
}


/* Put the filter into a paused state */

STDMETHODIMP
CBaseMediaFilter::Pause()
{
	CComAutoLock cObjectLock(m_pLock);

	m_State = State_Paused;
	return S_OK;
}


// Put the filter into a running state.

// The time parameter is the offset to be added to the samples'
// stream time to get the reference time at which they should be presented.
//
// you can either add these two and compare it against the reference clock,
// or you can call CBaseMediaFilter::StreamTime and compare that against
// the sample timestamp.

STDMETHODIMP
CBaseMediaFilter::Run(REFERENCE_TIME tStart)
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
	m_State = State_Running;
	return S_OK;
}


//
// return the current stream time - samples with start timestamps of this
// time or before should be rendered by now
HRESULT
CBaseMediaFilter::StreamTime(CRefTime& rtStream)
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
