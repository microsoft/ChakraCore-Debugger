// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "SchemaImpl.h"

#include "ProtocolHandler.h"

namespace JsDebug
{
    using protocol::Response;

    namespace
    {
        const char c_ErrorNotImplemented[] = "Not implemented";
    }

    SchemaImpl::SchemaImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel)
        : m_handler(handler)
        , m_frontend(frontendChannel)
    {
    }

    SchemaImpl::~SchemaImpl()
    {
    }

    Response SchemaImpl::getDomains(std::unique_ptr<protocol::Array<protocol::Schema::Domain>>* out_domains)
    {
        *out_domains = m_handler->GetSupportedDomains();
        return Response::OK();
    }
}