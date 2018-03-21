//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "DebugClient.h"

#include <ChakraCore.h>

namespace JsDebug
{
    typedef void(CHAKRA_CALLBACK *ProtocolHandlerSendResponseCallback)(const char *response);

    class ProtocolHandler
    {
    public:
        ProtocolHandler(JsRuntimeHandle runtime);
        ~ProtocolHandler();

        void Connect(bool breakOnNextLine, ProtocolHandlerSendResponseCallback callback, void* state);
        void Disconnect();

        void SendCommand(const char* command);

    private:
        JsRuntimeHandle m_runtime;
        DebugClient m_client;
    };
}
