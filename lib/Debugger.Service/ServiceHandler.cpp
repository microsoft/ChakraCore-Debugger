#include "stdafx.h"
#include "ServiceHandler.h"

namespace JsDebug
{
    typedef websocketpp::server<websocketpp::config::asio> server;

    using websocketpp::connection_hdl;

    using websocketpp::lib::bind;
    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;

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
                connection->close(websocketpp::close::status::going_away, "Server shutting down...");
            }
        }
        catch (const websocketpp::exception& e)
        {
            // Catch any exceptions thrown during destruction
            std::cerr << "Failed to close connection: " << e.what() << std::endl;
        }

        if (m_connected)
        {
            JsDebugProtocolHandlerDisconnect(m_protocolHandler);
        }
    }

    bool ServiceHandler::Connect(connection_hdl hdl)
    {
        if (m_hdl.expired())
        {
            auto connection = m_server->get_con_from_hdl(hdl);
            connection->set_message_handler(bind(&ServiceHandler::OnMessage, this, _1, _2));
            connection->set_close_handler(bind(&ServiceHandler::OnClose, this, _1));

            JsDebugProtocolHandlerConnect(
                m_protocolHandler,
                m_breakOnNextLine,
                &ServiceHandler::SendResponseCallback,
                this);

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
            m_server->close(m_hdl, websocketpp::close::status::going_away, "Server shutting down...");
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
        JsDebugProtocolHandlerSendCommand(m_protocolHandler, msg->get_payload().c_str());
    }

    void ServiceHandler::OnClose(connection_hdl hdl)
    {
        if (m_connected)
        {
            JsDebugProtocolHandlerDisconnect(m_protocolHandler);
            m_connected = false;
        }
    }
}
