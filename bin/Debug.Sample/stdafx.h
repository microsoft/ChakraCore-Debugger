//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

#include <ChakraCore.h>
#include <ChakraDebugProtocolHandler.h>
#include <ChakraDebugService.h>

#define IfFailError(v, e) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            fwprintf(stderr, L"chakrahost: fatal error: %s.\n", (e)); \
            goto error; \
        } \
    }

#define IfFailRet(v) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            return error; \
        } \
    }

#define IfFailThrowJsError(v, e) \
    { \
        JsErrorCode error = (v); \
        if (error != JsNoError) \
        { \
            ThrowJsError((e)); \
            return JS_INVALID_REFERENCE; \
        } \
    }

#define IfComFailError(v) \
    { \
        hr = (v); \
        if (FAILED(hr)) \
        { \
            goto error; \
        } \
    }

#define IfComFailRet(v) \
    { \
        HRESULT hr = (v); \
        if (FAILED(hr)) \
        { \
            return hr; \
        } \
    }
