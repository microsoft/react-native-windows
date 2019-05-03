// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ChakraValue.h"
#include "ChakraUtils.h"

#include <stdexcept>
#include <algorithm>
#include <functional>

namespace facebook {
namespace react {

class MinimalChakraRuntime
{
public:

  MinimalChakraRuntime(bool multithreaded);

private:

  std::unique_ptr<JsRuntimeHandle, std::function<void(JsRuntimeHandle*)>> runtime;
  std::unique_ptr<JsContextRef, std::function<void(JsContextRef*)>> context;
};

class ChakraJSException : public std::exception {
public:
  explicit ChakraJSException(const char* msg)
    : msg_(msg) {}

  explicit ChakraJSException(JsValueRef exn, const char* msg) {
    buildMessage(exn, nullptr, msg);
  }

  explicit ChakraJSException(JsValueRef exn, JsValueRef sourceURL) {
    buildMessage(exn, sourceURL, nullptr);
  }

  const std::string& getStack() const {
    return stack_;
  }

  virtual const char* what() const noexcept override {
    return msg_.c_str();
  }

private:
  std::string msg_;
  std::string stack_;

  void buildMessage(JsValueRef exn, JsValueRef sourceURL, const char* errorMsg);
};

JsValueRef functionCaller(
  JsContextRef ctx,
  JsValueRef function,
  JsValueRef thisObject,
  size_t argumentCount,
  const JsValueRef arguments[],
  JsValueRef* exception);

inline void throwJSExecutionException(const char* msg) {
  throw ChakraJSException(msg);
}

template <typename... Args>
inline void throwJSExecutionException(const char* fmt, Args... args) {
  int msgSize = snprintf(nullptr, 0, fmt, args...);
  msgSize = std::min(512, msgSize + 1);
  char *msg = (char*)alloca(msgSize);
  snprintf(msg, msgSize, fmt, args...);
  throw ChakraJSException(msg);
}

template <typename... Args>
inline void throwJSExecutionExceptionWithStack(const char* msg, const char* stack) {
  // throw JSException(msg, stack);
}

using ChakraJSFunction = std::function<JsValueRef(JsContextRef, JsValueRef, size_t, const JsValueRef[])>;

JsValueRef makeFunction(
  JsValueRef name,
  ChakraJSFunction function);

JsValueRef makeFunction(
  const char* name,
  ChakraJSFunction function);

void installGlobalFunction(
  const char* name,
  ChakraJSFunction function);

JsValueRef makeFunction(
  const char* name,
  JsNativeFunction callback);

void installGlobalFunction(
  const char* name,
  JsNativeFunction callback);

void removeGlobal(const char* name);

static JsSourceContext getNextSourceContext();

JsValueRef evaluateScript(
  JsValueRef script,
  JsValueRef sourceURL);

JsValueRef evaluateScript(
  std::unique_ptr<const JSBigString>&& script,
  JsValueRef sourceURL);

JsValueRef evaluateScriptWithBytecode(
  std::unique_ptr<const JSBigString>&& script,
  uint64_t scriptVersion,
  JsValueRef scriptFileName,
  std::string&& bytecodeFileName,
  bool asyncBytecodeGeneration = true);

#if WITH_FBJSCEXTENSIONS
JsValueRef evaluateSourceCode(
  JSSourceCodeRef source,
  JsValueRef sourceURL);
#endif

void formatAndThrowJSException(
  JsValueRef exn,
  JsValueRef sourceURL);

JsValueRef translatePendingCppExceptionToJSError(const char *exceptionLocation);
JsValueRef translatePendingCppExceptionToJSError(JsValueRef jsFunctionCause);

JsValueRef GetJSONObject();
int StringGetLength(_In_ JsValueRef string);
JsValueRef JSObjectCallAsFunction(JsValueRef methodJSRef, JsValueRef batchedBridgeRef, unsigned nArgs, const JsValueRef args[], JsValueRef* error);
JsValueRef JSValueMakeFromJSONString(JsValueRef string);
JsValueRef JSValueCreateJSONString(JsValueRef value, unsigned /*indent*/, JsValueRef* error);

// TODO ensure proper clean up of error states in the middle of function
JsValueRef JSObjectGetProperty(JsValueRef object, JsValueRef propertyName, JsValueRef* exception);
JsValueRef JSObjectGetPropertyAtIndex(JsValueRef object, unsigned propertyIndex, JsValueRef* exception);

// Only support None
void JSObjectSetProperty(JsValueRef jsObj, JsValueRef propertyName, JsValueRef value, JSPropertyAttributes /*attributes*/, JsValueRef* exception);

JsValueRef JSObjectCopyPropertyNames(JsValueRef object);
unsigned JSPropertyNameArrayGetCount(JsValueRef namesRef);
JsValueRef JSPropertyNameArrayGetNameAtIndex(JsValueRef namesRef, unsigned idx);

double JSValueToNumber(JsValueRef obj, JsValueRef* exception);
JsValueRef JSValueToStringCopy(JsValueRef obj, JsValueRef* exception);
JsValueRef ValueMakeUndefined();

} }
