//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

// WebSocket++ generates some build warnings internally that just add noise to the build. The library author considers
// them safe to be ignored so we'll do so here.
// * C4267 - narrowing conversion of size_t
// * C4996 - usage of deprecated functions
#pragma warning( push )
#pragma warning( disable : 4267 4996 )
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning( pop )

#include <cstdint>
#include <map>
#include <memory>
#include <set>

#include <ChakraCore.h>
#include <ChakraDebugProtocolHandler.h>
