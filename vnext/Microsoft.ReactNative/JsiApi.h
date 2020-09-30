// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "JsiError.g.h"
#include "JsiPreparedJavaScript.g.h"
#include "JsiRuntime.g.h"
#include <unordered_map>
#include "winrt/Microsoft.ReactNative.h"

// facebook::jsi::Runtime hides all methods that we need to call.
// We "open" them up here by redeclaring the private and protected keywords.
#define private public
#define protected public
#include "jsi/jsi.h"
#undef protected
#undef private

#include "ChakraRuntimeHolder.h"

namespace facebook::jsi {
class Runtime;
class Pointer;
} // namespace facebook::jsi

namespace winrt::Microsoft::ReactNative::implementation {

struct JsiError : JsiErrorT<JsiError> {
  JsiError() = default;

  JsiError(facebook::jsi::JSError &&jsError) noexcept;
  JsiError(facebook::jsi::JSINativeException &&nativeException) noexcept;

  JsiErrorType ErrorType() noexcept;
  hstring ErrorDetails() noexcept;
  hstring Message() noexcept;
  hstring Stack() noexcept;
  JsiValueRef Value() noexcept;

  void RethrowError();

 private:
  JsiErrorType const m_errorType;
  std::optional<facebook::jsi::JSError> const m_jsError;
  std::optional<facebook::jsi::JSINativeException> const m_nativeException;
};

struct JsiRuntime : JsiRuntimeT<JsiRuntime> {
  JsiRuntime(
      std::shared_ptr<::Microsoft::JSI::ChakraRuntimeHolder> runtimeHolder,
      std::shared_ptr<facebook::jsi::Runtime> runtime) noexcept;
  ~JsiRuntime() noexcept;

  static ReactNative::JsiRuntime FromRuntime(facebook::jsi::Runtime &runtime) noexcept;

 public: // JsiRuntime
  static Microsoft::ReactNative::JsiRuntime MakeChakraRuntime();

  JsiValueRef EvaluateJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  ReactNative::JsiPreparedJavaScript PrepareJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  JsiValueRef EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js);
  JsiObjectRef Global();
  hstring Description();
  bool IsInspectable();

  JsiSymbolRef CloneSymbol(JsiSymbolRef symbol);
  JsiStringRef CloneString(JsiStringRef str);
  JsiObjectRef CloneObject(JsiObjectRef obj);
  JsiPropertyIdRef ClonePropertyId(JsiPropertyIdRef propertyId);

  JsiPropertyIdRef CreatePropertyId(hstring const &name);
  JsiPropertyIdRef CreatePropertyIdFromAscii(array_view<uint8_t const> ascii);
  JsiPropertyIdRef CreatePropertyIdFromUtf8(array_view<uint8_t const> utf8);
  JsiPropertyIdRef CreatePropertyIdFromString(JsiStringRef str);
  hstring PropertyIdToString(JsiPropertyIdRef propertyId);
  void PropertyIdToUtf8(JsiPropertyIdRef propertyId, JsiByteArrayUser const &useUtf8String);
  bool PropertyIdEquals(JsiPropertyIdRef left, JsiPropertyIdRef right);

  hstring SymbolToString(JsiSymbolRef symbol);
  void SymbolToUtf8(JsiSymbolRef symbol, JsiByteArrayUser const &useUtf8String);

  JsiStringRef CreateString(hstring const &value);
  JsiStringRef CreateStringFromAscii(array_view<uint8_t const> utf8);
  JsiStringRef CreateStringFromUtf8(array_view<uint8_t const> utf8);
  hstring StringToString(JsiStringRef str);
  void StringToUtf8(JsiStringRef str, JsiByteArrayUser const &useUtf8String);

  JsiValueRef CreateValueFromJson(hstring const &json);
  JsiValueRef CreateValueFromJsonUtf8(array_view<uint8_t const> json);

  JsiObjectRef CreateObject();
  JsiObjectRef CreateObjectWithHostObject(IJsiHostObject const &hostObject);
  IJsiHostObject GetHostObject(JsiObjectRef obj);
  JsiHostFunction GetHostFunction(JsiObjectRef func);

  JsiValueRef GetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId);
  bool HasProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId);
  void SetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId, JsiValueRef const &value);
  JsiObjectRef GetPropertyIdArray(JsiObjectRef obj);

  bool IsArray(JsiObjectRef obj);
  bool IsArrayBuffer(JsiObjectRef obj);
  bool IsFunction(JsiObjectRef obj);
  bool IsHostObject(JsiObjectRef obj);
  bool IsHostFunction(JsiObjectRef obj);

  JsiWeakObjectRef CreateWeakObject(JsiObjectRef obj);
  JsiValueRef LockWeakObject(JsiWeakObjectRef weakObject);

  JsiObjectRef CreateArray(uint32_t size);
  uint32_t GetArraySize(JsiObjectRef arr);
  uint32_t GetArrayBufferSize(JsiObjectRef arrayBuffer);
  void GetArrayBufferData(JsiObjectRef arrayBuffer, JsiByteArrayUser const &useArrayBytes);
  JsiValueRef GetValueAtIndex(JsiObjectRef arr, uint32_t index);
  void SetValueAtIndex(JsiObjectRef arr, uint32_t index, JsiValueRef const &value);

  JsiObjectRef
  CreateFunctionFromHostFunction(JsiPropertyIdRef funcName, uint32_t paramCount, JsiHostFunction const &hostFunc);
  JsiValueRef Call(JsiObjectRef func, JsiValueRef const &thisArg, array_view<JsiValueRef const> args);
  JsiValueRef CallAsConstructor(JsiObjectRef func, array_view<JsiValueRef const> args);

  JsiScopeState PushScope();
  void PopScope(JsiScopeState scopeState);

  bool SymbolStrictEquals(JsiSymbolRef left, JsiSymbolRef right);
  bool StringStrictEquals(JsiStringRef left, JsiStringRef right);
  bool ObjectStrictEquals(JsiObjectRef left, JsiObjectRef right);
  bool InstanceOf(JsiObjectRef obj, JsiObjectRef constructor);

  void ReleaseSymbol(JsiSymbolRef const &symbol);
  void ReleaseString(JsiStringRef const &str);
  void ReleaseObject(JsiObjectRef const &obj);
  void ReleasePropertyId(JsiPropertyIdRef const &propertyId);

  ReactNative::JsiError GetAndClearError() noexcept;
  void SetError(JsiErrorType errorType, hstring const &errorDetails, JsiValueRef const &value) noexcept;
  static void RethrowJsiError(facebook::jsi::Runtime &runtime);

 private:
  void SetError(facebook::jsi::JSError const &jsError) noexcept;
  void SetError(facebook::jsi::JSINativeException const &nativeException) noexcept;

 private:
  std::shared_ptr<::Microsoft::JSI::ChakraRuntimeHolder> m_runtimeHolder;
  std::shared_ptr<facebook::jsi::Runtime> m_runtime;
  std::mutex m_mutex;
  ReactNative::JsiError m_error{nullptr};

  static std::mutex s_mutex;
  static std::map<uintptr_t, winrt::weak_ref<ReactNative::JsiRuntime>> s_jsiRuntimeMap;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct JsiRuntime : JsiRuntimeT<JsiRuntime, implementation::JsiRuntime> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
