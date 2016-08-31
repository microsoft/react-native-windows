#include "pch.h"
#include "ChakraHost.h"
#include "JsStringify.h"
#include "SerializedSourceContext.h"

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

JsValueRef InvokeConsole(const wchar_t* kind, JsValueRef callee, bool isConstructCall, JsValueRef *arguments, unsigned short argumentCount, void *callbackState)
{
#ifdef _DEBUG

    wchar_t buff[56];
    swprintf(buff, 56, L"[JS {%s}] ", kind);
    OutputDebugStringW(buff);

    // First argument is this-context, ignore...
    for (USHORT i = 1; i < argumentCount; i++)
    {
        std::set<JsValueRef> values;
        IfFailThrow(StringifyJsValue(arguments[i], 0, values), L"Failed to convert object to string");
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

JsErrorCode LoadByteCode(const wchar_t* szPath, BYTE** pData, HANDLE* hFile, HANDLE* hMap)
{
    *pData = nullptr;

    *hFile = CreateFile2(szPath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, OPEN_EXISTING, nullptr);
    if (*hFile == INVALID_HANDLE_VALUE)
    {
        return JsErrorFatal;
    }

    *hMap = CreateFileMapping(*hFile, nullptr, PAGE_READWRITE | SEC_RESERVE, 0, 0, L"ReactNativeMapping");
    if (*hMap == NULL)
    {
        CloseHandle(*hFile);
        return JsErrorFatal;
    }

    *pData = (BYTE*)MapViewOfFile(*hMap, FILE_MAP_READ | FILE_MAP_WRITE, 0, 0, 0);
    if (*pData == NULL)
    {
        CloseHandle(*hMap);
        CloseHandle(*hFile);
        return JsErrorFatal;
    }

    return JsNoError;
}

JsErrorCode LoadFileContents(const wchar_t* szPath, wchar_t** pszData)
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

JsErrorCode ChakraHost::RunSerializedScript(const wchar_t* szPath, const wchar_t* szSerializedPath, const wchar_t* szSourceUri, JsValueRef* result)
{
    HANDLE hFile = NULL;
    HANDLE hMap = NULL;
    JsErrorCode status = JsNoError;
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
    else
    {
        IfFailRet(LoadByteCode(szSerializedPath, &buffer, &hFile, &hMap));
    }

    SerializedSourceContext* context = new SerializedSourceContext();
    context->byteBuffer = buffer;
    context->scriptBuffer = szScriptBuffer;
    context->fileHandle = hFile;
    context->mapHandle = hMap;

    IfFailRet(JsRunSerializedScriptWithCallback(&LoadSourceCallback, &UnloadSourceCallback, buffer, (JsSourceContext)context, szSourceUri, result));
    return status;
}

JsErrorCode ChakraHost::RunScript(const wchar_t* szFileName, const wchar_t* szSourceUri, JsValueRef* result)
{
    wchar_t* contents = nullptr;
    IfFailRet(LoadFileContents(szFileName, &contents));

    JsErrorCode status = JsRunScript(contents, currentSourceContext++, szSourceUri, result);
    free(contents);

    return status;
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

    IfFailRet(DefineHostCallback(consoleObject, L"info", ConsoleInfo, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"log", ConsoleLog, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"warn", ConsoleWarn, nullptr));
    IfFailRet(DefineHostCallback(consoleObject, L"error", ConsoleError, nullptr));

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
