#pragma once

//=====================================================================
//=====================================================================
// Defines CBasePin
//
// Abstract class that supports the basics of IPin
//=====================================================================
//=====================================================================

class CBaseFilter;

class  AM_NOVTABLE CBasePin : public CUnknown, public IPin, public IQualityControl
{

protected:

	WCHAR *         m_pName;		        // This pin's name
	IPin            *m_Connected;               // Pin we have connected to
	PIN_DIRECTION   m_dir;                      // Direction of this pin
	CCritSec        *m_pLock;                   // Object we use for locking
	bool            m_bRunTimeError;            // Run time error generated
	bool            m_bCanReconnectWhenActive;  // OK to reconnect when active
	bool            m_bTryMyTypesFirst;         // When connecting enumerate
	// this pin's types first
	CBaseFilter    *m_pFilter;                  // Filter we were created by
	IQualityControl *m_pQSink;                  // Target for Quality messages
	LONG            m_TypeVersion;              // Holds current type version
	CMediaType      m_mt;                       // Media type of connection

	CRefTime        m_tStart;                   // time from NewSegment call
	CRefTime        m_tStop;                    // time from NewSegment
	double          m_dRate;                    // rate from NewSegment

#ifdef DEBUG
	LONG            m_cRef;                     // Ref count tracing
#endif

	// displays pin connection information

#ifdef DEBUG
	void DisplayPinInfo(IPin *pReceivePin);
	void DisplayTypeInfo(IPin *pPin, const CMediaType *pmt);
#else
	void DisplayPinInfo(IPin *pReceivePin) {};
	void DisplayTypeInfo(IPin *pPin, const CMediaType *pmt) {};
#endif

	// used to agree a media type for a pin connection

	// given a specific media type, attempt a connection (includes
	// checking that the type is acceptable to this pin)
	HRESULT
		AttemptConnection(
		IPin* pReceivePin,      // connect to this pin
		const CMediaType* pmt   // using this type
		);

	// try all the media types in this enumerator - for each that
	// we accept, try to connect using ReceiveConnection.
	HRESULT TryMediaTypes(
		IPin *pReceivePin,      // connect to this pin
		const CMediaType *pmt,        // proposed type from Connect
		IEnumMediaTypes *pEnum);    // try this enumerator

	// establish a connection with a suitable mediatype. Needs to
	// propose a media type if the pmt pointer is null or partially
	// specified - use TryMediaTypes on both our and then the other pin's
	// enumerator until we find one that works.
	HRESULT AgreeMediaType(
		IPin *pReceivePin,      // connect to this pin
		const CMediaType *pmt);       // proposed type from Connect

public:

	CBasePin(
		TCHAR *pObjectName,         // Object description
		CBaseFilter *pFilter,       // Owning filter who knows about pins
		CCritSec *pLock,            // Object who implements the lock
		HRESULT *phr,               // General OLE return code
		LPCWSTR pName,              // Pin name for us
		PIN_DIRECTION dir);         // Either PINDIR_INPUT or PINDIR_OUTPUT
#ifdef UNICODE
	CBasePin(
		CHAR *pObjectName,         // Object description
		CBaseFilter *pFilter,       // Owning filter who knows about pins
		CCritSec *pLock,            // Object who implements the lock
		HRESULT *phr,               // General OLE return code
		LPCWSTR pName,              // Pin name for us
		PIN_DIRECTION dir);         // Either PINDIR_INPUT or PINDIR_OUTPUT
#endif
	virtual ~CBasePin();

	DECLARE_IUNKNOWN

	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void ** ppv);
	STDMETHODIMP_(ULONG) NonDelegatingRelease();
	STDMETHODIMP_(ULONG) NonDelegatingAddRef();

	// --- IPin methods ---

	// take lead role in establishing a connection. Media type pointer
	// may be null, or may point to partially-specified mediatype
	// (subtype or format type may be GUID_NULL).
	STDMETHODIMP Connect(
		IPin * pReceivePin,
		const AM_MEDIA_TYPE *pmt   // optional media type
		);

	// (passive) accept a connection from another pin
	STDMETHODIMP ReceiveConnection(
		IPin * pConnector,      // this is the initiating connecting pin
		const AM_MEDIA_TYPE *pmt   // this is the media type we will exchange
		);

	STDMETHODIMP Disconnect();

	STDMETHODIMP ConnectedTo(IPin **pPin);

	STDMETHODIMP ConnectionMediaType(AM_MEDIA_TYPE *pmt);

	STDMETHODIMP QueryPinInfo(
		PIN_INFO * pInfo
		);

	STDMETHODIMP QueryDirection(
		PIN_DIRECTION * pPinDir
		);

	STDMETHODIMP QueryId(
		LPWSTR * Id
		);

	// does the pin support this media type
	STDMETHODIMP QueryAccept(
		const AM_MEDIA_TYPE *pmt
		);

	// return an enumerator for this pins preferred media types
	STDMETHODIMP EnumMediaTypes(
		IEnumMediaTypes **ppEnum
		);

	// return an array of IPin* - the pins that this pin internally connects to
	// All pins put in the array must be AddReffed (but no others)
	// Errors: "Can't say" - FAIL, not enough slots - return S_FALSE
	// Default: return E_NOTIMPL
	// The filter graph will interpret NOT_IMPL as any input pin connects to
	// all visible output pins and vice versa.
	// apPin can be NULL if nPin==0 (not otherwise).
	STDMETHODIMP QueryInternalConnections(
		IPin* *apPin,     // array of IPin*
		ULONG *nPin       // on input, the number of slots
		// on output  the number of pins
		) { return E_NOTIMPL; }

	// Called when no more data will be sent
	STDMETHODIMP EndOfStream(void);

	// Begin/EndFlush still PURE

	// NewSegment notifies of the start/stop/rate applying to the data
	// about to be received. Default implementation records data and
	// returns S_OK.
	// Override this to pass downstream.
	STDMETHODIMP NewSegment(
		REFERENCE_TIME tStart,
		REFERENCE_TIME tStop,
		double dRate);

	//================================================================================
	// IQualityControl methods
	//================================================================================

	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

	STDMETHODIMP SetSink(IQualityControl * piqc);

	// --- helper methods ---

	// Returns true if the pin is connected. false otherwise.
	BOOL IsConnected(void) {return (m_Connected != NULL); };
	// Return the pin this is connected to (if any)
	IPin * GetConnected() { return m_Connected; };

	// Check if our filter is currently stopped
	BOOL IsStopped() {
		return (m_pFilter->m_State == State_Stopped);
	};

	// find out the current type version (used by enumerators)
	virtual LONG GetMediaTypeVersion();
	void IncrementTypeVersion();

	// switch the pin to active (paused or running) mode
	// not an error to call this if already active
	virtual HRESULT Active(void);

	// switch the pin to inactive state - may already be inactive
	virtual HRESULT Inactive(void);

	// Notify of Run() from filter
	virtual HRESULT Run(REFERENCE_TIME tStart);

	// check if the pin can support this specific proposed type and format
	virtual HRESULT CheckMediaType(const CMediaType *) PURE;

	// set the connection to use this format (previously agreed)
	virtual HRESULT SetMediaType(const CMediaType *);

	// check that the connection is ok before verifying it
	// can be overridden eg to check what interfaces will be supported.
	virtual HRESULT CheckConnect(IPin *);

	// Set and release resources required for a connection
	virtual HRESULT BreakConnect();
	virtual HRESULT CompleteConnect(IPin *pReceivePin);

	// returns the preferred formats for a pin
	virtual HRESULT GetMediaType(int iPosition,CMediaType *pMediaType);

	// access to NewSegment values
	REFERENCE_TIME CurrentStopTime() {
		return m_tStop;
	}
	REFERENCE_TIME CurrentStartTime() {
		return m_tStart;
	}
	double CurrentRate() {
		return m_dRate;
	}

	//  Access name
	LPWSTR Name() { return m_pName; };

	//  Can reconnectwhen active?
	void SetReconnectWhenActive(bool bCanReconnect)
	{
		m_bCanReconnectWhenActive = bCanReconnect;
	}

	bool CanReconnectWhenActive()
	{
		return m_bCanReconnectWhenActive;
	}

protected:
	STDMETHODIMP DisconnectInternal();
};
