// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ChakraDebugService.h"
#include "Service.h"
#include <TranslateExceptionToJsErrorCode.h>

CHAKRA_API JsDebugServiceCreate(JsDebugService* service)
{
    if (service == nullptr)
    {
        return JsErrorInvalidArgument;
    }

    return JsDebug::TranslateExceptionToJsErrorCode(
        [&]() -> void
        {
            auto instance = std::make_unique<JsDebug::Service>();

            // Release ownership of the pointer
            *service = reinterpret_cast<JsDebugService>(instance.release());
        });
}

CHAKRA_API JsDebugServiceDestroy(JsDebugService service)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::Service*>(
        service,
        [&](JsDebug::Service* instance) -> void
        {
            // Take ownership of the pointer so that it gets released at the exit of the function.
            auto holder = std::unique_ptr<JsDebug::Service>(instance);
        });
}

CHAKRA_API JsDebugServiceRegisterHandler(
    JsDebugService service,
    const char* id,
    JsDebugProtocolHandler protocolHandler,
    bool breakOnNextLine)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::Service*>(
        service,
        [&](JsDebug::Service* instance) -> void
        {
            instance->RegisterHandler(id, protocolHandler, breakOnNextLine);
        });
}

CHAKRA_API JsDebugServiceUnregisterHandler(JsDebugService service, const char* id)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::Service*>(
        service,
        [&](JsDebug::Service* instance) -> void
        {
            instance->UnregisterHandler(id);
        });
}

CHAKRA_API JsDebugServiceListen(JsDebugService service, uint16_t port)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::Service*>(
        service,
        [&](JsDebug::Service* instance) -> void
        {
            instance->Listen(port);
        });
}

CHAKRA_API JsDebugServiceClose(JsDebugService service)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::Service*>(
        service,
        [&](JsDebug::Service* instance) -> void
        {
            instance->Close();
        });
}
