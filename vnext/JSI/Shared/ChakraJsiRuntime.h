// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#if defined(CHAKRACORE)
#include <ChakraCore.h>
#else
#include <jsrt.h>
#endif

#include "ChakraJsiRuntimeArgs.h"

#include <jsi/jsi.h>

#include <memory>
#include <mutex>
#include <sstream>

#if !defined(CHAKRACORE)
typedef JsValueRef JsWeakRef;
#endif

#if defined(CHAKRACORE)
#include "ChakraCoreDebugger.h"
#else
class DebugProtocolHandler {};
class DebugService {};
#endif

namespace facebook {
namespace jsi {
namespace chakraruntime {

class ByteArrayBuffer final : public jsi::Buffer {
 public:
  size_t size() const override {
    return size_;
  }

  const uint8_t *data() const {
    return byteArray_.get();
  }

  uint8_t *data() {
    return byteArray_.get();
  }

  ByteArrayBuffer(int size)
      : size_(size), byteArray_(std::make_unique<uint8_t[]>(size)) {}

 private:
  int size_;
  std::unique_ptr<uint8_t[]> byteArray_;
};

class ChakraJsiRuntime final : public jsi::Runtime {
 public:
  ChakraJsiRuntime(ChakraJsiRuntimeArgs &&args) noexcept;
  ~ChakraJsiRuntime() noexcept;

  jsi::Value evaluateJavaScript(
      const std::shared_ptr<const jsi::Buffer> &buffer,
      const std::string &sourceURL) override;

  jsi::Object global() override;

  std::string description() override;

  bool isInspectable() override;

  void setDescription(const std::string &desc);

  // JsValueRef->JSValue (needs make.*Value so it must be member function)
  jsi::Value createValue(JsValueRef value) const;

  // Value->JsValueRef (similar to above)
  JsValueRef valueRef(const jsi::Value &value);

 protected:
  ChakraJsiRuntimeArgs &runtimeArgs() {
    return m_args;
  }

 private:
  static void CALLBACK
  PromiseContinuationCallback(JsValueRef funcRef, void *callbackState) noexcept;
  static void CALLBACK PromiseRejectionTrackerCallback(
      JsValueRef promise,
      JsValueRef reason,
      bool handled,
      void *callbackState);

  void PromiseContinuation(JsValueRef value) noexcept;
  void
  PromiseRejectionTracker(JsValueRef promise, JsValueRef reason, bool handled);

  void setupNativePromiseContinuation() noexcept;
  void setupMemoryTracker() noexcept;

 private:
  // Arguments shared by the specializations ..
  ChakraJsiRuntimeArgs m_args;

  // Note :: For simplicity, We are pinning the script and serialized script
  // buffers in the JSI::Runtime instance assuming as these buffers are needed
  // to stay alive for the lifetime of the JSI::Runtime implementation. This
  // approach doesn't make sense for other external buffers which may get
  // created during the execution as that will stop the backing buffer from
  // getting released when the JSValue gets collected.

  // These buffers back the external array buffers that we handover to
  // ChakraCore.
  std::vector<std::shared_ptr<const jsi::Buffer>> m_pinnedPreparedScripts;

  // These buffers are kept to serve the source callbacks when evaluating
  // serialized scripts.
  std::vector<std::shared_ptr<const jsi::Buffer>> m_pinnedScripts;

 private:
  static std::once_flag s_runtimeVersionInitFlag;

  static uint64_t s_runtimeVersion;
  static constexpr const char *s_runtimeType = "ChakraJsiRuntime";

  static constexpr const char *const s_proxyGetHostObjectTargetPropName =
      "$$ProxyGetHostObjectTarget$$";
  static constexpr const char *const s_proxyIsHostObjectPropName =
      "$$ProxyIsHostObject$$";

  static void initRuntimeVersion() noexcept;
  static uint64_t getRuntimeVersion() {
    return s_runtimeVersion;
  }

 private:
  class HostObjectProxy {
   public:
    jsi::Value Get(const PropNameID &propNameId) {
      return hostObject_->get(runtime_, propNameId);
    }

    void Set(const PropNameID &propNameId, const Value &value) {
      hostObject_->set(runtime_, propNameId, value);
    }

    std::vector<PropNameID> Enumerator() {
      return hostObject_->getPropertyNames(runtime_);
    }

    HostObjectProxy(
        ChakraJsiRuntime &rt,
        const std::shared_ptr<facebook::jsi::HostObject> &hostObject)
        : runtime_(rt), hostObject_(hostObject) {}
    std::shared_ptr<facebook::jsi::HostObject> getHostObject() {
      return hostObject_;
    }

   private:
    ChakraJsiRuntime &runtime_;
    std::shared_ptr<facebook::jsi::HostObject> hostObject_;
  };

  template <class T>
  class ObjectWithExternalData : public jsi::Object {
   public:
    static jsi::Object create(ChakraJsiRuntime &rt, T *externalData);
    static ObjectWithExternalData<T> fromExisting(
        ChakraJsiRuntime &rt,
        jsi::Object &&obj);

   public:
    T *getExternalData();
    ObjectWithExternalData(const Runtime::PointerValue *value)
        : Object(const_cast<Runtime::PointerValue *>(value)) {
    } // TODO :: const_cast

    ObjectWithExternalData(ObjectWithExternalData &&) = default;
    ObjectWithExternalData &operator=(ObjectWithExternalData &&) = default;
  };

  template <class T>
  friend class ObjectWithExternalData;

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

  class ChakraWeakRefValue final : public PointerValue {
    ChakraWeakRefValue(JsWeakRef obj);
    ~ChakraWeakRefValue();

    void invalidate() override;

   protected:
    friend class ChakraJsiRuntime;
    JsWeakRef m_obj;
  };

 private:
  jsi::Object createProxy(jsi::Object &&target, jsi::Object &&handler) noexcept;
  jsi::Function createProxyConstructor() noexcept;
  jsi::Object createHostObjectProxyHandler() noexcept;

  std::unique_ptr<const jsi::Buffer> generatePreparedScript(
      const std::string &sourceURL,
      const jsi::Buffer &sourceBuffer) noexcept;
  jsi::Value evaluateJavaScriptSimple(
      const jsi::Buffer &buffer,
      const std::string &sourceURL);
  bool evaluateSerializedScript(
      const jsi::Buffer &scriptBuffer,
      const jsi::Buffer &serializedScriptBuffer,
      const std::string &sourceURL);

  std::shared_ptr<const facebook::jsi::PreparedJavaScript> prepareJavaScript(
      const std::shared_ptr<const facebook::jsi::Buffer> &,
      std::string) override;
  facebook::jsi::Value evaluatePreparedJavaScript(
      const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &)
      override;

  PointerValue *cloneString(const Runtime::PointerValue *pv) override;
  PointerValue *cloneObject(const Runtime::PointerValue *pv) override;
  PointerValue *clonePropNameID(const Runtime::PointerValue *pv) override;
  facebook::jsi::Runtime::PointerValue *cloneSymbol(
      const facebook::jsi::Runtime::PointerValue *) override;

  std::string symbolToString(const facebook::jsi::Symbol &) override;

  jsi::PropNameID createPropNameIDFromAscii(const char *str, size_t length)
      override;
  jsi::PropNameID createPropNameIDFromUtf8(const uint8_t *utf8, size_t length)
      override;
  jsi::PropNameID createPropNameIDFromString(const jsi::String &str) override;
  std::string utf8(const jsi::PropNameID &) override;
  bool compare(const jsi::PropNameID &, const jsi::PropNameID &) override;

  jsi::String createStringFromAscii(const char *str, size_t length) override;
  jsi::String createStringFromUtf8(const uint8_t *utf8, size_t length) override;
  std::string utf8(const jsi::String &) override;

  jsi::Object createObject() override;
  jsi::Object createObject(
      std::shared_ptr<jsi::HostObject> hostObject) override;
  virtual std::shared_ptr<jsi::HostObject> getHostObject(
      const jsi::Object &) override;
  jsi::HostFunctionType &getHostFunction(const jsi::Function &) override;

  jsi::Value getProperty(const jsi::Object &, const jsi::String &name) override;
  jsi::Value getProperty(const jsi::Object &, const jsi::PropNameID &name)
      override;
  bool hasProperty(const jsi::Object &, const jsi::String &name) override;
  bool hasProperty(const jsi::Object &, const jsi::PropNameID &name) override;
  void setPropertyValue(
      jsi::Object &,
      const jsi::String &name,
      const jsi::Value &value) override;
  void setPropertyValue(
      jsi::Object &,
      const jsi::PropNameID &name,
      const jsi::Value &value) override;
  bool isArray(const jsi::Object &) const override;
  bool isArrayBuffer(const jsi::Object &) const override;
  bool isFunction(const jsi::Object &) const override;
  bool isHostObject(const jsi::Object &) const override;
  bool isHostFunction(const jsi::Function &) const override;
  jsi::Array getPropertyNames(const jsi::Object &) override;

  jsi::WeakObject createWeakObject(const jsi::Object &) override;
  jsi::Value lockWeakObject(const jsi::WeakObject &) override;

  jsi::Array createArray(size_t length) override;
  size_t size(const jsi::Array &) override;
  size_t size(const jsi::ArrayBuffer &) override;
  uint8_t *data(const jsi::ArrayBuffer &) override;
  jsi::Value getValueAtIndex(const jsi::Array &, size_t i) override;
  void setValueAtIndexImpl(jsi::Array &, size_t i, const jsi::Value &value)
      override;

  jsi::Function createFunctionFromHostFunction(
      const jsi::PropNameID &name,
      unsigned int paramCount,
      jsi::HostFunctionType func) override;
  jsi::Value call(
      const jsi::Function &,
      const jsi::Value &jsThis,
      const jsi::Value *args,
      size_t count) override;
  jsi::Value callAsConstructor(
      const jsi::Function &,
      const jsi::Value *args,
      size_t count) override;

  bool strictEquals(const jsi::String &a, const jsi::String &b) const override;
  bool strictEquals(const jsi::Object &a, const jsi::Object &b) const override;
  bool strictEquals(
      const facebook::jsi::Symbol &,
      const facebook::jsi::Symbol &) const override;

  bool instanceOf(const jsi::Object &o, const jsi::Function &f) override;

 private:
  // Basically convenience casts
  static JsValueRef stringRef(const jsi::String &str);
  static JsPropertyIdRef propIdRef(const jsi::PropNameID &sym);
  static JsValueRef objectRef(const jsi::Object &obj);
  static JsWeakRef objectRef(const jsi::WeakObject &obj);

  static JsWeakRef newWeakObjectRef(const jsi::Object &obj);
  static JsValueRef strongObjectRef(const jsi::WeakObject &obj);

  // Factory methods for creating String/Object
  jsi::String createString(JsValueRef stringRef) const;
  jsi::PropNameID createPropNameID(JsValueRef stringRef);

  template <class T>
  jsi::Object createObject(JsValueRef objectRef, T *externalData) const;
  jsi::Object createObject(JsValueRef objectRef) const;

  // Used by factory methods and clone methods
  jsi::Runtime::PointerValue *makeStringValue(JsValueRef str) const;

  template <class T>
  jsi::Runtime::PointerValue *makeObjectValue(JsValueRef obj, T *externaldata)
      const;
  jsi::Runtime::PointerValue *makeObjectValue(JsValueRef obj) const;

  jsi::Runtime::PointerValue *makePropertyIdValue(JsPropertyIdRef propId) const;

  jsi::Runtime::PointerValue *makeWeakRefValue(JsWeakRef obj) const;

  inline void checkException(JsErrorCode res);
  inline void checkException(JsErrorCode res, const char *msg);

  void startDebuggingIfNeeded();
  void stopDebuggingIfNeeded();

  JsErrorCode enableDebugging(
      JsRuntimeHandle runtime,
      std::string const &runtimeName,
      bool breakOnNextLine,
      uint16_t port,
      std::unique_ptr<DebugProtocolHandler> &debugProtocolHandler,
      std::unique_ptr<DebugService> &debugService);
  void ProcessDebuggerCommandQueue();

 private:
  JsRuntimeHandle m_runtime;
  JsContextRef m_ctx;
  std::string m_desc;

  std::string m_debugRuntimeName;
  int m_debugPort{0};
  std::unique_ptr<DebugProtocolHandler> m_debugProtocolHandler;
  std::unique_ptr<DebugService> m_debugService;

 private:
  constexpr static char DebuggerDefaultRuntimeName[] = "runtime1";
  constexpr static int DebuggerDefaultPort = 9229;

  static void CALLBACK ProcessDebuggerCommandQueueCallback(void *callbackState);
  static JsValueRef CALLBACK HostFunctionCall(
      JsValueRef callee,
      bool isConstructCall,
      JsValueRef *argumentsIncThis,
      unsigned short argumentCountIncThis,
      void *callbackState);

  // String helpers
  static std::wstring JSStringToSTLWString(JsValueRef str);
  static std::string JSStringToSTLString(JsValueRef str);

  static JsValueRef createJSString(const char *data, size_t length);
  static JsValueRef createJSPropertyId(const char *data, size_t length);
};

} // namespace chakraruntime
} // namespace jsi
} // namespace facebook
