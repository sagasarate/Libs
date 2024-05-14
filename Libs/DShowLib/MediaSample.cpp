#include "StdAfx.h"


//=====================================================================
//=====================================================================
// Memory allocation class, implements CMediaSample
//=====================================================================
//=====================================================================


/* NOTE The implementation of this class calls the CUnknown constructor with
a NULL outer unknown pointer. This has the effect of making us a self
contained class, ie any QueryInterface, AddRef or Release calls will be
routed to the class's NonDelegatingUnknown methods. You will typically
find that the classes that do this then override one or more of these
virtual functions to provide more specialised behaviour. A good example
of this is where a class wants to keep the QueryInterface internal but
still wants it's lifetime controlled by the external object */

/* The last two parameters have default values of NULL and zero */

CMediaSample::CMediaSample(TCHAR *pName,
						   CBaseAllocator *pAllocator,
						   HRESULT *phr,
						   LPBYTE pBuffer,
						   LONG length) :
m_pBuffer(pBuffer),             // Initialise the buffer
m_cbBuffer(length),             // And it's length
m_lActual(length),              // By default, actual = length
m_pMediaType(NULL),             // No media type change
m_dwFlags(0),                   // Nothing set
m_cRef(0),                      // 0 ref count
m_dwTypeSpecificFlags(0),       // Type specific flags
m_dwStreamId(AM_STREAM_MEDIA),  // Stream id
m_pAllocator(pAllocator)        // Allocator
{

	/* We must have an owner and it must also be derived from class
	CBaseAllocator BUT we do not hold a reference count on it */

	ASSERT(pAllocator);
}

#ifdef UNICODE
CMediaSample::CMediaSample(CHAR *pName,
						   CBaseAllocator *pAllocator,
						   HRESULT *phr,
						   LPBYTE pBuffer,
						   LONG length) :
m_pBuffer(pBuffer),             // Initialise the buffer
m_cbBuffer(length),             // And it's length
m_lActual(length),              // By default, actual = length
m_pMediaType(NULL),             // No media type change
m_dwFlags(0),                   // Nothing set
m_cRef(0),                      // 0 ref count
m_dwTypeSpecificFlags(0),       // Type specific flags
m_dwStreamId(AM_STREAM_MEDIA),  // Stream id
m_pAllocator(pAllocator)        // Allocator
{

	/* We must have an owner and it must also be derived from class
	CBaseAllocator BUT we do not hold a reference count on it */

	ASSERT(pAllocator);
}
#endif

/* Destructor deletes the media type memory */

CMediaSample::~CMediaSample()
{

	if (m_pMediaType) {
		DeleteMediaType(m_pMediaType);
	}
}

/* Override this to publicise our interfaces */

STDMETHODIMP
CMediaSample::QueryInterface(REFIID riid, void **ppv)
{
	if (riid == IID_IMediaSample ||
		riid == IID_IMediaSample2 ||
		riid == IID_IUnknown) {
			return GetInterface((IMediaSample *) this, ppv);
	} else {
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG)
CMediaSample::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}


// --  CMediaSample lifetimes --
//
// On final release of this sample buffer it is not deleted but
// returned to the freelist of the owning memory allocator
//
// The allocator may be waiting for the last buffer to be placed on the free
// list in order to decommit all the memory, so the ReleaseBuffer() call may
// result in this sample being deleted. We also need to hold a refcount on
// the allocator to stop that going away until we have finished with this.
// However, we cannot release the allocator before the ReleaseBuffer, as the
// release may cause us to be deleted. Similarly we can't do it afterwards.
//
// Thus we must leave it to the allocator to hold an addref on our behalf.
// When he issues us in GetBuffer, he addref's himself. When ReleaseBuffer
// is called, he releases himself, possibly causing us and him to be deleted.


STDMETHODIMP_(ULONG)
CMediaSample::Release()
{
	/* Decrement our own private reference count */
	LONG lRef;
	if (m_cRef == 1) {
		lRef = 0;
		m_cRef = 0;
	} else {
		lRef = InterlockedDecrement(&m_cRef);
	}
	ASSERT(lRef >= 0);

	DbgLog((LOG_MEMORY,3,TEXT("    Unknown %X ref-- = %d"),
		this, m_cRef));

	/* Did we release our final reference count */
	if (lRef == 0) {
		/* Free all resources */
		if (m_dwFlags & Sample_TypeChanged) {
			SetMediaType(NULL);
		}
		ASSERT(m_pMediaType == NULL);
		m_dwFlags = 0;
		m_dwTypeSpecificFlags = 0;
		m_dwStreamId = AM_STREAM_MEDIA;

		/* This may cause us to be deleted */
		// Our refcount is reliably 0 thus no-one will mess with us
		m_pAllocator->ReleaseBuffer(this);
	}
	return (ULONG)lRef;
}


// set the buffer pointer and length. Used by allocators that
// want variable sized pointers or pointers into already-read data.
// This is only available through a CMediaSample* not an IMediaSample*
// and so cannot be changed by clients.
HRESULT
CMediaSample::SetPointer(BYTE * ptr, LONG cBytes)
{
	m_pBuffer = ptr;            // new buffer area (could be null)
	m_cbBuffer = cBytes;        // length of buffer
	m_lActual = cBytes;         // length of data in buffer (assume full)

	return S_OK;
}


// get me a read/write pointer to this buffer's memory. I will actually
// want to use sizeUsed bytes.
STDMETHODIMP
CMediaSample::GetPointer(BYTE ** ppBuffer)
{
	ValidateReadWritePtr(ppBuffer,sizeof(BYTE *));

	// creator must have set pointer either during
	// constructor or by SetPointer
	ASSERT(m_pBuffer);

	*ppBuffer = m_pBuffer;
	return NOERROR;
}


// return the size in bytes of this buffer
STDMETHODIMP_(LONG)
CMediaSample::GetSize(void)
{
	return m_cbBuffer;
}


// get the stream time at which this sample should start and finish.
STDMETHODIMP
CMediaSample::GetTime(
					  REFERENCE_TIME * pTimeStart,     // put time here
					  REFERENCE_TIME * pTimeEnd
					  )
{
	ValidateReadWritePtr(pTimeStart,sizeof(REFERENCE_TIME));
	ValidateReadWritePtr(pTimeEnd,sizeof(REFERENCE_TIME));

	if (!(m_dwFlags & Sample_StopValid)) {
		if (!(m_dwFlags & Sample_TimeValid)) {
			return VFW_E_SAMPLE_TIME_NOT_SET;
		} else {
			*pTimeStart = m_Start;

			//  Make sure old stuff works
			*pTimeEnd = m_Start + 1;
			return VFW_S_NO_STOP_TIME;
		}
	}

	*pTimeStart = m_Start;
	*pTimeEnd = m_End;
	return NOERROR;
}


// Set the stream time at which this sample should start and finish.
// NULL pointers means the time is reset
STDMETHODIMP
CMediaSample::SetTime(
					  REFERENCE_TIME * pTimeStart,
					  REFERENCE_TIME * pTimeEnd
					  )
{
	if (pTimeStart == NULL) {
		ASSERT(pTimeEnd == NULL);
		m_dwFlags &= ~(Sample_TimeValid | Sample_StopValid);
	} else {
		if (pTimeEnd == NULL) {
			m_Start = *pTimeStart;
			m_dwFlags |= Sample_TimeValid;
			m_dwFlags &= ~Sample_StopValid;
		} else {
			ValidateReadPtr(pTimeStart,sizeof(REFERENCE_TIME));
			ValidateReadPtr(pTimeEnd,sizeof(REFERENCE_TIME));
			ASSERT(*pTimeEnd >= *pTimeStart);

			m_Start = *pTimeStart;
			m_End = *pTimeEnd;
			m_dwFlags |= Sample_TimeValid | Sample_StopValid;
		}
	}
	return NOERROR;
}


// get the media times (eg bytes) for this sample
STDMETHODIMP
CMediaSample::GetMediaTime(
						   LONGLONG * pTimeStart,
						   LONGLONG * pTimeEnd
						   )
{
	ValidateReadWritePtr(pTimeStart,sizeof(LONGLONG));
	ValidateReadWritePtr(pTimeEnd,sizeof(LONGLONG));

	if (!(m_dwFlags & Sample_MediaTimeValid)) {
		return VFW_E_MEDIA_TIME_NOT_SET;
	}

	*pTimeStart = m_MediaStart;
	*pTimeEnd = (m_MediaStart + m_MediaEnd);
	return NOERROR;
}


// Set the media times for this sample
STDMETHODIMP
CMediaSample::SetMediaTime(
						   LONGLONG * pTimeStart,
						   LONGLONG * pTimeEnd
						   )
{
	if (pTimeStart == NULL) {
		ASSERT(pTimeEnd == NULL);
		m_dwFlags &= ~Sample_MediaTimeValid;
	} else {
		ValidateReadPtr(pTimeStart,sizeof(LONGLONG));
		ValidateReadPtr(pTimeEnd,sizeof(LONGLONG));
		ASSERT(*pTimeEnd >= *pTimeStart);

		m_MediaStart = *pTimeStart;
		m_MediaEnd = (LONG)(*pTimeEnd - *pTimeStart);
		m_dwFlags |= Sample_MediaTimeValid;
	}
	return NOERROR;
}


STDMETHODIMP
CMediaSample::IsSyncPoint(void)
{
	if (m_dwFlags & Sample_SyncPoint) {
		return S_OK;
	} else {
		return S_FALSE;
	}
}


STDMETHODIMP
CMediaSample::SetSyncPoint(BOOL bIsSyncPoint)
{
	if (bIsSyncPoint) {
		m_dwFlags |= Sample_SyncPoint;
	} else {
		m_dwFlags &= ~Sample_SyncPoint;
	}
	return NOERROR;
}

// returns S_OK if there is a discontinuity in the data (this same is
// not a continuation of the previous stream of data
// - there has been a seek).
STDMETHODIMP
CMediaSample::IsDiscontinuity(void)
{
	if (m_dwFlags & Sample_Discontinuity) {
		return S_OK;
	} else {
		return S_FALSE;
	}
}

// set the discontinuity property - TRUE if this sample is not a
// continuation, but a new sample after a seek.
STDMETHODIMP
CMediaSample::SetDiscontinuity(BOOL bDiscont)
{
	// should be TRUE or FALSE
	if (bDiscont) {
		m_dwFlags |= Sample_Discontinuity;
	} else {
		m_dwFlags &= ~Sample_Discontinuity;
	}
	return S_OK;
}

STDMETHODIMP
CMediaSample::IsPreroll(void)
{
	if (m_dwFlags & Sample_Preroll) {
		return S_OK;
	} else {
		return S_FALSE;
	}
}


STDMETHODIMP
CMediaSample::SetPreroll(BOOL bIsPreroll)
{
	if (bIsPreroll) {
		m_dwFlags |= Sample_Preroll;
	} else {
		m_dwFlags &= ~Sample_Preroll;
	}
	return NOERROR;
}

STDMETHODIMP_(LONG)
CMediaSample::GetActualDataLength(void)
{
	return m_lActual;
}


STDMETHODIMP
CMediaSample::SetActualDataLength(LONG lActual)
{
	if (lActual > m_cbBuffer) {
		ASSERT(lActual <= GetSize());
		return VFW_E_BUFFER_OVERFLOW;
	}
	m_lActual = lActual;
	return NOERROR;
}


/* These allow for limited format changes in band */

STDMETHODIMP
CMediaSample::GetMediaType(AM_MEDIA_TYPE **ppMediaType)
{
	ValidateReadWritePtr(ppMediaType,sizeof(AM_MEDIA_TYPE *));
	ASSERT(ppMediaType);

	/* Do we have a new media type for them */

	if (!(m_dwFlags & Sample_TypeChanged)) {
		ASSERT(m_pMediaType == NULL);
		*ppMediaType = NULL;
		return S_FALSE;
	}

	ASSERT(m_pMediaType);

	/* Create a copy of our media type */

	*ppMediaType = CreateMediaType(m_pMediaType);
	if (*ppMediaType == NULL) {
		return E_OUTOFMEMORY;
	}
	return NOERROR;
}


/* Mark this sample as having a different format type */

STDMETHODIMP
CMediaSample::SetMediaType(AM_MEDIA_TYPE *pMediaType)
{
	/* Delete the current media type */

	if (m_pMediaType) {
		DeleteMediaType(m_pMediaType);
		m_pMediaType = NULL;
	}

	/* Mechanism for resetting the format type */

	if (pMediaType == NULL) {
		m_dwFlags &= ~Sample_TypeChanged;
		return NOERROR;
	}

	ASSERT(pMediaType);
	ValidateReadPtr(pMediaType,sizeof(AM_MEDIA_TYPE));

	/* Take a copy of the media type */

	m_pMediaType = CreateMediaType(pMediaType);
	if (m_pMediaType == NULL) {
		m_dwFlags &= ~Sample_TypeChanged;
		return E_OUTOFMEMORY;
	}

	m_dwFlags |= Sample_TypeChanged;
	return NOERROR;
}

// Set and get properties (IMediaSample2)
STDMETHODIMP CMediaSample::GetProperties(
	DWORD cbProperties,
	BYTE * pbProperties
	)
{
	if (0 != cbProperties) {
		CheckPointer(pbProperties, E_POINTER);
		//  Return generic stuff up to the length
		AM_SAMPLE2_PROPERTIES Props;
		Props.cbData     = (DWORD) (min(cbProperties, sizeof(Props)));
		Props.dwSampleFlags = m_dwFlags & ~Sample_MediaTimeValid;
		Props.dwTypeSpecificFlags = m_dwTypeSpecificFlags;
		Props.pbBuffer   = m_pBuffer;
		Props.cbBuffer   = m_cbBuffer;
		Props.lActual    = m_lActual;
		Props.tStart     = m_Start;
		Props.tStop      = m_End;
		Props.dwStreamId = m_dwStreamId;
		if (m_dwFlags & AM_SAMPLE_TYPECHANGED) {
			Props.pMediaType = m_pMediaType;
		} else {
			Props.pMediaType = NULL;
		}
		CopyMemory(pbProperties, &Props, Props.cbData);
	}
	return S_OK;
}

#define CONTAINS_FIELD(type, field, offset) \
	((FIELD_OFFSET(type, field) + sizeof(((type *)0)->field)) <= offset)

HRESULT CMediaSample::SetProperties(
									DWORD cbProperties,
									const BYTE * pbProperties
									)
{

	/*  Generic properties */
	AM_MEDIA_TYPE *pMediaType = NULL;

	if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbData, cbProperties)) {
		CheckPointer(pbProperties, E_POINTER);
		AM_SAMPLE2_PROPERTIES *pProps =
			(AM_SAMPLE2_PROPERTIES *)pbProperties;

		/*  Don't use more data than is actually there */
		if (pProps->cbData < cbProperties) {
			cbProperties = pProps->cbData;
		}
		/*  We only handle IMediaSample2 */
		if (cbProperties > sizeof(*pProps) ||
			pProps->cbData > sizeof(*pProps)) {
				return E_INVALIDARG;
		}
		/*  Do checks first, the assignments (for backout) */
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwSampleFlags, cbProperties)) {
			/*  Check the flags */
			if (pProps->dwSampleFlags &
				(~Sample_ValidFlags | Sample_MediaTimeValid)) {
					return E_INVALIDARG;
			}
			/*  Check a flag isn't being set for a property
			not being provided
			*/
			if ((pProps->dwSampleFlags & AM_SAMPLE_TIMEVALID) &&
				!(m_dwFlags & AM_SAMPLE_TIMEVALID) &&
				!CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStop, cbProperties)) {
					return E_INVALIDARG;
			}
		}
		/*  NB - can't SET the pointer or size */
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pbBuffer, cbProperties)) {

			/*  Check pbBuffer */
			if (pProps->pbBuffer != 0 && pProps->pbBuffer != m_pBuffer) {
				return E_INVALIDARG;
			}
		}
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbBuffer, cbProperties)) {

			/*  Check cbBuffer */
			if (pProps->cbBuffer != 0 && pProps->cbBuffer != m_cbBuffer) {
				return E_INVALIDARG;
			}
		}
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, cbBuffer, cbProperties) &&
			CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, lActual, cbProperties)) {

				/*  Check lActual */
				if (pProps->cbBuffer < pProps->lActual) {
					return E_INVALIDARG;
				}
		}

		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pMediaType, cbProperties)) {

			/*  Check pMediaType */
			if (pProps->dwSampleFlags & AM_SAMPLE_TYPECHANGED) {
				CheckPointer(pProps->pMediaType, E_POINTER);
				pMediaType = CreateMediaType(pProps->pMediaType);
				if (pMediaType == NULL) {
					return E_OUTOFMEMORY;
				}
			}
		}

		/*  Now do the assignments */
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwStreamId, cbProperties)) {
			m_dwStreamId = pProps->dwStreamId;
		}
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwSampleFlags, cbProperties)) {
			/*  Set the flags */
			m_dwFlags = pProps->dwSampleFlags |
				(m_dwFlags & Sample_MediaTimeValid);
			m_dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
		} else {
			if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, dwTypeSpecificFlags, cbProperties)) {
				m_dwTypeSpecificFlags = pProps->dwTypeSpecificFlags;
			}
		}

		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, lActual, cbProperties)) {
			/*  Set lActual */
			m_lActual = pProps->lActual;
		}

		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStop, cbProperties)) {

			/*  Set the times */
			m_End   = pProps->tStop;
		}
		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, tStart, cbProperties)) {

			/*  Set the times */
			m_Start = pProps->tStart;
		}

		if (CONTAINS_FIELD(AM_SAMPLE2_PROPERTIES, pMediaType, cbProperties)) {
			/*  Set pMediaType */
			if (pProps->dwSampleFlags & AM_SAMPLE_TYPECHANGED) {
				if (m_pMediaType != NULL) {
					DeleteMediaType(m_pMediaType);
				}
				m_pMediaType = pMediaType;
			}
		}

		/*  Fix up the type changed flag to correctly reflect the current state
		If, for instance the input contained no type change but the
		output does then if we don't do this we'd lose the
		output media type.
		*/
		if (m_pMediaType) {
			m_dwFlags |= Sample_TypeChanged;
		} else {
			m_dwFlags &= ~Sample_TypeChanged;
		}
	}

	return S_OK;
}
