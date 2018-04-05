//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ProtocolHelpers.h"
#include "PropertyHelpers.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    using protocol::Debugger::Location;
    using protocol::Runtime::RemoteObject;
    using protocol::String;
    using protocol::Value;

    namespace
    {
        String GetObjectId(int handle)
        {
            return "{\"handle\":" + String::fromInteger(handle) + "}";
        }

        std::unique_ptr<Value> ToProtocolValue(JsValueRef object)
        {
            // TODO: traverse object graph and build protocol value.
            return Value::null();
        }

        std::unique_ptr<RemoteObject> CreateObject(JsValueRef object)
        {
            return RemoteObject::create()
                .setType(PropertyHelpers::GetPropertyString(object, "type"))
                .build();
        }
    }

    std::unique_ptr<RemoteObject> ProtocolHelpers::WrapObject(JsValueRef object)
    {
        auto remoteObject = CreateObject(object);

        String className;
        if (PropertyHelpers::TryGetProperty(object, "className", &className))
        {
            remoteObject->setClassName(className);
        }

        JsValueRef value = JS_INVALID_REFERENCE;
        bool hasValue = PropertyHelpers::TryGetProperty(object, "value", &value);
        if (hasValue)
        {
            remoteObject->setValue(ToProtocolValue(value));
        }

        String display;
        bool hasDisplay = PropertyHelpers::TryGetProperty(object, "display", &display);

        // A description is required for values to be shown in the debugger.
        if (hasValue && !hasDisplay)
        {
            display = PropertyHelpers::GetPropertyStringConvert(object, "value");
        }

        if (display.empty())
        {
            throw std::runtime_error("no display string found");
        }

        remoteObject->setDescription(display);

        int handle = 0;
        if (PropertyHelpers::TryGetProperty(object, "handle", &handle))
        {
            remoteObject->setObjectId(GetObjectId(handle));
        }

        return remoteObject;
    }

    std::unique_ptr<RemoteObject> ProtocolHelpers::WrapException(JsValueRef exception)
    {
        std::unique_ptr<RemoteObject> wrapped = WrapObject(exception);
        wrapped->setSubtype("error");

        return wrapped;
    }

    std::unique_ptr<Location> ProtocolHelpers::WrapLocation(JsValueRef location)
    {
        return Location::create()
            .setColumnNumber(PropertyHelpers::GetPropertyInt(location, "column"))
            .setLineNumber(PropertyHelpers::GetPropertyInt(location, "line"))
            .setScriptId(PropertyHelpers::GetPropertyStringConvert(location, "scriptId"))
            .build();
    }

    std::unique_ptr<protocol::Runtime::RemoteObject> ProtocolHelpers::GetUndefinedObject()
    {
        return RemoteObject::create()
            .setType("undefined")
            .build();
    }
}
