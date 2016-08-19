#include "pch.h"
#include "ChakraHost.h"
#include "SerializedSourceContext.h"

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
	wchar_t buff[56];
	swprintf(buff, 56, L"[JS {%s}] ", kind);
	OutputDebugStringW(buff);

	// First argument is this-context, ignore...
	for (USHORT i = 1; i < argumentCount; i++)
	{
		JsValueRef resultJSString;
		IfFailThrow(self->JsonStringify(arguments[i], &resultJSString), L"JSON.stringify failed.");

		const wchar_t* szBuf;
		size_t szBufLen;
		IfFailThrow(JsStringToPointer(resultJSString, &szBuf, &szBufLen), L"Failed to get string from pointer.");

		OutputDebugStringW(szBuf);
		OutputDebugStringW(L" ");
	}

	OutputDebugStringW(L"\n");

#endif

	return JS_INVALID_REFERENCE;
};

JsValueRef CALLBACK ConsoleLog(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"log", callee, isConstructCall, arguments, argumentCount, callbackState);
}

JsValueRef CALLBACK ConsoleWarn(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"warn", callee, isConstructCall, arguments, argumentCount, callbackState);
}

JsValueRef CALLBACK ConsoleInfo(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"info", callee, isConstructCall, arguments, argumentCount, callbackState);
}

JsValueRef CALLBACK ConsoleError(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	return InvokeConsole(L"error", callee, isConstructCall, arguments, argumentCount, callbackState);
}

bool CALLBACK LoadSourceCallback(JsSourceContext sourceContext, const wchar_t** scriptBuffer)
{
	SerializedSourceContext* context = (SerializedSourceContext*)sourceContext;
	FILE* file;
	
	_wfopen_s(&file, context->sourcePath, L"rb");

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	char* rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		return false;
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);
	fclose(file);

	wchar_t* contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		free(rawBytes);
		return false;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		free(rawBytes);
		free(contents);
		return false;
	}

	free(rawBytes);
	*scriptBuffer = contents;

	return true;
}

void CALLBACK UnloadSourceCallback(JsSourceContext sourceContext)
{
	SerializedSourceContext* context = (SerializedSourceContext*)sourceContext;
	delete context;
}

JsErrorCode ChakraHost::RunSerailizedScript(BYTE* buffer, const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result)
{
	SerializedSourceContext* context = new SerializedSourceContext();
	context->byteCode = buffer;
	context->sourcePath = szPath;
	context->scriptBuffer = nullptr;

	IfFailRet(JsRunSerializedScriptWithCallback(
		&LoadSourceCallback,
		&UnloadSourceCallback,
		buffer,
		(JsSourceContext)context,
		szSourceUri,
		result));

	return JsNoError;
}

JsErrorCode ChakraHost::RunSerializedScriptFromFile(const wchar_t* szSerializedPath, const wchar_t* szPath, const wchar_t* szSourceUri, JsValueRef* result)
{
	FILE* file = NULL;
	BYTE* buffer = NULL;
	if (_wfopen_s(&file, szSerializedPath, L"rb"))
	{
		return JsErrorInvalidArgument;
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	buffer = (BYTE*)calloc(lengthBytes + 1, sizeof(BYTE));
	fread((void *)buffer, sizeof(BYTE), lengthBytes, file);
	fclose(file);

	IfFailRet(RunSerailizedScript(buffer, szPath, szSourceUri, result));

	return JsNoError;
}

JsErrorCode ChakraHost::SerializeScriptFromFile(const wchar_t* szPath, const wchar_t* szDestination)
{
	JsErrorCode status = JsNoError;
	FILE *file;
	char *rawBytes = nullptr;
	wchar_t *contents = nullptr;

	if (_wfopen_s(&file, szPath, L"rb"))
	{
		status = JsErrorInvalidArgument;
		goto cleanup;
	}

	unsigned int current = ftell(file);
	fseek(file, 0, SEEK_END);
	unsigned int end = ftell(file);
	fseek(file, current, SEEK_SET);
	unsigned int lengthBytes = end - current;
	rawBytes = (char *)calloc(lengthBytes + 1, sizeof(char));

	if (rawBytes == nullptr)
	{
		status = JsErrorFatal;
		goto cleanup;
	}

	fread((void *)rawBytes, sizeof(char), lengthBytes, file);
	fclose(file);

	contents = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (contents == nullptr)
	{
		status = JsErrorFatal;
		goto cleanup;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, contents, lengthBytes + 1) == 0)
	{
		status = JsErrorFatal;
		goto cleanup;
	}

	IfFailCleanup(SerializeScript(contents, szDestination));

cleanup:
	if (rawBytes)
	{
		free(rawBytes);
	}
	if (contents)
	{
		free(contents);
	}
	
	return status;
}

JsErrorCode ChakraHost::SerializeScript(const wchar_t* szScript, const wchar_t* szDestination)
{
	JsErrorCode status;
	BYTE* buf = NULL;
	ULONG bufSize = 0L;
	IfFailCleanup(JsSerializeScript(szScript, buf, &bufSize));
	
	buf = new BYTE[bufSize];
	IfFailCleanup(JsSerializeScript(szScript, buf, &bufSize));

	FILE *file;
	if (_wfopen_s(&file, szDestination, L"wb"))
	{
		status = JsErrorInvalidArgument;
		goto cleanup;
	}

	if (fwrite(buf, sizeof(BYTE), bufSize, file) != bufSize)
	{
		status = JsErrorFatal;
		goto cleanup;
	}
	
	if (fclose(file))
	{
		status = JsErrorFatal;
		goto cleanup;
	}

cleanup:
	if (buf)
	{
		delete[] buf;
	}
	return status;
}

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
	if (fclose(file))
	{
		return JsErrorFatal;
	}

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
}

JsErrorCode ChakraHost::JsonStringify(JsValueRef argument, JsValueRef* result)
{
	JsValueRef args[2] = { globalObject, argument };
	IfFailRet(JsCallFunction(jsonStringifyObject, args, 2, result));
	return JsNoError;
}

JsErrorCode ChakraHost::JsonParse(JsValueRef argument, JsValueRef* result)
{
	JsValueRef args[2] = { globalObject, argument };
	IfFailRet(JsCallFunction(jsonParseObject, args, 2, result));
	return JsNoError;
}

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
}

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
}

JsErrorCode ChakraHost::Init()
{
	currentSourceContext = 0;

	IfFailRet(JsCreateRuntime(JsRuntimeAttributeNone, nullptr, &runtime));
	IfFailRet(JsCreateContext(runtime, &context));
	IfFailRet(JsSetCurrentContext(context));

	IfFailRet(JsGetGlobalObject(&globalObject));

	IfFailRet(InitJson());
	IfFailRet(InitConsole());

	return JsNoError;
}

JsErrorCode ChakraHost::Destroy()
{
	IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));
	IfFailRet(JsDisposeRuntime(runtime));

	return JsNoError;
}