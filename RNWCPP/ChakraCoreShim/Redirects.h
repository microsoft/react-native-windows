// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <cstdint>

namespace Redirects {

unsigned int JsAddRef(void* ref, unsigned int *count);
unsigned int JsCallFunction(void* function, void** arguments, unsigned short argumentCount, void** result);
unsigned int JsConstructObject(void* function, void** arguments, unsigned short argumentCount, void** result);
unsigned int JsConvertValueToObject(void* value, void** object);
unsigned int JsConvertValueToString(void* value, void** stringValue);

unsigned int JsCopyString(void*  value, char* buffer, size_t length, size_t* written);

unsigned int JsCreateContext(void* runtime, void** newContext);
unsigned int JsCreateError(void* message, void** error);
unsigned int JsCreateExternalArrayBuffer(void *data, unsigned int byteLength, void* finalizeCallback, void* callbackState, void** result);
unsigned int JsCreateExternalObject(void *data, void* finalizeCallback, void** object);
unsigned int JsCreateFunction(void* nativeFunction, void* callbackState, void** function);
unsigned int JsCreateNamedFunction(void* name, void* nativeFunction, void *callbackState, void** function);
unsigned int JsCreateObject(void** object);
unsigned int JsCreateRuntime(int attributes, void* threadService, void** runtime);
unsigned int JsDisposeRuntime(void* runtime);
unsigned int JsDoubleToNumber(double doubleValue, void** value);
unsigned int JsGetAndClearException(void** exception);
unsigned int JsGetContextData(void* context, void **data);
unsigned int JsGetCurrentContext(void** currentContext);
unsigned int JsGetGlobalObject(void** globalObject);
unsigned int JsGetIndexedProperty(void* object, void* index, void** result);
unsigned int JsGetNullValue(void** nullValue);
unsigned int JsGetOwnPropertyNames(void* object, void** propertyNames);
unsigned int JsGetProperty(void* object, void* propertyId, void** value);
unsigned int JsGetPropertyIdFromName(const wchar_t *name, void** propertyId);
unsigned int JsGetStringLength(void* stringValue, int *length);
unsigned int JsGetUndefinedValue(void** nullValue);
unsigned int JsGetValueType(void* value, unsigned int* type);
unsigned int JsHasException(bool *hasException);
unsigned int JsIntToNumber(int intValue, void** value);
unsigned int JsNumberToDouble(void* value, double *doubleValue);
unsigned int JsNumberToInt(void* value, int *intValue);
unsigned int JsPointerToString(const wchar_t *stringValue, size_t stringLength, void** value);
unsigned int JsRelease(void* ref, unsigned int *count);
unsigned int JsRun(void* script, uintptr_t sourceContext, void* sourceUrl, unsigned int parseAttributes, void** result);
unsigned int JsRunScript(const wchar_t *script, uintptr_t sourceContext, const wchar_t *sourceUrl, void** result);
unsigned int JsRunSerialized(void* buffer, void* scriptLoadCallback, uintptr_t sourceContext, void* sourceUrl, void** result);
unsigned int JsSerializeScript(const wchar_t *script, void* buffer, unsigned int* bufferSize);
unsigned int JsSetContextData(void* context, void *data);
unsigned int JsSetCurrentContext(void* context);
unsigned int JsSetProperty(void* object, void* propertyId, void* value, bool useStrictRules);
unsigned int JsSetRuntimeMemoryLimit(void* runtime, size_t memoryLimit);
unsigned int JsStringToPointer(void* value, const wchar_t **stringValue, size_t *stringLength);

}
