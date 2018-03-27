//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ConsoleImpl.h"

namespace JsDebug
{
    ConsoleImpl::ConsoleImpl(ProtocolHandler* handler)
        : m_handler(handler)
    {
    }

    ConsoleImpl::~ConsoleImpl()
    {
    }

    Response ConsoleImpl::enable()
    {
        return Response();
    }

    Response ConsoleImpl::disable()
    {
        return Response();
    }

    Response ConsoleImpl::clearMessages()
    {
        return Response();
    }
}