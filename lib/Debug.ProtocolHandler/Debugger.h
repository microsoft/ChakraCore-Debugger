//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "DebuggerBreak.h"
#include "DebuggerBreakpoint.h"
#include "DebuggerCallFrame.h"
#include "DebuggerScript.h"

#include <ChakraCore.h>
#include <vector>

namespace JsDebug
{
    class ProtocolHandler;

    enum SkipPauseRequest {
        RequestNoSkip,
        RequestContinue,
        RequestStepInto,
        RequestStepOut,
        RequestStepFrame
    };

    typedef void(*DebuggerSourceEventHandler)(const DebuggerScript& scriptInfo, bool success, void* callbackState);
    typedef SkipPauseRequest(*DebuggerBreakEventHandler)(const DebuggerBreak& breakInfo, void* callbackState);

    class Debugger
    {
    public:
        Debugger(ProtocolHandler* handler, JsRuntimeHandle runtime);
        ~Debugger();
        Debugger(const Debugger&) = delete;
        Debugger& operator=(const Debugger&) = delete;

        void Enable();
        void Disable();

        void SetSourceEventHandler(DebuggerSourceEventHandler callback, void* callbackState);
        void SetBreakEventHandler(DebuggerBreakEventHandler callback, void* callbackState);

        void RequestAsyncBreak();
        void PauseOnNextStatement();

        std::vector<DebuggerScript> GetScripts();
        std::vector<DebuggerCallFrame> GetCallFrames(int limit = 0);

        void SetBreakpoint(DebuggerBreakpoint& breakpoint);
        void RemoveBreakpoint(DebuggerBreakpoint& breakpoint);

        bool IsPaused();
        void Continue();
        void StepIn();
        void StepOut();
        void StepOver();

    private:
        static void CHAKRA_CALLBACK DebugEventCallback(
            JsDiagDebugEvent debugEvent,
            JsValueRef eventData,
            void* callbackState);

        void HandleDebugEvent(JsDiagDebugEvent debugEvent, JsValueRef eventData);
        void HandleSourceEvent(JsValueRef eventData, bool success);
        void HandleBreak(JsValueRef eventData);

        void ClearBreakpoints();

        ProtocolHandler* m_handler;
        JsRuntimeHandle m_runtime;

        bool m_isEnabled;
        bool m_isPaused;
        bool m_isRunningNestedMessageLoop;
        bool m_shouldPauseOnNextStatement;

        DebuggerSourceEventHandler m_sourceEventCallback;
        void* m_sourceEventCallbackState;

        DebuggerBreakEventHandler m_breakEventCallback;
        void* m_breakEventCallbackState;
    };
}
