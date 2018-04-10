//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerBreakpoint.h"

namespace JsDebug
{
    using protocol::Debugger::Location;
    using protocol::String;

    namespace
    {
        const char c_ErrorRegexNotImplemented[] = "Regex query not implemented";
    }

    DebuggerBreakpoint::DebuggerBreakpoint(
        const String& query,
        QueryType queryType,
        int lineNumber,
        int columnNumber,
        const String& condition)
        : m_query(query)
        , m_queryType(queryType)
        , m_lineNumber(lineNumber)
        , m_columnNumber(columnNumber)
        , m_condition(condition)
        , m_actualBreakpointId(-1)
        , m_actualLineNumber(-1)
        , m_actualColumnNumber(-1)
    {
        if (m_queryType == QueryType::ScriptId)
        {
            m_scriptId = query;
        }
    }

    DebuggerBreakpoint DebuggerBreakpoint::FromLocation(Location* location, const String& condition)
    {
        return DebuggerBreakpoint(
            location->getScriptId(),
            QueryType::ScriptId,
            location->getLineNumber(),
            location->getColumnNumber(0),
            condition);
    }

    String DebuggerBreakpoint::GetQuery() const
    {
        return m_query;
    }

    DebuggerBreakpoint::QueryType DebuggerBreakpoint::GetQueryType() const
    {
        return m_queryType;
    }

    int DebuggerBreakpoint::GetLineNumber() const
    {
        return m_lineNumber;
    }

    int DebuggerBreakpoint::GetColumnNumber() const
    {
        return m_columnNumber;
    }

    String DebuggerBreakpoint::GetCondition() const
    {
        return m_condition;
    }

    String DebuggerBreakpoint::GetScriptId() const
    {
        return m_scriptId;
    }

    String DebuggerBreakpoint::GenerateKey() const
    {
        return String::fromInteger(static_cast<int>(m_queryType)) + ":" +
            m_query + ":" +
            String::fromInteger(m_lineNumber) + ":" +
            String::fromInteger(m_columnNumber);
    }

    bool DebuggerBreakpoint::IsScriptLoaded() const
    {
        return !m_scriptId.empty();
    }

    bool DebuggerBreakpoint::IsResolved() const
    {
        return m_actualLineNumber >= 0 && m_actualColumnNumber >= 0;
    }

    int DebuggerBreakpoint::GetActualId() const
    {
        return m_actualBreakpointId;
    }

    std::unique_ptr<protocol::Debugger::Location> DebuggerBreakpoint::GetActualLocation() const
    {
        if (IsResolved())
        {
            return Location::create()
                .setScriptId(m_scriptId)
                .setLineNumber(m_actualLineNumber)
                .setColumnNumber(m_actualColumnNumber)
                .build();
        }

        return nullptr;
    }

    bool DebuggerBreakpoint::TryLoadScript(const DebuggerScript& script)
    {
        if (IsScriptLoaded())
        {
            return false;
        }

        if (!IsScriptMatch(script))
        {
            return false;
        }

        m_scriptId = script.ScriptId();
        return true;
    }

    void DebuggerBreakpoint::OnBreakpointResolved(
        int actualBreakpointId,
        int actualLineNumber,
        int actualColumnNumber)
    {
        m_actualBreakpointId = actualBreakpointId;
        m_actualLineNumber = actualLineNumber;
        m_actualColumnNumber = actualColumnNumber;
    }

    bool DebuggerBreakpoint::IsScriptMatch(const DebuggerScript& script) const
    {
        switch (m_queryType)
        {
        case QueryType::ScriptId:
            return script.ScriptId() == m_query;

        case QueryType::Url:
            return script.SourceUrl() == m_query;

        case QueryType::UrlRegex:
            // TODO: Implement regex matching
            throw std::runtime_error(c_ErrorRegexNotImplemented);

        default:
            return false;
        }
    }
}
