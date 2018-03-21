//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include <ChakraCore.h>
#include <ChakraDebugProtocolHandler.h>
#include <ChakraDebugService.h>

#include <cstdio>

int main()
{
    // TODO: Handle errors appropriately. This code is just enough to get the major components loaded and executing,
    // not ready for actual usage yet.
    JsRuntimeHandle runtime;
    JsErrorCode err = JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime);

    JsDebugProtocolHandler protocolHandler;
    err = JsDebugProtocolHandlerCreate(runtime, &protocolHandler);

    JsDebugService service;
    err = JsDebugServiceCreate(&service);
    err = JsDebugServiceListen(service, 9229);
    err = JsDebugServiceRegisterHandler(service, "runtime1", protocolHandler, /*breakOnNextLine*/ true);

    printf("Listening on ws://127.0.0.1:9229/runtime1");
    getchar();

    err = JsDebugServiceUnregisterHandler(service, "runtime1");
    err = JsDebugServiceClose(service);
    err = JsDebugServiceDestroy(service);
    service = nullptr;

    err = JsDebugProtocolHandlerDestroy(protocolHandler);
    protocolHandler = nullptr;

    err = JsDisposeRuntime(runtime);
    runtime = nullptr;

    return 0;
}
