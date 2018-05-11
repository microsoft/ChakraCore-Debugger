// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef JsDebug_protocol_Schema_api_h
#define JsDebug_protocol_Schema_api_h

#include "Common.h"

namespace JsDebug {
namespace protocol {
namespace Schema {
namespace API {

// ------------- Enums.

// ------------- Types.

class LIB_EXPORT Domain {
public:
    virtual std::unique_ptr<StringBuffer> toJSONString() const = 0;
    virtual ~Domain() { }
    static std::unique_ptr<protocol::Schema::API::Domain> fromJSONString(const StringView& json);
};

} // namespace API
} // namespace Schema
} // namespace JsDebug
} // namespace protocol

#endif // !defined(JsDebug_protocol_Schema_api_h)
