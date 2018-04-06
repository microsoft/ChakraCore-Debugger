//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "DebuggerScript.h"
#include <protocol/Debugger.h>

namespace JsDebug
{
    class DebuggerBreakpoint
    {
    public:
        enum class QueryType
        {
            ScriptId,
            Url,
            UrlRegex,
        };

        DebuggerBreakpoint(
            const protocol::String& query,
            QueryType queryType,
            int lineNumber,
            int columnNumber,
            const protocol::String& condition);

        static DebuggerBreakpoint FromLocation(
            protocol::Debugger::Location* location,
            const protocol::String& condition);

        protocol::String GetQuery() const;
        QueryType GetQueryType() const;
        int GetLineNumber() const;
        int GetColumnNumber() const;
        protocol::String GetCondition() const;

        protocol::String GetScriptId() const;
        protocol::String GenerateKey() const;
        bool IsScriptLoaded() const;
        bool IsResolved() const;

        int GetActualId() const;
        std::unique_ptr<protocol::Debugger::Location> GetActualLocation() const;

        bool TryLoadScript(const DebuggerScript& script);
        void OnBreakpointResolved(int actualBreakpointId, int actualLineNumber, int actualColumnNumber);

    private:
        bool IsScriptMatch(const DebuggerScript& script) const;

        protocol::String m_query;
        QueryType m_queryType;
        int m_lineNumber;
        int m_columnNumber;
        protocol::String m_condition;

        protocol::String m_scriptId;

        int m_actualBreakpointId;
        int m_actualLineNumber;
        int m_actualColumnNumber;
    };
}
