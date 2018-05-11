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
    using websocketpp::lib::thread;
    using websocketpp::lib::unique_lock;

    using websocketpp::log::alevel;
    using websocketpp::log::elevel;

    Service::Service()
        : m_port(0)
    {
        m_server.set_error_channels(elevel::all);
        m_server.set_access_channels(alevel::all ^ alevel::frame_payload);

        m_server.init_asio();
        m_server.set_validate_handler(bind(&Service::OnValidate, this, _1));
        m_server.set_http_handler(bind(&Service::OnHttpRequest, this, _1));
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
        m_port = port;

        try
        {
            m_server.listen("127.0.0.1", std::to_string(m_port));
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
            m_port = 0;

            {
                unique_lock<mutex> lock(m_lock);

                for (const auto& handler : m_handlers)
                {
                    handler.second->Disconnect();
                }
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
        if (connection != nullptr)
        {
            auto resource = connection->get_uri()->get_resource();
            resource.erase(0, 1);

            {
                unique_lock<mutex> lock(m_lock);

                auto handler = m_handlers.find(resource);
                if (handler != m_handlers.end()) {
                    return handler->second->Connect(hdl);
                }
            }
        }

        return false;
    }

    void Service::OnHttpRequest(connection_hdl hdl)
    {
        auto connection = m_server.get_con_from_hdl(hdl);
        if (connection != nullptr)
        {
            auto resource = connection->get_uri()->get_resource();

            if (resource.rfind("/json/protocol") == 0)
            {
                HandleProtocolRequest(hdl);
            }
            else if (resource.rfind("/json/version") == 0)
            {
                HandleVersionRequest(hdl);
            }
            else if (resource.rfind("/json/list") == 0 || resource.rfind("/json") == 0)
            {
                HandleListRequest(hdl);
            }
            else
            {
                connection->set_status(websocketpp::http::status_code::not_found);
            }
        }
    }

    void Service::HandleListRequest(connection_hdl hdl)
    {
        bool first = true;
        std::ostringstream json;
        json << "[";

        {
            unique_lock<mutex> lock(m_lock);

            for (const auto& handler : m_handlers) {
                if (first)
                {
                    first = false;
                }
                else
                {
                    json << ",";
                }

                // TODO: Tweak these values to be more accurate.
                json << " {\n";
                json << "  \"description\": \"ChakraCore instance\",\n";
                json << "  \"devtoolsFrontendUrl\": " <<
                    "\"chrome-devtools://devtools/bundled/inspector.html?experiments=true&v8only=true&ws=localhost:" <<
                    m_port << "/" << handler.second->Id() << "\",\n";
                json << "  \"id\": \"" << handler.second->Id() << "\",\n";
                json << "  \"title\": \"ChakraCore instance\",\n";
                json << "  \"type\": \"node\",\n";
                json << "  \"url\": \"file://\",\n";
                json << "  \"webSocketDebuggerUrl\": \"ws://localhost:" << m_port << "/" << handler.second->Id() << "\"\n";
                json << "}";
            }
        }

        json << " ]";

        SendHttpJsonResponse(hdl, json.str());
    }

    void Service::HandleProtocolRequest(connection_hdl hdl)
    {
        SendHttpJsonResponse(hdl, "{}");
    }

    void Service::HandleVersionRequest(connection_hdl hdl)
    {
        std::ostringstream json;
        json << "{\n";
        json << "  \"Browser\": \"ChakraCore/1.8.3\",\n";
        json << "  \"Protocol-Version\": \"1.2\"\n";
        json << "}";

        // TODO: Can we get the version of ChakraCore?
        SendHttpJsonResponse(hdl, json.str());
    }

    void Service::SendHttpJsonResponse(connection_hdl hdl, const std::string& jsonBody)
    {
        auto connection = m_server.get_con_from_hdl(hdl);
        if (connection != nullptr)
        {
            connection->append_header("Content-Type", "application/json; charset=UTF-8");
            connection->append_header("Cache-Control", "no-cache");
            connection->set_body(jsonBody);
            connection->set_status(websocketpp::http::status_code::ok);
        }
    }
}