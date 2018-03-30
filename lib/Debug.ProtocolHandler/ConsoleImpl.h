//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#include <protocol\Console.h>
#include <protocol\Forward.h>

namespace JsDebug
{
    using protocol::Response;

    class ProtocolHandler;

    class ConsoleImpl : public protocol::Console::Backend
    {
    public:
        ConsoleImpl(ProtocolHandler* handler, protocol::FrontendChannel* frontendChannel);
        ~ConsoleImpl() override;
        ConsoleImpl(const ConsoleImpl&) = delete;
        ConsoleImpl& operator=(const ConsoleImpl&) = delete;

        // protocol::Console::Backend implementation
        Response enable() override;
        Response disable() override;
        Response clearMessages() override;

    private:
        ProtocolHandler* m_handler;
        protocol::Console::Frontend m_frontend;
    };
}
