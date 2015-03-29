#pragma once

//=====================================================================
//=====================================================================
// Defines CEnumPins
//
// Pin enumerator class that works by calling CBaseFilter. This interface
// is provided by CBaseFilter::EnumPins and calls GetPinCount() and
// GetPin() to enumerate existing pins. Needs to be a separate object so
// that it can be cloned (creating an existing object at the same
// position in the enumeration)
//
//=====================================================================
//=====================================================================

class CBaseFilter;

class CEnumPins : public IEnumPins      // The interface we support
{
	int m_Position;                 // Current ordinal position
	int m_PinCount;                 // Number of pins available
	CBaseFilter *m_pFilter;         // The filter who owns us
	LONG m_Version;                 // Pin version information
	LONG m_cRef;

	typedef CGenericList<CBasePin> CPinList;

	CPinList m_PinCache;	    // These pointers have not been AddRef'ed and
	// so they should not be dereferenced.  They are
	// merely kept to ID which pins have been enumerated.

#ifdef DEBUG
	DWORD m_dwCookie;
#endif

	/* If while we are retrieving a pin for example from the filter an error
	occurs we assume that our internal state is stale with respect to the
	filter (someone may have deleted all the pins). We can check before
	starting whether or not the operation is likely to fail by asking the
	filter what it's current version number is. If the filter has not
	overriden the GetPinVersion method then this will always match */

	BOOL AreWeOutOfSync() {
		return (m_pFilter->GetPinVersion() == m_Version ? FALSE : TRUE);
	};

	/* This method performs the same operations as Reset, except is does not clear
	the cache of pins already enumerated. */

	STDMETHODIMP Refresh();

public:

	CEnumPins(
		CBaseFilter *pFilter,
		CEnumPins *pEnumPins);

	virtual ~CEnumPins();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IEnumPins
	STDMETHODIMP Next(
		ULONG cPins,         // place this many pins...
		IPin ** ppPins,      // ...in this array of IPin*
		ULONG * pcFetched    // actual count passed returned here
		);

	STDMETHODIMP Skip(ULONG cPins);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumPins **ppEnum);


};

