// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiAbiApi.h"
#include <utility>

using namespace facebook::jsi;

namespace winrt::Microsoft::ReactNative {

// The macro to simplify recording JSI exceptions.
// It looks strange to keep the normal structure of the try/catch in code.
#define JSI_RUNTIME_SET_ERROR(runtime)                            \
facebook::jsi::JSError const &jsError) {                          \
    JsiAbiRuntime::FromJsiRuntime(runtime)->SetJsiError(jsError); \
    throw;                                                        \
  }                                                               \
  catch (std::exception const &ex) {                              \
    JsiAbiRuntime::FromJsiRuntime(runtime)->SetJsiError(ex);      \
    throw;                                                        \
  } catch (...

//===========================================================================
// AbiJSError implementation
//===========================================================================

struct AbiJSError : facebook::jsi::JSError {
  AbiJSError(JsiAbiRuntime &rt, JsiError &&jsiError) noexcept
      : facebook::jsi::JSError{to_string(jsiError.What()),
                               rt,
                               facebook::jsi::Value(rt, JsiAbiRuntime::ValueRef(jsiError.Value()))},
        m_jsiError{std::move(jsiError)} {}

 private:
  JsiError m_jsiError;
};

//===========================================================================
// AbiJSINativeException implementation
//===========================================================================

struct AbiJSINativeException : facebook::jsi::JSINativeException {
  AbiJSINativeException(JsiError &&jsiError) noexcept
      : facebook::jsi::JSINativeException{to_string(jsiError.What())}, m_jsiError{std::move(jsiError)} {}

 private:
  JsiError m_jsiError;
};

//===========================================================================
// JsiBufferWrapper implementation
//===========================================================================

JsiByteBufferWrapper::JsiByteBufferWrapper(
    JsiRuntime const &runtime,
    std::shared_ptr<Buffer const> const &buffer) noexcept
    : m_runtime{runtime}, m_buffer{buffer} {}

JsiByteBufferWrapper::~JsiByteBufferWrapper() = default;

uint32_t JsiByteBufferWrapper::Size() try {
  return static_cast<uint32_t>(m_buffer->size());
} catch (JSI_RUNTIME_SET_ERROR(m_runtime)) {
  throw;
}

void JsiByteBufferWrapper::GetData(JsiByteArrayUser const &useBytes) try {
  useBytes(winrt::array_view<uint8_t const>{m_buffer->data(), m_buffer->data() + m_buffer->size()});
} catch (JSI_RUNTIME_SET_ERROR(m_runtime)) {
  throw;
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
/*static*/ std::map<uint64_t, JsiHostObjectWrapper *> JsiHostObjectWrapper::s_objectDataToObjectWrapper;

JsiHostObjectWrapper::JsiHostObjectWrapper(std::shared_ptr<HostObject> &&hostObject) noexcept
    : m_hostObject(std::move(hostObject)) {}

JsiHostObjectWrapper::~JsiHostObjectWrapper() noexcept {
  if (m_objectData.Data) {
    std::scoped_lock lock{s_mutex};
    s_objectDataToObjectWrapper.erase(m_objectData.Data);
  }
}

JsiValueData JsiHostObjectWrapper::GetProperty(JsiRuntime const &runtime, JsiPropertyNameIdData const &name) try {
  JsiAbiRuntime *rt = JsiAbiRuntime::FromJsiRuntime(runtime);
  JsiAbiRuntime::PropNameIDRef nameRef{name};
  return JsiAbiRuntime::DetachJsiValueData(m_hostObject->get(*rt, nameRef));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

void JsiHostObjectWrapper::SetProperty(
    JsiRuntime const &runtime,
    JsiPropertyNameIdData const &name,
    JsiValueData const &value) try {
  JsiAbiRuntime *rt = JsiAbiRuntime::FromJsiRuntime(runtime);
  m_hostObject->set(*rt, JsiAbiRuntime::PropNameIDRef{name}, JsiAbiRuntime::ValueRef(value));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
}

Windows::Foundation::Collections::IVector<JsiPropertyNameIdData> JsiHostObjectWrapper::GetPropertyNames(
    JsiRuntime const &runtime) try {
  JsiAbiRuntime *rt = JsiAbiRuntime::FromJsiRuntime(runtime);
  auto names = m_hostObject->getPropertyNames(*rt);
  std::vector<JsiPropertyNameIdData> result;
  result.reserve(names.size());
  for (auto &name : names) {
    result.push_back(JsiAbiRuntime::DetachJsiPropertyNameIdData(std::move(name)));
  }

  return winrt::single_threaded_vector<JsiPropertyNameIdData>(std::move(result));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
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
/*static*/ std::map<uint32_t, JsiHostFunctionWrapper *> JsiHostFunctionWrapper::s_functionIdToFunctionWrapper;
/*static*/ std::map<uint64_t, JsiHostFunctionWrapper *> JsiHostFunctionWrapper::s_functionDataToFunctionWrapper;

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
operator()(JsiRuntime const &runtime, JsiValueData const &thisArg, array_view<JsiValueData const> args) try {
  JsiAbiRuntime *rt = JsiAbiRuntime::FromJsiRuntime(runtime);
  JsiAbiRuntime::ValueRefArray valueRefArgs{args};
  return JsiAbiRuntime::DetachJsiValueData(
      m_hostFunction(*rt, JsiAbiRuntime::ValueRef{thisArg}, valueRefArgs.Data(), valueRefArgs.Size()));
} catch (JSI_RUNTIME_SET_ERROR(runtime)) {
  throw;
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

/*static*/ std::mutex JsiAbiRuntime::s_mutex;
/*static*/ std::map<void *, JsiAbiRuntime *> JsiAbiRuntime::s_jsiAbiRuntimeMap;

JsiAbiRuntime::JsiAbiRuntime(JsiRuntime const &runtime) noexcept : m_runtime{runtime} {
  std::scoped_lock lock{s_mutex};
  s_jsiAbiRuntimeMap.try_emplace(get_abi(runtime), this);
}

JsiAbiRuntime::~JsiAbiRuntime() {
  std::scoped_lock lock{s_mutex};
  s_jsiAbiRuntimeMap.erase(get_abi(m_runtime));
}

/*static*/ JsiAbiRuntime *JsiAbiRuntime::FromJsiRuntime(JsiRuntime const &jsiRuntime) noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = s_jsiAbiRuntimeMap.find(get_abi(jsiRuntime));
  if (it != s_jsiAbiRuntimeMap.end()) {
    return it->second;
  } else {
    return nullptr;
  }
}

Value JsiAbiRuntime::evaluateJavaScript(const std::shared_ptr<const Buffer> &buffer, const std::string &sourceURL) try {
  return MakeValue(
      m_runtime.EvaluateJavaScript(winrt::make<JsiByteBufferWrapper>(m_runtime, buffer), to_hstring(sourceURL)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::shared_ptr<const PreparedJavaScript> JsiAbiRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceURL) try {
  return std::make_shared<JsiPreparedJavaScriptWrapper>(m_runtime.PrepareJavaScript(
      winrt::make<JsiByteBufferWrapper>(m_runtime, std::move(buffer)), to_hstring(sourceURL)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::evaluatePreparedJavaScript(const std::shared_ptr<const PreparedJavaScript> &js) try {
  return MakeValue(
      m_runtime.EvaluatePreparedJavaScript(std::static_pointer_cast<JsiPreparedJavaScriptWrapper const>(js)->Get()));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::global() try { return MakeObject(m_runtime.Global()); } catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::description() try {
  return to_string(m_runtime.Description());
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isInspectable() try { return m_runtime.IsInspectable(); } catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}
Instrumentation &JsiAbiRuntime::instrumentation() try {
  // TODO: implement
  VerifyElseCrash(false);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneSymbol(const Runtime::PointerValue *pv) try {
  return new SymbolPointerValue{make_weak(m_runtime), m_runtime.CloneSymbol(AsJsiSymbolData(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneString(const Runtime::PointerValue *pv) try {
  return new StringPointerValue{make_weak(m_runtime), m_runtime.CloneString(AsJsiStringData(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::cloneObject(const Runtime::PointerValue *pv) try {
  return new ObjectPointerValue{make_weak(m_runtime), m_runtime.CloneObject(AsJsiObjectData(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::PointerValue *JsiAbiRuntime::clonePropNameID(const Runtime::PointerValue *pv) try {
  return new PropNameIDPointerValue{make_weak(m_runtime), m_runtime.ClonePropertyNameId(AsJsiPropertyNameIdData(pv))};
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromAscii(const char *str, size_t length) try {
  auto data = reinterpret_cast<uint8_t const *>(str);
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromAscii({data, data + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) try {
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromUtf8({utf8, utf8 + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

PropNameID JsiAbiRuntime::createPropNameIDFromString(const String &str) try {
  return MakePropNameID(m_runtime.CreatePropertyNameIdFromString(AsJsiStringData(str)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::utf8(const PropNameID &propertyNameId) try {
  std::string result;
  m_runtime.PropertyNameIdToUtf8(AsJsiPropertyNameIdData(propertyNameId), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::compare(const PropNameID &left, const PropNameID &right) try {
  return m_runtime.PropertyNameIdEquals(AsJsiPropertyNameIdData(left), AsJsiPropertyNameIdData(right));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::symbolToString(const Symbol &symbol) try {
  std::string result;
  m_runtime.SymbolToUtf8(AsJsiSymbolData(symbol), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

String JsiAbiRuntime::createStringFromAscii(const char *str, size_t length) try {
  auto ascii = reinterpret_cast<uint8_t const *>(str);
  return MakeString(m_runtime.CreateStringFromAscii({ascii, ascii + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

String JsiAbiRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length) try {
  return MakeString(m_runtime.CreateStringFromAscii({utf8, utf8 + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::string JsiAbiRuntime::utf8(const String &str) try {
  std::string result;
  m_runtime.StringToUtf8(AsJsiStringData(str), [&result](array_view<uint8_t const> utf8) {
    result.assign(reinterpret_cast<char const *>(utf8.data()), utf8.size());
  });
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::createValueFromJsonUtf8(const uint8_t *json, size_t length) try {
  return MakeValue(m_runtime.CreateValueFromJsonUtf8({json, json + length}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::createObject() try {
  return MakeObject(m_runtime.CreateObject());
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Object JsiAbiRuntime::createObject(std::shared_ptr<HostObject> ho) try {
  auto hostObjectWrapper = winrt::make<JsiHostObjectWrapper>(std::move(ho));
  Object result = MakeObject(m_runtime.CreateObjectWithHostObject(hostObjectWrapper));
  JsiHostObjectWrapper::RegisterHostObject(AsJsiObjectData(result), get_self<JsiHostObjectWrapper>(hostObjectWrapper));
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

std::shared_ptr<HostObject> JsiAbiRuntime::getHostObject(const Object &obj) try {
  return JsiHostObjectWrapper::GetHostObject(AsJsiObjectData(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

HostFunctionType &JsiAbiRuntime::getHostFunction(const Function &func) try {
  return JsiHostFunctionWrapper::GetHostFunction(AsJsiFunctionData(func));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getProperty(const Object &obj, const PropNameID &name) try {
  return MakeValue(m_runtime.GetPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getProperty(const Object &obj, const String &name) try {
  return MakeValue(m_runtime.GetPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const PropNameID &name) try {
  return m_runtime.HasPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::hasProperty(const Object &obj, const String &name) try {
  return m_runtime.HasPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const PropNameID &name, const Value &value) try {
  m_runtime.SetPropertyById(AsJsiObjectData(obj), AsJsiPropertyNameIdData(name), AsJsiValueData(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setPropertyValue(Object &obj, const String &name, const Value &value) try {
  m_runtime.SetPropertyByName(AsJsiObjectData(obj), AsJsiStringData(name), AsJsiValueData(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isArray(const Object &obj) const try {
  return m_runtime.IsArray(AsJsiObjectData(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isArrayBuffer(const Object &obj) const try {
  return m_runtime.IsArrayBuffer(AsJsiObjectData(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isFunction(const Object &obj) const try {
  return m_runtime.IsFunction(AsJsiObjectData(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isHostObject(const Object &obj) const try {
  return m_runtime.IsHostObject(AsJsiObjectData(obj));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::isHostFunction(const Function &func) const try {
  return JsiHostFunctionWrapper::IsHostFunction(AsJsiFunctionData(func));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Array JsiAbiRuntime::getPropertyNames(const Object &obj) try {
  return MakeArray(m_runtime.GetPropertyNames(AsJsiObjectData(obj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

WeakObject JsiAbiRuntime::createWeakObject(const Object &obj) try {
  return MakeWeakObject(m_runtime.CreateWeakObject(AsJsiObjectData(obj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::lockWeakObject(const WeakObject &weakObj) try {
  return MakeValue(m_runtime.LockWeakObject(AsJsiWeakObjectData(weakObj)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Array JsiAbiRuntime::createArray(size_t length) try {
  return MakeArray(m_runtime.CreateArray(static_cast<uint32_t>(length)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

size_t JsiAbiRuntime::size(const Array &arr) try {
  return m_runtime.GetArraySize(AsJsiArrayData(arr));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

size_t JsiAbiRuntime::size(const ArrayBuffer &arrayBuffer) try {
  return m_runtime.GetArrayBufferSize(AsJsiArrayBufferData(arrayBuffer));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

uint8_t *JsiAbiRuntime::data(const ArrayBuffer &arrayBuffer) try {
  uint8_t *result{};
  m_runtime.GetArrayBufferData(AsJsiArrayBufferData(arrayBuffer), [&result](array_view<uint8_t const> dataView) {
    result = const_cast<uint8_t *>(dataView.data());
  });
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::getValueAtIndex(const Array &arr, size_t i) try {
  return MakeValue(m_runtime.GetValueAtIndex(AsJsiArrayData(arr), static_cast<uint32_t>(i)));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::setValueAtIndexImpl(Array &arr, size_t i, const Value &value) try {
  m_runtime.SetValueAtIndex(AsJsiArrayData(arr), static_cast<uint32_t>(i), AsJsiValueData(value));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Function JsiAbiRuntime::createFunctionFromHostFunction(
    const PropNameID &name,
    unsigned int paramCount,
    HostFunctionType func) try {
  uint32_t functionId = JsiHostFunctionWrapper::GetNextFunctionId();
  Function result = MakeFunction(m_runtime.CreateFunctionFromHostFunction(
      AsJsiPropertyNameIdData(name), paramCount, JsiHostFunctionWrapper(std::move(func), functionId)));
  JsiHostFunctionWrapper::RegisterHostFunction(functionId, AsJsiFunctionData(result));
  return result;
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::call(const Function &func, const Value &jsThis, const Value *args, size_t count) try {
  VerifyElseCrashSz(count <= MaxCallArgCount, "Argument count must not exceed the supported max arg count.");
  std::array<JsiValueData, MaxCallArgCount> argsData;
  for (size_t i = 0; i < count; ++i) {
    argsData[i] = AsJsiValueData(*(args + i));
  }

  return MakeValue(
      m_runtime.Call(AsJsiFunctionData(func), AsJsiValueData(jsThis), {argsData.data(), argsData.data() + count}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Value JsiAbiRuntime::callAsConstructor(const Function &func, const Value *args, size_t count) try {
  VerifyElseCrashSz(count <= MaxCallArgCount, "Argument count must not exceed the supported max arg count.");
  std::array<JsiValueData, MaxCallArgCount> argsData;
  for (size_t i = 0; i < count; ++i) {
    argsData[i] = AsJsiValueData(*(args + i));
  }

  return MakeValue(m_runtime.CallAsConstructor(AsJsiFunctionData(func), {argsData.data(), argsData.data() + count}));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

Runtime::ScopeState *JsiAbiRuntime::pushScope() try {
  return reinterpret_cast<ScopeState *>(m_runtime.PushScope().Data);
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::popScope(ScopeState *scope) try {
  m_runtime.PopScope({reinterpret_cast<uint64_t>(scope)});
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const Symbol &a, const Symbol &b) const try {
  return m_runtime.SymbolStrictEquals(AsJsiSymbolData(a), AsJsiSymbolData(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const String &a, const String &b) const try {
  return m_runtime.StringStrictEquals(AsJsiStringData(a), AsJsiStringData(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::strictEquals(const Object &a, const Object &b) const try {
  return m_runtime.ObjectStrictEquals(AsJsiObjectData(a), AsJsiObjectData(b));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

bool JsiAbiRuntime::instanceOf(const Object &o, const Function &f) try {
  return m_runtime.InstanceOf(AsJsiObjectData(o), AsJsiFunctionData(f));
} catch (hresult_error const &) {
  RethrowJsiError();
  throw;
}

void JsiAbiRuntime::RethrowJsiError() const {
  auto jsiError = m_runtime.GetAndRemoveError();
  if (jsiError.ErrorType() == JsiErrorType::JSError) {
    throw AbiJSError{*const_cast<JsiAbiRuntime *>(this), std::move(jsiError)};
  } else {
    throw AbiJSINativeException{std::move(jsiError)};
  }
}

void JsiAbiRuntime::SetJsiError(facebook::jsi::JSError const &jsError) noexcept {
  m_runtime.SetError(JsiErrorType::JSError, to_hstring(jsError.what()), AsJsiValueData(jsError.value()));
}

void JsiAbiRuntime::SetJsiError(std::exception const &nativeException) noexcept {
  m_runtime.SetError(
      JsiErrorType::NativeException,
      to_hstring(nativeException.what()),
      AsJsiValueData(facebook::jsi::Value::undefined()));
}

//===========================================================================
// JsiAbiRuntime utility functions implementation
//===========================================================================

/*static*/ JsiSymbolData const &JsiAbiRuntime::AsJsiSymbolData(PointerValue const *pv) noexcept {
  return SymbolPointerValue::GetData(pv);
}

/*static*/ JsiStringData const &JsiAbiRuntime::AsJsiStringData(PointerValue const *pv) noexcept {
  return StringPointerValue::GetData(pv);
}

/*static*/ JsiObjectData const &JsiAbiRuntime::AsJsiObjectData(PointerValue const *pv) noexcept {
  return ObjectPointerValue::GetData(pv);
}

/*static*/ JsiPropertyNameIdData const &JsiAbiRuntime::AsJsiPropertyNameIdData(PointerValue const *pv) noexcept {
  return PropNameIDPointerValue::GetData(pv);
}

/*static*/ JsiSymbolData const &JsiAbiRuntime::AsJsiSymbolData(Symbol const &symbol) noexcept {
  return SymbolPointerValue::GetData(getPointerValue(symbol));
}

/*static*/ JsiStringData const &JsiAbiRuntime::AsJsiStringData(String const &str) noexcept {
  return StringPointerValue::GetData(getPointerValue(str));
}

/*static*/ JsiObjectData const &JsiAbiRuntime::AsJsiObjectData(Object const &obj) noexcept {
  return ObjectPointerValue::GetData(getPointerValue(obj));
}

/*static*/ JsiPropertyNameIdData const &JsiAbiRuntime::AsJsiPropertyNameIdData(PropNameID const &propertyId) noexcept {
  return PropNameIDPointerValue::GetData(getPointerValue(propertyId));
}

/*static*/ JsiFunctionData const &JsiAbiRuntime::AsJsiFunctionData(Function const &func) noexcept {
  return reinterpret_cast<JsiFunctionData const &>(ObjectPointerValue::GetData(getPointerValue(func)));
}

/*static*/ JsiWeakObjectData const &JsiAbiRuntime::AsJsiWeakObjectData(WeakObject const &weakObject) noexcept {
  return reinterpret_cast<JsiWeakObjectData const &>(ObjectPointerValue::GetData(getPointerValue(weakObject)));
}

/*static*/ JsiArrayData const &JsiAbiRuntime::AsJsiArrayData(facebook::jsi::Array const &arr) noexcept {
  return reinterpret_cast<JsiArrayData const &>(ObjectPointerValue::GetData(getPointerValue(arr)));
}

/*static*/ JsiArrayBufferData const &JsiAbiRuntime::AsJsiArrayBufferData(
    facebook::jsi::ArrayBuffer const &arrayBuffer) noexcept {
  return reinterpret_cast<JsiArrayBufferData const &>(ObjectPointerValue::GetData(getPointerValue(arrayBuffer)));
}

/*static*/ JsiValueData JsiAbiRuntime::AsJsiValueData(Value const &value) noexcept {
  // We assume that the JsiValueData and Value have the same layout.
  auto valuePtr = reinterpret_cast<JsiValueData const *>(&value);
  // Fix up the data part
  switch (valuePtr->Kind) {
    case JsiValueKind::Symbol:
      return {valuePtr->Kind, SymbolPointerValue::GetData(getPointerValue(value)).Data};
    case JsiValueKind::String:
      return {valuePtr->Kind, StringPointerValue::GetData(getPointerValue(value)).Data};
    case JsiValueKind::Object:
      return {valuePtr->Kind, ObjectPointerValue::GetData(getPointerValue(value)).Data};
    default: // return a copy for other cases.
      return *valuePtr;
  }
}

/*static*/ JsiPropertyNameIdData JsiAbiRuntime::DetachJsiPropertyNameIdData(PropNameID &&propertyId) noexcept {
  // This method detaches JsiPropertyNameIdData from the PropNameID.
  // It lets the PropNameIDPointerValue destructor run, but it must not destroy the underlying JS engine object.
  return PropNameIDPointerValue::Detach(getPointerValue(propertyId));
}

/*static*/ JsiValueData JsiAbiRuntime::DetachJsiValueData(facebook::jsi::Value &&value) noexcept {
  // Here we should move data from Value to JsiValueData.
  // For Pointer types it means that we should allow running the PointerValue
  // destructor, but it must not call the Release method to keep the underlying
  // data alive. Thus, we must detach the value.
  // We assume that the JsiValueData and Value have the same layout.
  auto valuePtr = reinterpret_cast<JsiValueData *>(&value);
  switch (valuePtr->Kind) {
    case JsiValueKind::Symbol:
      return {valuePtr->Kind, SymbolPointerValue::Detach(getPointerValue(value)).Data};
    case JsiValueKind::String:
      return {valuePtr->Kind, StringPointerValue::Detach(getPointerValue(value)).Data};
    case JsiValueKind::Object:
      return {valuePtr->Kind, ObjectPointerValue::Detach(getPointerValue(value)).Data};
    default: // return a copy for other cases.
      return *valuePtr;
  }
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
      return Value(reinterpret_cast<Value *>(&value)->getBool());
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

//===========================================================================
// JsiAbiRuntime::DataPointerValue implementation
//===========================================================================

JsiAbiRuntime::DataPointerValue::DataPointerValue(winrt::weak_ref<JsiRuntime> &&weakRuntime, uint64_t data) noexcept
    : m_data{data}, m_weakRuntime{std::move(weakRuntime)} {}

JsiAbiRuntime::DataPointerValue::DataPointerValue(uint64_t data) noexcept : m_data{data} {}

void JsiAbiRuntime::DataPointerValue::invalidate() {}

//===========================================================================
// JsiAbiRuntime::SymbolPointerValue implementation
//===========================================================================

JsiAbiRuntime::SymbolPointerValue::SymbolPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiSymbolData &&symbol) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(symbol.Data, 0)} {}

void JsiAbiRuntime::SymbolPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseSymbol({m_data});
    }
  }
}

/*static*/ JsiSymbolData const &JsiAbiRuntime::SymbolPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiSymbolData const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiSymbolData JsiAbiRuntime::SymbolPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::StringPointerValue implementation
//===========================================================================

JsiAbiRuntime::StringPointerValue::StringPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiStringData &&str) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(str.Data, 0)} {}

void JsiAbiRuntime::StringPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseString({m_data});
    }
  }

  delete this;
}

/*static*/ JsiStringData const &JsiAbiRuntime::StringPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiStringData const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiStringData JsiAbiRuntime::StringPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::ObjectPointerValue implementation
//===========================================================================

JsiAbiRuntime::ObjectPointerValue::ObjectPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiObjectData &&obj) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(obj.Data, 0)} {}

void JsiAbiRuntime::ObjectPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleaseObject({m_data});
    }
  }

  delete this;
}

/*static*/ JsiObjectData const &JsiAbiRuntime::ObjectPointerValue::GetData(PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiObjectData const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiObjectData JsiAbiRuntime::ObjectPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::PropNameIDPointerValue implementation
//===========================================================================

JsiAbiRuntime::PropNameIDPointerValue::PropNameIDPointerValue(
    winrt::weak_ref<JsiRuntime> &&weakRuntime,
    JsiPropertyNameIdData &&propertyId) noexcept
    : DataPointerValue{std::move(weakRuntime), std::exchange(propertyId.Data, 0)} {}

void JsiAbiRuntime::PropNameIDPointerValue::invalidate() {
  if (m_data) {
    if (auto runtime = m_weakRuntime.get()) {
      m_weakRuntime = nullptr;
      runtime.ReleasePropertyNameId({m_data});
    }
  }

  delete this;
}

/*static*/ JsiPropertyNameIdData const &JsiAbiRuntime::PropNameIDPointerValue::GetData(
    PointerValue const *pv) noexcept {
  return *reinterpret_cast<JsiPropertyNameIdData const *>(&static_cast<DataPointerValue const *>(pv)->m_data);
}

/*static*/ JsiPropertyNameIdData JsiAbiRuntime::PropNameIDPointerValue::Detach(PointerValue const *pv) noexcept {
  return {std::exchange(static_cast<DataPointerValue *>(const_cast<PointerValue *>(pv))->m_data, 0)};
}

//===========================================================================
// JsiAbiRuntime::ValueRef implementation
//===========================================================================

JsiAbiRuntime::ValueRef::ValueRef(JsiValueData const &data) noexcept {
  InitValueRef(data, &m_value, std::addressof(m_pointerStore));
}

JsiAbiRuntime::ValueRef::~ValueRef() noexcept {}

JsiAbiRuntime::ValueRef::operator Value const &() const noexcept {
  return m_value;
}

/*static*/ void
JsiAbiRuntime::ValueRef::InitValueRef(JsiValueData const &data, Value *value, StoreType *store) noexcept {
  // We assume that the JsiValueData and Value have the same layout.
  auto valueAsDataPtr = reinterpret_cast<JsiValueData *>(value);
  valueAsDataPtr->Kind = data.Kind;
  switch (valueAsDataPtr->Kind) {
    case JsiValueKind::Symbol:
    case JsiValueKind::String:
    case JsiValueKind::Object:
      // Do in-place initialization in 'store'
      valueAsDataPtr->Data = reinterpret_cast<uint64_t>(new (store) DataPointerValue(data.Data));
      break;
    default:
      valueAsDataPtr->Data = data.Data;
      break;
  }
}

//===========================================================================
// JsiAbiRuntime::ValueRefArray implementation
//===========================================================================

JsiAbiRuntime::ValueRefArray::ValueRefArray(array_view<JsiValueData const> args) noexcept : m_size{args.size()} {
  VerifyElseCrashSz(m_size <= MaxCallArgCount, "Argument count must not exceed the MaxCallArgCount");
  for (uint32_t i = 0; i < args.size(); ++i) {
    ValueRef::InitValueRef(args[i], &m_valueArray[i], std::addressof(m_pointerStoreArray[i]));
  }
}

facebook::jsi::Value const *JsiAbiRuntime::ValueRefArray::Data() const noexcept {
  return m_valueArray.data();
}

size_t JsiAbiRuntime::ValueRefArray::Size() const noexcept {
  return m_size;
}

//===========================================================================
// JsiAbiRuntime::PropNameIDRef implementation
//===========================================================================

JsiAbiRuntime::PropNameIDRef::PropNameIDRef(JsiPropertyNameIdData const &data) noexcept
    : m_propertyId{make<PropNameID>(new (std::addressof(m_pointerStore)) DataPointerValue(data.Data))} {}

JsiAbiRuntime::PropNameIDRef::~PropNameIDRef() noexcept {}

JsiAbiRuntime::PropNameIDRef::operator facebook::jsi::PropNameID const &() const noexcept {
  return m_propertyId;
}

} // namespace winrt::Microsoft::ReactNative
