// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"
#include "JsiApi.h"
#include "JsiPreparedJavaScript.g.cpp"
#include "JsiRuntime.g.cpp"
#include <Threading/MessageDispatchQueue.h>
#include <crash/verifyElseCrash.h>
#include <winrt/Windows.Foundation.Collections.h>
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

// PointerAccessor allows accessing protected ptr_ value of facebook::jsi::Pointer.
// It contains all functions that use the protected ptr_ value.
struct PointerAccessor : facebook::jsi::Pointer {
  static PointerAccessor const &AsPointerAccessor(facebook::jsi::Pointer const &pointer) noexcept {
    return static_cast<PointerAccessor const &>(pointer);
  }

  static PointerAccessor &&AsPointerAccessor(facebook::jsi::Pointer &&pointer) noexcept {
    return static_cast<PointerAccessor &&>(pointer);
  }

  static JsiPropertyIdRef ToJsiPropertyNameIdData(facebook::jsi::PropNameID const &propNameId) noexcept {
    return {reinterpret_cast<uint64_t>(AsPointerAccessor(propNameId).ptr_)};
  }

  static JsiSymbolRef MakeJsiSymbolData(facebook::jsi::Symbol &&symbol) {
    auto &&accessor = AsPointerAccessor(std::move(symbol));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiStringRef MakeJsiStringData(facebook::jsi::String &&str) {
    auto &&accessor = AsPointerAccessor(std::move(str));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiObjectRef MakeJsiObjectData(facebook::jsi::Object &&obj) {
    auto &&accessor = AsPointerAccessor(std::move(obj));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiPropertyIdRef MakeJsiPropertyNameIdData(facebook::jsi::PropNameID &&propertyNameId) {
    auto &&accessor = AsPointerAccessor(std::move(propertyNameId));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiObjectRef MakeJsiArrayData(facebook::jsi::Array &&arr) {
    auto &&accessor = AsPointerAccessor(std::move(arr));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiWeakObjectRef MakeJsiWeakObjectData(facebook::jsi::WeakObject &&weakObj) {
    auto &&accessor = AsPointerAccessor(std::move(weakObj));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }

  static JsiObjectRef MakeJsiFunctionData(facebook::jsi::Function &&func) {
    auto &&accessor = AsPointerAccessor(std::move(func));
    return {reinterpret_cast<uint64_t>(std::exchange(accessor.ptr_, nullptr))};
  }
};

// ValueAccessor provides access to private fields of facebook::jsi::Value.
// We expect it to have the same layout as facebook::jsi::Value.
// It is ugly, but faster than using the facebook::jsi::Value public API when we
// convert between JsiValueRef and facebook::jsi::Value.
struct ValueAccessor {
  static JsiValueRef ToJsiValueData(facebook::jsi::Value const &value) noexcept {
    ValueAccessor const &accessor = reinterpret_cast<ValueAccessor const &>(value);
    return {
        static_cast<JsiValueKind>(static_cast<int32_t>(accessor.m_kind)),
        *reinterpret_cast<uint64_t const *>(&accessor.m_data)};
  }

  static JsiValueRef MakeJsiValueData(facebook::jsi::Value &&value) {
    ValueAccessor &&accessor = reinterpret_cast<ValueAccessor &&>(value);
    return {
        static_cast<JsiValueKind>(static_cast<int32_t>(std::exchange(accessor.m_kind, UndefinedKind))),
        *reinterpret_cast<uint64_t *>(&accessor.m_data)};
  }

 private:
  enum ValueKind { UndefinedKind } m_kind;
  double m_data;

  static_assert(sizeof(m_data) == sizeof(uint64_t), "Value data should fit in a 64-bit register");
};

static_assert(
    sizeof(ValueAccessor) == sizeof(facebook::jsi::Value),
    "ValueAccessor must have the same size as facebook::jsi::Value");

// RuntimeAccessor is used to access facebook::jsi::Runtime protected members.
struct RuntimeAccessor : facebook::jsi::Runtime {
  using facebook::jsi::Runtime::PointerValue;
  using facebook::jsi::Runtime::ScopeState;

  using facebook::jsi::Runtime::make;

  using facebook::jsi::Runtime::call;
  using facebook::jsi::Runtime::callAsConstructor;
  using facebook::jsi::Runtime::cloneObject;
  using facebook::jsi::Runtime::clonePropNameID;
  using facebook::jsi::Runtime::cloneString;
  using facebook::jsi::Runtime::cloneSymbol;
  using facebook::jsi::Runtime::compare;
  using facebook::jsi::Runtime::createArray;
  using facebook::jsi::Runtime::createFunctionFromHostFunction;
  using facebook::jsi::Runtime::createObject;
  using facebook::jsi::Runtime::createPropNameIDFromAscii;
  using facebook::jsi::Runtime::createPropNameIDFromString;
  using facebook::jsi::Runtime::createPropNameIDFromUtf8;
  using facebook::jsi::Runtime::createStringFromUtf8;
  using facebook::jsi::Runtime::createValueFromJsonUtf8;
  using facebook::jsi::Runtime::createWeakObject;
  using facebook::jsi::Runtime::data;
  using facebook::jsi::Runtime::getHostObject;
  using facebook::jsi::Runtime::getProperty;
  using facebook::jsi::Runtime::getPropertyNames;
  using facebook::jsi::Runtime::hasProperty;
  using facebook::jsi::Runtime::instanceOf;
  using facebook::jsi::Runtime::isArray;
  using facebook::jsi::Runtime::isArrayBuffer;
  using facebook::jsi::Runtime::isFunction;
  using facebook::jsi::Runtime::isHostFunction;
  using facebook::jsi::Runtime::isHostObject;
  using facebook::jsi::Runtime::lockWeakObject;
  using facebook::jsi::Runtime::popScope;
  using facebook::jsi::Runtime::pushScope;
  using facebook::jsi::Runtime::setPropertyValue;
  using facebook::jsi::Runtime::setValueAtIndexImpl;
  using facebook::jsi::Runtime::size;
  using facebook::jsi::Runtime::strictEquals;
  using facebook::jsi::Runtime::symbolToString;
  using facebook::jsi::Runtime::utf8;

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

  static facebook::jsi::HostFunctionType MakeHostFunction(
      Microsoft::ReactNative::JsiHostFunction const &hostFunc,
      std::shared_ptr<JsiRuntime::HostFunctionCleaner> &&cleaner) {
    return [hostFunc, cleaner](
               facebook::jsi::Runtime &runtime,
               facebook::jsi::Value const &thisVal,
               facebook::jsi::Value const *args,
               size_t count) -> facebook::jsi::Value {
      try {
        ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
        auto argsData = reinterpret_cast<JsiValueRef const *>(args);
        return ToValue(hostFunc(jsiRuntime, ValueAccessor::ToJsiValueData(thisVal), {argsData, argsData + count}));
      } catch (hresult_error const &) {
        JsiRuntime::RethrowJsiError(runtime);
        throw;
      }
    };
  }
};

//===========================================================================
// HostObjectWrapper implementation
//===========================================================================

HostObjectWrapper::HostObjectWrapper(Microsoft::ReactNative::IJsiHostObject const &hostObject) noexcept
    : m_hostObject{hostObject} {}

facebook::jsi::Value HostObjectWrapper::get(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name) try {
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  return RuntimeAccessor::ToValue(m_hostObject.GetProperty(jsiRuntime, PointerAccessor::ToJsiPropertyNameIdData(name)));
} catch (hresult_error const &) {
  JsiRuntime::RethrowJsiError(runtime);
  throw;
}

void HostObjectWrapper::set(
    facebook::jsi::Runtime &runtime,
    facebook::jsi::PropNameID const &name,
    facebook::jsi::Value const &value) try {
  ReactNative::JsiRuntime jsiRuntime = JsiRuntime::FromRuntime(runtime);
  m_hostObject.SetProperty(
      jsiRuntime, PointerAccessor::ToJsiPropertyNameIdData(name), ValueAccessor::ToJsiValueData(value));
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
      auto ptr = reinterpret_cast<RuntimeAccessor::PointerValue *>(nameBuffer[i].Data);
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
    return ValueAccessor::ToJsiValueData(m_jsError->value());
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
/*static*/ std::unordered_map<uintptr_t, weak_ref<ReactNative::JsiRuntime>> JsiRuntime::s_jsiRuntimeMap;
/*static*/ std::unordered_map<int64_t, ReactNative::JsiHostFunction> JsiRuntime::s_jsiHostFunctionMap;
/*static*/ int64_t JsiRuntime::s_jsiNextHostFunctionId{0};

/*static*/ ReactNative::JsiRuntime JsiRuntime::FromRuntime(facebook::jsi::Runtime &jsiRuntime) noexcept {
  std::scoped_lock lock{s_mutex};
  auto it = s_jsiRuntimeMap.find(reinterpret_cast<uintptr_t>(&jsiRuntime));
  if (it != s_jsiRuntimeMap.end()) {
    return it->second.get();
  } else {
    return nullptr;
  }
}

/*static*/ ReactNative::JsiRuntime JsiRuntime::GetOrCreate(
    std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> const &jsiRuntimeHolder,
    std::shared_ptr<facebook::jsi::Runtime> const &jsiRuntime) noexcept {
  {
    std::scoped_lock lock{s_mutex};
    auto it = s_jsiRuntimeMap.find(reinterpret_cast<uintptr_t>(jsiRuntime.get()));
    if (it != s_jsiRuntimeMap.end()) {
      return it->second.get();
    }
  }

  return Create(jsiRuntimeHolder, jsiRuntime);
}

/*static*/ ReactNative::JsiRuntime JsiRuntime::Create(
    std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> const &jsiRuntimeHolder,
    std::shared_ptr<facebook::jsi::Runtime> const &jsiRuntime) noexcept {
  // There are some functions that we cannot do using JSI such as
  // defining a property or using Symbol as a key.
  // The __jsi_api defines functions to associate a hostFunctionId with a function object
  // in a way that it is not discoverable by other components.
  // We use a unique symbol and make read-only, non-enumerable, and non-configurable (deletable).
  auto jsiPalBuffer = std::make_shared<facebook::jsi::StringBuffer>(R"JS(
      var __jsi_pal = function() {
        var hostFunctionId = Symbol('hostFunctionId');
        return {
          getHostFunctionId: function(func) { return func[hostFunctionId]; },
          setHostFunctionId: function(func, id) { Object.defineProperty(func, hostFunctionId, { value: id }); }
        };
      }();
    )JS");
  // TODO: consider implementing this script as a resource file and loading it with the resource URL.
  jsiRuntime->evaluateJavaScript(jsiPalBuffer, "Form_JSI_API_not_a_real_file");
  ReactNative::JsiRuntime abiJsiResult{make<JsiRuntime>(Mso::Copy(jsiRuntimeHolder), Mso::Copy(jsiRuntime))};
  std::scoped_lock lock{s_mutex};
  s_jsiRuntimeMap.try_emplace(reinterpret_cast<uintptr_t>(jsiRuntime.get()), abiJsiResult);
  return abiJsiResult;
}

ReactNative::JsiRuntime JsiRuntime::MakeChakraRuntime() {
  auto jsDispatchQueue = Mso::DispatchQueue::MakeLooperQueue();
  auto jsThread = std::make_shared<Mso::React::MessageDispatchQueue>(jsDispatchQueue, nullptr, nullptr);
  auto devSettings = std::make_shared<facebook::react::DevSettings>();

  auto runtimeHolder = std::make_shared<::Microsoft::JSI::ChakraRuntimeHolder>(
      std::move(devSettings), std::move(jsThread), nullptr, nullptr);
  auto runtime = runtimeHolder->getRuntime();
  return Create(runtimeHolder, runtime);
}

JsiRuntime::JsiRuntime(
    std::shared_ptr<facebook::jsi::RuntimeHolderLazyInit> &&runtimeHolder,
    std::shared_ptr<facebook::jsi::Runtime> &&runtime) noexcept
    : m_runtimeHolder{std::move(runtimeHolder)},
      m_runtime{std::move(runtime)},
      m_runtimeAccessor{static_cast<RuntimeAccessor *>(m_runtime.get())} {}

JsiRuntime::~JsiRuntime() noexcept {
  std::scoped_lock lock{s_mutex};
  s_jsiRuntimeMap.erase(reinterpret_cast<uintptr_t>(m_runtime.get()));
}

JsiValueRef JsiRuntime::EvaluateJavaScript(IJsiByteBuffer const &buffer, hstring const &sourceUrl) try {
  facebook::jsi::Value result;
  buffer.GetData([this, &result, &sourceUrl](array_view<uint8_t const> bytes) {
    try {
      result = m_runtimeAccessor->evaluateJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl));
    } catch (JSI_SET_ERROR) {
      throw;
    }
  });
  return ValueAccessor::MakeJsiValueData(std::move(result));
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
          m_runtimeAccessor->prepareJavaScript(std::make_shared<JsiBufferWrapper>(bytes), to_string(sourceUrl)));
    } catch (JSI_SET_ERROR) {
      throw;
    }
  });
  return result;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::EvaluatePreparedJavaScript(ReactNative::JsiPreparedJavaScript const &js) try {
  return ValueAccessor::MakeJsiValueData(
      m_runtimeAccessor->evaluatePreparedJavaScript(get_self<JsiPreparedJavaScript>(js)->Get()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::Global() try {
  return PointerAccessor::MakeJsiObjectData(m_runtimeAccessor->global());
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::Description() try { return to_hstring(m_runtimeAccessor->description()); } catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsInspectable() try { return m_runtimeAccessor->isInspectable(); } catch (JSI_SET_ERROR) {
  throw;
}

JsiSymbolRef JsiRuntime::CloneSymbol(JsiSymbolRef symbol) try {
  return PointerAccessor::MakeJsiSymbolData(m_runtimeAccessor->make<facebook::jsi::Symbol>(
      m_runtimeAccessor->cloneSymbol(RuntimeAccessor::AsPointerValue(symbol))));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CloneString(JsiStringRef str) try {
  return PointerAccessor::MakeJsiStringData(m_runtimeAccessor->make<facebook::jsi::String>(
      m_runtimeAccessor->cloneString(RuntimeAccessor::AsPointerValue(str))));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CloneObject(JsiObjectRef obj) try {
  return PointerAccessor::MakeJsiObjectData(m_runtimeAccessor->make<facebook::jsi::Object>(
      m_runtimeAccessor->cloneObject(RuntimeAccessor::AsPointerValue(obj))));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::ClonePropertyId(JsiPropertyIdRef propertyNameId) try {
  return PointerAccessor::MakeJsiPropertyNameIdData(m_runtimeAccessor->make<facebook::jsi::PropNameID>(
      m_runtimeAccessor->clonePropNameID(RuntimeAccessor::AsPointerValue(propertyNameId))));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyId(hstring const &name) try {
  std::string utf8Name = to_string(name);
  return PointerAccessor::MakeJsiPropertyNameIdData(
      m_runtimeAccessor->createPropNameIDFromUtf8(reinterpret_cast<uint8_t const *>(utf8Name.data()), utf8Name.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromAscii(array_view<uint8_t const> ascii) try {
  return PointerAccessor::MakeJsiPropertyNameIdData(
      m_runtimeAccessor->createPropNameIDFromAscii(reinterpret_cast<char const *>(ascii.data()), ascii.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromUtf8(array_view<uint8_t const> utf8) try {
  return PointerAccessor::MakeJsiPropertyNameIdData(
      m_runtimeAccessor->createPropNameIDFromUtf8(utf8.data(), utf8.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiPropertyIdRef JsiRuntime::CreatePropertyIdFromString(JsiStringRef str) try {
  auto strPtr = RuntimeAccessor::AsPointerValue(str);
  return PointerAccessor::MakeJsiPropertyNameIdData(
      m_runtimeAccessor->createPropNameIDFromString(RuntimeAccessor::AsString(&strPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::PropertyIdToString(JsiPropertyIdRef propertyId) try {
  auto ptr = RuntimeAccessor::AsPointerValue(propertyId);
  std::string utf8 = m_runtimeAccessor->utf8(RuntimeAccessor::AsPropNameID(&ptr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::PropertyIdToUtf8(JsiPropertyIdRef propertyNameId, JsiByteArrayUser const &useUtf8String) try {
  auto ptr = RuntimeAccessor::AsPointerValue(propertyNameId);
  std::string utf8 = m_runtimeAccessor->utf8(RuntimeAccessor::AsPropNameID(&ptr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::PropertyIdEquals(JsiPropertyIdRef left, JsiPropertyIdRef right) try {
  auto leftPtr = RuntimeAccessor::AsPointerValue(left);
  auto rightPtr = RuntimeAccessor::AsPointerValue(right);
  return m_runtimeAccessor->compare(RuntimeAccessor::AsPropNameID(&leftPtr), RuntimeAccessor::AsPropNameID(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::SymbolToString(JsiSymbolRef symbol) try {
  auto symbolPtr = RuntimeAccessor::AsPointerValue(symbol);
  std::string utf8 = m_runtimeAccessor->symbolToString(RuntimeAccessor::AsSymbol(&symbolPtr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SymbolToUtf8(JsiSymbolRef symbol, JsiByteArrayUser const &useUtf8String) try {
  auto symbolPtr = RuntimeAccessor::AsPointerValue(symbol);
  std::string utf8 = m_runtimeAccessor->symbolToString(RuntimeAccessor::AsSymbol(&symbolPtr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateString(hstring const &value) try {
  std::string utf8Value = to_string(value);
  return PointerAccessor::MakeJsiStringData(
      m_runtimeAccessor->createStringFromUtf8(reinterpret_cast<uint8_t const *>(utf8Value.data()), utf8Value.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateStringFromAscii(array_view<uint8_t const> ascii) try {
  return PointerAccessor::MakeJsiStringData(m_runtimeAccessor->createStringFromUtf8(ascii.data(), ascii.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiStringRef JsiRuntime::CreateStringFromUtf8(array_view<uint8_t const> utf8) try {
  return PointerAccessor::MakeJsiStringData(m_runtimeAccessor->createStringFromUtf8(utf8.data(), utf8.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

hstring JsiRuntime::StringToString(JsiStringRef str) try {
  auto strPtr = RuntimeAccessor::AsPointerValue(str);
  std::string utf8 = m_runtimeAccessor->utf8(RuntimeAccessor::AsString(&strPtr));
  return to_hstring(utf8);
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::StringToUtf8(JsiStringRef str, JsiByteArrayUser const &useUtf8String) try {
  auto strPtr = RuntimeAccessor::AsPointerValue(str);
  std::string utf8 = m_runtimeAccessor->utf8(RuntimeAccessor::AsString(&strPtr));
  uint8_t const *data = reinterpret_cast<uint8_t const *>(utf8.data());
  useUtf8String({data, data + utf8.size()});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CreateValueFromJson(hstring const &json) try {
  std::string utf8Json = to_string(json);
  return ValueAccessor::MakeJsiValueData(
      m_runtimeAccessor->createValueFromJsonUtf8(reinterpret_cast<uint8_t const *>(utf8Json.data()), utf8Json.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CreateValueFromJsonUtf8(array_view<uint8_t const> json) try {
  return ValueAccessor::MakeJsiValueData(m_runtimeAccessor->createValueFromJsonUtf8(json.data(), json.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateObject() try {
  return PointerAccessor::MakeJsiObjectData(m_runtimeAccessor->createObject());
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateObjectWithHostObject(IJsiHostObject const &hostObject) try {
  return PointerAccessor::MakeJsiObjectData(
      m_runtimeAccessor->createObject(RuntimeAccessor::MakeHostObject(hostObject)));
} catch (JSI_SET_ERROR) {
  throw;
}

IJsiHostObject JsiRuntime::GetHostObject(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  auto hostObject = m_runtimeAccessor->getHostObject(RuntimeAccessor::AsObject(&objPtr));
  // TODO: using static_pointer_cast is unsafe here. How to use shared_ptr without RTTI?
  auto wrapper = std::static_pointer_cast<HostObjectWrapper>(hostObject);
  return wrapper ? wrapper->Get() : nullptr;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiHostFunction JsiRuntime::GetHostFunction(JsiObjectRef func) try {
  auto funcPtr = RuntimeAccessor::AsPointerValue(func);
  auto const &jsiFunc = RuntimeAccessor::AsFunction(&funcPtr);
  auto &rt = *m_runtime;
  int64_t hostFunctionId = static_cast<int64_t>(rt.global()
                                                    .getPropertyAsObject(rt, "__jsi_pal")
                                                    .getPropertyAsFunction(rt, "getHostFunctionId")
                                                    .call(rt, jsiFunc)
                                                    .getNumber());
  {
    std::scoped_lock lock{m_mutex};
    auto it = s_jsiHostFunctionMap.find(hostFunctionId);
    if (it != s_jsiHostFunctionMap.end()) {
      return it->second;
    }
  }
  return nullptr;
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::GetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  auto propertyIdPtr = RuntimeAccessor::AsPointerValue(propertyId);
  return ValueAccessor::MakeJsiValueData(m_runtimeAccessor->getProperty(
      RuntimeAccessor::AsObject(&objPtr), RuntimeAccessor::AsPropNameID(&propertyIdPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::HasProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  auto propertyIdPtr = RuntimeAccessor::AsPointerValue(propertyId);
  return m_runtimeAccessor->hasProperty(
      RuntimeAccessor::AsObject(&objPtr), RuntimeAccessor::AsPropNameID(&propertyIdPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SetProperty(JsiObjectRef obj, JsiPropertyIdRef propertyId, JsiValueRef const &value) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  auto propertyIdPtr = RuntimeAccessor::AsPointerValue(propertyId);
  m_runtimeAccessor->setPropertyValue(
      const_cast<facebook::jsi::Object &>(RuntimeAccessor::AsObject(&objPtr)),
      RuntimeAccessor::AsPropNameID(&propertyIdPtr),
      *RuntimeAccessor::AsValue(value));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::GetPropertyIdArray(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return PointerAccessor::MakeJsiArrayData(m_runtimeAccessor->getPropertyNames(RuntimeAccessor::AsObject(&objPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsArray(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return m_runtimeAccessor->isArray(RuntimeAccessor::AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsArrayBuffer(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return m_runtimeAccessor->isArrayBuffer(RuntimeAccessor::AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsFunction(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return m_runtimeAccessor->isFunction(RuntimeAccessor::AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsHostObject(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return m_runtimeAccessor->isHostObject(RuntimeAccessor::AsObject(&objPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::IsHostFunction(JsiObjectRef func) try {
  auto funcPtr = RuntimeAccessor::AsPointerValue(func);
  return m_runtimeAccessor->isHostFunction(RuntimeAccessor::AsFunction(&funcPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiWeakObjectRef JsiRuntime::CreateWeakObject(JsiObjectRef obj) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  return PointerAccessor::MakeJsiWeakObjectData(
      m_runtimeAccessor->createWeakObject(RuntimeAccessor::AsObject(&objPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::LockWeakObject(JsiWeakObjectRef weakObject) try {
  auto weakObjectPtr = const_cast<RuntimeAccessor::PointerValue *>(RuntimeAccessor::AsPointerValue(weakObject));
  return ValueAccessor::MakeJsiValueData(
      m_runtimeAccessor->lockWeakObject(RuntimeAccessor::AsWeakObject(&weakObjectPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiObjectRef JsiRuntime::CreateArray(uint32_t size) try {
  return PointerAccessor::MakeJsiArrayData(m_runtimeAccessor->createArray(size));
} catch (JSI_SET_ERROR) {
  throw;
}

uint32_t JsiRuntime::GetArraySize(JsiObjectRef arr) try {
  auto arrPtr = RuntimeAccessor::AsPointerValue(arr);
  return static_cast<uint32_t>(m_runtimeAccessor->size(RuntimeAccessor::AsArray(&arrPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

uint32_t JsiRuntime::GetArrayBufferSize(JsiObjectRef arrayBuffer) try {
  auto arrayBufferPtr = RuntimeAccessor::AsPointerValue(arrayBuffer);
  return static_cast<uint32_t>(m_runtimeAccessor->size(RuntimeAccessor::AsArrayBuffer(&arrayBufferPtr)));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::GetArrayBufferData(JsiObjectRef arrayBuffer, JsiByteArrayUser const &useArrayBytes) try {
  auto arrayBufferPtr = RuntimeAccessor::AsPointerValue(arrayBuffer);
  auto data = m_runtimeAccessor->data(RuntimeAccessor::AsArrayBuffer(&arrayBufferPtr));
  return useArrayBytes({data, data + m_runtimeAccessor->size(RuntimeAccessor::AsArrayBuffer(&arrayBufferPtr))});
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::GetValueAtIndex(JsiObjectRef arr, uint32_t index) try {
  auto arrPtr = RuntimeAccessor::AsPointerValue(arr);
  return ValueAccessor::MakeJsiValueData(RuntimeAccessor::AsArray(&arrPtr).getValueAtIndex(*m_runtime, index));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::SetValueAtIndex(JsiObjectRef arr, uint32_t index, JsiValueRef const &value) try {
  auto arrPtr = RuntimeAccessor::AsPointerValue(arr);
  m_runtimeAccessor->setValueAtIndexImpl(
      const_cast<facebook::jsi::Array &>(RuntimeAccessor::AsArray(&arrPtr)), index, *RuntimeAccessor::AsValue(value));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiRuntime::HostFunctionCleaner::HostFunctionCleaner(int64_t hostFunctionId) : m_hostFunctionId{hostFunctionId} {}

JsiRuntime::HostFunctionCleaner::~HostFunctionCleaner() {
  std::scoped_lock lock{s_mutex};
  s_jsiHostFunctionMap.erase(m_hostFunctionId);
}

JsiObjectRef JsiRuntime::CreateFunctionFromHostFunction(
    JsiPropertyIdRef propNameId,
    uint32_t paramCount,
    JsiHostFunction const &hostFunc) try {
  int64_t hostFunctionId;
  {
    std::scoped_lock lock{s_mutex};
    hostFunctionId = ++s_jsiNextHostFunctionId;
    s_jsiHostFunctionMap[hostFunctionId] = hostFunc;
  }
  auto cleaner = std::make_shared<HostFunctionCleaner>(hostFunctionId);
  auto &rt = *m_runtimeAccessor;
  auto propertyIdPtr = RuntimeAccessor::AsPointerValue(propNameId);
  auto func = rt.createFunctionFromHostFunction(
      RuntimeAccessor::AsPropNameID(&propertyIdPtr),
      paramCount,
      RuntimeAccessor::MakeHostFunction(hostFunc, std::move(cleaner)));
  rt.global()
      .getPropertyAsObject(rt, "__jsi_pal")
      .getPropertyAsFunction(rt, "setHostFunctionId")
      .call(rt, func, static_cast<double>(hostFunctionId));
  return PointerAccessor::MakeJsiFunctionData(std::move(func));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::Call(JsiObjectRef func, JsiValueRef const &thisArg, array_view<JsiValueRef const> args) try {
  auto funcPtr = RuntimeAccessor::AsPointerValue(func);
  return ValueAccessor::MakeJsiValueData(m_runtimeAccessor->call(
      RuntimeAccessor::AsFunction(&funcPtr),
      *RuntimeAccessor::AsValue(thisArg),
      reinterpret_cast<facebook::jsi::Value const *>(args.data()),
      args.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiValueRef JsiRuntime::CallAsConstructor(JsiObjectRef func, array_view<JsiValueRef const> args) try {
  auto funcPtr = RuntimeAccessor::AsPointerValue(func);
  return ValueAccessor::MakeJsiValueData(m_runtimeAccessor->callAsConstructor(
      RuntimeAccessor::AsFunction(&funcPtr), reinterpret_cast<facebook::jsi::Value const *>(args.data()), args.size()));
} catch (JSI_SET_ERROR) {
  throw;
}

JsiScopeState JsiRuntime::PushScope() try {
  return {reinterpret_cast<uint64_t>(m_runtimeAccessor->pushScope())};
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::PopScope(JsiScopeState scopeState) try {
  m_runtimeAccessor->popScope(reinterpret_cast<RuntimeAccessor::ScopeState *>(scopeState.Data));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::SymbolStrictEquals(JsiSymbolRef left, JsiSymbolRef right) try {
  auto leftPtr = RuntimeAccessor::AsPointerValue(left);
  auto rightPtr = RuntimeAccessor::AsPointerValue(right);
  return m_runtimeAccessor->strictEquals(RuntimeAccessor::AsSymbol(&leftPtr), RuntimeAccessor::AsSymbol(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::StringStrictEquals(JsiStringRef left, JsiStringRef right) try {
  auto leftPtr = RuntimeAccessor::AsPointerValue(left);
  auto rightPtr = RuntimeAccessor::AsPointerValue(right);
  return m_runtimeAccessor->strictEquals(RuntimeAccessor::AsString(&leftPtr), RuntimeAccessor::AsString(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::ObjectStrictEquals(JsiObjectRef left, JsiObjectRef right) try {
  auto leftPtr = RuntimeAccessor::AsPointerValue(left);
  auto rightPtr = RuntimeAccessor::AsPointerValue(right);
  return m_runtimeAccessor->strictEquals(RuntimeAccessor::AsObject(&leftPtr), RuntimeAccessor::AsObject(&rightPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

bool JsiRuntime::InstanceOf(JsiObjectRef obj, JsiObjectRef constructor) try {
  auto objPtr = RuntimeAccessor::AsPointerValue(obj);
  auto ctorPtr = RuntimeAccessor::AsPointerValue(constructor);
  return m_runtimeAccessor->instanceOf(RuntimeAccessor::AsObject(&objPtr), RuntimeAccessor::AsFunction(&ctorPtr));
} catch (JSI_SET_ERROR) {
  throw;
}

void JsiRuntime::ReleaseSymbol(JsiSymbolRef const &symbolData) {
  auto symbol =
      RuntimeAccessor::make<facebook::jsi::Symbol>(reinterpret_cast<RuntimeAccessor::PointerValue *>(symbolData.Data));
}

void JsiRuntime::ReleaseString(JsiStringRef const &stringData) {
  auto str =
      RuntimeAccessor::make<facebook::jsi::String>(reinterpret_cast<RuntimeAccessor::PointerValue *>(stringData.Data));
}

void JsiRuntime::ReleaseObject(JsiObjectRef const &objectData) {
  auto obj =
      RuntimeAccessor::make<facebook::jsi::Object>(reinterpret_cast<RuntimeAccessor::PointerValue *>(objectData.Data));
}

void JsiRuntime::ReleasePropertyId(JsiPropertyIdRef const &propertyNameIdData) {
  auto prop = RuntimeAccessor::make<facebook::jsi::PropNameID>(
      reinterpret_cast<RuntimeAccessor::PointerValue *>(propertyNameIdData.Data));
}

ReactNative::JsiError JsiRuntime::GetAndClearError() noexcept {
  std::scoped_lock lock{m_mutex};
  ReactNative::JsiError result{std::move(m_error)};
  return result;
}

void JsiRuntime::SetError(JsiErrorType errorType, hstring const &errorDetails, JsiValueRef const &value) noexcept {
  std::scoped_lock lock{m_mutex};
  if (errorType == JsiErrorType::JSError) {
    m_error = make<JsiError>(facebook::jsi::JSError{
        to_string(errorDetails), *m_runtime, facebook::jsi::Value{*m_runtime, *RuntimeAccessor::AsValue(value)}});
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
