//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "String16.h"
#include "JsPersistent.h"
#include <ChakraCore.h>

namespace JsDebug
{
    class DebuggerScript
    {
    public:
        DebuggerScript(JsValueRef scriptInfo);

        String16 ScriptId() const;
        String16 Url() const;
        bool HasSourceUrl() const;
        String16 SourceUrl() const;
        String16 SourceMappingUrl() const;
        String16 Source() const;
        String16 Hash() const;
        int StartLine() const;
        int StartColumn() const;
        int EndLine() const;
        int EndColumn() const;
        int ExecutionContextId() const;
        String16 ExecutionContextAuxData() const;
        bool IsLiveEdit() const;

    private:
        JsPersistent m_scriptInfo;
        JsPersistent m_scriptSource;

        int m_scriptId;
        String16 m_sourceUrl;
        String16 m_sourceMappingUrl;
        String16 m_hash;
    };
}
