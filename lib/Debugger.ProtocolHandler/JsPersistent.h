//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

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
