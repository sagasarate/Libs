//------------------------------------------------------------------------------
// File: AMFilter.h
//
// Desc: DirectShow base classes - efines class hierarchy for streams
//       architecture.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


#ifndef __FILTER__
#define __FILTER__

/* The following classes are declared in this header: */

class CBaseMediaFilter;     // IMediaFilter support
class CBaseFilter;          // IBaseFilter,IMediaFilter support
class CBasePin;             // Abstract base class for IPin interface
class CEnumPins;            // Enumerate input and output pins
class CEnumMediaTypes;      // Enumerate the pin's preferred formats
class CBaseOutputPin;       // Adds data provider member functions
class CBaseInputPin;        // Implements IMemInputPin interface
class CMediaSample;         // Basic transport unit for IMemInputPin
class CBaseAllocator;       // General list guff for most allocators
class CMemAllocator;        // Implements memory buffer allocation


//=====================================================================
//=====================================================================
//
// QueryFilterInfo and QueryPinInfo AddRef the interface pointers
// they return.  You can use the macro below to release the interface.
//
//=====================================================================
//=====================================================================














//=====================================================================
//=====================================================================
// Memory allocators
//
// the shared memory transport between pins requires the input pin
// to provide a memory allocator that can provide sample objects. A
// sample object supports the IMediaSample interface.
//
// CBaseAllocator handles the management of free and busy samples. It
// allocates CMediaSample objects. CBaseAllocator is an abstract class:
// in particular it has no method of initializing the list of free
// samples. CMemAllocator is derived from CBaseAllocator and initializes
// the list of samples using memory from the standard IMalloc interface.
//
// If you want your buffers to live in some special area of memory,
// derive your allocator object from CBaseAllocator. If you derive your
// IMemInputPin interface object from CBaseMemInputPin, you will get
// CMemAllocator-based allocation etc for free and will just need to
// supply the Receive handling, and media type / format negotiation.
//=====================================================================
//=====================================================================













///////////////////////////////////////////////////////////////////////////
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
// ------------------------------------------------------------------------
///////////////////////////////////////////////////////////////////////////

#endif /* __FILTER__ */



