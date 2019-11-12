// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifndef CHAKRACORE

#include "ChakraRtRuntime.h"

#include "ByteArrayBuffer.h"
#include "Unicode.h"
#include "Utilities.h"

#include <jsrt.h>

namespace Microsoft::JSI {

facebook::jsi::Value ChakraRtRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJs) {
  assert(preparedJs);

  std::shared_ptr<const ChakraPreparedJavaScript> chakraPreparedJs =
      std::static_pointer_cast<const ChakraPreparedJavaScript>(preparedJs);

  JsValueRef result = nullptr;

  std::wstring sourceCodeUtf16 = Common::Unicode::Utf8ToUtf16(
      Common::Utilities::CheckedReinterpretCast<const char *>(chakraPreparedJs->SourceCode()->data()),
      chakraPreparedJs->SourceCode()->size());

  std::wstring sourceUrlUtf16 = Common::Unicode::Utf8ToUtf16(chakraPreparedJs->SourceUrl());

  // We have to use a const_cast here due to JsRunSerializedScript taking in a
  // BYTE* instead of a const BYTE*.
  JsErrorCode error = JsRunSerializedScript(
      sourceCodeUtf16.c_str(),
      static_cast<BYTE *>(const_cast<uint8_t *>(chakraPreparedJs->Bytecode()->data())),
      JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
      sourceUrlUtf16.c_str(),
      &result);

  // Fall back to evaluating source code if bytecode evaluation fails.
  if (error == JsErrorBadSerializedScript) {
    JsValueRef exception = nullptr;
    VerifyJsErrorElseThrow(JsGetAndClearException(&exception));
    return EvaluateJavaScriptImpl(sourceCodeUtf16.c_str(), sourceUrlUtf16.c_str());
  }

  VerifyJsErrorElseThrow(error);
  return ToJsiValue(ChakraObjectRef(result));
}

std::string ChakraRtRuntime::description() {
  return "ChakraRtRuntime";
}

facebook::jsi::WeakObject ChakraRtRuntime::createWeakObject(const facebook::jsi::Object &object) {
  return make<facebook::jsi::WeakObject>(CloneChakraPointerValue(getPointerValue(object)));
}

facebook::jsi::Value ChakraRtRuntime::lockWeakObject(const facebook::jsi::WeakObject &weakObject) {
  // We need to make a copy of the ChakraObjectRef held within weakObj's
  // member PointerValue for the returned jsi::Value here.
  ChakraObjectRef ref = GetChakraObjectRef(weakObject);
  return ToJsiValue(std::move(ref));
}

facebook::jsi::Value ChakraRtRuntime::EvaluateJavaScriptSimple(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceUrl) {
  assert(buffer);

  const std::wstring sourceCodeUtf16 = Common::Unicode::Utf8ToUtf16(
      Common::Utilities::CheckedReinterpretCast<const char *>(buffer->data()), buffer->size());

  const std::wstring sourceUrlUtf16 = Common::Unicode::Utf8ToUtf16(sourceUrl);

  return EvaluateJavaScriptImpl(sourceCodeUtf16.c_str(), sourceUrlUtf16.c_str());
}

std::shared_ptr<const facebook::jsi::Buffer> ChakraRtRuntime::SerializeScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
  assert(buffer);

  std::wstring sourceCodeUtf16 = Common::Unicode::Utf8ToUtf16(
      Common::Utilities::CheckedReinterpretCast<const char *>(buffer->data()), buffer->size());

  unsigned long bytecodeSize = 0;
  VerifyJsErrorElseThrow(JsSerializeScript(sourceCodeUtf16.c_str(), nullptr, &bytecodeSize));

  assert(
      static_cast<unsigned long long>(bytecodeSize) <=
      static_cast<unsigned long long>((std::numeric_limits<size_t>::max)()));

  std::shared_ptr<ByteArrayBuffer> bytecode = std::make_shared<ByteArrayBuffer>(static_cast<size_t>(bytecodeSize));

  VerifyJsErrorElseThrow(
      JsSerializeScript(sourceCodeUtf16.c_str(), static_cast<BYTE *>(bytecode->data()), &bytecodeSize));

  assert(
      static_cast<unsigned long long>(bytecodeSize) <=
      static_cast<unsigned long long>((std::numeric_limits<size_t>::max)()));

  if (static_cast<size_t>(bytecodeSize) != bytecode->size()) {
    throw facebook::jsi::JSINativeException("Unknown error occurred when calling JsSerializeScript.");
  }

  return bytecode;
}

void ChakraRtRuntime::StartDebugging() {
  VerifyJsErrorElseThrow(JsStartDebugging());
}

facebook::jsi::Value ChakraRtRuntime::EvaluateJavaScriptImpl(const wchar_t *sourceCode, const wchar_t *sourceUrl) {
  JsValueRef result;
  VerifyJsErrorElseThrow(JsRunScript(sourceCode, JS_SOURCE_CONTEXT_NONE /*sourceContext*/, sourceUrl, &result));
  return ToJsiValue(ChakraObjectRef(result));
}

} // namespace Microsoft::JSI

#endif
