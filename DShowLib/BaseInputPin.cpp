#include "StdAfx.h"

//=====================================================================
//=====================================================================
// Implements CBaseInputPin
//=====================================================================
//=====================================================================


/* Constructor creates a default allocator object */

CBaseInputPin::CBaseInputPin(TCHAR *pObjectName,
							 CBaseFilter *pFilter,
							 CCritSec *pLock,
							 HRESULT *phr,
							 LPCWSTR pPinName) :
CBasePin(pObjectName, pFilter, pLock, phr, pPinName, PINDIR_INPUT),
m_pAllocator(NULL),
m_bReadOnly(FALSE),
m_bFlushing(FALSE)
{
	ZeroMemory(&m_SampleProps, sizeof(m_SampleProps));
}

#ifdef UNICODE
CBaseInputPin::CBaseInputPin(CHAR *pObjectName,
							 CBaseFilter *pFilter,
							 CCritSec *pLock,
							 HRESULT *phr,
							 LPCWSTR pPinName) :
CBasePin(pObjectName, pFilter, pLock, phr, pPinName, PINDIR_INPUT),
m_pAllocator(NULL),
m_bReadOnly(FALSE),
m_bFlushing(FALSE)
{
	ZeroMemory(&m_SampleProps, sizeof(m_SampleProps));
}
#endif

/* Destructor releases it's reference count on the default allocator */

CBaseInputPin::~CBaseInputPin()
{
	if (m_pAllocator != NULL) {
		m_pAllocator->Release();
		m_pAllocator = NULL;
	}
}


// override this to publicise our interfaces
STDMETHODIMP
CBaseInputPin::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	/* Do we know about this interface */

	if (riid == IID_IMemInputPin) {
		return GetInterface((IMemInputPin *) this, ppv);
	} else {
		return CBasePin::NonDelegatingQueryInterface(riid, ppv);
	}
}


/* Return the allocator interface that this input pin would like the output
pin to use. NOTE subsequent calls to GetAllocator should all return an
interface onto the SAME object so we create one object at the start

Note:
The allocator is Release()'d on disconnect and replaced on
NotifyAllocator().

Override this to provide your own allocator.
*/

STDMETHODIMP
CBaseInputPin::GetAllocator(
							IMemAllocator **ppAllocator)
{
	CheckPointer(ppAllocator,E_POINTER);
	ValidateReadWritePtr(ppAllocator,sizeof(IMemAllocator *));
	CComAutoLock cObjectLock(m_pLock);

	if (m_pAllocator == NULL) {
		HRESULT hr = CreateMemoryAllocator(&m_pAllocator);
		if (FAILED(hr)) {
			return hr;
		}
	}
	ASSERT(m_pAllocator != NULL);
	*ppAllocator = m_pAllocator;
	m_pAllocator->AddRef();
	return NOERROR;
}


/* Tell the input pin which allocator the output pin is actually going to use
Override this if you care - NOTE the locking we do both here and also in
GetAllocator is unnecessary but derived classes that do something useful
will undoubtedly have to lock the object so this might help remind people */

STDMETHODIMP
CBaseInputPin::NotifyAllocator(
							   IMemAllocator * pAllocator,
							   BOOL bReadOnly)
{
	CheckPointer(pAllocator,E_POINTER);
	ValidateReadPtr(pAllocator,sizeof(IMemAllocator));
	CComAutoLock cObjectLock(m_pLock);

	IMemAllocator *pOldAllocator = m_pAllocator;
	pAllocator->AddRef();
	m_pAllocator = pAllocator;

	if (pOldAllocator != NULL) {
		pOldAllocator->Release();
	}

	// the readonly flag indicates whether samples from this allocator should
	// be regarded as readonly - if true, then inplace transforms will not be
	// allowed.
	m_bReadOnly = (BYTE)bReadOnly;
	return NOERROR;
}


HRESULT
CBaseInputPin::BreakConnect()
{
	/* We don't need our allocator any more */
	if (m_pAllocator) {
		// Always decommit the allocator because a downstream filter may or
		// may not decommit the connection's allocator.  A memory leak could
		// occur if the allocator is not decommited when a pin is disconnected.
		HRESULT hr = m_pAllocator->Decommit();
		if( FAILED( hr ) ) {
			return hr;
		}

		m_pAllocator->Release();
		m_pAllocator = NULL;
	}

	return S_OK;
}


/* Do something with this media sample - this base class checks to see if the
format has changed with this media sample and if so checks that the filter
will accept it, generating a run time error if not. Once we have raised a
run time error we set a flag so that no more samples will be accepted

It is important that any filter should override this method and implement
synchronization so that samples are not processed when the pin is
disconnected etc
*/

STDMETHODIMP
CBaseInputPin::Receive(IMediaSample *pSample)
{
	CheckPointer(pSample,E_POINTER);
	ValidateReadPtr(pSample,sizeof(IMediaSample));
	ASSERT(pSample);

	HRESULT hr = CheckStreaming();
	if (S_OK != hr) {
		return hr;
	}



	/* Check for IMediaSample2 */
	IMediaSample2 *pSample2;
	if (SUCCEEDED(pSample->QueryInterface(IID_IMediaSample2, (void **)&pSample2))) {
		hr = pSample2->GetProperties(sizeof(m_SampleProps), (PBYTE)&m_SampleProps);
		pSample2->Release();
		if (FAILED(hr)) {
			return hr;
		}
	} else {
		/*  Get the properties the hard way */
		m_SampleProps.cbData = sizeof(m_SampleProps);
		m_SampleProps.dwTypeSpecificFlags = 0;
		m_SampleProps.dwStreamId = AM_STREAM_MEDIA;
		m_SampleProps.dwSampleFlags = 0;
		if (S_OK == pSample->IsDiscontinuity()) {
			m_SampleProps.dwSampleFlags |= AM_SAMPLE_DATADISCONTINUITY;
		}
		if (S_OK == pSample->IsPreroll()) {
			m_SampleProps.dwSampleFlags |= AM_SAMPLE_PREROLL;
		}
		if (S_OK == pSample->IsSyncPoint()) {
			m_SampleProps.dwSampleFlags |= AM_SAMPLE_SPLICEPOINT;
		}
		if (SUCCEEDED(pSample->GetTime(&m_SampleProps.tStart,
			&m_SampleProps.tStop))) {
				m_SampleProps.dwSampleFlags |= AM_SAMPLE_TIMEVALID |
					AM_SAMPLE_STOPVALID;
		}
		if (S_OK == pSample->GetMediaType(&m_SampleProps.pMediaType)) {
			m_SampleProps.dwSampleFlags |= AM_SAMPLE_TYPECHANGED;
		}
		pSample->GetPointer(&m_SampleProps.pbBuffer);
		m_SampleProps.lActual = pSample->GetActualDataLength();
		m_SampleProps.cbBuffer = pSample->GetSize();
	}

	/* Has the format changed in this sample */

	if (!(m_SampleProps.dwSampleFlags & AM_SAMPLE_TYPECHANGED)) {
		return NOERROR;
	}

	/* Check the derived class accepts this format */
	/* This shouldn't fail as the source must call QueryAccept first */

	hr = CheckMediaType((CMediaType *)m_SampleProps.pMediaType);

	if (hr == NOERROR) {
		return NOERROR;
	}

	/* Raise a runtime error if we fail the media type */

	m_bRunTimeError = TRUE;
	EndOfStream();
	m_pFilter->NotifyEvent(EC_ERRORABORT,VFW_E_TYPE_NOT_ACCEPTED,0);
	return VFW_E_INVALIDMEDIATYPE;
}


/*  Receive multiple samples */
STDMETHODIMP
CBaseInputPin::ReceiveMultiple (
								IMediaSample **pSamples,
								long nSamples,
								long *nSamplesProcessed)
{
	CheckPointer(pSamples,E_POINTER);
	ValidateReadPtr(pSamples,nSamples * sizeof(IMediaSample *));

	HRESULT hr = S_OK;
	*nSamplesProcessed = 0;
	while (nSamples-- > 0) {
		hr = Receive(pSamples[*nSamplesProcessed]);

		/*  S_FALSE means don't send any more */
		if (hr != S_OK) {
			break;
		}
		(*nSamplesProcessed)++;
	}
	return hr;
}

/*  See if Receive() might block */
STDMETHODIMP
CBaseInputPin::ReceiveCanBlock()
{
	/*  Ask all the output pins if they block
	If there are no output pin assume we do block
	*/
	int cPins = m_pFilter->GetPinCount();
	int cOutputPins = 0;
	for (int c = 0; c < cPins; c++) {
		CBasePin *pPin = m_pFilter->GetPin(c);
		PIN_DIRECTION pd;
		HRESULT hr = pPin->QueryDirection(&pd);
		if (FAILED(hr)) {
			return hr;
		}

		if (pd == PINDIR_OUTPUT) {

			IPin *pConnected;
			hr = pPin->ConnectedTo(&pConnected);
			if (SUCCEEDED(hr)) {
				ASSERT(pConnected != NULL);
				cOutputPins++;
				IMemInputPin *pInputPin;
				hr = pConnected->QueryInterface(
					IID_IMemInputPin,
					(void **)&pInputPin);
				pConnected->Release();
				if (SUCCEEDED(hr)) {
					hr = pInputPin->ReceiveCanBlock();
					pInputPin->Release();
					if (hr != S_FALSE) {
						return S_OK;
					}
				} else {
					/*  There's a transport we don't understand here */
					return S_OK;
				}
			}
		}
	}
	return cOutputPins == 0 ? S_OK : S_FALSE;
}

// Default handling for BeginFlush - call at the beginning
// of your implementation (makes sure that all Receive calls
// fail). After calling this, you need to free any queued data
// and then call downstream.
STDMETHODIMP
CBaseInputPin::BeginFlush(void)
{
	//  BeginFlush is NOT synchronized with streaming but is part of
	//  a control action - hence we synchronize with the filter
	CComAutoLock lck(m_pLock);

	// if we are already in mid-flush, this is probably a mistake
	// though not harmful - try to pick it up for now so I can think about it
	ASSERT(!m_bFlushing);

	// first thing to do is ensure that no further Receive calls succeed
	m_bFlushing = TRUE;

	// now discard any data and call downstream - must do that
	// in derived classes
	return S_OK;
}

// default handling for EndFlush - call at end of your implementation
// - before calling this, ensure that there is no queued data and no thread
// pushing any more without a further receive, then call downstream,
// then call this method to clear the m_bFlushing flag and re-enable
// receives
STDMETHODIMP
CBaseInputPin::EndFlush(void)
{
	//  Endlush is NOT synchronized with streaming but is part of
	//  a control action - hence we synchronize with the filter
	CComAutoLock lck(m_pLock);

	// almost certainly a mistake if we are not in mid-flush
	ASSERT(m_bFlushing);

	// before calling, sync with pushing thread and ensure
	// no more data is going downstream, then call EndFlush on
	// downstream pins.

	// now re-enable Receives
	m_bFlushing = FALSE;

	// No more errors
	m_bRunTimeError = FALSE;

	return S_OK;
}


STDMETHODIMP
CBaseInputPin::Notify(IBaseFilter * pSender, Quality q)
{
	UNREFERENCED_PARAMETER(q);
	CheckPointer(pSender,E_POINTER);
	ValidateReadPtr(pSender,sizeof(IBaseFilter));
	DbgBreak("IQuality::Notify called on an input pin");
	return NOERROR;
} // Notify

/* Free up or unprepare allocator's memory, this is called through
IMediaFilter which is responsible for locking the object first */

HRESULT
CBaseInputPin::Inactive(void)
{
	m_bRunTimeError = FALSE;
	if (m_pAllocator == NULL) {
		return VFW_E_NO_ALLOCATOR;
	}

	m_bFlushing = FALSE;

	return m_pAllocator->Decommit();
}

// what requirements do we have of the allocator - override if you want
// to support other people's allocators but need a specific alignment
// or prefix.
STDMETHODIMP
CBaseInputPin::GetAllocatorRequirements(ALLOCATOR_PROPERTIES*pProps)
{
	UNREFERENCED_PARAMETER(pProps);
	return E_NOTIMPL;
}

//  Check if it's OK to process data
//
HRESULT
CBaseInputPin::CheckStreaming()
{
	//  Shouldn't be able to get any data if we're not connected!
	ASSERT(IsConnected());

	//  Don't process stuff in Stopped state
	if (IsStopped()) {
		return VFW_E_WRONG_STATE;
	}
	if (m_bFlushing) {
		return S_FALSE;
	}
	if (m_bRunTimeError) {
		return VFW_E_RUNTIME_ERROR;
	}
	return S_OK;
}

// Pass on the Quality notification q to
// a. Our QualityControl sink (if we have one) or else
// b. to our upstream filter
// and if that doesn't work, throw it away with a bad return code
HRESULT
CBaseInputPin::PassNotify(Quality& q)
{
	// We pass the message on, which means that we find the quality sink
	// for our input pin and send it there

	DbgLog((LOG_TRACE,3,TEXT("Passing Quality notification through transform")));
	if (m_pQSink!=NULL) {
		return m_pQSink->Notify(m_pFilter, q);
	} else {
		// no sink set, so pass it upstream
		HRESULT hr;
		IQualityControl * pIQC;

		hr = VFW_E_NOT_FOUND;                   // default
		if (m_Connected) {
			m_Connected->QueryInterface(IID_IQualityControl, (void**)&pIQC);

			if (pIQC!=NULL) {
				hr = pIQC->Notify(m_pFilter, q);
				pIQC->Release();
			}
		}
		return hr;
	}

} // PassNotify
