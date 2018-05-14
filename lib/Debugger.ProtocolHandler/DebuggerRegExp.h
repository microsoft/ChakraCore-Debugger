//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "JsPersistent.h"
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

        std::vector<protocol::String> Exec(JsValueRef value);
        std::vector<protocol::String> Exec(const protocol::String& str);
        bool Test(const protocol::String& str);

    private:
        Debugger* m_debugger;
        JsPersistent m_regExp;
    };
}
