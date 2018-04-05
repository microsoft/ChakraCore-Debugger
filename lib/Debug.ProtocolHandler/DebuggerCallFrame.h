//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "JsPersistent.h"

#include <ChakraCore.h>
#include <protocol/Debugger.h>

namespace JsDebug
{
    class DebuggerCallFrame
    {
    public:
        DebuggerCallFrame(JsValueRef callFrameInfo);

        int SourceId() const;
        int Line() const;
        int Column() const;
        int ContextId() const;
        bool IsAtReturn() const;

        std::unique_ptr<protocol::Debugger::CallFrame> ToProtocolValue() const;

    private:
        protocol::String GetCallFrameId() const;
        std::unique_ptr<protocol::Debugger::Location> GetFunctionLocation() const;
        protocol::String GetFunctionName() const;
        std::unique_ptr<protocol::Debugger::Location> GetLocation() const;
        std::unique_ptr<protocol::Runtime::RemoteObject> GetReturnValue() const;
        std::unique_ptr<protocol::Array<protocol::Debugger::Scope>> GetScopeChain() const;
        std::unique_ptr<protocol::Runtime::RemoteObject> GetThis() const;

        JsPersistent m_callFrameInfo;
        int m_callFrameIndex;
    };
}
