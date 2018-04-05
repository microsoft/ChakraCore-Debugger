//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerCallFrame.h"

#include "ErrorHelpers.h"
#include "PropertyHelpers.h"
#include "ProtocolHelpers.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Debugger::CallFrame;
    using protocol::Debugger::Location;
    using protocol::Debugger::Scope;
    using protocol::Runtime::RemoteObject;
    using protocol::String;

    DebuggerCallFrame::DebuggerCallFrame(JsValueRef callFrameInfo)
        : m_callFrameInfo(callFrameInfo)
        , m_callFrameIndex(PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "index"))
    {
    }

    int DebuggerCallFrame::SourceId() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "scriptId");
    }

    int DebuggerCallFrame::Line() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "line");
    }

    int DebuggerCallFrame::Column() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "column");
    }

    int DebuggerCallFrame::ContextId() const
    {
        return 1;
    }

    bool DebuggerCallFrame::IsAtReturn() const
    {
        JsValueRef properties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &properties));

        JsValueRef propVal = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(m_callFrameInfo.Get(), "returnValue", &propVal))
        {
            return PropertyHelpers::HasProperty(propVal, "handle");
        }

        return false;
    }

    std::unique_ptr<CallFrame> DebuggerCallFrame::ToProtocolValue() const
    {
        return CallFrame::create()
            .setCallFrameId(GetCallFrameId())
            .setFunctionLocation(GetFunctionLocation())
            .setFunctionName(GetFunctionName())
            .setLocation(GetLocation())
            .setReturnValue(GetReturnValue())
            .setScopeChain(GetScopeChain())
            .setThis(GetThis())
            .build();
    }

    String DebuggerCallFrame::GetCallFrameId() const
    {
        return "{\"ordinal\":" + String::fromInteger(m_callFrameIndex) + "}";
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetFunctionLocation() const
    {
        int functionHandle = PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "functionHandle");

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        return ProtocolHelpers::WrapLocation(funcObj);
    }

    String DebuggerCallFrame::GetFunctionName() const
    {
        int functionHandle = PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), "functionHandle");

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        return PropertyHelpers::GetPropertyString(funcObj, "name");
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetLocation() const
    {
        return ProtocolHelpers::WrapLocation(m_callFrameInfo.Get());
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetReturnValue() const
    {
        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &stackProperties));

        JsValueRef returnObj = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(stackProperties, "returnValue", &returnObj))
        {
            return ProtocolHelpers::WrapObject(returnObj);
        }

        return nullptr;
    }

    std::unique_ptr<Array<Scope>> DebuggerCallFrame::GetScopeChain() const
    {
        // TODO: Fill this in
        return Array<Scope>::create();
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetThis() const
    {
        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &stackProperties));

        JsValueRef thisObject = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(stackProperties, "thisObject", &thisObject))
        {
            return ProtocolHelpers::WrapObject(thisObject);
        }

        // The protocol requires a "this" member, so create an undefined object to return.
        return ProtocolHelpers::GetUndefinedObject();
    }
}
