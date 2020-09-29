// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiApi.h"
#include "JsiPreparedJavaScript.g.cpp"
#include "JsiRuntime.g.cpp"
#include <Threading/MessageDispatchQueue.h>
#include <crash/verifyElseCrash.h>
#include "ReactHost/MsoUtils.h"

namespace winrt::Microsoft::ReactNative::implementation {

struct JsiPreparedJavaScript : JsiPreparedJavaScriptT<JsiPreparedJavaScript> {
  JsiPreparedJavaScript() = default;
  JsiPreparedJavaScript(std::shared_ptr<facebook::jsi::PreparedJavaScript const> &&js) noexcept : m_js{std::move(js)} {}

  std::shared_ptr<facebook::jsi::PreparedJavaScript const> const &Get() const noexcept {
    return m_js;
  }

 private:
  std::shared_ptr<facebook::jsi::PreparedJavaScript const> m_js;
};

// Wraps up the IJsiHostObject
struct HostObjectWrapper : facebook::jsi::HostObject {
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

template <class TDataHolder>
static facebook::jsi::Runtime::PointerValue const *AsPointerValue(TDataHolder const &dataHolder) noexcept {
  return reinterpret_cast<facebook::jsi::Runtime::PointerValue const *>(dataHolder.Data);
}

static facebook::jsi::PropNameID const &AsPropNameID(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::PropNameID const *>(ptr);
}

static facebook::jsi::Symbol const &AsSymbol(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::Symbol const *>(ptr);
}

static facebook::jsi::String const &AsString(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::String const *>(ptr);
}

static facebook::jsi::Object const &AsObject(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::Object const *>(ptr);
}

static facebook::jsi::Function const &AsFunction(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::Function const *>(ptr);
}

static facebook::jsi::WeakObject &AsWeakObject(facebook::jsi::Runtime::PointerValue **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::WeakObject *>(ptr);
}

static facebook::jsi::Array const &AsArray(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::Array const *>(ptr);
}

static facebook::jsi::ArrayBuffer const &AsArrayBuffer(facebook::jsi::Runtime::PointerValue const **ptr) noexcept {
  return *reinterpret_cast<facebook::jsi::ArrayBuffer const *>(ptr);
}

static facebook::jsi::Value const *AsValue(JsiValueRef const &data) noexcept {
  return reinterpret_cast<facebook::jsi::Value const *>(&data);
}

static JsiValueRef ToJsiValueData(facebook::jsi::Value const &value) noexcept {
  return {static_cast<JsiValueKind>(static_cast<int32_t>(value.kind_)),
          *reinterpret_cast<uint64_t const *>(&value.data_.number)};
}

static JsiPropertyIdRef ToJsiPropertyNameIdData(facebook::jsi::PropNameID const &propNameId) noexcept {
  return {reinterpret_cast<uint64_t>(propNameId.ptr_)};
}

static JsiSymbolRef MakeJsiSymbolData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiStringRef MakeJsiStringData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiObjectRef MakeJsiObjectData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiPropertyIdRef MakeJsiPropertyNameIdData(facebook::jsi::Runtime::PointerValue *pointerValue) {
  return {reinterpret_cast<uint64_t>(pointerValue)};
}

static JsiSymbolRef MakeJsiSymbolData(facebook::jsi::Symbol &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiStringRef MakeJsiStringData(facebook::jsi::String &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiObjectRef MakeJsiObjectData(facebook::jsi::Object &&obj) {
  return {reinterpret_cast<uint64_t>(std::exchange(obj.ptr_, nullptr))};
}

static JsiPropertyIdRef MakeJsiPropertyNameIdData(facebook::jsi::PropNameID &&propertyNameId) {
  return {reinterpret_cast<uint64_t>(std::exchange(propertyNameId.ptr_, nullptr))};
}

static JsiObjectRef MakeJsiArrayData(facebook::jsi::Array &&arr) {
  return {reinterpret_cast<uint64_t>(std::exchange(arr.ptr_, nullptr))};
}

static JsiWeakObjectRef MakeJsiWeakObjectData(facebook::jsi::WeakObject &&weakObj) {
  return {reinterpret_cast<uint64_t>(std::exchange(weakObj.ptr_, nullptr))};
}

static JsiObjectRef MakeJsiFunctionData(facebook::jsi::Function &&func) {
  return {reinterpret_cast<uint64_t>(std::exchange(func.ptr_, nullptr))};
}

static JsiValueRef MakeJsiValueData(facebook::jsi::Value &&value) {
  return {
      static_cast<JsiValueKind>(static_cast<int32_t>(std::exchange(value.kind_, facebook::jsi::Value::UndefinedKind))),
      *reinterpret_cast<uint64_t *>(&value.data_.number)};
}

static std::shared_ptr<facebook::jsi::HostObject> MakeHostObject(
    Microsoft::ReactNative::IJsiHostObject const &hostObject) {
  return std::make_shared<HostObjectWrapper>(hostObject);
}

static JsiValueRef const *AsJsiValueData(facebook::jsi::Value const *value) noexcept {
  return reinterpret_cast<Microsoft::ReactNative::JsiValueRef const *>(value);
}

static facebook::jsi::Value &&ToValue(JsiValueRef &&value) noexcept {
  return reinterpret_cast<facebook::jsi::Value &&>(value);
}

static facebook::jsi::HostFunctionType MakeHostFunction(Microsoft::ReactNative::JsiHostFunction const &hostFunc) {
  // TODO: implement host function mapping
  return [hostFunc](
             facebook::jsi::Runtime &runtime,
             facebook::jsi::Value const &thisVal,
             facebook::jsi::Value const *args,
             size_t count) -> facebook::jsi::Value {
    try {
      ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
      auto argsData = reinterpret_cast<JsiValueRef const *>(args);
      return ToValue(hostFunc(jsiRuntime, ToJsiValueData(thisVal), {argsData, argsData + count}));
    } catch (hresult_error const &) {
      JsiRuntime::RethrowJsiError(runtime);
      throw;
    }
  };
}

//===========================================================================
// HostObjectWrapper implementation
//===========================================================================

HostObjectWrapper::HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept
    : m_hostObject{hostObject} {}

facebook::jsi::Value HostObjectWrapper::get(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name) try {
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  return ToValue(m_hostObject.GetProperty(jsiRuntime, ToJsiPropertyNameIdData(name)));
} catch (hresult_error const &) {
  JsiRuntime::RethrowJsiError(runtime);
  throw;
}

void HostObjectWrapper::set(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name,
    facebook::jsi::Value const &value) try {
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  m_hostObject.SetProperty(jsiRuntime, ToJsiPropertyNameIdData(name), ToJsiValueData(value));
} catch (hresult_error const &) {
  JsiRuntime::RethrowJsiError(runtime);
  throw;
}

std::vector<facebook::jsi::PropNameID> HostObjectWrapper::getPropertyNames(facebook::jsi::Runtime &runtime) try {
  std::vector<facebook::jsi::PropNameID> result;
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  auto names = m_hostObject.GetPropertyIds(jsiRuntime);
  result.reserve(names.Size());
  constexpr uint32_t bufferSize = 200;
  JsiPropertyIdRef nameBuffer[bufferSize];
  for (uint32_t startIndex = 0; startIndex < names.Size(); startIndex += bufferSize) {
    names.GetMany(startIndex, nameBuffer);
    size_t bufferLength = std::min(bufferSize, names.Size() - startIndex);
    for (size_t i = 0; i < bufferLength; ++i) {
      auto ptr = reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(nameBuffer[i].Data);
      result.emplace_back(std::move(*reinterpret_cast<facebook::jsi::PropNameID *>(&ptr)));
    }
  }

  return result;
} catch (hresult_error const &) {
  JsiRuntime::RethrowJsiError(runtime);
  throw;
}

//===========================================================================
// JsiError implementation
//===========================================================================

JsiError::JsiError(facebook::jsi::JSError &&jsError) noexcept
    : m_errorType{JsiErrorType::JSError}, m_jsError{std::move(jsError)} {}

JsiError::JsiError(facebook::jsi::JSINativeException &&nativeException) noexcept
    : m_errorType{JsiErrorType::NativeException}, m_nativeException{std::move(nativeException)} {}

JsiErrorType JsiError::ErrorType() noexcept {
  return m_errorType;
}

hstring JsiError::ErrorDetails() noexcept {
  if (m_errorType == JsiErrorType::JSError) {
    return to_hstring(m_jsError->what());
  } else {
    return to_hstring(m_nativeException->what());
  }
}

hstring JsiError::Message() noexcept {
  if (m_errorType == JsiErrorType::JSError) {
    return to_hstring(m_jsError->getMessage());
  } else {
    return to_hstring("");
  }
}

hstring JsiError::Stack() noexcept {
  if (m_errorType == JsiErrorType::JSError) {
    return to_hstring(m_jsError->getStack());
  } else {
    return to_hstring("");
  }
}

JsiValueRef JsiError::Value() noexcept {
  if (m_errorType == JsiErrorType::JSError) {
    return ToJsiValueData(m_jsError->value());
  } else {
    return {JsiValueKind::Undefined, 0};
  }
}

void JsiError::RethrowError() {
  if (m_errorType == JsiErrorType::JSError) {
    throw *m_jsError;
  } else {
    throw *m_nativeException;
  }
}

//===========================================================================
// JsiRuntime implementation
//===========================================================================

// The macro to simplify recording JSI exceptions.
// It looks strange to keep the normal structure of the try/catch in code.
#define JSI_SET_ERROR                                                \
facebook::jsi::JSError const &jsError) {                             \
    SetError(jsError);                                               \
    throw;                                                           \
  }                                                                  \
  catch (facebook::jsi::JSINativeException const &nativeException) { \
    SetError(nativeException);                                       \
    throw;                                                           \
  } catch (...

/*static*/ std::mutex JsiRuntime::s_mutex;
/*static*/ std::map<uintptr_t, weak_ref<ReactNative::JsiRuntime>> JsiRuntime::s_jsiRuntimeMap;

/*static*/ ReactNative::JsiRuntime JsiRuntime::FromRuntime(facebook::jsi::Runtime &runtime) noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = s_jsiRuntimeMap.find(reinterpret_cast<uintptr_t>(&runtime));
  if (it != s_jsiRuntimeMap.end()) {
    return it->second.get();
  } else {
    return nullptr;
  }
}

ReactNative::JsiRuntime JsiRuntime::MakeChakraRuntime() {
  auto jsDispatchQueue = Mso::DispatchQueue::MakeLooperQueue();
  auto jsThread = std::make_shared<Mso::React::MessageDispatchQueue>(jsDispatchQueue, nullptr, nullptr);
  auto devSettings = std::make_shared<facebook::react::DevSettings>();

  auto runtimeHolder = std::make_shared<::Microsoft::JSI::ChakraRuntimeHolder>(
      std::move(devSettings), std::move(jsThread), nullptr, nullptr);
  auto runtime = runtimeHolder->getRuntime();
  ReactNative::JsiRuntime result{make<JsiRuntime>(std::move(runtimeHolder), runtime)};
  std::scoped_lock lock{s_mutex};
  s_jsiRuntimeMap.try_emplace(reinterpret_cast<uintptr_t>(runtime.get()), result);
  return result;
}

JsiRuntime::JsiRuntime(
    std::shared_ptr<::Microsoft::JSI::ChakraRuntimeHolder> runtimeHolder,
    std::shared_ptr<facebook::jsi::Runtime> runtime) noexcept
    : m_runtimeHolder{std::move(runtimeHolder)}, m_runtime{std::move(runtime)} {}

JsiRuntime::~JsiRuntime() noexcept {
  std::scoped_lock lock{s_mutex};
  s_jsiRuntimeMap.erase(reinterpret_cast<uintptr_t>(m_runtime.get()));
}

JsiValueRef JsiRuntime::EvaluateJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl) try {
  facebook::jsi::Value result;
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    try {
      result = m_runtime->evaluateJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl));
    } catch (JSI_SET_ERROR) {
      throw;
    }
  });
  return MakeJsiValueData(std::move(result));
} catch (JSI_SET_ERROR) {
  throw;
}

ReactNative::JsiPreparedJavaScript JsiRuntime::PrepareJavaScript(
    IJsiByteBuffer const &buffer,
    hstring const &sourceUrl) try {
  ReactNative::JsiPreparedJavaScript result{nullptr};
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    try {
      result = make<JsiPreparedJavaScript>(
          m_runtime->prepareJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl)));
    } catch (JSI_SET_ERROR) {
      throw;
    }
  });
  return result;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js) try {
  return MakeJsiValueData(m_runtime->evaluatePreparedJavaScript(get_self<JsiPreparedJavaScript>(js)->Get()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::Global() try { return MakeJsiObjectData(m_runtime->global()); } catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::Description() try { return to_hstring(m_runtime->description()); } catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsInspectable() try { return m_runtime->isInspectable(); } catch (JSI_SET_ERROR) {
  throw;
}

JsiSymbolRef JsiRuntime::CloneSymbol(JsiSymbolRef symbol) try {
  return MakeJsiSymbolData(m_runtime->cloneSymbol(AsPointerValue(symbol)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CloneString(JsiStringRef str) try {
  return MakeJsiStringData(m_runtime->cloneString(AsPointerValue(str)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CloneObject(JsiObjectRef obj) try {
  return MakeJsiObjectData(m_runtime->cloneObject(AsPointerValue(obj)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::ClonePropertyId(JsiPropertyIdRef propertyNameId) try {
  return MakeJsiPropertyNameIdData(m_runtime->clonePropNameID(AsPointerValue(propertyNameId)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyId(hstring const &name) try {
  std::string utf8Name = to_string(name);
  return MakeJsiPropertyNameIdData(
      m_runtime->createPropNameIDFromUtf8(reinterpret_cast<uint8_t const *>(utf8Name.data()), utf8Name.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromAscii(array_view<uint8_t const> ascii) try {
  return MakeJsiPropertyNameIdData(
      m_runtime->createPropNameIDFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromUtf8(array_view<uint8_t const> utf8) try {
  return MakeJsiPropertyNameIdData(m_runtime->createPropNameIDFromUtf8(utf8.data(), utf8.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromString(JsiStringRef str) try {
  auto strPtr = AsPointerValue(str);
  return MakeJsiPropertyNameIdData(m_runtime->createPropNameIDFromString(AsString(&strPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::PropertyIdToString(JsiPropertyIdRef propertyId) try {
  auto ptr = AsPointerValue(propertyId);
  std::string utf8 = m_runtime->utf8(AsPropNameID(&ptr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::PropertyIdToUtf8(JsiPropertyIdRef propertyNameId, JsiByteArrayUser const &useUtf8String) try {
  auto ptr = AsPointerValue(propertyNameId);
  std::string utf8 = m_runtime->utf8(AsPropNameID(&ptr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::PropertyIdEquals(JsiPropertyIdRef left, JsiPropertyIdRef right) try {
  auto leftPtr = AsPointerValue(left);
  auto rightPtr = AsPointerValue(right);
  return m_runtime->compare(AsPropNameID(&leftPtr), AsPropNameID(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::SymbolToString(JsiSymbolRef symbol) try {
  auto symbolPtr = AsPointerValue(symbol);
  std::string utf8 = m_runtime->symbolToString(AsSymbol(&symbolPtr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SymbolToUtf8(JsiSymbolRef symbol, JsiByteArrayUser const &useUtf8String) try {
  auto symbolPtr = AsPointerValue(symbol);
  std::string utf8 = m_runtime->symbolToString(AsSymbol(&symbolPtr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateString(hstring const &value) try {
  std::string utf8Value = to_string(value);
  return MakeJsiStringData(
      m_runtime->createStringFromUtf8(reinterpret_cast<uint8_t const *>(utf8Value.data()), utf8Value.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateStringFromAscii(array_view<uint8_t const> ascii) try {
  return MakeJsiStringData(m_runtime->createStringFromUtf8(ascii.data(), ascii.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateStringFromUtf8(array_view<uint8_t const> utf8) try {
  return MakeJsiStringData(m_runtime->createStringFromUtf8(utf8.data(), utf8.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::StringToString(JsiStringRef str) try {
  auto strPtr = AsPointerValue(str);
  std::string utf8 = m_runtime->utf8(AsString(&strPtr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::StringToUtf8(JsiStringRef str, JsiByteArrayUser const &useUtf8String) try {
  auto strPtr = AsPointerValue(str);
  std::string utf8 = m_runtime->utf8(AsString(&strPtr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CreateValueFromJson(hstring const &json) try {
  std::string utf8Json = to_string(json);
  return MakeJsiValueData(
      m_runtime->createValueFromJsonUtf8(reinterpret_cast<uint8_t const *>(utf8Json.data()), utf8Json.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CreateValueFromJsonUtf8(array_view<uint8_t const> json) try {
  return MakeJsiValueData(m_runtime->createValueFromJsonUtf8(json.data(), json.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateObject() try {
  return MakeJsiObjectData(m_runtime->createObject());
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateObjectWithHostObject(IJsiHostObject const &hostObject) try {
  return MakeJsiObjectData(m_runtime->createObject(MakeHostObject(hostObject)));
} catch (JSI_SET_ERROR) {
  throw;
}

IJsiHostObject JsiRuntime::GetHostObject(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  auto hostObject = m_runtime->getHostObject(AsObject(&objPtr));
  // TODO: using static_pointer_cast is unsafe here. How to use shared_ptr without RTTI?
  auto wrapper = std::static_pointer_cast<HostObjectWrapper>(hostObject);
  return wrapper ? wrapper->Get() : nullptr;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiHostFunction JsiRuntime::GetHostFunction(JsiObjectRef func) try {
  auto funcPtr = AsPointerValue(func);
  // auto hostFunction = m_runtime->getHostFunction(AsFunction(func));
  // TODO: implement mapping
  return nullptr;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::GetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId) try {
  auto objPtr = AsPointerValue(obj);
  auto propertyIdPtr = AsPointerValue(propertyId);
  return MakeJsiValueData(m_runtime->getProperty(AsObject(&objPtr), AsPropNameID(&propertyIdPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::HasProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId) try {
  auto objPtr = AsPointerValue(obj);
  auto propertyIdPtr = AsPointerValue(propertyId);
  return m_runtime->hasProperty(AsObject(&objPtr), AsPropNameID(&propertyIdPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId, JsiValueRef const &value) try {
  auto objPtr = AsPointerValue(obj);
  auto propertyIdPtr = AsPointerValue(propertyId);
  m_runtime->setPropertyValue(
      const_cast<facebook::jsi::Object &>(AsObject(&objPtr)), AsPropNameID(&propertyIdPtr), *AsValue(value));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::GetPropertyIdArray(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return MakeJsiArrayData(m_runtime->getPropertyNames(AsObject(&objPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsArray(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return m_runtime->isArray(AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsArrayBuffer(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return m_runtime->isArrayBuffer(AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsFunction(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return m_runtime->isFunction(AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsHostObject(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return m_runtime->isHostObject(AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsHostFunction(JsiObjectRef func) try {
  auto funcPtr = AsPointerValue(func);
  return m_runtime->isHostFunction(AsFunction(&funcPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiWeakObjectRef JsiRuntime::CreateWeakObject(JsiObjectRef obj) try {
  auto objPtr = AsPointerValue(obj);
  return MakeJsiWeakObjectData(m_runtime->createWeakObject(AsObject(&objPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::LockWeakObject(JsiWeakObjectRef weakObject) try {
  auto weakObjectPtr = reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(weakObject.Data);
  return MakeJsiValueData(m_runtime->lockWeakObject(AsWeakObject(&weakObjectPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateArray(uint32_t size) try {
  return MakeJsiArrayData(m_runtime->createArray(size));
} catch (JSI_SET_ERROR) {
  throw;
}

uint32_t JsiRuntime::GetArraySize(JsiObjectRef arr) try {
  auto arrPtr = AsPointerValue(arr);
  return static_cast<uint32_t>(m_runtime->size(AsArray(&arrPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

uint32_t JsiRuntime::GetArrayBufferSize(JsiObjectRef arrayBuffer) try {
  auto arrayBufferPtr = AsPointerValue(arrayBuffer);
  return static_cast<uint32_t>(m_runtime->size(AsArrayBuffer(&arrayBufferPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::GetArrayBufferData(JsiObjectRef arrayBuffer, JsiByteArrayUser const &useArrayBytes) try {
  auto arrayBufferPtr = AsPointerValue(arrayBuffer);
  auto data = m_runtime->data(AsArrayBuffer(&arrayBufferPtr));
  return useArrayBytes({data, data + m_runtime->size(AsArrayBuffer(&arrayBufferPtr))});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::GetValueAtIndex(JsiObjectRef arr, uint32_t index) try {
  auto arrPtr = AsPointerValue(arr);
  return MakeJsiValueData(AsArray(&arrPtr).getValueAtIndex(*m_runtime, index));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SetValueAtIndex(JsiObjectRef arr, uint32_t index, JsiValueRef const &value) try {
  auto arrPtr = AsPointerValue(arr);
  m_runtime->setValueAtIndexImpl(const_cast<facebook::jsi::Array &>(AsArray(&arrPtr)), index, *AsValue(value));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateFunctionFromHostFunction(
    JsiPropertyIdRef propNameId,
    uint32_t paramCount,
    JsiHostFunction const &hostFunc) try {
  auto propertyIdPtr = AsPointerValue(propNameId);
  return MakeJsiFunctionData(
      m_runtime->createFunctionFromHostFunction(AsPropNameID(&propertyIdPtr), paramCount, MakeHostFunction(hostFunc)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::Call(JsiObjectRef func, JsiValueRef const &thisArg, array_view<JsiValueRef const> args) try {
  auto funcPtr = AsPointerValue(func);
  return MakeJsiValueData(m_runtime->call(
      AsFunction(&funcPtr),
      *AsValue(thisArg),
      reinterpret_cast<facebook::jsi::Value const *>(args.data()),
      args.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CallAsConstructor(JsiObjectRef func, array_view<JsiValueRef const> args) try {
  auto funcPtr = AsPointerValue(func);
  return MakeJsiValueData(m_runtime->callAsConstructor(
      AsFunction(&funcPtr), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiScopeState JsiRuntime::PushScope() try {
  return {reinterpret_cast<uint64_t>(m_runtime->pushScope())};
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::PopScope(JsiScopeState scopeState) try {
  m_runtime->popScope(reinterpret_cast<facebook::jsi::Runtime::ScopeState *>(scopeState.Data));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::SymbolStrictEquals(JsiSymbolRef left, JsiSymbolRef right) try {
  auto leftPtr = AsPointerValue(left);
  auto rightPtr = AsPointerValue(right);
  return m_runtime->strictEquals(AsSymbol(&leftPtr), AsSymbol(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::StringStrictEquals(JsiStringRef left, JsiStringRef right) try {
  auto leftPtr = AsPointerValue(left);
  auto rightPtr = AsPointerValue(right);
  return m_runtime->strictEquals(AsString(&leftPtr), AsString(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::ObjectStrictEquals(JsiObjectRef left, JsiObjectRef right) try {
  auto leftPtr = AsPointerValue(left);
  auto rightPtr = AsPointerValue(right);
  return m_runtime->strictEquals(AsObject(&leftPtr), AsObject(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::InstanceOf(JsiObjectRef obj, JsiObjectRef constructor) try {
  auto objPtr = AsPointerValue(obj);
  auto ctorPtr = AsPointerValue(constructor);
  return m_runtime->instanceOf(AsObject(&objPtr), AsFunction(&ctorPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::ReleaseSymbol(JsiSymbolRef const &symbolData) {
  facebook::jsi::Symbol symbol{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(symbolData.Data)};
}

void JsiRuntime::ReleaseString(JsiStringRef const &stringData) {
  facebook::jsi::String str{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(stringData.Data)};
}

void JsiRuntime::ReleaseObject(JsiObjectRef const &objectData) {
  facebook::jsi::Object obj{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(objectData.Data)};
}

void JsiRuntime::ReleasePropertyId(JsiPropertyIdRef const &propertyNameIdData) {
  facebook::jsi::PropNameID prop{reinterpret_cast<facebook::jsi::Runtime::PointerValue *>(propertyNameIdData.Data)};
}

ReactNative::JsiError JsiRuntime::GetAndClearError() noexcept {
  std::scoped_lock lock{m_mutex};
  ReactNative::JsiError result{std::move(m_error)};
  return result;
}

void JsiRuntime::SetError(JsiErrorType errorType, hstring const &errorDetails, JsiValueRef const &value) noexcept {
  std::scoped_lock lock{m_mutex};
  if (errorType == JsiErrorType::JSError) {
    m_error = make<JsiError>(
        facebook::jsi::JSError{to_string(errorDetails), *m_runtime, facebook::jsi::Value{*m_runtime, *AsValue(value)}});
  } else {
    m_error = make<JsiError>(facebook::jsi::JSINativeException{to_string(errorDetails)});
  }
}

void JsiRuntime::SetError(facebook::jsi::JSError const &jsError) noexcept {
  std::scoped_lock lock{m_mutex};
  m_error = make<JsiError>(Mso::Copy(jsError));
}

void JsiRuntime::SetError(facebook::jsi::JSINativeException const &nativeException) noexcept {
  std::scoped_lock lock{m_mutex};
  m_error = make<JsiError>(Mso::Copy(nativeException));
}

/*static*/ void JsiRuntime::RethrowJsiError(facebook::jsi::Runtime &runtime) {
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  if (auto abiError = jsiRuntime.GetAndClearError()) {
    get_self<JsiError>(abiError)->RethrowError();
  }
}

} // namespace winrt::Microsoft::ReactNative::implementation
