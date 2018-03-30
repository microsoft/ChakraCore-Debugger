//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "Debugger.h"

#include "ErrorHelpers.h"
#include "PropertyHelpers.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    Debugger::Debugger(ProtocolHandler* handler, JsRuntimeHandle runtime)
        : m_handler(handler)
        , m_runtime(runtime)
        , m_isEnabled(false)
        , m_isPaused(false)
        , m_isRunningNestedMessageLoop(false)
        , m_shouldPauseOnNextStatement(false)
        , m_sourceEventCallback(nullptr)
        , m_sourceEventCallbackState(nullptr)
        , m_breakEventCallback(nullptr)
        , m_breakEventCallbackState(nullptr)
    {
        IfJsErrorThrow(JsDiagStartDebugging(m_runtime, &Debugger::DebugEventCallback, this));
    }

    Debugger::~Debugger()
    {
        try
        {
            IfJsErrorThrow(JsDiagStopDebugging(m_runtime, nullptr));
        }
        catch (const std::exception& e)
        {
            std::cerr << e.what() << std::endl;
        }
    }

    void Debugger::Enable()
    {
        if (m_isEnabled)
        {
            return;
        }

        m_isEnabled = true;

        // TODO: Other initialization
    }

    void Debugger::Disable()
    {
        if (!m_isEnabled)
        {
            return;
        }

        m_isEnabled = false;
        ClearBreakpoints();
    }

    void Debugger::SetSourceEventHandler(DebuggerSourceEventHandler callback, void* callbackState)
    {
        m_sourceEventCallback = callback;
        m_sourceEventCallbackState = callbackState;
    }

    void Debugger::SetBreakEventHandler(DebuggerBreakEventHandler callback, void* callbackState)
    {
        m_breakEventCallback = callback;
        m_breakEventCallbackState = callbackState;
    }

    void Debugger::Continue()
    {
        m_handler->RunIfWaitingForDebugger();
    }

    bool Debugger::IsPaused()
    {
        return m_isPaused;
    }

    void Debugger::RequestAsyncBreak()
    {
        IfJsErrorThrow(JsDiagRequestAsyncBreak(m_runtime));
    }

    std::vector<DebuggerScript> Debugger::GetScripts()
    {
        std::vector<DebuggerScript> scripts;

        JsValueRef scriptsArray = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetScripts(&scriptsArray));

        int length = 0;
        PropertyHelpers::GetProperty(scriptsArray, "length", &length);

        for (int i = 0; i < length; i++)
        {
            JsValueRef scriptValue = PropertyHelpers::GetIndexedProperty(scriptsArray, i);

            scripts.emplace_back(scriptValue);
        }

        return scripts;
    }

    std::vector<DebuggerCallFrame> Debugger::GetCallFrames(int limit)
    {
        JsValueRef stackTrace = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackTrace(&stackTrace));

        int length = 0;
        PropertyHelpers::GetProperty(stackTrace, "length", &length);

        if (limit > 0 && limit < length) {
            length = limit;
        }

        std::vector<DebuggerCallFrame> callFrames;

        for (int i = 0; i < length; ++i) {
            JsValueRef callFrameValue = PropertyHelpers::GetIndexedProperty(stackTrace, i);

            callFrames.emplace_back(callFrameValue);
        }

        return callFrames;
    }

    void Debugger::DebugEventCallback(JsDiagDebugEvent debugEvent, JsValueRef eventData, void* callbackState)
    {
        auto protocolHandler = static_cast<Debugger*>(callbackState);
        protocolHandler->HandleDebugEvent(debugEvent, eventData);
    }

    void Debugger::HandleDebugEvent(JsDiagDebugEvent debugEvent, JsValueRef eventData)
    {
        m_handler->ProcessCommandQueue(false);

        if (!m_isEnabled)
        {
            return;
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
            if (m_shouldPauseOnNextStatement)
            {
                m_shouldPauseOnNextStatement = false;
                HandleBreak(eventData);
            }

            break;
        }
    }

    void Debugger::HandleSourceEvent(JsValueRef eventData, bool success)
    {
        if (m_sourceEventCallback != nullptr)
        {
            DebuggerScript scriptInfo(eventData);
            m_sourceEventCallback(scriptInfo, success, m_sourceEventCallbackState);
        }
    }

    void Debugger::HandleBreak(JsValueRef eventData)
    {
        if (m_isRunningNestedMessageLoop)
        {
            // Don't allow reentrancy
            return;
        }

        if (m_breakEventCallback != nullptr)
        {
            m_isPaused = true;

            DebuggerBreak breakInfo(eventData);
            SkipPauseRequest request = m_breakEventCallback(breakInfo, m_breakEventCallbackState);

            if (request == SkipPauseRequest::RequestNoSkip)
            {
                m_isRunningNestedMessageLoop = true;
                m_handler->WaitForDebugger();
            }

            m_isPaused = false;
            
            if (request == SkipPauseRequest::RequestStepFrame ||
                request == SkipPauseRequest::RequestStepInto)
            {
                JsDiagSetStepType(JsDiagStepTypeStepIn);
            }
            else if (request == SkipPauseRequest::RequestStepOut)
            {
                JsDiagSetStepType(JsDiagStepTypeStepOut);
            }
        }
    }

    void Debugger::ClearBreakpoints()
    {
    }
}
