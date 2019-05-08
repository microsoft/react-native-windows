// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraJsiRuntime.h"
#include "ChakraJsiRuntimeArgs.h"

#include <MemoryTracker.h>
#include <cxxreact/MessageQueueThread.h>

#include <mutex>
#include <sstream>
#include "UnicodeConversion.h"

#include <jsi/ScriptStore.h>

using namespace facebook::react;

namespace facebook { 
namespace jsi {
namespace chakraruntime {

/*static*/ std::once_flag ChakraJsiRuntime::s_runtimeVersionInitFlag;
/*static*/ uint64_t ChakraJsiRuntime::s_runtimeVersion = 0;

ChakraJsiRuntime::ChakraJsiRuntime(ChakraJsiRuntimeArgs&& args)  noexcept
  : m_args { std::move(args) } {
  
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (!runtimeArgs().enableJITCompilation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(runtimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration | JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  if (JsCreateRuntime(runtimeAttributes, nullptr, &m_runtime) != JsNoError) {
    std::terminate();
  }

  setupMemoryTracker();
  setupNativePromiseContinuation();

  // Create an execution context
  JsCreateContext(m_runtime, &m_ctx);
  JsAddRef(m_ctx, nullptr);

  // Note :: We currently assume that the runtime will be created and exclusively used in a single thread.
  JsSetCurrentContext(m_ctx);

  std::call_once(s_runtimeVersionInitFlag, initRuntimeVersion);
}

ChakraJsiRuntime::~ChakraJsiRuntime() noexcept {
  JsSetCurrentContext(JS_INVALID_REFERENCE);
  JsRelease(m_ctx, nullptr);

  JsDisposeRuntime(m_runtime);
}

void ChakraJsiRuntime::setupMemoryTracker() noexcept{

  if (runtimeArgs().memoryTracker) {
    size_t initialMemoryUsage = 0;
    JsGetRuntimeMemoryUsage(m_runtime, &initialMemoryUsage);
    runtimeArgs().memoryTracker->Initialize(initialMemoryUsage);

    if (runtimeArgs().runtimeMemoryLimit > 0)
      JsSetRuntimeMemoryLimit(m_runtime, runtimeArgs().runtimeMemoryLimit);

    JsSetRuntimeMemoryAllocationCallback(m_runtime, runtimeArgs().memoryTracker.get(),
      [](void* callbackState, JsMemoryEventType allocationEvent, size_t allocationSize) -> bool
    {
      auto memoryTrackerPtr = static_cast<MemoryTracker*>(callbackState);
      switch (allocationEvent)
      {
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
    }
    );
  }
}
      
void ChakraJsiRuntime::evaluateJavaScript(
  std::unique_ptr<const jsi::Buffer> buffer,
  const std::string& sourceURL) {

  // Simple evaluate if scriptStore not available as it's risky to utilize the byte codes without checking the script version.
  if (!runtimeArgs().scriptStore) {
    if (!buffer) throw JSINativeException("Script buffer is empty!");
    evaluateJavaScriptSimple(*buffer, sourceURL);
    return;
  }

  uint64_t scriptVersion = 0;
  std::unique_ptr<const jsi::Buffer> scriptBuffer;

  if (buffer) {
    scriptBuffer = std::move(buffer);
    scriptVersion = runtimeArgs().scriptStore->getScriptVersion(sourceURL);
  }
  else {
    auto versionedScript = runtimeArgs().scriptStore->getVersionedScript(sourceURL);
    scriptBuffer = std::move(versionedScript.buffer);
    scriptVersion = versionedScript.version;
  }

  if (!scriptBuffer) {
    throw JSINativeException("Script buffer is empty!");
  }

  // Simple evaluate if script version can't be computed.
  if (scriptVersion == 0) {
    evaluateJavaScriptSimple(*scriptBuffer, sourceURL);
    return;
  }

  auto sharedScriptBuffer = std::shared_ptr<const jsi::Buffer>(std::move(scriptBuffer));

  jsi::ScriptSignature scriptSignature = { sourceURL, scriptVersion };
  jsi::JSRuntimeSignature runtimeSignature = { s_runtimeType, getRuntimeVersion() };

  auto preparedScript = runtimeArgs().preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, nullptr);

  std::shared_ptr<const jsi::Buffer> sharedPreparedScript;
  if (preparedScript) {
    sharedPreparedScript = std::shared_ptr<const jsi::Buffer>(std::move(preparedScript));
  }
  else {
    auto genPreparedScript = generatePreparedScript(sourceURL, *sharedScriptBuffer);
    if (!genPreparedScript) std::terminate(); // Cache generation can't fail unless something really wrong. but we should get rid of this abort before shipping.

    sharedPreparedScript = std::shared_ptr<const jsi::Buffer>(std::move(genPreparedScript));
    runtimeArgs().preparedScriptStore->persistPreparedScript(sharedPreparedScript, scriptSignature, runtimeSignature, nullptr);
  }

  // We are pinning the buffers which are backing the external array buffers to the duration of this.
  // This is not good if the external array buffers have a reduced liftime compared to the runtime itself.
  // But, it's ok for the script and prepared script buffer as their lifetime is expected to be same as the JSI runtime.
  m_pinnedPreparedScripts.push_back(sharedPreparedScript);
  m_pinnedScripts.push_back(sharedScriptBuffer);

  if (evaluateSerializedScript(*sharedScriptBuffer, *sharedPreparedScript, sourceURL)) {
    return;
  }

  // If we reach here, fall back to simple evaluation.
  evaluateJavaScriptSimple(*sharedScriptBuffer, sourceURL);
}

jsi::Object ChakraJsiRuntime::global() {
  JsValueRef value;
  JsGetGlobalObject(&value);
  return createObject(value);
}

std::string ChakraJsiRuntime::description() {
  if (m_desc.empty()) {
    m_desc = std::string("<ChakraJsiRuntime>");
  }
  return m_desc;
}

bool ChakraJsiRuntime::isInspectable() {
  return false;
}

ChakraJsiRuntime::ChakraPropertyIdValue::~ChakraPropertyIdValue() {
  JsRelease(m_propId, nullptr);
}

void ChakraJsiRuntime::ChakraPropertyIdValue::invalidate() {
  delete this;
}

ChakraJsiRuntime::ChakraPropertyIdValue::ChakraPropertyIdValue(JsPropertyIdRef propIdRef)
  : m_propId(propIdRef) {
  JsAddRef(propIdRef, nullptr);
}

ChakraJsiRuntime::ChakraStringValue::ChakraStringValue(JsValueRef str)
  : m_str(str) {
  JsAddRef(str, nullptr);
}

void ChakraJsiRuntime::ChakraStringValue::invalidate() {
  delete this;
}

ChakraJsiRuntime::ChakraStringValue::~ChakraStringValue() {
  JsRelease(m_str, nullptr);
}

ChakraJsiRuntime::ChakraObjectValue::ChakraObjectValue(
  JsValueRef obj
) : m_obj(obj)
{
  JsAddRef(m_obj, nullptr);
}

void ChakraJsiRuntime::ChakraObjectValue::invalidate() {
  delete this;
}

ChakraJsiRuntime::ChakraObjectValue::~ChakraObjectValue() {
  JsRelease(m_obj, nullptr);
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::cloneString(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }
  const ChakraStringValue* string = static_cast<const ChakraStringValue*>(pv);
  return makeStringValue(string->m_str);
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::cloneObject(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }

  const ChakraObjectValue* object = static_cast<const ChakraObjectValue*>(pv);
  return makeObjectValue(object->m_obj);
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::clonePropNameID(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }

  const ChakraPropertyIdValue* propId = static_cast<const ChakraPropertyIdValue*>(pv);
  return makePropertyIdValue(propId->m_propId);
}

jsi::PropNameID ChakraJsiRuntime::createPropNameIDFromAscii(
  const char* str,
  size_t length) {

  JsValueRef propIdRef = createJSPropertyId(str, length);

  auto res = createPropNameID(propIdRef);
  return res;
}

jsi::PropNameID ChakraJsiRuntime::createPropNameIDFromUtf8(
  const uint8_t* utf8,
  size_t length) {

  JsValueRef prpoIdRef = createJSPropertyId(reinterpret_cast<const char*>(utf8), length);

  auto res = createPropNameID(prpoIdRef);
  return res;
}

jsi::PropNameID ChakraJsiRuntime::createPropNameIDFromString(const jsi::String& str) {
  std::string propNameString = JSStringToSTLString(stringRef(str));
  return createPropNameIDFromUtf8(reinterpret_cast<const uint8_t*>(propNameString.c_str()), propNameString.length());
}

std::string ChakraJsiRuntime::utf8(const jsi::PropNameID& sym) {
  const wchar_t* name;
  checkException(JsGetPropertyNameFromId(propIdRef(sym), &name));
  return facebook::react::UnicodeConversion::Utf16ToUtf8(name, wcslen(name));
}

bool ChakraJsiRuntime::compare(const jsi::PropNameID& a, const jsi::PropNameID& b) {
  bool result;
  JsEquals(propIdRef(a), propIdRef(b), &result);
  return result;
}

jsi::String ChakraJsiRuntime::createStringFromAscii(const char* str, size_t length) {
  // Yes we end up double casting for semantic reasons (UTF8 contains ASCII,
  // not the other way around)
  return this->createStringFromUtf8(
    reinterpret_cast<const uint8_t*>(str), length);
}

jsi::String ChakraJsiRuntime::createStringFromUtf8(
  const uint8_t* str,
  size_t length) {

  JsValueRef stringRef = createJSString(reinterpret_cast<const char*>(str), length);
  return createString(stringRef);
}

std::string ChakraJsiRuntime::utf8(const jsi::String& str) {
  return JSStringToSTLString(stringRef(str));
}

jsi::Object ChakraJsiRuntime::createObject() {
  return createObject(static_cast<JsValueRef>(nullptr));
}

jsi::Object ChakraJsiRuntime::createObject(std::shared_ptr<jsi::HostObject> hostObject) {
  throw std::runtime_error("ChakraJsiRuntime::createObject from HostObject is not implemented.");
}

std::shared_ptr<jsi::HostObject> ChakraJsiRuntime::getHostObject(
  const jsi::Object& obj) {
  throw std::runtime_error("ChakraJsiRuntime::createObject is not implemented.");
}

jsi::Value ChakraJsiRuntime::getProperty(
  const jsi::Object& obj,
  const jsi::String& name) {
  JsValueRef objRef = objectRef(obj);

  std::wstring propName = JSStringToSTLWString(stringRef(name));
  JsPropertyIdRef propIdRef;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propIdRef));

  JsValueRef value;
  checkException(JsGetProperty(objectRef(obj), propIdRef, &value));
  return createValue(value);
}

jsi::Value ChakraJsiRuntime::getProperty(
  const jsi::Object& obj,
  const jsi::PropNameID& name) {
  JsValueRef objRef = objectRef(obj);
  JsValueRef exc = nullptr;
  JsValueRef res;
  checkException(JsGetProperty(objRef, propIdRef(name), &res));
  return createValue(res);
}

bool ChakraJsiRuntime::hasProperty(const jsi::Object& obj, const jsi::String& name) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propId, &hasProperty));
  return hasProperty;
}

bool ChakraJsiRuntime::hasProperty(
  const jsi::Object& obj,
  const jsi::PropNameID& name) {
  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propIdRef(name), &hasProperty));
  return hasProperty;
}

void ChakraJsiRuntime::setPropertyValue(
  jsi::Object& object,
  const jsi::PropNameID& name,
  const jsi::Value& value) {
  checkException(JsSetProperty(objectRef(object), propIdRef(name), valueRef(value), true));
}

void ChakraJsiRuntime::setPropertyValue(
  jsi::Object& object,
  const jsi::String& name,
  const jsi::Value& value) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  checkException(JsSetProperty(objectRef(object), propId, valueRef(value), true));
}

bool ChakraJsiRuntime::isArray(const jsi::Object& obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsArray;
}

bool ChakraJsiRuntime::isArrayBuffer(const jsi::Object& /*obj*/) const {
  throw std::runtime_error("Unsupported");
}

uint8_t* ChakraJsiRuntime::data(const jsi::ArrayBuffer& /*obj*/) {
  throw std::runtime_error("Unsupported");
}

size_t ChakraJsiRuntime::size(const jsi::ArrayBuffer& /*obj*/) {
  throw std::runtime_error("Unsupported");
}

bool ChakraJsiRuntime::isFunction(const jsi::Object& obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsFunction;
}

bool ChakraJsiRuntime::isHostObject(const jsi::Object& obj) const {
  throw std::runtime_error("Unsupported");
}

jsi::Array ChakraJsiRuntime::getPropertyNames(const jsi::Object& obj) {

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
    checkException(JsGetIndexedProperty(propertyNamesArrayRef, index, &propertyName));
    result.setValueAtIndex(*this, i, createString(propertyName));
  }

  return result;
}

jsi::WeakObject ChakraJsiRuntime::createWeakObject(const jsi::Object&) {
  throw std::logic_error("Not implemented");
}

jsi::Value ChakraJsiRuntime::lockWeakObject(const jsi::WeakObject&) {
  throw std::logic_error("Not implemented");
}

jsi::Array ChakraJsiRuntime::createArray(size_t length) {
  JsValueRef result;
  checkException(JsCreateArray(static_cast<unsigned int>(length), &result));
  return createObject(result).getArray(*this);
}

size_t ChakraJsiRuntime::size(const jsi::Array& arr) {
  std::string lengthStr = "length";

  JsPropertyIdRef propId = createJSPropertyId(lengthStr.c_str(), lengthStr.length());

  JsValueRef valueObject;
  checkException(JsGetProperty(objectRef(arr), propId, &valueObject));

  JsValueRef numberValue;
  checkException(JsConvertValueToNumber(valueObject, &numberValue));

  int intValue;
  checkException(JsNumberToInt(numberValue, &intValue));

  return intValue;
}

jsi::Value ChakraJsiRuntime::getValueAtIndex(const jsi::Array& arr, size_t i) {
  JsValueRef index;
  JsIntToNumber(static_cast<int>(i), &index);
  JsValueRef property;
  checkException(JsGetIndexedProperty(objectRef(arr), index, &property));
  return createValue(property);
}

void ChakraJsiRuntime::setValueAtIndexImpl(
  jsi::Array& arr,
  size_t i,
  const jsi::Value& value) {
  JsValueRef index;
  JsIntToNumber(static_cast<int>(i), &index);

  checkException(JsSetIndexedProperty(objectRef(arr), index, valueRef(value)));
}

class HostFunctionProxy {
public:
  HostFunctionProxy(jsi::HostFunctionType hostFunction, ChakraJsiRuntime& runtime)
    : m_hostFunction(hostFunction), m_runtime(runtime) {}

  inline const jsi::HostFunctionType& getHostFunction() const{
    return m_hostFunction;
  }

  inline ChakraJsiRuntime& getRuntime() const{
    return m_runtime;
  }

private:
  const jsi::HostFunctionType m_hostFunction;
  ChakraJsiRuntime& m_runtime;
};

JsValueRef CALLBACK ChakraJsiRuntime::HostFunctionCall(JsValueRef callee, bool isConstructCall, JsValueRef *argumentsIncThis, unsigned short argumentCountIncThis, void *callbackState)
{
  const HostFunctionProxy& hostFuncProxy = *reinterpret_cast<HostFunctionProxy*>(callbackState);

  const unsigned maxStackArgCount = 8;
  jsi::Value stackArgs[maxStackArgCount];
  std::unique_ptr<jsi::Value[]> heapArgs;
  jsi::Value* args;

  // Accounting for 'this' object at 0
  unsigned short argumentCount = argumentCountIncThis - 1;

  if (argumentCount > maxStackArgCount) {
    heapArgs = std::make_unique<jsi::Value[]>(argumentCount);
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      heapArgs[i - 1] = hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = heapArgs.get();
  }
  else {
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      stackArgs[i - 1] = hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = stackArgs;
  }
  JsValueRef res { JS_INVALID_REFERENCE };
  jsi::Value thisVal(hostFuncProxy.getRuntime().createObject(argumentsIncThis[0]));
  try {
    jsi::Value retVal = hostFuncProxy.getHostFunction()(hostFuncProxy.getRuntime(), thisVal, args, argumentCount);
    res = hostFuncProxy.getRuntime().valueRef(retVal);
  }
  catch (const jsi::JSError& error) {
    JsSetException(hostFuncProxy.getRuntime().valueRef(error.value()));
  }
  catch (const std::exception& ex) {
    std::string exwhat(ex.what());
    JsValueRef exn;
    exn = createJSString(exwhat.c_str(), exwhat.size());
    JsSetException(exn);
  }
  catch (...) {
    std::string exceptionString("Exception in HostFunction: <unknown>");
    JsValueRef exn;
    exn = createJSString(exceptionString.c_str(), exceptionString.size());
    JsSetException(exn);
  }

  return res;
}

jsi::Function ChakraJsiRuntime::createFunctionFromHostFunction(
  const jsi::PropNameID& name,
  unsigned int paramCount,
  jsi::HostFunctionType func) {

  // Currently, we are allocating this proxy object in heap .. and deleting it whenever the JS object is garbage collected.
  HostFunctionProxy* hostFuncProxy = new HostFunctionProxy(func, *this);

  JsValueRef funcRef;
  checkException(JsCreateFunction(ChakraJsiRuntime::HostFunctionCall, hostFuncProxy, &funcRef));
  checkException(JsSetObjectBeforeCollectCallback(funcRef, hostFuncProxy, [](JsRef ref, void *hostFuncProxy) { delete hostFuncProxy; }));

  return createObject(funcRef).getFunction(*this);
}

namespace detail {

  class ArgsConverterForCall {
  public:
    ArgsConverterForCall(ChakraJsiRuntime& rt, JsValueRef thisObj, const jsi::Value* args, size_t count) {
      JsValueRef* destination = inline_;
      if (count + 1 > maxStackArgs) {
        outOfLine_ = std::make_unique<JsValueRef[]>(count + 1);
        destination = outOfLine_.get();
      }

      if (thisObj == nullptr)
      {
        JsValueRef undefinedValue;
        JsGetUndefinedValue(&undefinedValue);
        destination[0] = undefinedValue;
      }
      else
      {
        destination[0] = thisObj;
      }

      for (size_t i = 0; i < count; ++i) {
        destination[i+1] = rt.valueRef(args[i]);
      }
    }

    operator JsValueRef*() {
      return outOfLine_ ? outOfLine_.get() : inline_;
    }

  private:
    constexpr static unsigned maxStackArgs = 8;
    JsValueRef inline_[maxStackArgs];
    std::unique_ptr<JsValueRef[]> outOfLine_;
  };

} // namespace detail

bool ChakraJsiRuntime::isHostFunction(const jsi::Function& obj) const {
  throw std::runtime_error("ChakraJsiRuntime::isHostFunction is not implemented.");
}

jsi::HostFunctionType& ChakraJsiRuntime::getHostFunction(const jsi::Function& obj) {
  throw std::runtime_error("ChakraJsiRuntime::getHostFunction is not implemented.");
}

jsi::Value ChakraJsiRuntime::call(
  const jsi::Function& f,
  const jsi::Value& jsThis,
  const jsi::Value* args,
  size_t count) {
  JsValueRef result = nullptr;
  checkException(JsCallFunction(
    objectRef(f),
    detail::ArgsConverterForCall(*this, jsThis.isUndefined() ? nullptr : objectRef(jsThis.getObject(*this)), args, count),
    static_cast<unsigned short>(count+1),
    &result));
  return createValue(result);
}

jsi::Value ChakraJsiRuntime::callAsConstructor(
  const jsi::Function& f,
  const jsi::Value* args,
  size_t count) {
  JsValueRef result = nullptr;
  checkException(JsConstructObject(
    objectRef(f),
    detail::ArgsConverterForCall(*this, nullptr, args, count),
    static_cast<unsigned short>(count),
    &result));
  return createValue(result);
}

bool ChakraJsiRuntime::strictEquals(const jsi::String& a, const jsi::String& b)
  const {
  bool result;
  JsStrictEquals(stringRef(a), stringRef(b), &result);
  return result;
}

bool ChakraJsiRuntime::strictEquals(const jsi::Object& a, const jsi::Object& b)
  const {
  bool result;
  JsStrictEquals(objectRef(a), objectRef(b), &result);
  return result;
}

bool ChakraJsiRuntime::instanceOf(const jsi::Object& o, const jsi::Function& f) {
  bool res;
  checkException(JsInstanceOf(objectRef(o), objectRef(f), &res));
  return res;
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::makeStringValue(
  JsValueRef stringRef) const {
  if (!stringRef) {
    JsValueRef emptyJsValue = createJSString("", 0);
    stringRef = emptyJsValue;
  }
  return new ChakraStringValue(stringRef);
}

jsi::String ChakraJsiRuntime::createString(JsValueRef str) const {
  return make<jsi::String>(makeStringValue(str));
}

jsi::PropNameID ChakraJsiRuntime::createPropNameID(JsValueRef str) {
  return make<jsi::PropNameID>(makePropertyIdValue(str));
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::makePropertyIdValue(
  JsPropertyIdRef propIdRef) const {
  if (!propIdRef) {
    std::terminate();
  }
  return new ChakraPropertyIdValue(propIdRef);
}

jsi::Runtime::PointerValue* ChakraJsiRuntime::makeObjectValue(
  JsValueRef objectRef) const {
  if (!objectRef) {
    JsCreateObject(&objectRef);
  }
  ChakraObjectValue* chakraObjValue = new ChakraObjectValue(objectRef);
  return chakraObjValue;
}

jsi::Object ChakraJsiRuntime::createObject(JsValueRef obj) const {
  return make<jsi::Object>(makeObjectValue(obj));
}

jsi::Value ChakraJsiRuntime::createValue(JsValueRef value) const {
  JsValueType type;
  JsGetValueType(value, &type);

  switch (type)
  {
  case JsUndefined:
    return jsi::Value();

  case JsNull:
    return jsi::Value(nullptr);

  case JsNumber:
  {
    double doubleValue;
    JsNumberToDouble(value, &doubleValue);
    jsi::Value val(doubleValue);
    return val;
  }

  case JsString:
  {
    std::string utf8str = JSStringToSTLString(value);
    return jsi::String::createFromUtf8(*const_cast<jsi::Runtime*>(reinterpret_cast<const jsi::Runtime*>(this)), reinterpret_cast<const uint8_t*>(utf8str.c_str()), utf8str.size());
    break;
  }

  case JsBoolean:
  {
    bool boolValue;
    JsBooleanToBool(value, &boolValue);
    jsi::Value val(boolValue);
    return val;
  }

  case JsObject:
  case JsFunction:
  case JsArray:
  {
    return jsi::Value(createObject(value));
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

JsValueRef ChakraJsiRuntime::valueRef(const jsi::Value& valueIn){
  if (valueIn.isUndefined()) {
    JsValueRef value;
    JsGetUndefinedValue(&value);
    return value;
  }
  else if (valueIn.isNull()) {
    JsValueRef value;
    JsGetNullValue(&value);
    return value;
  }
  else if (valueIn.isBool()) {
    JsValueRef value;
    JsBoolToBoolean(valueIn.getBool(), &value);
    return value;
  }
  else if (valueIn.isNumber()) {
    JsValueRef value;
    JsDoubleToNumber(valueIn.getNumber(), &value);
    return value;
  }
  else if (valueIn.isString()) {
    return stringRef(valueIn.getString(*this));
  }
  else if (valueIn.isObject()) {
    return objectRef(valueIn.getObject(*this));
  }
  else {
    // What are you?
    abort();
  }
}

JsValueRef ChakraJsiRuntime::stringRef(const jsi::String& str) {
  return static_cast<const ChakraStringValue*>(getPointerValue(str))->m_str;
}

JsPropertyIdRef ChakraJsiRuntime::propIdRef(const jsi::PropNameID& sym) {
  return static_cast<const ChakraPropertyIdValue*>(getPointerValue(sym))->m_propId;
}

JsValueRef ChakraJsiRuntime::objectRef(const jsi::Object& obj) {
  return static_cast<const ChakraObjectValue*>(getPointerValue(obj))->m_obj;
}

void ChakraJsiRuntime::checkException(JsErrorCode result)  {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  checkException(result, nullptr);
}

void ChakraJsiRuntime::checkException(JsErrorCode result, const char* message) {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  std::ostringstream errorStream;

  if (message != nullptr)
    errorStream << message << ". ";

  if (result != JsNoError) {
    errorStream << "ChakraCore API Error :" << std::hex << result << ". ";
  }

  if (hasException || result == JsErrorScriptException)
  {
    errorStream << "JS exception found: ";

    JsValueRef exn;
    JsGetAndClearException(&exn);

    JsPropertyIdRef messageName;
    JsGetPropertyIdFromName(L"stack", &messageName);

    JsValueRef messageValue;
    if (JsGetProperty(exn, messageName, &messageValue) == JsNoError)
    {
      if (JsConvertValueToString(exn, &messageValue) != JsNoError) {
        errorStream << "Unable to retrieve JS exception stack";
      }
      else {
        errorStream << JSStringToSTLString(messageValue);
      }
    }
    else
    {
      JsValueRef exnJStr;
      if (JsConvertValueToString(exn, &exnJStr) != JsNoError) {
        errorStream << "Unable to describe JS exception";
      }
      else {
        errorStream << JSStringToSTLString(exnJStr);
      }
    }
  }

  std::string errorString = errorStream.str();
  throw jsi::JSError(*this, createStringFromAscii(errorString.c_str(), errorString.length()));
}

std::wstring ChakraJsiRuntime::JSStringToSTLWString(JsValueRef str) {
  const wchar_t *value;
  size_t length;

  if (JsNoError != JsStringToPointer(str, &value, &length)) {
    std::terminate();
  }

  // Note: Copying the string out of JsString, as required.
  return std::wstring(value, length);
}

std::string ChakraJsiRuntime::JSStringToSTLString(JsValueRef str) {
  const wchar_t *value;
  size_t length;

  if (JsNoError != JsStringToPointer(str, &value, &length)) {
    std::terminate();
  }

  // Note: This results in multiple buffer copyings. We should look for optimization.
  return facebook::react::UnicodeConversion::Utf16ToUtf8(std::wstring(value, length));
}


}}} // facebook::jsi::chakraruntime
