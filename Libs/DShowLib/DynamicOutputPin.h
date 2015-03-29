#pragma once

///////////////////////////////////////////////////////////////////////////
// CDynamicOutputPin
//

class CDynamicOutputPin : public CBaseOutputPin,
	public IPinFlowControl
{
public:
#ifdef UNICODE
	CDynamicOutputPin(
		CHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);
#endif

	CDynamicOutputPin(
		TCHAR *pObjectName,
		CBaseFilter *pFilter,
		CCritSec *pLock,
		HRESULT *phr,
		LPCWSTR pName);

	~CDynamicOutputPin();

	// IUnknown Methods
	DECLARE_IUNKNOWN
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	// IPin Methods
	STDMETHODIMP Disconnect(void);

	// IPinFlowControl Methods
	STDMETHODIMP Block(DWORD dwBlockFlags, HANDLE hEvent);

	//  Set graph config info
	void SetConfigInfo(IGraphConfig *pGraphConfig, HANDLE hStopEvent);

#ifdef DEBUG
	virtual HRESULT Deliver(IMediaSample *pSample);
	virtual HRESULT DeliverEndOfStream(void);
	virtual HRESULT DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate);
#endif // DEBUG

	HRESULT DeliverBeginFlush(void);
	HRESULT DeliverEndFlush(void);

	HRESULT Inactive(void);
	HRESULT Active(void);
	virtual HRESULT CompleteConnect(IPin *pReceivePin);

	virtual HRESULT StartUsingOutputPin(void);
	virtual void StopUsingOutputPin(void);
	virtual bool StreamingThreadUsingOutputPin(void);

	HRESULT ChangeOutputFormat
		(
		const AM_MEDIA_TYPE *pmt,
		REFERENCE_TIME tSegmentStart,
		REFERENCE_TIME tSegmentStop,
		double dSegmentRate
		);
	HRESULT ChangeMediaType(const CMediaType *pmt);
	HRESULT DynamicReconnect(const CMediaType *pmt);

protected:
	HRESULT SynchronousBlockOutputPin(void);
	HRESULT AsynchronousBlockOutputPin(HANDLE hNotifyCallerPinBlockedEvent);
	HRESULT UnblockOutputPin(void);

	void BlockOutputPin(void);
	void ResetBlockState(void);

	static HRESULT WaitEvent(HANDLE hEvent);

	enum BLOCK_STATE
	{
		NOT_BLOCKED,
		PENDING,
		BLOCKED
	};

	// This lock should be held when the following class members are
	// being used: m_hNotifyCallerPinBlockedEvent, m_BlockState,
	// m_dwBlockCallerThreadID and m_dwNumOutstandingOutputPinUsers.
	CCritSec m_BlockStateLock;

	// This event should be signaled when the output pin is
	// not blocked.  This is a manual reset event.  For more
	// information on events, see the documentation for
	// CreateEvent() in the Windows SDK.
	HANDLE m_hUnblockOutputPinEvent;

	// This event will be signaled when block operation succeedes or
	// when the user cancels the block operation.  The block operation
	// can be canceled by calling IPinFlowControl2::Block( 0, NULL )
	// while the block operation is pending.
	HANDLE m_hNotifyCallerPinBlockedEvent;

	// The state of the current block operation.
	BLOCK_STATE m_BlockState;

	// The ID of the thread which last called IPinFlowControl::Block().
	// For more information on thread IDs, see the documentation for
	// GetCurrentThreadID() in the Windows SDK.
	DWORD m_dwBlockCallerThreadID;

	// The number of times StartUsingOutputPin() has been sucessfully
	// called and a corresponding call to StopUsingOutputPin() has not
	// been made.  When this variable is greater than 0, the streaming
	// thread is calling IPin::NewSegment(), IPin::EndOfStream(),
	// IMemInputPin::Receive() or IMemInputPin::ReceiveMultiple().  The
	// streaming thread could also be calling: DynamicReconnect(),
	// ChangeMediaType() or ChangeOutputFormat().  The output pin cannot
	// be blocked while the output pin is being used.
	DWORD m_dwNumOutstandingOutputPinUsers;

	// This event should be set when the IMediaFilter::Stop() is called.
	// This is a manual reset event.  It is also set when the output pin
	// delivers a flush to the connected input pin.
	HANDLE m_hStopEvent;
	IGraphConfig* m_pGraphConfig;

	// TRUE if the output pin's allocator's samples are read only.
	// Otherwise FALSE.  For more information, see the documentation
	// for IMemInputPin::NotifyAllocator().
	BOOL m_bPinUsesReadOnlyAllocator;

private:
	HRESULT Initialize(void);
	HRESULT ChangeMediaTypeHelper(const CMediaType *pmt);

#ifdef DEBUG
	void AssertValid(void);
#endif // DEBUG
};


#ifdef DEBUG

inline HRESULT CDynamicOutputPin::Deliver(IMediaSample *pSample)
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	return CBaseOutputPin::Deliver(pSample);
}

inline HRESULT CDynamicOutputPin::DeliverEndOfStream(void)
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT( StreamingThreadUsingOutputPin() );

	return CBaseOutputPin::DeliverEndOfStream();
}

inline HRESULT CDynamicOutputPin::DeliverNewSegment(REFERENCE_TIME tStart, REFERENCE_TIME tStop, double dRate)
{
	// The caller should call StartUsingOutputPin() before calling this
	// method.
	ASSERT(StreamingThreadUsingOutputPin());

	return CBaseOutputPin::DeliverNewSegment(tStart, tStop, dRate);
}

#endif // DEBUG