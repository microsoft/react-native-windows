#include "pch.h"
#include "NativeJavaScriptExecutor.h"

#if _WINRT_DLL
#define declareWcharDataForString(str) auto wcharDataFor_##str = str->Data();
#define declareManagedStringFor(str, length) String^ managedStringFor_##str = ref new String(str, length)
#define stringLengthFor(str) str->Length()

#else _WINRT_DLL
#include <vcclr.h>

#define declareWcharDataForString(str) pin_ptr<const wchar_t> wcharDataFor_##str = PtrToStringChars(str)
#define declareManagedStringFor(str, length) String^ managedStringFor_##str = gcnew String(str)
#define stringLengthFor(str) str->Length
#endif _WINRT_DLL

const wchar_t* BATCH_BRIDGE = L"__fbBatchedBridge";

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
    declareWcharDataForString(stringifiedText);
    IfFailRet(JsPointerToString((const wchar_t*)wcharDataFor_stringifiedText, stringLengthFor(stringifiedText), &valueStringified));

    JsValueRef valueJson;
    IfFailRet(this->host.JsonParse(valueStringified, &valueJson));

    declareWcharDataForString(variableName);
    IfFailRet(this->host.SetGlobalVariable(wcharDataFor_variableName, valueJson));

    return JsNoError;
}

ChakraStringResult NativeJavaScriptExecutor::GetGlobalVariable(String^ variableName)
{
    
    JsValueRef globalVariable;
    declareWcharDataForString(variableName);
    IfFailRetNullPtr(this->host.GetGlobalVariable(wcharDataFor_variableName, &globalVariable));

    JsValueRef globalVariableJson;
    IfFailRetNullPtr(this->host.JsonStringify(globalVariable, &globalVariableJson));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(globalVariableJson, &szBuf, &bufLen));

    declareManagedStringFor(szBuf, bufLen);
    ChakraStringResult finalResult = { JsNoError, managedStringFor_szBuf };
    return finalResult;
}

int NativeJavaScriptExecutor::RunScript(String^ source, String^ sourceUri)
{
    JsValueRef result;
    declareWcharDataForString(source);
    declareWcharDataForString(sourceUri);
    IfFailRet(this->host.RunScript(wcharDataFor_source, wcharDataFor_sourceUri, &result));

    return JsNoError;
}

int NativeJavaScriptExecutor::RunSerializedScript(String^ source, String^ serialized, String^ sourceUri)
{
    JsValueRef result;
    declareWcharDataForString(serialized);
    declareWcharDataForString(source);
    declareWcharDataForString(sourceUri);

    IfFailRet(this->host.RunSerializedScript(wcharDataFor_source, wcharDataFor_serialized, wcharDataFor_sourceUri, &result));

    return JsNoError;
}

ChakraStringResult NativeJavaScriptExecutor::CallFunctionAndReturnFlushedQueue(String^ moduleName, String^ methodName, String^ args)
{
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(BATCH_BRIDGE, &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRetNullPtr(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"callFunctionReturnFlushedQueue", &methodId));

    JsValueRef method;
    IfFailRetNullPtr(JsGetProperty(fbBridgeObj, methodId, &method));

    JsValueRef moduleNameRef;
    declareWcharDataForString(moduleName);
    IfFailRetNullPtr(JsPointerToString(wcharDataFor_moduleName, stringLengthFor(moduleName), &moduleNameRef));

    JsValueRef methodNameRef;
    declareWcharDataForString(methodName);
    IfFailRetNullPtr(JsPointerToString(wcharDataFor_methodName, stringLengthFor(methodName), &methodNameRef));

    JsValueRef argsObj;
    declareWcharDataForString(args);
    IfFailRetNullPtr(JsPointerToString(wcharDataFor_args, stringLengthFor(args), &argsObj));

    JsValueRef jsonObj;
    IfFailRetNullPtr(host.JsonParse(argsObj, &jsonObj));

    JsValueRef result;
    JsValueRef newArgs[4] = { host.globalObject, moduleNameRef, methodNameRef, jsonObj };
    IfFailRetNullPtr(JsCallFunction(method, newArgs, 4, &result));

    JsValueRef stringifiedResult;
    IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

    const wchar_t* szBuf;
    size_t bufLen;
    IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

    declareManagedStringFor(szBuf, bufLen);
    ChakraStringResult finalResult = { JsNoError, managedStringFor_szBuf };
    return finalResult;
}

ChakraStringResult NativeJavaScriptExecutor::InvokeCallbackAndReturnFlushedQueue(int callbackId, String^ args)
{
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(BATCH_BRIDGE, &fbBridgeId));

    JsValueRef fbBridgeObj;
    IfFailRetNullPtr(JsGetProperty(host.globalObject, fbBridgeId, &fbBridgeObj));

    JsPropertyIdRef methodId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(L"invokeCallbackAndReturnFlushedQueue", &methodId));

    JsValueRef method;
    IfFailRetNullPtr(JsGetProperty(fbBridgeObj, methodId, &method));

    JsValueRef callbackIdRef;
    IfFailRetNullPtr(JsIntToNumber(callbackId, &callbackIdRef));

    JsValueRef argsObj;
    declareWcharDataForString(args);
    IfFailRetNullPtr(JsPointerToString(wcharDataFor_args, stringLengthFor(args), &argsObj));

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

    declareManagedStringFor(szBuf, bufLen);
    ChakraStringResult finalResult = { JsNoError, managedStringFor_szBuf };
    return finalResult;
}

ChakraStringResult NativeJavaScriptExecutor::FlushedQueue()
{
    JsPropertyIdRef fbBridgeId;
    IfFailRetNullPtr(JsGetPropertyIdFromName(BATCH_BRIDGE, &fbBridgeId));

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

    declareManagedStringFor(szBuf, bufLen);
    ChakraStringResult finalResult = { JsNoError, managedStringFor_szBuf };
    return finalResult;
}
