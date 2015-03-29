#include "StdAfx.h"


//=====================================================================
//=====================================================================
// Implements CBaseAllocator
//=====================================================================
//=====================================================================


/* Constructor overrides the default settings for the free list to request
that it be alertable (ie the list can be cast to a handle which can be
passed to WaitForSingleObject). Both of the allocator lists also ask for
object locking, the all list matches the object default settings but I
have included them here just so it is obvious what kind of list it is */

CBaseAllocator::CBaseAllocator(TCHAR *pName,
							   LPUNKNOWN pUnk,
							   HRESULT *phr,
							   BOOL bEvent,
							   BOOL fEnableReleaseCallback
							   ) :
CUnknown(pName, pUnk),
m_lAllocated(0),
m_bChanged(FALSE),
m_bCommitted(FALSE),
m_bDecommitInProgress(FALSE),
m_lSize(0),
m_lCount(0),
m_lAlignment(0),
m_lPrefix(0),
m_hSem(NULL),
m_lWaiting(0),
m_fEnableReleaseCallback(fEnableReleaseCallback),
m_pNotify(NULL)
{

	if (bEvent) {
		m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
		if (m_hSem == NULL) {
			*phr = E_OUTOFMEMORY;
			return;
		}
	}
}

#ifdef UNICODE
CBaseAllocator::CBaseAllocator(CHAR *pName,
							   LPUNKNOWN pUnk,
							   HRESULT *phr,
							   BOOL bEvent,
							   BOOL fEnableReleaseCallback) :
CUnknown(pName, pUnk),
m_lAllocated(0),
m_bChanged(FALSE),
m_bCommitted(FALSE),
m_bDecommitInProgress(FALSE),
m_lSize(0),
m_lCount(0),
m_lAlignment(0),
m_lPrefix(0),
m_hSem(NULL),
m_lWaiting(0),
m_fEnableReleaseCallback(fEnableReleaseCallback),
m_pNotify(NULL)
{

	if (bEvent) {
		m_hSem = CreateSemaphore(NULL, 0, 0x7FFFFFFF, NULL);
		if (m_hSem == NULL) {
			*phr = E_OUTOFMEMORY;
			return;
		}
	}
}
#endif

/* Destructor */

CBaseAllocator::~CBaseAllocator()
{
	// we can't call Decommit here since that would mean a call to a
	// pure virtual in destructor.
	// We must assume that the derived class has gone into decommit state in
	// its destructor.

	ASSERT(!m_bCommitted);
	if (m_hSem != NULL) {
		EXECUTE_ASSERT(CloseHandle(m_hSem));
	}
	if (m_pNotify) {
		m_pNotify->Release();
	}
}


/* Override this to publicise our interfaces */

STDMETHODIMP
CBaseAllocator::NonDelegatingQueryInterface(REFIID riid, void **ppv)
{
	/* Do we know about this interface */

	if (riid == IID_IMemAllocator ||
		riid == IID_IMemAllocatorCallbackTemp && m_fEnableReleaseCallback) {
			return GetInterface((IMemAllocatorCallbackTemp *) this, ppv);
	} else {
		return CUnknown::NonDelegatingQueryInterface(riid, ppv);
	}
}


/* This sets the size and count of the required samples. The memory isn't
actually allocated until Commit() is called, if memory has already been
allocated then assuming no samples are outstanding the user may call us
to change the buffering, the memory will be released in Commit() */

STDMETHODIMP
CBaseAllocator::SetProperties(
							  ALLOCATOR_PROPERTIES* pRequest,
							  ALLOCATOR_PROPERTIES* pActual)
{
	CheckPointer(pRequest, E_POINTER);
	CheckPointer(pActual, E_POINTER);
	ValidateReadWritePtr(pActual, sizeof(ALLOCATOR_PROPERTIES));
	CComAutoLock cObjectLock(this);

	ZeroMemory(pActual, sizeof(ALLOCATOR_PROPERTIES));

	ASSERT(pRequest->cbBuffer > 0);

	/*  Check the alignment requested */
	if (pRequest->cbAlign != 1) {
		DbgLog((LOG_ERROR, 2, TEXT("Alignment requested was 0x%x, not 1"),
			pRequest->cbAlign));
		return VFW_E_BADALIGN;
	}

	/* Can't do this if already committed, there is an argument that says we
	should not reject the SetProperties call if there are buffers still
	active. However this is called by the source filter, which is the same
	person who is holding the samples. Therefore it is not unreasonable
	for them to free all their samples before changing the requirements */

	if (m_bCommitted) {
		return VFW_E_ALREADY_COMMITTED;
	}

	/* Must be no outstanding buffers */

	if (m_lAllocated != m_lFree.GetCount()) {
		return VFW_E_BUFFERS_OUTSTANDING;
	}

	/* There isn't any real need to check the parameters as they
	will just be rejected when the user finally calls Commit */

	pActual->cbBuffer = m_lSize = pRequest->cbBuffer;
	pActual->cBuffers = m_lCount = pRequest->cBuffers;
	pActual->cbAlign = m_lAlignment = pRequest->cbAlign;
	pActual->cbPrefix = m_lPrefix = pRequest->cbPrefix;

	m_bChanged = TRUE;
	return NOERROR;
}

STDMETHODIMP
CBaseAllocator::GetProperties(
							  ALLOCATOR_PROPERTIES * pActual)
{
	CheckPointer(pActual,E_POINTER);
	ValidateReadWritePtr(pActual,sizeof(ALLOCATOR_PROPERTIES));

	CComAutoLock cObjectLock(this);
	pActual->cbBuffer = m_lSize;
	pActual->cBuffers = m_lCount;
	pActual->cbAlign = m_lAlignment;
	pActual->cbPrefix = m_lPrefix;
	return NOERROR;
}

// get container for a sample. Blocking, synchronous call to get the
// next free buffer (as represented by an IMediaSample interface).
// on return, the time etc properties will be invalid, but the buffer
// pointer and size will be correct.

HRESULT CBaseAllocator::GetBuffer(IMediaSample **ppBuffer,
								  REFERENCE_TIME *pStartTime,
								  REFERENCE_TIME *pEndTime,
								  DWORD dwFlags
								  )
{
	UNREFERENCED_PARAMETER(pStartTime);
	UNREFERENCED_PARAMETER(pEndTime);
	UNREFERENCED_PARAMETER(dwFlags);
	CMediaSample *pSample;

	*ppBuffer = NULL;
	for (;;)
	{
		{  // scope for lock
			CComAutoLock cObjectLock(this);

			/* Check we are committed */
			if (!m_bCommitted) {
				return VFW_E_NOT_COMMITTED;
			}
			pSample = (CMediaSample *) m_lFree.RemoveHead();
			if (pSample == NULL) {
				SetWaiting();
			}
		}

		/* If we didn't get a sample then wait for the list to signal */

		if (pSample) {
			break;
		}
		if (dwFlags & AM_GBF_NOWAIT) {
			return VFW_E_TIMEOUT;
		}
		ASSERT(m_hSem != NULL);
		WaitForSingleObject(m_hSem, INFINITE);
	}

	/* Addref the buffer up to one. On release
	back to zero instead of being deleted, it will requeue itself by
	calling the ReleaseBuffer member function. NOTE the owner of a
	media sample must always be derived from CBaseAllocator */


	ASSERT(pSample->m_cRef == 0);
	pSample->m_cRef = 1;
	*ppBuffer = pSample;


	return NOERROR;
}


/* Final release of a CMediaSample will call this */

STDMETHODIMP
CBaseAllocator::ReleaseBuffer(IMediaSample * pSample)
{
	CheckPointer(pSample,E_POINTER);
	ValidateReadPtr(pSample,sizeof(IMediaSample));



	BOOL bRelease = FALSE;
	{
		CComAutoLock cal(this);

		/* Put back on the free list */

		m_lFree.Add((CMediaSample *)pSample);
		if (m_lWaiting != 0) {
			NotifySample();
		}

		// if there is a pending Decommit, then we need to complete it by
		// calling Free() when the last buffer is placed on the free list

		LONG l1 = m_lFree.GetCount();
		if (m_bDecommitInProgress && (l1 == m_lAllocated)) {
			Free();
			m_bDecommitInProgress = FALSE;
			bRelease = TRUE;
		}
	}

	if (m_pNotify) {

		ASSERT(m_fEnableReleaseCallback);

		//
		// Note that this is not synchronized with setting up a notification
		// method.
		//
		m_pNotify->NotifyRelease();
	}

	/* For each buffer there is one AddRef, made in GetBuffer and released
	here. This may cause the allocator and all samples to be deleted */

	if (bRelease) {
		Release();
	}
	return NOERROR;
}

STDMETHODIMP
CBaseAllocator::SetNotify(
						  IMemAllocatorNotifyCallbackTemp* pNotify
						  )
{
	ASSERT(m_fEnableReleaseCallback);
	CComAutoLock lck(this);
	if (pNotify) {
		pNotify->AddRef();
	}
	if (m_pNotify) {
		m_pNotify->Release();
	}
	m_pNotify = pNotify;
	return S_OK;
}

STDMETHODIMP
CBaseAllocator::GetFreeCount(
							 LONG* plBuffersFree
							 )
{
	ASSERT(m_fEnableReleaseCallback);
	CComAutoLock cObjectLock(this);
	*plBuffersFree = m_lCount - m_lAllocated + m_lFree.GetCount();
	return NOERROR;
}

void
CBaseAllocator::NotifySample()
{
	if (m_lWaiting != 0) {
		ASSERT(m_hSem != NULL);
		ReleaseSemaphore(m_hSem, m_lWaiting, 0);
		m_lWaiting = 0;
	}
}

STDMETHODIMP
CBaseAllocator::Commit()
{
	/* Check we are not decommitted */
	CComAutoLock cObjectLock(this);

	// cannot need to alloc or re-alloc if we are committed
	if (m_bCommitted) {
		return NOERROR;
	}

	/* Allow GetBuffer calls */

	m_bCommitted = TRUE;

	// is there a pending decommit ? if so, just cancel it
	if (m_bDecommitInProgress) {
		m_bDecommitInProgress = FALSE;

		// don't call Alloc at this point. He cannot allow SetProperties
		// between Decommit and the last free, so the buffer size cannot have
		// changed. And because some of the buffers are not free yet, he
		// cannot re-alloc anyway.
		return NOERROR;
	}

	DbgLog((LOG_MEMORY, 1, TEXT("Allocating: %ldx%ld"), m_lCount, m_lSize));

	// actually need to allocate the samples
	HRESULT hr = Alloc();
	if (FAILED(hr)) {
		m_bCommitted = FALSE;
		return hr;
	}
	AddRef();
	return NOERROR;
}


STDMETHODIMP
CBaseAllocator::Decommit()
{
	BOOL bRelease = FALSE;
	{
		/* Check we are not already decommitted */
		CComAutoLock cObjectLock(this);
		if (m_bCommitted == FALSE) {
			if (m_bDecommitInProgress == FALSE) {
				return NOERROR;
			}
		}

		/* No more GetBuffer calls will succeed */
		m_bCommitted = FALSE;

		// are any buffers outstanding?
		if (m_lFree.GetCount() < m_lAllocated) {
			// please complete the decommit when last buffer is freed
			m_bDecommitInProgress = TRUE;
		} else {
			m_bDecommitInProgress = FALSE;

			// need to complete the decommit here as there are no
			// outstanding buffers

			Free();
			bRelease = TRUE;
		}

		// Tell anyone waiting that they can go now so we can
		// reject their call
		NotifySample();
	}

	if (bRelease) {
		Release();
	}
	return NOERROR;
}


/* Base definition of allocation which checks we are ok to go ahead and do
the full allocation. We return S_FALSE if the requirements are the same */

HRESULT
CBaseAllocator::Alloc(void)
{
	/* Error if he hasn't set the size yet */
	if (m_lCount <= 0 || m_lSize <= 0 || m_lAlignment <= 0) {
		return VFW_E_SIZENOTSET;
	}

	/* should never get here while buffers outstanding */
	ASSERT(m_lFree.GetCount() == m_lAllocated);

	/* If the requirements haven't changed then don't reallocate */
	if (m_bChanged == FALSE) {
		return S_FALSE;
	}

	return NOERROR;
}

/*  Implement CBaseAllocator::CSampleList::Remove(pSample)
Removes pSample from the list
*/
void
CBaseAllocator::CSampleList::Remove(CMediaSample * pSample)
{
	CMediaSample **pSearch;
	for (pSearch = &m_List;
		*pSearch != NULL;
		pSearch = &(CBaseAllocator::NextSample(*pSearch))) {
			if (*pSearch == pSample) {
				*pSearch = CBaseAllocator::NextSample(pSample);
				CBaseAllocator::NextSample(pSample) = NULL;
				m_nOnList--;
				return;
			}
	}
	DbgBreak("Couldn't find sample in list");
}