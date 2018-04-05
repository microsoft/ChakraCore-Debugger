//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Forward.h>
#include <protocol\Debugger.h>

#include "Debugger.h"
#include "DebuggerScript.h"
#include "DebuggerCallFrame.h"

#include <ChakraCore.h>

namespace JsDebug
{
    class ProtocolHandler;

    class DebuggerImpl : public protocol::Debugger::Backend
    {
    public:
        DebuggerImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel, Debugger* debugger);
        ~DebuggerImpl() override;
        DebuggerImpl(const DebuggerImpl&) = delete;
        DebuggerImpl& operator=(const DebuggerImpl&) = delete;

        // protocol::Debugger::Backend implementation
        protocol::Response enable() override;
        protocol::Response disable() override;
        protocol::Response setBreakpointsActive(bool in_active) override;
        protocol::Response setSkipAllPauses(bool in_skip) override;
        protocol::Response setBreakpointByUrl(
            int in_lineNumber,
            protocol::Maybe<protocol::String> in_url,
            protocol::Maybe<protocol::String> in_urlRegex,
            protocol::Maybe<int> in_columnNumber,
            protocol::Maybe<protocol::String> in_condition,
            protocol::String* out_breakpointId,
            std::unique_ptr<protocol::Array<protocol::Debugger::Location>>* out_locations) override;
        protocol::Response setBreakpoint(
            std::unique_ptr<protocol::Debugger::Location> in_location,
            protocol::Maybe<protocol::String> in_condition,
            protocol::String* out_breakpointId,
            std::unique_ptr<protocol::Debugger::Location>* out_actualLocation) override;
        protocol::Response removeBreakpoint(const protocol::String& in_breakpointId) override;
        protocol::Response continueToLocation(std::unique_ptr<protocol::Debugger::Location> in_location) override;
        protocol::Response stepOver() override;
        protocol::Response stepInto() override;
        protocol::Response stepOut() override;
        protocol::Response pause() override;
        protocol::Response resume() override;
        protocol::Response searchInContent(
            const protocol::String& in_scriptId,
            const protocol::String& in_query,
            protocol::Maybe<bool> in_caseSensitive,
            protocol::Maybe<bool> in_isRegex,
            std::unique_ptr<protocol::Array<protocol::Debugger::SearchMatch>>* out_result) override;
        protocol::Response setScriptSource(
            const protocol::String& in_scriptId,
            const protocol::String& in_scriptSource,
            protocol::Maybe<bool> in_dryRun,
            protocol::Maybe<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
            protocol::Maybe<bool>* out_stackChanged,
            protocol::Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        protocol::Response restartFrame(
            const protocol::String& in_callFrameId,
            std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
            protocol::Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace) override;
        protocol::Response getScriptSource(
            const protocol::String& in_scriptId,
            protocol::String* out_scriptSource) override;
        protocol::Response setPauseOnExceptions(const protocol::String& in_state) override;
        protocol::Response evaluateOnCallFrame(
            const protocol::String& in_callFrameId,
            const protocol::String& in_expression,
            protocol::Maybe<protocol::String> in_objectGroup,
            protocol::Maybe<bool> in_includeCommandLineAPI,
            protocol::Maybe<bool> in_silent,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            std::unique_ptr<protocol::Runtime::RemoteObject>* out_result,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        protocol::Response setVariableValue(
            int in_scopeNumber,
            const protocol::String& in_variableName,
            std::unique_ptr<protocol::Runtime::CallArgument> in_newValue,
            const protocol::String& in_callFrameId) override;
        protocol::Response setAsyncCallStackDepth(int in_maxDepth) override;
        protocol::Response setBlackboxPatterns(
            std::unique_ptr<protocol::Array<protocol::String>> in_patterns) override;
        protocol::Response setBlackboxedRanges(
            const protocol::String& in_scriptId,
            std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>> in_positions) override;

    private:
        static void SourceEventHandler(const DebuggerScript& script, bool success, void* callbackState);
        static SkipPauseRequest BreakEventHandler(const DebuggerBreak& breakInfo, void* callbackState);
        void HandleSourceEvent(const DebuggerScript& script, bool success);
        SkipPauseRequest HandleBreakEvent(const DebuggerBreak& breakInfo);


        ProtocolHandler* m_handler;
        protocol::Debugger::Frontend m_frontend;
        Debugger* m_debugger;

        bool m_isEnabled;
        bool m_shouldSkipAllPauses;

        protocol::HashMap<protocol::String, DebuggerScript> m_scriptMap;
        std::vector<DebuggerCallFrame> m_callFrames;
    };
}
