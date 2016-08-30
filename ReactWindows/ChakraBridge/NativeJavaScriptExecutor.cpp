#include "pch.h"
#include "NativeJavaScriptExecutor.h"

using namespace ChakraBridge;

int NativeJavaScriptExecutor::InitializeHost()
{
    return this->host.Init();
}

int NativeJavaScriptExecutor::DisposeHost()
{
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

int NativeJavaScriptExecutor::RunScriptFromFile(String^ sourceFilePath, String^ sourceUri)
{
    JsValueRef result;
    IfFailRet(this->host.RunScriptFromFile(sourceFilePath->Data(), sourceUri->Data(), &result));
    return JsNoError;
}

ChakraStringResult NativeJavaScriptExecutor::CallFunctionAndReturnFlushedQueue(String^ moduleName, String^ methodName, String^ args)
{
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"__fbBatchedBridge", &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRetNullPtr(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"callFunctionReturnFlushedQueue", &methodId));

    JsValueRef method;
    IfFailRetNullPtr(JsGetProperty(fbBridgeObj, methodId, &method));

    JsValueRef moduleNameRef, methodNameRef;
    IfFailRetNullPtr(JsPointerToString(moduleName->Data(), moduleName->Length(), &moduleNameRef));
    IfFailRetNullPtr(JsPointerToString(methodName->Data(), methodName->Length(), &methodNameRef));

    JsValueRef argObj;
    IfFailRetNullPtr(JsPointerToString(args->Data(), args->Length(), &argObj));

    JsValueRef jsonObj;
    IfFailRetNullPtr(host.JsonParse(argObj, &jsonObj));

    JsValueRef result;
    JsValueRef newArgs[4] = { host.globalObject, moduleNameRef, methodNameRef, jsonObj };
    IfFailRetNullPtr(JsCallFunction(method, newArgs, 4, &result));

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
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"__fbBatchedBridge", &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRetNullPtr(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"invokeCallbackAndReturnFlushedQueue", &methodId));

    JsValueRef method;
    IfFailRetNullPtr(JsGetProperty(fbBridgeObj, methodId, &method));

    JsValueRef callbackIdRef;
    IfFailRetNullPtr(JsIntToNumber(callbackId, &callbackIdRef));

    JsValueRef argsObj;
    IfFailRetNullPtr(JsPointerToString(args->Data(), args->Length(), &argsObj));

    JsValueRef argsJson;
    IfFailRetNullPtr(host.JsonParse(argsObj, &argsJson));

    JsValueRef result;
    JsValueRef newArgs[3] = { host.globalObject, callbackIdRef, argsJson };
    IfFailRetNullPtr(JsCallFunction(method, newArgs, 3, &result));

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
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"__fbBatchedBridge", &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRetNullPtr(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"flushedQueue", &methodId));

    JsValueRef method;
    IfFailRetNullPtr(JsGetProperty(fbBridgeObj, methodId, &method));

    JsValueRef result;
    JsValueRef newArgs[1] = { host.globalObject };
    IfFailRetNullPtr(JsCallFunction(method, newArgs, 1, &result));

    JsValueRef stringifiedResult;
    IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

    ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
    return finalResult;
}
