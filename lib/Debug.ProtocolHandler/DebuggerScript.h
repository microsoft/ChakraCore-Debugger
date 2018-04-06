//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "JsPersistent.h"

#include <StringUtil.h>
#include <ChakraCore.h>

namespace JsDebug
{
    class DebuggerScript
    {
    public:
        DebuggerScript(JsValueRef scriptInfo);

        protocol::String ScriptId() const;
        protocol::String Url() const;
        bool HasSourceUrl() const;
        protocol::String SourceUrl() const;
        protocol::String SourceMappingUrl() const;
        protocol::String Source() const;
        protocol::String Hash() const;
        int StartLine() const;
        int StartColumn() const;
        int EndLine() const;
        int EndColumn() const;
        int ExecutionContextId() const;
        protocol::String ExecutionContextAuxData() const;
        bool IsLiveEdit() const;

    private:
        JsPersistent m_scriptInfo;
        JsPersistent m_scriptSource;

        int m_scriptId;
        protocol::String m_sourceUrl;
        protocol::String m_sourceMappingUrl;
        protocol::String m_hash;
    };
}
