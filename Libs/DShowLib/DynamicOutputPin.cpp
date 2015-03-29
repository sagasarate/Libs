#include "StdAfx.h"

//
// The streaming thread calls IPin::NewSegment(), IPin::EndOfStream(),
// IMemInputPin::Receive() and IMemInputPin::ReceiveMultiple() on the
// connected input pin.  The application thread calls Block().  The
// following class members can only be called by the streaming thread.
//
//    Deliver()
//    DeliverNewSegment()
//    StartUsingOutputPin()
//    StopUsingOutputPin()
//    ChangeOutputFormat()
//    ChangeMediaType()
//    DynamicReconnect()
//
// The following class members can only be called by the application thread.
//
//    Block()
//    SynchronousBlockOutputPin()
//    AsynchronousBlockOutputPin()
//

CDynamicOutputPin::CDynamicOutputPin(
									 TCHAR *pObjectName,
									 CBaseFilter *pFilter,
									 CCritSec *pLock,
									 HRESULT *phr,
									 LPCWSTR pName) :
CBaseOutputPin(pObjectName, pFilter, pLock, phr, pName),
m_hStopEvent(NULL),
m_pGraphConfig(NULL),
m_bPinUsesReadOnlyAllocator(FALSE),
m_BlockState(NOT_BLOCKED),
m_hUnblockOutputPinEvent(NULL),
m_hNotifyCallerPinBlockedEvent(NULL),
m_dwBlockCallerThreadID(0),
m_dwNumOutstandingOutputPinUsers(0)
{
	HRESULT hr = Initialize();
	if( FAILED( hr ) ) {
		*phr = hr;
		return;
	}
}

#ifdef UNICODE
CDynamicOutputPin::CDynamicOutputPin(
									 CHAR *pObjectName,
									 CBaseFilter *pFilter,
									 CCritSec *pLock,
									 HRESULT *phr,
									 LPCWSTR pName) :
CBaseOutputPin(pObjectName, pFilter, pLock, phr, pName),
m_hStopEvent(NULL),
m_pGraphConfig(NULL),
m_bPinUsesReadOnlyAllocator(FALSE),
m_BlockState(NOT_BLOCKED),
m_hUnblockOutputPinEvent(NULL),
m_hNotifyCallerPinBlockedEvent(NULL),
m_dwBlockCallerThreadID(0),
m_dwNumOutstandingOutputPinUsers(0)
{
	HRESULT hr = Initialize();
	if( FAILED( hr ) ) {
		*phr = hr;
		return;
	}
}
#endif

CDynamicOutputPin::~CDynamicOutputPin()
{
	if(NULL != m_hUnblockOutputPinEvent) {
		// This call should not fail because we have access to m_hUnblockOutputPinEvent
		// and m_hUnblockOutputPinEvent is a valid event.
		EXECUTE_ASSERT(::CloseHandle(m_hUnblockOutputPinEvent));
	}

	if(NULL != m_hNotifyCallerPinBlockedEvent) {
		// This call should not fail because we have access to m_hNotifyCallerPinBlockedEvent
		// and m_hNotifyCallerPinBlockedEvent is a valid event.
		EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));
	}
}

HRESULT CDynamicOutputPin::Initialize(void)
{
	m_hUnblockOutputPinEvent = ::CreateEvent( NULL,   // The event will have the default security descriptor.
		TRUE,   // This is a manual reset event.
		TRUE,   // The event is initially signaled.
		NULL ); // The event is not named.

	// CreateEvent() returns NULL if an error occurs.
	if(NULL == m_hUnblockOutputPinEvent) {
		return AmGetLastErrorToHResult();
	}

	//  Set flag to say we can reconnect while streaming.
	SetReconnectWhenActive(true);

	return S_OK;
}

STDMETHODIMP CDynamicOutputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	if(riid == IID_IPinFlowControl) {
		return GetInterface(static_cast<IPinFlowControl*>(this), ppv);
	} else {
		return CBaseOutputPin::NonDelegatingQueryInterface(riid, ppv);
	}
}

STDMETHODIMP CDynamicOutputPin::Disconnect(void)
{
	CComAutoLock cObjectLock(m_pLock);
	return DisconnectInternal();
}

STDMETHODIMP CDynamicOutputPin::Block(DWORD dwBlockFlags, HANDLE hEvent)
{
	const DWORD VALID_FLAGS = AM_PIN_FLOW_CONTROL_BLOCK;

	// Check for illegal flags.
	if(dwBlockFlags & ~VALID_FLAGS) {
		return E_INVALIDARG;
	}

	// Make sure the event is unsignaled.
	if((dwBlockFlags & AM_PIN_FLOW_CONTROL_BLOCK) && (NULL != hEvent)) {
		if( !::ResetEvent( hEvent ) ) {
			return AmGetLastErrorToHResult();
		}
	}

	// No flags are set if we are unblocking the output pin.
	if(0 == dwBlockFlags) {

		// This parameter should be NULL because unblock operations are always synchronous.
		// There is no need to notify the caller when the event is done.
		if(NULL != hEvent) {
			return E_INVALIDARG;
		}
	}

#ifdef DEBUG
	AssertValid();
#endif // DEBUG

	HRESULT hr;

	if(dwBlockFlags & AM_PIN_FLOW_CONTROL_BLOCK) {
		// IPinFlowControl::Block()'s hEvent parameter is NULL if the block is synchronous.
		// If hEvent is not NULL, the block is asynchronous.
		if(NULL == hEvent) {
			hr = SynchronousBlockOutputPin();
		} else {
			hr = AsynchronousBlockOutputPin(hEvent);
		}
	} else {
		hr = UnblockOutputPin();
	}

#ifdef DEBUG
	AssertValid();
#endif // DEBUG

	if(FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

HRESULT CDynamicOutputPin::SynchronousBlockOutputPin(void)
{
	HANDLE hNotifyCallerPinBlockedEvent = :: CreateEvent( NULL,   // The event will have the default security attributes.
		FALSE,  // This is an automatic reset event.
		FALSE,  // The event is initially unsignaled.
		NULL ); // The event is not named.

	// CreateEvent() returns NULL if an error occurs.
	if(NULL == hNotifyCallerPinBlockedEvent) {
		return AmGetLastErrorToHResult();
	}

	HRESULT hr = AsynchronousBlockOutputPin(hNotifyCallerPinBlockedEvent);
	if(FAILED(hr)) {
		// This call should not fail because we have access to hNotifyCallerPinBlockedEvent
		// and hNotifyCallerPinBlockedEvent is a valid event.
		EXECUTE_ASSERT(::CloseHandle(hNotifyCallerPinBlockedEvent));

		return hr;
	}

	hr = WaitEvent(hNotifyCallerPinBlockedEvent);

	// This call should not fail because we have access to hNotifyCallerPinBlockedEvent
	// and hNotifyCallerPinBlockedEvent is a valid event.
	EXECUTE_ASSERT(::CloseHandle(hNotifyCallerPinBlockedEvent));

	if(FAILED(hr)) {
		return hr;
	}

	return S_OK;
}

HRESULT CDynamicOutputPin::AsynchronousBlockOutputPin(HANDLE hNotifyCallerPinBlockedEvent)
{
	// This function holds the m_BlockStateLock because it uses
	// m_dwBlockCallerThreadID, m_BlockState and
	// m_hNotifyCallerPinBlockedEvent.
	CComAutoLock alBlockStateLock(&m_BlockStateLock);

	if(NOT_BLOCKED != m_BlockState) {
		if(m_dwBlockCallerThreadID == ::GetCurrentThreadId()) {
			return VFW_E_PIN_ALREADY_BLOCKED_ON_THIS_THREAD;
		} else {
			return VFW_E_PIN_ALREADY_BLOCKED;
		}
	}

	BOOL fSuccess = ::DuplicateHandle( ::GetCurrentProcess(),
		hNotifyCallerPinBlockedEvent,
		::GetCurrentProcess(),
		&m_hNotifyCallerPinBlockedEvent,
		EVENT_MODIFY_STATE,
		FALSE,
		0 );
	if( !fSuccess ) {
		return AmGetLastErrorToHResult();
	}

	m_BlockState = PENDING;
	m_dwBlockCallerThreadID = ::GetCurrentThreadId();

	// The output pin cannot be blocked if the streaming thread is
	// calling IPin::NewSegment(), IPin::EndOfStream(), IMemInputPin::Receive()
	// or IMemInputPin::ReceiveMultiple() on the connected input pin.  Also, it
	// cannot be blocked if the streaming thread is calling DynamicReconnect(),
	// ChangeMediaType() or ChangeOutputFormat().
	if(!StreamingThreadUsingOutputPin()) {

		// The output pin can be immediately blocked.
		BlockOutputPin();
	}

	return S_OK;
}

void CDynamicOutputPin::BlockOutputPin(void)
{
	// The caller should always hold the m_BlockStateLock because this function
	// uses m_BlockState and m_hNotifyCallerPinBlockedEvent.
	ASSERT(CritCheckIn(&m_BlockStateLock));

	// This function should not be called if the streaming thread is modifying
	// the connection state or it's passing data downstream.
	ASSERT(!StreamingThreadUsingOutputPin());

	// This should not fail because we successfully created the event
	// and we have the security permissions to change it's state.
	EXECUTE_ASSERT(::ResetEvent(m_hUnblockOutputPinEvent));

	// This event should not fail because AsynchronousBlockOutputPin() successfully
	// duplicated this handle and we have the appropriate security permissions.
	EXECUTE_ASSERT(::SetEvent(m_hNotifyCallerPinBlockedEvent));
	EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));

	m_BlockState = BLOCKED;
	m_hNotifyCallerPinBlockedEvent = NULL;
}

HRESULT CDynamicOutputPin::UnblockOutputPin(void)
{
	// UnblockOutputPin() holds the m_BlockStateLock because it
	// uses m_BlockState, m_dwBlockCallerThreadID and
	// m_hNotifyCallerPinBlockedEvent.
	CComAutoLock alBlockStateLock(&m_BlockStateLock);

	if(NOT_BLOCKED == m_BlockState) {
		return S_FALSE;
	}

	// This should not fail because we successfully created the event
	// and we have the security permissions to change it's state.
	EXECUTE_ASSERT(::SetEvent(m_hUnblockOutputPinEvent));

	// Cancel the block operation if it's still pending.
	if(NULL != m_hNotifyCallerPinBlockedEvent) {
		// This event should not fail because AsynchronousBlockOutputPin() successfully
		// duplicated this handle and we have the appropriate security permissions.
		EXECUTE_ASSERT(::SetEvent(m_hNotifyCallerPinBlockedEvent));
		EXECUTE_ASSERT(::CloseHandle(m_hNotifyCallerPinBlockedEvent));
	}

	m_BlockState = NOT_BLOCKED;
	m_dwBlockCallerThreadID = 0;
	m_hNotifyCallerPinBlockedEvent = NULL;

	return S_OK;
}

HRESULT CDynamicOutputPin::StartUsingOutputPin(void)
{
	// The caller should not hold m_BlockStateLock.  If the caller does,
	// a deadlock could occur.
	ASSERT(CritCheckOut(&m_BlockStateLock));

	CComAutoLock alBlockStateLock(&m_BlockStateLock);

#ifdef DEBUG
	AssertValid();
#endif // DEBUG

	// Are we in the middle of a block operation?
	while(BLOCKED == m_BlockState) {
		m_BlockStateLock.Unlock();

		// If this ASSERT fires, a deadlock could occur.  The caller should make sure
		// that this thread never acquires the Block State lock more than once.
		ASSERT(CritCheckOut( &m_BlockStateLock ));

		// WaitForMultipleObjects() returns WAIT_OBJECT_0 if the unblock event
		// is fired.  It returns WAIT_OBJECT_0 + 1 if the stop event if fired.
		// See the Windows SDK documentation for more information on
		// WaitForMultipleObjects().
		const DWORD UNBLOCK = WAIT_OBJECT_0;
		const DWORD STOP = WAIT_OBJECT_0 + 1;

		HANDLE ahWaitEvents[] = { m_hUnblockOutputPinEvent, m_hStopEvent };
		DWORD dwNumWaitEvents = sizeof(ahWaitEvents)/sizeof(HANDLE);

		DWORD dwReturnValue = ::WaitForMultipleObjects( dwNumWaitEvents, ahWaitEvents, FALSE, INFINITE );

		m_BlockStateLock.Lock();

#ifdef DEBUG
		AssertValid();
#endif // DEBUG

		switch( dwReturnValue ) {
		case UNBLOCK:
			break;

		case STOP:
			return VFW_E_STATE_CHANGED;

		case WAIT_FAILED:
			return AmGetLastErrorToHResult();

		default:
			DbgBreak( "An Unexpected case occured in CDynamicOutputPin::StartUsingOutputPin()." );
			return E_UNEXPECTED;
		}
	}

	m_dwNumOutstandingOutputPinUsers++;

#ifdef DEBUG
	AssertValid();
#endif // DEBUG

	return S_OK;
}

void CDynamicOutputPin::StopUsingOutputPin(void)
{
	CComAutoLock alBlockStateLock(&m_BlockStateLock);

#ifdef DEBUG
	AssertValid();
#endif // DEBUG

	m_dwNumOutstandingOutputPinUsers--;

	if((m_dwNumOutstandingOutputPinUsers == 0) && (NOT_BLOCKED != m_BlockState)) {
		BlockOutputPin();
	}

#ifdef DEBUG
	AssertValid();
#endif // DEBUG
}

bool CDynamicOutputPin::StreamingThreadUsingOutputPin(void)
{
	CComAutoLock alBlockStateLock(&m_BlockStateLock);

	return (m_dwNumOutstandingOutputPinUsers > 0);
}

void CDynamicOutputPin::SetConfigInfo(IGraphConfig *pGraphConfig, HANDLE hStopEvent)
{
	// This pointer is not addrefed because filters are not allowed to
	// hold references to the filter graph manager.  See the documentation for
	// IBaseFilter::JoinFilterGraph() in the Direct Show SDK for more information.
	m_pGraphConfig = pGraphConfig;

	m_hStopEvent = hStopEvent;
}

HRESULT CDynamicOutputPin::Active(void)
{
	// Make sure the user initialized the object by calling SetConfigInfo().
	if((NULL == m_hStopEvent) || (NULL == m_pGraphConfig)) {
		DbgBreak( ERROR: CDynamicOutputPin::Active() failed because m_pGraphConfig and m_hStopEvent were not initialized.  Call SetConfigInfo() to initialize them. );
		return E_FAIL;
	}

	// If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
	// The ASSERT can also fire if the event if destroyed and then Active() is called.  An event
	// handle is invalid if 1) the event does not exist or the user does not have the security
	// permissions to use the event.
	EXECUTE_ASSERT(ResetEvent(m_hStopEvent));

	return CBaseOutputPin::Active();
}

HRESULT CDynamicOutputPin::Inactive(void)
{
	// If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
	// The ASSERT can also fire if the event if destroyed and then Active() is called.  An event
	// handle is invalid if 1) the event does not exist or the user does not have the security
	// permissions to use the event.
	EXECUTE_ASSERT(SetEvent(m_hStopEvent));

	return CBaseOutputPin::Inactive();
}

HRESULT CDynamicOutputPin::DeliverBeginFlush(void)
{
	// If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
	// The ASSERT can also fire if the event if destroyed and then DeliverBeginFlush() is called.
	// An event handle is invalid if 1) the event does not exist or the user does not have the security
	// permissions to use the event.
	EXECUTE_ASSERT(SetEvent(m_hStopEvent));

	return CBaseOutputPin::DeliverBeginFlush();
}

HRESULT CDynamicOutputPin::DeliverEndFlush(void)
{
	// If this ASSERT fires, the user may have passed an invalid event handle to SetConfigInfo().
	// The ASSERT can also fire if the event if destroyed and then DeliverBeginFlush() is called.
	// An event handle is invalid if 1) the event does not exist or the user does not have the security
	// permissions to use the event.
	EXECUTE_ASSERT(ResetEvent(m_hStopEvent));

	return CBaseOutputPin::DeliverEndFlush();
}


// ChangeOutputFormat() either dynamicly changes the connection's format type or it dynamicly
// reconnects the output pin.
HRESULT CDynamicOutputPin::ChangeOutputFormat
(
 const AM_MEDIA_TYPE *pmt,
 REFERENCE_TIME tSegmentStart,
 REFERENCE_TIME tSegmentStop,
 double dSegmentRate
 )
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	// Callers should always pass a valid media type to ChangeOutputFormat() .
	ASSERT(NULL != pmt);

	CMediaType cmt(*pmt);
	HRESULT hr = ChangeMediaType(&cmt);
	if (FAILED(hr)) {
		return hr;
	}

	hr = DeliverNewSegment(tSegmentStart, tSegmentStop, dSegmentRate);
	if( FAILED( hr ) ) {
		return hr;
	}

	return S_OK;
}

HRESULT CDynamicOutputPin::ChangeMediaType(const CMediaType *pmt)
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	// This function assumes the filter graph is running.
	ASSERT(!IsStopped());

	if(!IsConnected()) {
		return VFW_E_NOT_CONNECTED;
	}

	/*  First check if the downstream pin will accept a dynamic
	format change
	*/
	QzCComPtr<IPinConnection> pConnection;

	m_Connected->QueryInterface(IID_IPinConnection, (void **)&pConnection);
	if(pConnection != NULL) {

		if(S_OK == pConnection->DynamicQueryAccept(pmt)) {

			HRESULT hr = ChangeMediaTypeHelper(pmt);
			if(FAILED(hr)) {
				return hr;
			}

			return S_OK;
		}
	}

	/*  Can't do the dynamic connection */
	return DynamicReconnect(pmt);
}

HRESULT CDynamicOutputPin::ChangeMediaTypeHelper(const CMediaType *pmt)
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	HRESULT hr = m_Connected->ReceiveConnection(this, pmt);
	if(FAILED(hr)) {
		return hr;
	}

	hr = SetMediaType(pmt);
	if(FAILED(hr)) {
		return hr;
	}

	// Does this pin use the local memory transport?
	if(NULL != m_pInputPin) {
		// This function assumes that m_pInputPin and m_Connected are
		// two different interfaces to the same object.
		ASSERT(::IsEqualObject(m_Connected, m_pInputPin));

		ALLOCATOR_PROPERTIES apInputPinRequirements;
		apInputPinRequirements.cbAlign = 0;
		apInputPinRequirements.cbBuffer = 0;
		apInputPinRequirements.cbPrefix = 0;
		apInputPinRequirements.cBuffers = 0;

		m_pInputPin->GetAllocatorRequirements(&apInputPinRequirements);

		// A zero allignment does not make any sense.
		if(0 == apInputPinRequirements.cbAlign) {
			apInputPinRequirements.cbAlign = 1;
		}

		hr = m_pAllocator->Decommit();
		if(FAILED(hr)) {
			return hr;
		}

		hr = DecideBufferSize(m_pAllocator,  &apInputPinRequirements);
		if(FAILED(hr)) {
			return hr;
		}

		hr = m_pAllocator->Commit();
		if(FAILED(hr)) {
			return hr;
		}

		hr = m_pInputPin->NotifyAllocator(m_pAllocator, m_bPinUsesReadOnlyAllocator);
		if(FAILED(hr)) {
			return hr;
		}
	}

	return S_OK;
}

// this method has to be called from the thread that is pushing data,
// and it's the caller's responsibility to make sure that the thread
// has no outstand samples because they cannot be delivered after a
// reconnect
//
HRESULT CDynamicOutputPin::DynamicReconnect( const CMediaType* pmt )
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	if((m_pGraphConfig == NULL) || (NULL == m_hStopEvent)) {
		return E_FAIL;
	}

	HRESULT hr = m_pGraphConfig->Reconnect(
		this,
		NULL,
		pmt,
		NULL,
		m_hStopEvent,
		AM_GRAPH_CONFIG_RECONNECT_CACHE_REMOVED_FILTERS );

	return hr;
}

HRESULT CDynamicOutputPin::CompleteConnect(IPin *pReceivePin)
{
	HRESULT hr = CBaseOutputPin::CompleteConnect(pReceivePin);
	if(SUCCEEDED(hr)) {
		if(!IsStopped() && m_pAllocator) {
			hr = m_pAllocator->Commit();
			ASSERT(hr != VFW_E_ALREADY_COMMITTED);
		}
	}

	return hr;
}

#ifdef DEBUG
void CDynamicOutputPin::AssertValid(void)
{
	// Make sure the object was correctly initialized.

	// This ASSERT only fires if the object failed to initialize
	// and the user ignored the constructor's return code (phr).
	ASSERT(NULL != m_hUnblockOutputPinEvent);

	// If either of these ASSERTs fire, the user did not correctly call
	// SetConfigInfo().
	ASSERT(NULL != m_hStopEvent);
	ASSERT(NULL != m_pGraphConfig);

	// Make sure the block state is consistent.

	CComAutoLock alBlockStateLock(&m_BlockStateLock);

	// BLOCK_STATE variables only have three legal values: PENDING, BLOCKED and NOT_BLOCKED.
	ASSERT((NOT_BLOCKED == m_BlockState) || (PENDING == m_BlockState) || (BLOCKED == m_BlockState));

	// m_hNotifyCallerPinBlockedEvent is only needed when a block operation cannot complete
	// immediately.
	ASSERT(((NULL == m_hNotifyCallerPinBlockedEvent) && (PENDING != m_BlockState)) ||
		((NULL != m_hNotifyCallerPinBlockedEvent) && (PENDING == m_BlockState)) );

	// m_dwBlockCallerThreadID should always be 0 if the pin is not blocked and
	// the user is not trying to block the pin.
	ASSERT((0 == m_dwBlockCallerThreadID) || (NOT_BLOCKED != m_BlockState));

	// If this ASSERT fires, the streaming thread is using the output pin and the
	// output pin is blocked.
	ASSERT(((0 != m_dwNumOutstandingOutputPinUsers) && (BLOCKED != m_BlockState)) ||
		((0 == m_dwNumOutstandingOutputPinUsers) && (NOT_BLOCKED != m_BlockState)) ||
		((0 == m_dwNumOutstandingOutputPinUsers) && (NOT_BLOCKED == m_BlockState)) );
}
#endif // DEBUG

HRESULT CDynamicOutputPin::WaitEvent(HANDLE hEvent)
{
	const DWORD EVENT_SIGNALED = WAIT_OBJECT_0;

	DWORD dwReturnValue = ::WaitForSingleObject(hEvent, INFINITE);

	switch( dwReturnValue ) {
	case EVENT_SIGNALED:
		return S_OK;

	case WAIT_FAILED:
		return AmGetLastErrorToHResult();

	default:
		DbgBreak( "An Unexpected case occured in CDynamicOutputPin::WaitEvent()." );
		return E_UNEXPECTED;
	}
}