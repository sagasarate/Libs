#pragma once

//=====================================================================
//=====================================================================
// Defines CMediaSample
//
// an object of this class supports IMediaSample and represents a buffer
// for media data with some associated properties. Releasing it returns
// it to a freelist managed by a CBaseAllocator derived object.
//=====================================================================
//=====================================================================

class CMediaSample : public IMediaSample2    // The interface we support
{

protected:

	friend class CBaseAllocator;

	/*  Values for dwFlags - these are used for backward compatiblity
	only now - use AM_SAMPLE_xxx
	*/
	enum { Sample_SyncPoint       = 0x01,   /* Is this a sync point */
		Sample_Preroll         = 0x02,   /* Is this a preroll sample */
		Sample_Discontinuity   = 0x04,   /* Set if start of new segment */
		Sample_TypeChanged     = 0x08,   /* Has the type changed */
		Sample_TimeValid       = 0x10,   /* Set if time is valid */
		Sample_MediaTimeValid  = 0x20,   /* Is the media time valid */
		Sample_TimeDiscontinuity = 0x40, /* Time discontinuity */
		Sample_StopValid       = 0x100,  /* Stop time valid */
		Sample_ValidFlags      = 0x1FF
	};

	/* Properties, the media sample class can be a container for a format
	change in which case we take a copy of a type through the SetMediaType
	interface function and then return it when GetMediaType is called. As
	we do no internal processing on it we leave it as a pointer */

	DWORD            m_dwFlags;         /* Flags for this sample */
	/* Type specific flags are packed
	into the top word
	*/
	DWORD            m_dwTypeSpecificFlags; /* Media type specific flags */
	LPBYTE           m_pBuffer;         /* Pointer to the complete buffer */
	LONG             m_lActual;         /* Length of data in this sample */
	LONG             m_cbBuffer;        /* Size of the buffer */
	CBaseAllocator  *m_pAllocator;      /* The allocator who owns us */
	CMediaSample     *m_pNext;          /* Chaining in free list */
	REFERENCE_TIME   m_Start;           /* Start sample time */
	REFERENCE_TIME   m_End;             /* End sample time */
	LONGLONG         m_MediaStart;      /* Real media start position */
	LONG             m_MediaEnd;        /* A difference to get the end */
	AM_MEDIA_TYPE    *m_pMediaType;     /* Media type change data */
	DWORD            m_dwStreamId;      /* Stream id */
public:
	LONG             m_cRef;            /* Reference count */


public:

	CMediaSample(
		TCHAR *pName,
		CBaseAllocator *pAllocator,
		HRESULT *phr,
		LPBYTE pBuffer = NULL,
		LONG length = 0);
#ifdef UNICODE
	CMediaSample(
		CHAR *pName,
		CBaseAllocator *pAllocator,
		HRESULT *phr,
		LPBYTE pBuffer = NULL,
		LONG length = 0);
#endif

	virtual ~CMediaSample();

	/* Note the media sample does not delegate to its owner */

	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// set the buffer pointer and length. Used by allocators that
	// want variable sized pointers or pointers into already-read data.
	// This is only available through a CMediaSample* not an IMediaSample*
	// and so cannot be changed by clients.
	HRESULT SetPointer(BYTE * ptr, LONG cBytes);

	// Get me a read/write pointer to this buffer's memory.
	STDMETHODIMP GetPointer(BYTE ** ppBuffer);

	STDMETHODIMP_(LONG) GetSize(void);

	// get the stream time at which this sample should start and finish.
	STDMETHODIMP GetTime(
		REFERENCE_TIME * pTimeStart,     // put time here
		REFERENCE_TIME * pTimeEnd
		);

	// Set the stream time at which this sample should start and finish.
	STDMETHODIMP SetTime(
		REFERENCE_TIME * pTimeStart,     // put time here
		REFERENCE_TIME * pTimeEnd
		);
	STDMETHODIMP IsSyncPoint(void);
	STDMETHODIMP SetSyncPoint(BOOL bIsSyncPoint);
	STDMETHODIMP IsPreroll(void);
	STDMETHODIMP SetPreroll(BOOL bIsPreroll);

	STDMETHODIMP_(LONG) GetActualDataLength(void);
	STDMETHODIMP SetActualDataLength(LONG lActual);

	// these allow for limited format changes in band

	STDMETHODIMP GetMediaType(AM_MEDIA_TYPE **ppMediaType);
	STDMETHODIMP SetMediaType(AM_MEDIA_TYPE *pMediaType);

	// returns S_OK if there is a discontinuity in the data (this same is
	// not a continuation of the previous stream of data
	// - there has been a seek).
	STDMETHODIMP IsDiscontinuity(void);
	// set the discontinuity property - TRUE if this sample is not a
	// continuation, but a new sample after a seek.
	STDMETHODIMP SetDiscontinuity(BOOL bDiscontinuity);

	// get the media times for this sample
	STDMETHODIMP GetMediaTime(
		LONGLONG * pTimeStart,
		LONGLONG * pTimeEnd
		);

	// Set the media times for this sample
	STDMETHODIMP SetMediaTime(
		LONGLONG * pTimeStart,
		LONGLONG * pTimeEnd
		);

	// Set and get properties (IMediaSample2)
	STDMETHODIMP GetProperties(
		DWORD cbProperties,
		BYTE * pbProperties
		);

	STDMETHODIMP SetProperties(
		DWORD cbProperties,
		const BYTE * pbProperties
		);
};
