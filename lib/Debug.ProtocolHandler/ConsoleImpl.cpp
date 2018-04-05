//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ConsoleImpl.h"

namespace JsDebug
{
    using protocol::Response;

    ConsoleImpl::ConsoleImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel)
        : m_handler(handler)
        , m_frontend(frontendChannel)
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