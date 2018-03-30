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
    {
    }

    int DebuggerCallFrame::SourceId() const
    {
        int scriptId = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "scriptId", &scriptId);

        return scriptId;
    }

    int DebuggerCallFrame::Line() const
    {
        int line = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "line", &line);

        return line;
    }

    int DebuggerCallFrame::Column() const
    {
        int column = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "column", &column);

        return column;
    }

    int DebuggerCallFrame::ContextId() const
    {
        return 1;
    }

    bool DebuggerCallFrame::IsAtReturn() const
    {
        int index = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &index);

        JsValueRef properties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(index, &properties));

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
        int callFrameIndex = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex);

        return "{\"ordinal\":" + String::fromInteger(callFrameIndex) + "}";
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetFunctionLocation() const
    {
        int functionHandle = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle);

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        return ProtocolHelpers::WrapLocation(funcObj);
    }

    String DebuggerCallFrame::GetFunctionName() const
    {
        int functionHandle = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "functionHandle", &functionHandle);

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        String functionName;
        PropertyHelpers::GetProperty(funcObj, "name", &functionName);

        return functionName;
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetLocation() const
    {
        return ProtocolHelpers::WrapLocation(m_callFrameInfo.Get());
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetReturnValue() const
    {
        int callFrameIndex = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex);

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(callFrameIndex, &stackProperties));

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
        int callFrameIndex = 0;
        PropertyHelpers::GetProperty(m_callFrameInfo.Get(), "index", &callFrameIndex);

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(callFrameIndex, &stackProperties));

        JsValueRef thisObject = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(stackProperties, "thisObject", &thisObject))
        {
            return ProtocolHelpers::WrapObject(thisObject);
        }

        // The protocol requires a "this" member, so create an undefined object to return.
        return ProtocolHelpers::GetUndefinedObject();
    }
}
