//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerRegExp.h"

#include "Debugger.h"
#include "DebuggerContext.h"
#include "ErrorHelpers.h"
#include "PropertyHelpers.h"

namespace JsDebug
{
    using protocol::String;

    DebuggerRegExp::DebuggerRegExp(
        Debugger* debugger,
        const String& pattern,
        const String& flags)
        : m_debugger(debugger)
    {
        DebuggerContext::Scope scope(*m_debugger->GetDebugContext());

        JsValueRef globalObject = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetGlobalObject(&globalObject));

        JsValueRef undefined = JS_INVALID_REFERENCE;
        JsGetUndefinedValue(&undefined);

        JsValueRef patternValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(pattern.characters16(), pattern.length(), &patternValue));

        JsValueRef flagsValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(flags.characters16(), flags.length(), &flagsValue));

        JsValueRef regExpConstructor = PropertyHelpers::GetProperty(globalObject, PropertyHelpers::Names::RegExp);

        JsValueRef regExp = JS_INVALID_REFERENCE;
        std::array<JsValueRef, 3> args{ undefined, patternValue, flagsValue };
        IfJsErrorThrow(JsConstructObject(
            regExpConstructor,
            args.data(),
            static_cast<unsigned short>(args.size()),
            &regExp));

        m_regExp = regExp;
    }

    std::vector<String> DebuggerRegExp::Exec(JsValueRef value)
    {
        DebuggerContext::Scope scope(*m_debugger->GetDebugContext());

        JsValueRef execFunction = PropertyHelpers::GetProperty(m_regExp.Get(), PropertyHelpers::Names::Exec);

        JsValueRef result = JS_INVALID_REFERENCE;
        std::array<JsValueRef, 2> args{ m_regExp.Get(), value };
        IfJsErrorThrow(JsCallFunction(
            execFunction,
            args.data(),
            static_cast<unsigned short>(args.size()),
            &result));

        std::vector<String> match;

        JsValueRef nullValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetNullValue(&nullValue));

        if (result != nullValue)
        {
            int length = PropertyHelpers::GetPropertyInt(result, PropertyHelpers::Names::Length);

            for (int i = 0; i < length; i++)
            {
                String strValue = PropertyHelpers::GetIndexedPropertyString(result, i);
                match.push_back(strValue);
            }
        }

        return match;
    }

    std::vector<String> DebuggerRegExp::Exec(const String& str)
    {
        JsValueRef strValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(str.characters16(), str.length(), &strValue));

        return Exec(strValue);
    }

    bool DebuggerRegExp::Test(const String& str)
    {
        DebuggerContext::Scope scope(*m_debugger->GetDebugContext());

        JsValueRef testFunction = PropertyHelpers::GetProperty(m_regExp.Get(), PropertyHelpers::Names::Test);

        JsValueRef strValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(str.characters16(), str.length(), &strValue));

        JsValueRef result = JS_INVALID_REFERENCE;
        std::array<JsValueRef, 2> args{ m_regExp.Get(), strValue };
        IfJsErrorThrow(JsCallFunction(
            testFunction, 
            args.data(), 
            static_cast<unsigned short>(args.size()), 
            &result));

        bool found = false;
        IfJsErrorThrow(JsBooleanToBool(result, &found));

        return found;
    }
}
