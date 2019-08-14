// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifdef CHAKRACORE

#include "ChakraCoreRuntime.h"

#include "utilities.h"

#include <windows.h>

#include <cstdint>
#include <mutex>

namespace facebook::jsi::chakra {

//==============================================================================
// ChakraCoreRuntime implementation.
//==============================================================================

ChakraCoreRuntime::ChakraCorePreparedJavaScript::ChakraCorePreparedJavaScript(
    ChakraCoreRuntime &runtime,
    const std::string &sourceCodeUrl,
    const std::shared_ptr<const Buffer> &sourceCodeBuffer)
    : m_sourceUrl(
          runtime.toJsString(sourceCodeUrl.c_str(), sourceCodeUrl.length())),
      m_source(runtime.toJsExternalArrayBuffer(sourceCodeBuffer)),
      m_bytecodeBuffer(nullptr),
      m_bytecode() {
  JsValueRef bytecode = nullptr;

  runtime.throwUponJsError(
      JsSerialize(m_source, &bytecode, JsParseScriptAttributeNone),
      "JsSerialize");

  m_bytecode.initialize(bytecode);
  m_bytecodeBuffer =
      std::make_shared<const ChakraCoreBytecodeBuffer>(runtime, m_bytecode);
}

ChakraCoreRuntime::ChakraCorePreparedJavaScript::ChakraCorePreparedJavaScript(
    ChakraCoreRuntime &runtime,
    const std::string &sourceCodeUrl,
    const std::shared_ptr<const Buffer> &sourceCodeBuffer,
    const std::shared_ptr<const Buffer> &bytecodeBuffer)
    : m_sourceUrl(
          runtime.toJsString(sourceCodeUrl.c_str(), sourceCodeUrl.length())),
      m_source(runtime.toJsExternalArrayBuffer(sourceCodeBuffer)),
      m_bytecode(runtime.toJsExternalArrayBuffer(bytecodeBuffer)),
      m_bytecodeBuffer(bytecodeBuffer) {}

const ChakraObjectRef &ChakraCoreRuntime::ChakraCorePreparedJavaScript::source()
    const {
  return m_source;
}

const ChakraObjectRef &
ChakraCoreRuntime::ChakraCorePreparedJavaScript::sourceUrl() const {
  return m_sourceUrl;
}

std::shared_ptr<const Buffer>
ChakraCoreRuntime::ChakraCorePreparedJavaScript::bytecodeBuffer() const {
  return std::static_pointer_cast<const Buffer>(m_bytecodeBuffer);
}

const ChakraObjectRef &
ChakraCoreRuntime::ChakraCorePreparedJavaScript::bytecode() const {
  return m_bytecode;
}

//==============================================================================
// Below are functions inherited from Runtime.

Value ChakraCoreRuntime::evaluateJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    const std::string &sourceUrl) {
  ChakraObjectRef jsBuffer = toJsExternalArrayBuffer(buffer);

  ChakraObjectRef jsSourceUrl =
      toJsString(sourceUrl.c_str(), sourceUrl.length());

  return evaluateJavaScriptImpl(jsBuffer, jsSourceUrl);
}

std::shared_ptr<const PreparedJavaScript> ChakraCoreRuntime::prepareJavaScript(
    const std::shared_ptr<const Buffer> &buffer,
    std::string sourceUrl) {
  auto result = std::make_shared<const ChakraCorePreparedJavaScript>(
      *this, sourceUrl, buffer);
  return std::static_pointer_cast<const PreparedJavaScript>(result);
}

Value ChakraCoreRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const PreparedJavaScript> &js) {
  auto preparedJs =
      std::static_pointer_cast<const ChakraCorePreparedJavaScript>(js);

  JsValueRef result = nullptr;
  JsErrorCode error = JsRunSerialized(
      preparedJs->bytecode(),
      [](JsSourceContext sourceContext,
         JsValueRef *value,
         JsParseScriptAttributes *parseAttributes) -> bool {
        *value = reinterpret_cast<JsValueRef>(sourceContext);
        *parseAttributes = JsParseScriptAttributeNone;
        return true;
      },
      reinterpret_cast<JsSourceContext>(JsValueRef(preparedJs->source())),
      preparedJs->sourceUrl(),
      &result);

  if (error == JsErrorBadSerializedScript) {
    JsValueRef exception = nullptr;
    JsGetAndClearException(&exception);
    return evaluateJavaScriptImpl(
        preparedJs->source(), preparedJs->sourceUrl());
  }

  throwUponJsError(error, "JsRunSerialized");
  return toJsiValue(ChakraObjectRef(result));
}

std::string ChakraCoreRuntime::description() {
  return "ChakraCoreRuntime";
}

PropNameID ChakraCoreRuntime::createPropNameIDFromUtf8(
    const uint8_t *utf8,
    size_t length) {
  JsPropertyIdRef id = nullptr;
  throwUponJsError(
      JsCreatePropertyId(
          react::utilities::checkedReinterpretCast<const char *>(utf8),
          length,
          &id),
      "JsCreatePropertyId");
  return make<PropNameID>(new ChakraPointerValue(ChakraObjectRef(id)));
}

std::string ChakraCoreRuntime::utf8(const String &str) {
  size_t length;
  throwUponJsError(
      JsCopyString(getChakraObjectRef(str), nullptr, 0, &length),
      "JsCopyString");

  std::string result{};
  result.resize(length);

  size_t writtenLength;
  throwUponJsError(
      JsCopyString(
          getChakraObjectRef(str), result.data(), length, &writtenLength),
      "JsCopyString");

  assert(length == writtenLength);
  return result;
}

Value ChakraCoreRuntime::getProperty(const Object &obj, const String &name) {
  JsValueRef result;
  throwUponJsError(
      JsObjectGetProperty(
          getChakraObjectRef(obj), getChakraObjectRef(name), &result),
      "JsObjectGetProperty");
  return toJsiValue(ChakraObjectRef(result));
}

bool ChakraCoreRuntime::hasProperty(const Object &obj, const String &name) {
  bool result;
  throwUponJsError(
      JsObjectHasProperty(
          getChakraObjectRef(obj), getChakraObjectRef(name), &result),
      "JsObjectHasProperty");
  return result;
}

void ChakraCoreRuntime::setPropertyValue(
    Object &obj,
    const String &name,
    const Value &value) {
  // TODO (yicyao): Need to figure out whether we should follow strict mode
  // rules here or not.
  throwUponJsError(
      JsObjectSetProperty(
          getChakraObjectRef(obj),
          getChakraObjectRef(name),
          toChakraObjectRef(value),
          false),
      "JsObjectSetProperty");
}

WeakObject ChakraCoreRuntime::createWeakObject(const Object &obj) {
  JsWeakRef weakRef = nullptr;
  throwUponJsError(
      JsCreateWeakReference(getChakraObjectRef(obj), &weakRef),
      "JsCreateWeakReference");
  return make<WeakObject>(new WeakChakraPointerValue(weakRef));
}

Value ChakraCoreRuntime::lockWeakObject(const WeakObject &weakObj) {
  JsValueRef ref = nullptr;

  throwUponJsError(
      JsGetWeakReferenceValue(
          static_cast<const WeakChakraPointerValue *>(getPointerValue(weakObj))
              ->getRef(),
          &ref),
      "JsGetWeakReferenceValue");

  if (ref == JS_INVALID_REFERENCE) {
    return Value{};
  }

  return toJsiValue(ChakraObjectRef(ref));
}

// Above are functions inherited from Runtime.
//==============================================================================

ChakraObjectRef ChakraCoreRuntime::toJsString(const char *utf8, size_t length) {
  JsValueRef str;
  throwUponJsError(JsCreateString(utf8, length, &str), "JsCreateString");
  return ChakraObjectRef(str);
}

ChakraObjectRef ChakraCoreRuntime::toJsExternalArrayBuffer(
    const std::shared_ptr<const Buffer> &buffer) {
  size_t size = buffer->size();
  assert(size < UINT_MAX);

  JsValueRef arrayBuffer = nullptr;
  auto bufferRef = std::make_unique<std::shared_ptr<const Buffer>>(buffer);

  // We allocate a copy of buffer on the heap, a shared_ptr which is deleted
  // when the JavaScript garbage collecotr releases the created external array
  // buffer. This ensures that buffer stays alive while the JavaScript engine is
  // using it.
  throwUponJsError(
      JsCreateExternalArrayBuffer(
          react::utilities::checkedReinterpretCast<char *>(
              const_cast<uint8_t *>(buffer->data())),
          static_cast<unsigned int>(size),
          [](void *bufferToDestroy) {
            // We wrap bufferToDestroy in a unique_ptr to avoid calling delete
            // explicitly.
            std::unique_ptr<std::shared_ptr<const Buffer>> wrapper{
                static_cast<std::shared_ptr<const Buffer> *>(bufferToDestroy)};
          },
          bufferRef.get(),
          &arrayBuffer),
      "JsCreateExternalArrayBuffer");

  bufferRef.release();

  return ChakraObjectRef(arrayBuffer);
}

Value ChakraCoreRuntime::evaluateJavaScriptImpl(
    const ChakraObjectRef &source,
    const ChakraObjectRef &sourceUrl) {
  JsValueRef result = nullptr;

  throwUponJsError(
      JsRun(
          source,
          JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
          sourceUrl,
          JsParseScriptAttributes::JsParseScriptAttributeNone,
          &result),
      "JsRun");

  return toJsiValue(ChakraObjectRef(result));
}

size_t ChakraCoreRuntime::ChakraCoreBytecodeBuffer::size() const {
  return m_runtime.size(m_buffer);
}

const uint8_t *ChakraCoreRuntime::ChakraCoreBytecodeBuffer::data() const {
  return m_runtime.data(m_buffer);
}

} // namespace facebook::jsi::chakra

#endif
