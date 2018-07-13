// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ConsoleImpl.h"

namespace JsDebug
{
    using protocol::Response;

    namespace
    {
        const char c_ErrorNotImplemented[] = "Not implemented";
    }

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
        return Response::OK();
    }

    Response ConsoleImpl::disable()
    {
        return Response::OK();
    }

    Response ConsoleImpl::clearMessages()
    {
        return Response::Error(c_ErrorNotImplemented);
    }
}