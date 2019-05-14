// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraJsiRuntime.h"
#include "UnicodeConversion.h"

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>

namespace facebook { 
namespace jsi {
namespace chakraruntime {

JsWeakRef ChakraJsiRuntime::newWeakObjectRef(const jsi::Object& obj) {
  return objectRef(obj);
}

JsValueRef ChakraJsiRuntime::strongObjectRef(const jsi::WeakObject& obj) {
  return objectRef(obj); // Return the original strong ref.
}


Value ChakraJsiRuntime::evaluateJavaScriptSimple(const jsi::Buffer& buffer, const std::string& sourceURL) {
  const std::wstring script16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(buffer.data()), buffer.size());
  if (script16.empty()) throw jsi::JSINativeException("Script can't be empty.");

  const std::wstring url16 = facebook::react::UnicodeConversion::Utf8ToUtf16(sourceURL);
  if (url16.empty()) throw jsi::JSINativeException("Script URL can't be empty.");

  JsValueRef result;
  checkException(JsRunScript(script16.c_str(), JS_SOURCE_CONTEXT_NONE /*sourceContext*/, url16.c_str(), &result));

  return createValue(result);
}

// TODO :: Return result
bool ChakraJsiRuntime::evaluateSerializedScript(const jsi::Buffer& scriptBuffer, const jsi::Buffer& serializedScriptBuffer, const std::string& sourceURL) {
  std::wstring script16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(scriptBuffer.data()), scriptBuffer.size());
  std::wstring url16 = facebook::react::UnicodeConversion::Utf8ToUtf16(sourceURL);

  // Note:: Bytecode caching on UWP is untested yet.
  JsValueRef result;
  JsErrorCode ret = JsRunSerializedScript(script16.c_str(), const_cast<uint8_t*>(serializedScriptBuffer.data()), 0, url16.c_str(), &result);

  if (ret == JsNoError) {
    return true;
  }
  else if (ret == JsErrorBadSerializedScript) {
    return false;
  }
  else {
    checkException(ret);
    return true;
  }
}

std::unique_ptr<const jsi::Buffer> ChakraJsiRuntime::generatePreparedScript(const std::string& sourceURL, const jsi::Buffer& sourceBuffer) noexcept {
  const std::wstring scriptUTF16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(sourceBuffer.data()), sourceBuffer.size());

  unsigned long bytecodeSize = 0;
  if (JsSerializeScript(scriptUTF16.c_str(), nullptr, &bytecodeSize) == JsNoError)
  {
    std::unique_ptr<ByteArrayBuffer> bytecodeBuffer(std::make_unique<ByteArrayBuffer>(bytecodeSize));
    if (JsSerializeScript(scriptUTF16.c_str(), bytecodeBuffer->data(), &bytecodeSize) == JsNoError)
    {
      return bytecodeBuffer;
    }
  }

  return nullptr;
}

JsValueRef ChakraJsiRuntime::createJSString(const char*data, size_t length) {
  const std::wstring script16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(data), length);
  JsValueRef value;
  JsPointerToString(script16.c_str(), script16.size(), &value);
  return value;
}

JsValueRef ChakraJsiRuntime::createJSPropertyId(const char*data, size_t length) {
  JsValueRef propIdRef;
  const std::wstring name16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(data), length);
  if (JsNoError != JsGetPropertyIdFromName(name16.c_str(), &propIdRef)) std::terminate();
  return propIdRef;
}

void ChakraJsiRuntime::setupNativePromiseContinuation() noexcept {
  // NOP
}

void ChakraJsiRuntime::initRuntimeVersion()  noexcept {
  // NOP
}

std::unique_ptr<jsi::Runtime> makeChakraJsiRuntime(ChakraJsiRuntimeArgs&& args)  noexcept {
  return std::make_unique<ChakraJsiRuntime>(std::move(args));
}

}}}

#endif
