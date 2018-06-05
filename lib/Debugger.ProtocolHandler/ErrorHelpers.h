// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <ChakraCore.h>
#include <string>

namespace JsDebug
{
    class JsErrorException
        : public std::runtime_error
    {
    public:
        explicit JsErrorException(JsErrorCode error);
        JsErrorException(JsErrorCode error, const std::string& message);

        JsErrorCode code() const;

    private:
        static std::string MakeMessage(JsErrorCode error);

        /// <summary>
        /// Uses error messages from the enum comments in ChakraCommon.h to create friendlier error messages.
        /// </summary>
        static std::string TranslateError(JsErrorCode error);

        JsErrorCode m_code;
    };

    inline void IfJsErrorThrow(JsErrorCode error)
    {
        if (error != JsNoError)
        {
            throw JsErrorException(error);
        }
    }
}
