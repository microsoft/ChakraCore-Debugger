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
#include <memory>
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

class DebugProtocolHandler
{
private:
    JsDebugProtocolHandler m_protocolHandler{ nullptr };

public:
    DebugProtocolHandler(JsRuntimeHandle runtime)
    {
        JsDebugProtocolHandler protocolHandler;

        JsErrorCode result = JsDebugProtocolHandlerCreate(runtime, &protocolHandler);

        if (result != JsNoError)
        {
            throw std::exception("Unable to create debug protocol handler.");
        }

        m_protocolHandler = protocolHandler;
    }

    ~DebugProtocolHandler()
    {
        Destroy();
    }
    
    JsErrorCode Connect(bool breakOnNextLine, JsDebugProtocolHandlerSendResponseCallback callback, void* callbackState)
    {
        JsErrorCode result = JsDebugProtocolHandlerConnect(m_protocolHandler,
            breakOnNextLine, callback, callbackState);

        return result;
    }

    JsErrorCode Destroy()
    {
        JsErrorCode result = JsNoError;

        if (m_protocolHandler != nullptr)
        {
            result = JsDebugProtocolHandlerDestroy(m_protocolHandler);

            if (result == JsNoError)
            {
                m_protocolHandler = nullptr;
            }
        }

        return result;
    }

    JsErrorCode Disconnect()
    {
        JsErrorCode result = JsDebugProtocolHandlerDisconnect(m_protocolHandler);

        return result;
    }

    JsDebugProtocolHandler GetHandle()
    {
        return m_protocolHandler;
    }

    JsErrorCode WaitForDebugger()
    {
        JsErrorCode result = JsDebugProtocolHandlerWaitForDebugger(m_protocolHandler);

        return result;
    }
};


class DebugService
{
private:
    JsDebugService m_service{ nullptr };

public:
    DebugService(JsRuntimeHandle runtime)
    {
        JsDebugService service;

        JsErrorCode result = JsDebugServiceCreate(&service);

        if (result != JsNoError)
        {
            throw new std::exception("Unable to create debug service.");
        }

        m_service = service;
    }

    ~DebugService()
    {
        Destroy();
    }

    JsErrorCode Close()
    {
        JsErrorCode result = JsDebugServiceClose(m_service);

        return result;
    }

    JsErrorCode Destroy()
    {
        JsErrorCode result = JsNoError;

        if (m_service != nullptr)
        {
            result = JsDebugServiceDestroy(m_service);

            if (result == JsNoError)
            {
                m_service = nullptr;
            }
        }

        return result;
    }

    JsErrorCode Listen(uint16_t port)
    {
        JsErrorCode result = JsDebugServiceListen(m_service, port);

        return result;
    }

    JsErrorCode RegisterHandler(std::string const& runtimeName, DebugProtocolHandler& protocolHandler, bool breakOnNextLine)
    {
        JsErrorCode result = JsDebugServiceRegisterHandler(m_service, runtimeName.c_str(), protocolHandler.GetHandle(), breakOnNextLine);

        return result;
    }

    JsErrorCode UnregisterHandler(std::string const& runtimeName)
    {
        JsErrorCode result = JsDebugServiceUnregisterHandler(m_service, runtimeName.c_str());

        return result;
    }
};
