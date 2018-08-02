// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "RuntimeImpl.h"

#include "PropertyHelpers.h"
#include "ProtocolHandler.h"
#include "ProtocolHelpers.h"

#include <StringUtil.h>

namespace JsDebug
{
    using protocol::Array;
    using protocol::DictionaryValue;
    using protocol::FrontendChannel;
    using protocol::Maybe;
    using protocol::Response;
    using protocol::Runtime::CallArgument;
    using protocol::Runtime::ExceptionDetails;
    using protocol::Runtime::InternalPropertyDescriptor;
    using protocol::Runtime::PropertyDescriptor;
    using protocol::Runtime::RemoteObject;
    using protocol::String;
    using protocol::StringUtil;
    using protocol::Value;

    namespace
    {
        const char c_ErrorInvalidObjectId[] = "Invalid object ID";
        const char c_ErrorNotEnabled[] = "Runtime is not enabled";
        const char c_ErrorNotImplemented[] = "Not implemented";
    }

    RuntimeImpl::RuntimeImpl(ProtocolHandler* handler, FrontendChannel* frontendChannel, Debugger* debugger)
        : m_handler(handler)
        , m_frontend(frontendChannel)
        , m_debugger(debugger)
        , m_isEnabled(false)
    {
    }

    RuntimeImpl::~RuntimeImpl()
    {
    }

    void RuntimeImpl::evaluate(
        const String& /*in_expression*/,
        Maybe<String> /*in_objectGroup*/,
        Maybe<bool> /*in_includeCommandLineAPI*/,
        Maybe<bool> /*in_silent*/,
        Maybe<int> /*in_contextId*/,
        Maybe<bool> /*in_returnByValue*/,
        Maybe<bool> /*in_generatePreview*/,
        Maybe<bool> /*in_userGesture*/,
        Maybe<bool> /*in_awaitPromise*/,
        Maybe<bool> /*in_throwOnSideEffect*/,
        Maybe<double> /*in_timeout*/,
        std::unique_ptr<EvaluateCallback> callback)
    {
        callback->sendFailure(Response::Error(c_ErrorNotImplemented));
    }

    void RuntimeImpl::awaitPromise(
        const String& /*in_promiseObjectId*/,
        Maybe<bool> /*in_returnByValue*/,
        Maybe<bool> /*in_generatePreview*/,
        std::unique_ptr<AwaitPromiseCallback> callback)
    {
        callback->sendFailure(Response::Error(c_ErrorNotImplemented));
    }

    void RuntimeImpl::callFunctionOn(
        const String& /*in_functionDeclaration*/,
        Maybe<String> /*in_objectId*/,
        Maybe<Array<CallArgument>> /*in_arguments*/,
        Maybe<bool> /*in_silent*/,
        Maybe<bool> /*in_returnByValue*/,
        Maybe<bool> /*in_generatePreview*/,
        Maybe<bool> /*in_userGesture*/,
        Maybe<bool> /*in_awaitPromise*/,
        Maybe<int> /*in_executionContextId*/,
        Maybe<String> /*in_objectGroup*/,
        std::unique_ptr<CallFunctionOnCallback> callback)
    {
        callback->sendFailure(Response::Error(c_ErrorNotImplemented));
    }

    Response RuntimeImpl::getProperties(
        const String& in_objectId,
        Maybe<bool> /*in_ownProperties*/,
        Maybe<bool> in_accessorPropertiesOnly,
        Maybe<bool> /*in_generatePreview*/,
        std::unique_ptr<Array<PropertyDescriptor>>* out_result,
        Maybe<Array<InternalPropertyDescriptor>>* out_internalProperties,
        Maybe<ExceptionDetails>* /*out_exceptionDetails*/)
    {
        if (in_accessorPropertiesOnly.fromMaybe(false))
        {
            // We don't support accessorPropertiesOnly queries, just return an empty list.
            *out_result = Array<PropertyDescriptor>::create();
            return Response::OK();
        }

        auto parsedId = ProtocolHelpers::ParseObjectId(in_objectId);

        int handle = 0;
        int ordinal = 0;
        String name;

        if (parsedId->getInteger(PropertyHelpers::Names::Handle, &handle))
        {
            DebuggerObject obj = m_debugger->GetObjectFromHandle(handle);
            *out_result = obj.GetPropertyDescriptors();
            *out_internalProperties = obj.GetInternalPropertyDescriptors();

            return Response::OK();
        }
        else if (parsedId->getInteger(PropertyHelpers::Names::Ordinal, &ordinal) &&
                 parsedId->getString(PropertyHelpers::Names::Name, &name))
        {
            DebuggerCallFrame callFrame = m_debugger->GetCallFrame(ordinal);

            if (name == PropertyHelpers::Names::Locals)
            {
                DebuggerLocalScope obj = callFrame.GetLocals();
                *out_result = obj.GetPropertyDescriptors();
                *out_internalProperties = obj.GetInternalPropertyDescriptors();

                return Response::OK();
            }
            else if (name == PropertyHelpers::Names::Globals)
            {
                DebuggerObject obj = callFrame.GetGlobals();
                *out_result = obj.GetPropertyDescriptors();
                *out_internalProperties = obj.GetInternalPropertyDescriptors();

                return Response::OK();
            }
        }

        return Response::Error(c_ErrorInvalidObjectId);
    }

    Response RuntimeImpl::releaseObject(const String& /*in_objectId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::releaseObjectGroup(const String& /*in_objectGroup*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::runIfWaitingForDebugger()
    {
        if (!IsEnabled())
        {
            return Response::Error(c_ErrorNotEnabled);
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
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::setCustomObjectFormatterEnabled(bool /*in_enabled*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::compileScript(
        const String& /*in_expression*/,
        const String& /*in_sourceURL*/,
        bool /*in_persistScript*/,
        Maybe<int> /*in_executionContextId*/,
        Maybe<String>* /*out_scriptId*/,
        Maybe<ExceptionDetails>* /*out_exceptionDetails*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    void RuntimeImpl::runScript(
        const String& /*in_scriptId*/,
        Maybe<int> /*in_executionContextId*/,
        Maybe<String> /*in_objectGroup*/,
        Maybe<bool> /*in_silent*/,
        Maybe<bool> /*in_includeCommandLineAPI*/,
        Maybe<bool> /*in_returnByValue*/,
        Maybe<bool> /*in_generatePreview*/,
        Maybe<bool> /*in_awaitPromise*/,
        std::unique_ptr<RunScriptCallback> callback)
    {
        callback->sendFailure(Response::Error(c_ErrorNotImplemented));
    }

    Response RuntimeImpl::getIsolateId(String* /*out_id*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::getHeapUsage(double* /*out_usedSize*/, double* /*out_totalSize*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::globalLexicalScopeNames(
        Maybe<int> /*in_executionContextId*/,
        std::unique_ptr<Array<String>>* /*out_names*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::queryObjects(
        const String& /*in_prototypeObjectId*/,
        Maybe<String> /*in_objectGroup*/,
        std::unique_ptr<RemoteObject>* /*out_objects*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::terminateExecution()
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    Response RuntimeImpl::setMaxCallStackSizeToCapture(int /*in_size*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    protocol::Response RuntimeImpl::addBinding(const String& /*in_name*/, Maybe<int> /*in_executionContextId*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    protocol::Response RuntimeImpl::removeBinding(const String& /*in_name*/)
    {
        return Response::Error(c_ErrorNotImplemented);
    }

    bool RuntimeImpl::IsEnabled()
    {
        return m_isEnabled;
    }
}