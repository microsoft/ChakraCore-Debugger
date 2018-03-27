//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "RuntimeImpl.h"
#include "ProtocolHandler.h"

namespace JsDebug
{
    RuntimeImpl::RuntimeImpl(ProtocolHandler* handler)
        : m_handler(handler)
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
        return Response();
    }

    Response RuntimeImpl::releaseObject(const String & in_objectId)
    {
        return Response();
    }

    Response RuntimeImpl::releaseObjectGroup(const String & in_objectGroup)
    {
        return Response();
    }

    Response RuntimeImpl::runIfWaitingForDebugger()
    {
        m_handler->RunIfWaitingForDebugger();
        return Response();
    }

    Response RuntimeImpl::enable()
    {
        return Response();
    }

    Response RuntimeImpl::disable()
    {
        return Response();
    }

    Response RuntimeImpl::discardConsoleEntries()
    {
        return Response();
    }

    Response RuntimeImpl::setCustomObjectFormatterEnabled(bool in_enabled)
    {
        return Response();
    }

    Response RuntimeImpl::compileScript(
        const String & in_expression,
        const String & in_sourceURL,
        bool in_persistScript,
        Maybe<int> in_executionContextId,
        Maybe<String>* out_scriptId,
        Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails)
    {
        return Response();
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
}