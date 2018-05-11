//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Schema::Domain;
    using protocol::Serializable;

    namespace
    {
        const char c_ErrorHandlerAlreadyConnected[] = "Handler is already connected";
    }

    ProtocolHandler::ProtocolHandler(JsRuntimeHandle runtime)
        : m_callback(nullptr)
        , m_callbackState(nullptr)
        , m_isConnected(false)
        , m_waitingForDebugger(false)
        , m_breakOnNextLine(false)
        , m_dispatcher(this)
    {
        m_debugger = std::make_unique<Debugger>(this, runtime);
    }

    ProtocolHandler::~ProtocolHandler()
    {
    }

    void ProtocolHandler::Connect(
        bool breakOnNextLine,
        ProtocolHandlerSendResponseCallback callback,
        void* callbackState)
    {
        {
            std::unique_lock<std::mutex> lock(m_lock);

            if (m_callback != nullptr)
            {
                throw std::runtime_error(c_ErrorHandlerAlreadyConnected);
            }

            m_callback = callback;
            m_callbackState = callbackState;
            m_breakOnNextLine = breakOnNextLine;

            EnqueueCommand(CommandType::Connect);
        }

        m_debugger->RequestAsyncBreak();
    }

    void ProtocolHandler::Disconnect()
    {
        {
            std::unique_lock<std::mutex> lock(m_lock);

            m_callback = nullptr;
            m_callbackState = nullptr;
            m_breakOnNextLine = false;

            EnqueueCommand(CommandType::Disconnect);
        }

        m_debugger->RequestAsyncBreak();
    }

    void ProtocolHandler::SendCommand(const char* command)
    {
#ifdef _DEBUG
        OutputDebugStringA("{\"type\":\"request\",\"payload\":");
        OutputDebugStringA(command);
        OutputDebugStringA("},\r\n");
#endif

        {
            std::unique_lock<std::mutex> lock(m_lock);
            EnqueueCommand(CommandType::MessageReceived, command);
        }

        m_debugger->RequestAsyncBreak();
    }

    void ProtocolHandler::WaitForDebugger()
    {
        m_waitingForDebugger = true;
        ProcessCommandQueue();
    }

    void ProtocolHandler::RunIfWaitingForDebugger()
    {
        m_waitingForDebugger = false;
    }

    std::unique_ptr<Array<Domain>> ProtocolHandler::GetSupportedDomains()
    {
        auto domains = Array<Domain>::create();

        domains->addItem(Domain::create()
            .setName(protocol::Console::Metainfo::domainName)
            .setVersion(protocol::Console::Metainfo::version)
            .build());

        domains->addItem(Domain::create()
            .setName(protocol::Debugger::Metainfo::domainName)
            .setVersion(protocol::Debugger::Metainfo::version)
            .build());

        domains->addItem(Domain::create()
            .setName(protocol::Runtime::Metainfo::domainName)
            .setVersion(protocol::Runtime::Metainfo::version)
            .build());

        return domains;
    }

    void ProtocolHandler::sendProtocolResponse(int callId, std::unique_ptr<Serializable> message)
    {
        sendProtocolNotification(std::move(message));
    }

    void ProtocolHandler::sendProtocolNotification(std::unique_ptr<Serializable> message)
    {
        protocol::String str = message->serialize();
        std::string asciiStr = str.toAscii();
        const char* response = asciiStr.c_str();

#ifdef _DEBUG
        OutputDebugStringA("{\"type\":\"response\",\"payload\":");
        OutputDebugStringA(response);
        OutputDebugStringA("},\r\n");
#endif

        SendResponse(response);
    }

    void ProtocolHandler::flushProtocolNotifications()
    {
    }

    void ProtocolHandler::ProcessCommandQueue()
    {
        std::vector<std::pair<CommandType, std::string>> current;

        do
        {
            current.clear();

            {
                std::unique_lock<std::mutex> lock(m_lock);

                if (m_waitingForDebugger && m_commandQueue.empty())
                {
                    m_commandWaiting.wait(lock);
                }

                std::swap(m_commandQueue, current);
            }

            for (const auto& command : current)
            {
                switch (command.first)
                {
                case CommandType::Connect:
                    HandleConnect();
                    break;

                case CommandType::Disconnect:
                    HandleDisconnect();
                    break;

                case CommandType::MessageReceived:
                    HandleMessageReceived(command.second);
                    break;

                default:
                    throw std::runtime_error("Unknown command type");
                }
            }
        } while (m_waitingForDebugger || !current.empty());
    }

    void ProtocolHandler::EnqueueCommand(ProtocolHandler::CommandType type, std::string message)
    {
        m_commandQueue.emplace_back(type, message);
        m_commandWaiting.notify_all();
    }

    void ProtocolHandler::SendResponse(const char* response)
    {
        if (m_callback != nullptr)
        {
            m_callback(response, m_callbackState);
        }
    }

    void ProtocolHandler::HandleConnect()
    {
        if (m_isConnected)
        {
            throw std::runtime_error("Already connected");
        }

        m_consoleAgent = std::make_unique<ConsoleImpl>(this, this);
        protocol::Console::Dispatcher::wire(&m_dispatcher, m_consoleAgent.get());

        m_debuggerAgent = std::make_unique<DebuggerImpl>(this, this, m_debugger.get());
        protocol::Debugger::Dispatcher::wire(&m_dispatcher, m_debuggerAgent.get());

        m_runtimeAgent = std::make_unique<RuntimeImpl>(this, this, m_debugger.get());
        protocol::Runtime::Dispatcher::wire(&m_dispatcher, m_runtimeAgent.get());

        m_schemaAgent = std::make_unique<SchemaImpl>(this, this);
        protocol::Schema::Dispatcher::wire(&m_dispatcher, m_schemaAgent.get());

        if (m_breakOnNextLine)
        {
            m_debugger->PauseOnNextStatement();
        }

        m_isConnected = true;
    }

    void ProtocolHandler::HandleDisconnect()
    {
        if (!m_isConnected)
        {
            throw std::runtime_error("Not currently connected");
        }

        m_consoleAgent.reset();
        m_debuggerAgent.reset();
        m_runtimeAgent.reset();
        m_schemaAgent.reset();

        RunIfWaitingForDebugger();
        m_isConnected = false;
    }

    void ProtocolHandler::HandleMessageReceived(const std::string& message)
    {
        m_dispatcher.dispatch(protocol::parseJSONCharacters(
            reinterpret_cast<const uint8_t*>(message.c_str()),
            static_cast<unsigned int>(message.length())));
    }
}
