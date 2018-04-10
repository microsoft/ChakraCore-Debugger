//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

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
