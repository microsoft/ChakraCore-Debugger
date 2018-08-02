// This file is generated

// Copyright (c) 2016 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef JsDebug_protocol_Debugger_api_h
#define JsDebug_protocol_Debugger_api_h

#include "Common.h"

namespace JsDebug {
namespace protocol {
namespace Debugger {
namespace API {

// ------------- Enums.

namespace Paused {
namespace ReasonEnum {
LIB_EXPORT extern const char* XHR;
LIB_EXPORT extern const char* DOM;
LIB_EXPORT extern const char* EventListener;
LIB_EXPORT extern const char* Exception;
LIB_EXPORT extern const char* Assert;
LIB_EXPORT extern const char* DebugCommand;
LIB_EXPORT extern const char* PromiseRejection;
LIB_EXPORT extern const char* OOM;
LIB_EXPORT extern const char* Other;
LIB_EXPORT extern const char* Ambiguous;
} // ReasonEnum
} // Paused

// ------------- Types.

class LIB_EXPORT SearchMatch {
public:
    virtual std::unique_ptr<StringBuffer> toJSONString() const = 0;
    virtual ~SearchMatch() { }
    static std::unique_ptr<protocol::Debugger::API::SearchMatch> fromJSONString(const StringView& json);
};

} // namespace API
} // namespace Debugger
} // namespace JsDebug
} // namespace protocol

#endif // !defined(JsDebug_protocol_Debugger_api_h)
