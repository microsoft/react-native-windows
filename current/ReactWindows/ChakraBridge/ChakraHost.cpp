// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "ChakraHost.h"
#include "JsIndexedModulesUnbundle.h"
#include "SerializedSourceContext.h"
#include <stdint.h>
#include "asprintf.h"

const unsigned int MagicFileHeader = 0xFB0BD1E5;

// Initializing by default to write debug output if there is a need to trace
// any log messages before NativeJavaScriptExecutor::InitializeHost gets initialized
extern "C" std::function<void(int, const wchar_t *)> g_loggingCallback = [=](int level, const wchar_t *logline)
{
#ifdef _DEBUG
    OutputDebugStringW(LogLevelToString(level));
    OutputDebugStringW(logline);
    OutputDebugStringW(L"\n");
#endif
};

#ifdef _DEBUG
extern "C" bool g_isLoggingEnabled = true;
#else
extern "C" bool g_isLoggingEnabled = false;
#endif

void ThrowException(const wchar_t* szException)
{
    // We ignore error since we're already in an error state.
    JsValueRef errorValue;
    JsValueRef errorObject;
    JsPointerToString(szException, wcslen(szException), &errorValue);
    JsCreateError(errorValue, &errorObject);
    JsSetException(errorObject);
}

JsErrorCode DefineHostCallback(JsValueRef globalObject, const wchar_t *callbackName, JsNativeFunction callback, void *callbackState)
{
    JsPropertyIdRef propertyId;
    IfFailRet(JsGetPropertyIdFromName(callbackName, &propertyId));

    JsValueRef function;
    IfFailRet(JsCreateFunction(callback, callbackState, &function));
    IfFailRet(JsSetProperty(globalObject, propertyId, function, true));

    return JsNoError;
}

JsValueRef CALLBACK NativeLoggingCallback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
    if (g_isLoggingEnabled) {

        double logLevelIndex;
        JsNumberToDouble(arguments[2], &logLevelIndex);

        const wchar_t *szBuf;
        size_t bufLen;
        auto status = JsStringToPointer(arguments[1], &szBuf, &bufLen);
        if (status == JsNoError)
        {
            g_loggingCallback((int)logLevelIndex, szBuf);
            return JS_INVALID_REFERENCE;
        }

        wchar_t* szError = NULL;
        if (_aswprintf(&szError, L"NativeLoggingCallback: Error '%u' while trying to get a string pointer from JsStringToPointer", status) < 0)
        {
            g_loggingCallback(3, L"NativeLoggingCallback: Fatal error");
            return JS_INVALID_REFERENCE;
        }

        g_loggingCallback(3, szError);
        free(szError);
    }
    return JS_INVALID_REFERENCE;
}

JsErrorCode ChakraHost::LoadByteCode(const wchar_t* szPath, BYTE** pData, HANDLE* hFile, HANDLE* hMap, bool bIsReadOnly)
{
	*pData = nullptr;

	DWORD dwShareMode = bIsReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
	*hFile = CreateFile2(szPath, dwShareMode, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	if (*hFile == INVALID_HANDLE_VALUE)
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #1");
		return JsErrorFatal;
	}

	DWORD flProtect = (bIsReadOnly ? PAGE_READONLY : PAGE_READWRITE) | SEC_RESERVE;
	*hMap = CreateFileMapping(*hFile, nullptr, flProtect, 0, 0, L"ReactNativeMapping");
	if (*hMap == NULL)
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #2");
        CloseHandle(*hFile);
		return JsErrorFatal;
	}

	DWORD dwDesiredAccess = bIsReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;
	*pData = (BYTE*)MapViewOfFile(*hMap, dwDesiredAccess, 0, 0, 0);
	if (*pData == NULL)
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #3");
        CloseHandle(*hMap);
		CloseHandle(*hFile);
		return JsErrorFatal;
	}

	return JsNoError;
}

JsErrorCode ChakraHost::LoadFileContents(const wchar_t* szPath, wchar_t** pszData)
{
	FILE *file;
	*pszData = nullptr;

	if (_wfopen_s(&file, szPath, L"rb"))
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
        g_loggingCallback(3, L"ChakraHost fatal condition #4");
        return JsErrorFatal;
	}

	fread(rawBytes, sizeof(char), lengthBytes, file);
	if (fclose(file))
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #5");
        free(rawBytes);
        return JsErrorFatal;
	}

	*pszData = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (*pszData == nullptr)
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #6");
        free(rawBytes);
		return JsErrorFatal;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, *pszData, lengthBytes + 1) == 0)
	{
        g_loggingCallback(3, L"ChakraHost fatal condition #8");
        free(*pszData);
		free(rawBytes);
		return JsErrorFatal;
	}

	return JsNoError;
}

bool CompareLastWrite(const wchar_t* szPath1, const wchar_t* szPath2)
{
    HANDLE hPath1, hPath2;
    FILETIME ftPath1Create, ftPath1Access, ftPath1Write;
    FILETIME ftPath2Create, ftPath2Access, ftPath2Write;

    hPath1 = CreateFile2(szPath1, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
    if (hPath1 == INVALID_HANDLE_VALUE && GetLastError() == ERROR_FILE_NOT_FOUND) {
        return false;
    }

    hPath2 = CreateFile2(szPath2, GENERIC_READ, FILE_SHARE_READ, OPEN_EXISTING, nullptr);

    GetFileTime(hPath1, &ftPath1Create, &ftPath1Access, &ftPath1Write);
    GetFileTime(hPath2, &ftPath2Create, &ftPath2Access, &ftPath2Write);

    CloseHandle(hPath1);
    CloseHandle(hPath2);

    return CompareFileTime(&ftPath1Write, &ftPath2Write) == 1;
}

bool CALLBACK LoadSourceCallback(JsSourceContext sourceContext, const wchar_t** scriptBuffer)
{
    SerializedSourceContext* context = (SerializedSourceContext*)sourceContext;
    *scriptBuffer = context->scriptBuffer;
    return true;
}

void CALLBACK UnloadSourceCallback(JsSourceContext sourceContext)
{
    SerializedSourceContext* context = (SerializedSourceContext*)sourceContext;
    delete context;
}

JsValueRef CALLBACK NativeRequire(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
	// cast the callback state to the ChakraHost instance
	auto host = (ChakraHost*)callbackState;

	// Assert the argument count.
	if (argumentCount != 2)
	{
		ThrowException(L"Expected only one parameter to nativeRequire.");
		return JS_INVALID_REFERENCE;
	}

	// Assert a valid module index
	double index;
	IfFailThrow(JsNumberToDouble(arguments[1], &index), L"Index parameter to nativeRequire must be a number.");
	uint32_t convertedIndex = (uint32_t)index;
	if (convertedIndex <= 0)
	{
		ThrowException(L"Index parameter to nativeRequire must be greater than zero.");
		return JS_INVALID_REFERENCE;
	}

	// Retrieve the unbundle module
	auto module = host->unbundle->GetModule(convertedIndex);
	if (!module)
	{
		ThrowException(L"Unable to resolve unbundle module.");
		return JS_INVALID_REFERENCE;
	}

	// Evaluate the unbundle module script
	if (host->EvaluateScript(module->source, module->sourceUrl, nullptr) != JsNoError)
	{
		// Note, control flow continues below, exception set to JS runtime
		ThrowException(L"Failure occurred while evaluating unbundle module.");
	}

	delete module;
	return JS_INVALID_REFERENCE;
}

JsValueRef CALLBACK NativeCallSyncHook(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
    // cast the callback state to the ChakraHost instance
    auto host = (ChakraHost*)callbackState;

    // Assert the argument count.
    if (argumentCount != 4)
    {
        ThrowException(L"Expected only three parameters to nativeCallSyncHook.");
        return JS_INVALID_REFERENCE;
    }

    // Assert the handler has been set.
    if (host->callSyncHandler == nullptr)
    {
        ThrowException(L"Sync hook has not been set.");
        return JS_INVALID_REFERENCE;
    }

    // Get the module ID and method ID
    double moduleId;
    IfFailThrow(JsNumberToDouble(arguments[1], &moduleId), L"The moduleId parameter to nativeCallSyncHook must be a number.");
    double methodId;
    IfFailThrow(JsNumberToDouble(arguments[2], &methodId), L"The methodId parameter to nativeCallSyncHook must be a number.");

    // Get the stringified arguments
    JsValueRef stringifiedArgs;
    IfFailThrow(host->JsonStringify(arguments[3], &stringifiedArgs), L"Could not stringify args parameter.");
    const wchar_t* argsBuf;
    size_t bufLen;
    IfFailThrow(JsStringToPointer(stringifiedArgs, &argsBuf, &bufLen), L"Could not get pointer to stringified args.");

    // Invoke the sync callback
    auto result = host->callSyncHandler((int)moduleId, (int)methodId, ref new String(argsBuf, (unsigned int)bufLen));
    if (result.ErrorCode == JsNoError)
    {
        // Return the parsed JSON result
        JsValueRef stringifiedResult;
        IfFailThrow(JsPointerToString(result.Result->Data(), result.Result->Length(), &stringifiedResult), L"Could not convert pointer to stringified result.");
        JsValueRef jsonResult;
        IfFailThrow(host->JsonParse(stringifiedResult, &jsonResult), L"Could not parse stringified result");
        return jsonResult;
    }
    else
    {
        // Set an error with the exception message
        JsValueRef stringifiedMessage;
        IfFailThrow(JsPointerToString(result.Result->Data(), result.Result->Length(), &stringifiedMessage), L"Could not convert pointer to stringified message.");
        JsValueRef jsonError;
        IfFailThrow(JsCreateError(stringifiedMessage, &jsonError), L"Could not create JavaScript error.");
        IfFailThrow(JsSetException(jsonError), L"Could not throw JavaScript error.");
        return JS_INVALID_REFERENCE;
    }
}

JsValueRef CALLBACK NativeFlushQueueImmediate(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
    // cast the callback state to the ChakraHost instance
    auto host = (ChakraHost*)callbackState;

    // Assert the argument count.
    if (argumentCount != 2)
    {
        ThrowException(L"Expected only one parameter to nativeFlushQueueImmediate.");
        return JS_INVALID_REFERENCE;
    }

    // Assert the handler has been set.
    if (host->flushQueueImmediateHandler == nullptr)
    {
        ThrowException(L"flushQueueImmediate callback has not been set.");
        return JS_INVALID_REFERENCE;
    }

    // Get the stringified arguments.
    JsValueRef stringifiedArgs;
    IfFailThrow(host->JsonStringify(arguments[1], &stringifiedArgs), L"Could not stringify the args parameter.");
    const wchar_t* argsBuf;
    size_t bufLen;
    IfFailThrow(JsStringToPointer(stringifiedArgs, &argsBuf, &bufLen), L"Could not get pointer to stringified args.");

    // Invoke the flushQueueImmediate callback.
    host->flushQueueImmediateHandler(ref new String(argsBuf, (unsigned int)bufLen));

    return JS_INVALID_REFERENCE;
}

bool HasMagicFileHeader(const wchar_t* szPath)
{
	FILE *file;
	if (_wfopen_s(&file, szPath, L"rb"))
	{
		return false;
	}

	uint32_t magicHeader;
	size_t nRead = fread(&magicHeader, sizeof(uint32_t), 1, file);
	if (fclose(file) || nRead != 1)
	{
		return false;
	}

	// The header is little-endian, this function assumes a little-endian host
	return magicHeader == MagicFileHeader;
}

bool IsUnbundle(const wchar_t* szSourcePath)
{
	wchar_t buffer[_MAX_DIR] = L"";
	auto filename = wcsrchr(szSourcePath, L'\\');
	wcsncat_s(buffer, szSourcePath, wcslen(szSourcePath) - wcslen(filename) + 1);
	wcscat_s(buffer, L"js-modules\\UNBUNDLE");
	return HasMagicFileHeader(buffer);
}

bool IsIndexedUnbundle(const wchar_t* szSourcePath)
{
	return HasMagicFileHeader(szSourcePath);
}

//
// This function generates a bytecode file ("serialized script") based on the original JS bundle.
// Subsequent applications launches get a speed benefit by directly executing this generasted file (with RunSerializedScript)
//
// Some care is taken to make sure there's a way out for any error/corruption that may happen. The first version of this code
// used to write straight to the output file. Application being terminated before the operation ended triggered the generaion of incomplete files.
// As a mitigation we use a "write to temp file + rename" pattern now.
//
JsErrorCode ChakraHost::SerializeScript(const wchar_t* szPath, const wchar_t* szSerializedPath)
{
    ULONG bufferSize = 0L;
    BYTE* buffer = nullptr;
    wchar_t* szScriptBuffer = nullptr;
    wchar_t* szTmpSerializedPath = nullptr;
    IfFailRet(LoadFileContents(szPath, &szScriptBuffer));

    JsErrorCode status = JsNoError;
    if (!CompareLastWrite(szSerializedPath, szPath))
    {
        IfFailCleanup(JsSerializeScript(szScriptBuffer, buffer, &bufferSize));
        buffer = new BYTE[bufferSize];
        IfFailCleanup(JsSerializeScript(szScriptBuffer, buffer, &bufferSize));

        size_t tmpFilePathSize = wcslen(szSerializedPath) + 4; // add string size of ".tmp"
        szTmpSerializedPath = new wchar_t[tmpFilePathSize + 1];
        swprintf(szTmpSerializedPath, tmpFilePathSize + 1, L"%ws.tmp", szSerializedPath);

        FILE* file;
        auto err = _wfopen_s(&file, szTmpSerializedPath, L"wb");
        if (err != 0)
        {
            status = JsErrorFatal;
            goto cleanup;
        }

        size_t nrBytes = fwrite(buffer, sizeof(BYTE), bufferSize, file);
        if (nrBytes != bufferSize)
        {
            status = JsErrorFatal;
            fclose(file);
            goto cleanup;
        }

        fclose(file);

        _wunlink(szSerializedPath);
        if (0 != _wrename(szTmpSerializedPath, szSerializedPath))
        {
            goto cleanup;
        }
    }
cleanup:
    free(buffer);
    free(szScriptBuffer);
    free(szTmpSerializedPath);
    return status;
}

JsErrorCode ChakraHost::RunSerializedScript(const wchar_t* szPath, const wchar_t* szSerializedPath, const wchar_t* szSourceUri, JsValueRef* result)
{
    HANDLE hFile = NULL;
    HANDLE hMap = NULL;
    BYTE* buffer = nullptr;
    wchar_t* szScriptBuffer = nullptr;
    IfFailRet(LoadFileContents(szPath, &szScriptBuffer));

    JsErrorCode error;
    if (!CompareLastWrite(szSerializedPath, szPath))
    {
        error = JsErrorBadSerializedScript;
    }
    else
    {
        error = LoadByteCode(szSerializedPath, &buffer, &hFile, &hMap, true);
    }
    if (error != JsNoError)
    {
        free(szScriptBuffer);
        return error;
    }

    SerializedSourceContext* context = new SerializedSourceContext();
    context->byteBuffer = buffer;
    context->scriptBuffer = szScriptBuffer;
    context->fileHandle = hFile;
    context->mapHandle = hMap;

    __try
    {
        error = JsRunSerializedScriptWithCallback(&LoadSourceCallback, &UnloadSourceCallback, buffer, (JsSourceContext)context, szSourceUri, result);
    }
    __finally
    {
        if (AbnormalTermination())
        {
            // If JsRunSerializedScriptWithCallback threw any SEH exception (due to corrupted bytecode file that can't be safely caught by
            // Chakra engine), we make sure we kill the bytecode so next run is fresh.
            context->Dispose();
            // This keeps the code clean even though we know app won't make it to the next if block..
            context = NULL;
            _wunlink(szSerializedPath);
        }
    }

    if (error != JsNoError)
    {
        // UnloadSourceCallback is called even in error case (though LoadSourceCallback never is), so we can't fully delete the context
        if (context != NULL)
        {
            context->Dispose();
        }
        return error;
    }
    return JsNoError;
}

JsErrorCode ChakraHost::RunScript(const wchar_t* szFileName, const wchar_t* szSourceUri, JsValueRef* result)
{
    wchar_t* contents = nullptr;
    if (IsUnbundle(szFileName))
    {
        this->unbundle = new JsModulesUnbundle(szFileName);
		IfFailRet(InitNativeRequire());
        IfFailRet(unbundle->GetStartupCode(&contents));
    }
    else if (IsIndexedUnbundle(szFileName))
    {
        this->unbundle = new JsIndexedModulesUnbundle(szFileName);
		IfFailRet(InitNativeRequire());
		IfFailRet(unbundle->GetStartupCode(&contents));
    }
    else
    {
        IfFailRet(LoadFileContents(szFileName, &contents));
    }

    JsErrorCode status = EvaluateScript(contents, szSourceUri, result);
    free(contents);

    return status;
}

JsErrorCode ChakraHost::EvaluateScript(const wchar_t* szScript, const wchar_t* szSourceUri, JsValueRef* result)
{
	if (szScript == nullptr)
	{
		return JsErrorNullArgument;
	}

	if (szSourceUri == nullptr)
	{
		return JsErrorNullArgument;
	}

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

void ChakraHost::SetCallSyncHook(ChakraBridge::CallSyncHandler^ handler)
{
    this->callSyncHandler = handler;
}

void ChakraHost::SetFlushQueueImmediate(ChakraBridge::FlushQueueImmediateHandler^ handler)
{
    this->flushQueueImmediateHandler = handler;
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
    IfFailRet(DefineHostCallback(globalObject, L"nativeLoggingHook", NativeLoggingCallback, nullptr));

    return JsNoError;
}

JsErrorCode ChakraHost::InitNativeRequire()
{
	IfFailRet(DefineHostCallback(globalObject, L"nativeRequire", NativeRequire, this));

	return JsNoError;
}

JsErrorCode ChakraHost::InitNativeCallSyncHook()
{
    IfFailRet(DefineHostCallback(globalObject, L"nativeCallSyncHook", NativeCallSyncHook, this));

    return JsNoError;
}

JsErrorCode ChakraHost::InitFlushQueueImmediate()
{
    IfFailRet(DefineHostCallback(globalObject, L"nativeFlushQueueImmediate", NativeFlushQueueImmediate, this));

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
    IfFailRet(InitNativeCallSyncHook());
    IfFailRet(InitFlushQueueImmediate());

    return JsNoError;
}

JsErrorCode ChakraHost::Destroy()
{
    IfFailRet(JsSetCurrentContext(JS_INVALID_REFERENCE));
    IfFailRet(JsDisposeRuntime(runtime));

    return JsNoError;
}
