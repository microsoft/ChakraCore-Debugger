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
        void GetProperty(JsValueRef object, const char* name, JsValueRef* value);
        void GetProperty(JsValueRef object, const char* name, bool* value);
        void GetProperty(JsValueRef object, const char* name, int* value);
        void GetProperty(JsValueRef object, const char* name, String16* value);
        void GetPropertyAsString(JsValueRef object, const char* name, String16* value);

        JsValueRef GetIndexedProperty(JsValueRef object, int index);

        bool HasProperty(JsValueRef object, const char* name);

        bool TryGetProperty(JsValueRef object, const char* name, JsValueRef* value);
        bool TryGetProperty(JsValueRef object, const char* name, bool* value);
        bool TryGetProperty(JsValueRef object, const char* name, int* value);
        bool TryGetProperty(JsValueRef object, const char* name, String16* value);
    }
}
