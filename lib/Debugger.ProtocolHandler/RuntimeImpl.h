// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "Debugger.h"

#include <protocol\Runtime.h>
#include <protocol\Forward.h>

namespace JsDebug
{
    class ProtocolHandler;

    class RuntimeImpl : public protocol::Runtime::Backend
    {
    public:
        RuntimeImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel, Debugger* debugger);
        ~RuntimeImpl() override;
        RuntimeImpl(const RuntimeImpl&) = delete;
        RuntimeImpl& operator=(const RuntimeImpl&) = delete;

        // protocol::Runtime::Backend implementation
        void awaitPromise(
            const protocol::String& in_promiseObjectId,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            std::unique_ptr<AwaitPromiseCallback> callback) override;
        void callFunctionOn(
            const protocol::String& in_functionDeclaration,
            protocol::Maybe<protocol::String> in_objectId,
            protocol::Maybe<protocol::Array<protocol::Runtime::CallArgument>> in_arguments,
            protocol::Maybe<bool> in_silent,
            protocol::Maybe<bool> in_returnByValue,
            protocol::Maybe<bool> in_generatePreview,
            protocol::Maybe<bool> in_userGesture,
            protocol::Maybe<bool> in_awaitPromise,
            protocol::Maybe<int> in_executionContextId,
            protocol::Maybe<protocol::String> in_objectGroup,
            std::unique_ptr<CallFunctionOnCallback> callback) override;
        protocol::Response compileScript(
            const protocol::String& in_expression,
            const protocol::String& in_sourceURL,
            bool in_persistScript,
            protocol::Maybe<int> in_executionContextId,
            protocol::Maybe<protocol::String>* out_scriptId,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        protocol::Response disable() override;
        protocol::Response discardConsoleEntries() override;
        protocol::Response enable() override;
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
            protocol::Maybe<bool> in_throwOnSideEffect,
            protocol::Maybe<double> in_timeout,
            std::unique_ptr<EvaluateCallback> callback) override;
        protocol::Response getIsolateId(protocol::String* out_id) override;
        protocol::Response getHeapUsage(double* out_usedSize, double* out_totalSize) override;
        protocol::Response getProperties(
            const protocol::String& in_objectId,
            protocol::Maybe<bool> in_ownProperties,
            protocol::Maybe<bool> in_accessorPropertiesOnly,
            protocol::Maybe<bool> in_generatePreview,
            std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>* out_result,
            protocol::Maybe<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>* out_internalProperties,
            protocol::Maybe<protocol::Runtime::ExceptionDetails>* out_exceptionDetails) override;
        protocol::Response globalLexicalScopeNames(
            protocol::Maybe<int> in_executionContextId,
            std::unique_ptr<protocol::Array<protocol::String>>* out_names) override;
        protocol::Response queryObjects(
            const protocol::String& in_prototypeObjectId,
            protocol::Maybe<protocol::String> in_objectGroup,
            std::unique_ptr<protocol::Runtime::RemoteObject>* out_objects) override;
        protocol::Response releaseObject(const protocol::String& in_objectId) override;
        protocol::Response releaseObjectGroup(const protocol::String& in_objectGroup) override;
        protocol::Response runIfWaitingForDebugger() override;
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
        protocol::Response setCustomObjectFormatterEnabled(bool in_enabled) override;
        protocol::Response setMaxCallStackSizeToCapture(int in_size) override;
        protocol::Response terminateExecution() override;
        protocol::Response addBinding(
            const protocol::String& in_name,
            protocol::Maybe<int> in_executionContextId) override;
        protocol::Response removeBinding(const protocol::String& in_name) override;

    private:
        bool IsEnabled();

        ProtocolHandler* m_handler;
        protocol::Runtime::Frontend m_frontend;
        Debugger* m_debugger;
        bool m_isEnabled;
    };
}
