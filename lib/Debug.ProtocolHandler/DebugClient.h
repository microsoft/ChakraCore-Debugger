//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "protocol\Forward.h"
#include "protocol\Protocol.h"

#include "ConsoleImpl.h"
#include "DebuggerImpl.h"
#include "RuntimeImpl.h"
#include "SchemaImpl.h"

namespace JsDebug
{
    using protocol::Serializable;

    class DebugClient : public protocol::FrontendChannel
    {
    public:
        DebugClient();
        ~DebugClient() override;

        void SendCommand(const char* command);

        // protocol::FrontendChannel implementation
        void sendProtocolResponse(int callId, std::unique_ptr<Serializable> message) override;
        void sendProtocolNotification(std::unique_ptr<Serializable> message) override;
        void flushProtocolNotifications() override;

    private:
        protocol::UberDispatcher m_dispatcher;
        std::unique_ptr<ConsoleImpl> m_consoleAgent;
        std::unique_ptr<DebuggerImpl> m_debuggerAgent;
        std::unique_ptr<RuntimeImpl> m_runtimeAgent;
        std::unique_ptr<SchemaImpl> m_schemaAgent;
    };
}
