// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Tracing.h>
#include <cxxreact/SystraceSection.h>

#include "ChakraHelpers.h"
#include "ChakraPlatform.h"
#include "ChakraTracing.h"

namespace facebook {
namespace react {

namespace {

INativeTraceHandler *g_nativeTracingHook = nullptr;

#ifdef ENABLE_JS_SYSTRACE

static JsValueRef __stdcall nativeTraceBeginSectionJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void * /*callbackState*/) {
  if (g_nativeTracingHook) {
    // arguments[0]: 'this'
    // arguments[1]: TRACE_TAG_REACT_APPS, constant int (1 << 17)
    // arguments[2]: profileName, string | undefined
    // arguments[3]: argsStr, string | undefined
    assert(argumentCount == 4);

    JsValueType argumentType;

    /* arguments[1]: TRACE_TAG_REACT_APPS */
#ifndef NDEBUG
    CHAKRA_ASSERTDO(JsGetValueType(arguments[1], &argumentType));
    assert(argumentType == JsValueType::JsNumber);
    int traceTagAsInt;
    CHAKRA_ASSERTDO(JsNumberToInt(arguments[1], &traceTagAsInt));
    assert(traceTagAsInt == TRACE_TAG_REACT_APPS);
#endif

    /* arguments[2]: profileName */
    int profileNameLength = 0;
    std::string profileName;
    CHAKRA_ASSERTDO(JsGetValueType(arguments[2], &argumentType));
    if (argumentType == JsValueType::JsString) {
      CHAKRA_ASSERTDO(JsGetStringLength(arguments[2], &profileNameLength));
      profileName.resize(profileNameLength);

      // TODO: CharkaRT doesn't have JsCopyString, implement alternative.
#if !defined(USE_EDGEMODE_JSRT)
      CHAKRA_ASSERTDO(JsCopyString(
          arguments[2],
          const_cast<char *>(profileName.data()),
          profileNameLength + 1 /* length */,
          nullptr /* written */));
#endif
    }
#ifndef NDEBUG
    else {
      assert(argumentType == JsValueType::JsUndefined);
    }
#endif

    /* arguments[3]: argsStr */
    int argsLength = 0;
    std::string argsStr;
    CHAKRA_ASSERTDO(JsGetValueType(arguments[3], &argumentType));
    if (argumentType == JsValueType::JsString) {
      CHAKRA_ASSERTDO(JsGetStringLength(arguments[3], &argsLength));
      argsStr.resize(argsLength + 1);

      // TODO: CharkaRT doesn't have JsCopyString, implement alternative.
#if !defined(USE_EDGEMODE_JSRT)
      CHAKRA_ASSERTDO(JsCopyString(
          arguments[3],
          const_cast<char *>(argsStr.data()),
          argsLength + 1 /* length */,
          nullptr /* written */));
#endif
    }
#ifndef NDEBUG
    else {
      assert(argumentType == JsValueType::JsUndefined);
    }
#endif

    /* Call hook */
    g_nativeTracingHook->JSBeginSection(profileName.c_str(), argsStr.c_str());
  }
  JsValueRef returnValue;
  CHAKRA_ASSERTDO(JsGetUndefinedValue(&returnValue));
  return returnValue;
}

static JsValueRef __stdcall nativeTraceEndSectionJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void * /*callbackState*/) {
  if (g_nativeTracingHook) {
    // arguments[0]: 'this'
    // arguments[1]: TRACE_TAG_REACT_APPS, constant int (1 << 17)
    assert(argumentCount == 2);

    JsValueType argumentType;

    /* arguments[1]: TRACE_TAG_REACT_APPS */
#ifndef NDEBUG
    CHAKRA_ASSERTDO(JsGetValueType(arguments[1], &argumentType));
    assert(argumentType == JsValueType::JsNumber);
    int traceTagAsInt;
    CHAKRA_ASSERTDO(JsNumberToInt(arguments[1], &traceTagAsInt));
    assert(traceTagAsInt == TRACE_TAG_REACT_APPS);
#endif

    /* Call hook */
    g_nativeTracingHook->JSEndSection();
  }
  JsValueRef returnValue;
  CHAKRA_ASSERTDO(JsGetUndefinedValue(&returnValue));
  return returnValue;
}

static void beginOrEndAsync(
    bool isEnd,
    JsValueRef arguments[],
    unsigned short argumentCount) {
  // arguments[0]: 'this'
  // arguments[1]: TRACE_TAG_REACT_APPS, constant int (1 << 17)
  // arguments[2]: profileName, string | undefined
  // arguments[3]: cookie, int
  assert(argumentCount == 4);

  JsValueType argumentType;

  /* arguments[1]: TRACE_TAG_REACT_APPS */
#ifndef NDEBUG
  CHAKRA_ASSERTDO(JsGetValueType(arguments[1], &argumentType));
  assert(argumentType == JsValueType::JsNumber);
  int traceTagAsInt;
  CHAKRA_ASSERTDO(JsNumberToInt(arguments[1], &traceTagAsInt));
  assert(traceTagAsInt == TRACE_TAG_REACT_APPS);
#endif

  /* arguments[2]: profileName */
  int profileNameLength = 0;
  std::string profileName;
  CHAKRA_ASSERTDO(JsGetValueType(arguments[2], &argumentType));
  if (argumentType == JsValueType::JsString) {
    CHAKRA_ASSERTDO(JsGetStringLength(arguments[2], &profileNameLength));
    profileName.resize(profileNameLength);

    // TODO: CharkaRT doesn't have JsCopyString, implement alternative.
#if !defined(USE_EDGEMODE_JSRT)
    CHAKRA_ASSERTDO(JsCopyString(
        arguments[2],
        const_cast<char *>(profileName.data()),
        profileNameLength + 1 /* length */,
        nullptr /* written */));
#endif
  }
#ifndef NDEBUG
  else {
    assert(argumentType == JsValueType::JsUndefined);
  }
#endif

  /* arguments[3]: cookie */
  int cookie;
#ifndef NDEBUG
  CHAKRA_ASSERTDO(JsGetValueType(arguments[3], &argumentType));
  assert(argumentType == JsValueType::JsNumber);
#endif
  CHAKRA_ASSERTDO(JsNumberToInt(arguments[3], &cookie));

  /* Call hook */
  if (isEnd) {
    g_nativeTracingHook->JSEndAsyncSection(profileName.c_str(), cookie);
  } else {
    g_nativeTracingHook->JSBeginAsyncSection(profileName.c_str(), cookie);
  }
}

static JsValueRef __stdcall nativeTraceBeginAsyncSectionJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void * /*callbackState*/) {
  if (g_nativeTracingHook) {
    beginOrEndAsync(false /* isEnd */, arguments, argumentCount);
  }
  JsValueRef returnValue;
  CHAKRA_ASSERTDO(JsGetUndefinedValue(&returnValue));
  return returnValue;
}

static JsValueRef __stdcall nativeTraceEndAsyncSectionJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void * /*callbackState*/) {
  if (g_nativeTracingHook) {
    beginOrEndAsync(true /* isEnd */, arguments, argumentCount);
  }
  JsValueRef returnValue;
  CHAKRA_ASSERTDO(JsGetUndefinedValue(&returnValue));
  return returnValue;
}

static JsValueRef __stdcall nativeTraceCounterJNF(
    JsValueRef /*function*/,
    bool /*isConstructCall*/,
    JsValueRef arguments[],
    unsigned short argumentCount,
    void * /*callbackState*/) {
  if (g_nativeTracingHook) {
    // arguments[0]: 'this'
    // arguments[1]: TRACE_TAG_REACT_APPS, constant int (1 << 17)
    // arguments[2]: profileName, string | undefined
    // arguments[3]: value, int
    assert(argumentCount == 4);

    JsValueType argumentType;

    /* arguments[1]: TRACE_TAG_REACT_APPS */
#ifndef NDEBUG
    CHAKRA_ASSERTDO(JsGetValueType(arguments[1], &argumentType));
    assert(argumentType == JsValueType::JsNumber);
    int traceTagAsInt;
    CHAKRA_ASSERTDO(JsNumberToInt(arguments[1], &traceTagAsInt));
    assert(traceTagAsInt == TRACE_TAG_REACT_APPS);
#endif

    /* arguments[2]: profileName */
    int profileNameLength = 0;
    std::string profileName;
    CHAKRA_ASSERTDO(JsGetValueType(arguments[2], &argumentType));
    if (argumentType == JsValueType::JsString) {
      CHAKRA_ASSERTDO(JsGetStringLength(arguments[2], &profileNameLength));
      profileName.resize(profileNameLength);

      // TODO: CharkaRT doesn't have JsCopyString, implement alternative.
#if !defined(USE_EDGEMODE_JSRT)
      CHAKRA_ASSERTDO(JsCopyString(
          arguments[2],
          const_cast<char *>(profileName.data()),
          profileNameLength + 1 /* length */,
          nullptr /* written */));
#endif
    }
#ifndef NDEBUG
    else {
      assert(argumentType == JsValueType::JsUndefined);
    }
#endif

    /* arguments[3]: value */
    int value;
#ifndef NDEBUG
    CHAKRA_ASSERTDO(JsGetValueType(arguments[3], &argumentType));
    assert(argumentType == JsValueType::JsNumber);
#endif
    CHAKRA_ASSERTDO(JsNumberToInt(arguments[3], &value));

    /* Call hook */
    g_nativeTracingHook->JSCounter(profileName.c_str(), value);
  }
  JsValueRef returnValue;
  CHAKRA_ASSERTDO(JsGetUndefinedValue(&returnValue));
  return returnValue;
}

#endif

} // end anonymous namespace

void SystraceBeginSection(const char *name, const char *args) noexcept {
  if (g_nativeTracingHook) {
    g_nativeTracingHook->NativeBeginSection(name, args);
  }
}

void SystraceEndSection(
    const char *name,
    const char *args,
    std::chrono::nanoseconds duration) noexcept {
  if (g_nativeTracingHook) {
    g_nativeTracingHook->NativeEndSection(name, args, duration);
  }
}

#ifdef ENABLE_JS_SYSTRACE

void addNativeTracingHooks() {
  installGlobalFunction("nativeTraceBeginSection", nativeTraceBeginSectionJNF);
  installGlobalFunction("nativeTraceEndSection", nativeTraceEndSectionJNF);
  installGlobalFunction(
      "nativeTraceBeginAsyncSection", nativeTraceBeginAsyncSectionJNF);
  installGlobalFunction(
      "nativeTraceEndAsyncSection", nativeTraceEndAsyncSectionJNF);
  installGlobalFunction("nativeTraceCounter", nativeTraceCounterJNF);
}

#endif

void InitializeTracing(INativeTraceHandler *handler) {
  g_nativeTracingHook = handler;
}

} // namespace react
} // namespace facebook
