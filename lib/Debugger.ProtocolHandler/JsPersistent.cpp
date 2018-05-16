//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "JsPersistent.h"

namespace JsDebug
{
    JsPersistent::JsPersistent()
        : JsPersistent(JS_INVALID_REFERENCE)
    {
    }

    JsPersistent::JsPersistent(JsValueRef value)
        : m_value(value)
    {
        if (m_value != JS_INVALID_REFERENCE)
        {
            IfJsErrorThrow(JsAddRef(m_value, nullptr));
        }
    }

    JsPersistent::~JsPersistent()
    {
        try
        {
            if (m_value != JS_INVALID_REFERENCE)
            {
                JsErrorCode err = JsNoError;
                err = JsRelease(m_value, nullptr);

                // If the host has already cleared the context, release will fail. Get the context of the object, set
                // it temporarily, release the reference, then clear the context again.
                if (err == JsErrorNoCurrentContext)
                {
                    JsContextRef objContext = JS_INVALID_REFERENCE;
                    IfJsErrorThrow(JsGetContextOfObject(m_value, &objContext));
                    IfJsErrorThrow(JsSetCurrentContext(objContext));

                    err = JsRelease(m_value, nullptr);

                    IfJsErrorThrow(JsSetCurrentContext(JS_INVALID_REFERENCE));
                }

                IfJsErrorThrow(err);
            }
        }
        catch (...)
        {
            // Don't allow the exception to propagate.
        }
    }

    JsPersistent::JsPersistent(const JsPersistent& other)
        : JsPersistent()
    {
        JsPersistent temp(other.m_value);
        std::swap(*this, temp);
    }

    JsPersistent::JsPersistent(JsPersistent&& other)
        : JsPersistent()
    {
        std::swap(m_value, other.m_value);
    }

    JsPersistent& JsPersistent::operator=(JsPersistent&& other)
    {
        std::swap(m_value, other.m_value);

        return *this;
    }

    JsPersistent& JsPersistent::operator=(const JsPersistent& other)
    {
        JsPersistent temp(other);
        std::swap(*this, temp);

        return *this;
    }

    JsPersistent& JsPersistent::operator=(const JsValueRef& other)
    {
        JsPersistent temp(other);
        std::swap(*this, temp);

        return *this;
    }

    bool JsPersistent::IsEmpty() const
    {
        return m_value == JS_INVALID_REFERENCE;
    }

    JsValueRef JsPersistent::Get() const
    {
        return m_value;
    }
}
