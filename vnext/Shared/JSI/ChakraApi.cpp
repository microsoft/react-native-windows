// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "ChakraApi.h"
#include <sstream>
#include <utility>
#include "Unicode.h"

namespace Microsoft::JSI {

//=============================================================================
// ChakraApi::JsRefHolder implementation
//=============================================================================

ChakraApi::JsRefHolder::JsRefHolder(JsRef ref) noexcept : m_ref{ref} {
  if (m_ref) {
    AddRef(m_ref);
  }
}

ChakraApi::JsRefHolder::JsRefHolder(JsRefHolder const &other) noexcept : m_ref{other.m_ref} {
  if (m_ref) {
    AddRef(m_ref);
  }
}

ChakraApi::JsRefHolder::JsRefHolder(JsRefHolder &&other) noexcept
    : m_ref{std::exchange(other.m_ref, JS_INVALID_REFERENCE)} {}

ChakraApi::JsRefHolder &ChakraApi::JsRefHolder::operator=(JsRefHolder const &other) noexcept {
  if (this != &other) {
    JsRefHolder temp{std::move(*this)};
    m_ref = other.m_ref;
    if (m_ref) {
      AddRef(m_ref);
    }
  }

  return *this;
}

ChakraApi::JsRefHolder &ChakraApi::JsRefHolder::operator=(JsRefHolder &&other) noexcept {
  if (this != &other) {
    JsRefHolder temp{std::move(*this)};
    m_ref = std::exchange(other.m_ref, JS_INVALID_REFERENCE);
  }

  return *this;
}

ChakraApi::JsRefHolder::~JsRefHolder() noexcept {
  if (m_ref) {
    // Clear m_ref before calling JsRelease on it to make sure that we always hold a valid m_ref.
    Release(std::exchange(m_ref, JS_INVALID_REFERENCE));
  }
}

//=============================================================================
// ChakraApi::ExceptionThrowerHolder implementation
//=============================================================================

/*static*/ thread_local ChakraApi::IExceptionThrower *ChakraApi::ExceptionThrowerHolder::tls_exceptionThrower{nullptr};

//=============================================================================
// ChakraApi implementation
//=============================================================================

[[noreturn]] void ChakraApi::ThrowJsException(JsErrorCode errorCode) {
  JsValueRef jsError{JS_INVALID_REFERENCE};
  ChakraVerifyElseCrash(JsGetAndClearException(&jsError) == JsNoError, "Cannot retrieve JS exception.");
  if (auto thrower = ExceptionThrowerHolder::Get()) {
    thrower->ThrowJsExceptionOverride(errorCode, jsError);
  } else {
    std::ostringstream errorString;
    errorString << "A call to Chakra API returned error code 0x" << std::hex << errorCode << '.';
    throw std::exception(errorString.str().c_str());
  }
}

[[noreturn]] void ChakraApi::ThrowNativeException(char const *errorMessage) {
  if (auto thrower = ExceptionThrowerHolder::Get()) {
    thrower->ThrowNativeExceptionOverride(errorMessage);
  } else {
    throw std::exception(errorMessage);
  }
}

/*static*/ JsRuntimeHandle ChakraApi::CreateRuntime(
    JsRuntimeAttributes attributes,
    JsThreadServiceCallback threadService) {
  JsRuntimeHandle runtime{JS_INVALID_REFERENCE};
  ChakraVerifyElseThrow(
      JsCreateRuntime(attributes, threadService, &runtime) == JsErrorCode::JsNoError, "Cannot create Chakra runtime.");
  return runtime;
}

/*static*/ void ChakraApi::DisposeRuntime(JsRuntimeHandle runtime) {
  ChakraVerifyElseThrow(JsDisposeRuntime(runtime) == JsErrorCode::JsNoError, "Cannot dispaose Chakra runtime.");
}

/*static*/ uint32_t ChakraApi::AddRef(JsRef ref) {
  uint32_t result{0};
  ChakraVerifyJsErrorElseThrow(JsAddRef(ref, &result));
  return result;
}

/*static*/ uint32_t ChakraApi::Release(JsRef ref) {
  uint32_t result{0};
  ChakraVerifyJsErrorElseThrow(JsRelease(ref, &result));
  return result;
}

/*static*/ JsContextRef ChakraApi::CreateContext(JsRuntimeHandle runtime) {
  JsContextRef context{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateContext(runtime, &context));
  return context;
}

/*static*/ JsContextRef ChakraApi::GetCurrentContext() {
  JsContextRef context{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetCurrentContext(&context));
  return context;
}

/*static*/ void ChakraApi::SetCurrentContext(JsContextRef context) {
  ChakraVerifyJsErrorElseThrow(JsSetCurrentContext(context));
}

/*static*/ JsPropertyIdRef ChakraApi::GetPropertyIdFromName(wchar_t const *name) {
  JsPropertyIdRef propertyId{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetPropertyIdFromName(name, &propertyId));
  return propertyId;
}

/*static*/ JsPropertyIdRef ChakraApi::GetPropertyIdFromString(JsValueRef value) {
  return GetPropertyIdFromName(StringToPointer(value).data());
}

/*static*/ JsPropertyIdRef ChakraApi::GetPropertyIdFromName(std::string_view name) {
  ChakraVerifyElseThrow(name.data(), "Property name cannot be a nullptr.");

  JsPropertyIdRef propertyId{JS_INVALID_REFERENCE};
  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCreatePropertyId API.
#ifdef CHAKRACORE
  ChakraVerifyJsErrorElseThrow(JsCreatePropertyId(name.data(), name.length(), &propertyId));
#else
  std::wstring utf16 = Common::Unicode::Utf8ToUtf16(name.data(), name.length());
  ChakraVerifyJsErrorElseThrow(JsGetPropertyIdFromName(utf16.data(), &propertyId));
#endif
  return propertyId;
}

/*static*/ wchar_t const *ChakraApi::GetPropertyNameFromId(JsPropertyIdRef propertyId) {
  ChakraVerifyElseThrow(
      GetPropertyIdType(propertyId) == JsPropertyIdTypeString,
      "It is illegal to retrieve the name associated with a property symbol.");

  wchar_t const *name{nullptr};
  ChakraVerifyJsErrorElseThrow(JsGetPropertyNameFromId(propertyId, &name));
  return name;
}

/*static*/ JsValueRef ChakraApi::GetPropertyStringFromId(JsPropertyIdRef propertyId) {
  return PointerToString(GetPropertyNameFromId(propertyId));
}

/*static*/ JsValueRef ChakraApi::GetSymbolFromPropertyId(JsPropertyIdRef propertyId) {
  ChakraVerifyElseThrow(
      GetPropertyIdType(propertyId) == JsPropertyIdTypeSymbol,
      "It is illegal to retrieve the symbol associated with a property name.");

  JsValueRef symbol{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetSymbolFromPropertyId(propertyId, &symbol));
  return symbol;
}

/*static*/ JsPropertyIdType ChakraApi::GetPropertyIdType(JsPropertyIdRef propertyId) {
  JsPropertyIdType type{JsPropertyIdType::JsPropertyIdTypeString};
  ChakraVerifyJsErrorElseThrow(JsGetPropertyIdType(propertyId, &type));
  return type;
}

/*static*/ JsPropertyIdRef ChakraApi::GetPropertyIdFromSymbol(JsValueRef symbol) {
  JsPropertyIdRef propertyId{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetPropertyIdFromSymbol(symbol, &propertyId));
  return propertyId;
}

/*static*/ JsPropertyIdRef ChakraApi::GetPropertyIdFromSymbol(std::wstring_view symbolDescription) {
  JsPropertyIdRef propertyId{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetPropertyIdFromSymbol(CreateSymbol(symbolDescription), &propertyId));
  return propertyId;
}

/*static*/ JsValueRef ChakraApi::CreateSymbol(JsValueRef symbolDescription) {
  JsValueRef symbol{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateSymbol(symbolDescription, &symbol));
  return symbol;
}

/*static*/ JsValueRef ChakraApi::CreateSymbol(std::wstring_view symbolDescription) {
  return CreateSymbol(PointerToString(symbolDescription));
}

/*static*/ JsValueRef ChakraApi::GetUndefinedValue() {
  JsValueRef undefinedValue{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetUndefinedValue(&undefinedValue));
  return undefinedValue;
}

/*static*/ JsValueRef ChakraApi::GetNullValue() {
  JsValueRef nullValue{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetNullValue(&nullValue));
  return nullValue;
}

/*static*/ JsValueRef ChakraApi::BoolToBoolean(bool value) {
  JsValueRef booleanValue{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsBoolToBoolean(value, &booleanValue));
  return booleanValue;
}

/*static*/ bool ChakraApi::BooleanToBool(JsValueRef value) {
  bool boolValue{false};
  ChakraVerifyJsErrorElseThrow(JsBooleanToBool(value, &boolValue));
  return boolValue;
}

/*static*/ JsValueType ChakraApi::GetValueType(JsValueRef value) {
  JsValueType type{JsValueType::JsUndefined};
  ChakraVerifyJsErrorElseThrow(JsGetValueType(value, &type));
  return type;
}

/*static*/ JsValueRef ChakraApi::DoubleToNumber(double value) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsDoubleToNumber(value, &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::IntToNumber(int32_t value) {
  JsValueRef numberValue{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsIntToNumber(value, &numberValue));
  return numberValue;
}

/*static*/ double ChakraApi::NumberToDouble(JsValueRef value) {
  double doubleValue{0};
  ChakraVerifyJsErrorElseThrow(JsNumberToDouble(value, &doubleValue));
  return doubleValue;
}

/*static*/ int32_t ChakraApi::NumberToInt(JsValueRef value) {
  int intValue{0};
  ChakraVerifyJsErrorElseThrow(JsNumberToInt(value, &intValue));
  return intValue;
}

/*static*/ JsValueRef ChakraApi::PointerToString(std::wstring_view value) {
  ChakraVerifyElseThrow(value.data(), "Cannot convert a nullptr to a JS string.");

  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsPointerToString(value.data(), value.size(), &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::PointerToString(std::string_view value) {
  ChakraVerifyElseThrow(value.data(), "Cannot convert a nullptr to a JS string.");

  // ChakraCore  API helps to reduce cost of UTF-8 to UTF-16 conversion.
#ifdef CHAKRACORE
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateString(value.data(), value.length(), &result));
  return result;
#else
  return PointerToString(Common::Unicode::Utf8ToUtf16(value));
#endif
}

/*static*/ std::wstring_view ChakraApi::StringToPointer(JsValueRef string) {
  wchar_t const *utf16{nullptr};
  size_t length{0};
  ChakraVerifyJsErrorElseThrow(JsStringToPointer(string, &utf16, &length));
  return {utf16, length};
}

/*static*/ std::string ChakraApi::StringToStdString(JsValueRef string) {
  ChakraVerifyElseThrow(
      GetValueType(string) == JsString, "Cannot convert a non JS string ChakraObjectRef to a std::string.");

  // We use a #ifdef here because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore's JsCopyString API.
#ifdef CHAKRACORE
  size_t length{0};
  ChakraVerifyJsErrorElseThrow(JsCopyString(string, nullptr, 0, &length));

  std::string result(length, 'a');
  ChakraVerifyJsErrorElseThrow(JsCopyString(string, result.data(), result.length(), &length));

  ChakraVerifyElseThrow(length == result.length(), "Failed to convert a JS string to a std::string.");
  return result;
#else
  return Common::Unicode::Utf16ToUtf8(StringToPointer(string));
#endif
}

/*static*/ JsValueRef ChakraApi::ConvertValueToString(JsValueRef value) {
  JsValueRef stringValue{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsConvertValueToString(value, &stringValue));
  return stringValue;
}

/*static*/ JsValueRef ChakraApi::GetGlobalObject() {
  JsValueRef globalObject{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetGlobalObject(&globalObject));
  return globalObject;
}

/*static*/ JsValueRef ChakraApi::CreateObject() {
  JsValueRef object{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateObject(&object));
  return object;
}

/*static*/ JsValueRef ChakraApi::CreateExternalObject(void *data, JsFinalizeCallback finalizeCallback) {
  JsValueRef object{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateExternalObject(data, finalizeCallback, &object));
  return object;
}

/*static*/ JsValueRef ChakraApi::GetPrototype(JsValueRef object) {
  JsValueRef prototype{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetPrototype(object, &prototype));
  return prototype;
}

/*static*/ bool ChakraApi::InstanceOf(JsValueRef object, JsValueRef constructor) {
  bool result{false};
  ChakraVerifyJsErrorElseThrow(JsInstanceOf(object, constructor, &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::GetProperty(JsValueRef object, JsPropertyIdRef propertyId) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetProperty(object, propertyId, &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::GetOwnPropertyNames(JsValueRef object) {
  JsValueRef propertyNames{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetOwnPropertyNames(object, &propertyNames));
  return propertyNames;
}

/*static*/ void ChakraApi::SetProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef value) {
  ChakraVerifyJsErrorElseThrow(JsSetProperty(object, propertyId, value, /*useStringRules:*/ true));
}

/*static*/ bool ChakraApi::HasProperty(JsValueRef object, JsPropertyIdRef propertyId) {
  bool hasProperty{false};
  ChakraVerifyJsErrorElseThrow(JsHasProperty(object, propertyId, &hasProperty));
  return hasProperty;
}

/*static*/ bool
ChakraApi::DefineProperty(JsValueRef object, JsPropertyIdRef propertyId, JsValueRef propertyDescriptor) {
  bool isSucceeded{false};
  ChakraVerifyJsErrorElseThrow(JsDefineProperty(object, propertyId, propertyDescriptor, &isSucceeded));
  return isSucceeded;
}

/*static*/ JsValueRef ChakraApi::GetIndexedProperty(JsValueRef object, int32_t index) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsGetIndexedProperty(object, IntToNumber(index), &result));
  return result;
}

/*static*/ void ChakraApi::SetIndexedProperty(JsValueRef object, int32_t index, JsValueRef value) {
  ChakraVerifyJsErrorElseThrow(JsSetIndexedProperty(object, IntToNumber(index), value));
}

/*static*/ bool ChakraApi::StrictEquals(JsValueRef object1, JsValueRef object2) {
  bool result{false};
  ChakraVerifyJsErrorElseThrow(JsStrictEquals(object1, object2, &result));
  return result;
}

/*static*/ void *ChakraApi::GetExternalData(JsValueRef object) {
  void *data{nullptr};
  ChakraVerifyJsErrorElseThrow(JsGetExternalData(object, &data));
  return data;
}

/*static*/ JsValueRef ChakraApi::CreateArray(size_t length) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateArray(static_cast<unsigned int>(length), &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::CreateArrayBuffer(size_t byteLength) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateArrayBuffer(static_cast<unsigned int>(byteLength), &result));
  return result;
}

/*static*/ ChakraApi::Span<std::byte> ChakraApi::GetArrayBufferStorage(JsValueRef arrayBuffer) {
  BYTE *buffer{nullptr};
  unsigned int bufferLength{0};
  ChakraVerifyJsErrorElseThrow(JsGetArrayBufferStorage(arrayBuffer, &buffer, &bufferLength));
  return {reinterpret_cast<std::byte *>(buffer), bufferLength};
}

/*static*/ JsValueRef ChakraApi::CallFunction(JsValueRef function, Span<JsValueRef> args) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(
      JsCallFunction(function, args.begin(), static_cast<unsigned short>(args.size()), &result));
  return result;
}

/*static*/ JsValueRef ChakraApi::ConstructObject(JsValueRef function, Span<JsValueRef> args) {
  JsValueRef result{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(
      JsConstructObject(function, args.begin(), static_cast<unsigned short>(args.size()), &result));
  return result;
}

/*static*/ JsValueRef
ChakraApi::CreateNamedFunction(JsValueRef name, JsNativeFunction nativeFunction, void *callbackState) {
  JsValueRef function{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateNamedFunction(name, nativeFunction, callbackState, &function));
  return function;
}

/*static*/ bool ChakraApi::SetException(JsValueRef error) noexcept {
  // This method must not throw. We return false in case of error.
  return JsSetException(error) == JsNoError;
}

/*static*/ bool ChakraApi::SetException(std::string_view message) noexcept try {
  JsValueRef error{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateError(PointerToString(message), &error));
  return SetException(error);
} catch (...) {
  // This method must not throw. We return false in case of error.
  return false;
}

/*static*/ bool ChakraApi::SetException(std::wstring_view message) noexcept try {
  JsValueRef error{JS_INVALID_REFERENCE};
  ChakraVerifyJsErrorElseThrow(JsCreateError(PointerToString(message), &error));
  return SetException(error);
} catch (...) {
  // This method must not throw. We return false in case of error.
  return false;
}

} // namespace Microsoft::JSI
