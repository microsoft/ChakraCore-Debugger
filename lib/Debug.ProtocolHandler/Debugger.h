//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ChakraCore.h>

namespace JsDebug
{
    typedef void (*DebuggerMessageHandler)(void* callbackState);

    class Debugger
    {
    public:
        Debugger(JsRuntimeHandle runtime);
        ~Debugger();

        void Enable();
        void Disable();

        void RequestAsyncBreak();
        void SetMessageHandler(DebuggerMessageHandler callback, void* callbackState);
        void SetDebugEventHandler(JsDiagDebugEventCallback callback, void* callbackState);

    private:
        static void CHAKRA_CALLBACK DebugEventCallback(
            JsDiagDebugEvent debugEvent,
            JsValueRef eventData,
            void* callbackState);

        void HandleDebugEvent(JsDiagDebugEvent debugEvent, JsValueRef eventData);
        void HandleSourceEvent(JsValueRef eventData, bool success);
        void HandleBreak(JsValueRef eventData);

        void ClearBreakpoints();

        JsRuntimeHandle m_runtime;
        DebuggerMessageHandler m_messageCallback;
        void* m_messageCallbackState;
        JsDiagDebugEventCallback m_debugCallback;
        void* m_debugCallbackState;
        bool m_enabled;
        bool m_pauseOnNextStatement;
    };
}
