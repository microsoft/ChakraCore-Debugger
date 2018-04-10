//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    using protocol::Serializable;

    namespace
    {
        const char c_ErrorHandlerAlreadyConnected[] = "Handler is already connected";
    }

    ProtocolHandler::ProtocolHandler(JsRuntimeHandle runtime)
        : m_callback(nullptr)
        , m_callbackState(nullptr)
        , m_waitingForDebugger(false)
        , m_dispatcher(this)
    {
        m_debugger = std::make_unique<Debugger>(this, runtime);

        m_consoleAgent = std::make_unique<ConsoleImpl>(this, this);
        protocol::Console::Dispatcher::wire(&m_dispatcher, m_consoleAgent.get());

        m_debuggerAgent = std::make_unique<DebuggerImpl>(this, this, m_debugger.get());
        protocol::Debugger::Dispatcher::wire(&m_dispatcher, m_debuggerAgent.get());

        m_runtimeAgent = std::make_unique<RuntimeImpl>(this, this, m_debugger.get());
        protocol::Runtime::Dispatcher::wire(&m_dispatcher, m_runtimeAgent.get());

        m_schemaAgent = std::make_unique<SchemaImpl>(this, this);
        protocol::Schema::Dispatcher::wire(&m_dispatcher, m_schemaAgent.get());
    }

    ProtocolHandler::~ProtocolHandler()
    {
    }

    void ProtocolHandler::Connect(
        bool breakOnNextLine,
        ProtocolHandlerSendResponseCallback callback,
        void* callbackState)
    {
        if (m_callback != nullptr)
        {
            throw std::runtime_error(c_ErrorHandlerAlreadyConnected);
        }

        m_callback = callback;
        m_callbackState = callbackState;

        if (breakOnNextLine)
        {
            m_debugger->PauseOnNextStatement();
        }
    }

    void ProtocolHandler::Disconnect()
    {
        m_callback = nullptr;
        m_callbackState = nullptr;
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
            m_commandQueue.emplace_back(command);
            m_commandWaiting.notify_all();
        }

        m_debugger->RequestAsyncBreak();
    }

    void ProtocolHandler::WaitForDebugger()
    {
        m_waitingForDebugger = true;

        while (m_waitingForDebugger)
        {
            ProcessCommandQueue(true);
        }
    }

    void ProtocolHandler::RunIfWaitingForDebugger()
    {
        m_waitingForDebugger = false;
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

    void ProtocolHandler::ProcessCommandQueue(bool waitForCommands)
    {
        std::vector<std::string> current;

        {
            std::unique_lock<std::mutex> lock(m_lock);

            if (waitForCommands && m_commandQueue.empty())
            {
                m_commandWaiting.wait(lock);
            }

            std::swap(m_commandQueue, current);
        }

        for (const auto& command : current)
        {
            m_dispatcher.dispatch(protocol::parseJSONCharacters(
                reinterpret_cast<const uint8_t*>(command.c_str()),
                static_cast<unsigned int>(command.length())));
        }
    }

    void ProtocolHandler::SendResponse(const char* response)
    {
        if (m_callback != nullptr)
        {
            m_callback(response, m_callbackState);
        }
    }
}
