// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiApi.h"
#include "JsiPreparedJavaScript.g.cpp"
#include <crash/verifyElseCrash.h>

// facebook::jsi::Runtime hides all methods that we need to call.
// We "open" them up here by redeclaring the private and protected keywords.
#define private public
#define protected public
#include "jsi/jsi.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct JsiPreparedJavaScript : JsiPreparedJavaScriptT<JsiPreparedJavaScript> {
  JsiPreparedJavaScript() = default;
  JsiPreparedJavaScript(std::shared_ptr<facebook::jsi::PreparedJavaScript const> &&js) noexcept : m_js{std::move(js)} {}

 private:
  std::shared_ptr<facebook::jsi::PreparedJavaScript const> m_js;
};

// Wraps up the IJsiHostObject
struct HostObjectWrapper : facebook::jsi::HostObject {
  HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept;

  facebook::jsi::Value get(facebook::jsi::Runtime &runtime, const facebook::jsi::PropNameID &name) override;
  void set(facebook::jsi::Runtime &, const facebook::jsi::PropNameID &name, const facebook::jsi::Value &value) override;
  std::vector<facebook::jsi::PropNameID> getPropertyNames(facebook::jsi::Runtime &runtime) noexcept override;

 private:
  Microsoft::ReactNative::IJsiHostObject m_hostObject;
};

struct JsiBufferWrapper : facebook::jsi::Buffer {
  JsiBufferWrapper(array_view<const uint8_t> bytes) : m_bytes{bytes} {}

  size_t size() const override {
    return m_bytes.size();
  }

  const uint8_t *data() const override {
    return m_bytes.data();
  }

 private:
  array_view<const uint8_t> m_bytes;
};

//===========================================================================
// Helper methods
//===========================================================================

static facebook::jsi::String const *AsString(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::String const *>(pointer);
}

static facebook::jsi::PropNameID const *AsPropNameID(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::PropNameID const *>(pointer);
}

static facebook::jsi::Symbol const *AsSymbol(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::Symbol const *>(pointer);
}

static facebook::jsi::Object const *AsObject(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::Object const *>(pointer);
}

static facebook::jsi::WeakObject const *AsWeakObject(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::WeakObject const *>(pointer);
}

static facebook::jsi::Function const *AsFunction(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::Function const *>(pointer);
}

static facebook::jsi::Array const *AsArray(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::Array const *>(pointer);
}

static facebook::jsi::ArrayBuffer const *AsArrayBuffer(JsiPointerHandle pointer) noexcept {
  return reinterpret_cast<facebook::jsi::ArrayBuffer const *>(pointer);
}

static std::shared_ptr<facebook::jsi::HostObject> MakeHostObject(
    Microsoft::ReactNative::IJsiHostObject const &hostObject) {
  return std::make_shared<HostObjectWrapper>(hostObject);
}

static JsiPointerHandle ToJsiPointerHandle(facebook::jsi::Runtime::PointerValue const *pointerValue) noexcept {
  return reinterpret_cast<JsiPointerHandle>(pointerValue);
}

static JsiPointerHandle ToJsiPointerHandle(facebook::jsi::Pointer const &pointer) noexcept {
  return ToJsiPointerHandle(facebook::jsi::Runtime::getPointerValue(pointer));
}

static Microsoft::ReactNative::JsiValueData const &ToJsiValueData(facebook::jsi::Value const &value) noexcept {
  return reinterpret_cast<Microsoft::ReactNative::JsiValueData const &>(value);
}

static facebook::jsi::Runtime::PointerValue *ToPointerValue(JsiPointerHandle pointerHandle) {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(pointerHandle);
}

static facebook::jsi::Value &&ToValue(Microsoft::ReactNative::JsiValueData &&value) noexcept {
  return reinterpret_cast<facebook::jsi::Value &&>(value);
}

static facebook::jsi::Value const &ToValue(Microsoft::ReactNative::JsiValueData const &value) noexcept {
  return reinterpret_cast<facebook::jsi::Value const &>(value);
}

static facebook::jsi::HostFunctionType MakeHostFunction(Microsoft::ReactNative::JsiHostFunction const &hostFunc) {
  // TODO: implement runtime mapping
  // TODO: implement host function mapping
  return [hostFunc](
             facebook::jsi::Runtime &runtime,
             facebook::jsi::Value const &thisVal,
             facebook::jsi::Value const *args,
             size_t count) -> facebook::jsi::Value {
    auto argsData = reinterpret_cast<JsiValueData const *>(args);
    return ToValue(hostFunc(make<JsiRuntime>(runtime), ToJsiValueData(thisVal), {argsData, argsData + count}));
  };
}

//===========================================================================
// HostObjectWrapper implementation
//===========================================================================

HostObjectWrapper::HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept
    : m_hostObject{hostObject} {}

facebook::jsi::Value HostObjectWrapper::get(facebook::jsi::Runtime &runtime, facebook::jsi::PropNameID const &name) {
  auto valueData =
      m_hostObject.GetProperty(make<JsiRuntime>(runtime), ToJsiPointerHandle(runtime.clonePropNameID(name.ptr_)));
  return ToValue(std::move(valueData));
}

void HostObjectWrapper::set(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name,
    facebook::jsi::Value const &value) {
  // TODO: map between JsiRuntime and Runtime
  m_hostObject.SetProperty(make<JsiRuntime>(runtime), ToJsiPointerHandle(name), ToJsiValueData(value));
}

std::vector<facebook::jsi::PropNameID> HostObjectWrapper::getPropertyNames(facebook::jsi::Runtime &runtime) noexcept {
  std::vector<facebook::jsi::PropNameID> result;
  auto *jsiResult = reinterpret_cast<std::vector<JsiPointerHandle> *>(&result);
  // TODO: map between JsiRuntime and Runtime
  auto vectorView = m_hostObject.GetPropertyNames(make<JsiRuntime>(runtime));
  jsiResult->resize(vectorView.Size());
  vectorView.GetMany(0, *jsiResult);
  return result;
}

//===========================================================================
// JsiRuntime implementation
//===========================================================================

JsiRuntime::JsiRuntime(facebook::jsi::Runtime &runtime) noexcept : m_runtime{runtime} {}

Microsoft::ReactNative::JsiValueData JsiRuntime::EvaluateJavaScript(
    Microsoft::ReactNative::IJsiBuffer const &buffer,
    hstring const &sourceUrl) {
  facebook::jsi::Value result;
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    result = m_runtime.evaluateJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl));
  });
  return ToJsiValueData(std::move(result));
}

Microsoft::ReactNative::JsiPreparedJavaScript JsiRuntime::PrepareJavaScript(
    Microsoft::ReactNative::IJsiBuffer const &buffer,
    hstring const &sourceUrl) {
  Microsoft::ReactNative::JsiPreparedJavaScript result{nullptr};
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    result = make<JsiPreparedJavaScript>(
        m_runtime.prepareJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl)));
  });
  return result;
}

Microsoft::ReactNative::JsiValueData JsiRuntime::EvaluatePreparedJavaScript(
    Microsoft::ReactNative::JsiPreparedJavaScript const &js) {
  return ToJsiValueData(m_runtime.evaluatePreparedJavaScript(get_self<JsiPreparedJavaScript>(js)->m_js));
}

JsiPointerHandle JsiRuntime::Global() noexcept {
  return ToJsiPointerHandle(m_runtime.global());
}

hstring JsiRuntime::Description() {
  return to_hstring(m_runtime.description());
}

bool JsiRuntime::IsInspectable() {
  return m_runtime.isInspectable();
}

JsiPointerHandle JsiRuntime::CloneSymbol(JsiPointerHandle symbol) {
  return ToJsiPointerHandle(m_runtime.cloneSymbol(ToPointerValue(symbol)));
}

JsiPointerHandle JsiRuntime::CloneString(JsiPointerHandle str) {
  return ToJsiPointerHandle(m_runtime.cloneString(ToPointerValue(str)));
}

JsiPointerHandle JsiRuntime::CloneObject(JsiPointerHandle obj) {
  return ToJsiPointerHandle(m_runtime.cloneObject(ToPointerValue(obj)));
}

JsiPointerHandle JsiRuntime::ClonePropertyNameId(JsiPointerHandle propertyNameId) {
  return ToJsiPointerHandle(m_runtime.clonePropNameID(ToPointerValue(propertyNameId)));
}

JsiPointerHandle JsiRuntime::CreatePropertyNameIdFromAscii(array_view<uint8_t const> ascii) {
  return ToJsiPointerHandle(
      m_runtime.createPropNameIDFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
}

JsiPointerHandle JsiRuntime::CreatePropertyNameIdFromUtf8(array_view<uint8_t const> utf8) {
  return ToJsiPointerHandle(m_runtime.createPropNameIDFromUtf8(utf8.data(), utf8.size()));
}

JsiPointerHandle JsiRuntime::CreatePropertyNameIdFromString(JsiPointerHandle str) {
  return ToJsiPointerHandle(m_runtime.createPropNameIDFromString(*AsString(str)));
}

void JsiRuntime::PropertyNameIdToUtf8(
    JsiPointerHandle propertyNameId,
    Microsoft::ReactNative::JsiDataHandler const &utf8Handler) {
  std::string utf8 = m_runtime.utf8(*AsPropNameID(propertyNameId));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

bool JsiRuntime::ComparePropertyNameIds(JsiPointerHandle left, JsiPointerHandle right) {
  return m_runtime.compare(*AsPropNameID(left), *AsPropNameID(right));
}

void JsiRuntime::SymbolToUtf8(JsiPointerHandle symbol, Microsoft::ReactNative::JsiDataHandler const &utf8Handler) {
  std::string utf8 = m_runtime.symbolToString(*AsSymbol(symbol));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

JsiPointerHandle JsiRuntime::CreateStringFromAscii(array_view<uint8_t const> ascii) {
  return ToJsiPointerHandle(
      m_runtime.createStringFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
}

JsiPointerHandle JsiRuntime::CreateStringFromUtf8(array_view<uint8_t const> utf8) {
  return ToJsiPointerHandle(m_runtime.createStringFromUtf8(utf8.data(), utf8.size()));
}

void JsiRuntime::StringToUtf8(JsiPointerHandle str, Microsoft::ReactNative::JsiDataHandler const &utf8Handler) {
  std::string utf8 = m_runtime.utf8(*AsString(str));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

Microsoft::ReactNative::JsiValueData JsiRuntime::CreateValueFromJsonUtf8(array_view<uint8_t const> json) {
  return ToJsiValueData(m_runtime.createValueFromJsonUtf8(json.data(), json.size()));
}

JsiPointerHandle JsiRuntime::CreateObject() {
  return ToJsiPointerHandle(m_runtime.createObject());
}

JsiPointerHandle JsiRuntime::CreateObjectWithHostObject(Microsoft::ReactNative::IJsiHostObject const &hostObject) {
  return ToJsiPointerHandle(m_runtime.createObject(MakeHostObject(hostObject)));
}

Microsoft::ReactNative::IJsiHostObject JsiRuntime::GetHostObject(JsiPointerHandle obj) {
  auto hostObject = m_runtime.getHostObject(*AsObject(obj));
  // TODO: using static_pointer_cast is unsafe here. How to use shared_ptr without RTTI?
  auto wrapper = std::static_pointer_cast<HostObjectWrapper>(hostObject);
  return wrapper ? wrapper->m_hostObject : nullptr;
}

Microsoft::ReactNative::JsiHostFunction JsiRuntime::GetHostFunction(JsiPointerHandle /*func*/) {
  // auto hostFunction = m_runtime.getHostFunction(AsFunction(func));
  // TODO: implement mapping
  return nullptr;
}

Microsoft::ReactNative::JsiValueData JsiRuntime::GetProperty(JsiPointerHandle obj, JsiPointerHandle propertyNameId) {
  return ToJsiValueData(m_runtime.getProperty(*AsObject(obj), *AsPropNameID(propertyNameId)));
}

Microsoft::ReactNative::JsiValueData JsiRuntime::GetPropertyWithString(JsiPointerHandle obj, JsiPointerHandle nameStr) {
  return ToJsiValueData(m_runtime.getProperty(*AsObject(obj), *AsString(nameStr)));
}

bool JsiRuntime::HasProperty(JsiPointerHandle obj, JsiPointerHandle propertyNameId) {
  return m_runtime.hasProperty(*AsObject(obj), *AsPropNameID(propertyNameId));
}

bool JsiRuntime::HasPropertyWithString(JsiPointerHandle obj, JsiPointerHandle nameStr) {
  return m_runtime.hasProperty(*AsObject(obj), *AsString(nameStr));
}

void JsiRuntime::SetProperty(
    JsiPointerHandle obj,
    JsiPointerHandle propertyNameId,
    Microsoft::ReactNative::JsiValueData const &value) {
  m_runtime.setPropertyValue(
      const_cast<facebook::jsi::Object &>(*AsObject(obj)), *AsPropNameID(propertyNameId), ToValue(value));
}

void JsiRuntime::SetPropertyWithString(
    JsiPointerHandle obj,
    JsiPointerHandle nameStr,
    Microsoft::ReactNative::JsiValueData const &value) {
  m_runtime.setPropertyValue(const_cast<facebook::jsi::Object &>(*AsObject(obj)), *AsString(nameStr), ToValue(value));
}

bool JsiRuntime::IsArray(JsiPointerHandle obj) {
  return m_runtime.isArray(*AsObject(obj));
}

bool JsiRuntime::IsArrayBuffer(JsiPointerHandle obj) {
  return m_runtime.isArrayBuffer(*AsObject(obj));
}

bool JsiRuntime::IsFunction(JsiPointerHandle obj) {
  return m_runtime.isFunction(*AsObject(obj));
}

bool JsiRuntime::IsHostObject(JsiPointerHandle obj) {
  return m_runtime.isHostObject(*AsObject(obj));
}

bool JsiRuntime::IsHostFunction(JsiPointerHandle obj) {
  return m_runtime.isHostFunction(*AsFunction(obj));
}

JsiPointerHandle JsiRuntime::GetPropertyNameArray(JsiPointerHandle obj) {
  return ToJsiPointerHandle(m_runtime.getPropertyNames(*AsObject(obj)));
}

JsiPointerHandle JsiRuntime::CreateWeakObject(JsiPointerHandle obj) {
  return ToJsiPointerHandle(m_runtime.createWeakObject(*AsObject(obj)));
}

Microsoft::ReactNative::JsiValueData JsiRuntime::LockWeakObject(JsiPointerHandle weakObject) {
  return ToJsiValueData(m_runtime.lockWeakObject(*AsWeakObject(weakObject)));
}

JsiPointerHandle JsiRuntime::CreateArray(uint32_t size) {
  return ToJsiPointerHandle(m_runtime.createArray(size));
}

uint32_t JsiRuntime::GetArraySize(JsiPointerHandle arr) {
  return static_cast<uint32_t>(m_runtime.size(*AsArray(arr)));
}

uint32_t JsiRuntime::GetArrayBufferSize(JsiPointerHandle arrayBuffer) {
  return static_cast<uint32_t>(m_runtime.size(*AsArrayBuffer(arrayBuffer)));
}

void JsiRuntime::ArrayBufferToUtf8(
    JsiPointerHandle arrayBuffer,
    Microsoft::ReactNative::JsiDataHandler const &utf8Handler) {
  auto data = m_runtime.data(*AsArrayBuffer(arrayBuffer));
  return utf8Handler({data, data + m_runtime.size(*AsArrayBuffer(arrayBuffer))});
}

Microsoft::ReactNative::JsiValueData JsiRuntime::GetValueAtIndex(JsiPointerHandle arr, uint32_t index) {
  return ToJsiValueData(AsArray(arr)->getValueAtIndex(m_runtime, index));
}

void JsiRuntime::SetValueAtIndex(
    JsiPointerHandle arr,
    uint32_t index,
    Microsoft::ReactNative::JsiValueData const &value) {
  m_runtime.setValueAtIndexImpl(const_cast<facebook::jsi::Array &>(*AsArray(arr)), index, ToValue(value));
}

JsiPointerHandle JsiRuntime::CreateFunctionFromHostFunction(
    JsiPointerHandle propNameId,
    uint32_t paramCount,
    Microsoft::ReactNative::JsiHostFunction const &hostFunc) {
  return ToJsiPointerHandle(
      m_runtime.createFunctionFromHostFunction(*AsPropNameID(propNameId), paramCount, MakeHostFunction(hostFunc)));
}

Microsoft::ReactNative::JsiValueData JsiRuntime::Call(
    JsiPointerHandle func,
    Microsoft::ReactNative::JsiValueData const &thisValue,
    array_view<Microsoft::ReactNative::JsiValueData const> args) {
  return ToJsiValueData(m_runtime.call(
      *AsFunction(func), ToValue(thisValue), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
}

Microsoft::ReactNative::JsiValueData JsiRuntime::CallAsConstructor(
    JsiPointerHandle func,
    array_view<Microsoft::ReactNative::JsiValueData const> args) {
  return ToJsiValueData(m_runtime.callAsConstructor(
      *AsFunction(func), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
}

JsiScopeHandle JsiRuntime::PushScope() {
  return reinterpret_cast<JsiScopeHandle>(m_runtime.pushScope());
}

void JsiRuntime::PopScope(JsiScopeHandle scopeState) {
  m_runtime.popScope(reinterpret_cast<facebook::jsi::Runtime::ScopeState *>(scopeState));
}

bool JsiRuntime::SymbolStrictEquals(JsiPointerHandle left, JsiPointerHandle right) {
  return m_runtime.strictEquals(*AsSymbol(left), *AsSymbol(right));
}

bool JsiRuntime::StringStrictEquals(JsiPointerHandle left, JsiPointerHandle right) {
  return m_runtime.strictEquals(*AsString(left), *AsString(right));
}

bool JsiRuntime::ObjectStrictEquals(JsiPointerHandle left, JsiPointerHandle right) {
  return m_runtime.strictEquals(*AsObject(left), *AsObject(right));
}

bool JsiRuntime::InstanceOf(JsiPointerHandle obj, JsiPointerHandle constructor) {
  return m_runtime.instanceOf(*AsObject(obj), *AsFunction(constructor));
}

} // namespace winrt::Microsoft::ReactNative::implementation
