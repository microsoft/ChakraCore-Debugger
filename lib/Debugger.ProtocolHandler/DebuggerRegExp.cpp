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
        , m_pattern(pattern)
        , m_flags(flags)
    {
    }

    bool DebuggerRegExp::Test(const String& str)
    {
        DebuggerContext::Scope scope(*m_debugger->GetDebugContext());

        JsValueRef globalObject = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetGlobalObject(&globalObject));

        JsValueRef undefined = JS_INVALID_REFERENCE;
        JsGetUndefinedValue(&undefined);

        JsValueRef pattern = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(m_pattern.characters16(), m_pattern.length(), &pattern));

        JsValueRef flags = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(m_flags.characters16(), m_flags.length(), &flags));

        JsValueRef regExpConstructor = PropertyHelpers::GetProperty(globalObject, "RegExp");

        JsValueRef regExp = JS_INVALID_REFERENCE;
        std::array<JsValueRef, 3> args{ undefined, pattern, flags };
        IfJsErrorThrow(JsConstructObject(
            regExpConstructor, 
            args.data(), 
            static_cast<unsigned short>(args.size()), 
            &regExp));

        JsValueRef testFunction = PropertyHelpers::GetProperty(regExp, "test");

        JsValueRef strValue = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateStringUtf16(str.characters16(), str.length(), &strValue));

        JsValueRef result = JS_INVALID_REFERENCE;
        std::array<JsValueRef, 2> args2{ regExp, strValue };
        IfJsErrorThrow(JsCallFunction(
            testFunction, 
            args2.data(), 
            static_cast<unsigned short>(args2.size()), 
            &result));

        bool found = false;
        IfJsErrorThrow(JsBooleanToBool(result, &found));

        return found;
    }
}
