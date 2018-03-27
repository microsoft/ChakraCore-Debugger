//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "StringUtil.h"
#include "protocol\Protocol.h"

#include <sstream>

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

namespace JsDebug
{
    namespace protocol
    {
        String StringUtil::substring(const String& s, size_t pos, size_t len)
        {
            return s.substring(pos, len);
        }

        String StringUtil::fromInteger(int number)
        {
            std::ostringstream o;
            o << number;
            return String(o.str().c_str());
        }

        String StringUtil::fromInteger(size_t number)
        {
            std::ostringstream o;
            o << number;
            return String(o.str().c_str());
        }

        String StringUtil::fromDouble(double number)
        {
            std::ostringstream o;
            o << number;
            return String(o.str().c_str());
        }

        double StringUtil::toDouble(const char* s, size_t len, bool* isOk)
        {
            std::istringstream i(std::string(s, len));
            double x;
            *isOk = !!(i >> x);
            return x;
        }

        size_t StringUtil::find(const String& s, const char* needle)
        {
            return s.find(needle);
        }

        size_t StringUtil::find(const String& s, const String& needle)
        {
            return s.find(needle);
        }

        void StringUtil::builderAppend(StringBuilder& builder, const String& s)
        {
            builder.append(s);
        }

        void StringUtil::builderAppend(StringBuilder& builder, UChar c)
        {
            builder.append(c);
        }

        void StringUtil::builderAppend(StringBuilder& builder, const char* s, size_t len)
        {
            builder.append(s, len);
        }

        void StringUtil::builderAppendQuotedString(StringBuilder& builder, const String& s)
        {
            builder.append('"');

            if (!s.empty())
            {
                escapeWideStringForJSON(
                    reinterpret_cast<const uint16_t*>(s.characters16()),
                    static_cast<int>(s.length()), &builder);
            }

            builder.append('"');
        }

        void StringUtil::builderReserve(StringBuilder& builder, size_t len)
        {
            builder.reserve(len);
        }

        String StringUtil::builderToString(StringBuilder& builder)
        {
            return builder.toString();
        }

        std::unique_ptr<protocol::Value> StringUtil::parseJSON(const String16& json)
        {
            if (!json.length())
            {
                return nullptr;
            }

            return parseJSONCharacters(json.characters16(), static_cast<int>(json.length()));
        }

        std::unique_ptr<protocol::Value> StringUtil::parseJSON(const StringView& json)
        {
            if (!json.length())
            {
                return nullptr;
            }

            if (json.is8Bit())
            {
                return parseJSONCharacters(json.characters8(), static_cast<int>(json.length()));
            }

            return parseJSONCharacters(json.characters16(), static_cast<int>(json.length()));
        }
    }
}