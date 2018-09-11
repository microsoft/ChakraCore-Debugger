#pragma once

#include <ChakraDebugProtocolHandler.h>
#include <exception>

class DebugProtocolHandler
{
private:
    JsDebugProtocolHandler m_protocolHandler{ nullptr };

public:
    DebugProtocolHandler(JsRuntimeHandle runtime)
    {
        JsDebugProtocolHandler protocolHandler;

        JsErrorCode result = JsDebugProtocolHandlerCreate(runtime, &protocolHandler);

        if (result != JsNoError)
        {
            throw std::exception("Unable to create debug protocol handler.");
        }

        m_protocolHandler = protocolHandler;
    }

    ~DebugProtocolHandler()
    {
        Destroy();
    }

    JsErrorCode Connect(bool breakOnNextLine, JsDebugProtocolHandlerSendResponseCallback callback, void* callbackState)
    {
        JsErrorCode result = JsDebugProtocolHandlerConnect(m_protocolHandler,
            breakOnNextLine, callback, callbackState);

        return result;
    }

    JsErrorCode Destroy()
    {
        JsErrorCode result = JsNoError;

        if (m_protocolHandler != nullptr)
        {
            result = JsDebugProtocolHandlerDestroy(m_protocolHandler);

            if (result == JsNoError)
            {
                m_protocolHandler = nullptr;
            }
        }

        return result;
    }

    JsErrorCode Disconnect()
    {
        JsErrorCode result = JsDebugProtocolHandlerDisconnect(m_protocolHandler);

        return result;
    }

    JsDebugProtocolHandler GetHandle()
    {
        return m_protocolHandler;
    }

    JsErrorCode WaitForDebugger()
    {
        JsErrorCode result = JsDebugProtocolHandlerWaitForDebugger(m_protocolHandler);

        return result;
    }
};
