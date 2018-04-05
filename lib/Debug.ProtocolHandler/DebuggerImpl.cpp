//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerImpl.h"

#include "ProtocolHandler.h"
#include "Debugger.h"
#include "DebuggerScript.h"

#include <StringUtil.h>

namespace JsDebug
{
    using protocol::Maybe;
    using protocol::Response;
    using protocol::String;

    DebuggerImpl::DebuggerImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel, Debugger* debugger)
        : m_handler(handler)
        , m_frontend(frontendChannel)
        , m_debugger(debugger)
        , m_isEnabled(false)
        , m_shouldSkipAllPauses(false)
    {
    }

    DebuggerImpl::~DebuggerImpl()
    {
        disable();
    }

    Response DebuggerImpl::enable()
    {
        if (m_isEnabled)
        {
            return Response::OK();
        }

        m_isEnabled = true;
        m_debugger->Enable();
        m_debugger->SetSourceEventHandler(&DebuggerImpl::SourceEventHandler, this);
        m_debugger->SetBreakEventHandler(&DebuggerImpl::BreakEventHandler, this);

        std::vector<DebuggerScript> scripts = m_debugger->GetScripts();
        for (const auto& script : scripts)
        {
            HandleSourceEvent(script, true);
        }

        return Response::OK();
    }

    Response DebuggerImpl::disable()
    {
        if (!m_isEnabled) {
            return Response::OK();
        }

        m_isEnabled = false;
        m_debugger->Disable();
        m_debugger->SetSourceEventHandler(nullptr, nullptr);

        return Response();
    }

    Response DebuggerImpl::setBreakpointsActive(bool in_active)
    {
        return Response();
    }

    Response DebuggerImpl::setSkipAllPauses(bool in_skip)
    {
        return Response();
    }

    Response DebuggerImpl::setBreakpointByUrl(
        int in_lineNumber,
        Maybe<String> in_url,
        Maybe<String> in_urlRegex,
        Maybe<int> in_columnNumber,
        Maybe<String> in_condition,
        String  *out_breakpointId,
        std::unique_ptr<protocol::Array<protocol::Debugger::Location>>* out_locations)
    {
        return Response();
    }

    Response DebuggerImpl::setBreakpoint(
        std::unique_ptr<protocol::Debugger::Location> in_location,
        Maybe<String> in_condition,
        String  *out_breakpointId,
        std::unique_ptr<protocol::Debugger::Location>* out_actualLocation)
    {
        return Response();
    }

    Response DebuggerImpl::removeBreakpoint(const String & in_breakpointId)
    {
        return Response();
    }

    Response DebuggerImpl::continueToLocation(std::unique_ptr<protocol::Debugger::Location> in_location)
    {
        return Response();
    }

    Response DebuggerImpl::stepOver()
    {
        return Response();
    }

    Response DebuggerImpl::stepInto()
    {
        return Response();
    }

    Response DebuggerImpl::stepOut()
    {
        return Response();
    }

    Response DebuggerImpl::pause()
    {
        return Response();
    }

    Response DebuggerImpl::resume()
    {
        m_debugger->Continue();
        return Response::OK();
    }

    Response DebuggerImpl::searchInContent(
        const String & in_scriptId,
        const String & in_query,
        Maybe<bool> in_caseSensitive,
        Maybe<bool> in_isRegex,
        std::unique_ptr<protocol::Array<protocol::Debugger::SearchMatch>>* out_result)
    {
        return Response();
    }

    Response DebuggerImpl::setScriptSource(
        const String & in_scriptId,
        const String & in_scriptSource, Maybe<bool> in_dryRun,
        Maybe<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
        Maybe<bool>* out_stackChanged,
        Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace,
        Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails)
    {
        return Response();
    }

    Response DebuggerImpl::restartFrame(
        const String & in_callFrameId,
        std::unique_ptr<protocol::Array<protocol::Debugger::CallFrame>>* out_callFrames,
        Maybe<protocol::Runtime::StackTrace>* out_asyncStackTrace)
    {
        return Response();
    }

    Response DebuggerImpl::getScriptSource(const String & in_scriptId, String  *out_scriptSource)
    {
        return Response();
    }

    Response DebuggerImpl::setPauseOnExceptions(const String & in_state)
    {
        return Response();
    }

    Response DebuggerImpl::evaluateOnCallFrame(
        const String & in_callFrameId,
        const String & in_expression,
        Maybe<String> in_objectGroup,
        Maybe<bool> in_includeCommandLineAPI,
        Maybe<bool> in_silent,
        Maybe<bool> in_returnByValue,
        Maybe<bool> in_generatePreview,
        std::unique_ptr<protocol::Runtime::RemoteObject>* out_result,
        Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails)
    {
        return Response();
    }

    Response DebuggerImpl::setVariableValue(
        int in_scopeNumber,
        const String & in_variableName,
        std::unique_ptr<protocol::Runtime::CallArgument> in_newValue,
        const String & in_callFrameId)
    {
        return Response();
    }

    Response DebuggerImpl::setAsyncCallStackDepth(int in_maxDepth)
    {
        return Response();
    }

    Response DebuggerImpl::setBlackboxPatterns(std::unique_ptr<protocol::Array<String>> in_patterns)
    {
        return Response();
    }

    Response DebuggerImpl::setBlackboxedRanges(
        const String & in_scriptId,
        std::unique_ptr<protocol::Array<protocol::Debugger::ScriptPosition>> in_positions)
    {
        return Response();
    }

    void DebuggerImpl::SourceEventHandler(const DebuggerScript& script, bool success, void* callbackState)
    {
        const auto debuggerImpl = static_cast<DebuggerImpl*>(callbackState);
        debuggerImpl->HandleSourceEvent(script, success);
    }

    SkipPauseRequest DebuggerImpl::BreakEventHandler(const DebuggerBreak& breakInfo, void* callbackState)
    {
        const auto debuggerImpl = static_cast<DebuggerImpl*>(callbackState);
        return debuggerImpl->HandleBreakEvent(breakInfo);
    }

    void DebuggerImpl::HandleSourceEvent(const DebuggerScript& script, bool success)
    {
        String16 scriptId = script.ScriptId();
        String16 scriptUrl = script.SourceUrl();

        std::unique_ptr<protocol::DictionaryValue> executionContextAuxData;
        if (!script.ExecutionContextAuxData().empty())
        {
            executionContextAuxData = protocol::DictionaryValue::cast(
                protocol::StringUtil::parseJSON(script.ExecutionContextAuxData()));
        }

        if (success)
        {
            m_frontend.scriptParsed(
                scriptId,
                scriptUrl,
                script.StartLine(),
                script.StartColumn(),
                script.EndLine(),
                script.EndColumn(),
                script.ExecutionContextId(),
                script.Hash(),
                std::move(executionContextAuxData),
                script.IsLiveEdit(),
                script.SourceMappingUrl(),
                script.HasSourceUrl());
        }
        else
        {
            m_frontend.scriptFailedToParse(
                scriptId,
                scriptUrl,
                script.StartLine(),
                script.StartColumn(),
                script.EndLine(),
                script.EndColumn(),
                script.ExecutionContextId(),
                script.Hash(),
                std::move(executionContextAuxData),
                script.SourceMappingUrl(),
                script.HasSourceUrl());
        }

        m_scriptMap.emplace(scriptId, script);

        // TODO: Process pending breakpoints
    }

    SkipPauseRequest DebuggerImpl::HandleBreakEvent(const DebuggerBreak& breakInfo)
    {
        SkipPauseRequest request = SkipPauseRequest::RequestNoSkip;

        if (m_shouldSkipAllPauses)
        {
            request = SkipPauseRequest::RequestContinue;
        }

        if (request != SkipPauseRequest::RequestNoSkip)
        {
            return request;
        }

        auto callFrames = protocol::Array<protocol::Debugger::CallFrame>::create();

        for (const DebuggerCallFrame& callFrame : m_debugger->GetCallFrames())
        {
            callFrames->addItem(callFrame.ToProtocolValue());
        }

        m_frontend.paused(
            std::move(callFrames),
            breakInfo.GetReason(),
            breakInfo.GetData(),
            breakInfo.GetHitBreakpoints(),
            breakInfo.GetAsyncStackTrace());

        return request;
    }
}