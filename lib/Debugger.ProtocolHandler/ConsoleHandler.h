// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ChakraCore.h>

namespace JsDebug
{
    class ProtocolHandler;
    class ConsoleHandler
    {
    public:
        ConsoleHandler(ProtocolHandler *protocolHandler)
            : m_protocolHandler(protocolHandler)
        {}

        JsValueRef CreateConsoleObject();
        ProtocolHandler * GetProtocolHandler() const
        {
            return m_protocolHandler;
        }

    private:
        JsErrorCode AddFunctionCallback(JsValueRef object,
            const wchar_t* callbackName,
            JsNativeFunction callback,
            void* callbackState);
            
        ProtocolHandler *m_protocolHandler;
    };
}
