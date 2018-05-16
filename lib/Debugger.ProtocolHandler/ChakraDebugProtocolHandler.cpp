//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ChakraDebugProtocolHandler.h"
#include "ProtocolHandler.h"
#include "TranslateExceptionToJsErrorCode.h"

CHAKRA_API JsDebugProtocolHandlerCreate(JsRuntimeHandle runtime, JsDebugProtocolHandler* protocolHandler)
{
    if (protocolHandler == nullptr)
    {
        return JsErrorInvalidArgument;
    }

    return JsDebug::TranslateExceptionToJsErrorCode(
        [&]() -> void
        {
            auto instance = std::make_unique<JsDebug::ProtocolHandler>(runtime);

            // Release ownership of the pointer
            *protocolHandler = reinterpret_cast<JsDebugProtocolHandler>(instance.release());
        });
}

CHAKRA_API JsDebugProtocolHandlerDestroy(JsDebugProtocolHandler protocolHandler)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::ProtocolHandler*>(
        protocolHandler,
        [&](JsDebug::ProtocolHandler* instance) -> void
        {
            // Take ownership of the pointer so that it gets released at the exit of the function.
            auto holder = std::unique_ptr<JsDebug::ProtocolHandler>(instance);
        });
}

CHAKRA_API JsDebugProtocolHandlerConnect(
    JsDebugProtocolHandler protocolHandler,
    bool breakOnNextLine,
    JsDebugProtocolHandlerSendResponseCallback callback,
    void* callbackState)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::ProtocolHandler*>(
        protocolHandler,
        [&](JsDebug::ProtocolHandler* instance) -> void
        {
            instance->Connect(breakOnNextLine, callback, callbackState);
        });
}

CHAKRA_API JsDebugProtocolHandlerDisconnect(JsDebugProtocolHandler protocolHandler)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::ProtocolHandler*>(
        protocolHandler,
        [&](JsDebug::ProtocolHandler* instance) -> void
        {
            instance->Disconnect();
        });
}

CHAKRA_API JsDebugProtocolHandlerSendCommand(JsDebugProtocolHandler protocolHandler, const char* command)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::ProtocolHandler*>(
        protocolHandler,
        [&](JsDebug::ProtocolHandler* instance) -> void
        {
            instance->SendCommand(command);
        });
}

CHAKRA_API JsDebugProtocolHandlerWaitForDebugger(JsDebugProtocolHandler protocolHandler)
{
    return JsDebug::TranslateExceptionToJsErrorCode<JsDebug::ProtocolHandler*>(
        protocolHandler,
        [&](JsDebug::ProtocolHandler* instance) -> void
        {
            instance->WaitForDebugger();
        });
}
