#include "pch.h"
#include "JsStringify.h"
#include "asprintf.h"

USHORT DEFAULT_MAX_DEPTH = 6;
USHORT DEFAULT_ARRAY_MAX_LENGTH = 50;
const wchar_t* DEFAULT_TRUNCATED_VALUE = L" ... ";
const wchar_t* DEFAULT_RECURSIVE_VALUE = L" recursive reference ";

JsErrorCode StringifyJsValue(JsValueRef value, USHORT depth, std::set<JsValueRef> seen)
{
    JsValueType type;
    IfFailRet(JsGetValueType(value, &type));

    switch (type)
    {
    case JsUndefined:
        OutputDebugStringW(L"undefined");
        break;
    case JsNull:
        OutputDebugStringW(L"null");
        break;
    case JsBoolean:
        IfFailRet(StringifyJsBoolean(value));
        break;
    case JsString:
        IfFailRet(StringifyJsString(value));
        break;
    case JsObject:
        IfFailRet(StringifyJsObject(value, depth, seen));
        break;
    case JsFunction:
        IfFailRet(StringifyJsFunction(value));
        break;
    case JsTypedArray:
        IfFailRet(StringifyJsTypedArray(value));
        break;
    case JsArray:
        IfFailRet(StringifyJsArray(value, depth, seen));
        break;
    case JsError:
    case JsNumber:
    case JsSymbol:
    case JsArrayBuffer:
        IfFailRet(StringifyToString(value));
        break;
    }

    return JsNoError;
}

JsErrorCode StringifyJsBoolean(JsValueRef value)
{
    bool bResult;
    IfFailRet(JsBooleanToBool(value, &bResult));
    OutputDebugStringW(bResult ? L"true" : L"false");
    return JsNoError;
}

JsErrorCode StringifyJsArray(JsValueRef value, USHORT depth, std::set<JsValueRef> seen)
{
    std::pair<std::set<JsValueRef>::iterator, bool> ret;
    ret = seen.insert(value);
    if (!ret.second)
    {
        OutputDebugStringW(DEFAULT_RECURSIVE_VALUE);
        return JsNoError;
    }

    if (depth > DEFAULT_MAX_DEPTH) {
        OutputDebugStringW(DEFAULT_TRUNCATED_VALUE);
        return JsNoError;
    }

    JsPropertyIdRef lengthId;
    JsValueRef lengthProp;
    int lengthValue;
    IfFailRet(JsGetPropertyIdFromName(L"length", &lengthId));
    IfFailRet(JsGetProperty(value, lengthId, &lengthProp));
    IfFailRet(JsNumberToInt(lengthProp, &lengthValue));

    OutputDebugStringW(L"[ ");

    for (int i = 0; i < lengthValue; i++)
    {
        if (i == DEFAULT_ARRAY_MAX_LENGTH)
        {
            OutputDebugStringW(DEFAULT_TRUNCATED_VALUE);
            break;
        }

        JsValueRef index, indexResult;
        IfFailRet(JsIntToNumber(i, &index));
        IfFailRet(JsGetIndexedProperty(value, index, &indexResult));
        IfFailRet(StringifyJsValue(indexResult, depth, seen));

        if (i != lengthValue - 1)
        {
            OutputDebugStringW(L", ");
        }
        else
        {
            OutputDebugStringW(L" ");
        }
    }

    OutputDebugStringW(L"]");

    return JsNoError;
}

JsErrorCode StringifyJsObject(JsValueRef value, USHORT depth, std::set<JsValueRef> seen)
{
    std::pair<std::set<JsValueRef>::iterator, bool> ret;
    ret = seen.insert(value);
    if (!ret.second)
    {
        OutputDebugStringW(DEFAULT_RECURSIVE_VALUE);
        return JsNoError;
    }

    if (depth > DEFAULT_MAX_DEPTH) {
        OutputDebugStringW(DEFAULT_TRUNCATED_VALUE);
        return JsNoError;
    }

    JsValueRef props;
    IfFailRet(JsGetOwnPropertyNames(value, &props));

    JsPropertyIdRef lengthId;
    JsValueRef lengthProp;
    int lengthValue;
    IfFailRet(JsGetPropertyIdFromName(L"length", &lengthId));
    IfFailRet(JsGetProperty(props, lengthId, &lengthProp));
    IfFailRet(JsNumberToInt(lengthProp, &lengthValue));

    OutputDebugStringW(L"{ ");

    for (int i = 0; i < lengthValue; i++)
    {
        JsPropertyIdRef propId;
        JsValueRef index, indexResult, prop;
        const wchar_t* szProp;
        size_t sProp;
        IfFailRet(JsIntToNumber(i, &index));
        IfFailRet(JsGetIndexedProperty(props, index, &indexResult));
        IfFailRet(JsStringToPointer(indexResult, &szProp, &sProp));
        IfFailRet(JsGetPropertyIdFromName(szProp, &propId));
        IfFailRet(JsGetProperty(value, propId, &prop));

        OutputDebugStringW(szProp);
        OutputDebugStringW(L": ");
        IfFailRet(StringifyJsValue(prop, depth + 1, seen));
        if (i != lengthValue - 1)
        {
            OutputDebugStringW(L", ");
        }
        else
        {
            OutputDebugStringW(L" ");
        }
    }

    OutputDebugStringW(L"}");

    return JsNoError;
}

JsErrorCode StringifyJsString(JsValueRef value)
{
    const wchar_t* szResult;
    size_t sResult;
    IfFailRet(JsStringToPointer(value, &szResult, &sResult));
    OutputDebugStringW(szResult);

    return JsNoError;
}

JsErrorCode StringifyJsFunction(JsValueRef value)
{
    JsPropertyIdRef nameId;
    JsValueRef nameObj;
    const wchar_t* szName;
    size_t sName;

    IfFailRet(JsGetPropertyIdFromName(L"name", &nameId));
    IfFailRet(JsGetProperty(value, nameId, &nameObj));
    IfFailRet(JsStringToPointer(nameObj, &szName, &sName));

    OutputDebugStringW(L"[Function");
    if (sName > 0)
    {
        wchar_t* szFn = NULL;
        if (_aswprintf(&szFn, L": %s", szName) < 0) { return JsErrorFatal; }
        OutputDebugStringW(szFn);
        free(szFn);
    }
    OutputDebugStringW(L"]");

    return JsNoError;
}

JsErrorCode StringifyJsTypedArray(JsValueRef value)
{
    JsTypedArrayType arrayType;
    JsValueRef arrayBuffer;
    UINT byteOffset, byteLength;
    IfFailRet(JsGetTypedArrayInfo(value, &arrayType, &arrayBuffer, &byteOffset, &byteLength));

    switch (arrayType)
    {
    case JsArrayTypeInt8:
        OutputDebugStringW(L"Int8Array ");
        break;
    case JsArrayTypeUint8:
        OutputDebugStringW(L"Uint8Array ");
        break;
    case JsArrayTypeUint8Clamped:
        OutputDebugStringW(L"Uint8ClampedArray ");
        break;
    case JsArrayTypeInt16:
        OutputDebugStringW(L"Int16Array ");
        break;
    case JsArrayTypeUint16:
        OutputDebugStringW(L"Uint16Array ");
        break;
    case JsArrayTypeInt32:
        OutputDebugStringW(L"Int32Array ");
        break;
    case JsArrayTypeUint32:
        OutputDebugStringW(L"Uint32Array ");
        break;
    case JsArrayTypeFloat32:
        OutputDebugStringW(L"Float32Array ");
        break;
    case JsArrayTypeFloat64:
        OutputDebugStringW(L"Float64Array ");
        break;
    }

    JsPropertyIdRef lengthId;
    JsValueRef lengthProp;
    int lengthValue;
    IfFailRet(JsGetPropertyIdFromName(L"length", &lengthId));
    IfFailRet(JsGetProperty(value, lengthId, &lengthProp));
    IfFailRet(JsNumberToInt(lengthProp, &lengthValue));

    OutputDebugStringW(L"[ ");

    for (int i = 0; i < lengthValue; i++)
    {
        if (i == DEFAULT_ARRAY_MAX_LENGTH)
        {
            OutputDebugStringW(DEFAULT_TRUNCATED_VALUE);
            break;
        }

        JsValueRef index, indexResult;
        IfFailRet(JsIntToNumber(i, &index));
        IfFailRet(JsGetIndexedProperty(value, index, &indexResult));
        IfFailRet(StringifyToString(indexResult));

        if (i != lengthValue - 1)
        {
            OutputDebugStringW(L", ");
        }
        else
        {
            OutputDebugStringW(L" ");
        }
    }

    OutputDebugStringW(L"]");

    return JsNoError;
}

JsErrorCode StringifyToString(JsValueRef value)
{
    JsValueRef resultString;
    const wchar_t* szStr;
    size_t sStr;
    IfFailRet(JsConvertValueToString(value, &resultString));
    IfFailRet(JsStringToPointer(resultString, &szStr, &sStr));
    OutputDebugStringW(szStr);

    return JsNoError;
}
