// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiAbiApi.h"

using namespace facebook::jsi;

namespace winrt::Microsoft::ReactNative {

//===========================================================================
// Helper functions
//===========================================================================

namespace {

JsiValueData &&ToJsiValueData(Value &&value) noexcept {
  return reinterpret_cast<JsiValueData &&>(value);
}

JsiValueData const &ToJsiValueData(Value const &value) noexcept {
  return reinterpret_cast<JsiValueData const &>(value);
}

Value &&ToValue(JsiValueData &&valueData) noexcept {
  return reinterpret_cast<Value &&>(valueData);
}

Value const &ToValue(JsiValueData const &valueData) noexcept {
  return reinterpret_cast<Value const &>(valueData);
}

} // namespace

//===========================================================================
// JsiBufferWrapper implementation
//===========================================================================

JsiBufferWrapper::JsiBufferWrapper(std::shared_ptr<Buffer const> const &buffer) noexcept : m_buffer{buffer} {}

JsiBufferWrapper::~JsiBufferWrapper() = default;

uint32_t JsiBufferWrapper::Size() {
  return static_cast<uint32_t>(m_buffer->size());
}

void JsiBufferWrapper::GetData(JsiDataHandler const &handler) {
  handler(winrt::array_view<uint8_t const>{m_buffer->data(), m_buffer->data() + m_buffer->size()});
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
/*static*/ std::unordered_map<JsiPointerHandle, JsiHostObjectWrapper *>
    JsiHostObjectWrapper::s_objectHandleToObjectWrapper;

JsiHostObjectWrapper::JsiHostObjectWrapper(std::shared_ptr<HostObject> &&hostObject) noexcept
    : m_hostObject(std::move(hostObject)) {}

JsiHostObjectWrapper::~JsiHostObjectWrapper() noexcept {
  if (m_objectHandle) {
    std::scoped_lock lock{s_mutex};
    s_objectHandleToObjectWrapper.erase(m_objectHandle);
  }
}

JsiValueData JsiHostObjectWrapper::GetProperty(IJsiRuntime const &runtime, JsiPointerHandle name) {
  JsiAbiRuntime rt{runtime};
  return ToJsiValueData(m_hostObject->get(rt, *reinterpret_cast<PropNameID *>(name)));
}

void JsiHostObjectWrapper::SetProperty(IJsiRuntime const &runtime, JsiPointerHandle name, JsiValueData const &value) {
  JsiAbiRuntime rt{runtime};
  m_hostObject->set(rt, *reinterpret_cast<PropNameID *>(name), *reinterpret_cast<Value const *>(&value));
}

Windows::Foundation::Collections::IVectorView<JsiPointerHandle> JsiHostObjectWrapper::GetPropertyNames(
    IJsiRuntime const &runtime) {
  JsiAbiRuntime rt{runtime};
  auto names = m_hostObject->getPropertyNames(rt);
  std::vector<JsiPointerHandle> result;
  result.reserve(names.size());
  for (auto &name : names) {
    result.push_back(*reinterpret_cast<JsiPointerHandle *>(&name));
  }

  return winrt::single_threaded_vector<JsiPointerHandle>(std::move(result)).GetView();
}

/*static*/ void JsiHostObjectWrapper::RegisterHostObject(
    JsiPointerHandle handle,
    JsiHostObjectWrapper *hostObject) noexcept {
  std::scoped_lock lock{s_mutex};
  s_objectHandleToObjectWrapper[handle] = hostObject;
  hostObject->m_objectHandle = handle;
}

/*static*/ bool JsiHostObjectWrapper::IsHostObject(JsiPointerHandle handle) noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = s_objectHandleToObjectWrapper.find(handle);
  return it != s_objectHandleToObjectWrapper.end();
}

/*static*/ std::shared_ptr<HostObject> JsiHostObjectWrapper::GetHostObject(JsiPointerHandle handle) noexcept {
  auto it = s_objectHandleToObjectWrapper.find(handle);
  if (it != s_objectHandleToObjectWrapper.end()) {
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
/*static*/ std::unordered_map<JsiPointerHandle, JsiHostFunctionWrapper *>
    JsiHostFunctionWrapper::s_functionHandleToFunctionWrapper;

JsiHostFunctionWrapper::JsiHostFunctionWrapper(HostFunctionType &&hostFunction, uint32_t functionId) noexcept
    : m_hostFunction{std::move(hostFunction)}, m_functionId{functionId} {
  VerifyElseCrashSz(functionId, "Function Id must be not zero");
  std::scoped_lock lock{s_functionMutex};
  s_functionIdToFunctionWrapper[functionId] = this;
}

JsiHostFunctionWrapper::JsiHostFunctionWrapper(JsiHostFunctionWrapper &&other) noexcept
    : m_hostFunction{std::move(other.m_hostFunction)},
      m_functionId{std::exchange(other.m_functionId, 0)},
      m_functionHandle{std::exchange(other.m_functionHandle, 0)} {
  std::scoped_lock lock{s_functionMutex};
  if (m_functionId) {
    s_functionIdToFunctionWrapper[m_functionId] = this;
  }
  if (m_functionHandle) {
    s_functionHandleToFunctionWrapper[m_functionHandle] = this;
  }
}

JsiHostFunctionWrapper &JsiHostFunctionWrapper::operator=(JsiHostFunctionWrapper &&other) noexcept {
  if (this != &other) {
    m_hostFunction = std::move(other.m_hostFunction);
    m_functionId = std::exchange(other.m_functionId, 0);
    m_functionHandle = std::exchange(other.m_functionHandle, 0);
    std::scoped_lock lock{s_functionMutex};
    if (m_functionId) {
      s_functionIdToFunctionWrapper[m_functionId] = this;
    }
    if (m_functionHandle) {
      s_functionHandleToFunctionWrapper[m_functionHandle] = this;
    }
  }
  return *this;
}

JsiHostFunctionWrapper::~JsiHostFunctionWrapper() noexcept {
  if (m_functionId || m_functionHandle) {
    std::scoped_lock lock{s_functionMutex};
    auto it1 = s_functionIdToFunctionWrapper.find(m_functionId);
    if (it1 != s_functionIdToFunctionWrapper.end()) {
      s_functionIdToFunctionWrapper.erase(it1);
    }
    auto it2 = s_functionHandleToFunctionWrapper.find(m_functionHandle);
    if (it2 != s_functionHandleToFunctionWrapper.end()) {
      s_functionHandleToFunctionWrapper.erase(it2);
    }
  }
}

JsiValueData JsiHostFunctionWrapper::
operator()(IJsiRuntime const &runtime, JsiValueData const &thisValue, array_view<JsiValueData const> args) {
  auto rt{JsiAbiRuntime{runtime}};
  return ToJsiValueData(
      m_hostFunction(rt, ToValue(thisValue), reinterpret_cast<Value const *>(args.data()), args.size()));
}

/*static*/ uint32_t JsiHostFunctionWrapper::GetNextFunctionId() noexcept {
  return ++s_functionIdGenerator;
}

/*static*/ void JsiHostFunctionWrapper::RegisterHostFunction(uint32_t functionId, JsiPointerHandle handle) noexcept {
  std::scoped_lock lock{s_functionMutex};
  auto it = s_functionIdToFunctionWrapper.find(functionId);
  VerifyElseCrashSz(it != s_functionIdToFunctionWrapper.end(), "Function Id is not found.");
  JsiHostFunctionWrapper *functionWrapper = it->second;
  s_functionHandleToFunctionWrapper[handle] = functionWrapper;
  functionWrapper->m_functionHandle = handle;
}

/*static*/ bool JsiHostFunctionWrapper::IsHostFunction(JsiPointerHandle functionHandle) noexcept {
  std::scoped_lock lock{s_functionMutex};
  return s_functionHandleToFunctionWrapper.find(functionHandle) != s_functionHandleToFunctionWrapper.end();
}

/*static*/ HostFunctionType &JsiHostFunctionWrapper::GetHostFunction(JsiPointerHandle functionHandle) noexcept {
  std::scoped_lock lock{s_functionMutex};
  auto it = s_functionHandleToFunctionWrapper.find(functionHandle);
  if (it != s_functionHandleToFunctionWrapper.end()) {
    return it->second->m_hostFunction;
  }

  VerifyElseCrashSz(false, "Function is not a host function");
}

//===========================================================================
// JsiAbiRuntime implementation
//===========================================================================

JsiAbiRuntime::JsiAbiRuntime(IJsiRuntime const &runtime) noexcept : m_runtime{runtime} {}

JsiAbiRuntime::~JsiAbiRuntime() = default;

/*static*/ String &&JsiAbiRuntime::AsString(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<String &&>(pointer);
}

/*static*/ PropNameID &&JsiAbiRuntime::AsPropNameID(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<PropNameID &&>(pointer);
}

/*static*/ Symbol &&JsiAbiRuntime::AsSymbol(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<Symbol &&>(pointer);
}

/*static*/ Object &&JsiAbiRuntime::AsObject(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<Object &&>(pointer);
}

/*static*/ WeakObject &&JsiAbiRuntime::AsWeakObject(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<WeakObject &&>(pointer);
}

/*static*/ Function &&JsiAbiRuntime::AsFunction(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<Function &&>(pointer);
}

/*static*/ Array &&JsiAbiRuntime::AsArray(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<Array &&>(pointer);
}

/*static*/ ArrayBuffer &&JsiAbiRuntime::AsArrayBuffer(JsiPointerHandle &&pointer) noexcept {
  return reinterpret_cast<ArrayBuffer &&>(pointer);
}

/*static*/ JsiPointerHandle JsiAbiRuntime::ToJsiPointerHandle(Runtime::PointerValue const *pointerValue) noexcept {
  return reinterpret_cast<JsiPointerHandle>(pointerValue);
}

/*static*/ JsiPointerHandle JsiAbiRuntime::ToJsiPointerHandle(Pointer const &pointer) noexcept {
  return ToJsiPointerHandle(Runtime::getPointerValue(pointer));
}

/*static*/ Runtime::PointerValue *JsiAbiRuntime::ToPointerValue(JsiPointerHandle pointerHandle) noexcept {
  return reinterpret_cast<Runtime::PointerValue *>(pointerHandle);
}

Value JsiAbiRuntime::evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer, const std::string &sourceURL) {
  return ToValue(m_runtime.EvaluateJavaScript(winrt::make<JsiBufferWrapper>(buffer), to_hstring(sourceURL)));
}

std::shared_ptr<const PreparedJavaScript> JsiAbiRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceURL) {
  return std::make_shared<JsiPreparedJavaScriptWrapper>(
      m_runtime.PrepareJavaScript(winrt::make<JsiBufferWrapper>(std::move(buffer)), to_hstring(sourceURL)));
}

Value JsiAbiRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const PreparedJavaScript> &js) {
  return ToValue(
      m_runtime.EvaluatePreparedJavaScript(std::static_pointer_cast<JsiPreparedJavaScriptWrapper const>(js)->Get()));
}

Object JsiAbiRuntime::global() {
  return AsObject(m_runtime.Global());
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
  return ToPointerValue(m_runtime.CloneSymbol(ToJsiPointerHandle(pv)));
}

Runtime::PointerValue *JsiAbiRuntime::cloneString(const Runtime::PointerValue *pv) {
  return ToPointerValue(m_runtime.CloneString(ToJsiPointerHandle(pv)));
}

Runtime::PointerValue *JsiAbiRuntime::cloneObject(const Runtime::PointerValue *pv) {
  return ToPointerValue(m_runtime.CloneObject(ToJsiPointerHandle(pv)));
}

Runtime::PointerValue *JsiAbiRuntime::clonePropNameID(const Runtime::PointerValue *pv) {
  return ToPointerValue(m_runtime.ClonePropertyNameId(ToJsiPointerHandle(pv)));
}

PropNameID JsiAbiRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  auto data = reinterpret_cast<uint8_t const *>(str);
  return AsPropNameID(m_runtime.CreatePropertyNameIdFromAscii({data, data + length}));
}

PropNameID JsiAbiRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  return AsPropNameID(m_runtime.CreatePropertyNameIdFromUtf8({utf8, utf8 + length}));
}

PropNameID JsiAbiRuntime::createPropNameIDFromString(const String &str) {
  return AsPropNameID(m_runtime.CreatePropertyNameIdFromString(ToJsiPointerHandle(str)));
}

std::string JsiAbiRuntime::utf8(const PropNameID &propertyNameId) {
  std::string result;
  m_runtime.PropertyNameIdToUtf8(ToJsiPointerHandle(propertyNameId), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

bool JsiAbiRuntime::compare(const PropNameID &left, const PropNameID &right) {
  return m_runtime.ComparePropertyNameIds(ToJsiPointerHandle(left), ToJsiPointerHandle(right));
}

std::string JsiAbiRuntime::symbolToString(const Symbol &symbol) {
  std::string result;
  m_runtime.SymbolToUtf8(ToJsiPointerHandle(symbol), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

String JsiAbiRuntime::createStringFromAscii(const char *str, size_t length) {
  auto ascii = reinterpret_cast<uint8_t const *>(str);
  return AsString(m_runtime.CreateStringFromAscii({ascii, ascii + length}));
}

String JsiAbiRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length) {
  return AsString(m_runtime.CreateStringFromAscii({utf8, utf8 + length}));
}

std::string JsiAbiRuntime::utf8(const String &str) {
  std::string result;
  m_runtime.StringToUtf8(ToJsiPointerHandle(str), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
}

Value JsiAbiRuntime::createValueFromJsonUtf8(const uint8_t *json, size_t length) {
  return ToValue(m_runtime.CreateValueFromJsonUtf8({json, json + length}));
}

Object JsiAbiRuntime::createObject() {
  return AsObject(m_runtime.CreateObject());
}

Object JsiAbiRuntime::createObject(std::shared_ptr<HostObject> ho) {
  auto hostObjectWrapper = winrt::make<JsiHostObjectWrapper>(std::move(ho));
  Object result = AsObject(m_runtime.CreateObjectWithHostObject(hostObjectWrapper));
  JsiHostObjectWrapper::RegisterHostObject(
      ToJsiPointerHandle(result), get_self<JsiHostObjectWrapper>(hostObjectWrapper));
  return result;
}

std::shared_ptr<HostObject> JsiAbiRuntime::getHostObject(const Object &obj) {
  return JsiHostObjectWrapper::GetHostObject(ToJsiPointerHandle(obj));
}

HostFunctionType &JsiAbiRuntime::getHostFunction(const Function &func) {
  return JsiHostFunctionWrapper::GetHostFunction(ToJsiPointerHandle(func));
}

Value JsiAbiRuntime::getProperty(const Object &obj, const PropNameID &name) {
  return ToValue(m_runtime.GetProperty(ToJsiPointerHandle(obj), ToJsiPointerHandle(name)));
}

Value JsiAbiRuntime::getProperty(const Object &obj, const String &name) {
  return ToValue(m_runtime.GetPropertyWithString(ToJsiPointerHandle(obj), ToJsiPointerHandle(name)));
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const PropNameID &name) {
  return m_runtime.HasProperty(ToJsiPointerHandle(obj), ToJsiPointerHandle(name));
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const String &name) {
  return m_runtime.HasPropertyWithString(ToJsiPointerHandle(obj), ToJsiPointerHandle(name));
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const PropNameID &name, const Value &value) {
  m_runtime.SetProperty(ToJsiPointerHandle(obj), ToJsiPointerHandle(name), ToJsiValueData(value));
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const String &name, const Value &value) {
  m_runtime.SetPropertyWithString(ToJsiPointerHandle(obj), ToJsiPointerHandle(name), ToJsiValueData(value));
}

bool JsiAbiRuntime::isArray(const Object &obj) const {
  return m_runtime.IsArray(ToJsiPointerHandle(obj));
}

bool JsiAbiRuntime::isArrayBuffer(const Object &obj) const {
  return m_runtime.IsArrayBuffer(ToJsiPointerHandle(obj));
}

bool JsiAbiRuntime::isFunction(const Object &obj) const {
  return m_runtime.IsFunction(ToJsiPointerHandle(obj));
}

bool JsiAbiRuntime::isHostObject(const Object &obj) const {
  return m_runtime.IsHostObject(ToJsiPointerHandle(obj));
}

bool JsiAbiRuntime::isHostFunction(const Function &func) const {
  return JsiHostFunctionWrapper::IsHostFunction(ToJsiPointerHandle(func));
}

Array JsiAbiRuntime::getPropertyNames(const Object &obj) {
  return AsArray(m_runtime.GetPropertyNameArray(ToJsiPointerHandle(obj)));
}

WeakObject JsiAbiRuntime::createWeakObject(const Object &obj) {
  return AsWeakObject(m_runtime.CreateWeakObject(ToJsiPointerHandle(obj)));
}

Value JsiAbiRuntime::lockWeakObject(const WeakObject &weakObj) {
  return ToValue(m_runtime.LockWeakObject(ToJsiPointerHandle(weakObj)));
}

Array JsiAbiRuntime::createArray(size_t length) {
  return AsArray(m_runtime.CreateArray(static_cast<uint32_t>(length)));
}

size_t JsiAbiRuntime::size(const Array &arr) {
  return m_runtime.GetArraySize(ToJsiPointerHandle(arr));
}

size_t JsiAbiRuntime::size(const ArrayBuffer &arrayBuffer) {
  return m_runtime.GetArrayBufferSize(ToJsiPointerHandle(arrayBuffer));
}

uint8_t *JsiAbiRuntime::data(const ArrayBuffer &arrayBuffer) {
  uint8_t *result{};
  m_runtime.ArrayBufferToUtf8(ToJsiPointerHandle(arrayBuffer), [&result](array_view<uint8_t const> dataView) {
    result = const_cast<uint8_t *>(dataView.data());
  });
  return result;
}

Value JsiAbiRuntime::getValueAtIndex(const Array &arr, size_t i) {
  return ToValue(m_runtime.GetValueAtIndex(ToJsiPointerHandle(arr), static_cast<uint32_t>(i)));
}

void JsiAbiRuntime::setValueAtIndexImpl(Array &arr, size_t i, const Value &value) {
  m_runtime.SetValueAtIndex(ToJsiPointerHandle(arr), static_cast<uint32_t>(i), ToJsiValueData(value));
}

Function
JsiAbiRuntime::createFunctionFromHostFunction(const PropNameID &name, unsigned int paramCount, HostFunctionType func) {
  uint32_t functionId = JsiHostFunctionWrapper::GetNextFunctionId();
  Function result = AsFunction(m_runtime.CreateFunctionFromHostFunction(
      ToJsiPointerHandle(name), paramCount, JsiHostFunctionWrapper(std::move(func), functionId)));
  JsiHostFunctionWrapper::RegisterHostFunction(functionId, ToJsiPointerHandle(result));
  return result;
}

Value JsiAbiRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count) {
  JsiValueData const *argsData = reinterpret_cast<JsiValueData const *>(args);
  return ToValue(m_runtime.Call(ToJsiPointerHandle(func), ToJsiValueData(jsThis), {argsData, argsData + count}));
}

Value JsiAbiRuntime::callAsConstructor(const Function &func, const Value *args, size_t count) {
  JsiValueData const *argsData = reinterpret_cast<JsiValueData const *>(args);
  return ToValue(m_runtime.CallAsConstructor(ToJsiPointerHandle(func), {argsData, argsData + count}));
}

Runtime::ScopeState *JsiAbiRuntime::pushScope() {
  return reinterpret_cast<ScopeState *>(m_runtime.PushScope());
}

void JsiAbiRuntime::popScope(ScopeState *scope) {
  m_runtime.PopScope(reinterpret_cast<uint64_t>(scope));
}

bool JsiAbiRuntime::strictEquals(const Symbol &a, const Symbol &b) const {
  return m_runtime.SymbolStrictEquals(ToJsiPointerHandle(a), ToJsiPointerHandle(b));
}

bool JsiAbiRuntime::strictEquals(const String &a, const String &b) const {
  return m_runtime.StringStrictEquals(ToJsiPointerHandle(a), ToJsiPointerHandle(b));
}

bool JsiAbiRuntime::strictEquals(const Object &a, const Object &b) const {
  return m_runtime.ObjectStrictEquals(ToJsiPointerHandle(a), ToJsiPointerHandle(b));
}

bool JsiAbiRuntime::instanceOf(const Object &o, const Function &f) {
  return m_runtime.InstanceOf(ToJsiPointerHandle(o), ToJsiPointerHandle(f));
}

} // namespace winrt::Microsoft::ReactNative
