// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "targetver.h"

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
#endif

// WebSocket++ generates some build warnings internally that just add noise to the build. The library author considers
// them safe to be ignored so we'll do so here.
// * C4127 - conditional expression is constant
// * C4244 - conversion' conversion from 'type1' to 'type2', possible loss of data
// * C4267 - narrowing conversion of size_t
// * C4834 - discarding return value of function with 'nodiscard' attribute
// * C4996 - usage of deprecated functions
#pragma warning( push )
#pragma warning( disable : 4127 4244 4267 4834 4996 )
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning( pop )

#include <cstdint>
#include <map>
#include <memory>
#include <set>
#include <string>

#include <ChakraCore.h>
#include <ChakraDebugProtocolHandler.h>
