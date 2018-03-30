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
        JsPersistent()
            : JsPersistent(JS_INVALID_REFERENCE)
        {
        }

        JsPersistent(JsValueRef value)
            : m_value(value)
        {
            if (m_value != JS_INVALID_REFERENCE)
            {
                IfJsErrorThrow(JsAddRef(m_value, nullptr));
            }
        }

        ~JsPersistent()
        {
            try
            {
                if (m_value != JS_INVALID_REFERENCE)
                {
                    IfJsErrorThrow(JsRelease(m_value, nullptr));
                }
            }
            catch (const std::exception& e)
            {
                std::cerr << e.what() << std::endl;
            }
        }

        JsPersistent(const JsPersistent& other)
            : JsPersistent()
        {
            JsPersistent temp(other.m_value);
            std::swap(*this, temp);
        }

        JsPersistent(JsPersistent&& other)
            : JsPersistent()
        {
            std::swap(m_value, other.m_value);
        }

        JsPersistent& operator=(JsPersistent&& other)
        {
            std::swap(m_value, other.m_value);

            return *this;
        }

        JsPersistent& operator=(const JsPersistent& other)
        {
            JsPersistent temp(other);
            std::swap(*this, temp);

            return *this;
        }
        
        JsPersistent& operator=(const JsValueRef& other)
        {
            JsPersistent temp(other);
            std::swap(*this, temp);

            return *this;
        }

        bool IsEmpty() const
        {
            return m_value == JS_INVALID_REFERENCE;
        }

        JsValueRef Get() const
        {
            return m_value;
        }

    private:
        JsValueRef m_value;
    };
}
