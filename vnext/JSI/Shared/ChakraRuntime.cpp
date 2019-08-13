// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraRuntime.h"

#include "unicode.h"
#include "utilities.h"

#include "windef.h"

#include <cassert>
#include <climits>
#include <exception>
#include <sstream>

namespace facebook::jsi::chakra {

namespace {

constexpr const char *const g_hostObjectNotImplementedErrorString =
    "Host Object is not yet implemented for ChakraRuntime.";

constexpr const char *const g_hostFunctionNotImplementedErrorString =
    "Host Funcion is not yet implemented for ChakraRuntime.";

inline static void crashUponJsError(JsErrorCode error) {
  if (error != JsNoError) {
    std::terminate();
  }
}

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

// TODO (yicyao)
class ChakraPreparedJavaScript : public PreparedJavaScript {
 public:
  ChakraPreparedJavaScript(
      const std::shared_ptr<const Buffer> &sourceCodeBuffer,
      const std::string &sourceCodeUrl) {
    // TODO (yicyao)
  }

  const wchar_t *sourceCode() const {
    // TODO (yicyao)
    return nullptr;
  }

  BYTE *bytecode() const {
    // TODO (yicyao)
    return nullptr;
  }

  const wchar_t *sourceUrl() const {
    // TODO (yicyao)
    return nullptr;
  }
};

} // anonymous namespace

ChakraObjectRef::ChakraObjectRef() noexcept
    : m_ref{nullptr}, m_state{State::Uninitialized} {}

ChakraObjectRef::ChakraObjectRef(JsRef ref)
    : m_ref{nullptr}, m_state{State::Uninitialized} {
  if (!ref) {
    throw JSINativeException(
        "Cannot initialize a ChakraObjectRef with a "
        "null reference.");
  }

  crashUponJsError(JsAddRef(ref, nullptr));

  m_ref = ref;
  m_state = State::Initialized;
}

ChakraObjectRef::ChakraObjectRef(const ChakraObjectRef &other) noexcept
    : m_ref{other.m_ref}, m_state{other.m_state} {
  if (m_state == State::Initialized) {
    crashUponJsError(JsAddRef(m_ref, nullptr));
  }
}

ChakraObjectRef::ChakraObjectRef(ChakraObjectRef &&other) noexcept
    : m_ref{other.m_ref}, m_state{other.m_state} {
  if (other.m_state == State::Initialized) {
    other.m_state = State::Invalidated;
  }
}

ChakraObjectRef &ChakraObjectRef::operator=(
    const ChakraObjectRef &rhs) noexcept {
  m_ref = rhs.m_ref;
  m_state = rhs.m_state;

  if (m_state == State::Initialized) {
    crashUponJsError(JsAddRef(m_ref, nullptr));
  }

  return *this;
}

ChakraObjectRef &ChakraObjectRef::operator=(ChakraObjectRef &&rhs) noexcept {
  m_ref = rhs.m_ref;
  m_state = rhs.m_state;

  if (rhs.m_state == State::Initialized) {
    rhs.m_state = State::Invalidated;
  }

  return *this;
}

ChakraObjectRef::~ChakraObjectRef() noexcept {
  if (m_state == State::Initialized) {
    crashUponJsError(JsRelease(m_ref, nullptr));
  }
}

void ChakraObjectRef::initialize(JsRef ref) {
  if (m_state != State::Uninitialized) {
    throw JSINativeException("A ChakraObjectRef can only be initialzed once.");
  }
  assert(!m_ref);

  if (!ref) {
    throw JSINativeException(
        "Cannot initialize a ChakraObjectRef with a null "
        "reference.");
  }

  crashUponJsError(JsAddRef(ref, nullptr));
  m_ref = ref;
  m_state = State::Initialized;
}

void ChakraObjectRef::invalidate() {
  if (m_state == State::Uninitialized) {
    assert(!m_ref);
    throw JSINativeException(
        "Cannot invalidate a ChakraObjectRef that has not "
        "been initialized.");
  }

  if (m_state == State::Invalidated) {
    throw JSINativeException(
        "Cannot invalidate a ChakraObjectRef that has "
        "already been invalidated.");
  }

  crashUponJsError(JsRelease(m_ref, nullptr));
  m_state = State::Invalidated;
}

ChakraRuntime::ChakraRuntime(ChakraRuntimeArgs &&args) noexcept
    : m_args(std::move(args)), m_runtime{nullptr}, m_context{} {
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (m_args.disableBackgroundWork) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(
        runtimeAttributes | JsRuntimeAttributeDisableBackgroundWork);
  }

  if (m_args.disableExecutablePageAllocation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(
        runtimeAttributes | JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  // TODO (yicyao):
  // The second argument to JsCreateRuntime is a JsThreadServiceCallback, which
  // allows the host to specify a background thread service for the created
  // runtime. If specified, then background work items will be passed to the
  // host using the provided JsThreadServiceCallback. We should explore this
  // option in the future.
  crashUponJsError(JsCreateRuntime(runtimeAttributes, nullptr, &m_runtime));

  JsContextRef context = nullptr;
  crashUponJsError(JsCreateContext(m_runtime, &context));
  // Even though initialize() can throw, we do not expect it to do so here.
  // Hence we can mark this constructor as noexcept.
  m_context.initialize(context);

  // TODO (yicyao):
  // The documentation for JsSetCurrentContext says that it "sets the current
  // script context on the thread." This suggests that ChakraCore is using
  // thread local storage for context data. We will need to revisit this when
  // we attempt to make ChakraRuntime thread safe.
  crashUponJsError(JsSetCurrentContext(context));
}

ChakraRuntime::~ChakraRuntime() noexcept {
  crashUponJsError(JsSetCurrentContext(JS_INVALID_REFERENCE));
  // Even though invalidate() can throw, we do not expect it to do so here.
  // Hence we can mark this destructor as noexcept.
  m_context.invalidate();
  crashUponJsError(JsDisposeRuntime(m_runtime));
}

//==============================================================================
// Below are functions inherited from Runtime.

Value ChakraRuntime::evaluateJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    const std::string &sourceURL) {
  const std::wstring scriptUtf16 = react::unicode::utf8ToUtf16(
      react::utilities::checkedReinterpretCast<const char *>(buffer->data()),
      buffer->size());

  const std::wstring urlUtf16 = react::unicode::utf8ToUtf16(sourceURL);

  return evaluateJavaScriptImpl(scriptUtf16.c_str(), urlUtf16.c_str());
}

std::shared_ptr<const PreparedJavaScript> ChakraRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceURL) {
  return std::make_shared<const ChakraPreparedJavaScript>(buffer, sourceURL);
}

Value ChakraRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const PreparedJavaScript> &js) {
  const std::shared_ptr<const ChakraPreparedJavaScript> preparedJs =
      std::static_pointer_cast<const ChakraPreparedJavaScript>(js);

  JsValueRef result = nullptr;
  JsErrorCode error = JsRunSerializedScript(
      preparedJs->sourceCode(),
      preparedJs->bytecode(),
      JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
      preparedJs->sourceUrl(),
      &result);

  if (error == JsErrorBadSerializedScript) {
    JsValueRef exception = nullptr;
    JsGetAndClearException(&exception);
    return evaluateJavaScriptImpl(
        preparedJs->sourceCode(), preparedJs->sourceUrl());
  }

  throwUponJsError(error, "JsRunSerializedScript");
  return toJsiValue(ChakraObjectRef(result));
}

Object ChakraRuntime::global() {
  JsValueRef global;
  throwUponJsError(JsGetGlobalObject(&global), "JsGetGlobalObject");
  return make<Object>(new ChakraPointerValue(ChakraObjectRef(global)));
}

std::string ChakraRuntime::description() {
  return "ChakraRuntime";
}

bool ChakraRuntime::isInspectable() {
  return false;
}

Runtime::PointerValue *ChakraRuntime::cloneSymbol(
    const Runtime::PointerValue *pv) {
  return cloneChakraPointerValue(pv);
}

Runtime::PointerValue *ChakraRuntime::cloneString(
    const Runtime::PointerValue *pv) {
  return cloneChakraPointerValue(pv);
}

Runtime::PointerValue *ChakraRuntime::cloneObject(
    const Runtime::PointerValue *pv) {
  return cloneChakraPointerValue(pv);
}

Runtime::PointerValue *ChakraRuntime::clonePropNameID(
    const Runtime::PointerValue *pv) {
  return cloneChakraPointerValue(pv);
}

PropNameID ChakraRuntime::createPropNameIDFromAscii(
    const char *str,
    size_t length) {
  // Unfortunately due to the typing used by JSI and Chakra, we have to do a
  // double reinterpret cast here.
  return createPropNameIDFromUtf8(
      react::utilities::checkedReinterpretCast<const uint8_t *>(str), length);
}

PropNameID ChakraRuntime::createPropNameIDFromUtf8(
    const uint8_t *utf8,
    size_t length) {
  std::wstring utf16 = react::unicode::utf8ToUtf16(
      react::utilities::checkedReinterpretCast<const char *>(utf8), length);
  return make<PropNameID>(new ChakraPointerValue(getPropertyId(utf16.c_str())));
}

PropNameID ChakraRuntime::createPropNameIDFromString(const String &str) {
  // We do not use the function
  //   std::string ChakraRuntime::utf8(const String& str)
  // here to avoid excessive Unicode conversions.
  std::wstring propName = toStdWstring(getChakraObjectRef(str));
  return make<PropNameID>(
      new ChakraPointerValue(getPropertyId(propName.c_str())));
}

std::string ChakraRuntime::utf8(const PropNameID &id) {
  return react::unicode::utf16ToUtf8(getPropertyName(getChakraObjectRef(id)));
}

bool ChakraRuntime::compare(const PropNameID &lhs, const PropNameID &rhs) {
  return comparePropertyIds(getChakraObjectRef(lhs), getChakraObjectRef(rhs));
}

std::string ChakraRuntime::symbolToString(const Symbol &s) {
  return utf8(
      make<String>(new ChakraPointerValue(toJsString(getChakraObjectRef(s)))));
}

String ChakraRuntime::createStringFromAscii(const char *str, size_t length) {
  // Unfortunately due to the typing used by JSI and Chakra, we have to do a
  // double reinterpret cast here.
  return createStringFromUtf8(
      react::utilities::checkedReinterpretCast<const uint8_t *>(str), length);
}

String ChakraRuntime::createStringFromUtf8(const uint8_t *utf8, size_t length) {
  return make<String>(new ChakraPointerValue(toJsString(
      react::utilities::checkedReinterpretCast<const char *>(utf8), length)));
}

std::string ChakraRuntime::utf8(const String &str) {
  return react::unicode::utf16ToUtf8(toStdWstring(getChakraObjectRef(str)));
}

Object ChakraRuntime::createObject() {
  JsValueRef obj;
  throwUponJsError(JsCreateObject(&obj), "JsCreateObject");
  return make<Object>(new ChakraPointerValue(ChakraObjectRef(obj)));
}

Object ChakraRuntime::createObject(std::shared_ptr<HostObject> ho) {
  throw std::runtime_error(g_hostObjectNotImplementedErrorString);
}

std::shared_ptr<HostObject> ChakraRuntime::getHostObject(const jsi::Object &) {
  throw std::runtime_error(g_hostObjectNotImplementedErrorString);
}

HostFunctionType &ChakraRuntime::getHostFunction(const jsi::Function &) {
  throw std::runtime_error(g_hostFunctionNotImplementedErrorString);
  std::terminate();
}

Value ChakraRuntime::getProperty(const Object &obj, const PropNameID &name) {
  JsValueRef result;
  throwUponJsError(
      JsGetProperty(getChakraObjectRef(obj), getChakraObjectRef(name), &result),
      "JsGetProperty");
  return toJsiValue(ChakraObjectRef(result));
}

Value ChakraRuntime::getProperty(const Object &obj, const String &name) {
  return getProperty(obj, createPropNameIDFromString(name));
}

bool ChakraRuntime::hasProperty(const Object &obj, const PropNameID &name) {
  bool result;
  throwUponJsError(
      JsHasProperty(getChakraObjectRef(obj), getChakraObjectRef(name), &result),
      "JsHasProperty");
  return result;
}

bool ChakraRuntime::hasProperty(const Object &obj, const String &name) {
  return hasProperty(obj, createPropNameIDFromString(name));
}

void ChakraRuntime::setPropertyValue(
    Object &obj,
    const PropNameID &name,
    const Value &value) {
  // TODO (yicyao): Need to figure out whether we should follow strict mode
  // rules here or not.
  throwUponJsError(
      JsSetProperty(
          getChakraObjectRef(obj),
          getChakraObjectRef(name),
          toChakraObjectRef(value),
          false),
      "JsSetProperty");
}

void ChakraRuntime::setPropertyValue(
    Object &obj,
    const String &name,
    const Value &value) {
  setPropertyValue(obj, createPropNameIDFromString(name), value);
}

bool ChakraRuntime::isArray(const Object &obj) const {
  return getValueType(getChakraObjectRef(obj)) == JsArray;
}

bool ChakraRuntime::isArrayBuffer(const Object &obj) const {
  return getValueType(getChakraObjectRef(obj)) == JsArrayBuffer;
}

bool ChakraRuntime::isFunction(const Object &obj) const {
  return getValueType(getChakraObjectRef(obj)) == JsFunction;
}

bool ChakraRuntime::isHostObject(const jsi::Object &obj) const {
  throw std::runtime_error(g_hostObjectNotImplementedErrorString);
}

bool ChakraRuntime::isHostFunction(const jsi::Function &func) const {
  throw std::runtime_error(g_hostFunctionNotImplementedErrorString);
}

Array ChakraRuntime::getPropertyNames(const Object &obj) {
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

  std::shared_ptr<StringBuffer> jsGetPropertyNamesSourceBuffer =
      std::make_shared<StringBuffer>(jsGetPropertyNamesSource);

  Function jsGetPropertyNames =
      evaluateJavaScript(jsGetPropertyNamesSourceBuffer, "")
          .asObject(*this)
          .asFunction(*this);

  Value objAsValue(*this, obj);
  return call(jsGetPropertyNames, Value::undefined(), &objAsValue, 1)
      .asObject(*this)
      .asArray(*this);
}

// Only ChakraCore supports weak reference semantics, so ChakraRuntime
// WeakObjects are in fact strong references.

WeakObject ChakraRuntime::createWeakObject(const Object &obj) {
  return make<WeakObject>(cloneChakraPointerValue(getPointerValue(obj)));
}

Value ChakraRuntime::lockWeakObject(const WeakObject &weakObj) {
  // We need to make a copy of the ChakraObjectRef held within weakObj's
  // member PointerValue for the returned jsi::Value here.
  ChakraObjectRef ref = getChakraObjectRef(weakObj);
  return toJsiValue(std::move(ref));
}

Array ChakraRuntime::createArray(size_t length) {
  assert(length <= UINT_MAX);
  JsValueRef a = nullptr;
  throwUponJsError(
      JsCreateArray(static_cast<unsigned int>(length), &a), "JsCreateArray");
  return make<Object>(new ChakraPointerValue(ChakraObjectRef(a)))
      .asArray(*this);
}

size_t ChakraRuntime::size(const Array &arr) {
  assert(isArray(arr));

  constexpr const uint8_t propName[] = {'l', 'e', 'n', 'g', 't', 'h'};
  return static_cast<size_t>(
      getProperty(
          arr,
          createPropNameIDFromUtf8(
              propName, react::utilities::arraySize(propName)))
          .asNumber());
}

size_t ChakraRuntime::size(const ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));

  constexpr const uint8_t propName[] = {
      'b', 'y', 't', 'e', 'l', 'e', 'n', 'g', 't', 'h'};
  return static_cast<size_t>(
      getProperty(
          arrBuf,
          createPropNameIDFromUtf8(
              propName, react::utilities::arraySize(propName)))
          .asNumber());
}

uint8_t *ChakraRuntime::data(const ArrayBuffer &arrBuf) {
  assert(isArrayBuffer(arrBuf));

  uint8_t *buffer = nullptr;
  unsigned int size = 0;
  throwUponJsError(
      JsGetArrayBufferStorage(getChakraObjectRef(arrBuf), &buffer, &size),
      "JsGetArrayBufferStorage");
  return buffer;
}

Value ChakraRuntime::getValueAtIndex(const Array &arr, size_t index) {
  assert(isArray(arr));

  JsValueRef result;
  throwUponJsError(
      JsGetIndexedProperty(
          getChakraObjectRef(arr),
          toChakraObjectRef(Value(static_cast<double>(index))),
          &result),
      "JsGetIndexedProperty");
  return toJsiValue(ChakraObjectRef(result));
}

void ChakraRuntime::setValueAtIndexImpl(
    Array &arr,
    size_t index,
    const Value &value) {
  assert(isArray(arr));

  throwUponJsError(
      JsSetIndexedProperty(
          getChakraObjectRef(arr),
          toChakraObjectRef(Value(static_cast<double>(index))),
          toChakraObjectRef(value)),
      "JsSetIndexedProperty");
}

Function ChakraRuntime::createFunctionFromHostFunction(
    const PropNameID &name,
    unsigned int paramCount,
    HostFunctionType func) {
  throw std::runtime_error(g_hostFunctionNotImplementedErrorString);
}

Value ChakraRuntime::call(
    const Function &func,
    const Value &jsThis,
    const Value *args,
    size_t count) {
  // We must store the vector returned by toChakraObjectRefs to make sure that
  // the ChakraObjectRefs it stores do not go out of scope.
  std::vector<ChakraObjectRef> argRefs = toChakraObjectRefs(args, count);

  std::vector<JsValueRef> argsWithThis =
      constructJsFunctionArguments(toChakraObjectRef(jsThis), argRefs);
  assert(argsWithThis.size() <= USHRT_MAX);

  JsValueRef result;
  throwUponJsError(
      JsCallFunction(
          getChakraObjectRef(func),
          argsWithThis.data(),
          static_cast<unsigned short>(argsWithThis.size()),
          &result),
      "JsCallFunction");
  return toJsiValue(ChakraObjectRef(result));
}

Value ChakraRuntime::callAsConstructor(
    const Function &func,
    const Value *args,
    size_t count) {
  // We must store the vector returned by toChakraObjectRefs to make sure that
  // the ChakraObjectRefs it stores do not go out of scope.
  std::vector<ChakraObjectRef> argRefs = toChakraObjectRefs(args, count);

  std::vector<JsValueRef> argsWithThis =
      constructJsFunctionArguments(toChakraObjectRef(Value()), argRefs);
  assert(argsWithThis.size() <= USHRT_MAX);

  JsValueRef result;
  throwUponJsError(
      JsConstructObject(
          getChakraObjectRef(func),
          argsWithThis.data(),
          static_cast<unsigned short>(argsWithThis.size()),
          &result),
      "JsConstructObject");
  return toJsiValue(ChakraObjectRef(result));
}

Runtime::ScopeState *ChakraRuntime::pushScope() {
  return nullptr;
}

void ChakraRuntime::popScope(Runtime::ScopeState *state) {
  assert(state == nullptr);
  throwUponJsError(JsCollectGarbage(m_runtime), "JsCollectGarbage");
}

bool ChakraRuntime::strictEquals(const Symbol &a, const Symbol &b) const {
  return areStrictlyEqual(getChakraObjectRef(a), getChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(const String &a, const String &b) const {
  return areStrictlyEqual(getChakraObjectRef(a), getChakraObjectRef(b));
}

bool ChakraRuntime::strictEquals(const Object &a, const Object &b) const {
  return areStrictlyEqual(getChakraObjectRef(a), getChakraObjectRef(b));
}

bool ChakraRuntime::instanceOf(const Object &o, const Function &f) {
  bool result;
  throwUponJsError(
      JsInstanceOf(getChakraObjectRef(o), getChakraObjectRef(f), &result),
      "JsInstanceOf");
  return result;
}

// Above are functions inherited from Runtime.
//==============================================================================

void ChakraRuntime::throwUponJsError(
    JsErrorCode error,
    const char *const chakraApiName) {
  switch (error) {
    case JsNoError: {
      return;
      break;
    }

    case JsErrorScriptException: {
      JsValueRef jsError;
      crashUponJsError(JsGetAndClearException(&jsError));
      throw JSError(
          "A JavaScript Error was thrown.",
          *this,
          toJsiValue(ChakraObjectRef(jsError)));
      break;
    }

    default: {
      std::ostringstream errorString;
      errorString << "A call to "
                  << (chakraApiName ? chakraApiName : "Chakra(Core) API")
                  << " returned error code 0x" << std::hex << error << '.';
      throw JSINativeException(errorString.str());
      break;
    }
  }
}

ChakraObjectRef ChakraRuntime::toChakraObjectRef(const Value &value) {
  if (value.isUndefined()) {
    JsValueRef ref;
    throwUponJsError(JsGetUndefinedValue(&ref), "JsGetUndefinedValue");
    return ChakraObjectRef(ref);
  } else if (value.isNull()) {
    JsValueRef ref;
    throwUponJsError(JsGetNullValue(&ref), "JsGetNullValue");
    return ChakraObjectRef(ref);
  } else if (value.isBool()) {
    JsValueRef ref;
    throwUponJsError(JsBoolToBoolean(value.getBool(), &ref), "JsBoolToBoolean");
    return ChakraObjectRef(ref);
  } else if (value.isNumber()) {
    JsValueRef ref;
    throwUponJsError(
        JsDoubleToNumber(value.asNumber(), &ref), "JsDoubleToNumber");
    return ChakraObjectRef(ref);
  } else if (value.isSymbol()) {
    return getChakraObjectRef(value.asSymbol(*this));
  } else if (value.isString()) {
    return getChakraObjectRef(value.asString(*this));
  } else if (value.isObject()) {
    return getChakraObjectRef(value.asObject(*this));
  } else {
    // Control flow should never reach here.
    std::terminate();
  }
}

std::vector<ChakraObjectRef> ChakraRuntime::toChakraObjectRefs(
    const Value *value,
    size_t count) {
  std::vector<ChakraObjectRef> result{};
  for (unsigned int i = 0; i < count; ++i) {
    result.emplace_back(toChakraObjectRef(*value));
    ++value;
  }
  return result;
}

Value ChakraRuntime::toJsiValue(ChakraObjectRef &&ref) {
  JsValueType type = getValueType(ref);

  switch (type) {
    case JsUndefined:
      return Value();
      break;

    case JsNull:
      return Value(nullptr);
      break;

    case JsNumber:
      double number;
      throwUponJsError(JsNumberToDouble(ref, &number), "JsNumberToDouble");
      return Value(number);
      break;

    case JsString:
      return Value(*this, make<String>(new ChakraPointerValue(std::move(ref))));
      break;

    case JsBoolean:
      bool b;
      throwUponJsError(JsBooleanToBool(ref, &b), "JsBooleanToBool");
      return Value(b);
      break;

    case JsSymbol:
      return Value(*this, make<Symbol>(new ChakraPointerValue(std::move(ref))));
      break;

    case JsObject:
    case JsFunction:
    case JsError:
    case JsArray:
    case JsArrayBuffer:
    case JsTypedArray:
    case JsDataView:
      return Value(*this, make<Object>(new ChakraPointerValue(std::move(ref))));
      break;

    default:
      // Control flow should never reach here.
      std::terminate();
      break;
  }

  // Control flow should never reach here.
  std::terminate();
}

JsValueType ChakraRuntime::getValueType(const ChakraObjectRef &value) const {
  JsValueType type;
  crashUponJsError(JsGetValueType(value, &type));
  return type;
}

bool ChakraRuntime::areStrictlyEqual(
    const ChakraObjectRef &lhs,
    const ChakraObjectRef &rhs) const {
  bool result = false;
  // Note that JsStrictEquals should only be used for JsValueRefs and not for
  // other types of JsRefs (e.g. JsPropertyIdRef, etc.).
  crashUponJsError(JsStrictEquals(lhs, rhs, &result));
  return result;
}

std::wstring ChakraRuntime::toStdWstring(const ChakraObjectRef jsString) {
  assert(getValueType(jsString) == JsString);

  const wchar_t *utf16;
  size_t length;
  throwUponJsError(
      JsStringToPointer(jsString, &utf16, &length), "JsStringToPointer");

  return std::wstring(utf16, length);
}

ChakraObjectRef ChakraRuntime::toJsString(const char *utf8, size_t length) {
  std::wstring utf16 = react::unicode::utf8ToUtf16(utf8, length);
  JsValueRef result;
  throwUponJsError(
      JsPointerToString(utf16.c_str(), utf16.length(), &result),
      "JsPointerToString");
  return ChakraObjectRef(result);
}

ChakraObjectRef ChakraRuntime::toJsString(const ChakraObjectRef &ref) {
  JsValueRef str = nullptr;
  throwUponJsError(JsConvertValueToString(ref, &str), "JsConvertValueToString");
  return ChakraObjectRef(str);
}

const wchar_t *ChakraRuntime::getPropertyName(JsPropertyIdRef id) {
  if (getPropertyIdType(id) != JsPropertyIdTypeString) {
    throw JSINativeException(
        "It is llegal to retrieve the name of a property "
        "symbol.");
  }

  const wchar_t *propertyName;
  throwUponJsError(
      JsGetPropertyNameFromId(id, &propertyName), "JsGetPropertyNameFromId");
  return propertyName;
}

ChakraObjectRef ChakraRuntime::getPropertySymbol(JsPropertyIdRef id) {
  if (getPropertyIdType(id) != JsPropertyIdTypeSymbol) {
    throw JSINativeException(
        "It is llegal to retrieve the symbol associated "
        "with a property name.");
  }

  JsValueRef symbol = nullptr;
  throwUponJsError(
      JsGetSymbolFromPropertyId(id, &symbol), "JsGetSymbolFromPropertyId");
  return ChakraObjectRef{symbol};
}

ChakraObjectRef ChakraRuntime::getPropertyId(const wchar_t *const name) {
  JsPropertyIdRef id = nullptr;
  throwUponJsError(
      JsGetPropertyIdFromName(name, &id), "JsGetPropertyIdFromName");
  return ChakraObjectRef(id);
}

JsPropertyIdType ChakraRuntime::getPropertyIdType(JsPropertyIdRef id) {
  JsPropertyIdType type;
  throwUponJsError(JsGetPropertyIdType(id, &type), "JsGetPropertyIdType");
  return type;
}

bool ChakraRuntime::comparePropertyIds(
    JsPropertyIdRef lhs,
    JsPropertyIdRef rhs) {
  JsPropertyIdType lhsType = getPropertyIdType(lhs);
  JsPropertyIdType rhsType = getPropertyIdType(rhs);

  if (lhsType != rhsType) {
    return false;
  }

  if (lhsType == JsPropertyIdTypeString) {
    assert(rhsType == JsPropertyIdTypeString);
    return wcscmp(getPropertyName(lhs), getPropertyName(rhs)) == 0;
  }

  if (lhsType == JsPropertyIdTypeSymbol) {
    assert(rhsType == JsPropertyIdTypeSymbol);
    return areStrictlyEqual(getPropertySymbol(lhs), getPropertySymbol(rhs));
  }

  // Control should never reach here.
  std::terminate();
}

Value ChakraRuntime::evaluateJavaScriptImpl(
    const wchar_t *scriptUtf16,
    const wchar_t *urlUtf16) {
  JsValueRef result;
  throwUponJsError(
      JsRunScript(
          scriptUtf16,
          JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
          urlUtf16,
          &result),
      "JsRunScript");

  return toJsiValue(ChakraObjectRef(result));
}

} // namespace facebook::jsi::chakra
