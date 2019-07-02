// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <folly/json.h>

#include "ChakraHelpers.h"
#include "ChakraValue.h"

// See the comment under Value::fromDynamic()
#if !defined(__APPLE__) && defined(WITH_FB_JSC_TUNING)
#define USE_FAST_FOLLY_DYNAMIC_CONVERSION 1
#else
#define USE_FAST_FOLLY_DYNAMIC_CONVERSION 0
#endif

namespace facebook {
namespace react {

ChakraValue::ChakraValue(JsValueRef value) : m_value(value) {}

ChakraValue::ChakraValue(ChakraValue &&other) : m_value(other.m_value) {
  other.m_value = nullptr;
}

std::string ChakraValue::toJSONString(unsigned indent) const {
  JsValueRef exn;
  auto stringToAdopt = JSValueCreateJSONString(m_value, indent, &exn);
  if (stringToAdopt == nullptr) {
    std::string exceptionText = ChakraValue(exn).toString().str();
    throwJSExecutionException(
        "Exception creating JSON string: %s", exceptionText.c_str());
  }
  return ChakraString::ref(stringToAdopt).str();
}

/* static */
ChakraValue ChakraValue::fromJSON(const ChakraString &json) {
  auto result = JSValueMakeFromJSONString(json);
  if (!result) {
    throwJSExecutionException(
        "Failed to create String from JSON: %s", json.str().c_str());
  }
  return ChakraValue(result);
}

JsValueRef ChakraValue::fromDynamic(const folly::dynamic &value) {
  // JavaScriptCore's iOS APIs have their own version of this direct conversion.
  // In addition, using this requires exposing some of JSC's private APIs,
  //  so it's limited to non-apple platforms and to builds that use the custom
  //  JSC.
  // Otherwise, we use the old way of converting through JSON.
#if USE_FAST_FOLLY_DYNAMIC_CONVERSION
  // Defer GC during the creation of the JSValue, as we don't want
  //  intermediate objects to be collected.
  // We could use JSValueProtect(), but it will make the process much slower.
  JSDeferredGCRef deferGC = JSDeferGarbageCollection(ctx);
  // Set a global lock for the whole process,
  //  instead of re-acquiring the lock for each operation.
  JSLock(ctx);
  JsValueRef jsVal = Value::fromDynamicInner(ctx, value);
  JSUnlock(ctx);
  JSResumeGarbageCollection(ctx, deferGC);
  return jsVal;
#else
  auto json = folly::toJson(value);
  return fromJSON(ChakraString(json.c_str()));
#endif
}

ChakraObject ChakraValue::asObject() {
  JsValueRef exn = nullptr;

  JsValueRef obj;
  auto result = JsConvertValueToObject(m_value, &obj);
  if (result != JsNoError) {
    bool hasException = false;
    if (JsHasException(&hasException) != JsNoError) {
      assert(false && "JsHasException() failed.");
      std::terminate();
    }

    if (hasException) {
      JsGetAndClearException(&exn);
      std::string exceptionText = ChakraValue(exn).toString().str();
      throwJSExecutionException(
          "Failed to convert to object: %s", exceptionText.c_str());
    } else {
      throwJSExecutionException(
          "JsConvertValueToObject() failed for reason other than script exception.");
    }
  }

  return ChakraObject(obj);
}

ChakraValue ChakraValue::makeError(const char *error) {
  JsValueRef exn = nullptr;
  JsValueRef message = ChakraValue(ChakraString(error));

  JsValueRef value;
  if (JsNoError != JsCreateError(message, &value)) {
    assert(false && "JsCreateError() failed");
    std::terminate();
  }

  return ChakraValue(value);
}

ChakraValue ChakraObject::callAsFunction(
    std::initializer_list<JsValueRef> args) const {
  // OFFICEDEV
  // explicitly pass global object instead of nullptr for Chakra
  assert(args.size() < INT_MAX);
  return callAsFunction(
      ChakraObject::getGlobalObject(),
      static_cast<int>(args.size()),
      args.begin());
}

ChakraValue ChakraObject::callAsFunction(
    const ChakraObject &thisObj,
    std::initializer_list<JsValueRef> args) const {
  assert(args.size() < INT_MAX);
  return callAsFunction(
      (JsValueRef)thisObj, static_cast<int>(args.size()), args.begin());
}

ChakraValue ChakraObject::callAsFunction(int nArgs, const JsValueRef args[])
    const {
  return callAsFunction(nullptr, nArgs, args);
}

ChakraValue ChakraObject::callAsFunction(
    const ChakraObject &thisObj,
    int nArgs,
    const JsValueRef args[]) const {
  return callAsFunction(static_cast<JsValueRef>(thisObj), nArgs, args);
}

ChakraValue ChakraObject::callAsFunction(
    JsValueRef thisObj,
    int nArgs,
    const JsValueRef args[]) const {
  JsValueRef exn = nullptr;

  auto argsWithThis = new JsValueRef[nArgs + 1];

  argsWithThis[0] = thisObj;
  for (int i = 0; i < nArgs; i++)
    argsWithThis[i + 1] = args[i];

  JsValueRef value;
  auto result = JsCallFunction(
      m_obj,
      const_cast<JsValueRef *>(argsWithThis),
      static_cast<unsigned short>(nArgs + 1),
      &value);
  delete[] argsWithThis;
  if (result != JsNoError) {
    bool hasException = false;
    if (JsHasException(&hasException) != JsNoError) {
      assert(false && "JsHasException() failed.");
      std::terminate();
    }

    if (hasException) {
      JsGetAndClearException(&exn);
      std::string exceptionText = ChakraValue(exn).toString().str();
      throwJSExecutionException(
          "Exception calling object as function: %s", exceptionText.c_str());
    } else {
      throwJSExecutionException(
          "JsCallFunction() failed for reason other than script exception.");
    }
  }

  return ChakraValue(value);
}

ChakraObject ChakraObject::callAsConstructor(
    std::initializer_list<JsValueRef> args) const {
  JsValueRef exn = nullptr;

  JsValueRef value;
  auto result = JsConstructObject(
      m_obj,
      const_cast<JsValueRef *>(args.begin()),
      static_cast<unsigned short>(args.size()),
      &value);

  if (result != JsNoError) {
    bool hasException = false;
    if (JsHasException(&hasException) != JsNoError) {
      assert(false && "JsHasException() failed.");
      std::terminate();
    }

    if (hasException) {
      JsGetAndClearException(&exn);
      std::string exceptionText = ChakraValue(exn).toString().str();
      throwJSExecutionException(
          "Exception calling object as constructor: %s", exceptionText.c_str());
    } else {
      throwJSExecutionException(
          "JsConstructObject() failed for reason other than script exception.");
    }
  }

  return ChakraObject(value);
}

ChakraValue ChakraObject::getProperty(const ChakraString &propName) const {
  JsValueRef exn;
  JsValueRef property = JSObjectGetProperty(m_obj, propName, &exn);
  if (!property) {
    std::string exceptionText = ChakraValue(exn).toString().str();
    throwJSExecutionException(
        "Failed to get property: %s", exceptionText.c_str());
  }
  return ChakraValue(property);
}

ChakraValue ChakraObject::getPropertyAtIndex(unsigned index) const {
  JsValueRef exn;
  JsValueRef property = JSObjectGetPropertyAtIndex(m_obj, index, &exn);
  if (!property) {
    std::string exceptionText = ChakraValue(exn).toString().str();
    throwJSExecutionException(
        "Failed to get property at index %u: %s", index, exceptionText.c_str());
  }
  return ChakraValue(property);
}

ChakraValue ChakraObject::getProperty(const char *propName) const {
  return getProperty(ChakraString(propName));
}

void ChakraObject::setProperty(
    const ChakraString &propName,
    const ChakraValue &value) const {
  JsValueRef exn = NULL;
  JSObjectSetProperty(m_obj, propName, value, kJSPropertyAttributeNone, &exn);
  if (exn) {
    std::string exceptionText = ChakraValue(exn).toString().str();
    throwJSExecutionException(
        "Failed to set property: %s", exceptionText.c_str());
  }
}

void ChakraObject::setProperty(const char *propName, const ChakraValue &value)
    const {
  setProperty(ChakraString(propName), value);
}

std::vector<ChakraString> ChakraObject::getPropertyNames() const {
  auto namesRef = JSObjectCopyPropertyNames(m_obj);
  auto count = JSPropertyNameArrayGetCount(namesRef);
  std::vector<ChakraString> names;
  names.reserve(count);
  for (decltype(count) i = 0; i < count; i++) {
    names.emplace_back(
        ChakraString::ref(JSPropertyNameArrayGetNameAtIndex(namesRef, i)));
  }
  // PropertyNameArrayRelease(namesRef);
  return names;
}

std::unordered_map<std::string, std::string> ChakraObject::toJSONMap() const {
  std::unordered_map<std::string, std::string> map;
  auto namesRef = JSObjectCopyPropertyNames(m_obj);
  auto count = JSPropertyNameArrayGetCount(namesRef);
  for (decltype(count) i = 0; i < count; i++) {
    auto key =
        ChakraString::ref(JSPropertyNameArrayGetNameAtIndex(namesRef, i));
    map.emplace(key.str(), getProperty(key).toJSONString());
  }
  // PropertyNameArrayRelease(namesRef);
  return map;
}

/* static */
ChakraObject ChakraObject::create() {
  JsValueRef value;
  JsCreateObject(&value);

  return ChakraObject(value);
}

} // namespace react
} // namespace facebook
