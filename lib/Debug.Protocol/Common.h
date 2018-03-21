//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <map>
#include <memory>

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

#define CHECK(condition)
#define DCHECK(condition)
#define DCHECK_LT(lhs, rhs)
#define LIB_EXPORT

namespace JsDebug
{
    using UChar = uint16_t;

    class String16
    {
    public:
        static const size_t kNotFound = static_cast<size_t>(-1);

        String16()
        {
        }

        String16(const char *str)
        {
        }

        String16 operator+(const String16& other) const
        {
            return String16();
        }

        bool operator==(const String16& other) const
        {
            return false;
        }

        size_t length()
        {
            return 0;
        }

        std::size_t hash() const
        {
            return 0;
        }
    };

    inline String16 operator+(const char* a, const String16& b)
    {
        return String16(a) + b;
    }

    class String16Builder
    {
    };

    class StringBuffer
    {
    };

    class StringView
    {
    };
}

namespace std
{
    template <>
    struct hash<JsDebug::String16>
    {
        std::size_t operator()(const JsDebug::String16& string) const
        {
            return string.hash();
        }
    };
}
