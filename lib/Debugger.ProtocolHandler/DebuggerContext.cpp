//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerContext.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    DebuggerContext::Scope::Scope(const DebuggerContext& context)
    {
        JsContextRef currentContext = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsGetCurrentContext(&currentContext));
        m_previousContext = currentContext;

        JsContextRef newContext = context.m_context.Get();
        IfJsErrorThrow(JsSetCurrentContext(context.m_context.Get()));
        m_currentContext = newContext;
    }

    DebuggerContext::Scope::~Scope()
    {
        try
        {
            JsContextRef currentContext = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsGetCurrentContext(&currentContext));
            assert(currentContext == m_currentContext.Get());
        }
        catch (const JsErrorException&)
        {
            // Consider this best effort, if it fails still continue and set the previous context.
        }

        try
        {
            IfJsErrorThrow(JsSetCurrentContext(m_previousContext.Get()));
        }
        catch (const JsErrorException&)
        {
            // We couldn't restore the context, we really can't continue from here.
            assert(false);
        }
    }

    DebuggerContext::DebuggerContext(JsRuntimeHandle runtime)
    {
        JsContextRef context = JS_INVALID_REFERENCE;
        IfJsErrorThrow(JsCreateContext(runtime, &context));
        m_context = context;
    }
}
