//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ChakraCore.h>

typedef struct JsDebugProtocolHandler__ *JsDebugProtocolHandler;
typedef void(CHAKRA_CALLBACK *JsDebugProtocolHandlerSendResponseCallback)(const char *response);

/// <summary>Creates a <seealso cref="JsDebugProtocolHandler" /> instance for a given runtime.</summary>
/// <remarks>
///     It also implicitly enables debugging on the given runtime, so it will need to only be done when the engine is
///     not currently running script. This should be called before any code has been executed in the runtime.
/// </remarks>
/// <param name="runtime">The runtime to debug.</param>
/// <param name="protocolHandler">The newly created instance.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugProtocolHandlerCreate(JsRuntimeHandle runtime, JsDebugProtocolHandler* protocolHandler);

/// <summary>Destroys the instance object.</summary>
/// <remarks>
///     It also implicitly disables debugging on the given runtime, so it will need to only be done when the engine is
///     not currently running script.
/// </remarks>
/// <param name="protocolHandler">The instance to destroy.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugProtocolHandlerDestroy(JsDebugProtocolHandler protocolHandler);

/// <summary>Connect a callback to the protocol handler.</summary>
/// <remarks>
///     Any events that occurred before connecting will be queued and dispatched upon successful connection.
/// </remarks>
/// <param name="protocolHandler">The instance to connect to.</param>
/// <param name="breakOnNextLine">Indicates whether to break on the next line of code.</param>
/// <param name="callback">The response callback function pointer.</param>
/// <param name="state">The state object to return on each invocation of the callback.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugProtocolHandlerConnect(
    JsDebugProtocolHandler protocolHandler,
    bool breakOnNextLine,
    JsDebugProtocolHandlerSendResponseCallback callback,
    void* state);

/// <summary>Disconnect from the protocol handler and clear any breakpoints.</summary>
/// <param name="protocolHandler">The instance to disconnect from.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugProtocolHandlerDisconnect(JsDebugProtocolHandler protocolHandler);

/// <summary>Send an incoming JSON-formatted command to the protocol handler.</summary>
/// <remarks>
///     The response will be returned asynchronously.
/// </remarks>
/// <param name="protocolHandler">The receiving protocol handler.</param>
/// <param name="command">The JSON-formatted command to send.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugProtocolHandlerSendCommand(JsDebugProtocolHandler protocolHandler, const char* command);
