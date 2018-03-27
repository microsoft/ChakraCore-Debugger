//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

//
// This file contains interfaces required by the `inspector_protocol` generated code.
//

#define CHECK(condition)
#define DCHECK(condition)
#define DCHECK_LT(lhs, rhs)
#define LIB_EXPORT

#include "String16.h"

namespace JsDebug
{
    class StringView
    {
    public:
        StringView();
        StringView(const uint8_t* characters, size_t length);
        StringView(const uint16_t* characters, size_t length);

        bool is8Bit() const;
        size_t length() const;
        const uint8_t* characters8() const;
        const uint16_t* characters16() const;

    private:
        bool m_is8Bit;
        size_t m_length;
        union {
            const uint8_t* m_characters8;
            const uint16_t* m_characters16;
        };
    };

    class StringBuffer
    {
    };
}
