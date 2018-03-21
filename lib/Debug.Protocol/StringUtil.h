//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "Common.h"

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
            static String substring(const String& s, size_t pos, size_t len)
            {
                return String();
            }

            static String fromInteger(int number)
            {
                return String();
            }

            static String fromInteger(size_t number)
            {
                return String();
            }

            static String fromDouble(double number)
            {
                return String();
            }

            static double toDouble(const char *s, size_t len, bool* isOk)
            {
                return 0;
            }

            static size_t find(const String& s, const char* needle)
            {
                return -1;
            }

            static size_t find(const String& s, const String& needle)
            {
                return -1;
            }

            static void builderAppend(StringBuilder& builder, const String& s)
            {
            }

            static void builderAppend(StringBuilder& builder, UChar c)
            {
            }

            static void builderAppend(StringBuilder& builder, const char *s, size_t len)
            {
            }

            static void builderAppendQuotedString(StringBuilder& builder, const String& s)
            {
            }

            static void builderReserve(StringBuilder& builder, size_t len)
            {
            }

            static String builderToString(StringBuilder& builder)
            {
                return String();
            }

            static std::unique_ptr<protocol::Value> parseJSON(const String16& json)
            {
                return nullptr;
            }

            static std::unique_ptr<protocol::Value> parseJSON(const StringView& json)
            {
                return nullptr;
            }
        };

    }

    class StringBufferImpl : public StringBuffer
    {
    public:
        static std::unique_ptr<StringBufferImpl> adopt(String16& s)
        {
            return nullptr;
        }

    private:
        explicit StringBufferImpl(String16& s)
        {
        }
    };
}
