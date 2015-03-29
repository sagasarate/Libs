#include "StdAfx.h"


//=====================================================================
//=====================================================================
// Implements CEnumMediaTypes
//=====================================================================
//=====================================================================


CEnumMediaTypes::CEnumMediaTypes(CBasePin *pPin,
								 CEnumMediaTypes *pEnumMediaTypes) :
m_Position(0),
m_pPin(pPin),
m_cRef(1)
{

#ifdef DEBUG
	m_dwCookie = DbgRegisterObjectCreation("CEnumMediaTypes", 0);
#endif

	/* We must be owned by a pin derived from CBasePin */

	ASSERT(pPin != NULL);

	/* Hold a reference count on our pin */
	m_pPin->AddRef();

	/* Are we creating a new enumerator */

	if (pEnumMediaTypes == NULL) {
		m_Version = m_pPin->GetMediaTypeVersion();
		return;
	}

	m_Position = pEnumMediaTypes->m_Position;
	m_Version = pEnumMediaTypes->m_Version;
}


/* Destructor releases the reference count on our base pin. NOTE since we hold
a reference count on the pin who created us we know it is safe to release
it, no access can be made to it afterwards though as we might have just
caused the last reference count to go and the object to be deleted */

CEnumMediaTypes::~CEnumMediaTypes()
{
#ifdef DEBUG
	DbgRegisterObjectDestruction(m_dwCookie);
#endif
	m_pPin->Release();
}


/* Override this to say what interfaces we support where */

STDMETHODIMP
CEnumMediaTypes::QueryInterface(REFIID riid,void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	/* Do we have this interface */

	if (riid == IID_IEnumMediaTypes || riid == IID_IUnknown) {
		return GetInterface((IEnumMediaTypes *) this, ppv);
	} else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG)
CEnumMediaTypes::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CEnumMediaTypes::Release()
{
	ULONG cRef = InterlockedDecrement(&m_cRef);
	if (cRef == 0) {
		delete this;
	}
	return cRef;
}

/* One of an enumerator's basic member functions allows us to create a cloned
interface that initially has the same state. Since we are taking a snapshot
of an object (current position and all) we must lock access at the start */

STDMETHODIMP
CEnumMediaTypes::Clone(IEnumMediaTypes **ppEnum)
{
	CheckPointer(ppEnum,E_POINTER);
	ValidateReadWritePtr(ppEnum,sizeof(IEnumMediaTypes *));
	HRESULT hr = NOERROR;

	/* Check we are still in sync with the pin */
	if (AreWeOutOfSync() == TRUE) {
		*ppEnum = NULL;
		hr = VFW_E_ENUM_OUT_OF_SYNC;
	} else {

		*ppEnum = new CEnumMediaTypes(m_pPin,
			this);

		if (*ppEnum == NULL) {
			hr =  E_OUTOFMEMORY;
		}
	}
	return hr;
}


/* Enumerate the next pin(s) after the current position. The client using this
interface passes in a pointer to an array of pointers each of which will
be filled in with a pointer to a fully initialised media type format
Return NOERROR if it all works,
S_FALSE if fewer than cMediaTypes were enumerated.
VFW_E_ENUM_OUT_OF_SYNC if the enumerator has been broken by
state changes in the filter
The actual count always correctly reflects the number of types in the array.
*/

STDMETHODIMP
CEnumMediaTypes::Next(ULONG cMediaTypes,          // place this many types...
					  AM_MEDIA_TYPE **ppMediaTypes,   // ...in this array
					  ULONG *pcFetched)           // actual count passed
{
	CheckPointer(ppMediaTypes,E_POINTER);
	ValidateReadWritePtr(ppMediaTypes,cMediaTypes * sizeof(AM_MEDIA_TYPE *));
	/* Check we are still in sync with the pin */
	if (AreWeOutOfSync() == TRUE) {
		return VFW_E_ENUM_OUT_OF_SYNC;
	}

	if (pcFetched!=NULL) {
		ValidateWritePtr(pcFetched, sizeof(ULONG));
		*pcFetched = 0;           // default unless we succeed
	}
	// now check that the parameter is valid
	else if (cMediaTypes>1) {     // pcFetched == NULL
		return E_INVALIDARG;
	}
	ULONG cFetched = 0;           // increment as we get each one.

	/* Return each media type by asking the filter for them in turn - If we
	have an error code retured to us while we are retrieving a media type
	we assume that our internal state is stale with respect to the filter
	(for example the window size changing) so we return
	VFW_E_ENUM_OUT_OF_SYNC */

	while (cMediaTypes) {

		CMediaType cmt;

		HRESULT hr = m_pPin->GetMediaType(m_Position++, &cmt);
		if (S_OK != hr) {
			break;
		}

		/* We now have a CMediaType object that contains the next media type
		but when we assign it to the array position we CANNOT just assign
		the AM_MEDIA_TYPE structure because as soon as the object goes out of
		scope it will delete the memory we have just copied. The function
		we use is CreateMediaType which allocates a task memory block */

		/*  Transfer across the format block manually to save an allocate
		and free on the format block and generally go faster */

		*ppMediaTypes = (AM_MEDIA_TYPE *)CoTaskMemAlloc(sizeof(AM_MEDIA_TYPE));
		if (*ppMediaTypes == NULL) {
			break;
		}

		/*  Do a regular copy */
		**ppMediaTypes = (AM_MEDIA_TYPE)cmt;

		/*  Make sure the destructor doesn't free these */
		cmt.pbFormat = NULL;
		cmt.cbFormat = NULL;
		cmt.pUnk     = NULL;


		ppMediaTypes++;
		cFetched++;
		cMediaTypes--;
	}

	if (pcFetched!=NULL) {
		*pcFetched = cFetched;
	}

	return ( cMediaTypes==0 ? NOERROR : S_FALSE );
}


/* Skip over one or more entries in the enumerator */

STDMETHODIMP
CEnumMediaTypes::Skip(ULONG cMediaTypes)
{
	//  If we're skipping 0 elements we're guaranteed to skip the
	//  correct number of elements
	if (cMediaTypes == 0) {
		return S_OK;
	}

	/* Check we are still in sync with the pin */
	if (AreWeOutOfSync() == TRUE) {
		return VFW_E_ENUM_OUT_OF_SYNC;
	}

	m_Position += cMediaTypes;

	/*  See if we're over the end */
	CMediaType cmt;
	return S_OK == m_pPin->GetMediaType(m_Position - 1, &cmt) ? S_OK : S_FALSE;
}


/* Set the current position back to the start */
/* Reset has 3 simple steps:
*
* set position to head of list
* sync enumerator with object being enumerated
* return S_OK
*/

STDMETHODIMP
CEnumMediaTypes::Reset()

{
	m_Position = 0;

	// Bring the enumerator back into step with the current state.  This
	// may be a noop but ensures that the enumerator will be valid on the
	// next call.
	m_Version = m_pPin->GetMediaTypeVersion();
	return NOERROR;
}
