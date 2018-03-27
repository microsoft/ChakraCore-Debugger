//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Debugger.h"

namespace JsDebug
{
    Debugger::Debugger(JsRuntimeHandle runtime)
        : m_runtime(runtime)
        , m_messageCallback(nullptr)
        , m_messageCallbackState(nullptr)
        , m_debugCallback(nullptr)
        , m_debugCallbackState(nullptr)
        , m_enabled(false)
        , m_pauseOnNextStatement(false)
    {
        IfJsErrorThrow(JsDiagStartDebugging(m_runtime, &Debugger::DebugEventCallback, this), "failed to start debugging");
    }

    Debugger::~Debugger()
    {
        try
        {
            IfJsErrorThrow(JsDiagStopDebugging(m_runtime, nullptr), "failed to stop debugging");
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void Debugger::Enable()
    {
        if (m_enabled)
        {
            return;
        }

        m_enabled = true;

        // TODO: Other initialization
    }

    void Debugger::Disable()
    {
        if (!m_enabled)
        {
            return;
        }

        m_enabled = false;
        ClearBreakpoints();
    }

    void Debugger::RequestAsyncBreak()
    {
        IfJsErrorThrow(JsDiagRequestAsyncBreak(m_runtime), "failed to request async break");
    }

    void Debugger::SetMessageHandler(DebuggerMessageHandler callback, void* callbackState)
    {
        m_messageCallback = callback;
        m_messageCallbackState = callbackState;
    }

    void Debugger::SetDebugEventHandler(JsDiagDebugEventCallback callback, void* callbackState)
    {
        m_debugCallback = callback;
        m_debugCallbackState = callbackState;
    }

    void Debugger::DebugEventCallback(JsDiagDebugEvent debugEvent, JsValueRef eventData, void* callbackState)
    {
        auto protocolHandler = static_cast<Debugger*>(callbackState);
        protocolHandler->HandleDebugEvent(debugEvent, eventData);
    }

    void Debugger::HandleDebugEvent(JsDiagDebugEvent debugEvent, JsValueRef eventData)
    {
        if (m_messageCallback != nullptr)
        {
            m_messageCallback(m_messageCallbackState);
        }

        if (!m_enabled)
        {
            return;
        }

        if (m_debugCallback != nullptr)
        {
            m_debugCallback(debugEvent, eventData, m_debugCallbackState);
        }

        switch (debugEvent) {
        case JsDiagDebugEventSourceCompile:
        case JsDiagDebugEventCompileError:
            HandleSourceEvent(eventData, debugEvent == JsDiagDebugEventSourceCompile);
            break;

        case JsDiagDebugEventBreakpoint:
        case JsDiagDebugEventStepComplete:
        case JsDiagDebugEventDebuggerStatement:
        case JsDiagDebugEventRuntimeException:
            HandleBreak(eventData);
            break;

        case JsDiagDebugEventAsyncBreak:
            if (m_pauseOnNextStatement)
            {
                m_pauseOnNextStatement = false;
                HandleBreak(eventData);
            }

            break;
        }
    }

    void Debugger::HandleSourceEvent(JsValueRef eventData, bool success)
    {

    }

    void Debugger::HandleBreak(JsValueRef eventData)
    {

    }

    void Debugger::ClearBreakpoints()
    {
    }
}
