#pragma once

//=====================================================================
//=====================================================================
// Defines CEnumMediaTypes
//
// Enumerates the preferred formats for input and output pins
//=====================================================================
//=====================================================================



class CEnumMediaTypes : public IEnumMediaTypes    // The interface we support
{
	int m_Position;           // Current ordinal position
	CBasePin *m_pPin;         // The pin who owns us
	LONG m_Version;           // Media type version value
	LONG m_cRef;
#ifdef DEBUG
	DWORD m_dwCookie;
#endif

	/* The media types a filter supports can be quite dynamic so we add to
	the general IEnumXXXX interface the ability to be signaled when they
	change via an event handle the connected filter supplies. Until the
	Reset method is called after the state changes all further calls to
	the enumerator (except Reset) will return E_UNEXPECTED error code */

	BOOL AreWeOutOfSync() {
		return (m_pPin->GetMediaTypeVersion() == m_Version ? FALSE : TRUE);
	};

public:

	CEnumMediaTypes(
		CBasePin *pPin,
		CEnumMediaTypes *pEnumMediaTypes);

	virtual ~CEnumMediaTypes();

	// IUnknown
	STDMETHODIMP QueryInterface(REFIID riid, void **ppv);
	STDMETHODIMP_(ULONG) AddRef();
	STDMETHODIMP_(ULONG) Release();

	// IEnumMediaTypes
	STDMETHODIMP Next(
		ULONG cMediaTypes,          // place this many pins...
		AM_MEDIA_TYPE ** ppMediaTypes,  // ...in this array
		ULONG * pcFetched           // actual count passed
		);

	STDMETHODIMP Skip(ULONG cMediaTypes);
	STDMETHODIMP Reset();
	STDMETHODIMP Clone(IEnumMediaTypes **ppEnum);
};