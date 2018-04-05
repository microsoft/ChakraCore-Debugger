//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "PropertyHelpers.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    namespace
    {
        template <bool CONVERT>
        inline String16 ValueAsString(JsValueRef object)
        {
            JsValueRef stringValue = JS_INVALID_REFERENCE;

            if (CONVERT)
            {
                IfJsErrorThrow(JsConvertValueToString(object, &stringValue));
            }
            else
            {
                stringValue = object;
            }

            int stringLength = 0;
            IfJsErrorThrow(JsGetStringLength(stringValue, &stringLength));

            std::vector<uint16_t> buffer;

            // Resize the vector an initialize the elements to zero.
            buffer.resize(stringLength, 0);

            IfJsErrorThrow(JsCopyStringUtf16(stringValue, 0, static_cast<int>(buffer.size()), buffer.data(), nullptr));

            return String16(buffer.data(), buffer.size());
        }

        template <class T, bool CONVERT = false, class JsConvertToValueFunc, class JsValueToNativeFunc>
        T ValueToNative(
            const JsConvertToValueFunc& JsConvertToValue,
            const JsValueToNativeFunc& JsValueToNative,
            JsValueRef value)
        {
            if (CONVERT)
            {
                IfJsErrorThrow(JsConvertToValue(value, &value));
            }

            T nativeValue = {};
            IfJsErrorThrow(JsValueToNative(value, &nativeValue));

            return nativeValue;
        }
    }

    JsValueRef PropertyHelpers::GetProperty(JsValueRef object, const char* name)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));

        JsValueRef value = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetProperty(object, propertyId, &value));

        return value;
    }

    bool PropertyHelpers::GetPropertyBool(JsValueRef object, const char* name)
    {
        JsValueRef objValue = GetProperty(object, name);
        return ValueToNative<bool>(JsConvertValueToBoolean, JsBooleanToBool, objValue);
    }

    int PropertyHelpers::GetPropertyInt(JsValueRef object, const char* name)
    {
        JsValueRef objValue = GetProperty(object, name);
        return ValueToNative<int>(JsConvertValueToNumber, JsNumberToInt, objValue);
    }

    String16 PropertyHelpers::GetPropertyString(JsValueRef object, const char* name)
    {
        JsValueRef objValue = PropertyHelpers::GetProperty(object, name);
        return ValueAsString</*CONVERT*/false>(objValue);
    }

    bool PropertyHelpers::GetPropertyBoolConvert(JsValueRef object, const char* name)
    {
        JsValueRef objValue = GetProperty(object, name);
        return ValueToNative<bool, /*CONVERT*/true>(JsConvertValueToBoolean, JsBooleanToBool, objValue);
    }

    int PropertyHelpers::GetPropertyIntConvert(JsValueRef object, const char* name)
    {
        JsValueRef objValue = GetProperty(object, name);
        return ValueToNative<int, /*CONVERT*/true>(JsConvertValueToNumber, JsNumberToInt, objValue);
    }

    String16 PropertyHelpers::GetPropertyStringConvert(JsValueRef object, const char* name)
    {
        JsValueRef objValue = PropertyHelpers::GetProperty(object, name);
        return ValueAsString</*CONVERT*/true>(objValue);
    }

    JsValueRef PropertyHelpers::GetIndexedProperty(JsValueRef object, int index)
    {
        JsValueRef indexValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsIntToNumber(index, &indexValue));

        JsValueRef value = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetIndexedProperty(object, indexValue, &value));

        return value;
    }

    bool PropertyHelpers::HasProperty(JsValueRef object, const char* name)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));

        bool hasProperty = false;
        IfJsErrorThrow(JsHasProperty(object, propertyId, &hasProperty));

        return hasProperty;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, JsValueRef* value)
    {
        JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreatePropertyId(name, std::strlen(name), &propertyId));

        bool hasProperty = false;
        IfJsErrorThrow(JsHasProperty(object, propertyId, &hasProperty));

        if (hasProperty)
        {
            IfJsErrorThrow(JsGetProperty(object, propertyId, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, bool* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            IfJsErrorThrow(JsBooleanToBool(propertyValue, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, int* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            IfJsErrorThrow(JsNumberToInt(propertyValue, value));
            return true;
        }

        return false;
    }

    bool PropertyHelpers::TryGetProperty(JsValueRef object, const char* name, String16* value)
    {
        JsValueRef propertyValue = JS_INVALID_REFERENCE;
        if (TryGetProperty(object, name, &propertyValue))
        {
            *value = ValueAsString</*CONVERT*/false>(propertyValue);
            return true;
        }

        return false;
    }
}
