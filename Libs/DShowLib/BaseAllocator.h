#pragma once

//=====================================================================
//=====================================================================
// Defines CBaseAllocator
//
// Abstract base class that manages a list of media samples
//
// This class provides support for getting buffers from the free list,
// including handling of commit and (asynchronous) decommit.
//
// Derive from this class and override the Alloc and Free functions to
// allocate your CMediaSample (or derived) objects and add them to the
// free list, preparing them as necessary.
//=====================================================================
//=====================================================================



class AM_NOVTABLE CBaseAllocator : public CUnknown,// A non delegating IUnknown
	public IMemAllocatorCallbackTemp, // The interface we support
	public CCritSec             // Provides object locking
{
	class CSampleList;
	friend class CSampleList;

	/*  Trick to get at protected member in CMediaSample */
	static CMediaSample * &NextSample(CMediaSample *pSample)
	{
		return pSample->m_pNext;
	};

	/*  Mini list class for the free list */
	class CSampleList
	{
	public:
		CSampleList() : m_List(NULL), m_nOnList(0) {};
#ifdef DEBUG
		~CSampleList()
		{
			ASSERT(m_nOnList == 0);
		};
#endif
		CMediaSample *Head() const { return m_List; };
		CMediaSample *Next(CMediaSample *pSample) const { return CBaseAllocator::NextSample(pSample); };
		int GetCount() const { return m_nOnList; };
		void Add(CMediaSample *pSample)
		{
			ASSERT(pSample != NULL);
			CBaseAllocator::NextSample(pSample) = m_List;
			m_List = pSample;
			m_nOnList++;
		};
		CMediaSample *RemoveHead()
		{
			CMediaSample *pSample = m_List;
			if (pSample != NULL) {
				m_List = CBaseAllocator::NextSample(m_List);
				m_nOnList--;
			}
			return pSample;
		};
		void Remove(CMediaSample *pSample);

	public:
		CMediaSample *m_List;
		int           m_nOnList;
	};
protected:

	CSampleList m_lFree;        // Free list

	/*  Note to overriders of CBaseAllocator.

	We use a lazy signalling mechanism for waiting for samples.
	This means we don't call the OS if no waits occur.

	In order to implement this:

	1. When a new sample is added to m_lFree call NotifySample() which
	calls ReleaseSemaphore on m_hSem with a count of m_lWaiting and
	sets m_lWaiting to 0.
	This must all be done holding the allocator's critical section.

	2. When waiting for a sample call SetWaiting() which increments
	m_lWaiting BEFORE leaving the allocator's critical section.

	3. Actually wait by calling WaitForSingleObject(m_hSem, INFINITE)
	having left the allocator's critical section.  The effect of
	this is to remove 1 from the semaphore's count.  You MUST call
	this once having incremented m_lWaiting.

	The following are then true when the critical section is not held :
	(let nWaiting = number about to wait or waiting)

	(1) if (m_lFree.GetCount() != 0) then (m_lWaiting == 0)
	(2) m_lWaiting + Semaphore count == nWaiting

	We would deadlock if
	nWaiting != 0 &&
	m_lFree.GetCount() != 0 &&
	Semaphore count == 0

	But from (1) if m_lFree.GetCount() != 0 then m_lWaiting == 0 so
	from (2) Semaphore count == nWaiting (which is non-0) so the
	deadlock can't happen.
	*/

	HANDLE m_hSem;              // For signalling
	long m_lWaiting;            // Waiting for a free element
	long m_lCount;              // how many buffers we have agreed to provide
	long m_lAllocated;          // how many buffers are currently allocated
	long m_lSize;               // agreed size of each buffer
	long m_lAlignment;          // agreed alignment
	long m_lPrefix;             // agreed prefix (preceeds GetPointer() value)
	BOOL m_bChanged;            // Have the buffer requirements changed

	// if true, we are decommitted and can't allocate memory
	BOOL m_bCommitted;
	// if true, the decommit has happened, but we haven't called Free yet
	// as there are still outstanding buffers
	BOOL m_bDecommitInProgress;

	//  Notification interface
	IMemAllocatorNotifyCallbackTemp *m_pNotify;

	BOOL m_fEnableReleaseCallback;

	// called to decommit the memory when the last buffer is freed
	// pure virtual - need to override this
	virtual void Free(void) PURE;

	// override to allocate the memory when commit called
	virtual HRESULT Alloc(void);

public:

	CBaseAllocator(
		TCHAR *, LPUNKNOWN, HRESULT *,
		BOOL bEvent = TRUE, BOOL fEnableReleaseCallback = FALSE);
#ifdef UNICODE
	CBaseAllocator(
		CHAR *, LPUNKNOWN, HRESULT *,
		BOOL bEvent = TRUE, BOOL fEnableReleaseCallback = FALSE);
#endif
	virtual ~CBaseAllocator();

	DECLARE_IUNKNOWN

	// override this to publicise our interfaces
	STDMETHODIMP NonDelegatingQueryInterface(REFIID riid, void **ppv);

	STDMETHODIMP SetProperties(
		ALLOCATOR_PROPERTIES* pRequest,
		ALLOCATOR_PROPERTIES* pActual);

	// return the properties actually being used on this allocator
	STDMETHODIMP GetProperties(
		ALLOCATOR_PROPERTIES* pProps);

	// override Commit to allocate memory. We handle the GetBuffer
	//state changes
	STDMETHODIMP Commit();

	// override this to handle the memory freeing. We handle any outstanding
	// GetBuffer calls
	STDMETHODIMP Decommit();

	// get container for a sample. Blocking, synchronous call to get the
	// next free buffer (as represented by an IMediaSample interface).
	// on return, the time etc properties will be invalid, but the buffer
	// pointer and size will be correct. The two time parameters are
	// optional and either may be NULL, they may alternatively be set to
	// the start and end times the sample will have attached to it
	// bPrevFramesSkipped is not used (used only by the video renderer's
	// allocator where it affects quality management in direct draw).

	STDMETHODIMP GetBuffer(IMediaSample **ppBuffer,
		REFERENCE_TIME * pStartTime,
		REFERENCE_TIME * pEndTime,
		DWORD dwFlags);

	// final release of a CMediaSample will call this
	STDMETHODIMP ReleaseBuffer(IMediaSample *pBuffer);
	// obsolete:: virtual void PutOnFreeList(CMediaSample * pSample);

	STDMETHODIMP SetNotify(IMemAllocatorNotifyCallbackTemp *pNotify);

	STDMETHODIMP GetFreeCount(LONG *plBuffersFree);

	// Notify that a sample is available
	void NotifySample();

	// Notify that we're waiting for a sample
	void SetWaiting() { m_lWaiting++; };
};
