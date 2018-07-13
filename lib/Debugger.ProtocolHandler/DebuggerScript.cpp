// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "DebuggerScript.h"

#include "Debugger.h"
#include "DebuggerRegExp.h"
#include "ErrorHelpers.h"
#include "PropertyHelpers.h"

namespace JsDebug
{
    using protocol::String;

    namespace
    {
        const char c_PropertySourceMappingURL[] = "sourceMappingURL";
        const char c_PropertySourceURL[] = "sourceURL";
        const char c_SourceInfoCommentFlags[] = "mg";

        /// <summary>
        /// Matches on attribute comments of the form:
        /// * <c>//# attribute=value</c>
        /// * <c>/*# attribute=value */</c>
        /// * <c>//@ attribute=value</c>
        /// * <c>/*@ attribute=value */</c>
        /// </summary>
        const char c_SourceInfoCommentPattern[] = "//[#@][ \\t]*([^=]+?)[ \\t]*=[ \\t]*(.+?)[ \\t]*$|/\\*[#@][ \\t]*([^=]+?)[ \\t]*=[ \\t]*(.+?)[ \\t]*\\*/";
    }

    DebuggerScript::DebuggerScript(Debugger* debugger, JsValueRef scriptInfo)
        : m_debugger(debugger)
        , m_scriptInfo(scriptInfo)
        , m_scriptId(0)
    {
        if (!m_scriptInfo.IsEmpty())
        {
            m_scriptId = PropertyHelpers::GetPropertyInt(m_scriptInfo.Get(), PropertyHelpers::Names::ScriptId);

            JsValueRef scriptSource = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsDiagGetSource(m_scriptId, &scriptSource));
            m_scriptSource = scriptSource;

            // TODO: calculate file hash
            ParseScriptSource();
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

    void DebuggerScript::ParseScriptSource()
    {
        DebuggerRegExp regExp(m_debugger, c_SourceInfoCommentPattern, c_SourceInfoCommentFlags);
        std::vector<String> matchGroups;

        JsValueRef sourceValue = PropertyHelpers::GetProperty(m_scriptSource.Get(), PropertyHelpers::Names::Source);

        while (true)
        {
            matchGroups = regExp.Exec(sourceValue);

            if (matchGroups.size() == 5)
            {
                if (matchGroups[1] == c_PropertySourceMappingURL)
                {
                    m_sourceMappingUrl = matchGroups[2];
                }
                else if (matchGroups[3] == c_PropertySourceMappingURL)
                {
                    m_sourceMappingUrl = matchGroups[4];
                }
                else if (matchGroups[1] == c_PropertySourceURL)
                {
                    m_sourceUrl = matchGroups[2];
                }
                else if (matchGroups[3] == c_PropertySourceURL)
                {
                    m_sourceUrl = matchGroups[4];
                }

                continue;
            }

            break;
        }
    }
}
