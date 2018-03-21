//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebugClient.h"
#include "ConsoleImpl.h"
#include "DebuggerImpl.h"
#include "RuntimeImpl.h"
#include "SchemaImpl.h"

namespace JsDebug
{
    DebugClient::DebugClient()
        : m_dispatcher(this)
    {
        m_consoleAgent = std::make_unique<ConsoleImpl>();
        protocol::Console::Dispatcher::wire(&m_dispatcher, m_consoleAgent.get());

        m_debuggerAgent = std::make_unique<DebuggerImpl>();
        protocol::Debugger::Dispatcher::wire(&m_dispatcher, m_debuggerAgent.get());

        m_runtimeAgent = std::make_unique<RuntimeImpl>();
        protocol::Runtime::Dispatcher::wire(&m_dispatcher, m_runtimeAgent.get());

        m_schemaAgent = std::make_unique<SchemaImpl>();
        protocol::Schema::Dispatcher::wire(&m_dispatcher, m_schemaAgent.get());
    }
    
    DebugClient::~DebugClient()
    {
    }

    void DebugClient::SendCommand(const char* command)
    {
    }
    
    void DebugClient::sendProtocolResponse(int callId, std::unique_ptr<Serializable> message)
    {
    }
    
    void DebugClient::sendProtocolNotification(std::unique_ptr<Serializable> message)
    {
    }
    
    void DebugClient::flushProtocolNotifications()
    {
    }
}