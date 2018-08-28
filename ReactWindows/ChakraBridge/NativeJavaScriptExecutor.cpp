// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "NativeJavaScriptExecutor.h"

const wchar_t* BATCH_BRIDGE = L"__fbBatchedBridge";

using namespace ChakraBridge;

int NativeJavaScriptExecutor::InitializeHost()
{
    return this->host.Init();

    callFunctionMethod = JS_INVALID_REFERENCE;
    invokeCallbackMethod = JS_INVALID_REFERENCE;
    flushQueueMethod = JS_INVALID_REFERENCE;
}

int NativeJavaScriptExecutor::DisposeHost()
{
    SafeReleaseJsValueRef(callFunctionMethod);
    SafeReleaseJsValueRef(invokeCallbackMethod);
    SafeReleaseJsValueRef(flushQueueMethod);

    return this->host.Destroy();
}

int NativeJavaScriptExecutor::SetGlobalVariable(String^ variableName, String^ stringifiedText)
{
    JsValueRef valueStringified;
    IfFailRet(JsPointerToString(stringifiedText->Data(), stringifiedText->Length(), &valueStringified));

    JsValueRef valueJson;
    IfFailRet(this->host.JsonParse(valueStringified, &valueJson));
    IfFailRet(this->host.SetGlobalVariable(variableName->Data(), valueJson));

    return JsNoError;
}

ChakraStringResult NativeJavaScriptExecutor::GetGlobalVariable(String^ variableName)
{
    JsValueRef globalVariable;
    IfFailRetNullPtr(this->host.GetGlobalVariable(variableName->Data(), &globalVariable));

    JsValueRef globalVariableJson;
    IfFailRetNullPtr(this->host.JsonStringify(globalVariable, &globalVariableJson));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(globalVariableJson, &szBuf, &bufLen));

    ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
    return finalResult;
}

int NativeJavaScriptExecutor::RunScript(String^ source, String^ sourceUri)
{
    JsValueRef result;
    IfFailRet(this->host.RunScript(source->Data(), sourceUri->Data(), &result));
    return JsNoError;
}

int NativeJavaScriptExecutor::SerializeScript(String^ source, String^ serialized)
{
    IfFailRet(this->host.SerializeScript(source->Data(), serialized->Data()));
    return JsNoError;
}

int NativeJavaScriptExecutor::RunSerializedScript(String^ source, String^ serialized, String^ sourceUri)
{
    JsValueRef result;
    IfFailRet(this->host.RunSerializedScript(source->Data(), serialized->Data(), sourceUri->Data(), &result));
    return JsNoError;
}

//
// The next three methods do cache JS method references based on the assumption no underlying objects change until the bridge disposing.
//

ChakraStringResult NativeJavaScriptExecutor::CallFunctionAndReturnFlushedQueue(String^ moduleName, String^ methodName, String^ args)
{
    CheckAndGetMethodRef(L"callFunctionReturnFlushedQueue", callFunctionMethod);

    JsValueRef moduleNameRef, methodNameRef;
    IfFailRetNullPtr(JsPointerToString(moduleName->Data(), moduleName->Length(), &moduleNameRef));
    IfFailRetNullPtr(JsPointerToString(methodName->Data(), methodName->Length(), &methodNameRef));

    JsValueRef argObj;
    IfFailRetNullPtr(JsPointerToString(args->Data(), args->Length(), &argObj));

    JsValueRef jsonObj;
    IfFailRetNullPtr(host.JsonParse(argObj, &jsonObj));

    JsValueRef result;
    JsValueRef newArgs[4] = { host.globalObject, moduleNameRef, methodNameRef, jsonObj };
    IfFailRetNullPtr(JsCallFunction(callFunctionMethod, newArgs, 4, &result));

    JsValueRef stringifiedResult;
    IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

    ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
    return finalResult;
}

ChakraStringResult NativeJavaScriptExecutor::InvokeCallbackAndReturnFlushedQueue(int callbackId, String^ args)
{
    CheckAndGetMethodRef(L"invokeCallbackAndReturnFlushedQueue", invokeCallbackMethod);

    JsValueRef callbackIdRef;
    IfFailRetNullPtr(JsIntToNumber(callbackId, &callbackIdRef));

    JsValueRef argsObj;
    IfFailRetNullPtr(JsPointerToString(args->Data(), args->Length(), &argsObj));

    JsValueRef argsJson;
    IfFailRetNullPtr(host.JsonParse(argsObj, &argsJson));

    JsValueRef result;
    JsValueRef newArgs[3] = { host.globalObject, callbackIdRef, argsJson };
    IfFailRetNullPtr(JsCallFunction(invokeCallbackMethod, newArgs, 3, &result));

    JsValueRef stringifiedResult;
    IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

    ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
    return finalResult;
}

ChakraStringResult NativeJavaScriptExecutor::FlushedQueue()
{
    CheckAndGetMethodRef(L"flushedQueue", flushQueueMethod);

    JsValueRef result;
    JsValueRef newArgs[1] = { host.globalObject };
    IfFailRetNullPtr(JsCallFunction(flushQueueMethod, newArgs, 1, &result));

    JsValueRef stringifiedResult;
    IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

    ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
    return finalResult;
}

void NativeJavaScriptExecutor::SetFlushQueueImmediate(FlushQueueImmediateHandler^ handler)
{
    host.SetFlushQueueImmediate(handler);
}

void NativeJavaScriptExecutor::SetCallSyncHook(CallSyncHandler^ handler)
{
    host.SetCallSyncHook(handler);
}

JsErrorCode NativeJavaScriptExecutor::CheckAndGetMethodRef(const wchar_t* methodName, JsValueRef &value)
{
    if (value != JS_INVALID_REFERENCE)
    {
        return JsNoError;
    }

    JsPropertyIdRef fbBridgeId;
    IfFailRet(JsGetPropertyIdFromName(BATCH_BRIDGE, &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRet(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRet(JsGetPropertyIdFromName(methodName, &methodId));

    JsValueRef method;
    IfFailRet(JsGetProperty(fbBridgeObj, methodId, &method));

    IfFailRet(JsAddRef(method, NULL));
    value = method;

    return JsNoError;
}

void NativeJavaScriptExecutor::SafeReleaseJsValueRef(JsValueRef &value)
{
    if (value != JS_INVALID_REFERENCE)
    {
        JsRelease(value, NULL);
        value = JS_INVALID_REFERENCE;
    }
}

