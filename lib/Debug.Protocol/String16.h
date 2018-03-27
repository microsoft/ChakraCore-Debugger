//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <string>
#include <vector>

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

namespace JsDebug
{
    using UChar = uint16_t;

    class String16
    {
    public:
        static const size_t kNotFound = std::basic_string<UChar>::npos;

        String16();
        String16(const UChar* str, size_t length);
        String16(const char* str);
        String16(const char* str, size_t length);
        explicit String16(const std::basic_string<UChar>& impl);

        String16 operator+(const String16& other) const;
        bool operator==(const String16& other) const;

        const UChar* characters16() const;
        size_t length() const;
        bool empty() const;
        size_t hash() const;

        size_t find(String16 str) const;
        String16 substring(size_t pos, size_t len) const;

    private:
        std::basic_string<UChar> m_impl;
    };

    inline String16 operator+(const char* a, const String16& b)
    {
        return String16(a) + b;
    }

    class String16Builder
    {
    public:
        String16Builder();
        void append(const String16& s);
        void append(UChar c);
        void append(const char* s, size_t len);
        void reserve(size_t len);
        String16 toString();

    private:
        std::vector<UChar> m_buffer;
    };
}

namespace std
{
    template <>
    struct hash<JsDebug::String16>
    {
        size_t operator()(const JsDebug::String16& string) const
        {
            return string.hash();
        }
    };
}