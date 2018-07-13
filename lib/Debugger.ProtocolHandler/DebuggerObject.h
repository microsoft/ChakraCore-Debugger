// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "JsPersistent.h"

#include <protocol/Runtime.h>

namespace JsDebug
{
    class DebuggerObject
    {
    public:
        explicit DebuggerObject(JsValueRef obj);

        virtual std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>
            GetPropertyDescriptors() const;
        virtual std::unique_ptr<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>
            GetInternalPropertyDescriptors() const;

    protected:
        JsPersistent m_object;
        int m_handle;
    };
}
