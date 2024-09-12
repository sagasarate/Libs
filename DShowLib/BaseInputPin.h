#pragma once

//=====================================================================
//=====================================================================
// Defines CBaseInputPin
//
// derive your standard input pin from this.
// you need to supply GetMediaType and CheckConnect etc (see CBasePin),
// and you need to supply Receive to do something more useful.
//
//=====================================================================
//=====================================================================

class AM_NOVTABLE CBaseInputPin : public CBasePin,
	public IMemInputPin
{

protected:

	IMemAllocator *m_pAllocator;    // Default memory allocator

	// allocator is read-only, so received samples
	// cannot be modified (probably only relevant to in-place
	// transforms
	BYTE m_bReadOnly;

	// in flushing state (between BeginFlush and EndFlush)
	// if TRUE, all Receives are returned with S_FALSE
	BYTE m_bFlushing;

	// Sample properties - initalized in Receive
	AM_SAMPLE2_PROPERTIES m_SampleProps;

public:

	CBaseInputPin(
		TCHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);
#ifdef UNICODE
	CBaseInputPin(
		CHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);
#endif
	virtual ~CBaseInputPin();

	DECLARE_IUNKNOWN

	// override this to publicise our interfaces
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	// return the allocator interface that this input pin
	// would like the output pin to use
	STDMETHODIMP GetAllocator(IMemAllocator ** ppAllocator);

	// tell the input pin which allocator the output pin is actually
	// going to use.
	STDMETHODIMP NotifyAllocator(
		IMemAllocator * pAllocator,
		BOOL bReadOnly);

	// do something with this media sample
	STDMETHODIMP Receive(IMediaSample *pSample);

	// do something with these media samples
	STDMETHODIMP ReceiveMultiple (
		IMediaSample **pSamples,
		long nSamples,
		long *nSamplesProcessed);

	// See if Receive() blocks
	STDMETHODIMP ReceiveCanBlock();

	// Default handling for BeginFlush - call at the beginning
	// of your implementation (makes sure that all Receive calls
	// fail). After calling this, you need to free any queued data
	// and then call downstream.
	STDMETHODIMP BeginFlush(void);

	// default handling for EndFlush - call at end of your implementation
	// - before calling this, ensure that there is no queued data and no thread
	// pushing any more without a further receive, then call downstream,
	// then call this method to clear the m_bFlushing flag and re-enable
	// receives
	STDMETHODIMP EndFlush(void);

	// this method is optional (can return E_NOTIMPL).
	// default implementation returns E_NOTIMPL. Override if you have
	// specific alignment or prefix needs, but could use an upstream
	// allocator
	STDMETHODIMP GetAllocatorRequirements(ALLOCATOR_PROPERTIES*pProps);

	// Release the pin's allocator.
	HRESULT BreakConnect();

	// helper method to check the read-only flag
	BOOL IsReadOnly() {
		return m_bReadOnly;
	};

	// helper method to see if we are flushing
	BOOL IsFlushing() {
		return m_bFlushing;
	};

	//  Override this for checking whether it's OK to process samples
	//  Also call this from EndOfStream.
	virtual HRESULT CheckStreaming();

	// Pass a Quality notification on to the appropriate sink
	HRESULT PassNotify(Quality& q);


	//================================================================================
	// IQualityControl methods (from CBasePin)
	//================================================================================

	STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);

	// no need to override:
	// STDMETHODIMP SetSink(IQualityControl * piqc);


	// switch the pin to inactive state - may already be inactive
	virtual HRESULT Inactive(void);

	// Return sample properties pointer
	AM_SAMPLE2_PROPERTIES * SampleProps() {
		ASSERT(m_SampleProps.cbData != 0);
		return &m_SampleProps;
	}

};