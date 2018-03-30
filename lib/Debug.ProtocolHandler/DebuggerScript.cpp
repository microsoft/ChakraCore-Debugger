//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerScript.h"
#include "PropertyHelpers.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    DebuggerScript::DebuggerScript(JsValueRef scriptInfo)
        : m_scriptInfo(scriptInfo)
        , m_scriptId(0)
    {
        if (!m_scriptInfo.IsEmpty())
        {
            PropertyHelpers::GetProperty(m_scriptInfo.Get(), "scriptId", &m_scriptId);

            JsValueRef scriptSource = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsDiagGetSource(m_scriptId, &scriptSource));
            m_scriptSource = scriptSource;

            // TODO: calculate hash
            // TODO: parse for source URL
            // TODO: parse for source map URL
        }
    }

    String16 DebuggerScript::ScriptId() const
    {
        return String16::fromInteger(m_scriptId);
    }

    String16 DebuggerScript::Url() const
    {
        String16 fileName;

        if (!m_scriptInfo.IsEmpty())
        {
            // Check the fileName property first
            if (!PropertyHelpers::TryGetProperty(m_scriptInfo.Get(), "fileName", &fileName))
            {
                // Fall back to the scriptType property
                PropertyHelpers::TryGetProperty(m_scriptInfo.Get(), "scriptType", &fileName);
            }
        }

        return fileName;
    }

    bool DebuggerScript::HasSourceUrl() const
    {
        return !m_sourceUrl.empty();
    }

    String16 DebuggerScript::SourceUrl() const
    {
        return HasSourceUrl() ? m_sourceUrl : Url();
    }

    String16 DebuggerScript::SourceMappingUrl() const
    {
        return m_sourceMappingUrl;
    }

    String16 DebuggerScript::Source() const
    {
        String16 source;

        if (!m_scriptSource.IsEmpty())
        {
            PropertyHelpers::GetProperty(m_scriptSource.Get(), "source", &source);
        }

        return source;
    }

    String16 DebuggerScript::Hash() const
    {
        return m_hash;
    }

    int DebuggerScript::StartLine() const
    {
        return 0;
    }

    int DebuggerScript::StartColumn() const
    {
        return 0;
    }

    int DebuggerScript::EndLine() const
    {
        int lineCount = 0;

        if (!m_scriptInfo.IsEmpty())
        {
            PropertyHelpers::GetProperty(m_scriptInfo.Get(), "lineCount", &lineCount);
        }

        return lineCount;
    }

    int DebuggerScript::EndColumn() const
    {
        return 0;
    }

    int DebuggerScript::ExecutionContextId() const
    {
        return 0;
    }

    String16 DebuggerScript::ExecutionContextAuxData() const
    {
        return String16();
    }

    bool DebuggerScript::IsLiveEdit() const
    {
        return false;
    }
}
