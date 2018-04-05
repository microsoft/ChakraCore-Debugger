//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"

#include "Service.h"
#include <iostream>

namespace JsDebug
{
    typedef websocketpp::server<websocketpp::config::asio> server;

    using websocketpp::connection_hdl;

    using websocketpp::lib::bind;
    using websocketpp::lib::mutex;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::thread;
    using websocketpp::lib::unique_lock;

    using websocketpp::log::alevel;
    using websocketpp::log::elevel;

    Service::Service()
    {
        m_server.set_error_channels(elevel::all);
        m_server.set_access_channels(alevel::all ^ alevel::frame_payload);

        m_server.init_asio();
        m_server.set_validate_handler(bind(&Service::OnValidate, this, _1));
        m_server.set_close_handler(bind(&Service::OnClose, this, _1));
    }

    Service::~Service()
    {
        try
        {
            m_server.stop();
        }
        catch (const websocketpp::exception& e)
        {
            // Catch any exceptions thrown during destruction
            std::cerr << "Failed to stop server: " << e.what() << std::endl;
        }
    }

    void Service::RegisterHandler(const char* id, JsDebugProtocolHandler protocolHandler, bool breakOnNextLine)
    {
        unique_lock<mutex> lock(m_lock);
        m_handlers.emplace(id, std::make_unique<ServiceHandler>(&m_server, id, protocolHandler, breakOnNextLine));
    }

    void Service::UnregisterHandler(const char* id)
    {
        unique_lock<mutex> lock(m_lock);
        m_handlers.erase(id);
    }

    void Service::Listen(uint16_t port)
    {
        try
        {
            m_server.listen("127.0.0.1", std::to_string(port));
            m_server.start_accept();
        }
        catch (const websocketpp::exception& e)
        {
            std::cerr << "Failed to start server: " << e.what() << std::endl;
        }

        m_thread = thread(&server::run, &m_server);
    }

    void Service::Close()
    {
        try
        {
            // Stop listening for new connections
            m_server.stop_listening();

            {
                unique_lock<mutex> lock(m_lock);

                for (const auto& conHandle : m_connections)
                {
                    m_server.close(conHandle, websocketpp::close::status::normal, "Server shutting down...");
                }

                m_connections.clear();
            }
        }
        catch (const websocketpp::exception& e)
        {
            std::cerr << "Failed to stop server: " << e.what() << std::endl;
        }

        // Wait for the thread to exit
        if (m_thread.joinable()) {
            m_thread.join();
        }
    }

    bool Service::OnValidate(connection_hdl hdl)
    {
        auto connection = m_server.get_con_from_hdl(hdl);
        if (connection != nullptr) {
            auto resource = connection->get_uri()->get_resource();
            resource.erase(0, 1);

            {
                unique_lock<mutex> lock(m_lock);

                auto handler = m_handlers.find(resource);
                if (handler == m_handlers.end()) {
                    return false;
                }

                if (handler->second->RegisterConnection(hdl))
                {
                    m_connections.insert(hdl);
                    return true;
                }
            }
        }

        return false;
    }

    void Service::OnClose(connection_hdl hdl)
    {
        unique_lock<mutex> lock(m_lock);
        m_connections.erase(hdl);
    }
}