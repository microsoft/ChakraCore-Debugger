//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerScript.h"

#include "ErrorHelpers.h"
#include "PropertyHelpers.h"

namespace JsDebug
{
    using protocol::String;

    DebuggerScript::DebuggerScript(JsValueRef scriptInfo)
        : m_scriptInfo(scriptInfo)
        , m_scriptId(0)
    {
        if (!m_scriptInfo.IsEmpty())
        {
            m_scriptId = PropertyHelpers::GetPropertyInt(m_scriptInfo.Get(), PropertyHelpers::Names::ScriptId);

            JsValueRef scriptSource = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsDiagGetSource(m_scriptId, &scriptSource));
            m_scriptSource = scriptSource;

            // TODO: calculate hash
            // TODO: parse for source URL
            // TODO: parse for source map URL
        }
    }

    String DebuggerScript::ScriptId() const
    {
        return String::fromInteger(m_scriptId);
    }

    String DebuggerScript::Url() const
    {
        String fileName;

        if (!m_scriptInfo.IsEmpty())
        {
            // Check the fileName property first
            if (!PropertyHelpers::TryGetProperty(m_scriptInfo.Get(), PropertyHelpers::Names::FileName, &fileName))
            {
                // Fall back to the scriptType property
                PropertyHelpers::TryGetProperty(m_scriptInfo.Get(), PropertyHelpers::Names::ScriptType, &fileName);
            }
        }

        return fileName;
    }

    bool DebuggerScript::HasSourceUrl() const
    {
        return !m_sourceUrl.empty();
    }

    String DebuggerScript::SourceUrl() const
    {
        return HasSourceUrl() ? m_sourceUrl : Url();
    }

    String DebuggerScript::SourceMappingUrl() const
    {
        return m_sourceMappingUrl;
    }

    String DebuggerScript::Source() const
    {
        if (!m_scriptSource.IsEmpty())
        {
            return PropertyHelpers::GetPropertyString(m_scriptSource.Get(), PropertyHelpers::Names::Source);
        }

        return String();
    }

    String DebuggerScript::Hash() const
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
        if (!m_scriptInfo.IsEmpty())
        {
            return PropertyHelpers::GetPropertyInt(m_scriptInfo.Get(), PropertyHelpers::Names::LineCount);
        }

        return 0;
    }

    int DebuggerScript::EndColumn() const
    {
        return 0;
    }

    int DebuggerScript::ExecutionContextId() const
    {
        return 0;
    }

    String DebuggerScript::ExecutionContextAuxData() const
    {
        return String();
    }

    bool DebuggerScript::IsLiveEdit() const
    {
        return false;
    }
}
