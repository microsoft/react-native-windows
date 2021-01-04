// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "JsiError.g.h"
#include "JsiPreparedJavaScript.g.h"
#include "JsiRuntime.g.h"
#include <unordered_map>
#include "winrt/Microsoft.ReactNative.h"

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

// Wraps up the IJsiHostObject
struct HostObjectWrapper final : facebook::jsi::HostObject {
  HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept;

  facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &name) override;
  void set(facebook::jsi::Runtime &, const facebook::jsi::PropNameID &name, const facebook::jsi::Value &value) override;
  std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime &runtime) override;

  Microsoft::ReactNative::IJsiHostObject const &Get() const noexcept {
    return m_hostObject;
  }

 private:
  Microsoft::ReactNative::IJsiHostObject m_hostObject;
};

struct RuntimeAccessor;

struct JsiRuntime : JsiRuntimeT<JsiRuntime> {
  JsiRuntime(
      std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> &&runtimeHolder,
      std::shared_ptr<facebook::jsi::Runtime> &&runtime) noexcept;
  ~JsiRuntime() noexcept;

  static ReactNative::JsiRuntime FromRuntime(facebook::jsi::Runtime &runtime) noexcept;
  static ReactNative::JsiRuntime GetOrCreate(
      std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> const &jsiRuntimeHolder,
      std::shared_ptr<facebook::jsi::Runtime> const &jsiRuntime) noexcept;
  static ReactNative::JsiRuntime Create(
      std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> const &jsiRuntimeHolder,
      std::shared_ptr<facebook::jsi::Runtime> const &jsiRuntime) noexcept;

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

 public:
  struct HostFunctionCleaner {
    HostFunctionCleaner(int64_t hostFunctionId);
    ~HostFunctionCleaner();

   private:
    int64_t m_hostFunctionId{0};
  };

 private:
  void SetError(facebook::jsi::JSError const &jsError) noexcept;
  void SetError(facebook::jsi::JSINativeException const &nativeException) noexcept;

 private:
  std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> m_runtimeHolder;
  std::shared_ptr<facebook::jsi::Runtime> m_runtime;
  RuntimeAccessor *m_runtimeAccessor{};
  std::mutex m_mutex;
  ReactNative::JsiError m_error{nullptr};

  static std::mutex s_mutex;
  static std::unordered_map<uintptr_t, winrt::weak_ref<ReactNative::JsiRuntime>> s_jsiRuntimeMap;
  static std::unordered_map<int64_t, ReactNative::JsiHostFunction> s_jsiHostFunctionMap;
  static int64_t s_jsiNextHostFunctionId;
};

} // namespace winrt::Microsoft::ReactNative::implementation

namespace winrt::Microsoft::ReactNative::factory_implementation {
struct JsiRuntime : JsiRuntimeT<JsiRuntime, implementation::JsiRuntime> {};
} // namespace winrt::Microsoft::ReactNative::factory_implementation
