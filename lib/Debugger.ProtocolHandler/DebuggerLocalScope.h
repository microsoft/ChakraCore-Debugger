// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "DebuggerObject.h"

namespace JsDebug
{
    class DebuggerLocalScope : public DebuggerObject
    {
    public:
        explicit DebuggerLocalScope(JsValueRef stackProperties);

        std::unique_ptr<protocol::Array<protocol::Runtime::PropertyDescriptor>>
            GetPropertyDescriptors() const override;
        std::unique_ptr<protocol::Array<protocol::Runtime::InternalPropertyDescriptor>>
            GetInternalPropertyDescriptors() const override;
    };
}
