//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Schema.h>
#include <protocol\Forward.h>

namespace JsDebug
{
    using protocol::Response;

    class SchemaImpl : public protocol::Schema::Backend
    {
    public:
        SchemaImpl();
        ~SchemaImpl() override;

        // protocol::Schema::Backend implementation
        Response getDomains(std::unique_ptr<protocol::Array<protocol::Schema::Domain>>* out_domains) override;
    };
}
