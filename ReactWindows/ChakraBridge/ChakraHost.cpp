#include "pch.h"
#include "ChakraHost.h"
#include "JsIndexedModulesUnbundle.h"
#include "JsStringify.h"
#include "SerializedSourceContext.h"
#include <stdint.h>

const unsigned int MagicFileHeader = 0xFB0BD1E5;

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

wchar_t* LogLevel(int logLevel)
{
    switch (logLevel)
    {
    case 0:
        return L"Trace";
    case 1:
        return L"Info";
    case 2:
        return L"Warn";
    case 3:
        return L"Error";
    default:
        return L"Log";
    }
}

JsValueRef CALLBACK NativeLoggingCallback(JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
#ifdef _DEBUG
    wchar_t buff[56];
    double logLevelIndex;
    JsNumberToDouble(arguments[2], &logLevelIndex);
    swprintf(buff, 56, L"[JS %s] ", LogLevel((int)logLevelIndex));
    OutputDebugStringW(buff);
    StringifyJsString(arguments[1]);
    OutputDebugStringW(L"\n");
#endif
    return JS_INVALID_REFERENCE;
}

JsErrorCode ChakraHost::LoadByteCode(const wchar_t* szPath, BYTE** pData, HANDLE* hFile, HANDLE* hMap, bool bIsReadOnly)
{
	*pData = nullptr;

	DWORD dwShareMode = bIsReadOnly ? GENERIC_READ : GENERIC_READ | GENERIC_WRITE;
	*hFile = CreateFile2(szPath, dwShareMode, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
	if (*hFile == INVALID_HANDLE_VALUE)
	{
		return JsErrorFatal;
	}

	DWORD flProtect = (bIsReadOnly ? PAGE_READONLY : PAGE_READWRITE) | SEC_RESERVE;
	*hMap = CreateFileMapping(*hFile, nullptr, flProtect, 0, 0, L"ReactNativeMapping");
	if (*hMap == NULL)
	{
		CloseHandle(*hFile);
		return JsErrorFatal;
	}

	DWORD dwDesiredAccess = bIsReadOnly ? FILE_MAP_READ : FILE_MAP_READ | FILE_MAP_WRITE;
	*pData = (BYTE*)MapViewOfFile(*hMap, dwDesiredAccess, 0, 0, 0);
	if (*pData == NULL)
	{
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
		return JsErrorFatal;
	}

	fread(rawBytes, sizeof(char), lengthBytes, file);
	if (fclose(file))
	{
		return JsErrorFatal;
	}

	*pszData = (wchar_t *)calloc(lengthBytes + 1, sizeof(wchar_t));
	if (*pszData == nullptr)
	{
		free(rawBytes);
		return JsErrorFatal;
	}

	if (MultiByteToWideChar(CP_UTF8, 0, rawBytes, lengthBytes + 1, *pszData, lengthBytes + 1) == 0)
	{
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

JsErrorCode ChakraHost::SerializeScript(const wchar_t* szPath, const wchar_t* szSerializedPath)
{
    ULONG bufferSize = 0L;
    BYTE* buffer = nullptr;
    wchar_t* szScriptBuffer = nullptr;
    IfFailRet(LoadFileContents(szPath, &szScriptBuffer));

    if (!CompareLastWrite(szSerializedPath, szPath))
    {
        IfFailRet(JsSerializeScript(szScriptBuffer, buffer, &bufferSize));
        buffer = new BYTE[bufferSize];
        IfFailRet(JsSerializeScript(szScriptBuffer, buffer, &bufferSize));

        FILE* file;
        _wfopen_s(&file, szSerializedPath, L"wb");
        fwrite(buffer, sizeof(BYTE), bufferSize, file);
        fclose(file);
    }

    return JsNoError;
}

JsErrorCode ChakraHost::RunSerializedScript(const wchar_t* szPath, const wchar_t* szSerializedPath, const wchar_t* szSourceUri, JsValueRef* result)
{
    HANDLE hFile = NULL;
    HANDLE hMap = NULL;
    BYTE* buffer = nullptr;
    wchar_t* szScriptBuffer = nullptr;
    IfFailRet(LoadFileContents(szPath, &szScriptBuffer));

    if (!CompareLastWrite(szSerializedPath, szPath))
    {
        return JsErrorBadSerializedScript;
    }
    else
    {
        IfFailRet(LoadByteCode(szSerializedPath, &buffer, &hFile, &hMap, false));
    }

    SerializedSourceContext* context = new SerializedSourceContext();
    context->byteBuffer = buffer;
    context->scriptBuffer = szScriptBuffer;
    context->fileHandle = hFile;
    context->mapHandle = hMap;

    IfFailRet(JsRunSerializedScriptWithCallback(&LoadSourceCallback, &UnloadSourceCallback, buffer, (JsSourceContext)context, szSourceUri, result));
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
