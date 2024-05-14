#include "StdAfx.h"


//=====================================================================
//=====================================================================
// Implements CBaseOutputPin
//=====================================================================
//=====================================================================


CBaseOutputPin::CBaseOutputPin(TCHAR *pObjectName,
							   CBaseFilter *pFilter,
							   CCritSec *pLock,
							   HRESULT *phr,
							   LPCWSTR pName) :
CBasePin(pObjectName, pFilter, pLock, phr, pName, PINDIR_OUTPUT),
m_pAllocator(NULL),
m_pInputPin(NULL)
{
	ASSERT(pFilter);
}

#ifdef UNICODE
CBaseOutputPin::CBaseOutputPin(CHAR *pObjectName,
							   CBaseFilter *pFilter,
							   CCritSec *pLock,
							   HRESULT *phr,
							   LPCWSTR pName) :
CBasePin(pObjectName, pFilter, pLock, phr, pName, PINDIR_OUTPUT),
m_pAllocator(NULL),
m_pInputPin(NULL)
{
	ASSERT(pFilter);
}
#endif

/*   This is called after a media type has been proposed

Try to complete the connection by agreeing the allocator
*/
HRESULT
CBaseOutputPin::CompleteConnect(IPin *pReceivePin)
{
	UNREFERENCED_PARAMETER(pReceivePin);
	return DecideAllocator(m_pInputPin, &m_pAllocator);
}


/* This method is called when the output pin is about to try and connect to
an input pin. It is at this point that you should try and grab any extra
interfaces that you need, in this case IMemInputPin. Because this is
only called if we are not currently connected we do NOT need to call
BreakConnect. This also makes it easier to derive classes from us as
BreakConnect is only called when we actually have to break a connection
(or a partly made connection) and not when we are checking a connection */

/* Overriden from CBasePin */

HRESULT
CBaseOutputPin::CheckConnect(IPin * pPin)
{
	HRESULT hr = CBasePin::CheckConnect(pPin);
	if (FAILED(hr)) {
		return hr;
	}

	// get an input pin and an allocator interface
	hr = pPin->QueryInterface(IID_IMemInputPin, (void **) &m_pInputPin);
	if (FAILED(hr)) {
		return hr;
	}
	return NOERROR;
}


/* Overriden from CBasePin */

HRESULT
CBaseOutputPin::BreakConnect()
{
	/* Release any allocator we hold */

	if (m_pAllocator) {
		// Always decommit the allocator because a downstream filter may or
		// may not decommit the connection's allocator.  A memory leak could
		// occur if the allocator is not decommited when a connection is broken.
		HRESULT hr = m_pAllocator->Decommit();
		if( FAILED( hr ) ) {
			return hr;
		}

		m_pAllocator->Release();
		m_pAllocator = NULL;
	}

	/* Release any input pin interface we hold */

	if (m_pInputPin) {
		m_pInputPin->Release();
		m_pInputPin = NULL;
	}
	return NOERROR;
}


/* This is called when the input pin didn't give us a valid allocator */

HRESULT
CBaseOutputPin::InitAllocator(IMemAllocator **ppAlloc)
{
	return CreateMemoryAllocator(ppAlloc);
}


/* Decide on an allocator, override this if you want to use your own allocator
Override DecideBufferSize to call SetProperties. If the input pin fails
the GetAllocator call then this will construct a CMemAllocator and call
DecideBufferSize on that, and if that fails then we are completely hosed.
If the you succeed the DecideBufferSize call, we will notify the input
pin of the selected allocator. NOTE this is called during Connect() which
therefore looks after grabbing and locking the object's critical section */

// We query the input pin for its requested properties and pass this to
// DecideBufferSize to allow it to fulfill requests that it is happy
// with (eg most people don't care about alignment and are thus happy to
// use the downstream pin's alignment request).

HRESULT
CBaseOutputPin::DecideAllocator(IMemInputPin *pPin, IMemAllocator **ppAlloc)
{
	HRESULT hr = NOERROR;
	*ppAlloc = NULL;

	// get downstream prop request
	// the derived class may modify this in DecideBufferSize, but
	// we assume that he will consistently modify it the same way,
	// so we only get it once
	ALLOCATOR_PROPERTIES prop;
	ZeroMemory(&prop, sizeof(prop));

	// whatever he returns, we assume prop is either all zeros
	// or he has filled it out.
	pPin->GetAllocatorRequirements(&prop);

	// if he doesn't care about alignment, then set it to 1
	if (prop.cbAlign == 0) {
		prop.cbAlign = 1;
	}

	/* Try the allocator provided by the input pin */

	hr = pPin->GetAllocator(ppAlloc);
	if (SUCCEEDED(hr)) {

		hr = DecideBufferSize(*ppAlloc, &prop);
		if (SUCCEEDED(hr)) {
			hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
			if (SUCCEEDED(hr)) {
				return NOERROR;
			}
		}
	}

	/* If the GetAllocator failed we may not have an interface */

	if (*ppAlloc) {
		(*ppAlloc)->Release();
		*ppAlloc = NULL;
	}

	/* Try the output pin's allocator by the same method */

	hr = InitAllocator(ppAlloc);
	if (SUCCEEDED(hr)) {

		// note - the properties passed here are in the same
		// structure as above and may have been modified by
		// the previous call to DecideBufferSize
		hr = DecideBufferSize(*ppAlloc, &prop);
		if (SUCCEEDED(hr)) {
			hr = pPin->NotifyAllocator(*ppAlloc, FALSE);
			if (SUCCEEDED(hr)) {
				return NOERROR;
			}
		}
	}

	/* Likewise we may not have an interface to release */

	if (*ppAlloc) {
		(*ppAlloc)->Release();
		*ppAlloc = NULL;
	}
	return hr;
}


/* This returns an empty sample buffer from the allocator WARNING the same
dangers and restrictions apply here as described below for Deliver() */

HRESULT
CBaseOutputPin::GetDeliveryBuffer(IMediaSample ** ppSample,
								  REFERENCE_TIME * pStartTime,
								  REFERENCE_TIME * pEndTime,
								  DWORD dwFlags)
{
	if (m_pAllocator != NULL) {
		return m_pAllocator->GetBuffer(ppSample,pStartTime,pEndTime,dwFlags);
	} else {
		return E_NOINTERFACE;
	}
}


/* Deliver a filled-in sample to the connected input pin. NOTE the object must
have locked itself before calling us otherwise we may get halfway through
executing this method only to find the filter graph has got in and
disconnected us from the input pin. If the filter has no worker threads
then the lock is best applied on Receive(), otherwise it should be done
when the worker thread is ready to deliver. There is a wee snag to worker
threads that this shows up. The worker thread must lock the object when
it is ready to deliver a sample, but it may have to wait until a state
change has completed, but that may never complete because the state change
is waiting for the worker thread to complete. The way to handle this is for
the state change code to grab the critical section, then set an abort event
for the worker thread, then release the critical section and wait for the
worker thread to see the event we set and then signal that it has finished
(with another event). At which point the state change code can complete */

// note (if you've still got any breath left after reading that) that you
// need to release the sample yourself after this call. if the connected
// input pin needs to hold onto the sample beyond the call, it will addref
// the sample itself.

// of course you must release this one and call GetDeliveryBuffer for the
// next. You cannot reuse it directly.

HRESULT
CBaseOutputPin::Deliver(IMediaSample * pSample)
{
	if (m_pInputPin == NULL) {
		return VFW_E_NOT_CONNECTED;
	}


	return m_pInputPin->Receive(pSample);
}


// called from elsewhere in our filter to pass EOS downstream to
// our connected input pin
HRESULT
CBaseOutputPin::DeliverEndOfStream(void)
{
	// remember this is on IPin not IMemInputPin
	if (m_Connected == NULL) {
		return VFW_E_NOT_CONNECTED;
	}
	return m_Connected->EndOfStream();
}


/* Commit the allocator's memory, this is called through IMediaFilter
which is responsible for locking the object before calling us */

HRESULT
CBaseOutputPin::Active(void)
{
	if (m_pAllocator == NULL) {
		return VFW_E_NO_ALLOCATOR;
	}
	return m_pAllocator->Commit();
}


/* Free up or unprepare allocator's memory, this is called through
IMediaFilter which is responsible for locking the object first */

HRESULT
CBaseOutputPin::Inactive(void)
{
	m_bRunTimeError = FALSE;
	if (m_pAllocator == NULL) {
		return VFW_E_NO_ALLOCATOR;
	}
	return m_pAllocator->Decommit();
}

// we have a default handling of EndOfStream which is to return
// an error, since this should be called on input pins only
STDMETHODIMP
CBaseOutputPin::EndOfStream(void)
{
	return E_UNEXPECTED;
}


// BeginFlush should be called on input pins only
STDMETHODIMP
CBaseOutputPin::BeginFlush(void)
{
	return E_UNEXPECTED;
}

// EndFlush should be called on input pins only
STDMETHODIMP
CBaseOutputPin::EndFlush(void)
{
	return E_UNEXPECTED;
}

// call BeginFlush on the connected input pin
HRESULT
CBaseOutputPin::DeliverBeginFlush(void)
{
	// remember this is on IPin not IMemInputPin
	if (m_Connected == NULL) {
		return VFW_E_NOT_CONNECTED;
	}
	return m_Connected->BeginFlush();
}

// call EndFlush on the connected input pin
HRESULT
CBaseOutputPin::DeliverEndFlush(void)
{
	// remember this is on IPin not IMemInputPin
	if (m_Connected == NULL) {
		return VFW_E_NOT_CONNECTED;
	}
	return m_Connected->EndFlush();
}
// deliver NewSegment to connected pin
HRESULT
CBaseOutputPin::DeliverNewSegment(
								  REFERENCE_TIME tStart,
								  REFERENCE_TIME tStop,
								  double dRate)
{
	if (m_Connected == NULL) {
		return VFW_E_NOT_CONNECTED;
	}
	return m_Connected->NewSegment(tStart, tStop, dRate);
}
