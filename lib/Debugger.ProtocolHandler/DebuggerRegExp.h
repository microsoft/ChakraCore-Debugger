//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol/Debugger.h>

namespace JsDebug
{
    class Debugger;

    class DebuggerRegExp
    {
    public:
        DebuggerRegExp(
            Debugger* debugger,
            const protocol::String& pattern,
            const protocol::String& flags);

        bool Test(const protocol::String& str);

    private:
        Debugger* m_debugger;
        protocol::String m_pattern;
        protocol::String m_flags;
    };
}
