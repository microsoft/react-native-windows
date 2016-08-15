#include "pch.h"
#include "ChakraHost.h"

void ThrowException(const wchar_t* szException)
{
	// We ignore error since we're already in an error state.
	JsValueRef errorValue;
	JsValueRef errorObject;
	JsPointerToString(szException, wcslen(szException), &errorValue);
	JsCreateError(errorValue, &errorObject);
	JsSetException(errorObject);
};

JsErrorCode DefineHostCallback(JsValueRef globalObject, const wchar_t *callbackName, JsNativeFunction callback, void *callbackState)
{
	JsPropertyIdRef propertyId;
	IfFailRet(JsGetPropertyIdFromName(callbackName, &propertyId));

	JsValueRef function;
	IfFailRet(JsCreateFunction(callback, callbackState, &function));
	IfFailRet(JsSetProperty(globalObject, propertyId, function, true));

	return JsNoError;
};

JsValueRef InvokeConsole(const wchar_t* kind, JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
#ifdef _DEBUG

	ChakraHost* self = (ChakraHost*)callbackState;

	wprintf(L"[JS {%s}] ", kind);

	// First argument is this-context, ignore...
	for (USHORT i = 1; i < argumentCount; i++)
	{
		JsValueRef resultJSString;
		IfFailThrow(self->JsonStringify(arguments[i], &resultJSString), L"JSON.stringify failed.");

		const wchar_t* szBuf;
		size_t szBufLen;
		IfFailThrow(JsStringToPointer(resultJSString, &szBuf, &szBufLen), L"Failed to get string from pointer.");
		wprintf(L"%s ", szBuf);
	}

	wprintf(L"\n");

#endif

	return JS_INVALID_REFERENCE;
};

JsValueRef CALLBACK ConsoleLog(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"log", callee, isConstructCall, arguments, argumentCount, callbackState);
};

JsValueRef CALLBACK ConsoleWarn(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"warn", callee, isConstructCall, arguments, argumentCount, callbackState);
};

JsValueRef CALLBACK ConsoleInfo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"info", callee, isConstructCall, arguments, argumentCount, callbackState);
};

JsValueRef CALLBACK ConsoleError(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"error", callee, isConstructCall, arguments, argumentCount, callbackState);
};

JsErrorCode ChakraHost::RunScriptFromFile(const wchar_t* szFileName, const wchar_t* szSourceUri, JsValueRef* result)
{
	JsErrorCode status = JsNoError;
	FILE *file;

	if (_wfopen_s(&file, szFileName, L"rb"))
	{
		return JsErrorInvalidArgument;
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char *rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		return JsErrorFatal;
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);

	wchar_t *contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		return JsErrorFatal;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		return JsErrorFatal;
	}

	status = RunScript(contents, szSourceUri, result);

	free(rawBytes);
	free(contents);

	return status;
}

JsErrorCode ChakraHost::RunScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result)
{
	return JsRunScript(szScript, currentSourceContext++, szSourceUri, result);
};

JsErrorCode ChakraHost::CallModuleMethod(const wchar_t* szModule, const wchar_t* szMethod, JsValueRef* arguments, USHORT argumentsLength, JsValueRef* result)
{
	JsPropertyIdRef modulePropertyId;
	IfFailRet(JsGetPropertyIdFromName(szModule, &modulePropertyId));

	JsValueRef moduleObject;
	IfFailRet(JsGetProperty(globalObject, modulePropertyId, &moduleObject));

	JsValueType moduleType;
	IfFailRet(JsGetValueType(moduleObject, &moduleType));

	// Call require to load function
	if (moduleType != JsObject)
	{
		JsValueRef moduleString;
		IfFailRet(JsPointerToString(szModule, wcslen(szModule), &moduleString));
		JsValueRef requireArguments[2] = { globalObject, moduleString };
		IfFailRet(JsCallFunction(requireObject, requireArguments, 2, &moduleObject));
	}

	JsPropertyIdRef methodPropertyId;
	IfFailRet(JsGetPropertyIdFromName(szMethod, &methodPropertyId));

	JsValueRef methodObject;
	IfFailRet(JsGetProperty(moduleObject, methodPropertyId, &methodObject));

	USHORT callArgsLength = argumentsLength + 1;
	JsValueRef arg;
	JsValueRef* callArguments = new JsValueRef[callArgsLength];
	callArguments[0] = globalObject;
	for (USHORT i = 0; i < argumentsLength; i++)
	{
		arg = arguments[i];
		UINT jsAddRefCount;
		IfFailRet(JsAddRef(arg, &jsAddRefCount));
		callArguments[i + 1] = arg;
	}

	IfFailRet(JsCallFunction(methodObject, callArguments, callArgsLength, result));

	for (USHORT i = 1; i < callArgsLength; i++)
	{
		arg = callArguments[i];
		UINT jsReleaseRefCount;
		IfFailRet(JsRelease(arg, &jsReleaseRefCount));
	}

	delete[] callArguments;

	return JsNoError;
};

JsErrorCode ChakraHost::JsonStringify(JsValueRef argument, JsValueRef* result)
{
	JsValueRef args[2] = { globalObject, argument };
	IfFailRet(JsCallFunction(jsonStringifyObject, args, 2, result));
	return JsNoError;
};

JsErrorCode ChakraHost::JsonParse(JsValueRef argument, JsValueRef* result)
{
	JsValueRef args[2] = { globalObject, argument };
	IfFailRet(JsCallFunction(jsonParseObject, args, 2, result));
	return JsNoError;
};

JsErrorCode ChakraHost::GetGlobalVariable(const wchar_t* szPropertyName, JsValueRef* result)
{
	JsPropertyIdRef globalVarId;
	IfFailRet(JsGetPropertyIdFromName(szPropertyName, &globalVarId));
	IfFailRet(JsGetProperty(globalObject, globalVarId, result));
	return JsNoError;
}

JsErrorCode ChakraHost::SetGlobalVariable(const wchar_t* szPropertyName, JsValueRef value)
{
	JsPropertyIdRef globalVarId;
	IfFailRet(JsGetPropertyIdFromName(szPropertyName, &globalVarId));
	IfFailRet(JsSetProperty(globalObject, globalVarId, value, true));
	return JsNoError;
}

JsErrorCode ChakraHost::InitJson()
{
	JsPropertyIdRef jsonPropertyId;
	IfFailRet(JsGetPropertyIdFromName(L"JSON", &jsonPropertyId));
	JsValueRef jsonObject;
	IfFailRet(JsGetProperty(globalObject, jsonPropertyId, &jsonObject));

	JsPropertyIdRef jsonParseId;
	IfFailRet(JsGetPropertyIdFromName(L"parse", &jsonParseId));
	IfFailRet(JsGetProperty(jsonObject, jsonParseId, &jsonParseObject));

	JsPropertyIdRef jsonStringifyId;
	IfFailRet(JsGetPropertyIdFromName(L"stringify", &jsonStringifyId));
	IfFailRet(JsGetProperty(jsonObject, jsonStringifyId, &jsonStringifyObject));

	return JsNoError;
};

JsErrorCode ChakraHost::InitConsole()
{
	JsPropertyIdRef consolePropertyId;
	IfFailRet(JsGetPropertyIdFromName(L"console", &consolePropertyId));

	JsValueRef consoleObject;
	IfFailRet(JsCreateObject(&consoleObject));
	IfFailRet(JsSetProperty(globalObject, consolePropertyId, consoleObject, true));

	IfFailRet(DefineHostCallback(consoleObject, L"info", ConsoleInfo, this));
	IfFailRet(DefineHostCallback(consoleObject, L"log", ConsoleLog, this));
	IfFailRet(DefineHostCallback(consoleObject, L"warn", ConsoleWarn, this));
	IfFailRet(DefineHostCallback(consoleObject, L"error", ConsoleError, this));

	return JsNoError;
};

JsErrorCode ChakraHost::InitRequire()
{
	JsPropertyIdRef requirePropertyId;
	IfFailRet(JsGetPropertyIdFromName(L"require", &requirePropertyId));
	IfFailRet(JsGetProperty(globalObject, requirePropertyId, &requireObject));

	return JsNoError;
};

JsErrorCode ChakraHost::Init()
{
	currentSourceContext = 0;

	IfFailRet(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime));
	IfFailRet(JsCreateContext(runtime, &context));
	IfFailRet(JsSetCurrentContext(context));

	IfFailRet(JsGetGlobalObject(&globalObject));

	IfFailRet(InitJson());
	IfFailRet(InitConsole());
	IfFailRet(InitRequire());

	return JsNoError;
};

JsErrorCode ChakraHost::Destroy()
{
	IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));
	IfFailRet(JsDisposeRuntime(runtime));

	return JsNoError;
};