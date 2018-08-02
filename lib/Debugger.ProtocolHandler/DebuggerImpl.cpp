// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DebuggerImpl.h"

#include "Debugger.h"
#include "DebuggerCallFrame.h"
#include "DebuggerScript.h"
#include "PropertyHelpers.h"
#include "ProtocolHandler.h"
#include "ProtocolHelpers.h"

#include <StringUtil.h>

namespace JsDebug
{
    using protocol::Array;
    using protocol::Debugger::BreakLocation;
    using protocol::Debugger::CallFrame;
    using protocol::Debugger::Location;
    using protocol::Debugger::ScriptPosition;
    using protocol::Debugger::SearchMatch;
    using protocol::FrontendChannel;
    using protocol::Maybe;
    using protocol::Response;
    using protocol::Runtime::CallArgument;
    using protocol::Runtime::ExceptionDetails;
    using protocol::Runtime::RemoteObject;
    using protocol::Runtime::StackTrace;
    using protocol::Runtime::StackTraceId;
    using protocol::String;
    using protocol::StringUtil;

    namespace
    {
        const char c_ErrorBreakpointCouldNotResolve[] = "Breakpoint could not be resolved";
        const char c_ErrorBreakpointExists[] = "Breakpoint at specified location already exists";
        const char c_ErrorBreakpointNotFound[] = "Breakpoint could not be found";
        const char c_ErrorCallFrameInvalidId[] = "Invalid call frame ID specified";
        const char c_ErrorInvalidColumnNumber[] = "Invalid column number specified";
        const char c_ErrorNotEnabled[] = "Debugger is not enabled";
        const char c_ErrorNotImplemented[] = "Debugger method not implemented";
        const char c_ErrorScriptMustBeLoaded[] = "Script must be loaded before resolving";
        const char c_ErrorUrlRequired[] = "Either url or urlRegex must be specified";
    }

    DebuggerImpl::DebuggerImpl(ProtocolHandler* handler, FrontendChannel* frontendChannel, Debugger* debugger)
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

    Response DebuggerImpl::enable(String* /*out_debuggerId*/)
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

        m_breakpointMap.clear();
        m_scriptMap.clear();
        m_shouldSkipAllPauses = false;

        return Response::OK();
    }

    Response DebuggerImpl::setBreakpointsActive(bool /*in_active*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setSkipAllPauses(bool /*in_skip*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setBreakpointByUrl(
        int in_lineNumber,
        Maybe<String> in_url,
        Maybe<String> in_urlRegex,
        Maybe<String> /*in_scriptHash*/,
        Maybe<int> in_columnNumber,
        Maybe<String> in_condition,
        String  *out_breakpointId,
        std::unique_ptr<Array<Location>>* out_locations)
    {
        String url;
        DebuggerBreakpoint::QueryType type;

        if (in_url.isJust())
        {
            url = in_url.fromJust();
            type = DebuggerBreakpoint::QueryType::Url;
        }
        else if (in_urlRegex.isJust())
        {
            url = in_urlRegex.fromJust();
            type = DebuggerBreakpoint::QueryType::UrlRegex;
        }
        else
        {
            return Response::Error(c_ErrorUrlRequired);
        }

        int columnNumber = in_columnNumber.fromMaybe(0);
        if (columnNumber < 0)
        {
            return Response::Error(c_ErrorInvalidColumnNumber);
        }

        String condition = in_condition.fromMaybe("");

        DebuggerBreakpoint breakpoint(
            m_debugger,
            url,
            type,
            in_lineNumber,
            columnNumber,
            condition);

        String breakpointId = breakpoint.GenerateKey();

        auto result = m_breakpointMap.find(breakpointId);
        if (result != m_breakpointMap.end())
        {
            return Response::Error(c_ErrorBreakpointExists);
        }

        auto locations = Array<Location>::create();

        try
        {
            for (const auto& script : m_scriptMap)
            {
                if (breakpoint.TryLoadScript(script.second))
                {
                    if (TryResolveBreakpoint(breakpoint))
                    {
                        locations->addItem(breakpoint.GetActualLocation());
                    }
                }
            }
        }
        catch (const JsErrorException& e)
        {
            return Response::Error(e.what());
        }

        m_breakpointMap.emplace(breakpointId, breakpoint);

        *out_breakpointId = breakpointId;
        *out_locations = std::move(locations);
        return Response::OK();
    }

    Response DebuggerImpl::setBreakpoint(
        std::unique_ptr<Location> in_location,
        Maybe<String> in_condition,
        String  *out_breakpointId,
        std::unique_ptr<Location>* out_actualLocation)
    {
        DebuggerBreakpoint breakpoint = DebuggerBreakpoint::FromLocation(
            m_debugger,
            in_location.get(),
            in_condition.fromMaybe(""));

        String breakpointId = breakpoint.GenerateKey();

        auto result = m_breakpointMap.find(breakpointId);
        if (result != m_breakpointMap.end())
        {
            return Response::Error(c_ErrorBreakpointExists);
        }

        if (TryResolveBreakpoint(breakpoint))
        {
            *out_breakpointId = breakpointId;
            *out_actualLocation = breakpoint.GetActualLocation();

            m_breakpointMap.emplace(breakpointId, breakpoint);
            return Response::OK();
        }

        return Response::Error(c_ErrorBreakpointCouldNotResolve);
    }

    Response DebuggerImpl::removeBreakpoint(const String & in_breakpointId)
    {
        auto result = m_breakpointMap.find(in_breakpointId);
        if (result != m_breakpointMap.end())
        {
            m_debugger->RemoveBreakpoint(result->second);
            m_breakpointMap.erase(in_breakpointId);
            return Response::OK();
        }

        return Response::Error(c_ErrorBreakpointNotFound);
    }

    Response DebuggerImpl::continueToLocation(
        std::unique_ptr<Location> /*in_location*/,
        Maybe<String> /*in_targetCallFrames*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::stepOver()
    {
        m_debugger->StepOver();
        return Response::OK();
    }

    Response DebuggerImpl::stepInto(Maybe<bool> /*in_breakOnAsyncCall*/)
    {
        m_debugger->StepIn();
        return Response::OK();
    }

    Response DebuggerImpl::stepOut()
    {
        m_debugger->StepOut();
        return Response::OK();
    }

    Response DebuggerImpl::pause()
    {
        m_debugger->PauseOnNextStatement();
        return Response::OK();
    }

    Response DebuggerImpl::resume()
    {
        if (!IsEnabled())
        {
            return Response::Error(c_ErrorNotEnabled);
        }

        m_debugger->Continue();
        return Response::OK();
    }

    Response DebuggerImpl::searchInContent(
        const String & /*in_scriptId*/,
        const String & /*in_query*/,
        Maybe<bool> /*in_caseSensitive*/,
        Maybe<bool> /*in_isRegex*/,
        std::unique_ptr<Array<SearchMatch>>* /*out_result*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setScriptSource(
        const String & /*in_scriptId*/,
        const String & /*in_scriptSource*/,
        Maybe<bool> /*in_dryRun*/,
        Maybe<Array<CallFrame>>* /*out_callFrames*/,
        Maybe<bool>* /*out_stackChanged*/,
        Maybe<StackTrace>* /*out_asyncStackTrace*/,
        Maybe<StackTraceId>* /*out_asyncStackTraceId*/,
        Maybe<ExceptionDetails>* /*out_exceptionDetails*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::restartFrame(
        const String & /*in_callFrameId*/,
        std::unique_ptr<Array<CallFrame>>* /*out_callFrames*/,
        Maybe<StackTrace>* /*out_asyncStackTrace*/,
        Maybe<StackTraceId>* /*out_asyncStackTraceId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::getScriptSource(const String & in_scriptId, String  *out_scriptSource)
    {
        if (!IsEnabled())
        {
            return Response::Error(c_ErrorNotEnabled);
        }

        auto result = m_scriptMap.find(in_scriptId);
        if (result == m_scriptMap.end())
        {
            return Response::Error("Script not found: " + in_scriptId);
        }

        *out_scriptSource = result->second.Source();
        return Response::OK();
    }

    Response DebuggerImpl::setPauseOnExceptions(const String & in_state)
    {
        JsDiagBreakOnExceptionAttributes attributes = JsDiagBreakOnExceptionAttributeNone;

        if (in_state == "none")
        {
            attributes = JsDiagBreakOnExceptionAttributeNone;
        }
        else if (in_state == "all")
        {
            attributes = JsDiagBreakOnExceptionAttributeFirstChance;
        }
        else if (in_state == "uncaught")
        {
            attributes = JsDiagBreakOnExceptionAttributeUncaught;
        }
        else
        {
            return Response::Error("Unrecognized state value: " + in_state);
        }

        m_debugger->SetBreakOnException(attributes);
        return Response::OK();
    }

    Response DebuggerImpl::evaluateOnCallFrame(
        const String & in_callFrameId,
        const String & in_expression,
        Maybe<String> in_objectGroup,
        Maybe<bool> /*in_includeCommandLineAPI*/,
        Maybe<bool> /*in_silent*/,
        Maybe<bool> in_returnByValue,
        Maybe<bool> /*in_generatePreview*/,
        Maybe<bool> /*in_throwOnSideEffect*/,
        Maybe<double> /*in_timeout*/,
        std::unique_ptr<RemoteObject>* out_result,
        Maybe<ExceptionDetails>* out_exceptionDetails)
    {
        auto parsedId = ProtocolHelpers::ParseObjectId(in_callFrameId);

        int ordinal = 0;
        if (parsedId->getInteger(PropertyHelpers::Names::Ordinal, &ordinal))
        {
            auto callFrame = m_debugger->GetCallFrame(ordinal);

            std::unique_ptr<ExceptionDetails> exceptionDetails;
            *out_result = callFrame.Evaluate(in_expression, in_returnByValue.fromMaybe(false), &exceptionDetails);

            if (exceptionDetails != nullptr)
            {
                *out_exceptionDetails = std::move(exceptionDetails);
            }

            return Response::OK();
        }

        return Response::Error(c_ErrorCallFrameInvalidId);
    }

    Response DebuggerImpl::setVariableValue(
        int /*in_scopeNumber*/,
        const String & /*in_variableName*/,
        std::unique_ptr<CallArgument> /*in_newValue*/,
        const String & /*in_callFrameId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setAsyncCallStackDepth(int /*in_maxDepth*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setBlackboxPatterns(std::unique_ptr<Array<String>> in_patterns)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setBlackboxedRanges(
        const String & /*in_scriptId*/,
        std::unique_ptr<Array<ScriptPosition>> /*in_positions*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }
    
    Response DebuggerImpl::getPossibleBreakpoints(
        std::unique_ptr<Location> /*in_start*/,
        Maybe<Location> /*in_end*/,
        Maybe<bool> /*in_restrictToFunction*/,
        std::unique_ptr<Array<BreakLocation>>* /*out_locations*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::getStackTrace(
        std::unique_ptr<StackTraceId> /*in_stackTraceId*/,
        std::unique_ptr<StackTrace>* /*out_stackTrace*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::pauseOnAsyncCall(std::unique_ptr<StackTraceId> /*in_parentStackTraceId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    void DebuggerImpl::scheduleStepIntoAsync(std::unique_ptr<ScheduleStepIntoAsyncCallback> callback)
    {
        callback->sendFailure(Response::Error(c_ErrorNotImplemented));
    }

    Response DebuggerImpl::setReturnValue(std::unique_ptr<CallArgument> in_newValue)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response DebuggerImpl::setBreakpointOnFunctionCall(
        const String& /*in_objectId*/,
        Maybe<String> /*in_condition*/,
        String* /*out_breakpointId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
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

    bool DebuggerImpl::IsEnabled()
    {
        return m_isEnabled;
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

        for (auto& breakpoint : m_breakpointMap)
        {
            if (breakpoint.second.TryLoadScript(script))
            {
                if (TryResolveBreakpoint(breakpoint.second))
                {
                    m_frontend.breakpointResolved(
                        breakpoint.first,
                        breakpoint.second.GetActualLocation());
                }
            }
        }
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

        auto callFrames = Array<CallFrame>::create();

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

    bool DebuggerImpl::TryResolveBreakpoint(DebuggerBreakpoint& breakpoint)
    {
        if (!breakpoint.IsScriptLoaded())
        {
            throw std::runtime_error(c_ErrorScriptMustBeLoaded);
        }

        m_debugger->SetBreakpoint(breakpoint);

        if (!breakpoint.IsResolved())
        {
            return false;
        }

        return true;
    }
}