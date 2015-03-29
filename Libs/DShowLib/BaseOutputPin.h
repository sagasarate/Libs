#pragma once


//=====================================================================
//=====================================================================
// Defines CBaseOutputPin
//
// class derived from CBasePin that can pass buffers to a connected pin
// that supports IMemInputPin. Supports IPin.
//
// Derive your output pin from this.
//
//=====================================================================
//=====================================================================

class  AM_NOVTABLE CBaseOutputPin : public CBasePin
{

protected:

	IMemAllocator *m_pAllocator;
	IMemInputPin *m_pInputPin;        // interface on the downstreaminput pin
	// set up in CheckConnect when we connect.

public:

	CBaseOutputPin(
		TCHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);
#ifdef UNICODE
	CBaseOutputPin(
		CHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);
#endif
	// override CompleteConnect() so we can negotiate an allocator
	virtual HRESULT CompleteConnect(IPin *pReceivePin);

	// negotiate the allocator and its buffer size/count and other properties
	// Calls DecideBufferSize to set properties
	virtual HRESULT DecideAllocator(IMemInputPin * pPin, IMemAllocator ** pAlloc);

	// override this to set the buffer size and count. Return an error
	// if the size/count is not to your liking.
	// The allocator properties passed in are those requested by the
	// input pin - use eg the alignment and prefix members if you have
	// no preference on these.
	virtual HRESULT DecideBufferSize(
		IMemAllocator * pAlloc,
		ALLOCATOR_PROPERTIES * ppropInputRequest
		) PURE;

	// returns an empty sample buffer from the allocator
	virtual HRESULT GetDeliveryBuffer(IMediaSample ** ppSample,
		REFERENCE_TIME * pStartTime,
		REFERENCE_TIME * pEndTime,
		DWORD dwFlags);

	// deliver a filled-in sample to the connected input pin
	// note - you need to release it after calling this. The receiving
	// pin will addref the sample if it needs to hold it beyond the
	// call.
	virtual HRESULT Deliver(IMediaSample *);

	// override this to control the connection
	virtual HRESULT InitAllocator(IMemAllocator **ppAlloc);
	HRESULT CheckConnect(IPin *pPin);
	HRESULT BreakConnect();

	// override to call Commit and Decommit
	HRESULT Active(void);
	HRESULT Inactive(void);

	// we have a default handling of EndOfStream which is to return
	// an error, since this should be called on input pins only
	STDMETHODIMP EndOfStream(void);

	// called from elsewhere in our filter to pass EOS downstream to
	// our connected input pin
	virtual HRESULT DeliverEndOfStream(void);

	// same for Begin/EndFlush - we handle Begin/EndFlush since it
	// is an error on an output pin, and we have Deliver methods to
	// call the methods on the connected pin
	STDMETHODIMP BeginFlush(void);
	STDMETHODIMP EndFlush(void);
	virtual HRESULT DeliverBeginFlush(void);
	virtual HRESULT DeliverEndFlush(void);

	// deliver NewSegment to connected pin - you will need to
	// override this if you queue any data in your output pin.
	virtual HRESULT DeliverNewSegment(
		REFERENCE_TIME tStart,
		REFERENCE_TIME tStop,
		double dRate);

	//================================================================================
	// IQualityControl methods
	//================================================================================

	// All inherited from CBasePin and not overridden here.
	// STDMETHODIMP Notify(IBaseFilter * pSender, Quality q);
	// STDMETHODIMP SetSink(IQualityControl * piqc);
};
