#pragma once

#include <ChakraCore.h>
#include <cstdio>
#include <string>

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

//
// This "throws" an exception in the Chakra space. Useful routine for callbacks
// that need to throw a JS error to indicate failure.
//
inline void ThrowJsError(std::wstring errorString)
{
    // We ignore error since we're already in an error state.
    JsValueRef errorValue = JS_INVALID_REFERENCE;
    JsValueRef errorObject = JS_INVALID_REFERENCE;
    JsPointerToString(errorString.c_str(), errorString.length(), &errorValue);
    JsCreateError(errorValue, &errorObject);
    JsSetException(errorObject);
}
