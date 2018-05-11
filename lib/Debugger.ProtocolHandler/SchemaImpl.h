//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Schema.h>
#include <protocol\Forward.h>

namespace JsDebug
{
    class ProtocolHandler;

    class SchemaImpl : public protocol::Schema::Backend
    {
    public:
        SchemaImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel);
        ~SchemaImpl() override;
        SchemaImpl(const SchemaImpl&) = delete;
        SchemaImpl& operator=(const SchemaImpl&) = delete;

        // protocol::Schema::Backend implementation
        protocol::Response getDomains(std::unique_ptr<protocol::Array<protocol::Schema::Domain>>* out_domains) override;

    private:
        ProtocolHandler* m_handler;
        protocol::Schema::Frontend m_frontend;
    };
}
