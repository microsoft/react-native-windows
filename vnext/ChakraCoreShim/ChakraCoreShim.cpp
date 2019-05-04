// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include <cstdint>  // for uint16_t

#include "Redirects.h"

#include "ChakraHeaders/ChakraCore.h"

// This file contains all the public API surface shim ChakraCore.dll exports.
// As such, all of the APIs are here are explicitly exported with proper name (e.g. with the actual
// ChakraCore export name, MyJsAddRef -> JsAddRef) in the relevant .def files in the same folder.
//
// As our react-native-win32.dll starts calling new ChakraCore exports that it didn't used to call,
// let's say JsFoo API, that export should also be added to this shim dll, by following these steps:
// 1. Add MyJsFoo export to this cpp file.
// 2. Add Redirects::JsFoo API in in Redirects.h
// 3. Export the MyJsFoo under the name JsFoo by adding JsFoo=MyJsFoo in x64 and x86 def files in this folder.
//
//
// ============================ DESIGN
// Here is the general strategy for the shim approach and some details for the design:
//
// 1) This shim dll can currently redirect to both ChakraRT and legacy Chakra(e.g.jscript9.dll) as a proof of concept.
//
// 2) Principle: This shim dll exports the exact same set APIs the react - native - win32.dll calls in
// real ChakraCore.dll, so that applications that used to load ChakraCore.dll can transparently continue to do so.
//
// That is why this file includes ChakraCore.h. The APIs this dll exports have the exact same signatures and
// types as ChakraCore APIs.
//
// 3) Given the API signatures are completely different between ChakraRT and legacy Chakra(jscript9.dll),
// this design adds a Redirect layer that does the proper redirection, which are all defined in Redirects.h.
//
// 4) Since the Redirect layer would shim calls to ChakraRT or jscript9.dll(based on USE_EDMODE_JSRT definition.
// More details in the Readme.md file), that means the Redirect layer cannot reference any types from ChakraCore
// or ChakraRT or jscript9.dll. After all, JsValueRef is actually defined in three different dlls in three
// different ways. There is a definition of JsValueRef in ChakraCore.h, ChakraRT.h and jsrt9.h, one
// for each engine.
//
// 5) Instead all types in the API signatures in Redirects layer should be "neutralized". That is,
// JsValueRef becomes void*, JsValueRef* becomes void**, JsValueType enum becomes int, similarly
// JsErrorCode becomes unsigned int.
//
// 6) Redirect layer receives the neutralized form of the call from the ChakraCoreShim entry points,
// then translates it either to ChakraRT or legacy Chakra.
//
// 7) Redirect layer receives a result from the actual JS engine. In the case of translating to ChakraRT,
// this would be the JsErrorCode defined in ChakraRT.h, and in the case of translating to legacy Chakra,
// this would be the JsErrorCode defined in jsrt9.h. Both of these types can be reduced to unsigned int.
//
// 8) Entry points in this cpp file, receives the unsigned int result from the Redirect layer, and then casts
// it to JsErrorCode type as defined in ChakraCommon.h header of ChakraCore and return it to caller
// (e.g.react - native - win32.dll) as if the call was actually made to ChakraCore.

JsErrorCode MyJsAddRef(JsRef ref, unsigned int *count)
{
	return static_cast<JsErrorCode>(Redirects::JsAddRef(ref, count));
}

JsErrorCode MyJsCallFunction(JsValueRef function, JsValueRef* arguments, unsigned short argumentCount, JsValueRef* result)
{
	return static_cast<JsErrorCode>(Redirects::JsCallFunction(function, arguments, argumentCount, result));
}

JsErrorCode MyJsConstructObject(JsValueRef function, JsValueRef *arguments, unsigned short argumentCount, JsValueRef *result)
{
	return static_cast<JsErrorCode>(Redirects::JsConstructObject(function, arguments, argumentCount, result));
}

JsErrorCode MyJsConvertValueToObject(JsValueRef value, JsValueRef *object)
{
	return static_cast<JsErrorCode>(Redirects::JsConvertValueToObject(value, object));
}

JsErrorCode MyJsConvertValueToString(JsValueRef value, JsValueRef* stringValue)
{
	return static_cast<JsErrorCode>(Redirects::JsConvertValueToString(value, stringValue));
}

JsErrorCode MyJsCopyString(JsValueRef value, char* buffer, size_t length, size_t* written)
{
	return static_cast<JsErrorCode>(Redirects::JsCopyString(value, buffer, length, written));
}

JsErrorCode MyJsCreateContext(JsRuntimeHandle runtime, JsContextRef* newContext)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateContext(runtime, newContext));
}

JsErrorCode MyJsCreateError(JsValueRef message, JsValueRef* error)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateError(message, error));
}

JsErrorCode MyJsCreateExternalArrayBuffer(void *data, unsigned int byteLength, JsFinalizeCallback finalizeCallback, void* callbackState, JsValueRef *result)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateExternalArrayBuffer(data, byteLength, finalizeCallback, callbackState, result));
}

JsErrorCode MyJsCreateExternalObject(void *data, JsFinalizeCallback finalizeCallback, JsValueRef *object)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateExternalObject(data, finalizeCallback, object));
}

JsErrorCode MyJsCreateFunction(JsNativeFunction nativeFunction, void *callbackState, JsValueRef *function)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateFunction(nativeFunction, callbackState, function));
}

JsErrorCode MyJsCreateNamedFunction(JsValueRef name, JsNativeFunction nativeFunction, void *callbackState, JsValueRef *function)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateNamedFunction(name, nativeFunction, callbackState, function));
}

JsErrorCode MyJsCreateObject(JsValueRef *object)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateObject(object));
}

JsErrorCode MyJsCreateRuntime(JsRuntimeAttributes attributes, JsThreadServiceCallback threadService, JsRuntimeHandle *runtime)
{
	return static_cast<JsErrorCode>(Redirects::JsCreateRuntime(attributes, threadService, runtime));
}

JsErrorCode MyJsDisposeRuntime(JsRuntimeHandle runtime)
{
	return static_cast<JsErrorCode>(Redirects::JsDisposeRuntime(runtime));
}

JsErrorCode MyJsDoubleToNumber(double doubleValue, JsValueRef *value)
{
	return static_cast<JsErrorCode>(Redirects::JsDoubleToNumber(doubleValue, value));
}

JsErrorCode MyJsGetAndClearException(JsValueRef *exception)
{
	return static_cast<JsErrorCode>(Redirects::JsGetAndClearException(exception));
}

JsErrorCode MyJsGetContextData(JsContextRef context, void **data)
{
	return static_cast<JsErrorCode>(Redirects::JsGetContextData(context, data));
}

JsErrorCode MyJsGetCurrentContext(JsContextRef *currentContext)
{
	return static_cast<JsErrorCode>(Redirects::JsGetCurrentContext(currentContext));
}

JsErrorCode MyJsGetGlobalObject(JsValueRef* globalObject)
{
	return static_cast<JsErrorCode>(Redirects::JsGetGlobalObject(globalObject));
}

JsErrorCode MyJsGetIndexedProperty(JsValueRef object, JsValueRef index, JsValueRef *result)
{
	return static_cast<JsErrorCode>(Redirects::JsGetIndexedProperty(object, index, result));
}

JsErrorCode MyJsGetNullValue(JsValueRef *nullValue)
{
	return static_cast<JsErrorCode>(Redirects::JsGetNullValue(nullValue));
}

JsErrorCode MyJsGetOwnPropertyNames(JsValueRef object, JsValueRef *propertyNames)
{
	return static_cast<JsErrorCode>(Redirects::JsGetOwnPropertyNames(object, propertyNames));
}

JsErrorCode MyJsGetProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef *value)
{
	return static_cast<JsErrorCode>(Redirects::JsGetProperty(object, propertyId, value));
}

JsErrorCode MyJsGetPropertyIdFromName(const wchar_t *name, JsPropertyIdRef *propertyId)
{
	return static_cast<JsErrorCode>(Redirects::JsGetPropertyIdFromName(name, propertyId));
}

JsErrorCode MyJsGetStringLength(JsValueRef stringValue, int *length)
{
	return static_cast<JsErrorCode>(Redirects::JsGetStringLength(stringValue, length));
}

JsErrorCode MyJsGetUndefinedValue(JsValueRef *value)
{
	return static_cast<JsErrorCode>(Redirects::JsGetUndefinedValue(value));
}

JsErrorCode MyJsGetValueType(JsValueRef value, JsValueType *type)
{
	return static_cast<JsErrorCode>(Redirects::JsGetValueType(value, reinterpret_cast<unsigned int*>(type)));
}

JsErrorCode MyJsHasException(bool *hasException)
{
	return static_cast<JsErrorCode>(Redirects::JsHasException(hasException));
}

JsErrorCode MyJsIntToNumber(int intValue, JsValueRef *value)
{
	return static_cast<JsErrorCode>(Redirects::JsIntToNumber(intValue, value));
}

JsErrorCode MyJsNumberToDouble(JsValueRef value, double *doubleValue)
{
	return static_cast<JsErrorCode>(Redirects::JsNumberToDouble(value, doubleValue));
}

JsErrorCode MyJsNumberToInt(JsValueRef value, int *intValue)
{
	return static_cast<JsErrorCode>(Redirects::JsNumberToInt(value, intValue));
}

JsErrorCode MyJsPointerToString(const wchar_t *stringValue, size_t stringLength, JsValueRef *value)
{
	return static_cast<JsErrorCode>(Redirects::JsPointerToString(stringValue, stringLength, value));
}

JsErrorCode MyJsRelease(JsRef ref, unsigned int *count)
{
	return static_cast<JsErrorCode>(Redirects::JsRelease(ref, count));
}

JsErrorCode MyJsRun(JsValueRef script, JsSourceContext sourceContext, JsValueRef sourceUrl, JsParseScriptAttributes parseAttributes, JsValueRef *result)
{
	return static_cast<JsErrorCode>(Redirects::JsRun(script, sourceContext, sourceUrl, parseAttributes, result));
}

JsErrorCode MyJsRunScript(const wchar_t *script, JsSourceContext sourceContext, const wchar_t *sourceUrl, JsValueRef *result)
{
	return static_cast<JsErrorCode>(Redirects::JsRunScript(script, sourceContext, sourceUrl, result));
}

JsErrorCode MyJsRunSerialized(
	_In_ JsValueRef buffer,
	_In_ JsSerializedLoadScriptCallback scriptLoadCallback,
	_In_ JsSourceContext sourceContext,
	_In_ JsValueRef sourceUrl,
	_Out_ JsValueRef* result)
{
	return static_cast<JsErrorCode>(Redirects::JsRunSerialized(buffer, scriptLoadCallback, sourceContext, sourceUrl, result));
}

JsErrorCode MyJsSerializeScript(
	_In_z_ const wchar_t *script,
	_Out_writes_to_opt_(*bufferSize, *bufferSize) BYTE *buffer,
	_Inout_ unsigned int *bufferSize)
{
  return static_cast<JsErrorCode>(Redirects::JsSerializeScript(script, buffer, bufferSize));
}

JsErrorCode MyJsSetContextData(JsContextRef context, void *data)
{
	return static_cast<JsErrorCode>(Redirects::JsSetContextData(context, data));
}

JsErrorCode MyJsSetCurrentContext(JsContextRef context)
{
	return static_cast<JsErrorCode>(Redirects::JsSetCurrentContext(context));
}

JsErrorCode MyJsSetProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef value, bool useStrictRules)
{
	return static_cast<JsErrorCode>(Redirects::JsSetProperty(object, propertyId, value, useStrictRules));
}

JsErrorCode MyJsSetRuntimeMemoryLimit(JsRuntimeHandle runtime, size_t memoryLimit)
{
  return static_cast<JsErrorCode>(Redirects::JsSetRuntimeMemoryLimit(runtime, memoryLimit));
}

JsErrorCode MyJsStringToPointer(JsValueRef value, const wchar_t **stringValue, size_t *stringLength)
{
	return static_cast<JsErrorCode>(Redirects::JsStringToPointer(value, stringValue, stringLength));
}

