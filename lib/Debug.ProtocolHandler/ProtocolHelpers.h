//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <ChakraCore.h>
#include <protocol/Debugger.h>
#include <protocol/Runtime.h>
#include <memory>

namespace JsDebug
{
    namespace ProtocolHelpers
    {
        protocol::String GetObjectId(int handle);
        std::unique_ptr<protocol::Runtime::RemoteObject> WrapObject(JsValueRef object);
        std::unique_ptr<protocol::Runtime::RemoteObject> WrapException(JsValueRef exception);
        std::unique_ptr<protocol::Runtime::PropertyDescriptor> WrapProperty(JsValueRef property);
        std::unique_ptr<protocol::Runtime::InternalPropertyDescriptor> WrapInternalProperty(JsValueRef property);
        std::unique_ptr<protocol::Debugger::Location> WrapLocation(JsValueRef exception);
        std::unique_ptr<protocol::Runtime::RemoteObject> GetUndefinedObject();
    }
}
