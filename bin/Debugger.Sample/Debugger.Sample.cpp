// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "stdafx.h"

//
// Class to store information about command-line arguments to the host.
//
class CommandLineArguments
{
public:
    bool breakOnNextLine;
    bool enableDebugging;
    bool help;
    bool enableConsoleRedirect;
    int port;
    std::vector<std::wstring> scripts;
    std::vector<std::wstring> scriptArgs;

    CommandLineArguments()
        : breakOnNextLine(false)
        , enableDebugging(false)
        , help(false)
        , enableConsoleRedirect(true)
        , port(9229)
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
                    if (index < argc)
                    {
                        // This will return zero if no number was found.
                        this->port = std::stoi(std::wstring(argv[index]));
                    }
                }
                else if (!arg.compare(L"--no-console-redirect"))
                {
                    this->enableConsoleRedirect = false;
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
                this->scriptArgs.emplace_back(std::move(arg));
            }
        }

        if (this->port <= 0 || this->port > 65535 || this->scriptArgs.empty())
        {
            this->help = true;
        }
    }

    void ShowHelp()
    {
        fwprintf(stderr,
            L"\n"
            L"Usage: ChakraCore.Debugger.Sample.exe [host-options] <script> [script-arguments]\n"
            L"\n"
            L"Options: \n"
            L"      --inspect              Enable debugging\n"
            L"      --inspect-brk          Enable debugging and break\n"
            L"      --no-console-redirect  Disable console redirection\n"
            L"  -p, --port <number>        Specify the port number\n"
            L"      --no-console-redirect  Do not send console output to the debugger\n"
            L"  -?  --help                 Show this help info\n"
            L"\n");
    }
};

class ScriptData
{
private:
    std::string m_script;

public:
    ScriptData(std::string const&& script)
    {
        m_script = std::move(script);
    }

    const char* c_str() const
    {
        return m_script.c_str();
    }

    size_t length() const 
    {
        return m_script.length();
    }

    static std::unique_ptr<ScriptData> Create(std::string const&& script)
    {
        return std::make_unique<ScriptData>(std::move(script));
    }

    static void CALLBACK Finalize(void* callbackState)
    {
        auto instance = std::unique_ptr<ScriptData>(reinterpret_cast<ScriptData*>(callbackState));
    }
};

std::wstring ConvertStringFromUtf8ToUtf16(const std::string& string)
{
    std::vector<wchar_t> contents(string.length());

    if (MultiByteToWideChar(CP_UTF8, 0, string.c_str(), static_cast<int>(string.length()), contents.data(), static_cast<int>(contents.size())) == 0)
    {
        const char* message = "chakrahost: unable to convert string from UTF-8 to UTF-16.";
        fprintf(stderr, message);
        fprintf(stderr, "\n");
        throw new std::runtime_error(message);
    }

    return std::wstring(begin(contents), end(contents));
}

unsigned GetNextSourceContext()
{
    static unsigned sourceContext = 0;
    
    return sourceContext++;
}

//
// Helper to load a script from disk.
//

std::string LoadScriptUtf8(const wchar_t* filename)
{
    std::ifstream ifs(filename, std::ifstream::in | std::ifstream::binary);

    if (!ifs.good())
    {
        fwprintf(stderr, L"chakrahost: unable to open file: %s.\n", filename);
        return std::string();
    }

    return std::string(
        (std::istreambuf_iterator<char>(ifs)),
        std::istreambuf_iterator<char>());
}

std::wstring LoadScriptUtf16(const wchar_t* filename)
{
    std::string scriptUtf8 = LoadScriptUtf8(filename);
    return ConvertStringFromUtf8ToUtf16(scriptUtf8);
}

JsErrorCode RunScript(bool loadScriptUsingBuffer, const wchar_t* filename, JsValueRef* result)
{
    wchar_t fullPath[MAX_PATH];
    DWORD pathLength = GetFullPathName(filename, MAX_PATH, fullPath, nullptr);
    if (pathLength > MAX_PATH || pathLength == 0)
    {
        return JsErrorInvalidArgument;
    }

    if (loadScriptUsingBuffer)
    {
        JsValueRef sourceUrl = JS_INVALID_REFERENCE;
        JsValueRef scriptValueRef = JS_INVALID_REFERENCE;

        // Load script from the file
        std::string script = LoadScriptUtf8(fullPath);
        if (script.empty())
        {
            return JsErrorInvalidArgument;
        }
        auto scriptData = ScriptData::Create(std::move(script));
        char* data = const_cast<char*>(scriptData->c_str());
        unsigned int dataLength = static_cast<unsigned int>(scriptData->length());
        JsFinalizeCallback callback = ScriptData::Finalize;
        ScriptData* callbackState = scriptData.release();

        IfFailRet(JsCreateExternalArrayBuffer(data, dataLength, callback, callbackState, &scriptValueRef));
        IfFailRet(JsPointerToString(fullPath, wcslen(fullPath), &sourceUrl));
        IfFailRet(JsRun(scriptValueRef, GetNextSourceContext(), sourceUrl, JsParseScriptAttributeNone, result));
    }
    else
    {
        std::wstring script = LoadScriptUtf16(fullPath);
        if (script.empty())
        {
            return JsErrorInvalidArgument;
        }

        IfFailRet(JsRunScript(script.c_str(), GetNextSourceContext(), fullPath, result));
    }

    return JsNoError;
}

//
// Callback to echo something to the command-line.
//
JsValueRef CALLBACK HostEcho(
    JsValueRef /*callee*/, 
    bool /*isConstructCall*/, 
    JsValueRef* arguments, 
    unsigned short argumentCount, 
    void* /*callbackState*/)
{
    for (unsigned int index = 1; index < argumentCount; index++)
    {
        if (index > 1)
        {
            wprintf(L" ");
        }

        JsValueRef stringValue = JS_INVALID_REFERENCE;
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
// Callback to test handling of exceptions thrown from native code. This isn't a realistic function for a host to
// implement, simply a sample function for testing purposes only.
//
JsValueRef CALLBACK HostThrow(
    JsValueRef /*callee*/,
    bool /*isConstructCall*/,
    JsValueRef* arguments,
    unsigned short argumentCount,
    void* /*callbackState*/)
{
    JsValueRef error = JS_INVALID_REFERENCE;

    if (argumentCount >= 2)
    {
        // Attempt to use the provided object as the error to set.
        error = arguments[1];
    }
    else
    {
        // By default create a sample error object with a message.
        const std::string errStr("Sample error message");
        JsValueRef errorMsg = JS_INVALID_REFERENCE;
        JsCreateString(errStr.c_str(), errStr.length(), &errorMsg);

        JsCreateError(errorMsg, &error);
    }

    JsSetException(error);

    return JS_INVALID_REFERENCE;
}

//
// Callback to load a script and run it.
//
JsValueRef CALLBACK HostRunScript(
    JsValueRef /*callee*/, 
    bool /*isConstructCall*/, 
    JsValueRef* arguments, 
    unsigned short argumentCount, 
    void* /*callbackState*/)
{
    JsValueRef result = JS_INVALID_REFERENCE;

    if (argumentCount < 2)
    {
        ThrowJsError(L"not enough arguments");
        return result;
    }

    // Convert filename.
    const wchar_t* filename = nullptr;
    size_t length = 0;

    IfFailThrowJsError(JsStringToPointer(arguments[1], &filename, &length), L"invalid filename argument");
    IfFailThrowJsError(RunScript(false, filename, &result), L"failed to run script");

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
    // Get property ID.
    JsPropertyIdRef propertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(callbackName, &propertyId));

    // Create a function
    JsValueRef function = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateFunction(callback, callbackState, &function));

    // Set the property
    IfFailRet(JsSetProperty(globalObject, propertyId, function, true));

    return JsNoError;
}

JsErrorCode RedirectConsoleToDebugger(DebugProtocolHandler *handler)
{
    // Get global.console object. If that is not defined just return, otherwise get the debugger.console and patch with global.console object
    JsValueRef globalObject = JS_INVALID_REFERENCE;
    IfFailRet(JsGetGlobalObject(&globalObject));

    JsPropertyIdRef consolePropertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"console", &consolePropertyId));

    JsValueRef consoleObject = JS_INVALID_REFERENCE;
    IfFailRet(JsGetProperty(globalObject, consolePropertyId, &consoleObject));

    JsValueRef undefinedValue = JS_INVALID_REFERENCE;
    IfFailRet(JsGetUndefinedValue(&undefinedValue));

    if (consoleObject == JS_INVALID_REFERENCE || consoleObject == undefinedValue)
    {
        return JsNoError;
    }

    JsValueRef debuggerConsoleObject = JS_INVALID_REFERENCE;
    IfFailRet(handler->GetConsoleObject(&debuggerConsoleObject));

    std::string script = "";

    script = script + "function patchConsoleObject$$1(global, console, debugConsole) {\n";
    script = script + "var obj = {};\n";
    script = script + "for (var fn in console) {\n";
    script = script + "if (typeof console[fn] === \"function\") {\n";
    script = script + "(function(name) {\n";
    script = script + "obj[name] = function(...rest) {\n";
    script = script + "console[name](rest);\n";
    script = script + "if (name in debugConsole && typeof debugConsole[name] === \"function\") {\n";
    script = script + "debugConsole[name](rest);\n";
    script = script + "}\n";
    script = script + "}\n";
    script = script + "})(fn);\n";
    script = script + "}\n";
    script = script + "}\n";
    script = script + "global.console = obj;\n";
    script = script + "}\n";
    script = script + "patchConsoleObject$$1;\n";

    JsValueRef patchFunction = JS_INVALID_REFERENCE;

    JsValueRef scriptUrl = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateString("", 0, &scriptUrl));

    JsValueRef scriptContentValue = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateString(script.c_str(), script.length(), &scriptContentValue));
    IfFailRet(JsRun(scriptContentValue, JS_SOURCE_CONTEXT_NONE, scriptUrl, JsParseScriptAttributeLibraryCode, &patchFunction));

    JsValueRef args[4] = { undefinedValue, globalObject, consoleObject, debuggerConsoleObject };
    IfFailRet(JsCallFunction(patchFunction, args, _countof(args), nullptr /*no return value*/));

    return JsNoError;
}

//
// Creates a host execution context and sets up the host object in it.
//
JsErrorCode CreateHostContext(JsRuntimeHandle runtime, std::vector<std::wstring>& scriptArgs, JsContextRef* context)
{
    // Create the context.
    IfFailRet(JsCreateContext(runtime, context));

    // Now set the execution context as being the current one on this thread.
    IfFailRet(JsSetCurrentContext(*context));

    // Create the host object the script will use.
    JsValueRef hostObject = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateObject(&hostObject));

    // Get the global object
    JsValueRef globalObject = JS_INVALID_REFERENCE;
    IfFailRet(JsGetGlobalObject(&globalObject));

    // Set the global object property name
    JsPropertyIdRef globalPropertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"global", &globalPropertyId));
    IfFailRet(JsSetProperty(globalObject, globalPropertyId, globalObject, true));

    JsValueRef consoleObject = JS_INVALID_REFERENCE;
    IfFailRet(JsCreateObject(&consoleObject));

    // Get the name of the property ("host") that we're going to set on the global object.
    JsPropertyIdRef hostPropertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"host", &hostPropertyId));

    JsPropertyIdRef consolePropertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"console", &consolePropertyId));

    // Set the property.
    IfFailRet(JsSetProperty(globalObject, hostPropertyId, hostObject, true));
    IfFailRet(JsSetProperty(globalObject, consolePropertyId, consoleObject, true));

    // Now create the host callbacks that we're going to expose to the script.
    IfFailRet(DefineHostCallback(hostObject, L"echo", HostEcho, nullptr));
    IfFailRet(DefineHostCallback(hostObject, L"runScript", HostRunScript, nullptr));
    IfFailRet(DefineHostCallback(hostObject, L"throw", HostThrow, nullptr));

    IfFailRet(DefineHostCallback(consoleObject, L"debug", HostEcho, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"error", HostEcho, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"info", HostEcho, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"log", HostEcho, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"warn", HostEcho, nullptr));

    // Create an array for arguments.
    JsValueRef arguments = JS_INVALID_REFERENCE;
    unsigned int argCount = static_cast<unsigned int>(scriptArgs.size());
    IfFailRet(JsCreateArray(argCount, &arguments));

    for (unsigned int index = 0; index < argCount; index++)
    {
        // Create the argument value.
        std::wstring& str = scriptArgs[index];

        JsValueRef argument = JS_INVALID_REFERENCE;
        IfFailRet(JsPointerToString(str.c_str(), str.length(), &argument));

        // Create the index.
        JsValueRef indexValue = JS_INVALID_REFERENCE;
        IfFailRet(JsIntToNumber(index, &indexValue));

        // Set the value.
        IfFailRet(JsSetIndexedProperty(arguments, indexValue, argument));
    }

    // Get the name of the property that we're going to set on the host object.
    JsPropertyIdRef argumentsPropertyId = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"arguments", &argumentsPropertyId));

    // Set the arguments property.
    IfFailRet(JsSetProperty(hostObject, argumentsPropertyId, arguments, true));

    // Clean up the current execution context.
    IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));

    return JsNoError;
}

//
// Print out a script exception.
//
JsErrorCode PrintScriptException()
{
    // Get script exception.
    JsValueRef exception = JS_INVALID_REFERENCE;
    IfFailRet(JsGetAndClearException(&exception));

    // Get message.
    JsPropertyIdRef messageName = JS_INVALID_REFERENCE;
    IfFailRet(JsGetPropertyIdFromName(L"message", &messageName));

    JsValueRef messageValue = JS_INVALID_REFERENCE;
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
    uint16_t port, 
    std::unique_ptr<DebugProtocolHandler>& debugProtocolHandler,
    std::unique_ptr<DebugService>& debugService)
{
    JsErrorCode result = JsNoError;
    auto protocolHandler = std::make_unique<DebugProtocolHandler>(runtime);
    auto service = std::make_unique<DebugService>();

    result = service->RegisterHandler(runtimeName, *protocolHandler, breakOnNextLine);

    if (result == JsNoError)
    {
        result = service->Listen(port);

        std::cout << "Listening on ws://127.0.0.1:" << port << "/" << runtimeName << std::endl;
    }

    if (result == JsNoError)
    {
        debugProtocolHandler = std::move(protocolHandler);
        debugService = std::move(service);
    }

    return result;
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
        std::unique_ptr<DebugProtocolHandler> debugProtocolHandler;
        std::unique_ptr<DebugService> debugService;
        std::string runtimeName("runtime1");

        // Create the runtime. We're only going to use one runtime for this host.
        IfFailError(
            JsCreateRuntime(JsRuntimeAttributeDispatchSetExceptionsToDebugger, nullptr, &runtime),
            L"failed to create runtime.");

        if (arguments.enableDebugging)
        {
            IfFailError(
                EnableDebugging(runtime, runtimeName, arguments.breakOnNextLine, static_cast<uint16_t>(arguments.port), debugProtocolHandler, debugService),
                L"failed to enable debugging.");
        }

        // Similarly, create a single execution context. Note that we're putting it on the stack here,
        // so it will stay alive through the entire run.
        IfFailError(CreateHostContext(runtime, arguments.scriptArgs, &context), L"failed to create execution context.");

        // Now set the execution context as being the current one on this thread.
        IfFailError(JsSetCurrentContext(context), L"failed to set current context.");

        IfFailError(RedirectConsoleToDebugger(debugProtocolHandler.get()), L"Failed to redirect to debugger's console");

        if (arguments.enableConsoleRedirect && arguments.enableDebugging)
        {
            IfFailError(RedirectConsoleToDebugger(debugProtocolHandler.get()), L"Failed to redirect console to debugger.");
        }

        if (debugProtocolHandler && arguments.breakOnNextLine)
        {
            std::cout << "Waiting for debugger to connect..." << std::endl;
            IfFailError(debugProtocolHandler->WaitForDebugger(), L"failed to wait for debugger");
            std::cout << "Debugger connected" << std::endl;
        }

        if (arguments.scriptArgs.size() > 0)
        {
            arguments.scripts.emplace_back(arguments.scriptArgs[0]);
        }

        // for each script
        for (unsigned int index = 0; index < arguments.scripts.size(); ++index)
        {
            // Run the script.
            const std::wstring& script = arguments.scripts[index];
            JsValueRef result = JS_INVALID_REFERENCE;
            JsErrorCode errorCode = RunScript(arguments.loadScriptUsingBuffer, script.c_str(), &result);

            if (errorCode == JsErrorScriptException)
            {
                IfFailError(PrintScriptException(), L"failed to print exception");
                return EXIT_FAILURE;
            }
            else
            {
                IfFailError(errorCode, L"failed to run script.");
            }

            // Convert the return value.
            JsValueRef numberResult = JS_INVALID_REFERENCE;
            double doubleResult;
            IfFailError(JsConvertValueToNumber(result, &numberResult), L"failed to convert return value.");
            IfFailError(JsNumberToDouble(numberResult, &doubleResult), L"failed to convert return value.");
            returnValue = (int) doubleResult;
            std::cout << returnValue << std::endl;
        }

        // Clean up the current execution context.
        IfFailError(JsSetCurrentContext(JS_INVALID_REFERENCE), L"failed to cleanup current context.");
        context = JS_INVALID_REFERENCE;

        if (debugService)
        {
            IfFailError(debugService->Close(), L"failed to close service");
            IfFailError(debugService->UnregisterHandler(runtimeName), L"failed to unregister handler");
            IfFailError(debugService->Destroy(), L"failed to destroy service");
        }

        if (debugProtocolHandler)
        {
            IfFailError(debugProtocolHandler->Destroy(), L"failed to destroy handler");
        }

        // Clean up the runtime.
        IfFailError(JsDisposeRuntime(runtime), L"failed to cleanup runtime.");
    }
    catch (...)
    {
        fwprintf(stderr, L"chakrahost: fatal error: internal error.\n");
    }

error:
    return returnValue;
}
