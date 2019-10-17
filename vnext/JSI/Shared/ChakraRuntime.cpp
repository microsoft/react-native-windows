// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraRuntime.h"

#include "Unicode.h"
#include "Utilities.h"

#include <MemoryTracker.h>
#include <cxxreact/MessageQueueThread.h>
#include <jsi/ScriptStore.h>

#include <climits>
#include <mutex>
#include <sstream>

namespace Microsoft::JSI {

namespace {

class HostFunctionProxy {
 public:
  HostFunctionProxy(
      facebook::jsi::HostFunctionType hostFunction,
      ChakraRuntime &runtime)
      : m_hostFunction(hostFunction), m_runtime(runtime) {}

  inline const facebook::jsi::HostFunctionType &getHostFunction() const {
    return m_hostFunction;
  }

  inline ChakraRuntime &getRuntime() const {
    return m_runtime;
  }

 private:
  const facebook::jsi::HostFunctionType m_hostFunction;
  ChakraRuntime &m_runtime;
};

// Callers of this functions must make sure that jsThis and args are alive when
// using the return value of this function.
std::vector<JsValueRef> constructJsFunctionArguments(
    const ChakraObjectRef &jsThis,
    const std::vector<ChakraObjectRef> &args) {
  std::vector<JsValueRef> result;
  result.push_back(JsRef(jsThis));
  for (const ChakraObjectRef &ref : args) {
    result.push_back(JsRef(ref));
  }
  return result;
}

} // namespace

ChakraRuntime::ChakraRuntime(ChakraRuntimeArgs &&args) noexcept
    : m_args{std::move(args)} {
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (!m_args.enableJITCompilation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(
        runtimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration |
        JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  ThrowUponChakraError(
      JsCreateRuntime(runtimeAttributes, nullptr, &m_runtime),
      "JsCreateRuntime");

  setupMemoryTracker();

  JsContextRef context = nullptr;
  ThrowUponChakraError(JsCreateContext(m_runtime, &context), "JsCreateContext");
  m_context.Initialize(context);

  // Note :: We currently assume that the runtime will be created and
  // exclusively used in a single thread.
  ThrowUponChakraError(JsSetCurrentContext(m_context), "JsSetCurrentContext");

  startDebuggingIfNeeded();

  setupNativePromiseContinuation();

  std::call_once(s_runtimeVersionInitFlag, initRuntimeVersion);
}

ChakraRuntime::~ChakraRuntime() noexcept {
  stopDebuggingIfNeeded();

  ThrowUponChakraError(
      JsSetCurrentContext(JS_INVALID_REFERENCE), "JsSetCurrentContext");
  m_context.Invalidate();

  JsSetRuntimeMemoryAllocationCallback(m_runtime, nullptr, nullptr);

  ThrowUponChakraError(JsDisposeRuntime(m_runtime), "JsDisposeRuntime");
}

#pragma region Functions_inherited_from_Runtime

facebook::jsi::Value ChakraRuntime::evaluateJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceURL) {
  // Simple evaluate if scriptStore not available as it's risky to utilize the
  // byte codes without checking the script version.
  if (!runtimeArgs().scriptStore) {
    if (!buffer)
      throw facebook::jsi::JSINativeException("Script buffer is empty!");
    evaluateJavaScriptSimple(*buffer, sourceURL);
    return facebook::jsi::Value::undefined();
  }

  uint64_t scriptVersion = 0;
  std::shared_ptr<const facebook::jsi::Buffer> scriptBuffer;

  if (buffer) {
    scriptBuffer = std::move(buffer);
    scriptVersion = runtimeArgs().scriptStore->getScriptVersion(sourceURL);
  } else {
    auto versionedScript =
        runtimeArgs().scriptStore->getVersionedScript(sourceURL);
    scriptBuffer = std::move(versionedScript.buffer);
    scriptVersion = versionedScript.version;
  }

  if (!scriptBuffer) {
    throw facebook::jsi::JSINativeException("Script buffer is empty!");
  }

  // Simple evaluate if script version can't be computed.
  if (scriptVersion == 0) {
    evaluateJavaScriptSimple(*scriptBuffer, sourceURL);
    return facebook::jsi::Value::undefined();
  }

  auto sharedScriptBuffer =
      std::shared_ptr<const facebook::jsi::Buffer>(std::move(scriptBuffer));

  facebook::jsi::ScriptSignature scriptSignature = {sourceURL, scriptVersion};
  facebook::jsi::JSRuntimeSignature runtimeSignature = {description().c_str(),
                                                        getRuntimeVersion()};

  auto preparedScript = runtimeArgs().preparedScriptStore->tryGetPreparedScript(
      scriptSignature, runtimeSignature, nullptr);

  std::shared_ptr<const facebook::jsi::Buffer> sharedPreparedScript;
  if (preparedScript) {
    sharedPreparedScript =
        std::shared_ptr<const facebook::jsi::Buffer>(std::move(preparedScript));
  } else {
    auto genPreparedScript =
        generatePreparedScript(sourceURL, *sharedScriptBuffer);
    if (!genPreparedScript)
      std::terminate(); // Cache generation can't fail unless something really
                        // wrong. but we should get rid of this abort before
                        // shipping.

    sharedPreparedScript = std::shared_ptr<const facebook::jsi::Buffer>(
        std::move(genPreparedScript));
    runtimeArgs().preparedScriptStore->persistPreparedScript(
        sharedPreparedScript, scriptSignature, runtimeSignature, nullptr);
  }

  // We are pinning the buffers which are backing the external array buffers to
  // the duration of this. This is not good if the external array buffers have a
  // reduced liftime compared to the runtime itself. But, it's ok for the script
  // and prepared script buffer as their lifetime is expected to be same as the
  // JSI runtime.
  m_pinnedPreparedScripts.push_back(sharedPreparedScript);
  m_pinnedScripts.push_back(sharedScriptBuffer);

  if (evaluateSerializedScript(
          *sharedScriptBuffer, *sharedPreparedScript, sourceURL)) {
    return facebook::jsi::Value::undefined();
  }

  // If we reach here, fall back to simple evaluation.
  return evaluateJavaScriptSimple(*sharedScriptBuffer, sourceURL);
}

std::shared_ptr<const facebook::jsi::PreparedJavaScript>
ChakraRuntime::prepareJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &,
    std::string) {
  throw facebook::jsi::JSINativeException("Not implemented!");
}

facebook::jsi::Value ChakraRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &) {
  throw facebook::jsi::JSINativeException("Not implemented!");
}

facebook::jsi::Object ChakraRuntime::global() {
  JsValueRef global;
  ThrowUponJsError(JsGetGlobalObject(&global), "JsGetGlobalObject");
  return MakePointer<facebook::jsi::Object>(global);
}

std::string ChakraRuntime::description() {
  return "ChakraRuntime";
}

bool ChakraRuntime::isInspectable() {
  return false;
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneSymbol(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneString(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneObject(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::clonePropNameID(
    const facebook::jsi::Runtime::PointerValue *pointerValue) {
  return CloneChakraPointerValue(pointerValue);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromAscii(
    const char *str,
    size_t length) {
  // Unfortunately due to the typing used by JSI and Chakra(Core), we have to do
  // a double reinterpret cast here.
  return createPropNameIDFromUtf8(
      Common::Utilities::CheckedReinterpretCast<const uint8_t *>(str), length);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromUtf8(
    const uint8_t *utf8,
    size_t length) {
  ChakraObjectRef propId = GetPropertyId(
      Common::Utilities::CheckedReinterpretCast<const char *>(utf8), length);
  return MakePointer<facebook::jsi::PropNameID>(std::move(propId));
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromString(
    const facebook::jsi::String &str) {
  // We don not use the functions:
  //   std::string ChakraRuntime::utf8(const String& str), and
  //   std::string ToStdString(const ChakraObjectRef &jsString)
  // here to avoud excessive Unicode conversions.
  std::wstring propName = ToStdWstring(GetChakraObjectRef(str));
  return MakePointer<facebook::jsi::PropNameID>(GetPropertyId(propName));
}

std::string ChakraRuntime::utf8(const facebook::jsi::PropNameID &id) {
  return Common::Unicode::Utf16ToUtf8(GetPropertyName(GetChakraObjectRef(id)));
}

bool ChakraRuntime::compare(
    const facebook::jsi::PropNameID &lhs,
    const facebook::jsi::PropNameID &rhs) {
  return CompareJsPropertyIds(GetChakraObjectRef(lhs), GetChakraObjectRef(rhs));
}

std::string ChakraRuntime::symbolToString(const facebook::jsi::Symbol &s) {
  return ToStdString(ToJsString(GetChakraObjectRef(s)));
}

facebook::jsi::String ChakraRuntime::createStringFromAscii(
    const char *str,
    size_t length) {
  // Unfortunately due to the typing used by JSI and Chakra, we have to do a
  // double reinterpret cast here.
  return createStringFromUtf8(
      Common::Utilities::CheckedReinterpretCast<const uint8_t *>(str), length);
}

facebook::jsi::String ChakraRuntime::createStringFromUtf8(
    const uint8_t *str,
    size_t length) {
  return MakePointer<facebook::jsi::String>(ToJsString(
      Common::Utilities::CheckedReinterpretCast<const char *>(str), length));
}

std::string ChakraRuntime::utf8(const facebook::jsi::String &str) {
  return ToStdString(GetChakraObjectRef(str));
}

facebook::jsi::Object ChakraRuntime::createObject() {
  JsValueRef obj;
  ThrowUponJsError(JsCreateObject(&obj), "JsCreateObject");
  return MakePointer<facebook::jsi::Object>(obj);
}

facebook::jsi::Object ChakraRuntime::createObject(
    std::shared_ptr<facebook::jsi::HostObject> hostObject) {
  facebook::jsi::Object proxyTarget =
      ObjectWithExternalData<HostObjectProxy>::create(
          *this, std::make_unique<HostObjectProxy>(*this, hostObject));
  return createProxy(std::move(proxyTarget), createHostObjectProxyHandler());
}

std::shared_ptr<facebook::jsi::HostObject> ChakraRuntime::getHostObject(
    const facebook::jsi::Object &obj) {
  if (!isHostObject(obj))
    return nullptr;

  facebook::jsi::Value value = obj.getProperty(
      const_cast<ChakraRuntime &>(*this), s_proxyGetHostObjectTargetPropName);
  if (!value.isObject())
    std::terminate();
  facebook::jsi::Object valueObj =
      value.getObject(const_cast<ChakraRuntime &>(*this));

  ObjectWithExternalData<HostObjectProxy> extObject =
      ObjectWithExternalData<HostObjectProxy>::fromExisting(
          *this, std::move(valueObj));
  HostObjectProxy *externalData = extObject.getExternalData();

  if (!externalData)
    std::terminate();
  return externalData->getHostObject();
}

facebook::jsi::HostFunctionType &ChakraRuntime::getHostFunction(
    const facebook::jsi::Function &obj) {
  throw std::runtime_error(
      "ChakraRuntime::getHostFunction is not implemented.");
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  JsValueRef result = nullptr;
  ThrowUponJsError(
      JsGetProperty(GetChakraObjectRef(obj), GetChakraObjectRef(name), &result),
      "JsGetProperty");
  return ToJsiValue(ChakraObjectRef(result));
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::String &name) {
  return getProperty(obj, createPropNameIDFromString(name));
}

bool ChakraRuntime::hasProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  bool result = false;
  ThrowUponJsError(
      JsHasProperty(GetChakraObjectRef(obj), GetChakraObjectRef(name), &result),
      "JsHasProperty");
  return result;
}

bool ChakraRuntime::hasProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::String &name) {
  return hasProperty(obj, createPropNameIDFromString(name));
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::PropNameID &name,
    const facebook::jsi::Value &value) {
  // We use strict rules for property assignments here, so any assignment that
  // silently fails in normal code (assignment to a non-writable global or
  // property, assignment to a getter-only property, assignment to a new
  // property on a non-extensible object) will throw.
  ThrowUponJsError(
      JsSetProperty(
          GetChakraObjectRef(object),
          GetChakraObjectRef(name),
          ToChakraObjectRef(value),
          true /* useStrictRules */),
      "JsSetProperty");
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::String &name,
    const facebook::jsi::Value &value) {
  setPropertyValue(object, createPropNameIDFromString(name), value);
}

bool ChakraRuntime::isArray(const facebook::jsi::Object &object) const {
  return GetValueType(GetChakraObjectRef(object)) == JsArray;
}

bool ChakraRuntime::isArrayBuffer(const facebook::jsi::Object &object) const {
  return GetValueType(GetChakraObjectRef(object)) == JsArrayBuffer;
}

bool ChakraRuntime::isFunction(const facebook::jsi::Object &obj) const {
  return GetValueType(GetChakraObjectRef(obj)) == JsFunction;
}

bool ChakraRuntime::isHostObject(const facebook::jsi::Object &obj) const {
  facebook::jsi::Value val = obj.getProperty(
      const_cast<ChakraRuntime &>(*this), s_proxyIsHostObjectPropName);
  if (val.isBool())
    return val.getBool();
  else
    return false;
}

bool ChakraRuntime::isHostFunction(const facebook::jsi::Function &obj) const {
  throw std::runtime_error("ChakraRuntime::isHostFunction is not implemented.");
}

facebook::jsi::Array ChakraRuntime::getPropertyNames(
    const facebook::jsi::Object &object) {
  constexpr const char *const jsGetPropertyNamesSource =
      "(function()\n"
      "{\n"
      "  return function(obj)\n"
      "  {\n"
      "    var propertyNames = []\n"
      "    for (propertyName in obj) \n"
      "    {\n"
      "      propertyNames.push(propertyName)\n"
      "    }\n"
      "    return propertyNames\n"
      "  }\n"
      "})()";

  static facebook::jsi::StringBuffer jsGetPropertyNamesSourceBuffer{
      jsGetPropertyNamesSource};

  facebook::jsi::Function jsGetPropertyNames =
      evaluateJavaScriptSimple(jsGetPropertyNamesSourceBuffer, "")
          .asObject(*this)
          .asFunction(*this);

  facebook::jsi::Value objAsValue(*this, object);
  return call(
             jsGetPropertyNames,
             facebook::jsi::Value::undefined(),
             &objAsValue,
             1)
      .asObject(*this)
      .asArray(*this);
}

// Only ChakraCore supports weak reference semantics, so ChakraRuntime
// WeakObjects are in fact strong references.

facebook::jsi::WeakObject ChakraRuntime::createWeakObject(
    const facebook::jsi::Object &object) {
  return make<facebook::jsi::WeakObject>(
      CloneChakraPointerValue(getPointerValue(object)));
}

facebook::jsi::Value ChakraRuntime::lockWeakObject(
    const facebook::jsi::WeakObject &weakObject) {
  // We need to make a copy of the ChakraObjectRef held within weakObj's
  // member PointerValue for the returned jsi::Value here.
  ChakraObjectRef ref = GetChakraObjectRef(weakObject);
  return ToJsiValue(std::move(ref));
}

facebook::jsi::Array ChakraRuntime::createArray(size_t length) {
  assert(length <= UINT_MAX);
  JsValueRef result = nullptr;

  ThrowUponJsError(
      JsCreateArray(static_cast<unsigned int>(length), &result),
      "JsCreateArray");

  return MakePointer<facebook::jsi::Object>(result).asArray(*this);
}

size_t ChakraRuntime::size(const facebook::jsi::Array &arr) {
  assert(isArray(arr));

  constexpr const uint8_t propName[] = {'l', 'e', 'n', 'g', 't', 'h'};

  facebook::jsi::PropNameID propId = createPropNameIDFromUtf8(
      propName, Common::Utilities::ArraySize(propName));

  return static_cast<size_t>(getProperty(arr, propId).asNumber());
}

uint8_t *ChakraRuntime::data(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));

  uint8_t *buffer = nullptr;
  unsigned int size = 0;

  ThrowUponJsError(
      JsGetArrayBufferStorage(GetChakraObjectRef(arrBuf), &buffer, &size),
      "JsGetArrayBufferStorage");

  return buffer;
}

size_t ChakraRuntime::size(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));

  constexpr const uint8_t propName[] = {
      'b', 'y', 't', 'e', 'l', 'e', 'n', 'g', 't', 'h'};

  facebook::jsi::PropNameID propId = createPropNameIDFromUtf8(
      propName, Common::Utilities::ArraySize(propName));

  return static_cast<size_t>(getProperty(arrBuf, propId).asNumber());
}

facebook::jsi::Value ChakraRuntime::getValueAtIndex(
    const facebook::jsi::Array &arr,
    size_t index) {
  assert(isArray(arr));
  assert(index <= INT_MAX);

  JsValueRef result = nullptr;
  ThrowUponJsError(
      JsGetIndexedProperty(
          GetChakraObjectRef(arr),
          ToJsNumber(static_cast<int>(index)),
          &result),
      "JsGetIndexedProperty");
  return ToJsiValue(ChakraObjectRef(result));
}

void ChakraRuntime::setValueAtIndexImpl(
    facebook::jsi::Array &arr,
    size_t index,
    const facebook::jsi::Value &value) {
  assert(isArray(arr));
  assert(index <= INT_MAX);

  ThrowUponJsError(
      JsSetIndexedProperty(
          GetChakraObjectRef(arr),
          ToJsNumber(static_cast<int>(index)),
          ToChakraObjectRef(value)),
      "JsSetIndexedProperty");
}

facebook::jsi::Function ChakraRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name,
    unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  // Currently, we are allocating this proxy object in heap .. and deleting it
  // whenever the JS object is garbage collected.
  HostFunctionProxy *hostFuncProxy = new HostFunctionProxy(func, *this);

  JsValueRef funcRef;
  ThrowUponJsError(
      JsCreateFunction(
          ChakraRuntime::HostFunctionCall, hostFuncProxy, &funcRef),
      "JsCreateFunction");

  ThrowUponJsError(
      JsSetObjectBeforeCollectCallback(
          funcRef,
          hostFuncProxy,
          [](JsRef ref, void *hostFuncProxy) { delete hostFuncProxy; }),
      "JsSetObjectBeforeCollectCallback");

  return MakePointer<facebook::jsi::Object>(funcRef).getFunction(*this);
}

facebook::jsi::Value ChakraRuntime::call(
    const facebook::jsi::Function &func,
    const facebook::jsi::Value &jsThis,
    const facebook::jsi::Value *args,
    size_t count) {
  // We must store these ChakraObjectRefs on the stack to make sure that they do
  // not go out of scope when JsCallFunction is called.
  ChakraObjectRef thisRef = ToChakraObjectRef(jsThis);
  std::vector<ChakraObjectRef> argRefs = ToChakraObjectRefs(args, count);

  std::vector<JsValueRef> argsWithThis =
      constructJsFunctionArguments(thisRef, argRefs);
  assert(argsWithThis.size() <= USHRT_MAX);

  JsValueRef result;
  ThrowUponJsError(
      JsCallFunction(
          GetChakraObjectRef(func),
          argsWithThis.data(),
          static_cast<unsigned short>(argsWithThis.size()),
          &result),
      "JsCallFunction");
  return ToJsiValue(ChakraObjectRef(result));
}

facebook::jsi::Value ChakraRuntime::callAsConstructor(
    const facebook::jsi::Function &func,
    const facebook::jsi::Value *args,
    size_t count) {
  // We must store these ChakraObjectRefs on the stack to make sure that they do
  // not go out of scope when JsConstructObject is called.
  ChakraObjectRef undefinedRef = ToChakraObjectRef(facebook::jsi::Value());
  std::vector<ChakraObjectRef> argRefs = ToChakraObjectRefs(args, count);

  std::vector<JsValueRef> argsWithThis =
      constructJsFunctionArguments(undefinedRef, argRefs);
  assert(argsWithThis.size() <= USHRT_MAX);

  JsValueRef result;
  ThrowUponJsError(
      JsConstructObject(
          GetChakraObjectRef(func),
          argsWithThis.data(),
          static_cast<unsigned short>(argsWithThis.size()),
          &result),
      "JsConstructObject");
  return ToJsiValue(ChakraObjectRef(result));
}

facebook::jsi::Runtime::ScopeState *ChakraRuntime::pushScope() {
  return nullptr;
}

void ChakraRuntime::popScope(Runtime::ScopeState *state) {
  assert(state == nullptr);
  ThrowUponJsError(JsCollectGarbage(m_runtime), "JsCollectGarbage");
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::String &a,
    const facebook::jsi::String &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::Object &a,
    const facebook::jsi::Object &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::Symbol &a,
    const facebook::jsi::Symbol &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::instanceOf(
    const facebook::jsi::Object &obj,
    const facebook::jsi::Function &func) {
  bool result;
  ThrowUponJsError(
      JsInstanceOf(GetChakraObjectRef(obj), GetChakraObjectRef(func), &result),
      "JsInstanceOf");
  return result;
}

#pragma endregion Functions_inherited_from_Runtime

facebook::jsi::Value ChakraRuntime::ToJsiValue(ChakraObjectRef &&ref) {
  JsValueType type = GetValueType(ref);

  switch (type) {
    case JsUndefined: {
      return facebook::jsi::Value();
      break;
    }
    case JsNull: {
      return facebook::jsi::Value(nullptr);
      break;
    }
    case JsNumber: {
      double number;
      ThrowUponJsError(JsNumberToDouble(ref, &number), "JsNumberToDouble");
      return facebook::jsi::Value(number);
      break;
    }
    case JsString: {
      return facebook::jsi::Value(
          *this, MakePointer<facebook::jsi::String>(std::move(ref)));
      break;
    }
    case JsBoolean: {
      bool b;
      ThrowUponJsError(JsBooleanToBool(ref, &b), "JsBooleanToBool");
      return facebook::jsi::Value(b);
      break;
    }
    case JsSymbol: {
      return facebook::jsi::Value(
          *this, MakePointer<facebook::jsi::Symbol>(std::move(ref)));
      break;
    }
    case JsObject:
    case JsFunction:
    case JsError:
    case JsArray:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView: {
      return facebook::jsi::Value(
          *this, MakePointer<facebook::jsi::Object>(std::move(ref)));
      break;
    }
    default: {
      // Control flow should never reach here.
      std::terminate();
      break;
    }
  }

  // Control flow should never reach here.
  std::terminate();
}

ChakraObjectRef ChakraRuntime::ToChakraObjectRef(
    const facebook::jsi::Value &value) {
  if (value.isUndefined()) {
    JsValueRef ref;
    ThrowUponJsError(JsGetUndefinedValue(&ref), "JsGetUndefinedValue");
    return ChakraObjectRef(ref);

  } else if (value.isNull()) {
    JsValueRef ref;
    ThrowUponJsError(JsGetNullValue(&ref), "JsGetNullValue");
    return ChakraObjectRef(ref);

  } else if (value.isBool()) {
    JsValueRef ref;
    ThrowUponJsError(JsBoolToBoolean(value.getBool(), &ref), "JsBoolToBoolean");
    return ChakraObjectRef(ref);

  } else if (value.isNumber()) {
    JsValueRef ref;
    ThrowUponJsError(
        JsDoubleToNumber(value.asNumber(), &ref), "JsDoubleToNumber");
    return ChakraObjectRef(ref);

  } else if (value.isSymbol()) {
    return GetChakraObjectRef(value.asSymbol(*this));

  } else if (value.isString()) {
    return GetChakraObjectRef(value.asString(*this));

  } else if (value.isObject()) {
    return GetChakraObjectRef(value.asObject(*this));

  } else {
    // Control flow should never reach here.
    std::terminate();
  }
}

std::vector<ChakraObjectRef> ChakraRuntime::ToChakraObjectRefs(
    const facebook::jsi::Value *value,
    size_t count) {
  std::vector<ChakraObjectRef> result{};

  for (unsigned int i = 0; i < count; ++i) {
    result.emplace_back(ToChakraObjectRef(*value));
    ++value;
  }

  return result;
}

// clang-format off
template <typename T>
/* static */ facebook::jsi::Object
ChakraRuntime::ObjectWithExternalData<T>::create(
    ChakraRuntime &runtime,
    std::unique_ptr<T> &&externalData) {
  return runtime.MakePointer<ObjectWithExternalData<T>>(
      ToJsObject(std::move(externalData)));
}

template <typename T>
/* static */ ChakraRuntime::ObjectWithExternalData<T>
ChakraRuntime::ObjectWithExternalData<T>::fromExisting(
    ChakraRuntime &runtime,
    facebook::jsi::Object &&obj) {
  return ObjectWithExternalData<T>(runtime.cloneObject(getPointerValue(obj)));
}
// clang-format on

template <typename T>
T *ChakraRuntime::ObjectWithExternalData<T>::getExternalData() {
  T *externalData;
  ThrowUponChakraError(
      JsGetExternalData(
          GetChakraObjectRef(*this), reinterpret_cast<void **>(&externalData)),
      "JsGetExternalData");
  return externalData;
}

void ChakraRuntime::ThrowUponJsError(
    JsErrorCode error,
    const char *const chakraApiName) {
  switch (error) {
    case JsNoError: {
      return;
      break;
    }

    case JsErrorScriptException: {
      JsValueRef jsError;
      CrashUponChakraError(JsGetAndClearException(&jsError));
      throw facebook::jsi::JSError(
          "A JavaScript Error was thrown.",
          *this,
          ToJsiValue(ChakraObjectRef(jsError)));
      break;
    }

    default: {
      ThrowUponChakraError(error, chakraApiName);
      break;
    }
  } // switch (error)

  // Control flow should never reach here.
  std::terminate();
}

facebook::jsi::Object ChakraRuntime::createProxy(
    facebook::jsi::Object &&target,
    facebook::jsi::Object &&handler) noexcept {
  // TODO :: Avoid creating the constuctor on each call.
  facebook::jsi::Function proxyConstructor = createProxyConstructor();
  facebook::jsi::Value hostObjectProxy =
      proxyConstructor.call(*this, target, handler);

  if (!hostObjectProxy.isObject())
    std::terminate();

  return hostObjectProxy.getObject(*this);
}

facebook::jsi::Function ChakraRuntime::createProxyConstructor() noexcept {
  auto buffer = std::make_unique<facebook::jsi::StringBuffer>(
      "var ctr=function(target, handler) { return new Proxy(target, handler);};ctr;");
  facebook::jsi::Value hostObjectProxyConstructor =
      evaluateJavaScriptSimple(*buffer, "proxy_constructor.js");

  if (!hostObjectProxyConstructor.isObject() ||
      !hostObjectProxyConstructor.getObject(*this).isFunction(*this))
    std::terminate();

  return hostObjectProxyConstructor.getObject(*this).getFunction(*this);
}

facebook::jsi::Object ChakraRuntime::createHostObjectProxyHandler() noexcept {
  // TODO :: This object can be cached and reused for multiple host objects.

  facebook::jsi::Object handlerObj = createObject();
  std::string getPropName("get"), setPropName("set"),
      enumeratePropName("enumerate");

  handlerObj.setProperty(
      *this,
      getPropName.c_str(),
      createFunctionFromHostFunction(
          createPropNameIDFromAscii(getPropName.c_str(), getPropName.size()),
          2,
          [this](
              Runtime &rt,
              const facebook::jsi::Value &thisVal,
              const facebook::jsi::Value *args,
              size_t count) -> facebook::jsi::Value {
            facebook::jsi::Object targetObj = args[0].getObject(*this);
            facebook::jsi::String propStr = args[1].getString(*this);

            if (propStr.utf8(rt) == s_proxyGetHostObjectTargetPropName) {
              return targetObj;
            }

            if (propStr.utf8(rt) == s_proxyIsHostObjectPropName) {
              return true;
            }

            ObjectWithExternalData<HostObjectProxy> extObject =
                ObjectWithExternalData<HostObjectProxy>::fromExisting(
                    *this, std::move(targetObj));
            HostObjectProxy *externalData = extObject.getExternalData();
            return externalData->Get(
                facebook::jsi::PropNameID::forString(*this, propStr));
          }));

  handlerObj.setProperty(
      *this,
      setPropName.c_str(),
      createFunctionFromHostFunction(
          createPropNameIDFromAscii(setPropName.c_str(), setPropName.size()),
          3,
          [this](
              Runtime &rt,
              const facebook::jsi::Value &thisVal,
              const facebook::jsi::Value *args,
              size_t count) -> facebook::jsi::Value {
            facebook::jsi::Object targetObj = args[0].getObject(*this);
            facebook::jsi::String propStr = args[1].getString(*this);
            const facebook::jsi::Value &propVal = args[2];

            ObjectWithExternalData<HostObjectProxy> extObject =
                ObjectWithExternalData<HostObjectProxy>::fromExisting(
                    *this, std::move(targetObj));
            HostObjectProxy *externalData = extObject.getExternalData();
            externalData->Set(
                facebook::jsi::PropNameID::forString(*this, propStr), propVal);
            return facebook::jsi::Value::undefined();
          }));

  handlerObj.setProperty(
      *this,
      enumeratePropName.c_str(),
      createFunctionFromHostFunction(
          createPropNameIDFromAscii(
              enumeratePropName.c_str(), enumeratePropName.size()),
          1,
          [this](
              Runtime &rt,
              const facebook::jsi::Value &thisVal,
              const facebook::jsi::Value *args,
              size_t count) -> facebook::jsi::Value {
            facebook::jsi::Object targetObj = args[0].getObject(*this);

            ObjectWithExternalData<HostObjectProxy> extObject =
                ObjectWithExternalData<HostObjectProxy>::fromExisting(
                    *this, std::move(targetObj));
            HostObjectProxy *externalData = extObject.getExternalData();
            auto keys = externalData->Enumerator();

            auto result = createArray(keys.size());

            for (size_t i = 0; i < count; i++) {
              std::string keyStr = keys[i].utf8(*this);
              result.setValueAtIndex(
                  *this,
                  i,
                  facebook::jsi::String::createFromUtf8(*this, keyStr));
            }

            return result;
          }));

  return handlerObj;
}

void ChakraRuntime::setupMemoryTracker() noexcept {
  if (runtimeArgs().memoryTracker) {
    size_t initialMemoryUsage = 0;
    JsGetRuntimeMemoryUsage(m_runtime, &initialMemoryUsage);
    runtimeArgs().memoryTracker->Initialize(initialMemoryUsage);

    if (runtimeArgs().runtimeMemoryLimit > 0)
      JsSetRuntimeMemoryLimit(m_runtime, runtimeArgs().runtimeMemoryLimit);

    JsSetRuntimeMemoryAllocationCallback(
        m_runtime,
        runtimeArgs().memoryTracker.get(),
        [](void *callbackState,
           JsMemoryEventType allocationEvent,
           size_t allocationSize) -> bool {
          auto memoryTrackerPtr =
              static_cast<facebook::react::MemoryTracker *>(callbackState);
          switch (allocationEvent) {
            case JsMemoryAllocate:
              memoryTrackerPtr->OnAllocation(allocationSize);
              break;

            case JsMemoryFree:
              memoryTrackerPtr->OnDeallocation(allocationSize);
              break;

            case JsMemoryFailure:
            default:
              break;
          }

          return true;
        });
  }
}

JsValueRef CALLBACK ChakraRuntime::HostFunctionCall(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef *argumentsIncThis,
    unsigned short argumentCountIncThis,
    void *callbackState) {
  const HostFunctionProxy &hostFuncProxy =
      *reinterpret_cast<HostFunctionProxy *>(callbackState);

  const unsigned maxStackArgCount = 8;
  facebook::jsi::Value stackArgs[maxStackArgCount];
  std::unique_ptr<facebook::jsi::Value[]> heapArgs;
  facebook::jsi::Value *args;

  // Accounting for 'this' object at 0
  unsigned short argumentCount = argumentCountIncThis - 1;

  if (argumentCount > maxStackArgCount) {
    heapArgs = std::make_unique<facebook::jsi::Value[]>(argumentCount);

    for (size_t i = 1; i < argumentCountIncThis; i++) {
      heapArgs[i - 1] = hostFuncProxy.getRuntime().ToJsiValue(
          ChakraObjectRef(argumentsIncThis[i]));
    }
    args = heapArgs.get();

  } else {
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      stackArgs[i - 1] = hostFuncProxy.getRuntime().ToJsiValue(
          ChakraObjectRef(argumentsIncThis[i]));
    }
    args = stackArgs;
  }

  JsValueRef res{JS_INVALID_REFERENCE};
  facebook::jsi::Value thisVal(
      hostFuncProxy.getRuntime().MakePointer<facebook::jsi::Object>(
          argumentsIncThis[0]));

  try {
    facebook::jsi::Value retVal = hostFuncProxy.getHostFunction()(
        hostFuncProxy.getRuntime(), thisVal, args, argumentCount);
    res = hostFuncProxy.getRuntime().ToChakraObjectRef(retVal);

  } catch (const facebook::jsi::JSError &error) {
    JsSetException(hostFuncProxy.getRuntime().ToChakraObjectRef(error.value()));

  } catch (const std::exception &ex) {
    std::string exwhat(ex.what());
    JsValueRef exn;
    exn = ToJsString(exwhat.c_str(), exwhat.size());
    JsSetException(exn);

  } catch (...) {
    std::string exceptionString("Exception in HostFunction: <unknown>");
    JsValueRef exn;
    exn = ToJsString(exceptionString.c_str(), exceptionString.size());
    JsSetException(exn);
  }

  return res;
}

/*static*/ std::once_flag ChakraRuntime::s_runtimeVersionInitFlag;
/*static*/ uint64_t ChakraRuntime::s_runtimeVersion = 0;

std::unique_ptr<facebook::jsi::Runtime> makeChakraRuntime(
    ChakraRuntimeArgs &&args) noexcept {
  return std::make_unique<ChakraRuntime>(std::move(args));
}

} // namespace Microsoft::JSI
