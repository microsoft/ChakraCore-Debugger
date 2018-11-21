// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "String16.h"
#include <ChakraCore.h>

namespace JsDebug
{
    namespace PropertyHelpers
    {
        namespace Names
        {
            constexpr char Arguments[] = "arguments";
            constexpr char BreakpointId[] = "breakpointId";
            constexpr char ClassName[] = "className";
            constexpr char Column[] = "column";
            constexpr char DebuggerOnlyProperties[] = "debuggerOnlyProperties";
            constexpr char Display[] = "display";
            constexpr char Exception[] = "exception";
            constexpr char Exec[] = "exec";
            constexpr char FileName[] = "fileName";
            constexpr char FunctionCallsReturn[] = "functionCallsReturn";
            constexpr char FunctionHandle[] = "functionHandle";
            constexpr char Globals[] = "globals";
            constexpr char Handle[] = "handle";
            constexpr char Index[] = "index";
            constexpr char Length[] = "length";
            constexpr char Line[] = "line";
            constexpr char LineCount[] = "lineCount";
            constexpr char Locals[] = "locals";
            constexpr char Name[] = "name";
            constexpr char Ordinal[] = "ordinal";
            constexpr char Properties[] = "properties";
            constexpr char PropertyAttributes[] = "propertyAttributes";
            constexpr char RegExp[] = "RegExp";
            constexpr char ReturnValue[] = "returnValue";
            constexpr char ScriptId[] = "scriptId";
            constexpr char ScriptType[] = "scriptType";
            constexpr char Scopes[] = "scopes";
            constexpr char Source[] = "source";
            constexpr char Test[] = "test";
            constexpr char ThisObject[] = "thisObject";
            constexpr char Type[] = "type";
            constexpr char Uncaught[] = "uncaught";
            constexpr char Value[] = "value";
        }

        JsValueRef GetProperty(JsValueRef object, const char* name);
        bool SetProperty(JsValueRef object, const char* propertyName, JsValueRef value);

        bool GetPropertyBool(JsValueRef object, const char* name);
        int GetPropertyInt(JsValueRef object, const char* name);
        String16 GetPropertyString(JsValueRef object, const char* name);

        bool GetPropertyBoolConvert(JsValueRef object, const char* name);
        int GetPropertyIntConvert(JsValueRef object, const char* name);
        String16 GetPropertyStringConvert(JsValueRef object, const char* name);

        JsValueRef GetIndexedProperty(JsValueRef object, int index);
        String16 GetIndexedPropertyString(JsValueRef object, int index);

        bool HasProperty(JsValueRef object, const char* name);

        bool TryGetProperty(JsValueRef object, const char* name, JsValueRef* value);
        bool TryGetProperty(JsValueRef object, const char* name, bool* value);
        bool TryGetProperty(JsValueRef object, const char* name, int* value);
        bool TryGetProperty(JsValueRef object, const char* name, String16* value);
    }
}
