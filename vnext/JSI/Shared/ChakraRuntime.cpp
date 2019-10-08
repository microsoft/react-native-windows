// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraRuntime.h"
#include "ChakraRuntimeArgs.h"

#include <MemoryTracker.h>
#include <cxxreact/MessageQueueThread.h>

#include <mutex>
#include <sstream>
#include "unicode.h"

#include <jsi/ScriptStore.h>

using namespace facebook::react;

namespace Microsoft::JSI {

/*static*/ std::once_flag ChakraRuntime::s_runtimeVersionInitFlag;
/*static*/ uint64_t ChakraRuntime::s_runtimeVersion = 0;

ChakraRuntime::ChakraRuntime(ChakraRuntimeArgs &&args) noexcept
    : m_args{std::move(args)} {
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (!runtimeArgs().enableJITCompilation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(
        runtimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration |
        JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  if (JsCreateRuntime(runtimeAttributes, nullptr, &m_runtime) != JsNoError) {
    std::terminate();
  }

  setupMemoryTracker();

  // Create an execution context
  JsCreateContext(m_runtime, &m_ctx);
  JsAddRef(m_ctx, nullptr);

  // Note :: We currently assume that the runtime will be created and
  // exclusively used in a single thread.
  JsSetCurrentContext(m_ctx);

  startDebuggingIfNeeded();

  setupNativePromiseContinuation();

  std::call_once(s_runtimeVersionInitFlag, initRuntimeVersion);
}

ChakraRuntime::~ChakraRuntime() noexcept {
  stopDebuggingIfNeeded();

  JsSetCurrentContext(JS_INVALID_REFERENCE);
  JsRelease(m_ctx, nullptr);

  JsSetRuntimeMemoryAllocationCallback(m_runtime, nullptr, nullptr);

  JsDisposeRuntime(m_runtime);
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
          auto memoryTrackerPtr = static_cast<MemoryTracker *>(callbackState);
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
  JsValueRef value;
  JsGetGlobalObject(&value);
  return createObject(value);
}

std::string ChakraRuntime::description() {
  return "ChakraRuntime";
}

bool ChakraRuntime::isInspectable() {
  return false;
}

ChakraRuntime::ChakraPropertyIdValue::~ChakraPropertyIdValue() {
  JsRelease(m_propId, nullptr);
}

void ChakraRuntime::ChakraPropertyIdValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraPropertyIdValue::ChakraPropertyIdValue(
    JsPropertyIdRef propIdRef)
    : m_propId(propIdRef) {
  JsAddRef(propIdRef, nullptr);
}

ChakraRuntime::ChakraStringValue::ChakraStringValue(JsValueRef str)
    : m_str(str) {
  JsAddRef(str, nullptr);
}

void ChakraRuntime::ChakraStringValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraStringValue::~ChakraStringValue() {
  JsRelease(m_str, nullptr);
}

ChakraRuntime::ChakraObjectValue::ChakraObjectValue(JsValueRef obj)
    : m_obj(obj) {
  JsAddRef(m_obj, nullptr);
}

void ChakraRuntime::ChakraObjectValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraObjectValue::~ChakraObjectValue() {
  JsRelease(m_obj, nullptr);
}

ChakraRuntime::ChakraWeakRefValue::ChakraWeakRefValue(JsWeakRef obj)
    : m_obj(obj) {
  JsAddRef(m_obj, nullptr);
}

void ChakraRuntime::ChakraWeakRefValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraWeakRefValue::~ChakraWeakRefValue() {
  JsRelease(m_obj, nullptr);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneString(
    const facebook::jsi::Runtime::PointerValue *pv) {
  if (!pv) {
    return nullptr;
  }
  const ChakraStringValue *string = static_cast<const ChakraStringValue *>(pv);
  return makeStringValue(string->m_str);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneObject(
    const facebook::jsi::Runtime::PointerValue *pv) {
  if (!pv) {
    return nullptr;
  }

  const ChakraObjectValue *object = static_cast<const ChakraObjectValue *>(pv);
  return makeObjectValue(object->m_obj);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::clonePropNameID(
    const facebook::jsi::Runtime::PointerValue *pv) {
  if (!pv) {
    return nullptr;
  }

  const ChakraPropertyIdValue *propId =
      static_cast<const ChakraPropertyIdValue *>(pv);
  return makePropertyIdValue(propId->m_propId);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::cloneSymbol(
    const facebook::jsi::Runtime::PointerValue *) {
  throw facebook::jsi::JSINativeException("Not implemented!");
}

std::string ChakraRuntime::symbolToString(const facebook::jsi::Symbol &) {
  throw facebook::jsi::JSINativeException("Not implemented!");
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromAscii(
    const char *str,
    size_t length) {
  JsValueRef propIdRef = createJSPropertyId(str, length);

  auto res = createPropNameID(propIdRef);
  return res;
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromUtf8(
    const uint8_t *utf8,
    size_t length) {
  JsValueRef prpoIdRef =
      createJSPropertyId(reinterpret_cast<const char *>(utf8), length);

  auto res = createPropNameID(prpoIdRef);
  return res;
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromString(
    const facebook::jsi::String &str) {
  std::string propNameString = JSStringToSTLString(stringRef(str));
  return createPropNameIDFromUtf8(
      reinterpret_cast<const uint8_t *>(propNameString.c_str()),
      propNameString.length());
}

std::string ChakraRuntime::utf8(const facebook::jsi::PropNameID &sym) {
  const wchar_t *name;
  checkException(JsGetPropertyNameFromId(propIdRef(sym), &name));
  return facebook::react::unicode::utf16ToUtf8(name, wcslen(name));
}

bool ChakraRuntime::compare(
    const facebook::jsi::PropNameID &a,
    const facebook::jsi::PropNameID &b) {
  bool result;
  JsEquals(propIdRef(a), propIdRef(b), &result);
  return result;
}

facebook::jsi::String ChakraRuntime::createStringFromAscii(
    const char *str,
    size_t length) {
  // Yes we end up double casting for semantic reasons (UTF8 contains ASCII,
  // not the other way around)
  return this->createStringFromUtf8(
      reinterpret_cast<const uint8_t *>(str), length);
}

facebook::jsi::String ChakraRuntime::createStringFromUtf8(
    const uint8_t *str,
    size_t length) {
  JsValueRef stringRef =
      createJSString(reinterpret_cast<const char *>(str), length);
  return createString(stringRef);
}

std::string ChakraRuntime::utf8(const facebook::jsi::String &str) {
  return JSStringToSTLString(stringRef(str));
}

facebook::jsi::Object ChakraRuntime::createObject() {
  return createObject(static_cast<JsValueRef>(nullptr));
}

facebook::jsi::Object ChakraRuntime::createObject(
    std::shared_ptr<facebook::jsi::HostObject> hostObject) {
  facebook::jsi::Object proxyTarget =
      ObjectWithExternalData<HostObjectProxy>::create(
          *this, new HostObjectProxy(*this, hostObject));
  return createProxy(std::move(proxyTarget), createHostObjectProxyHandler());
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::String &name) {
  JsValueRef objRef = objectRef(obj);

  std::wstring propName = JSStringToSTLWString(stringRef(name));
  JsPropertyIdRef propIdRef;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propIdRef));

  JsValueRef value;
  checkException(JsGetProperty(objectRef(obj), propIdRef, &value));
  return createValue(value);
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  JsValueRef objRef = objectRef(obj);
  JsValueRef exc = nullptr;
  JsValueRef res;
  checkException(JsGetProperty(objRef, propIdRef(name), &res));
  return createValue(res);
}

bool ChakraRuntime::hasProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::String &name) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propId, &hasProperty));
  return hasProperty;
}

bool ChakraRuntime::hasProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propIdRef(name), &hasProperty));
  return hasProperty;
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::PropNameID &name,
    const facebook::jsi::Value &value) {
  checkException(
      JsSetProperty(objectRef(object), propIdRef(name), valueRef(value), true));
}

void ChakraRuntime::setPropertyValue(
    facebook::jsi::Object &object,
    const facebook::jsi::String &name,
    const facebook::jsi::Value &value) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  checkException(
      JsSetProperty(objectRef(object), propId, valueRef(value), true));
}

bool ChakraRuntime::isArray(const facebook::jsi::Object &obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsArray;
}

bool ChakraRuntime::isArrayBuffer(const facebook::jsi::Object & /*obj*/) const {
  throw std::runtime_error("Unsupported");
}

uint8_t *ChakraRuntime::data(const facebook::jsi::ArrayBuffer & /*obj*/) {
  throw std::runtime_error("Unsupported");
}

size_t ChakraRuntime::size(const facebook::jsi::ArrayBuffer & /*obj*/) {
  throw std::runtime_error("Unsupported");
}

bool ChakraRuntime::isFunction(const facebook::jsi::Object &obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsFunction;
}

facebook::jsi::Array ChakraRuntime::getPropertyNames(
    const facebook::jsi::Object &obj) {
  JsValueRef propertyNamesArrayRef;
  checkException(JsGetOwnPropertyNames(objectRef(obj), &propertyNamesArrayRef));

  JsPropertyIdRef propertyId;
  checkException(JsGetPropertyIdFromName(L"length", &propertyId));
  JsValueRef countRef;
  checkException(JsGetProperty(propertyNamesArrayRef, propertyId, &countRef));
  int count;
  checkException(JsNumberToInt(countRef, &count));

  auto result = createArray(count);
  for (int i = 0; i < count; i++) {
    JsValueRef index;
    checkException(JsIntToNumber(i, &index));
    JsValueRef propertyName;
    checkException(
        JsGetIndexedProperty(propertyNamesArrayRef, index, &propertyName));
    result.setValueAtIndex(*this, i, createString(propertyName));
  }

  return result;
}

facebook::jsi::WeakObject ChakraRuntime::createWeakObject(
    const facebook::jsi::Object &obj) {
  return make<facebook::jsi::WeakObject>(
      makeWeakRefValue(newWeakObjectRef(obj)));
}

facebook::jsi::Value ChakraRuntime::lockWeakObject(
    const facebook::jsi::WeakObject &weakObj) {
  return createValue(strongObjectRef(weakObj));
}

facebook::jsi::Array ChakraRuntime::createArray(size_t length) {
  JsValueRef result;
  checkException(JsCreateArray(static_cast<unsigned int>(length), &result));
  return createObject(result).getArray(*this);
}

size_t ChakraRuntime::size(const facebook::jsi::Array &arr) {
  std::string lengthStr = "length";

  JsPropertyIdRef propId =
      createJSPropertyId(lengthStr.c_str(), lengthStr.length());

  JsValueRef valueObject;
  checkException(JsGetProperty(objectRef(arr), propId, &valueObject));

  JsValueRef numberValue;
  checkException(JsConvertValueToNumber(valueObject, &numberValue));

  int intValue;
  checkException(JsNumberToInt(numberValue, &intValue));

  return intValue;
}

facebook::jsi::Value ChakraRuntime::getValueAtIndex(
    const facebook::jsi::Array &arr,
    size_t i) {
  JsValueRef index;
  JsIntToNumber(static_cast<int>(i), &index);
  JsValueRef property;
  checkException(JsGetIndexedProperty(objectRef(arr), index, &property));
  return createValue(property);
}

void ChakraRuntime::setValueAtIndexImpl(
    facebook::jsi::Array &arr,
    size_t i,
    const facebook::jsi::Value &value) {
  JsValueRef index;
  JsIntToNumber(static_cast<int>(i), &index);

  checkException(JsSetIndexedProperty(objectRef(arr), index, valueRef(value)));
}

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
      heapArgs[i - 1] =
          hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = heapArgs.get();
  } else {
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      stackArgs[i - 1] =
          hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = stackArgs;
  }
  JsValueRef res{JS_INVALID_REFERENCE};
  facebook::jsi::Value thisVal(
      hostFuncProxy.getRuntime().createObject(argumentsIncThis[0]));
  try {
    facebook::jsi::Value retVal = hostFuncProxy.getHostFunction()(
        hostFuncProxy.getRuntime(), thisVal, args, argumentCount);
    res = hostFuncProxy.getRuntime().valueRef(retVal);
  } catch (const facebook::jsi::JSError &error) {
    JsSetException(hostFuncProxy.getRuntime().valueRef(error.value()));
  } catch (const std::exception &ex) {
    std::string exwhat(ex.what());
    JsValueRef exn;
    exn = createJSString(exwhat.c_str(), exwhat.size());
    JsSetException(exn);
  } catch (...) {
    std::string exceptionString("Exception in HostFunction: <unknown>");
    JsValueRef exn;
    exn = createJSString(exceptionString.c_str(), exceptionString.size());
    JsSetException(exn);
  }

  return res;
}

facebook::jsi::Function ChakraRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name,
    unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  // Currently, we are allocating this proxy object in heap .. and deleting it
  // whenever the JS object is garbage collected.
  HostFunctionProxy *hostFuncProxy = new HostFunctionProxy(func, *this);

  JsValueRef funcRef;
  checkException(JsCreateFunction(
      ChakraRuntime::HostFunctionCall, hostFuncProxy, &funcRef));
  checkException(JsSetObjectBeforeCollectCallback(
      funcRef, hostFuncProxy, [](JsRef ref, void *hostFuncProxy) {
        delete hostFuncProxy;
      }));

  return createObject(funcRef).getFunction(*this);
}

namespace detail {

class ArgsConverterForCall {
 public:
  ArgsConverterForCall(
      ChakraRuntime &rt,
      JsValueRef thisObj,
      const facebook::jsi::Value *args,
      size_t count) {
    JsValueRef *destination = inline_;
    if (count + 1 > maxStackArgs) {
      outOfLine_ = std::make_unique<JsValueRef[]>(count + 1);
      destination = outOfLine_.get();
    }

    if (thisObj == nullptr) {
      JsValueRef undefinedValue;
      JsGetUndefinedValue(&undefinedValue);
      destination[0] = undefinedValue;
    } else {
      destination[0] = thisObj;
    }

    for (size_t i = 0; i < count; ++i) {
      destination[i + 1] = rt.valueRef(args[i]);
    }
  }

  operator JsValueRef *() {
    return outOfLine_ ? outOfLine_.get() : inline_;
  }

 private:
  constexpr static unsigned maxStackArgs = 8;
  JsValueRef inline_[maxStackArgs];
  std::unique_ptr<JsValueRef[]> outOfLine_;
};

} // namespace detail

bool ChakraRuntime::isHostFunction(const facebook::jsi::Function &obj) const {
  throw std::runtime_error("ChakraRuntime::isHostFunction is not implemented.");
}

facebook::jsi::HostFunctionType &ChakraRuntime::getHostFunction(
    const facebook::jsi::Function &obj) {
  throw std::runtime_error(
      "ChakraRuntime::getHostFunction is not implemented.");
}

facebook::jsi::Value ChakraRuntime::call(
    const facebook::jsi::Function &f,
    const facebook::jsi::Value &jsThis,
    const facebook::jsi::Value *args,
    size_t count) {
  JsValueRef result = nullptr;
  checkException(JsCallFunction(
      objectRef(f),
      detail::ArgsConverterForCall(
          *this,
          jsThis.isUndefined() ? nullptr : objectRef(jsThis.getObject(*this)),
          args,
          count),
      static_cast<unsigned short>(count + 1),
      &result));
  return createValue(result);
}

facebook::jsi::Value ChakraRuntime::callAsConstructor(
    const facebook::jsi::Function &f,
    const facebook::jsi::Value *args,
    size_t count) {
  JsValueRef result = nullptr;
  checkException(JsConstructObject(
      objectRef(f),
      detail::ArgsConverterForCall(*this, nullptr, args, count),
      static_cast<unsigned short>(count),
      &result));
  return createValue(result);
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::String &a,
    const facebook::jsi::String &b) const {
  bool result;
  JsStrictEquals(stringRef(a), stringRef(b), &result);
  return result;
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::Object &a,
    const facebook::jsi::Object &b) const {
  bool result;
  JsStrictEquals(objectRef(a), objectRef(b), &result);
  return result;
}

bool ChakraRuntime::strictEquals(
    const facebook::jsi::Symbol &,
    const facebook::jsi::Symbol &) const {
  throw facebook::jsi::JSINativeException("Not implemented!");
}

bool ChakraRuntime::instanceOf(
    const facebook::jsi::Object &o,
    const facebook::jsi::Function &f) {
  bool res;
  checkException(JsInstanceOf(objectRef(o), objectRef(f), &res));
  return res;
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::makeStringValue(
    JsValueRef stringRef) const {
  if (!stringRef) {
    JsValueRef emptyJsValue = createJSString("", 0);
    stringRef = emptyJsValue;
  }
  return new ChakraStringValue(stringRef);
}

facebook::jsi::String ChakraRuntime::createString(JsValueRef str) const {
  return make<facebook::jsi::String>(makeStringValue(str));
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameID(JsValueRef str) {
  return make<facebook::jsi::PropNameID>(makePropertyIdValue(str));
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::makePropertyIdValue(
    JsPropertyIdRef propIdRef) const {
  if (!propIdRef) {
    std::terminate();
  }
  return new ChakraPropertyIdValue(propIdRef);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::makeWeakRefValue(
    JsWeakRef objWeakRef) const {
  if (!objWeakRef)
    std::terminate();
  return new ChakraWeakRefValue(objWeakRef);
}

facebook::jsi::Runtime::PointerValue *ChakraRuntime::makeObjectValue(
    JsValueRef objectRef) const {
  if (!objectRef) {
    JsCreateObject(&objectRef);
  }

  ChakraObjectValue *chakraObjValue = new ChakraObjectValue(objectRef);
  return chakraObjValue;
}

template <class T>
facebook::jsi::Runtime::PointerValue *ChakraRuntime::makeObjectValue(
    JsValueRef objectRef,
    T *externaldata) const {
  if (!externaldata) {
    return makeObjectValue(objectRef);
  }

  // Note :: We explicitly delete the external data proxy when the JS value is
  // finalized. The proxy is expected to do the right thing in destructor, for
  // e.g. decrease the ref count of a shared resource.
  if (!objectRef) {
    JsCreateExternalObject(
        externaldata, [](void *data) { delete data; }, &objectRef);
  } else {
    JsSetExternalData(
        objectRef, externaldata); // TODO : Is there an API to listen to
                                  // finalization of arbitrary objects ?
  }

  ChakraObjectValue *chakraObjValue = new ChakraObjectValue(objectRef);
  return chakraObjValue;
}

template <class T>
/*static */ facebook::jsi::Object ChakraRuntime::ObjectWithExternalData<
    T>::create(ChakraRuntime &rt, T *externalData) {
  return rt.createObject(static_cast<JsValueRef>(nullptr), externalData);
}

template <class T>
/*static */ ChakraRuntime::ObjectWithExternalData<T>
ChakraRuntime::ObjectWithExternalData<T>::fromExisting(
    ChakraRuntime &rt,
    facebook::jsi::Object &&obj) {
  return ObjectWithExternalData<T>(rt.cloneObject(getPointerValue(obj)));
}

template <class T>
T *ChakraRuntime::ObjectWithExternalData<T>::getExternalData() {
  T *externalData;
  JsGetExternalData(
      static_cast<const ChakraObjectValue *>(getPointerValue(*this))->m_obj,
      reinterpret_cast<void **>(&externalData));
  return externalData;
}

template <class T>
facebook::jsi::Object ChakraRuntime::createObject(
    JsValueRef objectRef,
    T *externalData) const {
  return make<facebook::jsi::Object>(makeObjectValue(objectRef, externalData));
}

facebook::jsi::Object ChakraRuntime::createObject(JsValueRef obj) const {
  return make<facebook::jsi::Object>(makeObjectValue(obj));
}

facebook::jsi::Value ChakraRuntime::createValue(JsValueRef value) const {
  JsValueType type;
  JsGetValueType(value, &type);

  switch (type) {
    case JsUndefined:
      return facebook::jsi::Value();

    case JsNull:
      return facebook::jsi::Value(nullptr);

    case JsNumber: {
      double doubleValue;
      JsNumberToDouble(value, &doubleValue);
      facebook::jsi::Value val(doubleValue);
      return val;
    }

    case JsString: {
      std::string utf8str = JSStringToSTLString(value);
      return facebook::jsi::String::createFromUtf8(
          *const_cast<facebook::jsi::Runtime *>(
              reinterpret_cast<const facebook::jsi::Runtime *>(this)),
          reinterpret_cast<const uint8_t *>(utf8str.c_str()),
          utf8str.size());
      break;
    }

    case JsBoolean: {
      bool boolValue;
      JsBooleanToBool(value, &boolValue);
      facebook::jsi::Value val(boolValue);
      return val;
    }

    case JsObject:
    case JsFunction:
    case JsArray: {
      return facebook::jsi::Value(createObject(value));
      break;
    }

    case JsError:
    case JsSymbol:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView:
    default:
      std::terminate();
      break;
  }
}

JsValueRef ChakraRuntime::valueRef(const facebook::jsi::Value &valueIn) {
  if (valueIn.isUndefined()) {
    JsValueRef value;
    JsGetUndefinedValue(&value);
    return value;
  } else if (valueIn.isNull()) {
    JsValueRef value;
    JsGetNullValue(&value);
    return value;
  } else if (valueIn.isBool()) {
    JsValueRef value;
    JsBoolToBoolean(valueIn.getBool(), &value);
    return value;
  } else if (valueIn.isNumber()) {
    JsValueRef value;
    JsDoubleToNumber(valueIn.getNumber(), &value);
    return value;
  } else if (valueIn.isString()) {
    return stringRef(valueIn.getString(*this));
  } else if (valueIn.isObject()) {
    return objectRef(valueIn.getObject(*this));
  } else {
    // What are you?
    abort();
  }
}

JsValueRef ChakraRuntime::stringRef(const facebook::jsi::String &str) {
  return static_cast<const ChakraStringValue *>(getPointerValue(str))->m_str;
}

JsPropertyIdRef ChakraRuntime::propIdRef(const facebook::jsi::PropNameID &sym) {
  return static_cast<const ChakraPropertyIdValue *>(getPointerValue(sym))
      ->m_propId;
}

JsValueRef ChakraRuntime::objectRef(const facebook::jsi::Object &obj) {
  return static_cast<const ChakraObjectValue *>(getPointerValue(obj))->m_obj;
}

JsWeakRef ChakraRuntime::objectRef(const facebook::jsi::WeakObject &obj) {
  return static_cast<const ChakraWeakRefValue *>(getPointerValue(obj))->m_obj;
}

void ChakraRuntime::checkException(JsErrorCode result) {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  checkException(result, nullptr);
}

void ChakraRuntime::checkException(JsErrorCode result, const char *message) {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  std::ostringstream errorStream;

  if (message != nullptr)
    errorStream << message << ". ";

  if (result != JsNoError) {
    errorStream << "ChakraCore API Error :" << std::hex << result << ". ";
  }

  if (hasException || result == JsErrorScriptException) {
    errorStream << "JS exception found: ";

    JsValueRef exn;
    JsGetAndClearException(&exn);

    JsPropertyIdRef messageName;
    JsGetPropertyIdFromName(L"stack", &messageName);

    JsValueRef messageValue;
    if (JsGetProperty(exn, messageName, &messageValue) == JsNoError) {
      if (JsConvertValueToString(exn, &messageValue) != JsNoError) {
        errorStream << "Unable to retrieve JS exception stack";
      } else {
        errorStream << JSStringToSTLString(messageValue);
      }
    } else {
      JsValueRef exnJStr;
      if (JsConvertValueToString(exn, &exnJStr) != JsNoError) {
        errorStream << "Unable to describe JS exception";
      } else {
        errorStream << JSStringToSTLString(exnJStr);
      }
    }
  }

  std::string errorString = errorStream.str();
  throw facebook::jsi::JSError(
      *this, createStringFromAscii(errorString.c_str(), errorString.length()));
}

std::wstring ChakraRuntime::JSStringToSTLWString(JsValueRef str) {
  const wchar_t *value;
  size_t length;

  if (JsNoError != JsStringToPointer(str, &value, &length)) {
    std::terminate();
  }

  // Note: Copying the string out of JsString, as required.
  return std::wstring(value, length);
}

std::string ChakraRuntime::JSStringToSTLString(JsValueRef str) {
  const wchar_t *value;
  size_t length;

  if (JsNoError != JsStringToPointer(str, &value, &length)) {
    std::terminate();
  }

  // Note: This results in multiple buffer copyings. We should look for
  // optimization.
  return facebook::react::unicode::utf16ToUtf8(std::wstring(value, length));
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

bool ChakraRuntime::isHostObject(const facebook::jsi::Object &obj) const {
  facebook::jsi::Value val = obj.getProperty(
      const_cast<ChakraRuntime &>(*this), s_proxyIsHostObjectPropName);
  if (val.isBool())
    return val.getBool();
  else
    return false;
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

facebook::jsi::Runtime::ScopeState *ChakraRuntime::pushScope() {
  return nullptr;
}

void ChakraRuntime::popScope(Runtime::ScopeState *state) {
  assert(state == nullptr);
  checkException(JsCollectGarbage(m_runtime), "JsCollectGarbage");
}

std::unique_ptr<facebook::jsi::Runtime> makeChakraRuntime(
    ChakraRuntimeArgs &&args) noexcept {
  return std::make_unique<ChakraRuntime>(std::move(args));
}

} // namespace Microsoft::JSI
