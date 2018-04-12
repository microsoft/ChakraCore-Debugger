//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "Debugger.h"

#include "protocol\Forward.h"
#include "protocol\Protocol.h"

#include "ConsoleImpl.h"
#include "DebuggerImpl.h"
#include "RuntimeImpl.h"
#include "SchemaImpl.h"

#include <ChakraCore.h>

#include <mutex>
#include <string>
#include <vector>

namespace JsDebug
{
    typedef void(CHAKRA_CALLBACK* ProtocolHandlerSendResponseCallback)(const char* response, void* callbackState);

    class ProtocolHandler : public protocol::FrontendChannel
    {
    public:
        ProtocolHandler(JsRuntimeHandle runtime);
        ~ProtocolHandler() override;
        ProtocolHandler(const ProtocolHandler&) = delete;
        ProtocolHandler& operator=(const ProtocolHandler&) = delete;

        void Connect(bool breakOnNextLine, ProtocolHandlerSendResponseCallback callback, void* callbackState);
        void Disconnect();

        void SendCommand(const char* command);
        void ProcessCommandQueue();
        void WaitForDebugger();
        void RunIfWaitingForDebugger();

        std::unique_ptr<protocol::Array<protocol::Schema::Domain>> GetSupportedDomains();

        // protocol::FrontendChannel implementation
        void sendProtocolResponse(int callId, std::unique_ptr<protocol::Serializable> message) override;
        void sendProtocolNotification(std::unique_ptr<protocol::Serializable> message) override;
        void flushProtocolNotifications() override;

    private:
        enum class CommandType
        {
            None,
            Connect,
            Disconnect,
            MessageReceived,
        };

        void SendResponse(const char* response);
        void EnqueueCommand(CommandType type, std::string message = "");
        void HandleConnect();
        void HandleDisconnect();
        void HandleMessageReceived(const std::string& message);

        std::unique_ptr<Debugger> m_debugger;
        ProtocolHandlerSendResponseCallback m_callback;
        void* m_callbackState;

        std::mutex m_lock;
        std::condition_variable m_commandWaiting;
        std::vector<std::pair<CommandType, std::string>> m_commandQueue;
        bool m_isConnected;
        bool m_waitingForDebugger;
        bool m_breakOnNextLine;

        protocol::UberDispatcher m_dispatcher;
        std::unique_ptr<ConsoleImpl> m_consoleAgent;
        std::unique_ptr<DebuggerImpl> m_debuggerAgent;
        std::unique_ptr<RuntimeImpl> m_runtimeAgent;
        std::unique_ptr<SchemaImpl> m_schemaAgent;
    };
}
