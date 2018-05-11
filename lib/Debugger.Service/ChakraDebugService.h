//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

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

/// <summary>Register a handler instance with this service.</summary>
/// <param name="service">The service instance to register with.</param>
/// <param name="id">The ID of the handler (it must be unique).</param>
/// <param name="handler">The handler instance.</param>
/// <param name="breakOnNextLine">Indicates whether to break on the next line of code.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceRegisterHandler(
    JsDebugService service,
    const char* id,
    JsDebugProtocolHandler handler,
    bool breakOnNextLine);

/// <summary>Unregister a handler instance from this service.</summary>
/// <param name="service">The service instance to unregister from.</param>
/// <param name="id">The ID of the handler to unregister.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceUnregisterHandler(JsDebugService service, const char* id);

/// <summary>Start listening on a given port.</summary>
/// <param name="service">The service instance to listen with.</param>
/// <param name="port">The port number to listen on.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceListen(JsDebugService service, uint16_t port);

/// <summary>Stop listening and close any connections.</summary>
/// <param name="service">The service instance to close.</param>
/// <returns>The code <c>JsNoError</c> if the operation succeeded, a failure code otherwise.</returns>
CHAKRA_API JsDebugServiceClose(JsDebugService service);
