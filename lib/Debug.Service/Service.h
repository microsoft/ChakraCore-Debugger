//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#pragma once

#pragma warning( push )
#pragma warning( disable : 4267 4996 )
#define ASIO_STANDALONE
#define _WEBSOCKETPP_CPP11_TYPE_TRAITS_
#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#pragma warning( pop )

#include <map>
#include <set>

#include <ChakraDebugProtocolHandler.h>
#include "ServiceHandler.h"

namespace JsDebug
{
    class Service
    {
    public:
        Service();
        ~Service();

        void RegisterHandler(const char* id, JsDebugProtocolHandler protocolHandler, bool breakOnNextLine);
        void UnregisterHandler(const char* id);

        void Listen(uint16_t port);
        void Close();

    private:
        bool OnValidate(websocketpp::connection_hdl hdl);
        void OnHttpRequest(websocketpp::connection_hdl hdl);

        void HandleListRequest(websocketpp::connection_hdl hdl);
        void HandleProtocolRequest(websocketpp::connection_hdl hdl);
        void HandleVersionRequest(websocketpp::connection_hdl hdl);

        void SendHttpJsonResponse(websocketpp::connection_hdl hdl, const std::string& jsonBody);

        typedef std::map<std::string, std::unique_ptr<ServiceHandler>> handler_map;

        // Although access to the server object is thread-safe, access to all other objects is not. The lock must be
        // taken before accessing any class members from either thread.
        websocketpp::server<websocketpp::config::asio> m_server;
        websocketpp::lib::thread m_thread;
        websocketpp::lib::mutex m_lock;

        uint16_t m_port;
        handler_map m_handlers;
    };
}
