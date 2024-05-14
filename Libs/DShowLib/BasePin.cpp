#include "StdAfx.h"


#define CONNECT_TRACE_LEVEL 3


//=====================================================================
//=====================================================================
// Implements CBasePin
//=====================================================================
//=====================================================================


/* NOTE The implementation of this class calls the CUnknown constructor with
a NULL outer unknown pointer. This has the effect of making us a self
contained class, ie any QueryInterface, AddRef or Release calls will be
routed to the class's NonDelegatingUnknown methods. You will typically
find that the classes that do this then override one or more of these
virtual functions to provide more specialised behaviour. A good example
of this is where a class wants to keep the QueryInterface internal but
still wants its lifetime controlled by the external object */

/* Constructor */

CBasePin::CBasePin(TCHAR *pObjectName,
				   CBaseFilter *pFilter,
				   CCritSec *pLock,
				   HRESULT *phr,
				   LPCWSTR pName,
				   PIN_DIRECTION dir) :
CUnknown( pObjectName, NULL ),
m_pFilter(pFilter),
m_pLock(pLock),
m_pName(NULL),
m_Connected(NULL),
m_dir(dir),
m_bRunTimeError(FALSE),
m_pQSink(NULL),
m_TypeVersion(1),
m_tStart(),
m_tStop(MAX_TIME),
m_bCanReconnectWhenActive(false),
m_bTryMyTypesFirst(false),
m_dRate(1.0)
{
	/*  WARNING - pFilter is often not a properly constituted object at
	this state (in particular QueryInterface may not work) - this
	is because its owner is often its containing object and we
	have been called from the containing object's constructor so
	the filter's owner has not yet had its CUnknown constructor
	called
	*/

	ASSERT(pFilter != NULL);
	ASSERT(pLock != NULL);

	if (pName) {
		DWORD nameLen = lstrlenW(pName)+1;
		m_pName = new WCHAR[nameLen];
		if (m_pName) {
			CopyMemory(m_pName, pName, nameLen*sizeof(WCHAR));
		}
	}

#ifdef DEBUG
	m_cRef = 0;
#endif
}

#ifdef UNICODE
CBasePin::CBasePin(CHAR *pObjectName,
				   CBaseFilter *pFilter,
				   CCritSec *pLock,
				   HRESULT *phr,
				   LPCWSTR pName,
				   PIN_DIRECTION dir) :
CUnknown( pObjectName, NULL ),
m_pFilter(pFilter),
m_pLock(pLock),
m_pName(NULL),
m_Connected(NULL),
m_dir(dir),
m_bRunTimeError(FALSE),
m_pQSink(NULL),
m_TypeVersion(1),
m_tStart(),
m_tStop(MAX_TIME),
m_bCanReconnectWhenActive(false),
m_bTryMyTypesFirst(false),
m_dRate(1.0)
{
	/*  WARNING - pFilter is often not a properly constituted object at
	this state (in particular QueryInterface may not work) - this
	is because its owner is often its containing object and we
	have been called from the containing object's constructor so
	the filter's owner has not yet had its CUnknown constructor
	called
	*/

	ASSERT(pFilter != NULL);
	ASSERT(pLock != NULL);

	if (pName) {
		DWORD nameLen = lstrlenW(pName)+1;
		m_pName = new WCHAR[nameLen];
		if (m_pName) {
			CopyMemory(m_pName, pName, nameLen*sizeof(WCHAR));
		}
	}

#ifdef DEBUG
	m_cRef = 0;
#endif
}
#endif

/* Destructor since a connected pin holds a reference count on us there is
no way that we can be deleted unless we are not currently connected */

CBasePin::~CBasePin()
{

	//  We don't call disconnect because if the filter is going away
	//  all the pins must have a reference count of zero so they must
	//  have been disconnected anyway - (but check the assumption)
	ASSERT(m_Connected == FALSE);

	delete[] m_pName;

	// check the internal reference count is consistent
	ASSERT(m_cRef == 0);
}


/* Override this to say what interfaces we support and where */

STDMETHODIMP
CBasePin::NonDelegatingQueryInterface(REFIID riid, void ** ppv)
{
	/* Do we have this interface */

	if (riid == IID_IPin) {
		return GetInterface((IPin *) this, ppv);
	} else if (riid == IID_IQualityControl) {
		return GetInterface((IQualityControl *) this, ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}


/* Override to increment the owning filter's reference count */

STDMETHODIMP_(ULONG)
CBasePin::NonDelegatingAddRef()
{
	ASSERT(InterlockedIncrement(&m_cRef) > 0);
	return m_pFilter->AddRef();
}


/* Override to decrement the owning filter's reference count */

STDMETHODIMP_(ULONG)
CBasePin::NonDelegatingRelease()
{
	ASSERT(InterlockedDecrement(&m_cRef) >= 0);
	return m_pFilter->Release();
}


/* Displays pin connection information */

#ifdef DEBUG
void
CBasePin::DisplayPinInfo(IPin *pReceivePin)
{

	if (DbgCheckModuleLevel(LOG_TRACE, CONNECT_TRACE_LEVEL)) {
		PIN_INFO ConnectPinInfo;
		PIN_INFO ReceivePinInfo;

		if (FAILED(QueryPinInfo(&ConnectPinInfo))) {
			lstrcpyW(ConnectPinInfo.achName, L"Bad Pin");
		} else {
			QueryPinInfoReleaseFilter(ConnectPinInfo);
		}

		if (FAILED(pReceivePin->QueryPinInfo(&ReceivePinInfo))) {
			lstrcpyW(ReceivePinInfo.achName, L"Bad Pin");
		} else {
			QueryPinInfoReleaseFilter(ReceivePinInfo);
		}

		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Trying to connect Pins :")));
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    <%ls>"), ConnectPinInfo.achName));
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    <%ls>"), ReceivePinInfo.achName));
	}
}
#endif


/* Displays general information on the pin media type */

#ifdef DEBUG
void CBasePin::DisplayTypeInfo(IPin *pPin, const CMediaType *pmt)
{
	UNREFERENCED_PARAMETER(pPin);
	if (DbgCheckModuleLevel(LOG_TRACE, CONNECT_TRACE_LEVEL)) {
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Trying media type:")));
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    major type:  %hs"),
			GuidNames[*pmt->Type()]));
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("    sub type  :  %hs"),
			GuidNames[*pmt->Subtype()]));
	}
}
#endif

/* Asked to connect to a pin. A pin is always attached to an owning filter
object so we always delegate our locking to that object. We first of all
retrieve a media type enumerator for the input pin and see if we accept
any of the formats that it would ideally like, failing that we retrieve
our enumerator and see if it will accept any of our preferred types */

STDMETHODIMP
CBasePin::Connect(
				  IPin * pReceivePin,
				  const AM_MEDIA_TYPE *pmt   // optional media type
				  )
{
	CheckPointer(pReceivePin,E_POINTER);
	ValidateReadPtr(pReceivePin,sizeof(IPin));
	CComAutoLock cObjectLock(m_pLock);
	DisplayPinInfo(pReceivePin);

	/* See if we are already connected */

	if (m_Connected) {
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Already connected")));
		return VFW_E_ALREADY_CONNECTED;
	}

	/* See if the filter is active */
	if (!IsStopped() && !m_bCanReconnectWhenActive) {
		return VFW_E_NOT_STOPPED;
	}


	// Find a mutually agreeable media type -
	// Pass in the template media type. If this is partially specified,
	// each of the enumerated media types will need to be checked against
	// it. If it is non-null and fully specified, we will just try to connect
	// with this.

	const CMediaType * ptype = (CMediaType*)pmt;
	HRESULT hr = AgreeMediaType(pReceivePin, ptype);
	if (FAILED(hr)) {
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Failed to agree type")));

		// Since the procedure is already returning an error code, there
		// is nothing else this function can do to report the error.
		EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );


		return hr;
	}

	DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Connection succeeded")));


	return NOERROR;
}

// given a specific media type, attempt a connection (includes
// checking that the type is acceptable to this pin)
HRESULT
CBasePin::AttemptConnection(
							IPin* pReceivePin,      // connect to this pin
							const CMediaType* pmt   // using this type
							)
{
	// The caller should hold the filter lock becasue this function
	// uses m_Connected.  The caller should also hold the filter lock
	// because this function calls SetMediaType(), IsStopped() and
	// CompleteConnect().
	ASSERT(CritCheckIn(m_pLock));

	// Check that the connection is valid  -- need to do this for every
	// connect attempt since BreakConnect will undo it.
	HRESULT hr = CheckConnect(pReceivePin);
	if (FAILED(hr)) {
		DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("CheckConnect failed")));

		// Since the procedure is already returning an error code, there
		// is nothing else this function can do to report the error.
		EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

		return hr;
	}

	DisplayTypeInfo(pReceivePin, pmt);

	/* Check we will accept this media type */

	hr = CheckMediaType(pmt);
	if (hr == NOERROR) {

		/*  Make ourselves look connected otherwise ReceiveConnection
		may not be able to complete the connection
		*/
		m_Connected = pReceivePin;
		m_Connected->AddRef();
		hr = SetMediaType(pmt);
		if (SUCCEEDED(hr)) {
			/* See if the other pin will accept this type */

			hr = pReceivePin->ReceiveConnection((IPin *)this, pmt);
			if (SUCCEEDED(hr)) {
				/* Complete the connection */

				hr = CompleteConnect(pReceivePin);
				if (SUCCEEDED(hr)) {
					return hr;
				} else {
					DbgLog((LOG_TRACE,
						CONNECT_TRACE_LEVEL,
						TEXT("Failed to complete connection")));
					pReceivePin->Disconnect();
				}
			}
		}
	} else {
		// we cannot use this media type

		// return a specific media type error if there is one
		// or map a general failure code to something more helpful
		// (in particular S_FALSE gets changed to an error code)
		if (SUCCEEDED(hr) ||
			(hr == E_FAIL) ||
			(hr == E_INVALIDARG)) {
				hr = VFW_E_TYPE_NOT_ACCEPTED;
		}
	}

	// BreakConnect and release any connection here in case CheckMediaType
	// failed, or if we set anything up during a call back during
	// ReceiveConnection.

	// Since the procedure is already returning an error code, there
	// is nothing else this function can do to report the error.
	EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

	/*  If failed then undo our state */
	if (m_Connected) {
		m_Connected->Release();
		m_Connected = NULL;
	}

	return hr;
}

/* Given an enumerator we cycle through all the media types it proposes and
firstly suggest them to our derived pin class and if that succeeds try
them with the pin in a ReceiveConnection call. This means that if our pin
proposes a media type we still check in here that we can support it. This
is deliberate so that in simple cases the enumerator can hold all of the
media types even if some of them are not really currently available */

HRESULT CBasePin::TryMediaTypes(
								IPin *pReceivePin,
								const CMediaType *pmt,
								IEnumMediaTypes *pEnum)
{
	/* Reset the current enumerator position */

	HRESULT hr = pEnum->Reset();
	if (FAILED(hr)) {
		return hr;
	}

	CMediaType *pMediaType = NULL;
	ULONG ulMediaCount = 0;

	// attempt to remember a specific error code if there is one
	HRESULT hrFailure = S_OK;

	for (;;) {

		/* Retrieve the next media type NOTE each time round the loop the
		enumerator interface will allocate another AM_MEDIA_TYPE structure
		If we are successful then we copy it into our output object, if
		not then we must delete the memory allocated before returning */

		hr = pEnum->Next(1, (AM_MEDIA_TYPE**)&pMediaType,&ulMediaCount);
		if (hr != S_OK) {
			if (S_OK == hrFailure) {
				hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;
			}
			return hrFailure;
		}


		ASSERT(ulMediaCount == 1);
		ASSERT(pMediaType);

		// check that this matches the partial type (if any)

		if ((pmt == NULL) ||
			pMediaType->MatchesPartial(pmt)) {

				hr = AttemptConnection(pReceivePin, pMediaType);

				// attempt to remember a specific error code
				if (FAILED(hr) &&
					SUCCEEDED(hrFailure) &&
					(hr != E_FAIL) &&
					(hr != E_INVALIDARG) &&
					(hr != VFW_E_TYPE_NOT_ACCEPTED)) {
						hrFailure = hr;
				}
		} else {
			hr = VFW_E_NO_ACCEPTABLE_TYPES;
		}

		DeleteMediaType(pMediaType);

		if (S_OK == hr) {
			return hr;
		}
	}
}


/* This is called to make the connection, including the taask of finding
a media type for the pin connection. pmt is the proposed media type
from the Connect call: if this is fully specified, we will try that.
Otherwise we enumerate and try all the input pin's types first and
if that fails we then enumerate and try all our preferred media types.
For each media type we check it against pmt (if non-null and partially
specified) as well as checking that both pins will accept it.
*/

HRESULT CBasePin::AgreeMediaType(
								 IPin *pReceivePin,
								 const CMediaType *pmt)
{
	ASSERT(pReceivePin);
	IEnumMediaTypes *pEnumMediaTypes = NULL;

	// if the media type is fully specified then use that
	if ( (pmt != NULL) && (!pmt->IsPartiallySpecified())) {

		// if this media type fails, then we must fail the connection
		// since if pmt is nonnull we are only allowed to connect
		// using a type that matches it.

		return AttemptConnection(pReceivePin, pmt);
	}


	/* Try the other pin's enumerator */

	HRESULT hrFailure = VFW_E_NO_ACCEPTABLE_TYPES;

	for (int i = 0; i < 2; i++) {
		HRESULT hr;
		if (i == (int)m_bTryMyTypesFirst) {
			hr = pReceivePin->EnumMediaTypes(&pEnumMediaTypes);
		} else {
			hr = EnumMediaTypes(&pEnumMediaTypes);
		}
		if (SUCCEEDED(hr)) {
			ASSERT(pEnumMediaTypes);
			hr = TryMediaTypes(pReceivePin,pmt,pEnumMediaTypes);
			pEnumMediaTypes->Release();
			if (SUCCEEDED(hr)) {
				return NOERROR;
			} else {
				// try to remember specific error codes if there are any
				if ((hr != E_FAIL) &&
					(hr != E_INVALIDARG) &&
					(hr != VFW_E_TYPE_NOT_ACCEPTED)) {
						hrFailure = hr;
				}
			}
		}
	}

	return hrFailure;
}


/* Called when we want to complete a connection to another filter. Failing
this will also fail the connection and disconnect the other pin as well */

HRESULT
CBasePin::CompleteConnect(IPin *pReceivePin)
{
	UNREFERENCED_PARAMETER(pReceivePin);
	return NOERROR;
}


/* This is called to set the format for a pin connection - CheckMediaType
will have been called to check the connection format and if it didn't
return an error code then this (virtual) function will be invoked */

HRESULT
CBasePin::SetMediaType(const CMediaType *pmt)
{
	HRESULT hr = m_mt.Set(*pmt);
	if (FAILED(hr)) {
		return hr;
	}

	return NOERROR;
}


/* This is called during Connect() to provide a virtual method that can do
any specific check needed for connection such as QueryInterface. This
base class method just checks that the pin directions don't match */

HRESULT
CBasePin::CheckConnect(IPin * pPin)
{
	/* Check that pin directions DONT match */

	PIN_DIRECTION pd;
	pPin->QueryDirection(&pd);

	ASSERT((pd == PINDIR_OUTPUT) || (pd == PINDIR_INPUT));
	ASSERT((m_dir == PINDIR_OUTPUT) || (m_dir == PINDIR_INPUT));

	// we should allow for non-input and non-output connections?
	if (pd == m_dir) {
		return VFW_E_INVALID_DIRECTION;
	}
	return NOERROR;
}


/* This is called when we realise we can't make a connection to the pin and
must undo anything we did in CheckConnect - override to release QIs done */

HRESULT
CBasePin::BreakConnect()
{
	return NOERROR;
}


/* Called normally by an output pin on an input pin to try and establish a
connection.
*/

STDMETHODIMP
CBasePin::ReceiveConnection(
							IPin * pConnector,      // this is the pin who we will connect to
							const AM_MEDIA_TYPE *pmt    // this is the media type we will exchange
							)
{
	CheckPointer(pConnector,E_POINTER);
	CheckPointer(pmt,E_POINTER);
	ValidateReadPtr(pConnector,sizeof(IPin));
	ValidateReadPtr(pmt,sizeof(AM_MEDIA_TYPE));
	CComAutoLock cObjectLock(m_pLock);

#ifdef _DEBUG
	PIN_INFO PinInfo;

	if(!FAILED(pConnector->QueryPinInfo(&PinInfo)))
	{

		CEasyString PinName=PinInfo.achName;



		PrintSystemLog(0,"RecvPin:%s",(LPCTSTR)PinName);


		if(PinInfo.pFilter)
		{

			FILTER_INFO FilterInfo;
			if (!FAILED(PinInfo.pFilter->QueryFilterInfo(&FilterInfo)))
			{

				CLSID ClassID;

				PinInfo.pFilter->GetClassID(&ClassID);

				CEasyString FilterName=FilterInfo.achName;

				PrintSystemLog(0,"RecvPinFilter:%s",(LPCTSTR)FilterName);


				// The FILTER_INFO structure holds a pointer to the Filter Graph
				// Manager, with a reference count that must be released.
				if (FilterInfo.pGraph != NULL)
				{
					FilterInfo.pGraph->Release();
				}
			}

			PinInfo.pFilter->Release();
		}

		//MessageBox(NULL, (LPCTSTR)PinName, TEXT("Filter Name"), MB_OK);
	}

#endif

	/* Are we already connected */
	if (m_Connected) {
		return VFW_E_ALREADY_CONNECTED;
	}

	/* See if the filter is active */
	if (!IsStopped() && !m_bCanReconnectWhenActive) {
		return VFW_E_NOT_STOPPED;
	}

	HRESULT hr = CheckConnect(pConnector);
	if (FAILED(hr)) {
		// Since the procedure is already returning an error code, there
		// is nothing else this function can do to report the error.
		EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );


		return hr;
	}

	/* Ask derived class if this media type is ok */

	CMediaType * pcmt = (CMediaType*) pmt;
	hr = CheckMediaType(pcmt);
	if (hr != NOERROR) {
		// no -we don't support this media type

		// Since the procedure is already returning an error code, there
		// is nothing else this function can do to report the error.
		EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );

		// return a specific media type error if there is one
		// or map a general failure code to something more helpful
		// (in particular S_FALSE gets changed to an error code)
		if (SUCCEEDED(hr) ||
			(hr == E_FAIL) ||
			(hr == E_INVALIDARG)) {
				hr = VFW_E_TYPE_NOT_ACCEPTED;
		}


		return hr;
	}

	/* Complete the connection */

	m_Connected = pConnector;
	m_Connected->AddRef();
	hr = SetMediaType(pcmt);
	if (SUCCEEDED(hr)) {
		hr = CompleteConnect(pConnector);
		if (SUCCEEDED(hr)) {


			return NOERROR;
		}
	}

	DbgLog((LOG_TRACE, CONNECT_TRACE_LEVEL, TEXT("Failed to set the media type or failed to complete the connection.")));
	m_Connected->Release();
	m_Connected = NULL;

	// Since the procedure is already returning an error code, there
	// is nothing else this function can do to report the error.
	EXECUTE_ASSERT( SUCCEEDED( BreakConnect() ) );


	return hr;
}


/* Called when we want to terminate a pin connection */

STDMETHODIMP
CBasePin::Disconnect()
{
	CComAutoLock cObjectLock(m_pLock);

	/* See if the filter is active */
	if (!IsStopped()) {
		return VFW_E_NOT_STOPPED;
	}

	return DisconnectInternal();
}

STDMETHODIMP
CBasePin::DisconnectInternal()
{
	ASSERT(CritCheckIn(m_pLock));

	if (m_Connected) {
		HRESULT hr = BreakConnect();
		if( FAILED( hr ) ) {


			// There is usually a bug in the program if BreakConnect() fails.
			DbgBreak( "WARNING: BreakConnect() failed in CBasePin::Disconnect()." );
			return hr;
		}

		m_Connected->Release();
		m_Connected = NULL;


		return S_OK;
	} else {
		// no connection - not an error


		return S_FALSE;
	}
}


/* Return an AddRef()'d pointer to the connected pin if there is one */
STDMETHODIMP
CBasePin::ConnectedTo(
					  IPin **ppPin
					  )
{
	CheckPointer(ppPin,E_POINTER);
	ValidateReadWritePtr(ppPin,sizeof(IPin *));
	//
	//  It's pointless to lock here.
	//  The caller should ensure integrity.
	//

	IPin *pPin = m_Connected;
	*ppPin = pPin;
	if (pPin != NULL) {
		pPin->AddRef();
		return S_OK;
	} else {
		ASSERT(*ppPin == NULL);
		return VFW_E_NOT_CONNECTED;
	}
}

/* Return the media type of the connection */
STDMETHODIMP
CBasePin::ConnectionMediaType(
							  AM_MEDIA_TYPE *pmt
							  )
{
	CheckPointer(pmt,E_POINTER);
	ValidateReadWritePtr(pmt,sizeof(AM_MEDIA_TYPE));
	CComAutoLock cObjectLock(m_pLock);

	/*  Copy constructor of m_mt allocates the memory */
	if (IsConnected()) {
		CopyMediaType( pmt, &m_mt );
		return S_OK;
	} else {
		((CMediaType *)pmt)->InitMediaType();
		return VFW_E_NOT_CONNECTED;
	}
}

/* Return information about the filter we are connect to */

STDMETHODIMP
CBasePin::QueryPinInfo(
					   PIN_INFO * pInfo
					   )
{
	CheckPointer(pInfo,E_POINTER);
	ValidateReadWritePtr(pInfo,sizeof(PIN_INFO));

	pInfo->pFilter = m_pFilter;
	if (m_pFilter) {
		m_pFilter->AddRef();
	}

	if (m_pName) {
		lstrcpynW(pInfo->achName, m_pName, sizeof(pInfo->achName)/sizeof(WCHAR));
	} else {
		pInfo->achName[0] = L'\0';
	}

	pInfo->dir = m_dir;

	return NOERROR;
}

STDMETHODIMP
CBasePin::QueryDirection(
						 PIN_DIRECTION * pPinDir
						 )
{
	CheckPointer(pPinDir,E_POINTER);
	ValidateReadWritePtr(pPinDir,sizeof(PIN_DIRECTION));

	*pPinDir = m_dir;
	return NOERROR;
}

// Default QueryId to return the pin's name
STDMETHODIMP
CBasePin::QueryId(
				  LPWSTR * Id
				  )
{
	//  We're not going away because someone's got a pointer to us
	//  so there's no need to lock

	return AMGetWideString(Name(), Id);
}

/* Does this pin support this media type WARNING this interface function does
not lock the main object as it is meant to be asynchronous by nature - if
the media types you support depend on some internal state that is updated
dynamically then you will need to implement locking in a derived class */

STDMETHODIMP
CBasePin::QueryAccept(
					  const AM_MEDIA_TYPE *pmt
					  )
{
	CheckPointer(pmt,E_POINTER);
	ValidateReadPtr(pmt,sizeof(AM_MEDIA_TYPE));

	/* The CheckMediaType method is valid to return error codes if the media
	type is horrible, an example might be E_INVALIDARG. What we do here
	is map all the error codes into either S_OK or S_FALSE regardless */

	HRESULT hr = CheckMediaType((CMediaType*)pmt);
	if (FAILED(hr)) {
		return S_FALSE;
	}
	// note that the only defined success codes should be S_OK and S_FALSE...
	return hr;
}


/* This can be called to return an enumerator for the pin's list of preferred
media types. An input pin is not obliged to have any preferred formats
although it can do. For example, the window renderer has a preferred type
which describes a video image that matches the current window size. All
output pins should expose at least one preferred format otherwise it is
possible that neither pin has any types and so no connection is possible */

STDMETHODIMP
CBasePin::EnumMediaTypes(
						 IEnumMediaTypes **ppEnum
						 )
{
	CheckPointer(ppEnum,E_POINTER);
	ValidateReadWritePtr(ppEnum,sizeof(IEnumMediaTypes *));

	/* Create a new ref counted enumerator */

	*ppEnum = new CEnumMediaTypes(this,
		NULL);

	if (*ppEnum == NULL) {
		return E_OUTOFMEMORY;
	}

	return NOERROR;
}



/* This is a virtual function that returns a media type corresponding with
place iPosition in the list. This base class simply returns an error as
we support no media types by default but derived classes should override */

HRESULT CBasePin::GetMediaType(int iPosition, CMediaType *pMediaType)
{
	UNREFERENCED_PARAMETER(iPosition);
	UNREFERENCED_PARAMETER(pMediaType);
	return E_UNEXPECTED;
}


/* This is a virtual function that returns the current media type version.
The base class initialises the media type enumerators with the value 1
By default we always returns that same value. A Derived class may change
the list of media types available and after doing so it should increment
the version either in a method derived from this, or more simply by just
incrementing the m_TypeVersion base pin variable. The type enumerators
call this when they want to see if their enumerations are out of date */

LONG CBasePin::GetMediaTypeVersion()
{
	return m_TypeVersion;
}


/* Increment the cookie representing the current media type version */

void CBasePin::IncrementTypeVersion()
{
	InterlockedIncrement(&m_TypeVersion);
}


/* Called by IMediaFilter implementation when the state changes from Stopped
to either paused or running and in derived classes could do things like
commit memory and grab hardware resource (the default is to do nothing) */

HRESULT
CBasePin::Active(void)
{
	return NOERROR;
}

/* Called by IMediaFilter implementation when the state changes from
to either paused to running and in derived classes could do things like
commit memory and grab hardware resource (the default is to do nothing) */

HRESULT
CBasePin::Run(REFERENCE_TIME tStart)
{
	UNREFERENCED_PARAMETER(tStart);
	return NOERROR;
}


/* Also called by the IMediaFilter implementation when the state changes to
Stopped at which point you should decommit allocators and free hardware
resources you grabbed in the Active call (default is also to do nothing) */

HRESULT
CBasePin::Inactive(void)
{
	m_bRunTimeError = FALSE;
	return NOERROR;
}


// Called when no more data will arrive
STDMETHODIMP
CBasePin::EndOfStream(void)
{
	return S_OK;
}


STDMETHODIMP
CBasePin::SetSink(IQualityControl * piqc)
{
	CComAutoLock cObjectLock(m_pLock);
	if (piqc) ValidateReadPtr(piqc,sizeof(IQualityControl));
	m_pQSink = piqc;
	return NOERROR;
} // SetSink


STDMETHODIMP
CBasePin::Notify(IBaseFilter * pSender, Quality q)
{
	UNREFERENCED_PARAMETER(q);
	UNREFERENCED_PARAMETER(pSender);
	DbgBreak("IQualityControl::Notify not over-ridden from CBasePin.  (IGNORE is OK)");
	return E_NOTIMPL;
} //Notify


// NewSegment notifies of the start/stop/rate applying to the data
// about to be received. Default implementation records data and
// returns S_OK.
// Override this to pass downstream.
STDMETHODIMP
CBasePin::NewSegment(
					 REFERENCE_TIME tStart,
					 REFERENCE_TIME tStop,
					 double dRate)
{
	m_tStart = tStart;
	m_tStop = tStop;
	m_dRate = dRate;

	return S_OK;
}
