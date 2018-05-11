//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "Common.h"

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

namespace JsDebug
{
    StringView::StringView()
        : m_is8Bit(true)
        , m_length(0)
        , m_characters8(nullptr)
    {
    }

    StringView::StringView(const uint8_t* characters, size_t length)
        : m_is8Bit(true)
        , m_length(length)
        , m_characters8(characters)
    {
    }

    StringView::StringView(const uint16_t* characters, size_t length)
        : m_is8Bit(false)
        , m_length(length)
        , m_characters16(characters)
    {
    }

    bool StringView::is8Bit() const
    {
        return m_is8Bit;
    }

    size_t StringView::length() const
    {
        return m_length;
    }

    const uint8_t* StringView::characters8() const
    {
        return m_characters8;
    }

    const uint16_t* StringView::characters16() const
    {
        return m_characters16;
    }
}
