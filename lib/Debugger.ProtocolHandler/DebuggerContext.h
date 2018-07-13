// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "JsPersistent.h"
#include <ChakraCore.h>

namespace JsDebug
{
    class DebuggerContext
    {
    public:
        class Scope
        {
        public:
            explicit Scope(const DebuggerContext& context);
            ~Scope();

        private:
            JsPersistent m_currentContext;
            JsPersistent m_previousContext;
        };

        explicit DebuggerContext(JsRuntimeHandle runtime);

    private:

        JsPersistent m_context;
    };
}
