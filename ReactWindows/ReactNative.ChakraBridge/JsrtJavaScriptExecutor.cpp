#include "pch.h"
#include "JsrtJavaScriptExecutor.h"

using namespace ReactNative::ChakraBridge;

int JsrtJavaScriptExectutor::InitializeHost()
{
	return this->host.Init();
}

int JsrtJavaScriptExectutor::DisposeHost()
{
	return this->host.Destroy();
}

int JsrtJavaScriptExectutor::SetGlobalVariable(String^ variableName, String^ stringifiedText)
{
	JsValueRef valueStringified;
	IfFailRet(JsPointerToString(stringifiedText->Data(), stringifiedText->Length(), &valueStringified));

	JsValueRef valueJson;
	IfFailRet(this->host.JsonParse(valueStringified, &valueJson));
	IfFailRet(this->host.SetGlobalVariable(variableName->Data(), valueJson));

	return JsNoError;
}

ChakraStringResult JsrtJavaScriptExectutor::GetGlobalVariable(String^ variableName)
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

ChakraStringResult JsrtJavaScriptExectutor::RunScript(String^ source, String^ sourceUri)
{
	JsValueRef result;
	IfFailRetNullPtr(this->host.RunScript(source->Data(), sourceUri->Data(), &result));

	JsValueRef resultJson;
	IfFailRetNullPtr(this->host.JsonStringify(result, &resultJson));

	const wchar_t* szBuf;
	size_t bufLen;
	IfFailRetNullPtr(JsStringToPointer(resultJson, &szBuf, &bufLen));

	ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
	return finalResult;
}

ChakraStringResult JsrtJavaScriptExectutor::RunScriptFromFile(String^ sourceFilePath, String^ sourceUri)
{
	JsValueRef result;
	IfFailRetNullPtr(this->host.RunScriptFromFile(sourceFilePath->Data(), sourceUri->Data(), &result));

	JsValueRef resultJson;
	IfFailRetNullPtr(this->host.JsonStringify(result, &resultJson));

	const wchar_t* szBuf;
	size_t bufLen;
	IfFailRetNullPtr(JsStringToPointer(resultJson, &szBuf, &bufLen));

	ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
	return finalResult;
}

ChakraStringResult JsrtJavaScriptExectutor::CallFunctionAndReturnFlushedQueue(String^ moduleName, String^ methodName, String^ args)
{
	JsPropertyIdRef modulePropertyId;
	IfFailRetNullPtr(JsGetPropertyIdFromName(moduleName->Data(), &modulePropertyId));

	JsValueRef moduleObject;
	IfFailRetNullPtr(JsGetProperty(host.globalObject, modulePropertyId, &moduleObject));

	JsValueType moduleType;
	IfFailRetNullPtr(JsGetValueType(moduleObject, &moduleType));

	// Call require to load function
	if (moduleType != JsObject)
	{
		JsValueRef moduleString;
		IfFailRetNullPtr(JsPointerToString(moduleName->Data(), moduleName->Length(), &moduleString));

		JsValueRef requireArguments[2] = { host.globalObject, moduleString };
		IfFailRetNullPtr(JsCallFunction(host.requireObject, requireArguments, 2, &moduleObject));
	}

	JsPropertyIdRef methodPropertyId;
	IfFailRetNullPtr(JsGetPropertyIdFromName(methodName->Data(), &methodPropertyId));

	JsValueRef methodObject;
	IfFailRetNullPtr(JsGetProperty(moduleObject, methodPropertyId, &methodObject));

	JsValueRef argObj;
	IfFailRetNullPtr(JsPointerToString(args->Data(), args->Length(), &argObj));

	JsValueRef jsonObj;
	IfFailRetNullPtr(host.JsonParse(argObj, &jsonObj));

	JsValueRef result;
	JsValueRef newArgs[2] = { host.globalObject, jsonObj };
	IfFailRetNullPtr(JsCallFunction(methodObject, newArgs, 2, &result));

	JsValueRef stringifiedResult;
	IfFailRetNullPtr(host.JsonStringify(result, &stringifiedResult));

	const wchar_t* szBuf;
	size_t bufLen;
	IfFailRetNullPtr(JsStringToPointer(stringifiedResult, &szBuf, &bufLen));

	ChakraStringResult finalResult = { JsNoError, ref new String(szBuf, bufLen) };
	return finalResult;
}

ChakraStringResult JsrtJavaScriptExectutor::InvokeCallbackAndReturnFlushedQueue(int callbackId, String^ args)
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

ChakraStringResult JsrtJavaScriptExectutor::FlushedQueue()
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