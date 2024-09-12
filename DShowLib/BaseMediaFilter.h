#pragma once

//=====================================================================
//=====================================================================
// Defines CBaseMediaFilter
//
// Abstract base class implementing IMediaFilter.
//
// Typically you will derive your filter from CBaseFilter rather than
// this,  unless you are implementing an object such as a plug-in
// distributor that needs to support IMediaFilter but not IBaseFilter.
//
// Note that IMediaFilter is derived from IPersist to allow query of
// class id.
//=====================================================================
//=====================================================================

class AM_NOVTABLE CBaseMediaFilter : public CUnknown,
	public IMediaFilter
{

protected:

	FILTER_STATE    m_State;            // current state: running, paused
	IReferenceClock *m_pClock;          // this filter's reference clock
	// note: all filters in a filter graph use the same clock

	// offset from stream time to reference time
	CRefTime        m_tStart;

	CLSID	    m_clsid;            // This filters clsid
	// used for serialization
	CCritSec        *m_pLock;           // Object we use for locking

public:

	CBaseMediaFilter(
		const TCHAR     *pName,
		LPUNKNOWN pUnk,
		CCritSec  *pLock,
		REFCLSID   clsid);

	virtual ~CBaseMediaFilter();

	DECLARE_IUNKNOWN

	// override this to say what interfaces we support where
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);

	//
	// --- IPersist method ---
	//

	STDMETHODIMP GetClassID(CLSID *pClsID);

	// --- IMediaFilter methods ---

	STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE *State);

	STDMETHODIMP SetSyncSource(IReferenceClock *pClock);

	STDMETHODIMP GetSyncSource(IReferenceClock **pClock);

	// default implementation of Stop and Pause just record the
	// state. Override to activate or de-activate your filter.
	// Note that Run when called from Stopped state will call Pause
	// to ensure activation, so if you are a source or transform
	// you will probably not need to override Run.
	STDMETHODIMP Stop();
	STDMETHODIMP Pause();


	// the start parameter is the difference to be added to the
	// sample's stream time to get the reference time for
	// its presentation
	STDMETHODIMP Run(REFERENCE_TIME tStart);

	// --- helper methods ---

	// return the current stream time - ie find out what
	// stream time should be appearing now
	virtual HRESULT StreamTime(CRefTime& rtStream);

	// Is the filter currently active? (running or paused)
	BOOL IsActive() {
		CComAutoLock cObjectLock(m_pLock);
		return ((m_State == State_Paused) || (m_State == State_Running));
	};
};
