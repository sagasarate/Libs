#include "StdAfx.h"

//=====================================================================
//=====================================================================
// Implements CEnumPins
//=====================================================================
//=====================================================================


CEnumPins::CEnumPins(CBaseFilter *pFilter,
					 CEnumPins *pEnumPins) :
m_Position(0),
m_PinCount(0),
m_pFilter(pFilter),
m_cRef(1),               // Already ref counted
m_PinCache(NAME("Pin Cache"))
{

#ifdef DEBUG
	m_dwCookie = DbgRegisterObjectCreation("CEnumPins", 0);
#endif

	/* We must be owned by a filter derived from CBaseFilter */

	ASSERT(pFilter != NULL);

	/* Hold a reference count on our filter */
	m_pFilter->AddRef();

	/* Are we creating a new enumerator */

	if (pEnumPins == NULL) {
		m_Version = m_pFilter->GetPinVersion();
		m_PinCount = m_pFilter->GetPinCount();
	} else {
		ASSERT(m_Position <= m_PinCount);
		m_Position = pEnumPins->m_Position;
		m_PinCount = pEnumPins->m_PinCount;
		m_Version = pEnumPins->m_Version;
		m_PinCache.AddTail(&(pEnumPins->m_PinCache));
	}
}


/* Destructor releases the reference count on our filter NOTE since we hold
a reference count on the filter who created us we know it is safe to
release it, no access can be made to it afterwards though as we have just
caused the last reference count to go and the object to be deleted */

CEnumPins::~CEnumPins()
{
	m_pFilter->Release();

#ifdef DEBUG
	DbgRegisterObjectDestruction(m_dwCookie);
#endif
}


/* Override this to say what interfaces we support where */

STDMETHODIMP
CEnumPins::QueryInterface(REFIID riid,void **ppv)
{
	CheckPointer(ppv, E_POINTER);

	/* Do we have this interface */

	if (riid == IID_IEnumPins || riid == IID_IUnknown) {
		return GetInterface((IEnumPins *) this, ppv);
	} else {
		*ppv = NULL;
		return E_NOINTERFACE;
	}
}

STDMETHODIMP_(ULONG)
CEnumPins::AddRef()
{
	return InterlockedIncrement(&m_cRef);
}

STDMETHODIMP_(ULONG)
CEnumPins::Release()
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
CEnumPins::Clone(IEnumPins **ppEnum)
{
	CheckPointer(ppEnum,E_POINTER);
	ValidateReadWritePtr(ppEnum,sizeof(IEnumPins *));
	HRESULT hr = NOERROR;

	/* Check we are still in sync with the filter */
	if (AreWeOutOfSync() == TRUE) {
		*ppEnum = NULL;
		hr =  VFW_E_ENUM_OUT_OF_SYNC;
	} else {

		*ppEnum = new CEnumPins(m_pFilter,
			this);
		if (*ppEnum == NULL) {
			hr = E_OUTOFMEMORY;
		}
	}
	return hr;
}


/* Return the next pin after the current position */

STDMETHODIMP
CEnumPins::Next(ULONG cPins,        // place this many pins...
				IPin **ppPins,      // ...in this array
				ULONG *pcFetched)   // actual count passed returned here
{
	CheckPointer(ppPins,E_POINTER);
	ValidateReadWritePtr(ppPins,cPins * sizeof(IPin *));

	ASSERT(ppPins);

	if (pcFetched!=NULL) {
		ValidateWritePtr(pcFetched, sizeof(ULONG));
		*pcFetched = 0;           // default unless we succeed
	}
	// now check that the parameter is valid
	else if (cPins>1) {   // pcFetched == NULL
		return E_INVALIDARG;
	}
	ULONG cFetched = 0;           // increment as we get each one.

	/* Check we are still in sync with the filter */
	if (AreWeOutOfSync() == TRUE) {
		// If we are out of sync, we should refresh the enumerator.
		// This will reset the position and update the other members, but
		// will not clear cache of pins we have already returned.
		Refresh();
	}

	/* Calculate the number of available pins */

	int cRealPins = min(m_PinCount - m_Position, (int) cPins);
	if (cRealPins == 0) {
		return S_FALSE;
	}

	/* Return each pin interface NOTE GetPin returns CBasePin * not addrefed
	so we must QI for the IPin (which increments its reference count)
	If while we are retrieving a pin from the filter an error occurs we
	assume that our internal state is stale with respect to the filter
	(for example someone has deleted a pin) so we
	return VFW_E_ENUM_OUT_OF_SYNC                            */

	while (cRealPins && (m_PinCount - m_Position)) {

		/* Get the next pin object from the filter */

		CBasePin *pPin = m_pFilter->GetPin(m_Position++);
		if (pPin == NULL) {
			// If this happend, and it's not the first time through, then we've got a problem,
			// since we should really go back and release the iPins, which we have previously
			// AddRef'ed.
			ASSERT( cFetched==0 );
			return VFW_E_ENUM_OUT_OF_SYNC;
		}

		/* We only want to return this pin, if it is not in our cache */
		if (0 == m_PinCache.Find(pPin))
		{
			/* From the object get an IPin interface */

			*ppPins = pPin;
			pPin->AddRef();

			cFetched++;
			ppPins++;

			m_PinCache.AddTail(pPin);

			cRealPins--;

		}
	}

	if (pcFetched!=NULL) {
		*pcFetched = cFetched;
	}

	return (cPins==cFetched ? NOERROR : S_FALSE);
}


/* Skip over one or more entries in the enumerator */

STDMETHODIMP
CEnumPins::Skip(ULONG cPins)
{
	/* Check we are still in sync with the filter */
	if (AreWeOutOfSync() == TRUE) {
		return VFW_E_ENUM_OUT_OF_SYNC;
	}

	/* Work out how many pins are left to skip over */
	/* We could position at the end if we are asked to skip too many... */
	/* ..which would match the base implementation for CEnumMediaTypes::Skip */

	ULONG PinsLeft = m_PinCount - m_Position;
	if (cPins > PinsLeft) {
		return S_FALSE;
	}
	m_Position += cPins;
	return NOERROR;
}


/* Set the current position back to the start */
/* Reset has 4 simple steps:
*
* Set position to head of list
* Sync enumerator with object being enumerated
* Clear the cache of pins already returned
* return S_OK
*/

STDMETHODIMP
CEnumPins::Reset()
{
	m_Version = m_pFilter->GetPinVersion();
	m_PinCount = m_pFilter->GetPinCount();

	m_Position = 0;

	// Clear the cache
	m_PinCache.RemoveAll();

	return S_OK;
}


/* Set the current position back to the start */
/* Refresh has 3 simple steps:
*
* Set position to head of list
* Sync enumerator with object being enumerated
* return S_OK
*/

STDMETHODIMP
CEnumPins::Refresh()
{
	m_Version = m_pFilter->GetPinVersion();
	m_PinCount = m_pFilter->GetPinCount();

	m_Position = 0;
	return S_OK;
}
