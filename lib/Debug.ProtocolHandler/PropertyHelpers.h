//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "String16.h"
#include <ChakraCore.h>

namespace JsDebug
{
    namespace PropertyHelpers
    {
        JsValueRef GetProperty(JsValueRef object, const char* name);

        bool GetPropertyBool(JsValueRef object, const char* name);
        int GetPropertyInt(JsValueRef object, const char* name);
        String16 GetPropertyString(JsValueRef object, const char* name);

        bool GetPropertyBoolConvert(JsValueRef object, const char* name);
        int GetPropertyIntConvert(JsValueRef object, const char* name);
        String16 GetPropertyStringConvert(JsValueRef object, const char* name);

        JsValueRef GetIndexedProperty(JsValueRef object, int index);

        bool HasProperty(JsValueRef object, const char* name);

        bool TryGetProperty(JsValueRef object, const char* name, JsValueRef* value);
        bool TryGetProperty(JsValueRef object, const char* name, bool* value);
        bool TryGetProperty(JsValueRef object, const char* name, int* value);
        bool TryGetProperty(JsValueRef object, const char* name, String16* value);
    }
}
