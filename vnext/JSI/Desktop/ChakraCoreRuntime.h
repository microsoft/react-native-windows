// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#ifdef CHAKRACORE

#include "ChakraRuntime.h"

#include <cstdint>
#include <mutex>

namespace facebook::jsi::chakra {

class ChakraCoreRuntime : public ChakraRuntime {
 public:
  class ChakraCorePreparedJavaScript
      : public facebook::jsi::PreparedJavaScript {
   public:
    // This constructor calls JsSerialize and initializes m_bytecode with the
    // returned JS ArrayBuffer. Note that the returned
    // ChakraCorePreparedJavaScript keeps a raw reference to runtime through its
    // m_bytecodeBuffer, so it can only be used while runtime is alive.
    // Keeping a shared_ptr to runtime here is an alternative, but this
    // constructor is called by ChakraCoreRuntime::prepareJavaScript, which
    // means we would have to derive ChakraCoreRuntime from
    // std::enable_shared_from_this. Doing so would require ChakraCoreRuntime to
    // be managed by shared_ptrs only, which conflicts with our usage of
    // unique_ptr<Runtime>'s. For now, usage of ChakraCorePreparedJavaScript is
    // sparse enough that a raw reference is reasonable to manage. We can
    // revisit this when the need arises.
    ChakraCorePreparedJavaScript(
        ChakraCoreRuntime &runtime,
        const std::string &sourceCodeUrl,
        const std::shared_ptr<const Buffer> &sourceCodeBuffer);

    // This constructor does not call JsSerialize. It initializes m_bytecode
    // with a JS ExternalArrayBuffer backed by sourceCodeBuffer.
    ChakraCorePreparedJavaScript(
        ChakraCoreRuntime &runtime,
        const std::string &sourceCodeUrl,
        const std::shared_ptr<const Buffer> &sourceCodeBuffer,
        const std::shared_ptr<const Buffer> &bytecodeBuffer);

    const ChakraObjectRef &source() const;
    const ChakraObjectRef &sourceUrl() const;

    std::shared_ptr<const Buffer> bytecodeBuffer() const;
    const ChakraObjectRef &bytecode() const;

   private:
    ChakraObjectRef m_sourceUrl; // A JS string.
    ChakraObjectRef m_source; // A JS ExternalArrayBuffer.

    ChakraObjectRef m_bytecode; // A JS ArrayBuffer or ExternalArrayBuffer.
    std::shared_ptr<const Buffer> m_bytecodeBuffer;
  };

  using ChakraRuntime::ChakraRuntime;

  //============================================================================
  // Below are functions inherited from Runtime.
  // Note that the performance characteristics detailed by the comments in jsi.h
  // is for HermesRuntime and might not apply to ChakraCoreRuntime.
  // TODO (yicyao): Document the performance characteristics of
  // ChakraCoreRuntime.

  // Buffer must contain UTF-8 encoded JavaScript source code.
  Value evaluateJavaScript(
      const std::shared_ptr<const Buffer> &buffer,
      const std::string &sourceUrl) override;

  // The returned PreparedJavaScript keeps a raw reference to this
  // ChakraCoreRuntime, so users of this function need to make sure that this
  // ChkaraCoreRuntime remains alive when the returned PreparedJavaScript is
  // being used.
  std::shared_ptr<const PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const Buffer> &buffer,
      std::string sourceUrl) override;

  // js must point to a ChakraCorePreparedJavaScript.
  Value evaluatePreparedJavaScript(
      const std::shared_ptr<const PreparedJavaScript> &js) override;

  std::string description() override;

  // We override the following functions because we can avoid a UTF-8 to UTF-16
  // conversion using ChakraCore APIs.
  PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length)
      override;
  std::string utf8(const String &str) override;

  // We override the following functions because we can avoid a string copy
  // using ChakraCore APIs.
  Value getProperty(const Object &obj, const String &name) override;
  bool hasProperty(const Object &obj, const String &name) override;
  void setPropertyValue(Object &obj, const String &name, const Value &value)
      override;

  // We override the following because ChakraCore (unlike Chakra) offers
  // weak reference semantics.
  WeakObject createWeakObject(const Object &obj) override;
  Value lockWeakObject(const WeakObject &weakObj) override;

  // Above are functions inherited from Runtime.
  //============================================================================

 private:
  using WeakChakraPointerValue = ChakraPointerValueTemplate<JsWeakRef>;

  // We override this function because we can avoid a UTF-8 to UTF-16 conversion
  // using ChakraCore APIs.
  ChakraObjectRef toJsString(const char *utf8, size_t length) override;

  ChakraObjectRef toJsExternalArrayBuffer(
      const std::shared_ptr<const Buffer> &buffer);

  // source must be a JS ArrayBuffer or JS ExternalArrayBuffer. sourceUrl must
  // be a JS string.
  Value evaluateJavaScriptImpl(
      const ChakraObjectRef &source,
      const ChakraObjectRef &sourceUrl);

  class ChakraCoreBytecodeBuffer final : public Buffer {
   public:
    // buffer must be an ArrayBuffer or an ExternalArrayBuffer
    ChakraCoreBytecodeBuffer(
        ChakraCoreRuntime &runtime,
        const ChakraObjectRef &buffer)
        : m_runtime(runtime),
          m_buffer(make<Object>(new ChakraPointerValue(buffer))
                       .getArrayBuffer(runtime)) {}

    size_t size() const override;

    const uint8_t *data() const override;

   private:
    ChakraCoreRuntime &m_runtime;
    ArrayBuffer m_buffer;
  };

}; // class ChakraCoreRuntime

} // namespace facebook::jsi::chakra

#endif // #ifdef CHAKRACORE
