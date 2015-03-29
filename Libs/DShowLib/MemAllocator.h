
#pragma once

//=====================================================================
//=====================================================================
// Defines CMemAllocator
//
// this is an allocator based on CBaseAllocator that allocates sample
// buffers in main memory (from 'new'). You must call SetProperties
// before calling Commit.
//
// we don't free the memory when going into Decommit state. The simplest
// way to implement this without complicating CBaseAllocator is to
// have a Free() function, called to go into decommit state, that does
// nothing and a ReallyFree function called from our destructor that
// actually frees the memory.
//=====================================================================
//=====================================================================



class CMemAllocator : public CBaseAllocator
{

protected:

	LPBYTE m_pBuffer;   // combined memory for all buffers

	// override to free the memory when decommit completes
	// - we actually do nothing, and save the memory until deletion.
	void Free(void);

	// called from the destructor (and from Alloc if changing size/count) to
	// actually free up the memory
	void ReallyFree(void);

	// overriden to allocate the memory when commit called
	HRESULT Alloc(void);

public:
	/* This goes in the factory template table to create new instances */
	static CUnknown *CreateInstance(LPUNKNOWN, HRESULT *);

	STDMETHODIMP SetProperties(
		ALLOCATOR_PROPERTIES* pRequest,
		ALLOCATOR_PROPERTIES* pActual);

	CMemAllocator(TCHAR *, LPUNKNOWN, HRESULT *);
#ifdef UNICODE
	CMemAllocator(CHAR *, LPUNKNOWN, HRESULT *);
#endif
	~CMemAllocator();
};
