//---------------------------------------------------------------------------------------------------
// Copyright (C) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.
//---------------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "ErrorHelpers.h"

namespace JsDebug
{
    JsErrorException::JsErrorException(JsErrorCode error)
        : std::runtime_error(MakeMessage(error))
    {
    }

    std::string JsErrorException::MakeMessage(JsErrorCode error)
    {
        std::string message = TranslateError(error);
        message.append(": ");
        message.append(std::to_string(error));

        return message;
    }

    std::string JsErrorException::TranslateError(JsErrorCode error)
    {
        //
        // Look for an exact match
        //

        switch (error)
        {
        case JsNoError:
            return "Success error code.";

        case JsErrorInvalidArgument:
            return "An argument to a hosting API was invalid.";

        case JsErrorNullArgument:
            return "An argument to a hosting API was null in a context where null is not allowed.";

        case JsErrorNoCurrentContext:
            return "The hosting API requires that a context be current, but there is no current context.";

        case JsErrorInExceptionState:
            return "The engine is in an exception state and no APIs can be called until the exception is cleared.";

        case JsErrorNotImplemented:
            return "A hosting API is not yet implemented.";

        case JsErrorWrongThread:
            return "A hosting API was called on the wrong thread.";

        case JsErrorRuntimeInUse:
            return "A runtime that is still in use cannot be disposed.";

        case JsErrorBadSerializedScript:
            return "A bad serialized script was used, or the serialized script was serialized by a different version of the Chakra engine.";

        case JsErrorInDisabledState:
            return "The runtime is in a disabled state.";

        case JsErrorCannotDisableExecution:
            return "Runtime does not support reliable script interruption.";

        case JsErrorHeapEnumInProgress:
            return "A heap enumeration is currently underway in the script context.";

        case JsErrorArgumentNotObject:
            return "A hosting API that operates on object values was called with a non-object value.";

        case JsErrorInProfileCallback:
            return "A script context is in the middle of a profile callback.";

        case JsErrorInThreadServiceCallback:
            return "A thread service callback is currently underway.";

        case JsErrorCannotSerializeDebugScript:
            return "Scripts cannot be serialized in debug contexts.";

        case JsErrorAlreadyDebuggingContext:
            return "The context cannot be put into a debug state because it is already in a debug state.";

        case JsErrorAlreadyProfilingContext:
            return "The context cannot start profiling because it is already profiling.";

        case JsErrorIdleNotEnabled:
            return "Idle notification given when the host did not enable idle processing.";

        case JsCannotSetProjectionEnqueueCallback:
            return "The context did not accept the enqueue callback.";

        case JsErrorCannotStartProjection:
            return "Failed to start projection.";

        case JsErrorInObjectBeforeCollectCallback:
            return "The operation is not supported in an object before collect callback.";

        case JsErrorObjectNotInspectable:
            return "Object cannot be unwrapped to IInspectable pointer.";

        case JsErrorPropertyNotSymbol:
            return "A hosting API that operates on symbol property ids but was called with a non-symbol property id. The error code is returned by JsGetSymbolFromPropertyId if the function is called with non-symbol property id.";

        case JsErrorPropertyNotString:
            return "A hosting API that operates on string property ids but was called with a non-string property id. The error code is returned by existing JsGetPropertyNamefromId if the function is called with non-string property id.";

        case JsErrorInvalidContext:
            return "Module evaluation is called in wrong context.";

        case JsInvalidModuleHostInfoKind:
            return "Module evaluation is called in wrong context.";

        case JsErrorModuleParsed:
            return "Module was parsed already when JsParseModuleSource is called.";

        case JsNoWeakRefRequired:
            return "Argument passed to JsCreateWeakReference is a primitive that is not managed by the GC. No weak reference is required, the value will never be collected.";

        case JsErrorOutOfMemory:
            return "The Chakra engine has run out of memory.";

        case JsErrorBadFPUState:
            return "The Chakra engine failed to set the Floating Point Unit state.";

        case JsErrorScriptException:
            return "A JavaScript exception occurred while running a script.";

        case JsErrorScriptCompile:
            return "JavaScript failed to compile.";

        case JsErrorScriptTerminated:
            return "A script was terminated due to a request to suspend a runtime.";

        case JsErrorScriptEvalDisabled:
            return "A script was terminated because it tried to use eval or function and eval was disabled.";

        case JsErrorFatal:
            return "A fatal error in the engine has occurred.";

        case JsErrorWrongRuntime:
            return "A hosting API was called with object created on different javascript runtime.";

        case JsErrorDiagAlreadyInDebugMode:
            return "The object for which the debugging API was called was not found";

        case JsErrorDiagNotInDebugMode:
            return "The debugging API can only be called when VM is in debug mode";

        case JsErrorDiagNotAtBreak:
            return "The debugging API can only be called when VM is at a break";

        case JsErrorDiagInvalidHandle:
            return "Debugging API was called with an invalid handle.";

        case JsErrorDiagObjectNotFound:
            return "The object for which the debugging API was called was not found";

        case JsErrorDiagUnableToPerformAction:
            return "VM was unable to perform the request action";
        }

        //
        // Try finding a category match
        //

        if ((error & JsErrorCategoryUsage) == JsErrorCategoryUsage)
        {
            return "Category of errors that relates to incorrect usage of the API itself.";
        }

        if ((error & JsErrorCategoryEngine) == JsErrorCategoryEngine)
        {
            return "Category of errors that relates to errors occurring within the engine itself.";
        }

        if ((error & JsErrorCategoryScript) == JsErrorCategoryScript)
        {
            return "Category of errors that relates to errors in a script.";
        }

        if ((error & JsErrorCategoryFatal) == JsErrorCategoryFatal)
        {
            return "Category of errors that are fatal and signify failure of the engine.";
        }

        if ((error & JsErrorCategoryDiagError) == JsErrorCategoryDiagError)
        {
            return "Category of errors that are related to failures during diagnostic operations.";
        }

        //
        // Couldn't find a match
        //

        return "Unknown error";
    }
}
