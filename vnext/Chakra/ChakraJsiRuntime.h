// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#if defined(USE_EDGEMODE_JSRT)
#include <jsrt.h>
#else
#include <ChakraCore.h>
#endif

#include "ChakraJsiRuntimeArgs.h"
#include "UnicodeConversion.h"

#include <jsi/jsi.h>

#include <memory>
#include <mutex> 
#include <sstream>

namespace facebook { 
namespace jsi { 
namespace chakraruntime {

class ByteArrayBuffer : public jsi::Buffer {
public:
  size_t size() const override {
    return size_;
  }

  const uint8_t* data() const {
    return byteArray_.get();
  }

  uint8_t* data() {
    return byteArray_.get();
  }

  ByteArrayBuffer(int size) : size_(size), byteArray_(std::make_unique<uint8_t[]>(size)) {}

private:
  int size_;
  std::unique_ptr<uint8_t[]> byteArray_;
};

class ChakraJsiRuntime : public jsi::Runtime {
public:
  ChakraJsiRuntime(ChakraJsiRuntimeArgs&& args) noexcept;
  ~ChakraJsiRuntime() noexcept;

  void evaluateJavaScript(std::unique_ptr<const jsi::Buffer> buffer, const std::string& sourceURL) override;
  jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  void setDescription(const std::string& desc);

  // JsValueRef->JSValue (needs make.*Value so it must be member function)
  jsi::Value createValue(JsValueRef value) const;

  // Value->JsValueRef (similar to above)
  JsValueRef valueRef(const jsi::Value& value);

protected:
  ChakraJsiRuntimeArgs& runtimeArgs() {
    return m_args;
  }

private:
  static void CALLBACK PromiseContinuationCallback(JsValueRef funcRef, void* callbackState) noexcept;
  void promiseContinuation(JsValueRef value)  noexcept;

  void setupNativePromiseContinuation() noexcept;
  void setupMemoryTracker() noexcept;

private:
  // Arguments shared by the specializations ..
  ChakraJsiRuntimeArgs m_args;

  // Note :: For simplicity, We are pinning the script and serialized script buffers in the JSI::Runtime instance assuming as these buffers 
  // are needed to stay alive for the lifetime of the JSI::Runtime implementation. This approach doesn't make sense
  // for other external buffers which may get created during the execution as that will stop the backing buffer from getting released
  // when the JSValue gets collected.

  // These buffers back the external array buffers that we handover to ChakraCore.
  std::vector<std::shared_ptr<const jsi::Buffer>> m_pinnedPreparedScripts;

  // These buffers are kept to serve the source callbacks when evaluating serialized scripts.
  std::vector<std::shared_ptr<const jsi::Buffer>> m_pinnedScripts;

private:
  static std::once_flag s_runtimeVersionInitFlag;

  static uint64_t s_runtimeVersion;
  static constexpr const char* s_runtimeType = "ChakraJsiRuntime";

  static void initRuntimeVersion()  noexcept;
  static uint64_t getRuntimeVersion() { return s_runtimeVersion; }

private:

  class ChakraPropertyIdValue final : public PointerValue {
    ChakraPropertyIdValue(JsPropertyIdRef str);
    ~ChakraPropertyIdValue();

    void invalidate() override;

    JsPropertyIdRef m_propId;
  protected:
    friend class ChakraJsiRuntime;
  };

  class ChakraStringValue final : public PointerValue {
    ChakraStringValue(JsValueRef str);
    ~ChakraStringValue();

    void invalidate() override;
  protected:
    friend class ChakraJsiRuntime;
    JsValueRef m_str;
  };

  class ChakraObjectValue final : public PointerValue {
    ChakraObjectValue(JsValueRef obj);
    ~ChakraObjectValue();

    void invalidate() override;
  protected:
    friend class ChakraJsiRuntime;
    JsValueRef m_obj;
  };

private:

  std::unique_ptr<const jsi::Buffer> generatePreparedScript(const std::string& sourceURL, const jsi::Buffer& sourceBuffer) noexcept;
  void evaluateJavaScriptSimple(const jsi::Buffer& buffer, const std::string& sourceURL);
  bool evaluateSerializedScript(const jsi::Buffer& scriptBuffer, const jsi::Buffer& serializedScriptBuffer, const std::string& sourceURL);

  PointerValue* cloneString(const Runtime::PointerValue* pv) override;
  PointerValue* cloneObject(const Runtime::PointerValue* pv) override;
  PointerValue* clonePropNameID(const Runtime::PointerValue* pv) override;

  jsi::PropNameID createPropNameIDFromAscii(const char* str, size_t length) override;
  jsi::PropNameID createPropNameIDFromUtf8(const uint8_t* utf8, size_t length) override;
  jsi::PropNameID createPropNameIDFromString(const jsi::String& str) override;
  std::string utf8(const jsi::PropNameID&) override;
  bool compare(const jsi::PropNameID&, const jsi::PropNameID&) override;

  jsi::String createStringFromAscii(const char* str, size_t length) override;
  jsi::String createStringFromUtf8(const uint8_t* utf8, size_t length) override;
  std::string utf8(const jsi::String&) override;

  jsi::Object createObject() override;
  jsi::Object createObject(std::shared_ptr<jsi::HostObject> hostObject) override;
  virtual std::shared_ptr<jsi::HostObject> getHostObject(const jsi::Object&) override;
  jsi::HostFunctionType& getHostFunction(const jsi::Function&) override;

  jsi::Value getProperty(const jsi::Object&, const jsi::String& name) override;
  jsi::Value getProperty(const jsi::Object&, const jsi::PropNameID& name)
    override;
  bool hasProperty(const jsi::Object&, const jsi::String& name) override;
  bool hasProperty(const jsi::Object&, const jsi::PropNameID& name) override;
  void setPropertyValue(
    jsi::Object&,
    const jsi::String& name,
    const jsi::Value& value) override;
  void setPropertyValue(
    jsi::Object&,
    const jsi::PropNameID& name,
    const jsi::Value& value) override;
  bool isArray(const jsi::Object&) const override;
  bool isArrayBuffer(const jsi::Object&) const override;
  bool isFunction(const jsi::Object&) const override;
  bool isHostObject(const jsi::Object&) const override;
  bool isHostFunction(const jsi::Function&) const override;
  jsi::Array getPropertyNames(const jsi::Object&) override;

  jsi::WeakObject createWeakObject(const jsi::Object&) override;
  jsi::Value lockWeakObject(const jsi::WeakObject&) override;

  jsi::Array createArray(size_t length) override;
  size_t size(const jsi::Array&) override;
  size_t size(const jsi::ArrayBuffer&) override;
  uint8_t* data(const jsi::ArrayBuffer&) override;
  jsi::Value getValueAtIndex(const jsi::Array&, size_t i) override;
  void setValueAtIndexImpl(jsi::Array&, size_t i, const jsi::Value& value) override;

  jsi::Function createFunctionFromHostFunction(
    const jsi::PropNameID& name,
    unsigned int paramCount,
    jsi::HostFunctionType func) override;
  jsi::Value call(
    const jsi::Function&,
    const jsi::Value& jsThis,
    const jsi::Value* args,
    size_t count) override;
  jsi::Value callAsConstructor(
    const jsi::Function&,
    const jsi::Value* args,
    size_t count) override;

  bool strictEquals(const jsi::String& a, const jsi::String& b) const override;
  bool strictEquals(const jsi::Object& a, const jsi::Object& b) const override;
  bool instanceOf(const jsi::Object& o, const jsi::Function& f) override;

private:
  // Basically convenience casts
  static JsValueRef stringRef(const jsi::String& str);
  static JsPropertyIdRef propIdRef(const jsi::PropNameID& sym);
  static JsValueRef objectRef(const jsi::Object& obj);

  // Factory methods for creating String/Object
  jsi::String createString(JsValueRef stringRef) const;
  jsi::PropNameID createPropNameID(JsValueRef stringRef);
  jsi::Object createObject(JsValueRef objectRef) const;

  // Used by factory methods and clone methods
  jsi::Runtime::PointerValue* makeStringValue(JsValueRef str) const;
  jsi::Runtime::PointerValue* makeObjectValue(JsValueRef obj) const;
  jsi::Runtime::PointerValue* makePropertyIdValue(JsPropertyIdRef propId) const;

  inline void checkException(JsErrorCode res);
  inline void checkException(JsErrorCode res, const char* msg);

  JsRuntimeHandle m_runtime;
  JsContextRef m_ctx;
  std::string m_desc;

  static JsValueRef CALLBACK HostFunctionCall(JsValueRef callee, bool isConstructCall, JsValueRef *argumentsIncThis, unsigned short argumentCountIncThis, void *callbackState);

  // String helpers
  static std::wstring JSStringToSTLWString(JsValueRef str);
  static std::string JSStringToSTLString(JsValueRef str);

  static JsValueRef createJSString(const char*data, size_t length);
  static JsValueRef createJSPropertyId(const char*data, size_t length);

  friend class ChakraJsiRuntimeWithDebugger;
};

}}} // namespace facebook::jsi::chakraruntime
