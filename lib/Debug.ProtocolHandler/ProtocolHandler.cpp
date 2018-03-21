//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    ProtocolHandler::ProtocolHandler(JsRuntimeHandle runtime)
        : m_runtime(runtime)
    {
    }

    ProtocolHandler::~ProtocolHandler()
    {
    }

    void ProtocolHandler::Connect(bool breakOnNextLine, ProtocolHandlerSendResponseCallback callback, void* state)
    {
    }

    void ProtocolHandler::Disconnect()
    {
    }

    void ProtocolHandler::SendCommand(const char* command)
    {
        m_client.SendCommand(command);
    }
}
