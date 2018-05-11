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
        namespace Names
        {
            const char Arguments[] = "arguments";
            const char BreakpointId[] = "breakpointId";
            const char ClassName[] = "className";
            const char Column[] = "column";
            const char DebuggerOnlyProperties[] = "debuggerOnlyProperties";
            const char Display[] = "display";
            const char Exception[] = "exception";
            const char FileName[] = "fileName";
            const char FunctionCallsReturn[] = "functionCallsReturn";
            const char FunctionHandle[] = "functionHandle";
            const char Globals[] = "globals";
            const char Handle[] = "handle";
            const char Index[] = "index";
            const char Length[] = "length";
            const char Line[] = "line";
            const char LineCount[] = "lineCount";
            const char Locals[] = "locals";
            const char Name[] = "name";
            const char Ordinal[] = "ordinal";
            const char Properties[] = "properties";
            const char PropertyAttributes[] = "propertyAttributes";
            const char ReturnValue[] = "returnValue";
            const char ScriptId[] = "scriptId";
            const char ScriptType[] = "scriptType";
            const char Scopes[] = "scopes";
            const char Source[] = "source";
            const char ThisObject[] = "thisObject";
            const char Type[] = "type";
            const char Uncaught[] = "uncaught";
            const char Value[] = "value";
        }

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
