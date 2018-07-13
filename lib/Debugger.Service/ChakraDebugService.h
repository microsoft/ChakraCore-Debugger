// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ChakraCore.h>
#include <ChakraDebugProtocolHandler.h>

typedef struct JsDebugService__* JsDebugService;

/// <summary>Creates a <seealso cref="JsDebugProtocolHandler" /> instance.</summary>
/// <param name="service">The newly created instance.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceCreate(JsDebugService* service);

/// <summary>Destroys the instance object.</summary>
/// <param name="service">The instance to destroy.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceDestroy(JsDebugService service);

/// <summary>Register a handler instance with a given instance.</summary>
/// <param name="service">The instance to register with.</param>
/// <param name="id">The ID of the handler (it must be unique).</param>
/// <param name="handler">The handler instance.</param>
/// <param name="breakOnNextLine">Indicates whether to break on the next line of code.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceRegisterHandler(
    JsDebugService service,
    const char* id,
    JsDebugProtocolHandler handler,
    bool breakOnNextLine);

/// <summary>Unregister a handler instance from a given instance.</summary>
/// <param name="service">The instance to unregister from.</param>
/// <param name="id">The ID of the handler to unregister.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceUnregisterHandler(JsDebugService service, const char* id);

/// <summary>Start listening on a given port.</summary>
/// <param name="service">The instance to listen with.</param>
/// <param name="port">The port number to listen on.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceListen(JsDebugService service, uint16_t port);

/// <summary>Stop listening and close any connections.</summary>
/// <param name="service">The instance to close.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceClose(JsDebugService service);
