#pragma once

class CAutoUsingOutputPin
{
public:
	CAutoUsingOutputPin( CDynamicOutputPin* pOutputPin, HRESULT* phr );
	~CAutoUsingOutputPin();

private:
	CDynamicOutputPin* m_pOutputPin;
};

inline CAutoUsingOutputPin::CAutoUsingOutputPin( CDynamicOutputPin* pOutputPin, HRESULT* phr ) :
m_pOutputPin(NULL)
{
	// The caller should always pass in valid pointers.
	ASSERT( NULL != pOutputPin );
	ASSERT( NULL != phr );

	// Make sure the user initialized phr.
	ASSERT( S_OK == *phr );

	HRESULT hr = pOutputPin->StartUsingOutputPin();
	if( FAILED( hr ) )
	{
		*phr = hr;
		return;
	}

	m_pOutputPin = pOutputPin;
}

inline CAutoUsingOutputPin::~CAutoUsingOutputPin()
{
	if( NULL != m_pOutputPin )
	{
		m_pOutputPin->StopUsingOutputPin();
	}
}
