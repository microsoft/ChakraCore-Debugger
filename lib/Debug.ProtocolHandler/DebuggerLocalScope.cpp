//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerLocalScope.h"

#include "PropertyHelpers.h"
#include "ProtocolHelpers.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Runtime::InternalPropertyDescriptor;
    using protocol::Runtime::PropertyDescriptor;

    DebuggerLocalScope::DebuggerLocalScope(JsValueRef stackProperties)
        : DebuggerObject(stackProperties)
    {
    }

    std::unique_ptr<Array<PropertyDescriptor>> DebuggerLocalScope::GetPropertyDescriptors() const
    {
        auto propertyDescriptors = Array<PropertyDescriptor>::create();

        JsValueRef arguments = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(m_object.Get(), PropertyHelpers::Names::Arguments, &arguments))
        {
            propertyDescriptors->addItem(ProtocolHelpers::WrapProperty(arguments));
        }

        JsValueRef functionCallsReturn = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(
                m_object.Get(),
                PropertyHelpers::Names::FunctionCallsReturn,
                &functionCallsReturn))
        {
            int length = PropertyHelpers::GetPropertyInt(functionCallsReturn, PropertyHelpers::Names::Length);

            for (int index = 0; index < length; index++)
            {
                JsValueRef prop = PropertyHelpers::GetIndexedProperty(functionCallsReturn, index);
                propertyDescriptors->addItem(ProtocolHelpers::WrapProperty(prop));
            }
        }

        JsValueRef locals = JS_INVALID_REFERENCE;
        if (PropertyHelpers::TryGetProperty(m_object.Get(), PropertyHelpers::Names::Locals, &locals))
        {
            int length = PropertyHelpers::GetPropertyInt(locals, PropertyHelpers::Names::Length);

            for (int index = 0; index < length; index++)
            {
                JsValueRef prop = PropertyHelpers::GetIndexedProperty(locals, index);
                propertyDescriptors->addItem(ProtocolHelpers::WrapProperty(prop));
            }
        }

        return propertyDescriptors;
    }

    std::unique_ptr<Array<InternalPropertyDescriptor>> DebuggerLocalScope::GetInternalPropertyDescriptors() const
    {
        return Array<InternalPropertyDescriptor>::create();
    }
}
