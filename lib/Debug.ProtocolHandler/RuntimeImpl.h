//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Runtime.h>
#include <protocol\Forward.h>

namespace JsDebug
{
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
            const protocol::String& in_expression,
            protocol::Maybe<protocol::String> in_objectGroup,
            protocol::Maybe<bool> in_includeCommandLineAPI,
            protocol::Maybe<bool> in_silent,
            protocol::Maybe<int> in_contextId,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            protocol::Maybe<bool> in_userGesture,
            protocol::Maybe<bool> in_awaitPromise,
            std::unique_ptr<EvaluateCallback> callback) override;
        void awaitPromise(
            const protocol::String& in_promiseObjectId,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            std::unique_ptr<AwaitPromiseCallback> callback) override;
        void callFunctionOn(
            const protocol::String& in_objectId,
            const protocol::String& in_functionDeclaration,
            protocol::Maybe<protocol::Array<protocol::Runtime::CallArgument>> in_arguments,
            protocol::Maybe<bool> in_silent,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            protocol::Maybe<bool> in_userGesture,
            protocol::Maybe<bool> in_awaitPromise,
            std::unique_ptr<CallFunctionOnCallback> callback) override;
        protocol::Response getProperties(
            const protocol::String& in_objectId,
            protocol::Maybe<bool> in_ownProperties,
            protocol::Maybe<bool> in_accessorPropertiesOnly,
            protocol::Maybe<bool> in_generatePreview,
            std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>* out_result,
            protocol::Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>* out_internalProperties,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        protocol::Response releaseObject(const protocol::String& in_objectId) override;
        protocol::Response releaseObjectGroup(const protocol::String& in_objectGroup) override;
        protocol::Response runIfWaitingForDebugger() override;
        protocol::Response enable() override;
        protocol::Response disable() override;
        protocol::Response discardConsoleEntries() override;
        protocol::Response setCustomObjectFormatterEnabled(bool in_enabled) override;
        protocol::Response compileScript(
            const protocol::String& in_expression,
            const protocol::String& in_sourceURL,
            bool in_persistScript,
            protocol::Maybe<int> in_executionContextId,
            protocol::Maybe<protocol::String>* out_scriptId,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        void runScript(
            const protocol::String& in_scriptId,
            protocol::Maybe<int> in_executionContextId,
            protocol::Maybe<protocol::String> in_objectGroup,
            protocol::Maybe<bool> in_silent,
            protocol::Maybe<bool> in_includeCommandLineAPI,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            protocol::Maybe<bool> in_awaitPromise,
            std::unique_ptr<RunScriptCallback> callback) override;

    private:
        bool IsEnabled();

        ProtocolHandler* m_handler;
        protocol::Runtime::Frontend m_frontend;
        bool m_isEnabled;
    };
}
