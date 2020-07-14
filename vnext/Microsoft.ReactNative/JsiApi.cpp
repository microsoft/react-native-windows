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

static facebook::jsi::PropNameID const *AsPropNameID(JsiPropertyNameIdData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::PropNameID const *>(data.Data);
}

static facebook::jsi::Symbol const *AsSymbol(JsiSymbolData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Symbol const *>(data.Data);
}

static facebook::jsi::String const *AsString(JsiStringData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::String const *>(data.Data);
}

static facebook::jsi::Object const *AsObject(JsiObjectData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Object const *>(data.Data);
}

static facebook::jsi::Function const *AsFunction(JsiFunctionData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Function const *>(data.Data);
}

static facebook::jsi::WeakObject const *AsWeakObject(JsiWeakObjectData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::WeakObject const *>(data.Data);
}

static facebook::jsi::Array const *AsArray(JsiArrayData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Array const *>(data.Data);
}

static facebook::jsi::ArrayBuffer const *AsArrayBuffer(JsiArrayBufferData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::ArrayBuffer const *>(data.Data);
}

static facebook::jsi::Value const *AsValue(JsiValueData const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Value const *>(data.Data);
}

static JsiValueData ToJsiValueData(facebook::jsi::Value const &value) noexcept {
  return {static_cast<JsiValueKind>(static_cast<int32_t>(value.kind_)),
          *reinterpret_cast<uint64_t const *>(&value.data_.number)};
}

static JsiPropertyNameIdData ToJsiPropertyNameIdData(facebook::jsi::PropNameID const &propNameId) noexcept {
  return {reinterpret_cast<uint64_t>(propNameId.ptr_)};
}

static facebook::jsi::Runtime::PointerValue *ToPointerValue(JsiSymbolData const &data) {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(data.Data);
}

static facebook::jsi::Runtime::PointerValue *ToPointerValue(JsiStringData const &data) {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(data.Data);
}

static facebook::jsi::Runtime::PointerValue *ToPointerValue(JsiObjectData const &data) {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(data.Data);
}

static facebook::jsi::Runtime::PointerValue *ToPointerValue(JsiPropertyNameIdData const &data) {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(data.Data);
}

static JsiSymbolData MakeJsiSymbolData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiStringData MakeJsiStringData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiObjectData MakeJsiObjectData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiPropertyNameIdData MakeJsiPropertyNameIdData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiSymbolData MakeJsiSymbolData(facebook::jsi::Symbol &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiStringData MakeJsiStringData(facebook::jsi::String &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiObjectData MakeJsiObjectData(facebook::jsi::Object &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiPropertyNameIdData MakeJsiPropertyNameIdData(facebook::jsi::PropNameID &&propertyNameId) {
  return {reinterpret_cast<uint64_t>(std::exchange(propertyNameId.ptr_, nullptr))};
}

static JsiArrayData MakeJsiArrayData(facebook::jsi::Array &&arr) {
  return {reinterpret_cast<uint64_t>(std::exchange(arr.ptr_, nullptr))};
}

static JsiWeakObjectData MakeJsiWeakObjectData(facebook::jsi::WeakObject &&weakObj) {
  return {reinterpret_cast<uint64_t>(std::exchange(weakObj.ptr_, nullptr))};
}

static JsiFunctionData MakeJsiFunctionData(facebook::jsi::Function &&func) {
  return {reinterpret_cast<uint64_t>(std::exchange(func.ptr_, nullptr))};
}

static JsiValueData MakeJsiValueData(facebook::jsi::Value &&value) {
  return {
      static_cast<JsiValueKind>(static_cast<int32_t>(std::exchange(value.kind_, facebook::jsi::Value::UndefinedKind))),
      *reinterpret_cast<uint64_t *>(&value.data_.number)};
}

static std::shared_ptr<facebook::jsi::HostObject> MakeHostObject(
    Microsoft::ReactNative::IJsiHostObject const &hostObject) {
  return std::make_shared<HostObjectWrapper>(hostObject);
}

static JsiValueData const *AsJsiValueData(facebook::jsi::Value const *value) noexcept {
  return reinterpret_cast<Microsoft::ReactNative::JsiValueData const *>(value);
}

static facebook::jsi::Value &&ToValue(JsiValueData &&value) noexcept {
  return reinterpret_cast<facebook::jsi::Value &&>(value);
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
    return ToValue(hostFunc(make<JsiRuntime>(runtime), *AsJsiValueData(&thisVal), {argsData, argsData + count}));
  };
}

//===========================================================================
// HostObjectWrapper implementation
//===========================================================================

HostObjectWrapper::HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept
    : m_hostObject{hostObject} {}

facebook::jsi::Value HostObjectWrapper::get(facebook::jsi::Runtime &runtime, facebook::jsi::PropNameID const &name) {
  // TODO: map between JsiRuntime and Runtime
  return ToValue(m_hostObject.GetProperty(make<JsiRuntime>(runtime), ToJsiPropertyNameIdData(name)));
}

void HostObjectWrapper::set(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name,
    facebook::jsi::Value const &value) {
  // TODO: map between JsiRuntime and Runtime
  m_hostObject.SetProperty(make<JsiRuntime>(runtime), ToJsiPropertyNameIdData(name), ToJsiValueData(value));
}

std::vector<facebook::jsi::PropNameID> HostObjectWrapper::getPropertyNames(facebook::jsi::Runtime &runtime) noexcept {
  std::vector<facebook::jsi::PropNameID> result;
  // TODO: map between JsiRuntime and Runtime
  auto names = m_hostObject.GetPropertyNames(make<JsiRuntime>(runtime));
  result.reserve(names.Size());
  constexpr uint32_t bufferSize = 200;
  JsiPropertyNameIdData nameBuffer[bufferSize];
  for (uint32_t startIndex = 0; startIndex < names.Size(); startIndex += bufferSize) {
    names.GetMany(startIndex, nameBuffer);
    size_t bufferLength = std::min(bufferSize, names.Size() - startIndex);
    for (size_t i = 0; i < bufferLength; ++i) {
      result.emplace_back(reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(nameBuffer[i].Data));
    }
  }

  return result;
}

//===========================================================================
// JsiRuntime implementation
//===========================================================================

JsiRuntime::JsiRuntime(facebook::jsi::Runtime &runtime) noexcept : m_runtime{runtime} {}

JsiValueData JsiRuntime::EvaluateJavaScript(IJsiBuffer const &buffer, hstring const &sourceUrl) {
  facebook::jsi::Value result;
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    result = m_runtime.evaluateJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl));
  });
  return MakeJsiValueData(std::move(result));
}

ReactNative::JsiPreparedJavaScript JsiRuntime::PrepareJavaScript(IJsiBuffer const &buffer, hstring const &sourceUrl) {
  ReactNative::JsiPreparedJavaScript result{nullptr};
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    result = make<JsiPreparedJavaScript>(
        m_runtime.prepareJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl)));
  });
  return result;
}

JsiValueData JsiRuntime::EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js) {
  return MakeJsiValueData(m_runtime.evaluatePreparedJavaScript(get_self<JsiPreparedJavaScript>(js)->m_js));
}

JsiObjectData JsiRuntime::Global() {
  return MakeJsiObjectData(m_runtime.global());
}

hstring JsiRuntime::Description() {
  return to_hstring(m_runtime.description());
}

bool JsiRuntime::IsInspectable() {
  return m_runtime.isInspectable();
}

JsiSymbolData JsiRuntime::CloneSymbol(JsiSymbolData symbol) {
  return MakeJsiSymbolData(m_runtime.cloneSymbol(ToPointerValue(symbol)));
}

JsiStringData JsiRuntime::CloneString(JsiStringData str) {
  return MakeJsiStringData(m_runtime.cloneString(ToPointerValue(str)));
}

JsiObjectData JsiRuntime::CloneObject(JsiObjectData obj) {
  return MakeJsiObjectData(m_runtime.cloneObject(ToPointerValue(obj)));
}

JsiPropertyNameIdData JsiRuntime::ClonePropertyNameId(JsiPropertyNameIdData propertyNameId) {
  return MakeJsiPropertyNameIdData(m_runtime.clonePropNameID(ToPointerValue(propertyNameId)));
}

JsiPropertyNameIdData JsiRuntime::CreatePropertyNameIdFromAscii(array_view<uint8_t const> ascii) {
  return MakeJsiPropertyNameIdData(
      m_runtime.createPropNameIDFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
}

JsiPropertyNameIdData JsiRuntime::CreatePropertyNameIdFromUtf8(array_view<uint8_t const> utf8) {
  return MakeJsiPropertyNameIdData(m_runtime.createPropNameIDFromUtf8(utf8.data(), utf8.size()));
}

JsiPropertyNameIdData JsiRuntime::CreatePropertyNameIdFromString(JsiStringData str) {
  auto ptr = reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(str.Data);
  return MakeJsiPropertyNameIdData(
      m_runtime.createPropNameIDFromString(*reinterpret_cast<facebook::jsi::String *>(ptr)));
}

void JsiRuntime::PropertyNameIdToUtf8(JsiPropertyNameIdData propertyNameId, JsiDataHandler const &utf8Handler) {
  auto ptr = reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(propertyNameId.Data);
  std::string utf8 = m_runtime.utf8(*reinterpret_cast<facebook::jsi::String *>(ptr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

bool JsiRuntime::PropertyNameIdEquals(JsiPropertyNameIdData left, JsiPropertyNameIdData right) {
  return m_runtime.compare(*AsPropNameID(left), *AsPropNameID(right));
}

void JsiRuntime::SymbolToUtf8(JsiSymbolData symbol, JsiDataHandler const &utf8Handler) {
  std::string utf8 = m_runtime.symbolToString(*AsSymbol(symbol));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

JsiStringData JsiRuntime::CreateStringFromAscii(array_view<uint8_t const> ascii) {
  return MakeJsiStringData(m_runtime.createStringFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
}

JsiStringData JsiRuntime::CreateStringFromUtf8(array_view<uint8_t const> utf8) {
  return MakeJsiStringData(m_runtime.createStringFromUtf8(utf8.data(), utf8.size()));
}

void JsiRuntime::StringToUtf8(JsiStringData str, JsiDataHandler const &utf8Handler) {
  std::string utf8 = m_runtime.utf8(*AsString(str));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  utf8Handler({data, data + utf8.size()});
}

JsiValueData JsiRuntime::CreateValueFromJsonUtf8(array_view<uint8_t const> json) {
  return MakeJsiValueData(m_runtime.createValueFromJsonUtf8(json.data(), json.size()));
}

JsiObjectData JsiRuntime::CreateObject() {
  return MakeJsiObjectData(m_runtime.createObject());
}

JsiObjectData JsiRuntime::CreateObjectWithHostObject(IJsiHostObject const &hostObject) {
  return MakeJsiObjectData(m_runtime.createObject(MakeHostObject(hostObject)));
}

IJsiHostObject JsiRuntime::GetHostObject(JsiObjectData obj) {
  auto hostObject = m_runtime.getHostObject(*AsObject(obj));
  // TODO: using static_pointer_cast is unsafe here. How to use shared_ptr without RTTI?
  auto wrapper = std::static_pointer_cast<HostObjectWrapper>(hostObject);
  return wrapper ? wrapper->m_hostObject : nullptr;
}

JsiHostFunction JsiRuntime::GetHostFunction(JsiFunctionData /*func*/) {
  // auto hostFunction = m_runtime.getHostFunction(AsFunction(func));
  // TODO: implement mapping
  return nullptr;
}

JsiValueData JsiRuntime::GetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId) {
  return MakeJsiValueData(m_runtime.getProperty(*AsObject(obj), *AsPropNameID(propertyNameId)));
}

JsiValueData JsiRuntime::GetPropertyByName(JsiObjectData obj, JsiStringData name) {
  return MakeJsiValueData(m_runtime.getProperty(*AsObject(obj), *AsString(name)));
}

bool JsiRuntime::HasPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId) {
  return m_runtime.hasProperty(*AsObject(obj), *AsPropNameID(propertyNameId));
}

bool JsiRuntime::HasPropertyByName(JsiObjectData obj, JsiStringData name) {
  return m_runtime.hasProperty(*AsObject(obj), *AsString(name));
}

void JsiRuntime::SetPropertyById(JsiObjectData obj, JsiPropertyNameIdData propertyNameId, JsiValueData const &value) {
  m_runtime.setPropertyValue(
      const_cast<facebook::jsi::Object &>(*AsObject(obj)), *AsPropNameID(propertyNameId), *AsValue(value));
}

void JsiRuntime::SetPropertyByName(JsiObjectData obj, JsiStringData name, JsiValueData const &value) {
  m_runtime.setPropertyValue(const_cast<facebook::jsi::Object &>(*AsObject(obj)), *AsString(name), *AsValue(value));
}

bool JsiRuntime::IsArray(JsiObjectData obj) {
  return m_runtime.isArray(*AsObject(obj));
}

bool JsiRuntime::IsArrayBuffer(JsiObjectData obj) {
  return m_runtime.isArrayBuffer(*AsObject(obj));
}

bool JsiRuntime::IsFunction(JsiObjectData obj) {
  return m_runtime.isFunction(*AsObject(obj));
}

bool JsiRuntime::IsHostObject(JsiObjectData obj) {
  return m_runtime.isHostObject(*AsObject(obj));
}

bool JsiRuntime::IsHostFunction(JsiFunctionData obj) {
  return m_runtime.isHostFunction(*AsFunction(obj));
}

JsiArrayData JsiRuntime::GetPropertyNames(JsiObjectData obj) {
  return MakeJsiArrayData(m_runtime.getPropertyNames(*AsObject(obj)));
}

JsiWeakObjectData JsiRuntime::CreateWeakObject(JsiObjectData obj) {
  return MakeJsiWeakObjectData(m_runtime.createWeakObject(*AsObject(obj)));
}

JsiValueData JsiRuntime::LockWeakObject(JsiWeakObjectData weakObject) {
  return MakeJsiValueData(m_runtime.lockWeakObject(*AsWeakObject(weakObject)));
}

JsiArrayData JsiRuntime::CreateArray(uint32_t size) {
  return MakeJsiArrayData(m_runtime.createArray(size));
}

uint32_t JsiRuntime::GetArraySize(JsiArrayData arr) {
  return static_cast<uint32_t>(m_runtime.size(*AsArray(arr)));
}

uint32_t JsiRuntime::GetArrayBufferSize(JsiArrayBufferData arrayBuffer) {
  return static_cast<uint32_t>(m_runtime.size(*AsArrayBuffer(arrayBuffer)));
}

void JsiRuntime::GetArrayBufferData(JsiArrayBufferData arrayBuffer, JsiDataHandler const &utf8Handler) {
  auto data = m_runtime.data(*AsArrayBuffer(arrayBuffer));
  return utf8Handler({data, data + m_runtime.size(*AsArrayBuffer(arrayBuffer))});
}

JsiValueData JsiRuntime::GetValueAtIndex(JsiArrayData arr, uint32_t index) {
  return MakeJsiValueData(AsArray(arr)->getValueAtIndex(m_runtime, index));
}

void JsiRuntime::SetValueAtIndex(JsiArrayData arr, uint32_t index, JsiValueData const &value) {
  m_runtime.setValueAtIndexImpl(const_cast<facebook::jsi::Array &>(*AsArray(arr)), index, *AsValue(value));
}

JsiFunctionData JsiRuntime::CreateFunctionFromHostFunction(
    JsiPropertyNameIdData propNameId,
    uint32_t paramCount,
    JsiHostFunction const &hostFunc) {
  return MakeJsiFunctionData(
      m_runtime.createFunctionFromHostFunction(*AsPropNameID(propNameId), paramCount, MakeHostFunction(hostFunc)));
}

JsiValueData JsiRuntime::Call(JsiFunctionData func, JsiValueData const &thisArg, array_view<JsiValueData const> args) {
  return MakeJsiValueData(m_runtime.call(
      *AsFunction(func), *AsValue(thisArg), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
}

JsiValueData JsiRuntime::CallAsConstructor(JsiFunctionData func, array_view<JsiValueData const> args) {
  return MakeJsiValueData(m_runtime.callAsConstructor(
      *AsFunction(func), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
}

JsiScopeState JsiRuntime::PushScope() {
  return {reinterpret_cast<uint64_t>(m_runtime.pushScope())};
}

void JsiRuntime::PopScope(JsiScopeState scopeState) {
  m_runtime.popScope(reinterpret_cast<facebook::jsi::Runtime::ScopeState *>(scopeState.Data));
}

bool JsiRuntime::SymbolStrictEquals(JsiSymbolData left, JsiSymbolData right) {
  return m_runtime.strictEquals(*AsSymbol(left), *AsSymbol(right));
}

bool JsiRuntime::StringStrictEquals(JsiStringData left, JsiStringData right) {
  return m_runtime.strictEquals(*AsString(left), *AsString(right));
}

bool JsiRuntime::ObjectStrictEquals(JsiObjectData left, JsiObjectData right) {
  return m_runtime.strictEquals(*AsObject(left), *AsObject(right));
}

bool JsiRuntime::InstanceOf(JsiObjectData obj, JsiFunctionData constructor) {
  return m_runtime.instanceOf(*AsObject(obj), *AsFunction(constructor));
}

void JsiRuntime::ReleaseSymbol(JsiSymbolData const &symbolData) {
  facebook::jsi::Symbol symbol{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(symbolData.Data)};
}

void JsiRuntime::ReleaseString(JsiStringData const &stringData) {
  facebook::jsi::String str{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(stringData.Data)};
}

void JsiRuntime::ReleaseObject(JsiObjectData const &objectData) {
  facebook::jsi::Object obj{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(objectData.Data)};
}

void JsiRuntime::ReleasePropertyNameId(JsiPropertyNameIdData const &propertyNameIdData) {
  facebook::jsi::PropNameID prop{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(propertyNameIdData.Data)};
}

} // namespace winrt::Microsoft::ReactNative::implementation
