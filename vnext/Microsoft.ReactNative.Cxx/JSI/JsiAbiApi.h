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

// An ABI-safe wrapper for facebook::jsi::Buffer.
struct JsiByteBufferWrapper : implements<JsiByteBufferWrapper, IJsiByteBuffer> {
  JsiByteBufferWrapper(std::shared_ptr<facebook::jsi::Buffer const> const &buffer) noexcept;
  ~JsiByteBufferWrapper() noexcept;
  uint32_t Size();
  void GetData(JsiByteArrayUser const &useBytes);

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

  JsiValueData GetProperty(IJsiRuntime const &runtime, JsiPropertyNameIdData const &name);
  void SetProperty(IJsiRuntime const &runtime, JsiPropertyNameIdData const &name, JsiValueData const &value);
  Windows::Foundation::Collections::IVector<JsiPropertyNameIdData> GetPropertyNames(IJsiRuntime const &runtime);

  static void RegisterHostObject(JsiObjectData const &objectData, JsiHostObjectWrapper *hostObject) noexcept;
  static bool IsHostObject(JsiObjectData const &objectData) noexcept;
  static std::shared_ptr<facebook::jsi::HostObject> GetHostObject(JsiObjectData const &objectData) noexcept;

 private:
  std::shared_ptr<facebook::jsi::HostObject> m_hostObject;
  JsiObjectData m_objectData{};

  static std::mutex s_mutex;
  static std::unordered_map<uint64_t, JsiHostObjectWrapper *> s_objectDataToObjectWrapper;
};

// The function object that wraps up the facebook::jsi::HostFunctionType
struct JsiHostFunctionWrapper {
  // We only support new and move constructors.
  JsiHostFunctionWrapper(facebook::jsi::HostFunctionType &&hostFunction, uint32_t functionId) noexcept;
  JsiHostFunctionWrapper(JsiHostFunctionWrapper &&other) noexcept;
  ~JsiHostFunctionWrapper() noexcept;

  // Disable other ways to construct or modify the wrapper.
  JsiHostFunctionWrapper &operator=(JsiHostFunctionWrapper &&other) = delete;
  JsiHostFunctionWrapper(JsiHostFunctionWrapper const &other) = delete;
  JsiHostFunctionWrapper &operator=(JsiHostFunctionWrapper const &other) = delete;

  JsiValueData
  operator()(IJsiRuntime const &runtime, JsiValueData const &thisValue, array_view<JsiValueData const> args);

  static uint32_t GetNextFunctionId() noexcept;
  static void RegisterHostFunction(uint32_t functionId, JsiFunctionData const &functionData) noexcept;
  static bool IsHostFunction(JsiFunctionData const &functionData) noexcept;
  static facebook::jsi::HostFunctionType &GetHostFunction(JsiFunctionData const &functionData) noexcept;

 private:
  facebook::jsi::HostFunctionType m_hostFunction;
  JsiFunctionData m_functionData{};
  uint32_t m_functionId{};

  static std::mutex s_functionMutex;
  static std::atomic<uint32_t> s_functionIdGenerator;
  static std::unordered_map<uint32_t, JsiHostFunctionWrapper *> s_functionIdToFunctionWrapper;
  static std::unordered_map<uint64_t, JsiHostFunctionWrapper *> s_functionDataToFunctionWrapper;
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

 private: // Convert JSI to ABI-safe JSI values
  static JsiSymbolData AsJsiSymbolData(PointerValue const *pv) noexcept;
  static JsiStringData AsJsiStringData(PointerValue const *pv) noexcept;
  static JsiObjectData AsJsiObjectData(PointerValue const *pv) noexcept;
  static JsiPropertyNameIdData AsJsiPropertyNameIdData(PointerValue const *pv) noexcept;

  static JsiSymbolData AsJsiSymbolData(facebook::jsi::Symbol const &symbol) noexcept;
  static JsiStringData AsJsiStringData(facebook::jsi::String const &str) noexcept;
  static JsiObjectData AsJsiObjectData(facebook::jsi::Object const &obj) noexcept;
  static JsiPropertyNameIdData AsJsiPropertyNameIdData(facebook::jsi::PropNameID const &propertyId) noexcept;
  static JsiFunctionData AsJsiFunctionData(facebook::jsi::Function const &func) noexcept;
  static JsiWeakObjectData AsJsiWeakObjectData(facebook::jsi::WeakObject const &weakObject) noexcept;
  static JsiArrayData AsJsiArrayData(facebook::jsi::Array const &arr) noexcept;
  static JsiArrayBufferData AsJsiArrayBufferData(facebook::jsi::ArrayBuffer const &arrayBuffer) noexcept;
  static JsiValueData const &AsJsiValueData(facebook::jsi::Value const &value) noexcept;

  static JsiPropertyNameIdData MakeJsiPropertyNameIdData(facebook::jsi::PropNameID &&propertyId) noexcept;
  static JsiValueData MakeJsiValueData(facebook::jsi::Value &&value) noexcept;

 private: // Convert ABI-safe JSI to JSI values
  static facebook::jsi::PropNameID const *AsPropNameID(JsiPropertyNameIdData const *data) noexcept;
  static facebook::jsi::Value const *AsValue(JsiValueData const *data) noexcept;

  PointerValue *MakeSymbolValue(JsiSymbolData &&symbol) const noexcept;
  PointerValue *MakeStringValue(JsiStringData &&str) const noexcept;
  PointerValue *MakeObjectValue(JsiObjectData &&obj) const noexcept;
  PointerValue *MakePropNameIDValue(JsiPropertyNameIdData &&propertyId) const noexcept;

  facebook::jsi::Symbol MakeSymbol(JsiSymbolData &&symbol) const noexcept;
  facebook::jsi::String MakeString(JsiStringData &&str) const noexcept;
  facebook::jsi::Object MakeObject(JsiObjectData &&obj) const noexcept;
  facebook::jsi::PropNameID MakePropNameID(JsiPropertyNameIdData &&propertyId) const noexcept;
  facebook::jsi::Array MakeArray(JsiArrayData &&arr) noexcept;
  facebook::jsi::WeakObject MakeWeakObject(JsiWeakObjectData &&weakObject) const noexcept;
  facebook::jsi::Function MakeFunction(JsiFunctionData &&func) noexcept;
  facebook::jsi::Value MakeValue(JsiValueData &&value) const noexcept;

  // Allow access to the helper function
  friend struct JsiHostObjectWrapper;
  friend struct JsiHostFunctionWrapper;

 private: // PointerValue structures
  struct SymbolPointerValue : PointerValue {
    SymbolPointerValue(winrt::weak_ref<IJsiRuntime> &&weakRuntime, JsiSymbolData &&symbol) noexcept;
    void invalidate() override;

   private:
    JsiSymbolData m_symbol;
    winrt::weak_ref<IJsiRuntime> m_weakRuntime;
  };

  struct StringPointerValue : PointerValue {
    StringPointerValue(winrt::weak_ref<IJsiRuntime> &&weakRuntime, JsiStringData &&str) noexcept;
    void invalidate() override;

   private:
    JsiStringData m_string;
    winrt::weak_ref<IJsiRuntime> m_weakRuntime;
  };

  struct ObjectPointerValue : PointerValue {
    ObjectPointerValue(winrt::weak_ref<IJsiRuntime> &&weakRuntime, JsiObjectData &&obj) noexcept;
    void invalidate() override;

   private:
    JsiObjectData m_object;
    winrt::weak_ref<IJsiRuntime> m_weakRuntime;
  };

  struct PropNameIDPointerValue : PointerValue {
    PropNameIDPointerValue(winrt::weak_ref<IJsiRuntime> &&weakRuntime, JsiPropertyNameIdData &&propertyId) noexcept;
    void invalidate() override;

   private:
    JsiPropertyNameIdData m_propertyId;
    winrt::weak_ref<IJsiRuntime> m_weakRuntime;
  };

 private:
  IJsiRuntime m_runtime;
};

} // namespace winrt::Microsoft::ReactNative

#endif // MICROSOFT_REACTNATIVE_JSIABIAPI
