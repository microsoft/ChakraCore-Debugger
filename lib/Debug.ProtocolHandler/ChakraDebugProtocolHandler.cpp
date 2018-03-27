//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ChakraDebugProtocolHandler.h"
#include "ProtocolHandler.h"

CHAKRA_API JsDebugProtocolHandlerCreate(JsRuntimeHandle runtime, JsDebugProtocolHandler* protocolHandler)
{
    auto handler = std::make_unique<JsDebug::ProtocolHandler>(runtime);

    // Release ownership of the pointer
    *protocolHandler = reinterpret_cast<JsDebugProtocolHandler>(handler.release());
    return JsNoError;
}

CHAKRA_API JsDebugProtocolHandlerDestroy(JsDebugProtocolHandler protocolHandler)
{
    // Take ownership of the pointer so that it gets released at the exit of the function.
    auto handler = std::unique_ptr<JsDebug::ProtocolHandler>(
        reinterpret_cast<JsDebug::ProtocolHandler*>(protocolHandler));

    return JsNoError;
}

CHAKRA_API JsDebugProtocolHandlerConnect(
    JsDebugProtocolHandler protocolHandler,
    bool breakOnNextLine,
    JsDebugProtocolHandlerSendResponseCallback callback,
    void* callbackState)
{
    auto handler = reinterpret_cast<JsDebug::ProtocolHandler*>(protocolHandler);
    handler->Connect(breakOnNextLine, callback, callbackState);

    return JsNoError;
}

CHAKRA_API JsDebugProtocolHandlerDisconnect(JsDebugProtocolHandler protocolHandler)
{
    auto handler = reinterpret_cast<JsDebug::ProtocolHandler*>(protocolHandler);
    handler->Disconnect();

    return JsNoError;
}

CHAKRA_API JsDebugProtocolHandlerSendCommand(JsDebugProtocolHandler protocolHandler, const char* command)
{
    auto handler = reinterpret_cast<JsDebug::ProtocolHandler*>(protocolHandler);
    handler->SendCommand(command);

    return JsNoError;
}

CHAKRA_API JsDebugProtocolHandlerWaitForDebugger(JsDebugProtocolHandler protocolHandler)
{
    auto handler = reinterpret_cast<JsDebug::ProtocolHandler*>(protocolHandler);
    handler->WaitForDebugger();

    return JsNoError;
}
