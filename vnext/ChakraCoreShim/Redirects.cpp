// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "Redirects.h"

#include <string>
#include <locale>
#include <map>
#include <memory>
#include <stdlib.h>

#include <assert.h>

// If we are not using edge mode jsrt, e.g. we are redirecting to legacy engine, then define the winapi_family
#ifndef USE_EDGEMODE_JSRT
#define WINAPI_FAMILY WINAPI_FAMILY_DESKTOP_APP
#endif // !USE_EDGEMODE_JSRT

#include <jsrt.h>

namespace Redirects {

struct ArrayBufferData;

// Global storages for APIs that we use from ChakraCore but are not available in jsrt9
static std::map<void*, void*> g_contextDataMap;
static std::map<::JsValueRef, std::shared_ptr<ArrayBufferData>> g_arrayBuffers;

// Helper structures for APIs that we use from ChakraCore but are not available in jsrt9
struct ArrayBufferData
{
	::JsValueRef Array = nullptr;
	void* Data = nullptr;
	unsigned int ByteLength = 0;
	::JsFinalizeCallback ActualFinalizeCallback = nullptr;
	void* CallbackState = nullptr;
	wchar_t* WideScript = nullptr;

	static void __stdcall FinalizeCallback(void* data)
	{
		std::unique_ptr<std::shared_ptr<ArrayBufferData>> thisPtr(static_cast<std::shared_ptr<ArrayBufferData>*>(data));
		if ((*thisPtr)->ActualFinalizeCallback)
			(*thisPtr)->ActualFinalizeCallback((*thisPtr)->CallbackState);

		// Delete our copy WideScript, if we had to do conversion during JsRun.
		if ((*thisPtr)->WideScript)
		{
			delete[]((*thisPtr)->WideScript);
			(*thisPtr)->WideScript = nullptr;
		}

		// Remove the arrayBuffer fromt he list of tracked buffers
		g_arrayBuffers.erase((*thisPtr)->Array);
	}
};

unsigned int JsAddRef(void* ref, unsigned int *count)
{
	return ::JsAddRef(ref, count);
}

unsigned int JsCallFunction(void* function, void** arguments, unsigned short argumentCount, void** result)
{
	return ::JsCallFunction(function, arguments, argumentCount, result);
}

unsigned int JsConstructObject(void* function, void** arguments, unsigned short argumentCount, void** result)
{
	return ::JsConstructObject(function, arguments, argumentCount, result);
}

unsigned int JsConvertValueToObject(void* value, void** object)
{
	return ::JsConvertValueToObject(value, object);
}

unsigned int JsConvertValueToString(void* value, void** stringValue)
{
	return ::JsConvertValueToString(value, stringValue);
}

unsigned int JsCopyString(void* value, char* buffer, size_t length, size_t* written)
{
	JsErrorCode result = JsNoError;

	int stringLength = 0;
	result = ::JsGetStringLength(value, &stringLength);
	if (result != JsNoError)
		return result;

	if (!buffer)
	{
		if (written)
			*written = length;
		return JsNoError;
	}

	const wchar_t* wzBuffer = new wchar_t[stringLength];
	size_t stringLength2 = 0;
	result = ::JsStringToPointer(value, &wzBuffer, &stringLength2);
	if (result != JsNoError)
		return result;

	wcstombs_s(&stringLength2, buffer, stringLength, wzBuffer, _TRUNCATE);

	if (written)
		*written = stringLength2;

	return result;
}

unsigned int JsCreateContext(void* runtime, void** newContext)
{
#ifdef USE_EDGEMODE_JSRT
	unsigned int returnResult = ::JsCreateContext(runtime, newContext);
#else
	unsigned int returnResult = ::JsCreateContext(runtime, nullptr, newContext);
#endif

	// Make all contexts created through this shim debuggable.
	// JsStartDebugging API expects an active context. So preverseve (any) current one to restore later.
	JsContextRef currentContext;
	::JsGetCurrentContext(&currentContext);

	::JsSetCurrentContext(*newContext);
#ifdef USE_EDGEMODE_JSRT
	::JsStartDebugging();
#else
	// TODO: DenizDem - Proper debugging support for jsrt9
	::JsStartDebugging(nullptr);
#endif

	// Restore
	::JsSetCurrentContext(currentContext);

	return returnResult;
}

unsigned int JsCreateError(void* message, void** error)
{
	return ::JsCreateError(message, error);
}

unsigned int JsCreateExternalArrayBuffer(void *data, unsigned int byteLength, void* finalizeCallback, void* callbackState, void** jsArrayBuffer)
{
	std::unique_ptr<std::shared_ptr<ArrayBufferData>> arrayBufferData(std::make_unique<std::shared_ptr<ArrayBufferData>>(std::make_shared<ArrayBufferData>()));
	(*arrayBufferData)->Data = data;
	(*arrayBufferData)->ByteLength = byteLength;
	(*arrayBufferData)->ActualFinalizeCallback = static_cast<::JsFinalizeCallback>(finalizeCallback);
	(*arrayBufferData)->CallbackState = callbackState;

	unsigned int result = ::JsCreateExternalObject(arrayBufferData.get(), static_cast<::JsFinalizeCallback>(ArrayBufferData::FinalizeCallback), jsArrayBuffer);
	if (result != JsNoError)
		return result;

	// Save the ArrayBuffer as well
	(*arrayBufferData)->Array = *jsArrayBuffer;
	g_arrayBuffers[*jsArrayBuffer] = *arrayBufferData.get();

	arrayBufferData.release();

	return JsNoError;
}

unsigned int JsCreateExternalObject(void *data, void* finalizeCallback, void** object)
{
	return ::JsCreateExternalObject(data, static_cast<::JsFinalizeCallback>(finalizeCallback), object);
}

unsigned int JsCreateFunction(void* nativeFunction, void* callbackState, void** function)
{
	return ::JsCreateFunction(static_cast<JsNativeFunction>(nativeFunction), callbackState, function);
}

unsigned int JsCreateNamedFunction(void* name, void* nativeFunction, void *callbackState, void** function)
{
	JsValueRef globalObject;
	unsigned int result = ::JsGetGlobalObject(&globalObject);
	if (result != JsNoError)
		return result;

	size_t nameLength = 0;
	const wchar_t* nameString = nullptr;
	result = ::JsStringToPointer(name, &nameString, &nameLength);
	if (result != JsNoError)
		return result;

	::JsPropertyIdRef propertyId;
	result = ::JsGetPropertyIdFromName(nameString, &propertyId);
	if (result != JsNoError)
		return result;

	result = ::JsCreateFunction(static_cast<::JsNativeFunction>(nativeFunction), callbackState, function);
	if (result != JsNoError)
		return result;

	result = ::JsSetProperty(globalObject, propertyId, *function, true);
	if (result != JsNoError)
		return result;

	return JsNoError;
}

unsigned int JsCreateObject(void** object)
{
	return ::JsCreateObject(object);
}

unsigned int JsCreateRuntime(int attributes, void* threadService, void** runtime)
{
#ifdef USE_EDGEMODE_JSRT
	return ::JsCreateRuntime(static_cast<::JsRuntimeAttributes>(attributes), static_cast<::JsThreadServiceCallback>(threadService), runtime);
#else
	return ::JsCreateRuntime(static_cast<::JsRuntimeAttributes>(attributes), JsRuntimeVersion11, static_cast<::JsThreadServiceCallback>(threadService), runtime);
#endif
}

unsigned int JsDisposeRuntime(void* runtime)
{
	return ::JsDisposeRuntime(runtime);
}

unsigned int JsDoubleToNumber(double doubleValue, void** value)
{
	return ::JsDoubleToNumber(doubleValue, value);
}

unsigned int JsGetAndClearException(void** exception)
{
	return ::JsGetAndClearException(exception);
}

unsigned int JsGetContextData(void* context, void **data)
{
	*data = g_contextDataMap[context];
	return JsNoError;
}

unsigned int JsGetCurrentContext(void** currentContext)
{
	return ::JsGetCurrentContext(currentContext);
}

unsigned int JsGetGlobalObject(void** globalObject)
{
	return ::JsGetGlobalObject(globalObject);
}

unsigned int JsGetIndexedProperty(void* object, void* index, void** result)
{
	return ::JsGetIndexedProperty(object, index, result);
}

unsigned int JsGetNullValue(void** nullValue)
{
	return ::JsGetNullValue(nullValue);
}

unsigned int JsGetOwnPropertyNames(void* object, void** propertyNames)
{
	return ::JsGetOwnPropertyNames(object, propertyNames);
}

unsigned int JsGetProperty(void* object, void* propertyId, void** value)
{
	return ::JsGetProperty(object, propertyId, value);
}

unsigned int JsGetPropertyIdFromName(const wchar_t *name, void** propertyId)
{
	return ::JsGetPropertyIdFromName(name, propertyId);
}

unsigned int JsGetStringLength(void* stringValue, int *length)
{
	return ::JsGetStringLength(stringValue, length);
}

unsigned int JsGetUndefinedValue(void** value)
{
	return ::JsGetUndefinedValue(value);
}

unsigned int JsGetValueType(void* value, unsigned int* type)
{
	return ::JsGetValueType(value, static_cast<JsValueType*>((void*) type));
}

unsigned int JsHasException(bool *hasException)
{
	return ::JsHasException(hasException);
}

unsigned int JsIntToNumber(int intValue, void** value)
{
	return ::JsIntToNumber(intValue, value);
}

unsigned int JsNumberToDouble(void* value, double *doubleValue)
{
	return ::JsNumberToDouble(value, doubleValue);
}

unsigned int JsNumberToInt(void* value, int *intValue)
{
	double doubleValue = 0;
	unsigned int errorCode = JsNumberToDouble(value, &doubleValue);
	if (errorCode != JsNoError)
		return errorCode;
	// Round and assign
	*intValue = static_cast<int>(doubleValue >= 0 ? doubleValue + 0.5 : doubleValue - 0.5);

	return JsNoError;
}

unsigned int JsPointerToString(const wchar_t *stringValue, size_t stringLength, void** value)
{
	return ::JsPointerToString(stringValue, stringLength, value);
}

unsigned int JsRelease(void* ref, unsigned int *count)
{
	return ::JsRelease(ref, count);
}

unsigned int JsRun(void* script, uintptr_t sourceContext, void* sourceUrl, unsigned int parseAttributes, void** scriptResult)
{
	// void* script must be the JsValueRef for the external array buffer
	std::shared_ptr<ArrayBufferData> arrayBufferData = g_arrayBuffers[script];
	if (!arrayBufferData)
		return JsErrorArgumentNotObject;

	wchar_t* wzScript = nullptr;

	const wchar_t* wzSourceUrl = nullptr;
	size_t sourceUrlLength = 0;
	unsigned int result = ::JsStringToPointer(sourceUrl, &wzSourceUrl, &sourceUrlLength);
	if (result != JsNoError)
		return result;

	// Take a look at the parse attributes to see the encoding of the array buffer
	if (parseAttributes & 0x2 /*JsParseScriptAttributeArrayBufferIsUtf16Encoded*/)
	{
		wzScript = static_cast<wchar_t*>(arrayBufferData->Data);
	}
	else // Buffer is utf8 encoded
	{
		const char* szScript = static_cast<const char*>(arrayBufferData->Data);
		size_t szScriptLength = strlen(szScript);
		// Reserve space for wzScript
		wzScript = new wchar_t[szScriptLength + 1];
		// Save this to delete later
		arrayBufferData->WideScript = wzScript;
		mbstowcs_s(nullptr, wzScript, szScriptLength + 1, szScript, szScriptLength);
	}

	return ::JsRunScript(wzScript, sourceContext, wzSourceUrl, scriptResult);
}

unsigned int JsRunScript(const wchar_t *script, uintptr_t sourceContext, const wchar_t *sourceUrl, void** result)
{
	return ::JsRunScript(script, sourceContext, sourceUrl, result);
}

unsigned int JsRunSerialized(
	/* _In_ JsValueRef */ void* /*buffer*/,
	/* _In_ JsSerializedLoadScriptCallback */ void* /*scriptLoadCallback*/,
	/* _In_ JsSourceContext */ uintptr_t /*sourceContext*/,
	/* _In_ JsValueRef */ void* /*sourceUrl*/,
	/* _Out_ JsValueRef* */ void** /*result*/)
{
	// 'JsRunSerialized' appears to be a ChakraCore-only API. Not sure if/how we can translate this to Chakra or JScript9.
	// For now, we just ensure that loading binaries does not fail due to missing symbols.
	assert(false);
	return JsErrorNotImplemented;
}

unsigned int JsSerializeScript(const wchar_t* /*script*/, void* /*buffer*/, unsigned int* /*bufferSize*/)
{
	// For now, we just ensure that loading binaries does not fail due to missing symbols.
	assert(false);
	return JsErrorNotImplemented;
}

unsigned int JsSetContextData(void* context, void *data)
{
	g_contextDataMap[context] = data;
	return JsNoError;
}

unsigned int JsSetCurrentContext(void* context)
{
	return ::JsSetCurrentContext(context);
}

unsigned int JsSetProperty(void* object, void* propertyId, void* value, bool useStrictRules)
{
	return ::JsSetProperty(object, propertyId, value, useStrictRules);
}

unsigned int JsSetRuntimeMemoryLimit(void* runtime, size_t memoryLimit)
{
	return ::JsSetRuntimeMemoryLimit(runtime, memoryLimit);
}

unsigned int JsStringToPointer(void* value, const wchar_t **stringValue, size_t *stringLength)
{
	return ::JsStringToPointer(value, stringValue, stringLength);
}

} // namespace Redirects
