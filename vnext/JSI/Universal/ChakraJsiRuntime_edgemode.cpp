// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "ChakraRuntime.h"
#include "unicode.h"

#if !defined(CHAKRACORE)
#include <jsrt.h>

namespace Microsoft::JSI {

void ChakraRuntime::startDebuggingIfNeeded() {
  if (runtimeArgs().enableDebugging)
    JsStartDebugging();
}

void ChakraRuntime::stopDebuggingIfNeeded() {
  // NOP AFAIK
}

JsWeakRef ChakraRuntime::newWeakObjectRef(const facebook::jsi::Object &obj) {
  return objectRef(obj);
}

JsValueRef ChakraRuntime::strongObjectRef(
    const facebook::jsi::WeakObject &obj) {
  return objectRef(obj); // Return the original strong ref.
}

facebook::jsi::Value ChakraRuntime::evaluateJavaScriptSimple(
    const facebook::jsi::Buffer &buffer,
    const std::string &sourceURL) {
  const std::wstring script16 = facebook::react::unicode::utf8ToUtf16(
      reinterpret_cast<const char *>(buffer.data()), buffer.size());
  if (script16.empty())
    throw facebook::jsi::JSINativeException("Script can't be empty.");

  const std::wstring url16 = facebook::react::unicode::utf8ToUtf16(sourceURL);
  if (url16.empty())
    throw facebook::jsi::JSINativeException("Script URL can't be empty.");

  JsValueRef result;
  checkException(JsRunScript(
      script16.c_str(),
      JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
      url16.c_str(),
      &result));

  return createValue(result);
}

// TODO :: Return result
bool ChakraRuntime::evaluateSerializedScript(
    const facebook::jsi::Buffer &scriptBuffer,
    const facebook::jsi::Buffer &serializedScriptBuffer,
    const std::string &sourceURL) {
  std::wstring script16 = facebook::react::unicode::utf8ToUtf16(
      reinterpret_cast<const char *>(scriptBuffer.data()), scriptBuffer.size());
  std::wstring url16 = facebook::react::unicode::utf8ToUtf16(sourceURL);

  // Note:: Bytecode caching on UWP is untested yet.
  JsValueRef result;
  JsErrorCode ret = JsRunSerializedScript(
      script16.c_str(),
      const_cast<uint8_t *>(serializedScriptBuffer.data()),
      0,
      url16.c_str(),
      &result);

  if (ret == JsNoError) {
    return true;
  } else if (ret == JsErrorBadSerializedScript) {
    return false;
  } else {
    checkException(ret);
    return true;
  }
}

std::unique_ptr<const facebook::jsi::Buffer>
ChakraRuntime::generatePreparedScript(
    const std::string &sourceURL,
    const facebook::jsi::Buffer &sourceBuffer) noexcept {
  const std::wstring scriptUTF16 = facebook::react::unicode::utf8ToUtf16(
      reinterpret_cast<const char *>(sourceBuffer.data()), sourceBuffer.size());

  unsigned long bytecodeSize = 0;
  if (JsSerializeScript(scriptUTF16.c_str(), nullptr, &bytecodeSize) ==
      JsNoError) {
    std::unique_ptr<ByteArrayBuffer> bytecodeBuffer(
        std::make_unique<ByteArrayBuffer>(bytecodeSize));
    if (JsSerializeScript(
            scriptUTF16.c_str(), bytecodeBuffer->data(), &bytecodeSize) ==
        JsNoError) {
      return bytecodeBuffer;
    }
  }

  return nullptr;
}

JsValueRef ChakraRuntime::createJSString(const char *data, size_t length) {
  const std::wstring script16 = facebook::react::unicode::utf8ToUtf16(
      reinterpret_cast<const char *>(data), length);
  JsValueRef value;
  JsPointerToString(script16.c_str(), script16.size(), &value);
  return value;
}

JsValueRef ChakraRuntime::createJSPropertyId(const char *data, size_t length) {
  JsValueRef propIdRef;
  const std::wstring name16 = facebook::react::unicode::utf8ToUtf16(
      reinterpret_cast<const char *>(data), length);
  if (JsNoError != JsGetPropertyIdFromName(name16.c_str(), &propIdRef))
    std::terminate();
  return propIdRef;
}

void ChakraRuntime::setupNativePromiseContinuation() noexcept {
  // NOP
}

void ChakraRuntime::initRuntimeVersion() noexcept {
  // NOP
}

} // namespace Microsoft::JSI

#endif
