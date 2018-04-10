//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerBreak.h"

#include "DebuggerCallFrame.h"
#include "PropertyHelpers.h"
#include "ProtocolHelpers.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Debugger::CallFrame;
    using protocol::DictionaryValue;
    using protocol::Maybe;
    using protocol::Runtime::RemoteObject;
    using protocol::Runtime::StackTrace;
    using protocol::String;

    DebuggerBreak::DebuggerBreak(JsValueRef breakInfo)
        : m_breakInfo(breakInfo)
    {
    }

    String DebuggerBreak::GetReason() const
    {
        JsValueRef exception = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(m_breakInfo.Get(), PropertyHelpers::Names::Exception, &exception))
        {
            return protocol::Debugger::Paused::ReasonEnum::Exception;
        }

        return protocol::Debugger::Paused::ReasonEnum::Other;
    }

    Maybe<DictionaryValue> DebuggerBreak::GetData() const
    {
        std::unique_ptr<RemoteObject> exception = GetException();
        if (exception != nullptr)
        {
            std::unique_ptr<DictionaryValue> data = exception->toValue();

            bool isUncaught = false;
            PropertyHelpers::TryGetProperty(m_breakInfo.Get(), PropertyHelpers::Names::Uncaught, &isUncaught);
            data->setBoolean(PropertyHelpers::Names::Uncaught, isUncaught);

            return std::move(data);
        }

        return Maybe<DictionaryValue>();
    }

    Maybe<Array<String>> DebuggerBreak::GetHitBreakpoints() const
    {
        auto breakpointIds = Array<String>::create();

        int breakpointId = 0;
        if (PropertyHelpers::TryGetProperty(m_breakInfo.Get(), PropertyHelpers::Names::BreakpointId, &breakpointId))
        {
            breakpointIds->addItem(String16::fromInteger(breakpointId));
        }

        return Maybe<Array<String>>(std::move(breakpointIds));
    }

    Maybe<StackTrace> DebuggerBreak::GetAsyncStackTrace() const
    {
        return Maybe<StackTrace>();
    }

    std::unique_ptr<RemoteObject> DebuggerBreak::GetException() const
    {
        JsValueRef exceptionProperty = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(m_breakInfo.Get(), PropertyHelpers::Names::Exception, &exceptionProperty))
        {
            return ProtocolHelpers::WrapException(exceptionProperty);
        }

        return nullptr;
    }
}
