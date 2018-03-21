//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ChakraDebugService.h"
#include "Service.h"
#include <ProtocolHandler.h>

#include <memory>

CHAKRA_API JsDebugServiceCreate(JsDebugService* service)
{
    auto svc = std::make_unique<JsDebug::Service>();

    // Do stuff.

    // Release ownership of the pointer
    *service = reinterpret_cast<JsDebugService>(svc.release());
    return JsNoError;
}

CHAKRA_API JsDebugServiceDestroy(JsDebugService service)
{
    // Take ownership of the pointer
    auto svc = std::unique_ptr<JsDebug::Service>(reinterpret_cast<JsDebug::Service*>(service));

    // Do stuff.

    return JsNoError;
}

CHAKRA_API JsDebugServiceRegisterHandler(
    JsDebugService service,
    const char* id,
    JsDebugProtocolHandler protocolHandler,
    bool breakOnNextLine)
{
    auto svc = reinterpret_cast<JsDebug::Service*>(service);
    svc->RegisterHandler(id, protocolHandler, breakOnNextLine);

    return JsNoError;
}

CHAKRA_API JsDebugServiceUnregisterHandler(JsDebugService service, const char* id)
{
    auto svc = reinterpret_cast<JsDebug::Service*>(service);
    svc->UnregisterHandler(id);

    return JsNoError;
}

CHAKRA_API JsDebugServiceListen(JsDebugService service, uint16_t port)
{
    auto svc = reinterpret_cast<JsDebug::Service*>(service);
    svc->Listen(port);

    return JsNoError;
}

CHAKRA_API JsDebugServiceClose(JsDebugService service)
{
    auto svc = reinterpret_cast<JsDebug::Service*>(service);
    svc->Close();

    return JsNoError;
}
