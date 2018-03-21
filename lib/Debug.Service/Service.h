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

#include <ChakraDebugProtocolHandler.h>

#include <map>
#include <set>

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
        void Run();
        void OnMessage(
            websocketpp::connection_hdl hdl,
            websocketpp::server<websocketpp::config::asio>::message_ptr msg);
        bool OnValidate(websocketpp::connection_hdl hdl);
        void OnFail(websocketpp::connection_hdl hdl);
        void OnClose(websocketpp::connection_hdl hdl);

        typedef std::set<websocketpp::connection_hdl, std::owner_less<websocketpp::connection_hdl>> con_list;
        typedef std::map<std::string, JsDebugProtocolHandler> handler_map;

        // Although access to the server object is thread-safe, access to all other objects is not. The lock must be
        // taken before accessing any class members from either thread.
        websocketpp::server<websocketpp::config::asio> m_server;
        websocketpp::lib::thread m_thread;
        websocketpp::lib::mutex m_lock;

        con_list m_connections;
        handler_map m_handlers;
    };
}
