//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "JsPersistent.h"
#include <ChakraCore.h>

#include <protocol/Debugger.h>
#include <protocol/Runtime.h>

#include <memory>

namespace JsDebug
{
    class DebuggerBreak
    {
    public:
        DebuggerBreak(JsValueRef breakInfo);

        protocol::String GetReason() const;
        protocol::Maybe<protocol::DictionaryValue> GetData() const;
        protocol::Maybe<protocol::Array<protocol::String>> GetHitBreakpoints() const;
        protocol::Maybe<protocol::Runtime::StackTrace> GetAsyncStackTrace() const;

    private:
        std::unique_ptr<protocol::Runtime::RemoteObject> GetException() const;

        JsPersistent m_breakInfo;
    };
}
