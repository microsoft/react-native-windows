// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <SystemChakraRuntime.h>
#include <ChakraRuntimeFactory.h>

#include "ByteArrayBuffer.h"
#include "Unicode.h"

// From <chakrart.h>
STDAPI_(JsErrorCode)
JsStartDebugging();

namespace Microsoft::JSI {

SystemChakraRuntime::SystemChakraRuntime(ChakraRuntimeArgs&& args) noexcept : ChakraRuntime(std::move(args)) {}

void SystemChakraRuntime::setupNativePromiseContinuation() noexcept {
  // NOP
}

void SystemChakraRuntime::startDebuggingIfNeeded() {
  if (runtimeArgs().enableDebugging)
    JsStartDebugging();
}

void SystemChakraRuntime::stopDebuggingIfNeeded() {
  // NOP AFAIK
}

std::unique_ptr<const facebook::jsi::Buffer> SystemChakraRuntime::generatePreparedScript(
    const std::string &sourceURL,
    const facebook::jsi::Buffer &sourceBuffer) noexcept {
  const std::wstring scriptUTF16 =
      Microsoft::Common::Unicode::Utf8ToUtf16(reinterpret_cast<const char *>(sourceBuffer.data()), sourceBuffer.size());

  //TODO: Use long when using System Chakra!
  unsigned long bytecodeSize = 0;
  if (JsSerializeScript(scriptUTF16.c_str(), nullptr, (unsigned int*)&bytecodeSize) == JsNoError) {
    std::unique_ptr<ByteArrayBuffer> bytecodeBuffer(std::make_unique<ByteArrayBuffer>(bytecodeSize));
    if (JsSerializeScript(scriptUTF16.c_str(), bytecodeBuffer->data(), (unsigned int *)&bytecodeSize) == JsNoError) {
      return bytecodeBuffer;
    }
  }

  return nullptr;
}

facebook::jsi::Value SystemChakraRuntime::evaluateJavaScriptSimple(
    const facebook::jsi::Buffer &buffer,
    const std::string &sourceURL) {
  const std::wstring script16 =
      Microsoft::Common::Unicode::Utf8ToUtf16(reinterpret_cast<const char *>(buffer.data()), buffer.size());
  if (script16.empty())
    throw facebook::jsi::JSINativeException("Script can't be empty.");

  const std::wstring url16 = Microsoft::Common::Unicode::Utf8ToUtf16(sourceURL);

  JsValueRef result;
  VerifyJsErrorElseThrow(
      JsRunScript(script16.c_str(), JS_SOURCE_CONTEXT_NONE /*sourceContext*/, url16.c_str(), &result));

  return ToJsiValue(ChakraObjectRef(result));
}

// TODO :: Return result
bool SystemChakraRuntime::evaluateSerializedScript(
    const facebook::jsi::Buffer &scriptBuffer,
    const facebook::jsi::Buffer &serializedScriptBuffer,
    const std::string &sourceURL) {
  std::wstring script16 =
      Microsoft::Common::Unicode::Utf8ToUtf16(reinterpret_cast<const char *>(scriptBuffer.data()), scriptBuffer.size());
  std::wstring url16 = Microsoft::Common::Unicode::Utf8ToUtf16(sourceURL);

  // Note:: Bytecode caching on UWP is untested yet.
  JsValueRef result;
  JsErrorCode ret = JsRunSerializedScript(
      script16.c_str(), const_cast<uint8_t *>(serializedScriptBuffer.data()), 0, url16.c_str(), &result);

  if (ret == JsNoError) {
    return true;
  } else if (ret == JsErrorBadSerializedScript) {
    return false;
  } else {
    VerifyChakraErrorElseThrow(ret);
    return true;
  }
}

std::unique_ptr<facebook::jsi::Runtime> MakeSystemChakraRuntime(ChakraRuntimeArgs&& args) noexcept
{
  return std::make_unique<SystemChakraRuntime>(std::move(args));
  // return std::unique_ptr<facebook::jsi::Runtime>(new SystemChakraRuntime(std::move(args)));
}

} // namespace Microsoft::JSI

//#endif
