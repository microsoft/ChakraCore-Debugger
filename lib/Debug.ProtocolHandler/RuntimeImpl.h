//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Runtime.h>
#include <protocol\Forward.h>

namespace JsDebug
{
    using protocol::Maybe;
    using protocol::Response;
    using String = String16;

    class ProtocolHandler;

    class RuntimeImpl : public protocol::Runtime::Backend
    {
    public:
        RuntimeImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel);
        ~RuntimeImpl() override;
        RuntimeImpl(const RuntimeImpl&) = delete;
        RuntimeImpl& operator=(const RuntimeImpl&) = delete;

        // protocol::Runtime::Backend implementation
        void evaluate(
            const String& in_expression,
            Maybe<String> in_objectGroup,
            Maybe<bool> in_includeCommandLineAPI,
            Maybe<bool> in_silent,
            Maybe<int> in_contextId,
            Maybe<bool> in_returnByValue,
            Maybe<bool> in_generatePreview,
            Maybe<bool> in_userGesture,
            Maybe<bool> in_awaitPromise,
            std::unique_ptr<EvaluateCallback> callback) override;
        void awaitPromise(
            const String& in_promiseObjectId,
            Maybe<bool> in_returnByValue,
            Maybe<bool> in_generatePreview,
            std::unique_ptr<AwaitPromiseCallback> callback) override;
        void callFunctionOn(
            const String& in_objectId,
            const String& in_functionDeclaration,
            Maybe<protocol::Array<protocol::Runtime::CallArgument>> in_arguments,
            Maybe<bool> in_silent,
            Maybe<bool> in_returnByValue,
            Maybe<bool> in_generatePreview,
            Maybe<bool> in_userGesture,
            Maybe<bool> in_awaitPromise,
            std::unique_ptr<CallFunctionOnCallback> callback) override;
        Response getProperties(
            const String& in_objectId,
            Maybe<bool> in_ownProperties,
            Maybe<bool> in_accessorPropertiesOnly,
            Maybe<bool> in_generatePreview,
            std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>* out_result,
            Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>* out_internalProperties,
            Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        Response releaseObject(const String& in_objectId) override;
        Response releaseObjectGroup(const String& in_objectGroup) override;
        Response runIfWaitingForDebugger() override;
        Response enable() override;
        Response disable() override;
        Response discardConsoleEntries() override;
        Response setCustomObjectFormatterEnabled(bool in_enabled) override;
        Response compileScript(
            const String& in_expression,
            const String& in_sourceURL,
            bool in_persistScript,
            Maybe<int> in_executionContextId,
            Maybe<String>* out_scriptId,
            Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        void runScript(
            const String& in_scriptId,
            Maybe<int> in_executionContextId,
            Maybe<String> in_objectGroup,
            Maybe<bool> in_silent,
            Maybe<bool> in_includeCommandLineAPI,
            Maybe<bool> in_returnByValue,
            Maybe<bool> in_generatePreview,
            Maybe<bool> in_awaitPromise,
            std::unique_ptr<RunScriptCallback> callback) override;

    private:        
        ProtocolHandler* m_handler;
        protocol::Runtime::Frontend m_frontend;
    };
}
