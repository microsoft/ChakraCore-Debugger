//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "Common.h"

#include <cassert>
#include <memory>

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

namespace JsDebug
{
    namespace protocol
    {
        class Value;

        using String = String16;
        using StringBuilder = String16Builder;

        class StringUtil
        {
        public:
            static const size_t kNotFound = String::kNotFound;

            static String substring(const String& s, size_t pos, size_t len);

            static String fromInteger(int number);
            static String fromInteger(size_t number);
            static String fromDouble(double number);
            static double toDouble(const char* s, size_t len, bool* isOk);

            static size_t find(const String& s, const char* needle);
            static size_t find(const String& s, const String& needle);

            static void builderAppend(StringBuilder& builder, const String& s);
            static void builderAppend(StringBuilder& builder, UChar c);
            static void builderAppend(StringBuilder& builder, const char* s, size_t len);
            static void builderAppendQuotedString(StringBuilder& builder, const String& s);
            static void builderReserve(StringBuilder& builder, size_t len);
            static String builderToString(StringBuilder& builder);

            static std::unique_ptr<protocol::Value> parseJSON(const String16& json);
            static std::unique_ptr<protocol::Value> parseJSON(const StringView& json);
        };

    }

    class StringBufferImpl : public StringBuffer
    {
    public:
        static std::unique_ptr<StringBufferImpl> adopt(String16& s)
        {
            // TODO: Implement this when it gets hit.
            assert(false);
            return nullptr;
        }

    private:
        explicit StringBufferImpl(String16& s)
        {
            // TODO: Implement this when it gets hit.
            assert(false);
        }
    };
}
