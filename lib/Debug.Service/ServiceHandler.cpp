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
        : m_server(server)
        , m_id(id)
        , m_protocolHandler(protocolHandler)
        , m_breakOnNextLine(breakOnNextLine)
    {
        JsDebugProtocolHandlerConnect(
            m_protocolHandler,
            m_breakOnNextLine,
            &ServiceHandler::SendResponseCallback,
            this);
    }

    ServiceHandler::~ServiceHandler()
    {
        JsDebugProtocolHandlerDisconnect(m_protocolHandler);
    }

    bool ServiceHandler::RegisterConnection(connection_hdl hdl)
    {
        if (!m_hdl.expired()) {
            return false;
        }

        auto connection = m_server->get_con_from_hdl(hdl);
        connection->set_message_handler(bind(&ServiceHandler::OnMessage, this, _1, _2));

        m_hdl = hdl;
        return true;
    }

    void ServiceHandler::SendResponseCallback(const char* response, void* callbackState)
    {
        auto serviceHandler = static_cast<ServiceHandler*>(callbackState);
        serviceHandler->SendResponse(response);
    }

    void ServiceHandler::SendResponse(const char* response)
    {
        if (m_hdl.expired()) {
            return;
        }

        m_server->send(m_hdl, std::string(response), websocketpp::frame::opcode::text);
    }

    void ServiceHandler::OnMessage(connection_hdl hdl, server::message_ptr msg)
    {
        JsDebugProtocolHandlerSendCommand(m_protocolHandler, msg->get_payload().c_str());
    }
}
