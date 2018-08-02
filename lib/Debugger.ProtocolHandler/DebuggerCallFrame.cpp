// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

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
    using protocol::Runtime::ExceptionDetails;
    using protocol::Runtime::RemoteObject;
    using protocol::String;

    DebuggerCallFrame::DebuggerCallFrame(JsValueRef callFrameInfo)
        : m_callFrameInfo(callFrameInfo)
        , m_callFrameIndex(PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), PropertyHelpers::Names::Index))
    {
    }

    int DebuggerCallFrame::SourceId() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), PropertyHelpers::Names::ScriptId);
    }

    int DebuggerCallFrame::Line() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), PropertyHelpers::Names::Line);
    }

    int DebuggerCallFrame::Column() const
    {
        return PropertyHelpers::GetPropertyInt(m_callFrameInfo.Get(), PropertyHelpers::Names::Column);
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
        if (PropertyHelpers::TryGetProperty(m_callFrameInfo.Get(), PropertyHelpers::Names::ReturnValue, &propVal))
        {
            return PropertyHelpers::HasProperty(propVal, PropertyHelpers::Names::Handle);
        }

        return false;
    }

    DebuggerLocalScope DebuggerCallFrame::GetLocals() const
    {
        JsValueRef properties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &properties));

        return DebuggerLocalScope(properties);
    }

    DebuggerObject DebuggerCallFrame::GetGlobals() const
    {
        JsValueRef properties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &properties));

        return DebuggerObject(PropertyHelpers::GetProperty(properties, PropertyHelpers::Names::Globals));
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::Evaluate(
        const String& expression,
        bool returnByValue,
        std::unique_ptr<ExceptionDetails>* exceptionDetails)
    {
        JsValueRef expressionStr = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(expression.characters16(), expression.length(), &expressionStr));

        JsValueRef evalResult = JS_INVALID_REFERENCE;
        JsErrorCode err = JsDiagEvaluate(
            expressionStr,
            m_callFrameIndex,
            JsParseScriptAttributeNone,
            returnByValue,
            &evalResult);

        if (err == JsErrorScriptException)
        {
            if (exceptionDetails != nullptr)
            {
                *exceptionDetails = ProtocolHelpers::WrapExceptionDetails(evalResult);
            }

            return ProtocolHelpers::WrapException(evalResult);
        }

        IfJsErrorThrow(err);
        return ProtocolHelpers::WrapObject(evalResult);
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
            .setUrl("")
            .build();
    }

    String DebuggerCallFrame::GetCallFrameId() const
    {
        return "{\"ordinal\":" + String::fromInteger(m_callFrameIndex) + "}";
    }

    String DebuggerCallFrame::GetObjectIdForFrameProp(const char* propName) const
    {
        return "{\"ordinal\":" + String::fromInteger(m_callFrameIndex) + ",\"name\":\"" + propName + "\"}";
    }

    std::unique_ptr<Location> DebuggerCallFrame::GetFunctionLocation() const
    {
        int functionHandle = PropertyHelpers::GetPropertyInt(
            m_callFrameInfo.Get(),
            PropertyHelpers::Names::FunctionHandle);

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        return ProtocolHelpers::WrapLocation(funcObj);
    }

    String DebuggerCallFrame::GetFunctionName() const
    {
        int functionHandle = PropertyHelpers::GetPropertyInt(
            m_callFrameInfo.Get(),
            PropertyHelpers::Names::FunctionHandle);

        JsValueRef funcObj = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetObjectFromHandle(functionHandle, &funcObj));

        return PropertyHelpers::GetPropertyString(funcObj, PropertyHelpers::Names::Name);
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
        if (PropertyHelpers::TryGetProperty(stackProperties, PropertyHelpers::Names::ReturnValue, &returnObj))
        {
            return ProtocolHelpers::WrapObject(returnObj);
        }

        return nullptr;
    }

    std::unique_ptr<RemoteObject> DebuggerCallFrame::GetThis() const
    {
        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &stackProperties));

        JsValueRef thisObject = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(stackProperties, PropertyHelpers::Names::ThisObject, &thisObject))
        {
            return ProtocolHelpers::WrapObject(thisObject);
        }

        // The protocol requires a "this" member, so create an undefined object to return.
        return ProtocolHelpers::GetUndefinedObject();
    }

    std::unique_ptr<Array<Scope>> DebuggerCallFrame::GetScopeChain() const
    {
        auto scopeChain = Array<Scope>::create();

        JsValueRef stackProperties = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsDiagGetStackProperties(m_callFrameIndex, &stackProperties));

        if (PropertyHelpers::HasProperty(stackProperties, PropertyHelpers::Names::Locals))
        {
            scopeChain->addItem(GetLocalScope());
        }

        JsValueRef scopes = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(stackProperties, PropertyHelpers::Names::Scopes, &scopes))
        {
            int length = PropertyHelpers::GetPropertyInt(scopes, PropertyHelpers::Names::Length);

            for (int index = 0; index < length; index++)
            {
                JsValueRef scopeObj = PropertyHelpers::GetIndexedProperty(scopes, index);
                scopeChain->addItem(GetClosureScope(scopeObj));
            }
        }

        if (PropertyHelpers::HasProperty(stackProperties, PropertyHelpers::Names::Globals))
        {
            scopeChain->addItem(GetGlobalScope());
        }

        return scopeChain;
    }

    std::unique_ptr<Scope> DebuggerCallFrame::GetLocalScope() const
    {
        auto remoteObj = RemoteObject::create()
            .setType("object")
            .setClassName("Object")
            .setDescription("Object")
            .setObjectId(GetObjectIdForFrameProp(PropertyHelpers::Names::Locals))
            .build();

        return Scope::create()
            .setType("local")
            .setObject(std::move(remoteObj))
            .build();
    }

    std::unique_ptr<Scope> DebuggerCallFrame::GetClosureScope(JsValueRef scopeObj) const
    {
        int handle = PropertyHelpers::GetPropertyInt(scopeObj, PropertyHelpers::Names::Handle);

        auto remoteObj = RemoteObject::create()
            .setType("object")
            .setClassName("Object")
            .setDescription("Object")
            .setObjectId(ProtocolHelpers::GetObjectId(handle))
            .build();

        return Scope::create()
            .setType("closure")
            .setObject(std::move(remoteObj))
            .build();
    }

    std::unique_ptr<Scope> DebuggerCallFrame::GetGlobalScope() const
    {
        auto remoteObj = RemoteObject::create()
            .setType("object")
            .setClassName("global")
            .setDescription("global")
            .setObjectId(GetObjectIdForFrameProp(PropertyHelpers::Names::Globals))
            .build();

        return Scope::create()
            .setType("global")
            .setObject(std::move(remoteObj))
            .build();
    }
}
