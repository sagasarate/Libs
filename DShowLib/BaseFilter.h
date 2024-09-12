#pragma once

//=====================================================================
//=====================================================================
// Defines CBaseFilter
//
// An abstract class providing basic IBaseFilter support for pin
// enumeration and filter information reading.
//
// We cannot derive from CBaseMediaFilter since methods in IMediaFilter
// are also in IBaseFilter and would be ambiguous. Since much of the code
// assumes that they derive from a class that has m_State and other state
// directly available, we duplicate code from CBaseMediaFilter rather than
// having a member variable.
//
// Derive your filter from this, or from a derived object such as
// CTransformFilter.
//=====================================================================
//=====================================================================


class AM_NOVTABLE CBaseFilter : public CUnknown,        // Handles an IUnknown
	public IBaseFilter,     // The Filter Interface
	public IAMovieSetup     // For un/registration
{

	friend class CBasePin;

protected:
	FILTER_STATE    m_State;            // current state: running, paused
	IReferenceClock *m_pClock;          // this graph's ref clock
	CRefTime        m_tStart;           // offset from stream time to reference time
	CLSID	    m_clsid;            // This filters clsid
	// used for serialization
	CCritSec        *m_pLock;           // Object we use for locking

	WCHAR           *m_pName;           // Full filter name
	IFilterGraph    *m_pGraph;          // Graph we belong to
	IMediaEventSink *m_pSink;           // Called with notify events
	LONG            m_PinVersion;       // Current pin version

public:

	CBaseFilter(
		const TCHAR *pName,     // Object description
		LPUNKNOWN pUnk,         // IUnknown of delegating object
		CCritSec  *pLock,       // Object who maintains lock
		REFCLSID   clsid);      // The clsid to be used to serialize this filter

	CBaseFilter(
		TCHAR     *pName,       // Object description
		LPUNKNOWN pUnk,         // IUnknown of delegating object
		CCritSec  *pLock,       // Object who maintains lock
		REFCLSID   clsid,       // The clsid to be used to serialize this filter
		HRESULT   *phr);        // General OLE return code
#ifdef UNICODE
	CBaseFilter(
		const CHAR *pName,     // Object description
		LPUNKNOWN pUnk,         // IUnknown of delegating object
		CCritSec  *pLock,       // Object who maintains lock
		REFCLSID   clsid);      // The clsid to be used to serialize this filter

	CBaseFilter(
		CHAR     *pName,       // Object description
		LPUNKNOWN pUnk,         // IUnknown of delegating object
		CCritSec  *pLock,       // Object who maintains lock
		REFCLSID   clsid,       // The clsid to be used to serialize this filter
		HRESULT   *phr);        // General OLE return code
#endif
	~CBaseFilter();

	DECLARE_IUNKNOWN

	// override this to say what interfaces we support where
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
#ifdef DEBUG
	STDMETHODIMP_(ULONG) NonDelegatingRelease();
#endif

	//
	// --- IPersist method ---
	//

	STDMETHODIMP GetClassID(CLSID *pClsID);

	// --- IMediaFilter methods ---

	STDMETHODIMP GetState(DWORD dwMSecs, FILTER_STATE *State);

	STDMETHODIMP SetSyncSource(IReferenceClock *pClock);

	STDMETHODIMP GetSyncSource(IReferenceClock **pClock);


	// override Stop and Pause so we can activate the pins.
	// Note that Run will call Pause first if activation needed.
	// Override these if you want to activate your filter rather than
	// your pins.
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

	// Is the filter currently active?
	BOOL IsActive() {
		CComAutoLock cObjectLock(m_pLock);
		return ((m_State == State_Paused) || (m_State == State_Running));
	};

	// Is this filter stopped (without locking)
	BOOL IsStopped() {
		return (m_State == State_Stopped);
	};

	//
	// --- IBaseFilter methods ---
	//

	// pin enumerator
	STDMETHODIMP EnumPins(
		IEnumPins ** ppEnum);


	// default behaviour of FindPin assumes pin ids are their names
	STDMETHODIMP FindPin(
		LPCWSTR Id,
		IPin ** ppPin
		);

	STDMETHODIMP QueryFilterInfo(
		FILTER_INFO * pInfo);

	STDMETHODIMP JoinFilterGraph(
		IFilterGraph * pGraph,
		LPCWSTR pName);

	// return a Vendor information string. Optional - may return E_NOTIMPL.
	// memory returned should be freed using CoTaskMemFree
	// default implementation returns E_NOTIMPL
	STDMETHODIMP QueryVendorInfo(
		LPWSTR* pVendorInfo
		);

	// --- helper methods ---

	// send an event notification to the filter graph if we know about it.
	// returns S_OK if delivered, S_FALSE if the filter graph does not sink
	// events, or an error otherwise.
	HRESULT NotifyEvent(
		long EventCode,
		LONG_PTR EventParam1,
		LONG_PTR EventParam2);

	// return the filter graph we belong to
	IFilterGraph *GetFilterGraph() {
		return m_pGraph;
	}

	// Request reconnect
	// pPin is the pin to reconnect
	// pmt is the type to reconnect with - can be NULL
	// Calls ReconnectEx on the filter graph
	HRESULT ReconnectPin(IPin *pPin, AM_MEDIA_TYPE const *pmt);

	// find out the current pin version (used by enumerators)
	virtual LONG GetPinVersion();
	void IncrementPinVersion();

	// you need to supply these to access the pins from the enumerator
	// and for default Stop and Pause/Run activation.
	virtual int GetPinCount() PURE;
	virtual CBasePin *GetPin(int n) PURE;

	// --- IAMovieSetup methods ---

	STDMETHODIMP Register();    // ask filter to register itself
	STDMETHODIMP Unregister();  // and unregister itself

	// --- setup helper methods ---
	// (override to return filters setup data)

	virtual LPAMOVIESETUP_FILTER GetSetupData(){ return NULL; }

};

