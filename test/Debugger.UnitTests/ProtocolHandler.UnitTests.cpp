// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

#define CATCH_CONFIG_MAIN
#include <catch.hpp>

#include <ChakraDebugProtocolHandler.h>
#include <ChakraCore.h>

class JsrtTestFixture
{
public:
    JsrtTestFixture()
        : runtime(nullptr)
        , context(JS_INVALID_REFERENCE)
    {
        REQUIRE(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &this->runtime) == JsNoError);
        REQUIRE(JsCreateContext(this->runtime, &this->context) == JsNoError);
        REQUIRE(JsSetCurrentContext(this->context) == JsNoError);
    }

    ~JsrtTestFixture()
    {
        REQUIRE(JsSetCurrentContext(nullptr) == JsNoError);
        REQUIRE(JsDisposeRuntime(this->runtime) == JsNoError);
    }

    JsErrorCode RunScript(const std::string& scriptName, const std::string& scriptContent, JsValueRef* result)
    {
        JsValueRef scriptNameValue = JS_INVALID_REFERENCE;
        REQUIRE(JsCreateString(scriptName.c_str(), scriptName.length(), &scriptNameValue) == JsNoError);

        JsValueRef scriptContentValue = JS_INVALID_REFERENCE;
        REQUIRE(JsCreateString(scriptContent.c_str(), scriptContent.length(), &scriptContentValue) == JsNoError);

        return JsRun(scriptContentValue, 0, scriptNameValue, JsParseScriptAttributeNone, result);
    }

protected:
    JsRuntimeHandle GetRuntime()
    {
        return this->runtime;
    }

    JsContextRef GetContext()
    {
        return this->context;
    }

private:
    JsRuntimeHandle runtime;
    JsContextRef context;
};

class JsrtDebugTestFixture : public JsrtTestFixture
{
public:
    JsrtDebugTestFixture()
        : JsrtTestFixture()
        , protocolHandler(nullptr)
    {
        JsDebugProtocolHandlerCreate(this->GetRuntime(), &this->protocolHandler);
    }

    ~JsrtDebugTestFixture()
    {
        REQUIRE(JsDebugProtocolHandlerDestroy(this->protocolHandler) == JsNoError);
    }

protected:
    JsDebugProtocolHandler GetProtocolHandler()
    {
        return this->protocolHandler;
    }

private:
    JsDebugProtocolHandler protocolHandler;
};

void ValidateResponses(const std::vector<std::string>& expected, const std::vector<std::string>& actual)
{
    REQUIRE(expected.size() == actual.size());
    
    for (size_t i = 0; i < actual.size(); ++i)
    {
        REQUIRE(expected[i] == actual[i]);
    }
}

TEST_CASE_METHOD(JsrtTestFixture, "JsDebugProtocolHandler Create")
{
    CHECK(JsDebugProtocolHandlerCreate(this->GetRuntime(), nullptr) == JsErrorInvalidArgument);

    JsDebugProtocolHandler handler = nullptr;
    REQUIRE(JsDebugProtocolHandlerCreate(nullptr, &handler) == JsErrorInvalidArgument);
    REQUIRE(handler == nullptr);
}

TEST_CASE_METHOD(JsrtTestFixture, "JsDebugProtocolHandler Destroy")
{
    CHECK(JsDebugProtocolHandlerDestroy(nullptr) == JsErrorInvalidArgument);

    JsDebugProtocolHandler handler = nullptr;
    REQUIRE(JsDebugProtocolHandlerCreate(this->GetRuntime(), &handler) == JsNoError);
    REQUIRE(JsDebugProtocolHandlerDestroy(handler) == JsNoError);
}

TEST_CASE_METHOD(JsrtDebugTestFixture, "JsDebugProtocolHandler Connect/Disconnect")
{
    auto callback = [](const char* /*response*/, void* /*callbackState*/) {};

    CHECK(JsDebugProtocolHandlerConnect(nullptr, false, callback, nullptr) == JsErrorInvalidArgument);
    CHECK(JsDebugProtocolHandlerConnect(this->GetProtocolHandler(), false, nullptr, nullptr) == JsErrorInvalidArgument);
    
    REQUIRE(JsDebugProtocolHandlerConnect(this->GetProtocolHandler(), false, callback, nullptr) == JsNoError);
    CHECK(JsDebugProtocolHandlerConnect(this->GetProtocolHandler(), false, callback, nullptr) == JsErrorFatal);
    
    CHECK(JsDebugProtocolHandlerDisconnect(nullptr) == JsErrorInvalidArgument);
    REQUIRE(JsDebugProtocolHandlerDisconnect(this->GetProtocolHandler()) == JsNoError);
    CHECK(JsDebugProtocolHandlerDisconnect(this->GetProtocolHandler()) == JsErrorFatal);
}

TEST_CASE_METHOD(JsrtDebugTestFixture, "JsDebugProtocolHandler SendMessage")
{
    std::vector<std::string> expectedResponses
    {
        "{\"error\":{\"code\":-32700,\"message\":\"Message must be a valid JSON\"}}",
        "{\"error\":{\"code\":-32700,\"message\":\"Message must be a valid JSON\"}}",
        "{\"error\":{\"code\":-32600,\"message\":\"Message must have integer 'id' property\"}}",
        "{\"error\":{\"code\":-32600,\"message\":\"Message must have string 'method' property\"},\"id\":0}",
        "{\"error\":{\"code\":-32601,\"message\":\"'Foo.bar' wasn't found\"},\"id\":1}",
        "{\"id\":2,\"result\":{\"domains\":[{\"name\":\"Console\",\"version\":\"1.2\"},{\"name\":\"Debugger\",\"version\":\"1.2\"},{\"name\":\"Runtime\",\"version\":\"1.2\"}]}}",
    };

    std::vector<std::string> actualResponses;
    auto callback = [](const char* response, void* callbackState)
    {
        auto responses = static_cast<std::vector<std::string>*>(callbackState);
        responses->emplace_back(response);
    };

    REQUIRE(JsDebugProtocolHandlerConnect(this->GetProtocolHandler(), false, callback, &actualResponses) == JsNoError);

    // Parameter validation
    REQUIRE(JsDebugProtocolHandlerSendCommand(nullptr, "") == JsErrorInvalidArgument);
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), nullptr) == JsErrorInvalidArgument);

    // Payload error cases
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "") == JsNoError);
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "Hi!") == JsNoError);
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "{\"foo\":\"bar\"}") == JsNoError);
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "{\"id\":0}") == JsNoError);
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "{\"id\":1,\"method\":\"Foo.bar\"}") == JsNoError);

    // Valid payload cases
    REQUIRE(JsDebugProtocolHandlerSendCommand(this->GetProtocolHandler(), "{\"id\":2,\"method\":\"Schema.getDomains\"}") == JsNoError);

    JsValueRef result = JS_INVALID_REFERENCE;
    REQUIRE(this->RunScript("test.js", "var i = 0;", &result) == JsNoError);
    
    ValidateResponses(expectedResponses, actualResponses);

    REQUIRE(JsDebugProtocolHandlerDisconnect(this->GetProtocolHandler()) == JsNoError);
}
