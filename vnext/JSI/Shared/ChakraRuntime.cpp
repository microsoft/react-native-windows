// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraRuntime.h"

#include "ByteArrayBuffer.h"
#include "ChakraRuntimeArgs.h"
#include "PreparedJavaScriptStore.h"

// TODO (yicyao): MemoryTracker is a defined in facebook::react. We ought to
// refactor and move it into Microsoft::JSI.
#include "MemoryTracker.h"
#include "Unicode.h"
#include "Utilities.h"

#include <cstring>
#include <exception>
#include <limits>
#include <set>
#include <string_view>

namespace Microsoft::JSI {

namespace {

class HostFunctionProxy {
 public:
  HostFunctionProxy(facebook::jsi::HostFunctionType &&hostFunction, ChakraRuntime &runtime)
      : m_hostFunction(std::move(hostFunction)), m_runtime(runtime) {}

  inline const facebook::jsi::HostFunctionType &GetHostFunction() const {
    return m_hostFunction;
  }

  inline ChakraRuntime &GetRuntime() {
    return m_runtime;
  }

 private:
  const facebook::jsi::HostFunctionType m_hostFunction;
  ChakraRuntime &m_runtime;
};

constexpr const char *const g_bootstrapBundleSource =
    "function $$ChakraRuntimeProxyConstructor$$(target, handler)\n"
    "{\n"
    "  return new Proxy(target, handler)\n"
    "}";

constexpr const char *const g_proxyConstructorBootstrapFuncName = "$$ChakraRuntimeProxyConstructor$$";

constexpr const char *const g_proxyGetHostObjectTargetPropName = "$$ProxyGetHostObjectTarget$$";
constexpr const char *const g_proxyIsHostObjectPropName = "$$ProxyIsHostObject$$";

constexpr const char *const g_functionIsHostFunctionPropName = "$$FunctionIsHostFunction$$";

// Callers of this functions must make sure that jsThis and args are alive when
// using the return value of this function.
std::vector<JsValueRef> ConstructJsFunctionArguments(
    const ChakraObjectRef &jsThis,
    const std::vector<ChakraObjectRef> &args) {
  std::vector<JsValueRef> result;
  result.push_back(JsRef(jsThis));
  for (const ChakraObjectRef &ref : args) {
    result.push_back(JsRef(ref));
  }
  return result;
}

bool CALLBACK
MemoryAllocationCallback(void *memoryTrackerAsCookie, JsMemoryEventType allocationEvent, size_t allocationSize) {
  facebook::react::MemoryTracker *memoryTracker = static_cast<facebook::react::MemoryTracker *>(memoryTrackerAsCookie);

  switch (allocationEvent) {
    case JsMemoryAllocate: {
      memoryTracker->OnAllocation(allocationSize);
      break;
    }
    case JsMemoryFree: {
      memoryTracker->OnDeallocation(allocationSize);
      break;
    }
    case JsMemoryFailure:
    default: {
      break;
    }
  }

  return true;
}

} // namespace

ChakraRuntime::ChakraRuntime(const std::shared_ptr<ChakraRuntimeArgs> &args) noexcept : m_args{args} {}

void ChakraRuntime::Initialize() {
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  switch (m_args->m_executablePageAllocationPolicy) {
    case ChakraRuntimeArgs::ExecutablePageAllocationPolicy::DisableAll: {
      runtimeAttributes = JsRuntimeAttributeDisableExecutablePageAllocation;
      break;
    }
    case ChakraRuntimeArgs::ExecutablePageAllocationPolicy::DisableNativeCodeGeneration: {
      runtimeAttributes = JsRuntimeAttributeDisableNativeCodeGeneration;
      break;
    }
    case ChakraRuntimeArgs::ExecutablePageAllocationPolicy::EnableAll: {
      break;
    }
    default: {
      // Control flow should never reach here.
      std::terminate();
    }
  }

  // Note: The second argument to JsCreateRuntime is a JsThreadServiceCallback,
  // which allows the host to specify a background thread service for the
  // created runtime. If specified, then background work items will be passed to
  // the host using the provided JsThreadServiceCallback. We might explore this
  // option in the future.
  VerifyChakraErrorElseThrow(JsCreateRuntime(runtimeAttributes, nullptr, &m_runtime));

  SetUpMemoryTracker();

  JsContextRef context = JS_INVALID_REFERENCE;
  VerifyChakraErrorElseThrow(JsCreateContext(m_runtime, &context));
  // Even though initialize() can throw, we do not expect it to do so here.
  // Hence we can mark this constructor as noexcept.
  m_context.Initialize(context);

  // Note: The documentation for JsSetCurrentContext says that it "sets the
  // current script context on the thread." This suggests that ChakraCore is
  // using thread local storage for context data. This is OK for now since we
  // currently assume that ChakraRuntime will be created and used exclusively in
  // a single thread, but we need to revisit this if our assumption is wrong.
  VerifyChakraErrorElseThrow(JsSetCurrentContext(m_context));

  EvaluateJavaScriptSimple(
      std::make_shared<facebook::jsi::StringBuffer>(g_bootstrapBundleSource), "ChakraRuntime_bootstrap.bundle");

  if (m_args->m_enableDebugging) {
    StartDebugging();
  }

  InitializeAdditionalFeatures();
}

ChakraRuntime::~ChakraRuntime() noexcept {
  VerifyChakraErrorElseCrash(JsSetCurrentContext(JS_INVALID_REFERENCE));

  // Note: We invalidate m_context explicitly here to avoid calling JsRelease
  // after the m_runtime has been disposed. Invalidate() can throw, but since we
  // marked this destructor as noexcept, std::terminate() will be invoked.
  m_context.Invalidate();

  VerifyChakraErrorElseCrash(JsSetRuntimeMemoryAllocationCallback(m_runtime, nullptr, nullptr));

  VerifyChakraErrorElseCrash(JsDisposeRuntime(m_runtime));
}

#pragma region Functions_inherited_from_Runtime

facebook::jsi::Value ChakraRuntime::evaluateJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceUrl) {
  if (!buffer) {
    throw facebook::jsi::JSINativeException("Error in ChakraRuntime::evaluateJavaScript() - buffer cannot be null.");
  }

  // PreparedJavaScript optimization is disabled when debugging.
  if (m_args->m_preparedJsStore && !m_args->m_enableDebugging) {
    std::unique_ptr<PreparedJavaScriptStore> &preparedJsStore = m_args->m_preparedJsStore;
    std::shared_ptr<const facebook::jsi::PreparedJavaScript> preparedJs =
        preparedJsStore->getPreparedJavaScript(sourceUrl);

    if (preparedJs) {
      return evaluatePreparedJavaScript(preparedJs);
    } else {
      preparedJsStore->persistPreparedJavaScript(sourceUrl);
    }
  }

  return EvaluateJavaScriptSimple(buffer, sourceUrl);
}

std::shared_ptr<const facebook::jsi::PreparedJavaScript> ChakraRuntime::prepareJavaScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    std::string sourceUrl) {
  if (!buffer) {
    throw facebook::jsi::JSINativeException("Error in ChakraRuntime::prepareJavaScript() - buffer cannot be null.");
  }

  return std::make_shared<const ChakraPreparedJavaScript>(
      std::string{sourceUrl}, buffer, SerializeScript(buffer));
}

facebook::jsi::Object ChakraRuntime::global() {
  JsValueRef global;
  VerifyJsErrorElseThrow(JsGetGlobalObject(&global));
  return MakePointer<facebook::jsi::Object>(global);
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

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromAscii(const char *str, size_t length) {
  ChakraObjectRef propId = GetPropertyId(std::string_view{str, length});
  return MakePointer<facebook::jsi::PropNameID>(std::move(propId));
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromUtf8(const uint8_t *utf8, size_t length) {
  return createPropNameIDFromAscii(Common::Utilities::CheckedReinterpretCast<const char *>(utf8), length);
}

facebook::jsi::PropNameID ChakraRuntime::createPropNameIDFromString(const facebook::jsi::String &str) {
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

bool ChakraRuntime::compare(const facebook::jsi::PropNameID &lhs, const facebook::jsi::PropNameID &rhs) {
  return CompareJsPropertyIds(GetChakraObjectRef(lhs), GetChakraObjectRef(rhs));
}

std::string ChakraRuntime::symbolToString(const facebook::jsi::Symbol &s) {
  return ToStdString(ToJsString(GetChakraObjectRef(s)));
}

facebook::jsi::String ChakraRuntime::createStringFromAscii(const char *str, size_t length) {
  // Unfortunately due to the typing used by JSI and Chakra, we have to do a
  // double reinterpret cast here.
  return createStringFromUtf8(Common::Utilities::CheckedReinterpretCast<const uint8_t *>(str), length);
}

facebook::jsi::String ChakraRuntime::createStringFromUtf8(const uint8_t *str, size_t length) {
  return MakePointer<facebook::jsi::String>(
      ToJsString(std::string_view{Common::Utilities::CheckedReinterpretCast<const char *>(str), length}));
}

std::string ChakraRuntime::utf8(const facebook::jsi::String &str) {
  return ToStdString(GetChakraObjectRef(str));
}

facebook::jsi::Object ChakraRuntime::createObject() {
  JsValueRef obj;
  VerifyJsErrorElseThrow(JsCreateObject(&obj));
  return MakePointer<facebook::jsi::Object>(obj);
}

facebook::jsi::Object ChakraRuntime::createObject(std::shared_ptr<facebook::jsi::HostObject> hostObject) {
  facebook::jsi::Function jsProxyConstructor =
      global().getPropertyAsFunction(*this, g_proxyConstructorBootstrapFuncName);

  return jsProxyConstructor
      .call(*this, MakePointer<facebook::jsi::Object>(ToJsObject(hostObject)), createHostObjectProxyHandler())
      .asObject(*this);
}

std::shared_ptr<facebook::jsi::HostObject> ChakraRuntime::getHostObject(const facebook::jsi::Object &obj) {
  if (!isHostObject(obj)) {
    throw facebook::jsi::JSINativeException("getHostObject() can only be called with HostObjects.");
  }

  facebook::jsi::Object target = obj.getPropertyAsObject(*this, g_proxyGetHostObjectTargetPropName);

  return GetExternalData<facebook::jsi::HostObject>(GetChakraObjectRef(target));
}

facebook::jsi::HostFunctionType &ChakraRuntime::getHostFunction(const facebook::jsi::Function &obj) {
  throw facebook::jsi::JSINativeException("ChakraRuntime::getHostFunction is not implemented.");
}

facebook::jsi::Value ChakraRuntime::getProperty(
    const facebook::jsi::Object &obj,
    const facebook::jsi::PropNameID &name) {
  return ToJsiValue(GetProperty(GetChakraObjectRef(obj), GetChakraObjectRef(name)));
}

facebook::jsi::Value ChakraRuntime::getProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) {
  return getProperty(obj, createPropNameIDFromString(name));
}

bool ChakraRuntime::hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::PropNameID &name) {
  bool result = false;
  VerifyJsErrorElseThrow(JsHasProperty(GetChakraObjectRef(obj), GetChakraObjectRef(name), &result));
  return result;
}

bool ChakraRuntime::hasProperty(const facebook::jsi::Object &obj, const facebook::jsi::String &name) {
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
  VerifyJsErrorElseThrow(JsSetProperty(
      GetChakraObjectRef(object), GetChakraObjectRef(name), ToChakraObjectRef(value), true /* useStrictRules */));
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
  facebook::jsi::Value val = GetObjectProperty(obj, g_proxyIsHostObjectPropName);

  if (val.isBool()) {
    return val.getBool();
  } else {
    return false;
  }
}

bool ChakraRuntime::isHostFunction(const facebook::jsi::Function &obj) const {
  facebook::jsi::Value val = GetObjectProperty(obj, g_functionIsHostFunctionPropName);

  if (val.isBool()) {
    return val.getBool();
  } else {
    return false;
  }
}

facebook::jsi::Array ChakraRuntime::getPropertyNames(const facebook::jsi::Object &object) {
  // Handle to the null JS value.
  ChakraObjectRef jsNull = ToChakraObjectRef(facebook::jsi::Value::null());

  // Handle to the Object constructor.
  ChakraObjectRef objectConstructor = GetProperty(GetChakraObjectRef(global()), "Object");

  // Handle to the Object.prototype Object.
  ChakraObjectRef objectPrototype = GetProperty(objectConstructor, "prototype");

  // Handle to the Object.prototype.propertyIsEnumerable() Function.
  ChakraObjectRef objectPrototypePropertyIsEnumerable = GetProperty(objectPrototype, "propertyIsEnumerable");

  // We now traverse the object's property chain and collect all enumerable
  // property names.
  std::vector<ChakraObjectRef> enumerablePropNames{};
  ChakraObjectRef currentObjectOnPrototypeChain = GetChakraObjectRef(object);

  // We have a small optimization here where we stop traversing the prototype
  // chain as soon as we hit Object.prototype. However, we still need to check
  // for null here, as one can create an Object with no prototype through
  // Object.create(null).
  while (!CompareJsValues(currentObjectOnPrototypeChain, objectPrototype) &&
         !CompareJsValues(currentObjectOnPrototypeChain, jsNull)) {
    JsValueRef propNamesRef = JS_INVALID_REFERENCE;
    VerifyJsErrorElseThrow(JsGetOwnPropertyNames(currentObjectOnPrototypeChain, &propNamesRef));
    ChakraObjectRef propNames(propNamesRef);

    int propNamesSize = ToInteger(GetProperty(propNames, "length"));

    for (int i = 0; i < propNamesSize; ++i) {
      JsValueRef propNameRef = JS_INVALID_REFERENCE;
      VerifyJsErrorElseThrow(JsGetIndexedProperty(propNames, ToJsNumber(i), &propNameRef));
      ChakraObjectRef propName(propNameRef);

      std::vector<JsValueRef> args = {currentObjectOnPrototypeChain, propName};

      JsValueRef result;
      VerifyJsErrorElseThrow(JsCallFunction(
          objectPrototypePropertyIsEnumerable, args.data(), static_cast<unsigned short>(args.size()), &result));

      bool propIsEnumerable = ToJsiValue(ChakraObjectRef(result)).getBool();

      if (propIsEnumerable) {
        enumerablePropNames.emplace_back(propName);
      }
    }

    JsValueRef prototype = JS_INVALID_REFERENCE;
    VerifyJsErrorElseThrow(JsGetPrototype(currentObjectOnPrototypeChain, &prototype));
    currentObjectOnPrototypeChain = ChakraObjectRef(prototype);
  }

  size_t enumerablePropNamesSize = enumerablePropNames.size();
  facebook::jsi::Array result = createArray(enumerablePropNamesSize);

  for (size_t i = 0; i < enumerablePropNamesSize; ++i) {
    result.setValueAtIndex(*this, i, MakePointer<facebook::jsi::String>(enumerablePropNames[i]));
  }

  return result;
}

facebook::jsi::Array ChakraRuntime::createArray(size_t length) {
  assert(
      static_cast<unsigned long long>(length) <=
      static_cast<unsigned long long>((std::numeric_limits<unsigned int>::max)()));

  JsValueRef result = JS_INVALID_REFERENCE;
  VerifyJsErrorElseThrow(JsCreateArray(static_cast<unsigned int>(length), &result));
  return MakePointer<facebook::jsi::Object>(result).asArray(*this);
}

size_t ChakraRuntime::size(const facebook::jsi::Array &arr) {
  assert(isArray(arr));

  int result = ToInteger(GetProperty(GetChakraObjectRef(arr), "length"));

  if (result < 0) {
    throw facebook::jsi::JSINativeException("Invalid JS array length detected.");
  }

  return static_cast<size_t>(result);
}

size_t ChakraRuntime::size(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));
  return GetArrayBufferLength(GetChakraObjectRef(arrBuf));
}

uint8_t *ChakraRuntime::data(const facebook::jsi::ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));
  return GetArrayBufferData(GetChakraObjectRef(arrBuf));
}

facebook::jsi::Value ChakraRuntime::getValueAtIndex(const facebook::jsi::Array &arr, size_t index) {
  assert(isArray(arr));
  assert(static_cast<unsigned long long>(index) <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()));

  JsValueRef result = JS_INVALID_REFERENCE;
  VerifyJsErrorElseThrow(JsGetIndexedProperty(GetChakraObjectRef(arr), ToJsNumber(static_cast<int>(index)), &result));
  return ToJsiValue(ChakraObjectRef(result));
}

void ChakraRuntime::setValueAtIndexImpl(facebook::jsi::Array &arr, size_t index, const facebook::jsi::Value &value) {
  assert(isArray(arr));
  assert(static_cast<unsigned long long>(index) <= static_cast<unsigned long long>((std::numeric_limits<int>::max)()));

  VerifyJsErrorElseThrow(
      JsSetIndexedProperty(GetChakraObjectRef(arr), ToJsNumber(static_cast<int>(index)), ToChakraObjectRef(value)));
}

facebook::jsi::Function ChakraRuntime::createFunctionFromHostFunction(
    const facebook::jsi::PropNameID &name,
    unsigned int paramCount,
    facebook::jsi::HostFunctionType func) {
  std::unique_ptr<HostFunctionProxy> hostFuncProxyWrapper = std::make_unique<HostFunctionProxy>(std::move(func), *this);

  JsValueRef funcRef;
  VerifyJsErrorElseThrow(
      JsCreateNamedFunction(ToJsString(utf8(name)), HostFunctionCall, hostFuncProxyWrapper.get(), &funcRef));

  // hostFuncProxy keeps a reference to this Runtime, so we must keep this
  // Runtime alive as long as hostFuncProxy is alive. We do so by making the
  // garbage collector delete hostFuncProxy when funcRef is finalized. Since
  // funcRef cannot out-live this Runtime, the reference stored in hostFuncProxy
  // stays valid during its lifetime.
  VerifyJsErrorElseThrow(JsSetObjectBeforeCollectCallback(
      funcRef, hostFuncProxyWrapper.get(), [](JsRef ref, void *hostFuncProxyToDestroy) {
        // We wrap hostFuncProxyToDestroy in a unique_ptr to avoid calling
        // delete explicitly.
        std::unique_ptr<HostFunctionProxy> wrapper{static_cast<HostFunctionProxy *>(hostFuncProxyToDestroy)};
      }));

  // We only call hostFuncProxyWrapper.release() after
  // JsSetObjectBeforeCollectCallback succeeds. Otherwise, when
  // JsSetObjectBeforeCollectCallback fails and an exception is thrown,
  // the HostFunctionProxy that hostFuncProxyWrapper used to own will be leaked.
  hostFuncProxyWrapper.release();

  facebook::jsi::Object hostFuncObj = MakePointer<facebook::jsi::Object>(funcRef);

  // We do not use the function
  //   Object::setProperty(Runtime & runtime, const char *name, T &&value)
  // here because it causes multiple copies of name.
  setPropertyValue(
      hostFuncObj,
      createPropNameIDFromAscii(g_functionIsHostFunctionPropName, strlen(g_functionIsHostFunctionPropName)),
      facebook::jsi::Value(true));

  return hostFuncObj.getFunction(*this);
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

  std::vector<JsValueRef> argsWithThis = ConstructJsFunctionArguments(thisRef, argRefs);
  assert(
      static_cast<unsigned long long>(argsWithThis.size()) <=
      static_cast<unsigned long long>((std::numeric_limits<unsigned short>::max)()));

  JsValueRef result;
  VerifyJsErrorElseThrow(JsCallFunction(
      GetChakraObjectRef(func), argsWithThis.data(), static_cast<unsigned short>(argsWithThis.size()), &result));
  return ToJsiValue(ChakraObjectRef(result));
}

facebook::jsi::Value
ChakraRuntime::callAsConstructor(const facebook::jsi::Function &func, const facebook::jsi::Value *args, size_t count) {
  // We must store these ChakraObjectRefs on the stack to make sure that they do
  // not go out of scope when JsConstructObject is called.
  ChakraObjectRef undefinedRef = ToChakraObjectRef(facebook::jsi::Value::undefined());
  std::vector<ChakraObjectRef> argRefs = ToChakraObjectRefs(args, count);

  std::vector<JsValueRef> argsWithThis = ConstructJsFunctionArguments(undefinedRef, argRefs);
  assert(
      static_cast<unsigned long long>(argsWithThis.size()) <=
      static_cast<unsigned long long>((std::numeric_limits<unsigned short>::max)()));

  JsValueRef result;
  VerifyJsErrorElseThrow(JsConstructObject(
      GetChakraObjectRef(func), argsWithThis.data(), static_cast<unsigned short>(argsWithThis.size()), &result));
  return ToJsiValue(ChakraObjectRef(result));
}

facebook::jsi::Runtime::ScopeState *ChakraRuntime::pushScope() {
  return nullptr;
}

void ChakraRuntime::popScope(Runtime::ScopeState *state) {
  assert(state == nullptr);
  VerifyJsErrorElseThrow(JsCollectGarbage(m_runtime));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::Symbol &a, const facebook::jsi::Symbol &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::String &a, const facebook::jsi::String &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(const facebook::jsi::Object &a, const facebook::jsi::Object &b) const {
  return CompareJsValues(GetChakraObjectRef(a), GetChakraObjectRef(b));
}

bool ChakraRuntime::instanceOf(const facebook::jsi::Object &obj, const facebook::jsi::Function &func) {
  bool result;
  VerifyJsErrorElseThrow(JsInstanceOf(GetChakraObjectRef(obj), GetChakraObjectRef(func), &result));
  return result;
}

#pragma endregion Functions_inherited_from_Runtime

void ChakraRuntime::VerifyJsErrorElseThrow(JsErrorCode error) {
  switch (error) {
    case JsNoError: {
      return;
      break;
    }

    case JsErrorScriptException: {
      JsValueRef jsError;
      VerifyChakraErrorElseThrow(JsGetAndClearException(&jsError));
      throw facebook::jsi::JSError("", *this, ToJsiValue(ChakraObjectRef(jsError)));
      break;
    }

    default: {
      VerifyChakraErrorElseThrow(error);
      break;
    }
  } // switch (error)

  // Control flow should never reach here.
  std::terminate();
}

facebook::jsi::Value ChakraRuntime::ToJsiValue(ChakraObjectRef &&ref) {
  JsValueType type = GetValueType(ref);

  switch (type) {
    case JsUndefined: {
      return facebook::jsi::Value::undefined();
      break;
    }
    case JsNull: {
      return facebook::jsi::Value::null();
      break;
    }
    case JsNumber: {
      double number;
      VerifyJsErrorElseThrow(JsNumberToDouble(ref, &number));
      return facebook::jsi::Value(number);
      break;
    }
    case JsString: {
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::String>(std::move(ref)));
      break;
    }
    case JsBoolean: {
      bool b;
      VerifyJsErrorElseThrow(JsBooleanToBool(ref, &b));
      return facebook::jsi::Value(b);
      break;
    }
    case JsSymbol: {
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::Symbol>(std::move(ref)));
      break;
    }
    case JsObject:
    case JsFunction:
    case JsError:
    case JsArray:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView: {
      return facebook::jsi::Value(*this, MakePointer<facebook::jsi::Object>(std::move(ref)));
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

ChakraObjectRef ChakraRuntime::ToChakraObjectRef(const facebook::jsi::Value &value) {
  if (value.isUndefined()) {
    JsValueRef ref;
    VerifyJsErrorElseThrow(JsGetUndefinedValue(&ref));
    return ChakraObjectRef(ref);

  } else if (value.isNull()) {
    JsValueRef ref;
    VerifyJsErrorElseThrow(JsGetNullValue(&ref));
    return ChakraObjectRef(ref);

  } else if (value.isBool()) {
    JsValueRef ref;
    VerifyJsErrorElseThrow(JsBoolToBoolean(value.getBool(), &ref));
    return ChakraObjectRef(ref);

  } else if (value.isNumber()) {
    JsValueRef ref;
    VerifyJsErrorElseThrow(JsDoubleToNumber(value.asNumber(), &ref));
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

std::vector<ChakraObjectRef> ChakraRuntime::ToChakraObjectRefs(const facebook::jsi::Value *value, size_t count) {
  std::vector<ChakraObjectRef> result{};

  for (unsigned int i = 0; i < count; ++i) {
    result.emplace_back(ToChakraObjectRef(*value));
    ++value;
  }

  return result;
}

void ChakraRuntime::SetUpMemoryTracker() {
  // Note: We must not use VerifyJsErrorElseThrow here because this function is
  // called in ChakraRuntime's constructor.

  if (m_args->m_memoryLimit) {
    size_t memoryLimit = *(m_args->m_memoryLimit);
    if (memoryLimit == 0) {
      throw facebook::jsi::JSINativeException("ChakraRuntime cannot have a memory limit of zero bytes.");
    }
    VerifyChakraErrorElseThrow(JsSetRuntimeMemoryLimit(m_runtime, memoryLimit));
  }

  if (m_args->m_memoryTracker) {
    std::shared_ptr<facebook::react::MemoryTracker> &memoryTracker = m_args->m_memoryTracker;

    size_t initialMemoryUsage = 0;
    VerifyChakraErrorElseThrow(JsGetRuntimeMemoryUsage(m_runtime, &initialMemoryUsage));
    memoryTracker->Initialize(initialMemoryUsage);

    VerifyChakraErrorElseThrow(
        JsSetRuntimeMemoryAllocationCallback(m_runtime, memoryTracker.get(), MemoryAllocationCallback));
  }
}

JsValueRef CALLBACK ChakraRuntime::HostFunctionCall(
    JsValueRef callee,
    bool isConstructCall,
    JsValueRef *argumentsIncThis,
    unsigned short argumentCountIncThis,
    void *callbackState) {
  HostFunctionProxy *hostFuncProxy = static_cast<HostFunctionProxy *>(callbackState);
  ChakraRuntime &runtime = hostFuncProxy->GetRuntime();
  const facebook::jsi::HostFunctionType &hostFunc = hostFuncProxy->GetHostFunction();

  constexpr uint32_t maxStackArgCount = 8;
  facebook::jsi::Value stackArgs[maxStackArgCount];
  std::unique_ptr<facebook::jsi::Value[]> heapArgs = nullptr;
  facebook::jsi::Value *args = nullptr;

  // Accounting for 'this' object at 0
  unsigned short argumentCount = argumentCountIncThis - 1;

  if (argumentCount > maxStackArgCount) {
    heapArgs = std::make_unique<facebook::jsi::Value[]>(argumentCount);
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      heapArgs[i - 1] = runtime.ToJsiValue(ChakraObjectRef(argumentsIncThis[i]));
    }
    args = heapArgs.get();

  } else {
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      stackArgs[i - 1] = runtime.ToJsiValue(ChakraObjectRef(argumentsIncThis[i]));
    }
    args = stackArgs;
  }

  JsValueRef result = JS_INVALID_REFERENCE;
  facebook::jsi::Value thisVal = runtime.ToJsiValue(ChakraObjectRef(argumentsIncThis[0]));

  try {
    result = runtime.ToChakraObjectRef(hostFunc(runtime, thisVal, args, argumentCount));

  } catch (const facebook::jsi::JSError &error) {
    runtime.VerifyJsErrorElseThrow(JsSetException(runtime.ToChakraObjectRef(error.value())));

  } catch (const std::exception &exn) {
    std::string message = "Exception in HostFunction: ";
    message += exn.what();
    ThrowJsException(message);

  } catch (...) {
    ThrowJsException("Exception in HostFunction: <unknown>");
  }

  return result;
}

facebook::jsi::Value ChakraRuntime::HostObjectGetTrap(
    Runtime &runtime,
    const facebook::jsi::Value & /*thisVal*/,
    const facebook::jsi::Value *args,
    size_t count) {
  // thisVal should always be bound to the hander returned by
  // createHostObjectProxyHandler() and is unused.

  // Three parameters are passed to the get() Proxy hander:
  //   arg[0] is the target object.
  //   arg[1] is the name of the property to get. (We do not support property
  //          Symbols at this point.)
  //   arg[2] is either the Proxy or an object that inherits from the proxy.
  //          We don't use this parameter.
  if (count != 3) {
    throw facebook::jsi::JSINativeException("The get() Proxy handler requires three arguments.");
  }

  ChakraRuntime &chakraRuntime = static_cast<ChakraRuntime &>(runtime);
  ChakraObjectRef target = chakraRuntime.ToChakraObjectRef(args[0]);
  std::string propName = ToStdString(chakraRuntime.ToChakraObjectRef(args[1]));

  if (propName == g_proxyIsHostObjectPropName) {
    return facebook::jsi::Value{true};

  } else if (propName == g_proxyGetHostObjectTargetPropName) {
    // We need to make a copy of target here for the returned jsi::Value
    return chakraRuntime.ToJsiValue(std::move(target));

  } else {
    const std::shared_ptr<facebook::jsi::HostObject> &hostObject = GetExternalData<facebook::jsi::HostObject>(target);
    return hostObject->get(chakraRuntime, chakraRuntime.createPropNameIDFromAscii(propName.c_str(), propName.length()));
  }

  // Control flow should never reach here.
  std::terminate();
}

facebook::jsi::Value ChakraRuntime::HostObjectSetTrap(
    Runtime &runtime,
    const facebook::jsi::Value & /*thisVal*/,
    const facebook::jsi::Value *args,
    size_t count) {
  // thisVal should always be bound to the hander returned by
  // createHostObjectProxyHandler() and is unused.

  // Four parameters are passed to the set() Proxy hander:
  //   arg[0] is the target object.
  //   arg[1] is the name of the property to set. (We do not support property
  //          Symbols at this point.)
  //   arg[2] is the new value of the property to set.
  //   arg[3] is the object to which the assignment was originally directed.
  //          We don't use this parameter.
  if (count != 4) {
    throw facebook::jsi::JSINativeException("The set() Proxy handler requires four arguments.");
  }

  ChakraRuntime &chakraRuntime = static_cast<ChakraRuntime &>(runtime);
  std::string propName = ToStdString(chakraRuntime.ToChakraObjectRef(args[1]));

  if (propName == g_proxyIsHostObjectPropName) {
    throw facebook::jsi::JSINativeException(
        std ::string{g_proxyIsHostObjectPropName} + " is a reserved property and must not be changed.");

  } else if (propName == g_proxyGetHostObjectTargetPropName) {
    throw facebook::jsi::JSINativeException(
        std ::string{g_proxyGetHostObjectTargetPropName} + " is a reserved property and must not be changed.");

  } else {
    ChakraObjectRef target = chakraRuntime.ToChakraObjectRef(args[0]);

    const std::shared_ptr<facebook::jsi::HostObject> &hostObject = GetExternalData<facebook::jsi::HostObject>(target);

    hostObject->set(
        chakraRuntime, chakraRuntime.createPropNameIDFromAscii(propName.c_str(), propName.length()), args[2]);

    return facebook::jsi::Value::undefined();
  }

  // Control flow should never reach here.
  std::terminate();
}

facebook::jsi::Value ChakraRuntime::HostObjectOwnKeysTrap(
    Runtime &runtime,
    const facebook::jsi::Value & /*thisVal*/,
    const facebook::jsi::Value *args,
    size_t count) {
  // thisVal should always be bound to the hander returned by
  // createHostObjectProxyHandler() and is unused.

  if (count != 1) {
    throw facebook::jsi::JSINativeException("HostObjectOwnKeysTrap() requires one argument.");
  }

  ChakraRuntime &chakraRuntime = static_cast<ChakraRuntime &>(runtime);
  ChakraObjectRef target = chakraRuntime.ToChakraObjectRef(args[0]);

  const std::shared_ptr<facebook::jsi::HostObject> &hostObject = GetExternalData<facebook::jsi::HostObject>(target);

  auto ownKeys = hostObject->getPropertyNames(chakraRuntime);

  std::set<std::string> dedupedOwnKeys{};
  for (size_t i = 0; i < ownKeys.size(); ++i) {
    dedupedOwnKeys.insert(ownKeys[i].utf8(chakraRuntime));
  }

  size_t numKeys = dedupedOwnKeys.size();
  facebook::jsi::Array result = chakraRuntime.createArray(numKeys);

  size_t index = 0;
  for (const std::string &key : dedupedOwnKeys) {
    result.setValueAtIndex(chakraRuntime, index, facebook::jsi::String::createFromUtf8(chakraRuntime, key));
    ++index;
  }

  return result;
}

facebook::jsi::Object ChakraRuntime::createHostObjectProxyHandler() noexcept {
  // TODO (yicyao): handler can be cached and reused for multiple HostObjects.

  facebook::jsi::Object handler = createObject();

  constexpr const char *const getPropName = "get";
  constexpr const char *const setPropName = "set";
  constexpr const char *const ownKeysPropName = "ownKeys";

  facebook::jsi::PropNameID getPropId = createPropNameIDFromAscii(getPropName, strlen(getPropName));
  facebook::jsi::PropNameID setPropId = createPropNameIDFromAscii(setPropName, strlen(setPropName));
  facebook::jsi::PropNameID ownKeysPropId = createPropNameIDFromAscii(ownKeysPropName, strlen(ownKeysPropName));

  handler.setProperty(*this, getPropName, createFunctionFromHostFunction(getPropId, 2, HostObjectGetTrap));

  handler.setProperty(*this, setPropName, createFunctionFromHostFunction(setPropId, 3, HostObjectSetTrap));

  handler.setProperty(*this, ownKeysPropName, createFunctionFromHostFunction(ownKeysPropId, 1, HostObjectOwnKeysTrap));

  return handler;
}

} // namespace Microsoft::JSI
