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
  hstring What() noexcept;
  hstring Message() noexcept;
  hstring Stack() noexcept;
  JsiValueData Value() noexcept;

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

  JsiValueData EvaluateJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  ReactNative::JsiPreparedJavaScript PrepareJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl);
  JsiValueData EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js);
  JsiObjectData Global();
  hstring Description();
  bool IsInspectable();
  JsiSymbolData CloneSymbol(JsiSymbolData symbol);
  JsiStringData CloneString(JsiStringData str);
  JsiObjectData CloneObject(JsiObjectData obj);
  JsiPropertyNameIdData ClonePropertyNameId(JsiPropertyNameIdData propertyNameId);
  JsiPropertyNameIdData CreatePropertyNameIdFromAscii(array_view<uint8_t const> ascii);
  JsiPropertyNameIdData CreatePropertyNameIdFromUtf8(array_view<uint8_t const> utf8);
  JsiPropertyNameIdData CreatePropertyNameIdFromString(JsiStringData str);
  void PropertyNameIdToUtf8(JsiPropertyNameIdData propertyNameId, JsiByteArrayUser const &useUtf8String);
  bool PropertyNameIdEquals(JsiPropertyNameIdData left, JsiPropertyNameIdData right);
  void SymbolToUtf8(JsiSymbolData symbol, JsiByteArrayUser const &useUtf8String);
  JsiStringData CreateStringFromAscii(array_view<uint8_t const> ascii);
  JsiStringData CreateStringFromUtf8(array_view<uint8_t const> utf8);
  void StringToUtf8(JsiStringData str, JsiByteArrayUser const &useUtf8String);
  JsiValueData CreateValueFromJsonUtf8(array_view<uint8_t const> json);
  JsiObjectData CreateObject();
  JsiObjectData CreateObjectWithHostObject(IJsiHostObject const &hostObject);
  IJsiHostObject GetHostObject(JsiObjectData obj);
  JsiHostFunction GetHostFunction(JsiFunctionData func);
  JsiValueData GetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId);
  JsiValueData GetPropertyByName(JsiObjectData obj, JsiStringData name);
  bool HasPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId);
  bool HasPropertyByName(JsiObjectData obj, JsiStringData name);
  void SetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId, JsiValueData const &value);
  void SetPropertyByName(JsiObjectData obj, JsiStringData name, JsiValueData const &value);
  bool IsArray(JsiObjectData obj);
  bool IsArrayBuffer(JsiObjectData obj);
  bool IsFunction(JsiObjectData obj);
  bool IsHostObject(JsiObjectData obj);
  bool IsHostFunction(JsiFunctionData obj);
  JsiArrayData GetPropertyNames(JsiObjectData obj);
  JsiWeakObjectData CreateWeakObject(JsiObjectData obj);
  JsiValueData LockWeakObject(JsiWeakObjectData weakObject);
  JsiArrayData CreateArray(uint32_t size);
  uint32_t GetArraySize(JsiArrayData arr);
  uint32_t GetArrayBufferSize(JsiArrayBufferData arrayBuffer);
  void GetArrayBufferData(JsiArrayBufferData arrayBuffer, JsiByteArrayUser const &useArrayBytes);
  JsiValueData GetValueAtIndex(JsiArrayData arr, uint32_t index);
  void SetValueAtIndex(JsiArrayData arr, uint32_t index, JsiValueData const &value);
  JsiFunctionData CreateFunctionFromHostFunction(
      JsiPropertyNameIdData propNameId,
      uint32_t paramCount,
      JsiHostFunction const &hostFunc);
  JsiValueData Call(JsiFunctionData func, JsiValueData const &thisArg, array_view<JsiValueData const> args);
  JsiValueData CallAsConstructor(JsiFunctionData func, array_view<JsiValueData const> args);
  JsiScopeState PushScope();
  void PopScope(JsiScopeState scopeState);
  bool SymbolStrictEquals(JsiSymbolData left, JsiSymbolData right);
  bool StringStrictEquals(JsiStringData left, JsiStringData right);
  bool ObjectStrictEquals(JsiObjectData left, JsiObjectData right);
  bool InstanceOf(JsiObjectData obj, JsiFunctionData constructor);

  void ReleaseSymbol(JsiSymbolData const &symbol);
  void ReleaseString(JsiStringData const &str);
  void ReleaseObject(JsiObjectData const &obj);
  void ReleasePropertyNameId(JsiPropertyNameIdData const &propertyNameId);

  ReactNative::JsiError GetAndRemoveError() noexcept;
  void SetError(JsiErrorType errorType, hstring const &what, JsiValueData const &value) noexcept;
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
