//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include "ErrorHelpers.h"
#include <ChakraCore.h>

namespace JsDebug
{
    template<class Func>
    JsErrorCode TranslateExceptionToJsErrorCode(Func func)
    {
        JsErrorCode err = JsNoError;

        try
        {
            func();
        }
        catch (const JsErrorException& e)
        {
            err = e.code();
        }
        catch (...)
        {
            err = JsErrorFatal;
        }

        return err;
    }

    template<class Class, class Handle, class Func>
    JsErrorCode TranslateExceptionToJsErrorCode(Handle handle, Func func)
    {
        if (handle == nullptr)
        {
            return JsErrorInvalidArgument;
        }

        auto obj = reinterpret_cast<Class>(handle);

        return TranslateExceptionToJsErrorCode([&]() -> void {
            func(obj);
        });
    }
}
