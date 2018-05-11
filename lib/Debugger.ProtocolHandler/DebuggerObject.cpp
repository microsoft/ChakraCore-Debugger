//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "DebuggerObject.h"

#include "PropertyHelpers.h"
#include "ProtocolHelpers.h"

namespace JsDebug
{
    using protocol::Array;
    using protocol::Runtime::InternalPropertyDescriptor;
    using protocol::Runtime::PropertyDescriptor;

    namespace
    {
        const int c_MaxPropertyCount = 5000;
    }

    DebuggerObject::DebuggerObject(JsValueRef obj)
        : m_object(obj)
        , m_handle(-1)
    {
        int handle = 0;
        if (PropertyHelpers::TryGetProperty(m_object.Get(), PropertyHelpers::Names::Handle, &handle))
        {
            m_handle = handle;
        }
    }

    std::unique_ptr<Array<PropertyDescriptor>> DebuggerObject::GetPropertyDescriptors() const
    {
        auto propertyDescriptors = Array<PropertyDescriptor>::create();

        if (m_handle >= 0)
        {
            JsValueRef diagProperties = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsDiagGetProperties(m_handle, 0, c_MaxPropertyCount, &diagProperties));

            JsValueRef properties = PropertyHelpers::GetProperty(diagProperties, PropertyHelpers::Names::Properties);
            int length = PropertyHelpers::GetPropertyInt(properties, PropertyHelpers::Names::Length);

            for (int index = 0; index < length; index++)
            {
                JsValueRef prop = PropertyHelpers::GetIndexedProperty(properties, index);
                propertyDescriptors->addItem(ProtocolHelpers::WrapProperty(prop));
            }
        }

        return propertyDescriptors;
    }

    std::unique_ptr<Array<InternalPropertyDescriptor>> DebuggerObject::GetInternalPropertyDescriptors() const
    {
        auto propertyDescriptors = Array<InternalPropertyDescriptor>::create();

        if (m_handle >= 0)
        {
            JsValueRef diagProperties = JS_INVALID_REFERENCE;
            IfJsErrorThrow(JsDiagGetProperties(m_handle, 0, c_MaxPropertyCount, &diagProperties));

            JsValueRef properties = PropertyHelpers::GetProperty(
                diagProperties,
                PropertyHelpers::Names::DebuggerOnlyProperties);
            int length = PropertyHelpers::GetPropertyInt(properties, PropertyHelpers::Names::Length);

            for (int index = 0; index < length; index++)
            {
                JsValueRef prop = PropertyHelpers::GetIndexedProperty(properties, index);
                propertyDescriptors->addItem(ProtocolHelpers::WrapInternalProperty(prop));
            }
        }

        return propertyDescriptors;
    }
}
