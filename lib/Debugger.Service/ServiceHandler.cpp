// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"
#include "ServiceHandler.h"

namespace JsDebug
{
    typedef websocketpp::server<websocketpp::config::asio> server;

    using websocketpp::connection_hdl;

    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

    namespace
    {
        const char c_MessageServerShutdown[] = "Server shutting down...";
    }

    ServiceHandler::ServiceHandler(
        server* server,
        const char* id,
        JsDebugProtocolHandler protocolHandler,
        bool breakOnNextLine)
        : m_connected(false)
        , m_server(server)
        , m_id(id)
        , m_protocolHandler(protocolHandler)
        , m_breakOnNextLine(breakOnNextLine)
    {
    }

    ServiceHandler::~ServiceHandler()
    {
        try
        {
            if (!m_hdl.expired())
            {
                auto connection = m_server->get_con_from_hdl(m_hdl);
                connection->set_message_handler(nullptr);
                connection->set_close_handler(nullptr);
                connection->close(websocketpp::close::status::going_away, c_MessageServerShutdown);
            }
        }
        catch (...)
        {
            // Don't allow the exception to propagate.
        }

        if (m_connected)
        {
            // Ignore any returned error codes
            JsErrorCode err = JsDebugProtocolHandlerDisconnect(m_protocolHandler);
            assert(err == JsNoError);
        }
    }

    bool ServiceHandler::Connect(connection_hdl hdl)
    {
        if (m_hdl.expired())
        {
            auto connection = m_server->get_con_from_hdl(hdl);
            connection->set_message_handler(bind(&ServiceHandler::OnMessage, this, _1, _2));
            connection->set_close_handler(bind(&ServiceHandler::OnClose, this, _1));

            if (JsDebugProtocolHandlerConnect(
                m_protocolHandler,
                m_breakOnNextLine,
                &ServiceHandler::SendResponseCallback,
                this) != JsNoError)
            {
                return false;
            }

            m_connected = true;
            m_hdl = hdl;

            return true;
        }

        return false;
    }

    void ServiceHandler::Disconnect()
    {
        if (!m_hdl.expired())
        {
            m_server->close(m_hdl, websocketpp::close::status::going_away, c_MessageServerShutdown);
        }
    }

    std::string ServiceHandler::Id()
    {
        return m_id;
    }

    void ServiceHandler::SendResponseCallback(const char* response, void* callbackState)
    {
        auto serviceHandler = static_cast<ServiceHandler*>(callbackState);
        serviceHandler->SendResponse(response);
    }

    void ServiceHandler::SendResponse(const char* response)
    {
        if (!m_hdl.expired())
        {
            m_server->send(m_hdl, std::string(response), websocketpp::frame::opcode::text);
        }
    }

    void ServiceHandler::OnMessage(connection_hdl hdl, server::message_ptr msg)
    {
        // Ignore any returned error codes
        JsErrorCode err = JsDebugProtocolHandlerSendCommand(m_protocolHandler, msg->get_payload().c_str());
        assert(err == JsNoError);
    }

    void ServiceHandler::OnClose(connection_hdl hdl)
    {
        if (m_connected)
        {
            // Ignore any returned error codes
            JsErrorCode err = JsDebugProtocolHandlerDisconnect(m_protocolHandler);
            assert(err == JsNoError);

            m_connected = false;
        }
    }
}
