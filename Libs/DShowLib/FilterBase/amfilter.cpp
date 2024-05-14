//------------------------------------------------------------------------------
// File: AMFilter.cpp
//
// Desc: DirectShow base classes - implements class hierarchy for streams
//       architecture.
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//------------------------------------------------------------------------------


//=====================================================================
//=====================================================================
// The following classes are declared in this header:
//
//
// CBaseMediaFilter            Basic IMediaFilter support (abstract class)
// CBaseFilter                 Support for IBaseFilter (incl. IMediaFilter)
// CEnumPins                   Enumerate input and output pins
// CEnumMediaTypes             Enumerate the preferred pin formats
// CBasePin                    Abstract base class for IPin interface
//    CBaseOutputPin           Adds data provider member functions
//    CBaseInputPin            Implements IMemInputPin interface
// CMediaSample                Basic transport unit for IMemInputPin
// CBaseAllocator              General list guff for most allocators
//    CMemAllocator            Implements memory buffer allocation
//
//=====================================================================
//=====================================================================

#include "StdAfx.h"



//=====================================================================
// Helpers
//=====================================================================

































//  Remove warnings about unreferenced inline functions
#pragma warning(disable:4514)

