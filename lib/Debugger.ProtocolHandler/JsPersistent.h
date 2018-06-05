// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ErrorHelpers.h"
#include <ChakraCore.h>

namespace JsDebug
{
    class JsPersistent
    {
    public:
        JsPersistent();
        JsPersistent(JsValueRef value);
        ~JsPersistent();

        JsPersistent(const JsPersistent& other);
        JsPersistent(JsPersistent&& other);

        JsPersistent& operator=(JsPersistent&& other);
        JsPersistent& operator=(const JsPersistent& other);
        JsPersistent& operator=(const JsValueRef& other);

        bool IsEmpty() const;
        JsValueRef Get() const;

    private:
        JsValueRef m_value;
    };
}
