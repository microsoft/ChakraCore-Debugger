// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ProtocolHelpers.h"
#include "PropertyHelpers.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    using protocol::DictionaryValue;
    using protocol::Debugger::Location;
    using protocol::Runtime::ExceptionDetails;
    using protocol::Runtime::InternalPropertyDescriptor;
    using protocol::Runtime::PropertyDescriptor;
    using protocol::Runtime::RemoteObject;
    using protocol::String;
    using protocol::StringUtil;
    using protocol::Value;

    namespace
    {
        const char c_DefaultExceptionText[] = "Uncaught";
        const char c_ErrorInvalidObjectId[] = "Invalid object ID";
        const char c_ErrorNoDisplayString[] = "No display string found";
        const int c_JsrtDebugPropertyReadOnly = 0x4;

        std::unique_ptr<Value> ToProtocolValue(JsValueRef /*object*/)
        {
            // TODO: traverse object graph and build protocol value.
            return Value::null();
        }

        std::unique_ptr<RemoteObject> CreateObject(JsValueRef object)
        {
            return RemoteObject::create()
                .setType(PropertyHelpers::GetPropertyString(object, PropertyHelpers::Names::Type))
                .build();
        }
    }

    String ProtocolHelpers::GetObjectId(int handle)
    {
        return "{\"handle\":" + String::fromInteger(handle) + "}";
    }

    std::unique_ptr<DictionaryValue> ProtocolHelpers::ParseObjectId(const String& objectId)
    {
        auto parsedValue = StringUtil::parseJSON(objectId);
        if (parsedValue == nullptr || parsedValue->type() != Value::TypeObject)
        {
            throw std::runtime_error(c_ErrorInvalidObjectId);
        }

        return std::unique_ptr<DictionaryValue>(DictionaryValue::cast(parsedValue.release()));
    }

    std::unique_ptr<RemoteObject> ProtocolHelpers::WrapObject(JsValueRef object)
    {
        auto remoteObject = CreateObject(object);

        String className;
        if (PropertyHelpers::TryGetProperty(object, PropertyHelpers::Names::ClassName, &className))
        {
            remoteObject->setClassName(className);
        }

        JsValueRef value = JS_INVALID_REFERENCE;
        bool hasValue = PropertyHelpers::TryGetProperty(object, PropertyHelpers::Names::Value, &value);
        if (hasValue)
        {
            remoteObject->setValue(ToProtocolValue(value));
        }

        String display;
        bool hasDisplay = PropertyHelpers::TryGetProperty(object, PropertyHelpers::Names::Display, &display);

        // A description is required for values to be shown in the debugger.
        if (!hasDisplay)
        {
            if (hasValue)
            {
                display = PropertyHelpers::GetPropertyStringConvert(object, PropertyHelpers::Names::Value);
            }
            else
            {
                throw std::runtime_error(c_ErrorNoDisplayString);
            }
        }

        remoteObject->setDescription(display);

        int handle = 0;
        if (PropertyHelpers::TryGetProperty(object, PropertyHelpers::Names::Handle, &handle))
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

    std::unique_ptr<ExceptionDetails> ProtocolHelpers::WrapExceptionDetails(JsValueRef exception)
    {
        int handle = PropertyHelpers::GetPropertyInt(exception, PropertyHelpers::Names::Handle);
        String text = PropertyHelpers::GetPropertyString(exception, PropertyHelpers::Names::Display);

        return ExceptionDetails::create()
            .setExceptionId(handle)
            .setText(!text.empty() ? text : c_DefaultExceptionText)
            .setLineNumber(0)
            .setColumnNumber(0)
            .setException(WrapException(exception))
            .build();
    }
    
    std::unique_ptr<PropertyDescriptor> ProtocolHelpers::WrapProperty(JsValueRef property)
    {
        String name = PropertyHelpers::GetPropertyString(property, PropertyHelpers::Names::Name);
        int propertyAttributes = PropertyHelpers::GetPropertyInt(property, PropertyHelpers::Names::PropertyAttributes);
        auto value = WrapObject(property);

        return PropertyDescriptor::create()
            .setName(name)
            .setValue(std::move(value))
            .setWritable((propertyAttributes & c_JsrtDebugPropertyReadOnly) == 0)
            .setConfigurable(true)
            .setEnumerable(true)
            .build();
    }

    std::unique_ptr<InternalPropertyDescriptor> ProtocolHelpers::WrapInternalProperty(JsValueRef property)
    {
        String name = PropertyHelpers::GetPropertyString(property, PropertyHelpers::Names::Name);
        auto value = WrapObject(property);

        return InternalPropertyDescriptor::create()
            .setName(name)
            .setValue(std::move(value))
            .build();
    }

    std::unique_ptr<Location> ProtocolHelpers::WrapLocation(JsValueRef location)
    {
        return Location::create()
            .setColumnNumber(PropertyHelpers::GetPropertyInt(location, PropertyHelpers::Names::Column))
            .setLineNumber(PropertyHelpers::GetPropertyInt(location, PropertyHelpers::Names::Line))
            .setScriptId(PropertyHelpers::GetPropertyStringConvert(location, PropertyHelpers::Names::ScriptId))
            .build();
    }

    std::unique_ptr<RemoteObject> ProtocolHelpers::GetUndefinedObject()
    {
        return RemoteObject::create()
            .setType("undefined")
            .build();
    }
}
