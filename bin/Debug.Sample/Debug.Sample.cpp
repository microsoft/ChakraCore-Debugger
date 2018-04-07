//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"

//
// Class to store information about command-line arguments to the host.
//

class CommandLineArguments
{
public:
    bool breakOnNextLine;
    bool enableDebugging;
    int port;
    bool help;

    std::vector<std::wstring> args;

    CommandLineArguments()
        : breakOnNextLine(false)
        , enableDebugging(false)
        , port(9229)
        , help(false)
    {
    }

    void ParseCommandLine(int argc, wchar_t* argv[])
    {
        bool foundScript = false;

        for (int index = 1; index < argc; ++index)
        {
            std::wstring arg(argv[index]);

            // Any flags before the script are considered host flags, anything else is passed to the script.
            if (!foundScript && (arg.length() > 0) && (arg[0] == L'-'))
            {
                if (!arg.compare(L"--inspect"))
                {
                    this->enableDebugging = true;
                }
                else if (!arg.compare(L"--inspect-brk"))
                {
                    this->enableDebugging = true;
                    this->breakOnNextLine = true;
                }
                else if (!arg.compare(L"--port") || !arg.compare(L"-p"))
                {
                    ++index;
                    if (argc > index)
                    {
                        // This will return zero if no number was found.
                        this->port = std::stoi(std::wstring(argv[index]));
                    }
                }
                else
                {
                    // Handle everything else including `-?` and `--help`
                    this->help = true;
                }
            }
            else
            {
                foundScript = true;

                // Collect any non-flag arguments
                args.emplace_back(std::move(arg));
            }
        }

        if (this->port <= 0 || this->port > 65535 || args.empty())
        {
            this->help = true;
        }
    }

    void ShowHelp()
    {
        fwprintf(stderr,
            L"\n"
            L"Usage: Debug.Sample.exe [host-options] <script> [script-arguments]\n"
            L"\n"
            L"Options: \n"
            L"      --inspect          Enable debugging\n"
            L"      --inspect-brk      Enable debugging and break\n"
            L"  -p, --port <number>    Specify the port number\n"
            L"  -?  --help             Show this help info\n"
            L"\n");
    }
};

//
// Source context counter.
//

unsigned currentSourceContext = 0;

//
// This "throws" an exception in the Chakra space. Useful routine for callbacks
// that need to throw a JS error to indicate failure.
//

void ThrowJsError(std::wstring errorString)
{
    // We ignore error since we're already in an error state.
    JsValueRef errorValue;
    JsValueRef errorObject;
    JsPointerToString(errorString.c_str(), errorString.length(), &errorValue);
    JsCreateError(errorValue, &errorObject);
    JsSetException(errorObject);
}

//
// Helper to load a script from disk.
//

std::wstring LoadScript(const std::wstring& fileName)
{
    FILE* file;
    if (_wfopen_s(&file, fileName.c_str(), L"rb"))
    {
        fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", fileName.c_str());
        return std::wstring();
    }

    unsigned int current = ftell(file);
    fseek(file, 0, SEEK_END);
    unsigned int end = ftell(file);
    fseek(file, current, SEEK_SET);
    unsigned int lengthBytes = end - current;
    char* rawBytes = (char*)calloc(lengthBytes + 1, sizeof(char));

    if (rawBytes == nullptr)
    {
        fwprintf(stderr, L"chakrahost: fatal error.\n");
        return std::wstring();
    }

    fread((void*)rawBytes, sizeof(char), lengthBytes, file);

    wchar_t* contents = (wchar_t*)calloc(lengthBytes + 1, sizeof(wchar_t));
    if (contents == nullptr)
    {
        free(rawBytes);
        fwprintf(stderr, L"chakrahost: fatal error.\n");
        return std::wstring();
    }

    if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
    {
        free(rawBytes);
        free(contents);
        fwprintf(stderr, L"chakrahost: fatal error.\n");
        return std::wstring();
    }

    std::wstring result = contents;
    free(rawBytes);
    free(contents);
    return result;
}

//
// Callback to echo something to the command-line.
//

JsValueRef CALLBACK Echo(
    JsValueRef callee, 
    bool isConstructCall, 
    JsValueRef* arguments, 
    unsigned short argumentCount, 
    void* callbackState)
{
    for (unsigned int index = 1; index < argumentCount; index++)
    {
        if (index > 1)
        {
            wprintf(L" ");
        }

        JsValueRef stringValue;
        IfFailThrowJsError(JsConvertValueToString(arguments[index], &stringValue), L"invalid argument");

        const wchar_t* string;
        size_t length;
        IfFailThrowJsError(JsStringToPointer(stringValue, &string, &length), L"invalid argument");

        wprintf(L"%s", string);
    }

    wprintf(L"\n");

    return JS_INVALID_REFERENCE;
}

//
// Callback to load a script and run it.
//

JsValueRef CALLBACK RunScript(
    JsValueRef callee, 
    bool isConstructCall, 
    JsValueRef* arguments, 
    unsigned short argumentCount, 
    void* callbackState)
{
    JsValueRef result = JS_INVALID_REFERENCE;

    if (argumentCount < 2)
    {
        ThrowJsError(L"not enough arguments");
        return result;
    }

    //
    // Convert filename.
    //
    const wchar_t* filename;
    size_t length;

    IfFailThrowJsError(JsStringToPointer(arguments[1], &filename, &length), L"invalid filename argument");

    //
    // Load the script from the disk.
    //

    std::wstring script = LoadScript(filename);
    if (script.empty())
    {
        ThrowJsError(L"invalid script");
        return result;
    }

    //
    // Run the script.
    //

    IfFailThrowJsError(JsRunScript(script.c_str(), currentSourceContext++, filename, &result), L"failed to run script.");

    return result;
}

//
// Helper to define a host callback method on the global host object.
//

JsErrorCode DefineHostCallback(
    JsValueRef globalObject, 
    const wchar_t* callbackName, 
    JsNativeFunction callback, 
    void* callbackState)
{
    //
    // Get property ID.
    //

    JsPropertyIdRef propertyId;
    IfFailRet(JsGetPropertyIdFromName(callbackName, &propertyId));

    //
    // Create a function
    //

    JsValueRef function;
    IfFailRet(JsCreateFunction(callback, callbackState, &function));

    //
    // Set the property
    //

    IfFailRet(JsSetProperty(globalObject, propertyId, function, true));

    return JsNoError;
}

//
// Creates a host execution context and sets up the host object in it.
//

JsErrorCode CreateHostContext(JsRuntimeHandle runtime, std::vector<std::wstring>& args, JsContextRef* context)
{
    //
    // Create the context.
    //

    IfFailRet(JsCreateContext(runtime, context));

    //
    // Now set the execution context as being the current one on this thread.
    //

    IfFailRet(JsSetCurrentContext(*context));

    //
    // Create the host object the script will use.
    //

    JsValueRef hostObject;
    IfFailRet(JsCreateObject(&hostObject));

    //
    // Get the global object
    //

    JsValueRef globalObject;
    IfFailRet(JsGetGlobalObject(&globalObject));

    //
    // Get the name of the property ("host") that we're going to set on the global object.
    //

    JsPropertyIdRef hostPropertyId;
    IfFailRet(JsGetPropertyIdFromName(L"host", &hostPropertyId));

    //
    // Set the property.
    //

    IfFailRet(JsSetProperty(globalObject, hostPropertyId, hostObject, true));

    //
    // Now create the host callbacks that we're going to expose to the script.
    //

    IfFailRet(DefineHostCallback(hostObject, L"echo", Echo, nullptr));
    IfFailRet(DefineHostCallback(hostObject, L"runScript", RunScript, nullptr));

    //
    // Create an array for arguments.
    //

    JsValueRef arguments = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateArray(static_cast<unsigned int>(args.size()), &arguments));

    for (int index = 0; index < args.size(); index++)
    {
        //
        // Create the argument value.
        //

        std::wstring& str = args[index];

        JsValueRef argument;
        IfFailRet(JsPointerToString(str.c_str(), str.length(), &argument));

        //
        // Create the index.
        //

        JsValueRef indexValue;
        IfFailRet(JsIntToNumber(index, &indexValue));

        //
        // Set the value.
        //

        IfFailRet(JsSetIndexedProperty(arguments, indexValue, argument));
    }

    //
    // Get the name of the property that we're going to set on the host object.
    //

    JsPropertyIdRef argumentsPropertyId;
    IfFailRet(JsGetPropertyIdFromName(L"arguments", &argumentsPropertyId));

    //
    // Set the arguments property.
    //

    IfFailRet(JsSetProperty(hostObject, argumentsPropertyId, arguments, true));

    //
    // Clean up the current execution context.
    //

    IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));

    return JsNoError;
}

//
// Print out a script exception.
//

JsErrorCode PrintScriptException()
{
    //
    // Get script exception.
    //

    JsValueRef exception;
    IfFailRet(JsGetAndClearException(&exception));

    //
    // Get message.
    //

    JsPropertyIdRef messageName;
    IfFailRet(JsGetPropertyIdFromName(L"message", &messageName));

    JsValueRef messageValue;
    IfFailRet(JsGetProperty(exception, messageName, &messageValue));

    const wchar_t* message;
    size_t length;
    IfFailRet(JsStringToPointer(messageValue, &message, &length));

    fwprintf(stderr, L"chakrahost: exception: %s\n", message);

    return JsNoError;
}

JsErrorCode EnableDebugging(
    JsRuntimeHandle runtime,
    std::string const& runtimeName,
    bool breakOnNextLine, 
    int port, 
    JsDebugProtocolHandler* pProtocolHandler, 
    JsDebugService* pService)
{
    JsDebugProtocolHandler protocolHandler = nullptr;
    JsDebugService service = nullptr;

    IfFailRet(JsDebugProtocolHandlerCreate(runtime, &protocolHandler));
    IfFailRet(JsDebugServiceCreate(&service));
    IfFailRet(JsDebugServiceRegisterHandler(service, runtimeName.c_str(), protocolHandler, breakOnNextLine));
    IfFailRet(JsDebugServiceListen(service, port));

    std::cout << "Listening on ws://127.0.0.1:" << port << "/" << runtimeName << std::endl;

    *pProtocolHandler = protocolHandler;
    *pService = service;

    return JsNoError;
}

//
// The main entry point for the host.
//

int _cdecl wmain(int argc, wchar_t* argv[])
{
    int returnValue = EXIT_FAILURE;
    CommandLineArguments arguments;

    arguments.ParseCommandLine(argc, argv);

    if (arguments.help)
    {
        arguments.ShowHelp();
        return returnValue;
    }

    try
    {
        JsRuntimeHandle runtime = JS_INVALID_RUNTIME_HANDLE;
        JsContextRef context = JS_INVALID_REFERENCE;
        JsDebugProtocolHandler protocolHandler = nullptr;
        JsDebugService service = nullptr;

        //
        // Create the runtime. We're only going to use one runtime for this host.
        //

        IfFailError(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime), L"failed to create runtime.");

        if (arguments.enableDebugging)
        {
            IfFailError(
                EnableDebugging(runtime, "runtime1", arguments.breakOnNextLine, arguments.port, &protocolHandler, &service),
                L"failed to enable debugging.");
        }

        //
        // Similarly, create a single execution context. Note that we're putting it on the stack here,
        // so it will stay alive through the entire run.
        //

        IfFailError(CreateHostContext(runtime, arguments.args, &context), L"failed to create execution context.");

        //
        // Now set the execution context as being the current one on this thread.
        //

        IfFailError(JsSetCurrentContext(context), L"failed to set current context.");

        //
        // Load the script from the disk.
        //
        const std::wstring& scriptName = arguments.args[0];
        std::wstring script = LoadScript(scriptName);
        if (script.empty())
        {
            goto error;
        }

        if (protocolHandler)
        {
            std::cout << "Waiting for debugger to connect..." << std::endl;
            IfFailError(JsDebugProtocolHandlerWaitForDebugger(protocolHandler), L"failed to wait for debugger");
            std::cout << "Debugger connected" << std::endl;
        }

        //
        // Run the script.
        //

        JsValueRef result = JS_INVALID_REFERENCE;
        JsErrorCode errorCode = JsRunScript(script.c_str(), currentSourceContext++, scriptName.c_str(), &result);

        if (errorCode == JsErrorScriptException)
        {
            IfFailError(PrintScriptException(), L"failed to print exception");
            return EXIT_FAILURE;
        }
        else
        {
            IfFailError(errorCode, L"failed to run script.");
        }

        //
        // Convert the return value.
        //

        JsValueRef numberResult;
        double doubleResult;
        IfFailError(JsConvertValueToNumber(result, &numberResult), L"failed to convert return value.");
        IfFailError(JsNumberToDouble(numberResult, &doubleResult), L"failed to convert return value.");
        returnValue = (int)doubleResult;
        std::cout << returnValue << std::endl;

        if (service)
        {
            IfFailError(JsDebugServiceClose(service), L"failed to close service");
            IfFailError(JsDebugServiceUnregisterHandler(service, "runtime1"), L"failed to unregister handler");
            IfFailError(JsDebugServiceDestroy(service), L"failed to destroy service");
            service = nullptr;
        }

        if (protocolHandler)
        {
            IfFailError(JsDebugProtocolHandlerDestroy(protocolHandler), L"failed to destroy handler");
            protocolHandler = nullptr;
        }

        //
        // Clean up the current execution context.
        //

        IfFailError(JsSetCurrentContext(JS_INVALID_REFERENCE), L"failed to cleanup current context.");

        //
        // Clean up the runtime.
        //

        IfFailError(JsDisposeRuntime(runtime), L"failed to cleanup runtime.");
    }
    catch (...)
    {
        fwprintf(stderr, L"chakrahost: fatal error: internal error.\n");
    }

error:
    return returnValue;
}
