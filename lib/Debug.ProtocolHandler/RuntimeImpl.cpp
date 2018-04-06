//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "RuntimeImpl.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    using protocol::Maybe;
    using protocol::Response;
    using protocol::String;

    namespace
    {
        const char c_RuntimeNotEnabledError[] = "Runtime is not enabled";
    }

    RuntimeImpl::RuntimeImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel)
        : m_handler(handler)
        , m_frontend(frontendChannel)
        , m_isEnabled(false)
    {
    }

    RuntimeImpl::~RuntimeImpl()
    {
    }

    void RuntimeImpl::evaluate(
        const String & in_expression,
        Maybe<String> in_objectGroup,
        Maybe<bool> in_includeCommandLineAPI,
        Maybe<bool> in_silent,
        Maybe<int> in_contextId,
        Maybe<bool> in_returnByValue,
        Maybe<bool> in_generatePreview,
        Maybe<bool> in_userGesture,
        Maybe<bool> in_awaitPromise,
        std::unique_ptr<EvaluateCallback> callback)
    {
    }

    void RuntimeImpl::awaitPromise(
        const String & in_promiseObjectId,
        Maybe<bool> in_returnByValue,
        Maybe<bool> in_generatePreview,
        std::unique_ptr<AwaitPromiseCallback> callback)
    {
    }

    void RuntimeImpl::callFunctionOn(
        const String & in_objectId,
        const String & in_functionDeclaration,
        Maybe<protocol::Array<protocol::Runtime::CallArgument>> in_arguments,
        Maybe<bool> in_silent,
        Maybe<bool> in_returnByValue,
        Maybe<bool> in_generatePreview,
        Maybe<bool> in_userGesture,
        Maybe<bool> in_awaitPromise,
        std::unique_ptr<CallFunctionOnCallback> callback)
    {
    }

    Response RuntimeImpl::getProperties(
        const String & in_objectId,
        Maybe<bool> in_ownProperties,
        Maybe<bool> in_accessorPropertiesOnly,
        Maybe<bool> in_generatePreview,
        std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>* out_result,
        Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>* out_internalProperties,
        Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails)
    {
        return Response::Error("Not implemented");
    }

    Response RuntimeImpl::releaseObject(const String & in_objectId)
    {
        return Response::Error("Not implemented");
    }

    Response RuntimeImpl::releaseObjectGroup(const String & in_objectGroup)
    {
        return Response::Error("Not implemented");
    }

    Response RuntimeImpl::runIfWaitingForDebugger()
    {
        if (!IsEnabled())
        {
            return Response::Error(c_RuntimeNotEnabledError);
        }

        m_handler->RunIfWaitingForDebugger();
        return Response::OK();
    }

    Response RuntimeImpl::enable()
    {
        if (m_isEnabled)
        {
            return Response::OK();
        }

        m_isEnabled = true;
        // TODO: Do other setup

        return Response::OK();
    }

    Response RuntimeImpl::disable()
    {
        if (!m_isEnabled)
        {
            return Response::OK();
        }

        m_isEnabled = false;
        // TODO: Do other cleanup

        return Response::OK();
    }

    Response RuntimeImpl::discardConsoleEntries()
    {
        return Response::Error("Not implemented");
    }

    Response RuntimeImpl::setCustomObjectFormatterEnabled(bool in_enabled)
    {
        return Response::Error("Not implemented");
    }

    Response RuntimeImpl::compileScript(
        const String & in_expression,
        const String & in_sourceURL,
        bool in_persistScript,
        Maybe<int> in_executionContextId,
        Maybe<String>* out_scriptId,
        Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails)
    {
        return Response::Error("Not implemented");
    }

    void RuntimeImpl::runScript(
        const String & in_scriptId,
        Maybe<int> in_executionContextId,
        Maybe<String> in_objectGroup,
        Maybe<bool> in_silent,
        Maybe<bool> in_includeCommandLineAPI,
        Maybe<bool> in_returnByValue,
        Maybe<bool> in_generatePreview,
        Maybe<bool> in_awaitPromise,
        std::unique_ptr<RunScriptCallback> callback)
    {
    }

    bool RuntimeImpl::IsEnabled()
    {
        return m_isEnabled;
    }
}