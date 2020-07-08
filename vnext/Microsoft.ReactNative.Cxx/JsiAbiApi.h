// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once
#ifndef MICROSOFT_REACTNATIVE_JSIABIAPI
#define MICROSOFT_REACTNATIVE_JSIABIAPI

#include <mutex>
#include <unordered_map>
#include "Crash.h"
#include "jsi/jsi.h"
#include "winrt/Microsoft.ReactNative.h"

namespace winrt::Microsoft::ReactNative {

// Forward declare the facebook::jsi::Runtime implementation on top of ABI-safe IJsiRuntime.
struct JsiAbiRuntime;

// Used as an ABI-safe opaque value for pointer values.
using JsiPointerHandle = uint64_t;

// An ABI-safe wrapper for facebook::jsi::Buffer.
struct JsiBufferWrapper : implements<JsiBufferWrapper, IJsiBuffer> {
  JsiBufferWrapper(std::shared_ptr<facebook::jsi::Buffer const> const &buffer) noexcept;
  ~JsiBufferWrapper() noexcept;
  uint32_t Size();
  void GetData(JsiDataHandler const &handler);

 private:
  std::shared_ptr<facebook::jsi::Buffer const> m_buffer;
};

// A wrapper for ABI-safe JsiPreparedJavaScript.
struct JsiPreparedJavaScriptWrapper : facebook::jsi::PreparedJavaScript {
  JsiPreparedJavaScriptWrapper(JsiPreparedJavaScript const &preparedScript) noexcept;
  ~JsiPreparedJavaScriptWrapper() noexcept;
  JsiPreparedJavaScript const &Get() const noexcept;

 private:
  JsiPreparedJavaScript m_preparedScript;
};

// An ABI-safe wrapper for facebook::jsi::HostObject.
struct JsiHostObjectWrapper : implements<JsiHostObjectWrapper, IJsiHostObject> {
  JsiHostObjectWrapper(std::shared_ptr<facebook::jsi::HostObject> &&hostObject) noexcept;
  ~JsiHostObjectWrapper() noexcept;

  JsiValueData GetProperty(IJsiRuntime const &runtime, JsiPointerHandle name);
  void SetProperty(IJsiRuntime const &runtime, JsiPointerHandle name, JsiValueData const &value);
  Windows::Foundation::Collections::IVectorView<JsiPointerHandle> GetPropertyNames(IJsiRuntime const &runtime);

  static void RegisterHostObject(JsiPointerHandle handle, JsiHostObjectWrapper *hostObject) noexcept;
  static bool IsHostObject(JsiPointerHandle handle) noexcept;
  static std::shared_ptr<facebook::jsi::HostObject> GetHostObject(JsiPointerHandle handle) noexcept;

 private:
  std::shared_ptr<facebook::jsi::HostObject> m_hostObject;
  JsiPointerHandle m_objectHandle{};

  static std::mutex s_mutex;
  static std::unordered_map<JsiPointerHandle, JsiHostObjectWrapper *> s_objectHandleToObjectWrapper;
};

// The function object that wraps up the facebook::jsi::HostFunctionType
struct JsiHostFunctionWrapper {
  JsiHostFunctionWrapper(facebook::jsi::HostFunctionType &&hostFunction, uint32_t functionId) noexcept;
  JsiHostFunctionWrapper(JsiHostFunctionWrapper &&other) noexcept;
  JsiHostFunctionWrapper &operator=(JsiHostFunctionWrapper &&other) noexcept;
  JsiHostFunctionWrapper(JsiHostFunctionWrapper const &other) = delete;
  JsiHostFunctionWrapper &operator=(JsiHostFunctionWrapper const &other) = delete;
  ~JsiHostFunctionWrapper() noexcept;

  JsiValueData
  operator()(IJsiRuntime const &runtime, JsiValueData const &thisValue, array_view<JsiValueData const> args);

  static uint32_t GetNextFunctionId() noexcept;
  static void RegisterHostFunction(uint32_t functionId, JsiPointerHandle handle) noexcept;
  static bool IsHostFunction(JsiPointerHandle functionHandle) noexcept;
  static facebook::jsi::HostFunctionType &GetHostFunction(JsiPointerHandle functionHandle) noexcept;

 private:
  facebook::jsi::HostFunctionType m_hostFunction;
  JsiPointerHandle m_functionHandle{};
  uint32_t m_functionId{};

  static std::mutex s_functionMutex;
  static std::atomic<uint32_t> s_functionIdGenerator;
  static std::unordered_map<uint32_t, JsiHostFunctionWrapper *> s_functionIdToFunctionWrapper;
  static std::unordered_map<JsiPointerHandle, JsiHostFunctionWrapper *> s_functionHandleToFunctionWrapper;
};

// JSI runtime implementation as a wrapper for the ABI-safe IJsiRuntime.
struct JsiAbiRuntime : facebook::jsi::Runtime {
  JsiAbiRuntime(IJsiRuntime const &runtime) noexcept;
  ~JsiAbiRuntime() noexcept;

  facebook::jsi::Value evaluateJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      const std::string &sourceURL) override;
  std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
      std::string sourceURL) override;
  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &js) override;
  facebook::jsi::Object global() override;
  std::string description() override;
  bool isInspectable() override;
  facebook::jsi::Instrumentation &instrumentation() override;

 protected:
  PointerValue *cloneSymbol(const PointerValue *pv) override;
  PointerValue *cloneString(const PointerValue *pv) override;
  PointerValue *cloneObject(const PointerValue *pv) override;
  PointerValue *clonePropNameID(const PointerValue *pv) override;

  facebook::jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) override;
  facebook::jsi::PropNameID createPropNameIDFromString(const facebook::jsi::String &str) override;
  std::string utf8(const facebook::jsi::PropNameID &propertyNameId) override;
  bool compare(const facebook::jsi::PropNameID &left, const facebook::jsi::PropNameID &right) override;

  std::string symbolToString(const facebook::jsi::Symbol &symbol) override;

  facebook::jsi::String createStringFromAscii(const char *str, size_t length) override;
  facebook::jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const facebook::jsi::String &str) override;

  facebook::jsi::Value createValueFromJsonUtf8(const uint8_t *json, size_t length) override;

  facebook::jsi::Object createObject() override;
  facebook::jsi::Object createObject(std::shared_ptr<facebook::jsi::HostObject> ho) override;
  std::shared_ptr<facebook::jsi::HostObject> getHostObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::HostFunctionType &getHostFunction(const facebook::jsi::Function &func) override;
  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  facebook::jsi::Value getProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) override;
  bool hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::PropNameID &name,
      const facebook::jsi::Value &value) override;
  void setPropertyValue(
      facebook::jsi::Object &obj,
      const facebook::jsi::String &name,
      const facebook::jsi::Value &value) override;

  bool isArray(const facebook::jsi::Object &obj) const override;
  bool isArrayBuffer(const facebook::jsi::Object &obj) const override;
  bool isFunction(const facebook::jsi::Object &obj) const override;
  bool isHostObject(const facebook::jsi::Object &obj) const override;
  bool isHostFunction(const facebook::jsi::Function &func) const override;
  facebook::jsi::Array getPropertyNames(const facebook::jsi::Object &obj) override;

  facebook::jsi::WeakObject createWeakObject(const facebook::jsi::Object &obj) override;
  facebook::jsi::Value lockWeakObject(const facebook::jsi::WeakObject &weakObj) override;

  facebook::jsi::Array createArray(size_t length) override;
  size_t size(const facebook::jsi::Array &arr) override;
  size_t size(const facebook::jsi::ArrayBuffer &arrayBuffer) override;
  uint8_t *data(const facebook::jsi::ArrayBuffer &arrayBuffer) override;
  facebook::jsi::Value getValueAtIndex(const facebook::jsi::Array &arr, size_t i) override;
  void setValueAtIndexImpl(facebook::jsi::Array &arr, size_t i, const facebook::jsi::Value &value) override;

  facebook::jsi::Function createFunctionFromHostFunction(
      const facebook::jsi::PropNameID &name,
      unsigned int paramCount,
      facebook::jsi::HostFunctionType func) override;
  facebook::jsi::Value call(
      const facebook::jsi::Function &func,
      const facebook::jsi::Value &jsThis,
      const facebook::jsi::Value *args,
      size_t count) override;
  facebook::jsi::Value
  callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) override;

  ScopeState *pushScope() override;
  void popScope(ScopeState *scope) override;

  bool strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const override;
  bool strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const override;
  bool strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const override;
  bool instanceOf(const facebook::jsi::Object &o, const facebook::jsi::Function &f) override;

  static facebook::jsi::String&& AsString(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::PropNameID &&AsPropNameID(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::Symbol &&AsSymbol(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::Object &&AsObject(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::WeakObject &&AsWeakObject(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::Function &&AsFunction(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::Array &&AsArray(JsiPointerHandle &&pointer) noexcept;
  static facebook::jsi::ArrayBuffer &&AsArrayBuffer(JsiPointerHandle &&pointer) noexcept;
  static JsiPointerHandle ToJsiPointerHandle(PointerValue const *pointerValue) noexcept;
  static JsiPointerHandle ToJsiPointerHandle(facebook::jsi::Pointer const &pointer) noexcept;
  static PointerValue *ToPointerValue(JsiPointerHandle pointerHandle) noexcept;

 private:
  IJsiRuntime m_runtime;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSIABIAPI
