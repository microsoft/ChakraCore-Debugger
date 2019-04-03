// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ChakraDebugProtocolHandler.h"
#include "ProtocolHandler.h"
#include "ConsoleHandler.h"
#include <cassert>

namespace JsDebug
{
    JsValueRef ConsoleAPIRedirection(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state,
        protocol::String apiType)
    {
        assert(callee);
        assert(!isConstructCall);
        assert(argumentCount > 0);
        if (argumentCount > 0)
        {
            ConsoleHandler *handler = reinterpret_cast<ConsoleHandler*>(state);
            handler->GetProtocolHandler()->ConsoleAPICalled(apiType, arguments, argumentCount);
        }

        // Returning undefined for the console api result.
        JsValueRef undefinedValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetUndefinedValue(&undefinedValue));
        return undefinedValue;

        UNREFERENCED_PARAMETER(callee);
        UNREFERENCED_PARAMETER(isConstructCall);
    }

    JsValueRef CALLBACK ConsoleLog(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Log);
    }

    JsValueRef CALLBACK ConsoleDebug(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Debug);
    }

    JsValueRef CALLBACK ConsoleInfo(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Info);
    }

    JsValueRef CALLBACK ConsoleError(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Error);
    }

    JsValueRef CALLBACK ConsoleWarning(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Warning);
    }

    JsValueRef CALLBACK ConsoleClear(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Clear);
    }

    JsValueRef CALLBACK ConsoleAssert(
        JsValueRef callee,
        bool isConstructCall,
        JsValueRef* arguments,
        unsigned short argumentCount,
        void* state)
    {
        return ConsoleAPIRedirection(callee, isConstructCall, arguments, argumentCount, state, protocol::Runtime::ConsoleAPICalled::TypeEnum::Assert);
    }

    JsErrorCode ConsoleHandler::AddFunctionCallback(
        JsValueRef object,
        const wchar_t* callbackName,
        JsNativeFunction callback,
        void* callbackState)
    {
        // Get property ID.
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetPropertyIdFromName(callbackName, &propertyId));

        JsValueRef function = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateFunction(callback, callbackState, &function));

        return JsSetProperty(object, propertyId, function, true);
    }

    JsValueRef ConsoleHandler::CreateConsoleObject()
    {
        // Create the console object
        JsValueRef consoleObject = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateObject(&consoleObject));

        // Now create the callbacks that this console object will expose
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"log", ConsoleLog, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"debug", ConsoleDebug, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"info", ConsoleInfo, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"error", ConsoleError, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"warn", ConsoleWarning, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"clear", ConsoleClear, this));
        IfJsErrorThrow(AddFunctionCallback(consoleObject, L"assert", ConsoleAssert, this));

        return consoleObject;
    }
}