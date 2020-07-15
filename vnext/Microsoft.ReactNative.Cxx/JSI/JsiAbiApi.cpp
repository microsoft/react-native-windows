// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiAbiApi.h"

using namespace facebook::jsi;

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// JsiBufferWrapper implementation
//===========================================================================

JsiByteBufferWrapper::JsiByteBufferWrapper(std::shared_ptr<Buffer const> const &buffer) noexcept : m_buffer{buffer} {}

JsiByteBufferWrapper::~JsiByteBufferWrapper() = default;

uint32_t JsiByteBufferWrapper::Size() {
  return static_cast<uint32_t>(m_buffer->size());
}

void JsiByteBufferWrapper::GetData(JsiByteArrayUser const &useBytes) {
  useBytes(winrt::array_view<uint8_t const>{m_buffer->data(), m_buffer->data() + m_buffer->size()});
}

//===========================================================================
// JsiPreparedJavaScriptWrapper implementation
//===========================================================================

JsiPreparedJavaScriptWrapper::JsiPreparedJavaScriptWrapper(JsiPreparedJavaScript const &preparedScript) noexcept
    : m_preparedScript{preparedScript} {}

JsiPreparedJavaScriptWrapper::~JsiPreparedJavaScriptWrapper() = default;

JsiPreparedJavaScript const &JsiPreparedJavaScriptWrapper::Get() const noexcept {
  return m_preparedScript;
}

//===========================================================================
// JsiHostObjectWrapper implementation
//===========================================================================

/*static*/ std::mutex JsiHostObjectWrapper::s_mutex;
/*static*/ std::unordered_map<uint64_t, JsiHostObjectWrapper *> JsiHostObjectWrapper::s_objectDataToObjectWrapper;

JsiHostObjectWrapper::JsiHostObjectWrapper(std::shared_ptr<HostObject> &&hostObject) noexcept
    : m_hostObject(std::move(hostObject)) {}

JsiHostObjectWrapper::~JsiHostObjectWrapper() noexcept {
  if (m_objectData.Data) {
    std::scoped_lock lock{s_mutex};
    s_objectDataToObjectWrapper.erase(m_objectData.Data);
  }
}

JsiValueData JsiHostObjectWrapper::GetProperty(IJsiRuntime const &runtime, JsiPropertyNameIdData const &name) {
  JsiAbiRuntime rt{runtime};
  return JsiAbiRuntime::MakeJsiValueData(m_hostObject->get(rt, *JsiAbiRuntime::AsPropNameID(&name)));
}

void JsiHostObjectWrapper::SetProperty(
    IJsiRuntime const &runtime,
    JsiPropertyNameIdData const &name,
    JsiValueData const &value) {
  JsiAbiRuntime rt{runtime};
  m_hostObject->set(rt, *JsiAbiRuntime::AsPropNameID(&name), *reinterpret_cast<Value const *>(&value));
}

Windows::Foundation::Collections::IVector<JsiPropertyNameIdData> JsiHostObjectWrapper::GetPropertyNames(
    IJsiRuntime const &runtime) {
  JsiAbiRuntime rt{runtime};
  auto names = m_hostObject->getPropertyNames(rt);
  std::vector<JsiPropertyNameIdData> result;
  result.reserve(names.size());
  for (auto &name : names) {
    result.push_back(JsiAbiRuntime::MakeJsiPropertyNameIdData(std::move(name)));
  }

  return winrt::single_threaded_vector<JsiPropertyNameIdData>(std::move(result));
}

/*static*/ void JsiHostObjectWrapper::RegisterHostObject(
    JsiObjectData const &objectData,
    JsiHostObjectWrapper *hostObject) noexcept {
  std::scoped_lock lock{s_mutex};
  s_objectDataToObjectWrapper[objectData.Data] = hostObject;
  hostObject->m_objectData = objectData;
}

/*static*/ bool JsiHostObjectWrapper::IsHostObject(JsiObjectData const &objectData) noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = s_objectDataToObjectWrapper.find(objectData.Data);
  return it != s_objectDataToObjectWrapper.end();
}

/*static*/ std::shared_ptr<HostObject> JsiHostObjectWrapper::GetHostObject(JsiObjectData const &objectData) noexcept {
  auto it = s_objectDataToObjectWrapper.find(objectData.Data);
  if (it != s_objectDataToObjectWrapper.end()) {
    return it->second->m_hostObject;
  } else {
    return nullptr;
  }
}

//===========================================================================
// JsiHostFunctionWrapper implementation
//===========================================================================

/*static*/ std::mutex JsiHostFunctionWrapper::s_functionMutex;
/*static*/ std::atomic<uint32_t> JsiHostFunctionWrapper::s_functionIdGenerator;
/*static*/ std::unordered_map<uint32_t, JsiHostFunctionWrapper *> JsiHostFunctionWrapper::s_functionIdToFunctionWrapper;
/*static*/ std::unordered_map<uint64_t, JsiHostFunctionWrapper *>
    JsiHostFunctionWrapper::s_functionDataToFunctionWrapper;

JsiHostFunctionWrapper::JsiHostFunctionWrapper(HostFunctionType &&hostFunction, uint32_t functionId) noexcept
    : m_hostFunction{std::move(hostFunction)}, m_functionId{functionId} {
  VerifyElseCrashSz(functionId, "Function Id must be not zero");
  std::scoped_lock lock{s_functionMutex};
  s_functionIdToFunctionWrapper[functionId] = this;
}

JsiHostFunctionWrapper::JsiHostFunctionWrapper(JsiHostFunctionWrapper &&other) noexcept
    : m_hostFunction{std::move(other.m_hostFunction)},
      m_functionId{std::exchange(other.m_functionId, 0)},
      m_functionData{std::exchange(other.m_functionData, {0})} {
  std::scoped_lock lock{s_functionMutex};
  if (m_functionId) {
    s_functionIdToFunctionWrapper[m_functionId] = this;
  }
  if (m_functionData.Data) {
    s_functionDataToFunctionWrapper[m_functionData.Data] = this;
  }
}

JsiHostFunctionWrapper::~JsiHostFunctionWrapper() noexcept {
  if (m_functionId || m_functionData.Data) {
    std::scoped_lock lock{s_functionMutex};
    auto it1 = s_functionIdToFunctionWrapper.find(m_functionId);
    if (it1 != s_functionIdToFunctionWrapper.end()) {
      s_functionIdToFunctionWrapper.erase(it1);
    }
    auto it2 = s_functionDataToFunctionWrapper.find(m_functionData.Data);
    if (it2 != s_functionDataToFunctionWrapper.end()) {
      s_functionDataToFunctionWrapper.erase(it2);
    }
  }
}

JsiValueData JsiHostFunctionWrapper::
operator()(IJsiRuntime const &runtime, JsiValueData const &thisValue, array_view<JsiValueData const> args) {
  auto rt{JsiAbiRuntime{runtime}};
  return JsiAbiRuntime::MakeJsiValueData(
      m_hostFunction(rt, *JsiAbiRuntime::AsValue(&thisValue), JsiAbiRuntime::AsValue(args.data()), args.size()));
}

/*static*/ uint32_t JsiHostFunctionWrapper::GetNextFunctionId() noexcept {
  return ++s_functionIdGenerator;
}

/*static*/ void JsiHostFunctionWrapper::RegisterHostFunction(
    uint32_t functionId,
    JsiFunctionData const &functionData) noexcept {
  std::scoped_lock lock{s_functionMutex};
  auto it = s_functionIdToFunctionWrapper.find(functionId);
  VerifyElseCrashSz(it != s_functionIdToFunctionWrapper.end(), "Function Id is not found.");
  JsiHostFunctionWrapper *functionWrapper = it->second;
  s_functionDataToFunctionWrapper[functionData.Data] = functionWrapper;
  functionWrapper->m_functionData = functionData;
}

/*static*/ bool JsiHostFunctionWrapper::IsHostFunction(JsiFunctionData const &functionData) noexcept {
  std::scoped_lock lock{s_functionMutex};
  return s_functionDataToFunctionWrapper.find(functionData.Data) != s_functionDataToFunctionWrapper.end();
}

/*static*/ HostFunctionType &JsiHostFunctionWrapper::GetHostFunction(JsiFunctionData const &functionData) noexcept {
  std::scoped_lock lock{s_functionMutex};
  auto it = s_functionDataToFunctionWrapper.find(functionData.Data);
  if (it != s_functionDataToFunctionWrapper.end()) {
    return it->second->m_hostFunction;
  }

  VerifyElseCrashSz(false, "Function is not a host function");
}

//===========================================================================
// JsiAbiRuntime implementation
//===========================================================================

JsiAbiRuntime::JsiAbiRuntime(IJsiRuntime const &runtime) noexcept : m_runtime{runtime} {}

JsiAbiRuntime::~JsiAbiRuntime() = default;

Value JsiAbiRuntime::evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer, const std::string &sourceURL) {
  return MakeValue(m_runtime.EvaluateJavaScript(winrt::make<JsiByteBufferWrapper>(buffer), to_hstring(sourceURL)));
}

std::shared_ptr<const PreparedJavaScript> JsiAbiRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceURL) {
  return std::make_shared<JsiPreparedJavaScriptWrapper>(
      m_runtime.PrepareJavaScript(winrt::make<JsiByteBufferWrapper>(std::move(buffer)), to_hstring(sourceURL)));
}

Value JsiAbiRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const PreparedJavaScript> &js) {
  return MakeValue(
      m_runtime.EvaluatePreparedJavaScript(std::static_pointer_cast<JsiPreparedJavaScriptWrapper const>(js)->Get()));
}

Object JsiAbiRuntime::global() {
  return MakeObject(m_runtime.Global());
}

std::string JsiAbiRuntime::description() {
  return to_string(m_runtime.Description());
}

bool JsiAbiRuntime::isInspectable() {
  return m_runtime.IsInspectable();
}

Instrumentation &JsiAbiRuntime::instrumentation() {
  // TODO: implement
  VerifyElseCrash(false);
}

Runtime::PointerValue *JsiAbiRuntime::cloneSymbol(const Runtime::PointerValue *pv) {
  return new SymbolPointerValue{make_weak(m_runtime), m_runtime.CloneSymbol(AsJsiSymbolData(pv))};
}

Runtime::PointerValue *JsiAbiRuntime::cloneString(const Runtime::PointerValue *pv) {
  return new StringPointerValue{make_weak(m_runtime), m_runtime.CloneString(AsJsiStringData(pv))};
}

Runtime::PointerValue *JsiAbiRuntime::cloneObject(const Runtime::PointerValue *pv) {
  return new ObjectPointerValue{make_weak(m_runtime), m_runtime.CloneObject(AsJsiObjectData(pv))};
}

Runtime::PointerValue *JsiAbiRuntime::clonePropNameID(const Runtime::PointerValue *pv) {
  return new PropNameIDPointerValue{make_weak(m_runtime), m_runtime.ClonePropertyNameId(AsJsiPropertyNameIdData(pv))};
}

PropNameID JsiAbiRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  auto data = reinterpret_cast<uint8_t const *>(str);
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromAscii({data, data + length}));
}

PropNameID JsiAbiRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromUtf8({utf8, utf8 + length}));
}

PropNameID JsiAbiRuntime::createPropNameIDFromString(const String &str) {
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromString(AsJsiStringData(str)));
}

std::string JsiAbiRuntime::utf8(const PropNameID &propertyNameId) {
  std::string result;
  m_runtime.PropertyNameIdToUtf8(AsJsiPropertyNameIdData(propertyNameId), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

bool JsiAbiRuntime::compare(const PropNameID &left, const PropNameID &right) {
  return m_runtime.PropertyNameIdEquals(AsJsiPropertyNameIdData(left), AsJsiPropertyNameIdData(right));
}

std::string JsiAbiRuntime::symbolToString(const Symbol &symbol) {
  std::string result;
  m_runtime.SymbolToUtf8(AsJsiSymbolData(symbol), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

String JsiAbiRuntime::createStringFromAscii(const char *str, size_t length) {
  auto ascii = reinterpret_cast<uint8_t const *>(str);
  return MakeString(m_runtime.CreateStringFromAscii({ascii, ascii + length}));
}

String JsiAbiRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length) {
  return MakeString(m_runtime.CreateStringFromAscii({utf8, utf8 + length}));
}

std::string JsiAbiRuntime::utf8(const String &str) {
  std::string result;
  m_runtime.StringToUtf8(AsJsiStringData(str), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

Value JsiAbiRuntime::createValueFromJsonUtf8(const uint8_t *json, size_t length) {
  return MakeValue(m_runtime.CreateValueFromJsonUtf8({json, json + length}));
}

Object JsiAbiRuntime::createObject() {
  return MakeObject(m_runtime.CreateObject());
}

Object JsiAbiRuntime::createObject(std::shared_ptr<HostObject> ho) {
  auto hostObjectWrapper = winrt::make<JsiHostObjectWrapper>(std::move(ho));
  Object result = MakeObject(m_runtime.CreateObjectWithHostObject(hostObjectWrapper));
  JsiHostObjectWrapper::RegisterHostObject(AsJsiObjectData(result), get_self<JsiHostObjectWrapper>(hostObjectWrapper));
  return result;
}

std::shared_ptr<HostObject> JsiAbiRuntime::getHostObject(const Object &obj) {
  return JsiHostObjectWrapper::GetHostObject(AsJsiObjectData(obj));
}

HostFunctionType &JsiAbiRuntime::getHostFunction(const Function &func) {
  return JsiHostFunctionWrapper::GetHostFunction(AsJsiFunctionData(func));
}

Value JsiAbiRuntime::getProperty(const Object &obj, const PropNameID &name) {
  return MakeValue(m_runtime.GetPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name)));
}

Value JsiAbiRuntime::getProperty(const Object &obj, const String &name) {
  return MakeValue(m_runtime.GetPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name)));
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const PropNameID &name) {
  return m_runtime.HasPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name));
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const String &name) {
  return m_runtime.HasPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name));
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const PropNameID &name, const Value &value) {
  m_runtime.SetPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name), AsJsiValueData(value));
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const String &name, const Value &value) {
  m_runtime.SetPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name), AsJsiValueData(value));
}

bool JsiAbiRuntime::isArray(const Object &obj) const {
  return m_runtime.IsArray(AsJsiObjectData(obj));
}

bool JsiAbiRuntime::isArrayBuffer(const Object &obj) const {
  return m_runtime.IsArrayBuffer(AsJsiObjectData(obj));
}

bool JsiAbiRuntime::isFunction(const Object &obj) const {
  return m_runtime.IsFunction(AsJsiObjectData(obj));
}

bool JsiAbiRuntime::isHostObject(const Object &obj) const {
  return m_runtime.IsHostObject(AsJsiObjectData(obj));
}

bool JsiAbiRuntime::isHostFunction(const Function &func) const {
  return JsiHostFunctionWrapper::IsHostFunction(AsJsiFunctionData(func));
}

Array JsiAbiRuntime::getPropertyNames(const Object &obj) {
  return MakeArray(m_runtime.GetPropertyNames(AsJsiObjectData(obj)));
}

WeakObject JsiAbiRuntime::createWeakObject(const Object &obj) {
  return MakeWeakObject(m_runtime.CreateWeakObject(AsJsiObjectData(obj)));
}

Value JsiAbiRuntime::lockWeakObject(const WeakObject &weakObj) {
  return MakeValue(m_runtime.LockWeakObject(AsJsiWeakObjectData(weakObj)));
}

Array JsiAbiRuntime::createArray(size_t length) {
  return MakeArray(m_runtime.CreateArray(static_cast<uint32_t>(length)));
}

size_t JsiAbiRuntime::size(const Array &arr) {
  return m_runtime.GetArraySize(AsJsiArrayData(arr));
}

size_t JsiAbiRuntime::size(const ArrayBuffer &arrayBuffer) {
  return m_runtime.GetArrayBufferSize(AsJsiArrayBufferData(arrayBuffer));
}

uint8_t *JsiAbiRuntime::data(const ArrayBuffer &arrayBuffer) {
  uint8_t *result{};
  m_runtime.GetArrayBufferData(AsJsiArrayBufferData(arrayBuffer), [&result](array_view<uint8_t const> dataView) {
    result = const_cast<uint8_t *>(dataView.data());
  });
  return result;
}

Value JsiAbiRuntime::getValueAtIndex(const Array &arr, size_t i) {
  return MakeValue(m_runtime.GetValueAtIndex(AsJsiArrayData(arr), static_cast<uint32_t>(i)));
}

void JsiAbiRuntime::setValueAtIndexImpl(Array &arr, size_t i, const Value &value) {
  m_runtime.SetValueAtIndex(AsJsiArrayData(arr), static_cast<uint32_t>(i), AsJsiValueData(value));
}

Function
JsiAbiRuntime::createFunctionFromHostFunction(const PropNameID &name, unsigned int paramCount, HostFunctionType func) {
  uint32_t functionId = JsiHostFunctionWrapper::GetNextFunctionId();
  Function result = MakeFunction(m_runtime.CreateFunctionFromHostFunction(
      AsJsiPropertyNameIdData(name), paramCount, JsiHostFunctionWrapper(std::move(func), functionId)));
  JsiHostFunctionWrapper::RegisterHostFunction(functionId, AsJsiFunctionData(result));
  return result;
}

Value JsiAbiRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count) {
  JsiValueData const *argsData = reinterpret_cast<JsiValueData const *>(args);
  return MakeValue(m_runtime.Call(AsJsiFunctionData(func), AsJsiValueData(jsThis), {argsData, argsData + count}));
}

Value JsiAbiRuntime::callAsConstructor(const Function &func, const Value *args, size_t count) {
  JsiValueData const *argsData = reinterpret_cast<JsiValueData const *>(args);
  return MakeValue(m_runtime.CallAsConstructor(AsJsiFunctionData(func), {argsData, argsData + count}));
}

Runtime::ScopeState *JsiAbiRuntime::pushScope() {
  return reinterpret_cast<ScopeState *>(m_runtime.PushScope().Data);
}

void JsiAbiRuntime::popScope(ScopeState *scope) {
  m_runtime.PopScope({reinterpret_cast<uint64_t>(scope)});
}

bool JsiAbiRuntime::strictEquals(const Symbol &a, const Symbol &b) const {
  return m_runtime.SymbolStrictEquals(AsJsiSymbolData(a), AsJsiSymbolData(b));
}

bool JsiAbiRuntime::strictEquals(const String &a, const String &b) const {
  return m_runtime.StringStrictEquals(AsJsiStringData(a), AsJsiStringData(b));
}

bool JsiAbiRuntime::strictEquals(const Object &a, const Object &b) const {
  return m_runtime.ObjectStrictEquals(AsJsiObjectData(a), AsJsiObjectData(b));
}

bool JsiAbiRuntime::instanceOf(const Object &o, const Function &f) {
  return m_runtime.InstanceOf(AsJsiObjectData(o), AsJsiFunctionData(f));
}

/*static*/ JsiSymbolData JsiAbiRuntime::AsJsiSymbolData(PointerValue const *pv) noexcept {
  return {reinterpret_cast<uint64_t>(pv)};
}

/*static*/ JsiStringData JsiAbiRuntime::AsJsiStringData(PointerValue const *pv) noexcept {
  return {reinterpret_cast<uint64_t>(pv)};
}

/*static*/ JsiObjectData JsiAbiRuntime::AsJsiObjectData(PointerValue const *pv) noexcept {
  return {reinterpret_cast<uint64_t>(pv)};
}

/*static*/ JsiPropertyNameIdData JsiAbiRuntime::AsJsiPropertyNameIdData(PointerValue const *pv) noexcept {
  return {reinterpret_cast<uint64_t>(pv)};
}

/*static*/ JsiSymbolData JsiAbiRuntime::AsJsiSymbolData(Symbol const &str) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(str))};
}

/*static*/ JsiStringData JsiAbiRuntime::AsJsiStringData(String const &str) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(str))};
}

/*static*/ JsiObjectData JsiAbiRuntime::AsJsiObjectData(Object const &obj) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(obj))};
}

/*static*/ JsiPropertyNameIdData JsiAbiRuntime::AsJsiPropertyNameIdData(PropNameID const &propertyId) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(propertyId))};
}

/*static*/ JsiFunctionData JsiAbiRuntime::AsJsiFunctionData(Function const &func) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(func))};
}

/*static*/ JsiWeakObjectData JsiAbiRuntime::AsJsiWeakObjectData(WeakObject const &weakObject) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(weakObject))};
}

/*static*/ JsiArrayData JsiAbiRuntime::AsJsiArrayData(facebook::jsi::Array const &arr) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(arr))};
}

/*static*/ JsiArrayBufferData JsiAbiRuntime::AsJsiArrayBufferData(
    facebook::jsi::ArrayBuffer const &arrayBuffer) noexcept {
  return {reinterpret_cast<uint64_t>(getPointerValue(arrayBuffer))};
}

/*static*/ JsiValueData const &JsiAbiRuntime::AsJsiValueData(Value const &value) noexcept {
  return reinterpret_cast<JsiValueData const &>(value);
}

/*static*/ JsiPropertyNameIdData JsiAbiRuntime::MakeJsiPropertyNameIdData(PropNameID &&propertyId) noexcept {
  auto ptr = reinterpret_cast<Runtime::PointerValue **>(&propertyId);
  return {reinterpret_cast<uint64_t>(std::exchange(*ptr, nullptr))};
}

/*static*/ JsiValueData JsiAbiRuntime::MakeJsiValueData(facebook::jsi::Value &&value) noexcept {
  auto ptr = reinterpret_cast<JsiValueData **>(&value);
  return {std::exchange((*ptr)->Kind, JsiValueKind::Undefined), (*ptr)->Data};
}

/*static*/ PropNameID const *JsiAbiRuntime::AsPropNameID(JsiPropertyNameIdData const *data) noexcept {
  return reinterpret_cast<PropNameID const *>(data);
}

/*static*/ Value const *JsiAbiRuntime::AsValue(JsiValueData const *data) noexcept {
  return reinterpret_cast<Value const *>(data);
}

Runtime::PointerValue *JsiAbiRuntime::MakeSymbolValue(JsiSymbolData &&symbol) const noexcept {
  return new SymbolPointerValue{make_weak(m_runtime), std::move(symbol)};
}

Runtime::PointerValue *JsiAbiRuntime::MakeStringValue(JsiStringData &&str) const noexcept {
  return new StringPointerValue{make_weak(m_runtime), std::move(str)};
}

Runtime::PointerValue *JsiAbiRuntime::MakeObjectValue(JsiObjectData &&obj) const noexcept {
  return new ObjectPointerValue{make_weak(m_runtime), std::move(obj)};
}

Runtime::PointerValue *JsiAbiRuntime::MakePropNameIDValue(JsiPropertyNameIdData &&propertyId) const noexcept {
  return new PropNameIDPointerValue{make_weak(m_runtime), std::move(propertyId)};
}

Symbol JsiAbiRuntime::MakeSymbol(JsiSymbolData &&symbol) const noexcept {
  return make<Symbol>(MakeSymbolValue(std::move(symbol)));
}

String JsiAbiRuntime::MakeString(JsiStringData &&str) const noexcept {
  return make<String>(MakeStringValue(std::move(str)));
}

Object JsiAbiRuntime::MakeObject(JsiObjectData &&obj) const noexcept {
  return make<Object>(MakeObjectValue(std::move(obj)));
}

PropNameID JsiAbiRuntime::MakePropNameID(JsiPropertyNameIdData &&propertyId) const noexcept {
  return make<PropNameID>(MakePropNameIDValue(std::move(propertyId)));
}

Array JsiAbiRuntime::MakeArray(JsiArrayData &&arr) noexcept {
  return MakeObject({arr.Data}).getArray(*this);
}

WeakObject JsiAbiRuntime::MakeWeakObject(JsiWeakObjectData &&weakObject) const noexcept {
  return make<WeakObject>(MakeObjectValue({weakObject.Data}));
}

Function JsiAbiRuntime::MakeFunction(JsiFunctionData &&func) noexcept {
  return MakeObject({func.Data}).getFunction(*this);
}

Value JsiAbiRuntime::MakeValue(JsiValueData &&value) const noexcept {
  switch (value.Kind) {
    case JsiValueKind::Undefined:
      return Value();
    case JsiValueKind::Null:
      return Value(nullptr);
    case JsiValueKind::Boolean:
      return Value(value.Data != 0);
    case JsiValueKind::Number:
      return Value(*reinterpret_cast<double *>(&value.Data));
    case JsiValueKind::Symbol:
      return Value(MakeSymbol(JsiSymbolData{value.Data}));
    case JsiValueKind::String:
      return Value(MakeString(JsiStringData{value.Data}));
    case JsiValueKind::Object:
      return Value(MakeObject(JsiObjectData{value.Data}));
    default:
      VerifyElseCrashSz(false, "Unexpected JsiValueKind value");
  }
}

JsiAbiRuntime::SymbolPointerValue::SymbolPointerValue(
    winrt::weak_ref<IJsiRuntime> &&weakRuntime,
    JsiSymbolData &&symbol) noexcept
    : m_weakRuntime{std::move(weakRuntime)}, m_symbol{std::move(symbol)} {}

void JsiAbiRuntime::SymbolPointerValue::invalidate() {
  if (auto runtime = m_weakRuntime.get()) {
    m_weakRuntime = nullptr;
    runtime.ReleaseSymbol(m_symbol);
  }
}

JsiAbiRuntime::StringPointerValue::StringPointerValue(
    winrt::weak_ref<IJsiRuntime> &&weakRuntime,
    JsiStringData &&str) noexcept
    : m_weakRuntime{std::move(weakRuntime)}, m_string{std::move(str)} {}

void JsiAbiRuntime::StringPointerValue::invalidate() {
  if (auto runtime = m_weakRuntime.get()) {
    m_weakRuntime = nullptr;
    runtime.ReleaseString(m_string);
  }
}

JsiAbiRuntime::ObjectPointerValue::ObjectPointerValue(
    winrt::weak_ref<IJsiRuntime> &&weakRuntime,
    JsiObjectData &&obj) noexcept
    : m_weakRuntime{std::move(weakRuntime)}, m_object{std::move(obj)} {}

void JsiAbiRuntime::ObjectPointerValue::invalidate() {
  if (auto runtime = m_weakRuntime.get()) {
    m_weakRuntime = nullptr;
    runtime.ReleaseObject(m_object);
  }
}

JsiAbiRuntime::PropNameIDPointerValue::PropNameIDPointerValue(
    winrt::weak_ref<IJsiRuntime> &&weakRuntime,
    JsiPropertyNameIdData &&propertyId) noexcept
    : m_weakRuntime{std::move(weakRuntime)}, m_propertyId{std::move(propertyId)} {}

void JsiAbiRuntime::PropNameIDPointerValue::invalidate() {
  if (auto runtime = m_weakRuntime.get()) {
    m_weakRuntime = nullptr;
    runtime.ReleasePropertyNameId(m_propertyId);
  }
}

} // namespace winrt::Microsoft::ReactNative
