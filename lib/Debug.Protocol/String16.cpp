//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "String16.h"

#include <sstream>

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

namespace JsDebug
{
    String16::String16()
    {
    }

    String16::String16(const UChar* str, size_t length)
        : m_impl(str, length)
    {
    }

    String16::String16(const char* str)
        : String16(str, std::strlen(str))
    {
    }

    String16::String16(const char* str, size_t length)
    {
        m_impl.resize(length);
        for (size_t i = 0; i < length; i++)
        {
            m_impl[i] = str[i];
        }
    }

    String16::String16(const std::basic_string<UChar>& impl)
        : m_impl(impl)
    {
    }

    String16 String16::operator+(const String16& other) const
    {
        return String16(m_impl + other.m_impl);
    }

    String16& String16::operator+=(const String16& other)
    {
        m_impl += other.m_impl;

        return *this;
    }

    bool String16::operator==(const String16& other) const
    {
        return m_impl == other.m_impl;
    }

    bool String16::operator!=(const String16& other) const
    {
        return m_impl != other.m_impl;
    }

    String16 String16::fromInteger(int number)
    {
        std::ostringstream o;
        o << number;
        return String16(o.str().c_str());
    }

    String16 String16::fromInteger(size_t number)
    {
        std::ostringstream o;
        o << number;
        return String16(o.str().c_str());
    }

    String16 String16::fromDouble(double number)
    {
        std::ostringstream o;
        o << number;
        return String16(o.str().c_str());
    }

    const UChar* String16::characters16() const
    {
        return m_impl.c_str();
    }

    size_t String16::length() const
    {
        return m_impl.length();
    }

    bool String16::empty() const
    {
        return m_impl.empty();
    }

    size_t String16::hash() const
    {
        std::hash<std::basic_string<UChar>> hash;
        return hash(m_impl);
    }

    size_t String16::find(String16 str) const
    {
        return m_impl.find(str.m_impl);
    }

    String16 String16::substring(size_t pos, size_t len) const
    {
        return String16(m_impl.substr(pos, len));
    }

    std::string String16::toAscii() const
    {
        const UChar* chars = m_impl.c_str();
        size_t len = m_impl.length();

        std::string retVal;
        retVal.reserve(len);

        for (size_t i = 0; i < len; i++)
        {
            // Make sure the character contains only ASCII character codes
            if ((chars[i] & 0x7f) != chars[i])
            {
                throw std::runtime_error("String contains invalid ASCII characters");
            }

            retVal.push_back(static_cast<char>(chars[i]));
        }

        return retVal;
    }

    int String16::toInteger() const
    {
        std::istringstream i(toAscii());

        int x = 0;

        // Make sure a valid integer is found and that the stream contains no more characters.
        if (!(i >> x) || !i.eof())
        {
            throw std::runtime_error("String doesn't not contain only a valid integer");
        }

        return x;
    }

    String16Builder::String16Builder()
    {
    }

    void String16Builder::append(const String16& s)
    {
        m_buffer.insert(m_buffer.end(), s.characters16(), s.characters16() + s.length());
    }

    void String16Builder::append(UChar c)
    {
        m_buffer.push_back(c);
    }

    void String16Builder::append(const char* s, size_t len)
    {
        m_buffer.insert(m_buffer.end(), s, s + len);
    }

    void String16Builder::reserve(size_t len)
    {
        m_buffer.reserve(len);
    }

    String16 String16Builder::toString()
    {
        return String16(m_buffer.data(), m_buffer.size());
    }
}
