// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraHelpers.h"
#include "ChakraUtils.h"
#include "ChakraValue.h"
#include "Unicode.h"

#ifdef WITH_FBSYSTRACE
#include <fbsystrace.h>
#endif

#include <folly/Memory.h>
#include <folly/String.h>
#include <glog/logging.h>

#include <cxxreact/ReactMarker.h>

#include <windows.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <future>
#include <memory>
#include <string>
#include <thread>
#include <type_traits>

namespace facebook {
namespace react {

MinimalChakraRuntime::MinimalChakraRuntime(bool multithreaded)
    : runtime{new JsRuntimeHandle,
              [](JsRuntimeHandle *h) {
                JsDisposeRuntime(*h);
                delete h;
              }},
      context{new JsContextRef, [](JsContextRef *c) {
                JsSetCurrentContext(JS_INVALID_REFERENCE);
                delete c;
              }} {
  JsErrorCode lastError = JsCreateRuntime(
      multithreaded ? JsRuntimeAttributeNone : JsRuntimeAttributeDisableBackgroundWork, nullptr, &(*runtime));
  assert(lastError == JsNoError && "JsCreateRuntime failed in MinimalChakraRuntime constructor.");

  lastError = JsCreateContext(*runtime, &(*context));
  assert(lastError == JsNoError && "JsCreateContext failed in MinimalChakraRuntime constructor.");

  lastError = JsSetCurrentContext(*context);
  assert(lastError == JsNoError && "JsSetCurrentContext failed in MinimalChakraRuntime constructor.");
}

JsValueRef functionCaller(
    JsContextRef ctx,
    JsValueRef /*function*/,
    JsValueRef thisObject,
    size_t argumentCount,
    const JsValueRef arguments[],
    JsValueRef * /*exception*/) {
  // JsContextRef ctx;
  // JsGetCurrentContext(&ctx);
  void *voidPtr;
  JsGetContextData(ctx, &voidPtr);

  auto *f = static_cast<ChakraJSFunction *>(voidPtr);
  return (*f)(ctx, thisObject, argumentCount, arguments);
}

JsValueRef makeFunction(JsValueRef name, ChakraJSFunction function) {
  // dealloc in kClassDef.finalize
  ChakraJSFunction *functionPtr = new ChakraJSFunction(std::move(function));

  JsValueRef value;
  JsCreateExternalObject(functionPtr, nullptr /*JsFinalizeCallback*/, &value);

  auto functionObject = ChakraObject(value);
  functionObject.setProperty("name", ChakraValue(name));
  return functionObject;
}

void ChakraJSException::buildMessage(JsValueRef exn, JsValueRef sourceURL, const char *errorMsg) {
  std::ostringstream msgBuilder;
  if (errorMsg && strlen(errorMsg) > 0) {
    msgBuilder << errorMsg << ": ";
  }

  ChakraValue exnValue = ChakraValue(exn);
  msgBuilder << exnValue.toString().str();

  // The null/empty-ness of source tells us if the JS came from a
  // file/resource, or was a constructed statement.  The location
  // info will include that source, if any.
  std::string locationInfo = sourceURL != nullptr ? ChakraString::ref(sourceURL).str() : "";
  ChakraObject exnObject = exnValue.asObject();
  auto line = exnObject.getProperty("line");
  if (line != nullptr && line.isNumber()) {
    if (locationInfo.empty() && line.asInteger() != 1) {
      // If there is a non-trivial line number, but there was no
      // location info, we include a placeholder, and the line
      // number.
      locationInfo = folly::to<std::string>("<unknown file>:", line.asInteger());
    } else if (!locationInfo.empty()) {
      // If there is location info, we always include the line
      // number, regardless of its value.
      locationInfo += folly::to<std::string>(":", line.asInteger());
    }
  }

  if (!locationInfo.empty()) {
    msgBuilder << " (" << locationInfo << ")";
  }

  auto exceptionText = msgBuilder.str();
  LOG(ERROR) << "Got JS Exception: " << exceptionText;
  msg_ = std::move(exceptionText);

  ChakraValue jsStack = exnObject.getProperty("stack");
  if (jsStack.isString()) {
    auto stackText = jsStack.toString().str();
    LOG(ERROR) << "Got JS Stack: " << stackText;
    stack_ = std::move(stackText);
  }
}

JsValueRef makeFunction(const char *name, ChakraJSFunction function) {
  return makeFunction(ChakraString(name), std::move(function));
}

void installGlobalFunction(const char *name, ChakraJSFunction function) {
  auto jsName = ChakraString(name);
  auto functionObj = makeFunction(jsName, std::move(function));
  ChakraObject::getGlobalObject().setProperty(jsName, ChakraValue(functionObj));
}

JsValueRef makeFunction(const char *name, JsNativeFunction callback) {
  auto jsName = ChakraString(name);
  JsValueRef functionObj;
  JsCreateNamedFunction(jsName, callback, nullptr /*callbackstate*/, &functionObj);
  return functionObj;
}

void installGlobalFunction(const char *name, JsNativeFunction callback) {
  ChakraString jsName(name);
  JsValueRef functionObj;
  JsCreateNamedFunction(jsName, callback, nullptr /*callbackstate*/, &functionObj);
  ChakraObject::getGlobalObject().setProperty(jsName, ChakraValue(functionObj));
}

void removeGlobal(const char *name) {
  ChakraObject::getGlobalObject().setProperty(name, ChakraValue::makeUndefined());
}

JsSourceContext getNextSourceContext() {
  static JsSourceContext nextSourceContext = 0;

  return nextSourceContext++;
}

JsValueRef evaluateScript(JsValueRef script, JsValueRef source) {
  JsValueRef exn = nullptr;
  JsValueRef value = nullptr;

  const wchar_t *scriptRaw;
  size_t scriptRawLength;
  JsStringToPointer(script, &scriptRaw, &scriptRawLength);

  const wchar_t *sourceRaw;
  size_t sourceRawLength;
  JsStringToPointer(source, &sourceRaw, &sourceRawLength);

  auto result = JsRunScript(scriptRaw, JS_SOURCE_CONTEXT_NONE /*sourceContext*/, sourceRaw, &value);
  bool hasException = false;
  if (result == JsErrorInExceptionState || (JsHasException(&hasException), hasException)) {
    JsGetAndClearException(&exn);
    throw ChakraJSException(exn, source);
  }

  if (value == nullptr) {
    formatAndThrowJSException(exn, source);
  }

  return value;
}

JsValueRef evaluateScript(std::unique_ptr<const JSBigString> &&script, JsValueRef sourceURL) {
  ReactMarker::logMarker(ReactMarker::JS_BUNDLE_STRING_CONVERT_START);
  JsValueRef jsScript = jsStringFromBigString(*script.get());
  ReactMarker::logMarker(ReactMarker::JS_BUNDLE_STRING_CONVERT_STOP);

  return evaluateScript(jsScript, sourceURL);
}

JsValueRef evaluateScriptWithBytecode(
    std::unique_ptr<const JSBigString> &&script,
    [[maybe_unused]] uint64_t scriptVersion,
    JsValueRef scriptFileName,
    [[maybe_unused]] std::string &&bytecodeFileName,
    [[maybe_unused]] bool asyncBytecodeGeneration) {
  // TODO:
  // ChakraRT does not support the JsRunSerialized() API.
  // Hence for UWP implementation, we fall back to using the original source
  // code right now.
  return evaluateScript(std::move(script), scriptFileName);
}

void formatAndThrowJSException(JsValueRef exn, JsValueRef source) {
  ChakraValue exception = ChakraValue(exn);
  std::string exceptionText = exception.toString().str();

  // The null/empty-ness of source tells us if the JS came from a
  // file/resource, or was a constructed statement.  The location
  // info will include that source, if any.
  std::string locationInfo = source != nullptr ? ChakraString::ref(source).str() : "";
  ChakraObject exObject = exception.asObject();
  auto line = exObject.getProperty("line");
  if (line != nullptr && line.isNumber()) {
    if (locationInfo.empty() && line.asInteger() != 1) {
      // If there is a non-trivial line number, but there was no
      // location info, we include a placeholder, and the line
      // number.
      locationInfo = folly::to<std::string>("<unknown file>:", line.asInteger());
    } else if (!locationInfo.empty()) {
      // If there is location info, we always include the line
      // number, regardless of its value.
      locationInfo += folly::to<std::string>(":", line.asInteger());
    }
  }

  if (!locationInfo.empty()) {
    exceptionText += " (" + locationInfo + ")";
  }

  LOG(ERROR) << "Got JS Exception: " << exceptionText;

  ChakraValue jsStack = exObject.getProperty("stack");
  if (jsStack.isNull() || !jsStack.isString()) {
    throwJSExecutionException("%s", exceptionText.c_str());
  } else {
    LOG(ERROR) << "Got JS Stack: " << jsStack.toString().str();
    throwJSExecutionExceptionWithStack(exceptionText.c_str(), jsStack.toString().str().c_str());
  }
}

JsValueRef translatePendingCppExceptionToJSError(const char *exceptionLocation) {
  std::ostringstream msg;
  try {
    throw;
  } catch (const std::bad_alloc &) {
    throw; // We probably shouldn't try to handle this in JS
  } catch (const std::exception &ex) {
    msg << "C++ Exception in '" << exceptionLocation << "': " << ex.what();
    return ChakraValue::makeError(msg.str().c_str());
  } catch (const char *ex) {
    msg << "C++ Exception (thrown as a char*) in '" << exceptionLocation << "': " << ex;
    return ChakraValue::makeError(msg.str().c_str());
  } catch (...) {
    msg << "Unknown C++ Exception in '" << exceptionLocation << "'";
    return ChakraValue::makeError(msg.str().c_str());
  }
}

JsValueRef translatePendingCppExceptionToJSError(JsValueRef jsFunctionCause) {
  try {
    auto functionName = ChakraObject(jsFunctionCause).getProperty("name").toString().str();
    return translatePendingCppExceptionToJSError(functionName.c_str());
  } catch (...) {
    return ChakraValue::makeError("Failed to get function name while handling exception");
  }
}

JsValueRef GetJSONObject() {
  JsValueRef globalObject;
  JsGetGlobalObject(&globalObject);

  JsPropertyIdRef propertyId;
  if (JsNoError != JsGetPropertyIdFromName(L"JSON", &propertyId)) {
    return nullptr;
  }
  JsValueRef jsonObject;
  if (JsNoError != JsGetProperty(globalObject, propertyId, &jsonObject)) {
    return nullptr;
  }
  return jsonObject;
}

int StringGetLength(_In_ JsValueRef string) {
  int length;
  JsGetStringLength(string, &length);
  return length;
}

JsValueRef JSObjectCallAsFunction(
    JsValueRef methodJSRef,
    JsValueRef batchedBridgeRef,
    unsigned nArgs,
    const JsValueRef args[],
    JsValueRef *error) {
  auto argsWithThis = new JsValueRef[nArgs + 1];

  argsWithThis[0] = batchedBridgeRef;
  for (unsigned i = 0; i < nArgs; i++)
    argsWithThis[i + 1] = args[i];

  JsValueRef value;
  auto result = JsCallFunction(
      methodJSRef, const_cast<JsValueRef *>(argsWithThis), static_cast<unsigned short>(nArgs + 1), &value);
  delete[] argsWithThis;
  bool hasException = false;
  if (result != JsNoError || (JsHasException(&hasException), hasException)) {
    JsGetAndClearException(error);
    return nullptr;
  }
  return value;
}

JsValueRef JSValueMakeFromJSONString(JsValueRef string) {
  // Handle special case with Chakra runtime where if you pass an empty string
  // to parseFunction it will throw an exception instead of returning nullptr
  if (StringGetLength(string) == 0) {
    JsValueRef value;
    JsGetNullValue(&value);
    return value;
  }

  auto jsonObject = GetJSONObject();

  JsPropertyIdRef propertyId;
  if (JsNoError != JsGetPropertyIdFromName(L"parse", &propertyId) || !jsonObject) {
    return nullptr;
  }
  JsValueRef parseFunction;
  if (JsNoError != JsGetProperty(jsonObject, propertyId, &parseFunction)) {
    return nullptr;
  }

  JsValueRef error;
  JsValueRef arguments[1] = {string};
  return JSObjectCallAsFunction(parseFunction, jsonObject, 1, arguments, &error);
}

JsValueRef JSValueCreateJSONString(JsValueRef value, unsigned /*indent*/, JsValueRef *error) {
  auto jsonObject = GetJSONObject();

  JsPropertyIdRef propertyId;
  if (JsNoError != JsGetPropertyIdFromName(L"stringify", &propertyId) || !jsonObject) {
    JsGetAndClearException(error);
    return nullptr;
  }
  JsValueRef stringifyFunction;
  if (JsNoError != JsGetProperty(jsonObject, propertyId, &stringifyFunction)) {
    JsGetAndClearException(error);
    return nullptr;
  }

  JsValueRef arguments[1] = {value};
  return JSObjectCallAsFunction(stringifyFunction, jsonObject, 1, arguments, error);
}

// TODO ensure proper clean up of error states in the middle of function
JsValueRef JSObjectGetProperty(JsValueRef object, JsValueRef propertyName, JsValueRef *exception) {
  std::string propName;

  auto result = JsStringToStdStringUtf8(propertyName, propName);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  result = JsGetPropertyIdFromNameUtf8(propName.c_str(), &propId);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);

  JsValueRef value;
  result = JsGetProperty(object, propId, &value);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
  return value;
}

JsValueRef JSObjectGetPropertyAtIndex(JsValueRef object, unsigned propertyIndex, JsValueRef *exception) {
  JsValueRef index;
  JsIntToNumber(propertyIndex, &index);
  JsValueRef property;
  auto result = JsGetIndexedProperty(object, index, &property);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);

  return property;
}

// Only support None
void JSObjectSetProperty(
    JsValueRef jsObj,
    JsValueRef propertyName,
    JsValueRef value,
    JSPropertyAttributes /*attributes*/,
    JsValueRef *exception) {
  // TODO exception handing -- ensure proper clean up if failure in middle of
  // function
  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  std::string propName;

  auto result = JsStringToStdStringUtf8(propertyName, propName);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
  result = JsGetPropertyIdFromNameUtf8(propName.c_str(), &propId);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);

  // TODO - should we use strict rules?
  result = JsSetProperty(jsObj, propId, value, true /*useStrictRules*/);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
}

JsValueRef JSObjectCopyPropertyNames(JsValueRef object) {
  JsValueRef propertyNamesArrayRef;
  JsGetOwnPropertyNames(object, &propertyNamesArrayRef);
  return propertyNamesArrayRef;
}

unsigned JSPropertyNameArrayGetCount(JsValueRef namesRef) {
  JsPropertyIdRef propertyId;
  JsGetPropertyIdFromName(L"length", &propertyId);
  JsValueRef countRef;
  JsGetProperty(namesRef, propertyId, &countRef);
  int count;
  JsNumberToInt(countRef, &count);
  return count;
}

JsValueRef JSPropertyNameArrayGetNameAtIndex(JsValueRef namesRef, unsigned idx) {
  JsValueRef index;
  JsIntToNumber(idx, &index);
  JsValueRef propertyName;
  JsGetIndexedProperty(namesRef, index, &propertyName);
  return propertyName;
}

double JSValueToNumber(JsValueRef obj, JsValueRef *exception) {
  double value;
  auto result = JsNumberToDouble(obj, &value);
  if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
  return value;
}

JsValueRef JSValueToStringCopy(JsValueRef obj, JsValueRef *exception) {
  JsValueRef value;
  auto result = JsConvertValueToString(obj, &value);
  if (JsNoError == result)
    JsAddRef(value, nullptr); // TODO is this the right lifetime symantics?
  else if (result == JsErrorScriptException)
    JsGetAndClearException(exception);
  return value;
}

JsValueRef ValueMakeUndefined() {
  JsValueRef value = nullptr;
  JsGetUndefinedValue(&value);
  return value;
}

} // namespace react
} // namespace facebook
