//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "SchemaImpl.h"

namespace JsDebug
{
    SchemaImpl::SchemaImpl()
    {
    }

    SchemaImpl::~SchemaImpl()
    {
    }

    Response SchemaImpl::getDomains(std::unique_ptr<protocol::Array<protocol::Schema::Domain>>* out_domains)
    {
        return Response();
    }
}