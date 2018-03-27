//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Forward.h>
#include <protocol\Debugger.h>

#include "Debugger.h"

namespace JsDebug
{
    using protocol::Maybe;
    using protocol::Response;
    using String = String16;

    class ProtocolHandler;

    class DebuggerImpl : public protocol::Debugger::Backend
    {
    public:
        DebuggerImpl(ProtocolHandler* handler, Debugger* debugger);
        ~DebuggerImpl() override;

        // protocol::Debugger::Backend implementation
        Response enable() override;
        Response disable() override;
        Response setBreakpointsActive(bool in_active) override;
        Response setSkipAllPauses(bool in_skip) override;
        Response setBreakpointByUrl(
            int in_lineNumber,
            Maybe<String> in_url,
            Maybe<String> in_urlRegex,
            Maybe<int> in_columnNumber,
            Maybe<String> in_condition,
            String* out_breakpointId,
            std::unique_ptr<protocol::Array<protocol::Debugger::Location>>* out_locations) override;
        Response setBreakpoint(
            std::unique_ptr<protocol::Debugger::Location> in_location,
            Maybe<String> in_condition,
            String* out_breakpointId,
            std::unique_ptr<protocol::Debugger::Location>* out_actualLocation) override;
        Response removeBreakpoint(const String& in_breakpointId) override;
        Response continueToLocation(std::unique_ptr<protocol::Debugger::Location> in_location) override;
        Response stepOver() override;
        Response stepInto() override;
        Response stepOut() override;
        Response pause() override;
        Response resume() override;
        Response searchInContent(
            const String& in_scriptId,
            const String& in_query,
            Maybe<bool> in_caseSensitive,
            Maybe<bool> in_isRegex,
            std::unique_ptr<protocol::Array<protocol::Debugger::SearchMatch>>* out_result) override;
        Response setScriptSource(
            const String& in_scriptId,
            const String& in_scriptSource,
            Maybe<bool> in_dryRun,
            Maybe<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
            Maybe<bool>* out_stackChanged,
            Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace,
            Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        Response restartFrame(
            const String& in_callFrameId,
            std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
            Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace) override;
        Response getScriptSource(const String& in_scriptId, String* out_scriptSource) override;
        Response setPauseOnExceptions(const String& in_state) override;
        Response evaluateOnCallFrame(
            const String& in_callFrameId,
            const String& in_expression,
            Maybe<String> in_objectGroup,
            Maybe<bool> in_includeCommandLineAPI,
            Maybe<bool> in_silent,
            Maybe<bool> in_returnByValue,
            Maybe<bool> in_generatePreview,
            std::unique_ptr<protocol::Runtime::RemoteObject>* out_result,
            Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        Response setVariableValue(
            int in_scopeNumber,
            const String& in_variableName,
            std::unique_ptr<protocol::Runtime::CallArgument> in_newValue,
            const String& in_callFrameId) override;
        Response setAsyncCallStackDepth(int in_maxDepth) override;
        Response setBlackboxPatterns(std::unique_ptr<protocol::Array<String>> in_patterns) override;
        Response setBlackboxedRanges(
            const String& in_scriptId,
            std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>> in_positions) override;

    private:
        ProtocolHandler* m_handler;
        Debugger* m_debugger;
        bool m_enabled;
    };
}
