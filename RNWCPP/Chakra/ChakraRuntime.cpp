// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <ChakraCore.h>

#ifndef JSI_CORE
#include "ChakraCoreDebugger.h"
#endif // JSI_CORE

#include "ChakraRuntime.h"

#include <MemoryTracker.h>
#include <cxxreact/MessageQueueThread.h>

#include <mutex> 
#include <sstream>
#include "UnicodeConversion.h"

namespace facebook { namespace react { namespace chakra {

class ChakraRuntime : public jsi::Runtime {
public:
  ChakraRuntime();
  ChakraRuntime(ChakraRuntimeArgs&& args);
  ~ChakraRuntime();

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
  ChakraRuntimeArgs& runtimeArgs() {
    return args_;
  }

private:
  // Arguments shared by the specialized child classes too ..
  ChakraRuntimeArgs args_;

  // These buffers back the external array buffers that we handover to ChakraCore.
  std::vector<std::shared_ptr<const jsi::Buffer>> pinnedPreparedScripts_;

  // These buffers are kept to serve the source callbacks when evaluating serialized scripts.
  std::vector<std::shared_ptr<const jsi::Buffer>> pinnedScripts_;

private:
  static std::once_flag s_runtime_version_init_flag_;

  static uint64_t s_runtimeVersion;
  static constexpr const char* s_runtimeType = "ChakraRuntime";

  static void initRuntimeVersion();
  static uint64_t getRuntimeVersion();

private:

  class ChakraPropertyIdValue final : public PointerValue {
    ChakraPropertyIdValue(JsPropertyIdRef str);
    ~ChakraPropertyIdValue();

    void invalidate() override;

    JsPropertyIdRef propId_;
  protected:
    friend class ChakraRuntime;
  };

  class ChakraStringValue final : public PointerValue {
    ChakraStringValue(JsValueRef str);
    ~ChakraStringValue();

    void invalidate() override;
  protected:
    friend class ChakraRuntime;
    JsValueRef str_;
  };

  class ChakraObjectValue final : public PointerValue {
    ChakraObjectValue(JsValueRef obj);
    ~ChakraObjectValue();

    void invalidate() override;
  protected:
    friend class ChakraRuntime;
    JsValueRef obj_;
  };

private:

  std::unique_ptr<const jsi::Buffer> generatePreparedScript(const std::string& sourceURL, std::shared_ptr<const jsi::Buffer> sourceBuffer) noexcept;
  void evaluateJavaScriptSimple(std::shared_ptr<const jsi::Buffer> buffer, const std::string& sourceURL);

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

  JsRuntimeHandle runtime_;
  JsContextRef ctx_;
  std::string desc_;

  static JsValueRef CALLBACK HostFunctionCall(JsValueRef callee, bool isConstructCall, JsValueRef *argumentsIncThis, unsigned short argumentCountIncThis, void *callbackState);

  // String helpers
  static std::wstring JSStringToSTLWString(JsValueRef str);
  static std::string JSStringToSTLString(JsValueRef str);

#ifndef JSI_CORE
  friend class ChakraRuntimeWithDebugger;
#else // JSI_CORE
  friend class ChakraRuntimeWithNativeAsync;
#endif // JSI_CORE
};

/*static*/ std::once_flag ChakraRuntime::s_runtime_version_init_flag_;
/*static*/ uint64_t ChakraRuntime::s_runtimeVersion;

ChakraRuntime::ChakraRuntime() : ChakraRuntime{ ChakraRuntimeArgs() } {}

ChakraRuntime::ChakraRuntime(ChakraRuntimeArgs&& args) 
  : args_ { std::move(args) } {
  
  JsRuntimeAttributes runtimeAttributes = JsRuntimeAttributeNone;

  if (!runtimeArgs().enableJITCompilation) {
    runtimeAttributes = static_cast<JsRuntimeAttributes>(runtimeAttributes | JsRuntimeAttributeDisableNativeCodeGeneration | JsRuntimeAttributeDisableExecutablePageAllocation);
  }

  if (JsCreateRuntime(runtimeAttributes, nullptr, &runtime_) != JsNoError) {
    throw jsi::JSINativeException("Chakra Runtime creation failed.");
  }

  // Create an execution context
  JsCreateContext(runtime_, &ctx_);
  JsAddRef(ctx_, nullptr);

  // Note :: We currently assume that the runtime will be created and exclusively used in a single thread.
  JsSetCurrentContext(ctx_);

  std::call_once(s_runtime_version_init_flag_, initRuntimeVersion);
}

ChakraRuntime::~ChakraRuntime() {
  JsSetCurrentContext(JS_INVALID_REFERENCE);
  JsRelease(ctx_, nullptr);

  JsDisposeRuntime(runtime_);
}

#ifndef JSI_CORE
class ChakraRuntimeWithDebugger : public ChakraRuntime {
public:
  ChakraRuntimeWithDebugger(ChakraRuntimeArgs&& args);
  ~ChakraRuntimeWithDebugger();

private:
  JsErrorCode enableDebugging(JsRuntimeHandle runtime, std::string const& runtimeName, bool breakOnNextLine, uint16_t port, std::unique_ptr<DebugProtocolHandler>& debugProtocolHandler, std::unique_ptr<DebugService>& debugService);
  void ProcessDebuggerCommandQueue();

  static void CHAKRA_CALLBACK ProcessDebuggerCommandQueueCallback(void* callbackState);

  constexpr static char DEBUGGER_DEFAULT_RUNTIME_NAME[] = "runtime1";
  constexpr static int DEBUGGER_DEFAULT_PORT = 9229;

  std::string debugRuntimeName_;
  int debugPort_ { 0 };
  std::unique_ptr<DebugProtocolHandler> debugProtocolHandler_;
  std::unique_ptr<DebugService> debugService_;
};

ChakraRuntimeWithDebugger::ChakraRuntimeWithDebugger(ChakraRuntimeArgs&& args)
  : ChakraRuntime{ std::move(args) } {
  bool enableDebugging = runtimeArgs().enableDebugging;
  bool breakOnNextLine = runtimeArgs().debuggerBreakOnNextLine;

  if (enableDebugging)
  {
    std::string runtimeName = runtimeArgs().debuggerRuntimeName;
    int port = runtimeArgs().debuggerPort;

    if (runtimeName.empty()) {
      runtimeName = DEBUGGER_DEFAULT_RUNTIME_NAME;
    }

    if (port == 0) {
      port = DEBUGGER_DEFAULT_PORT;
    }

    JsErrorCode result = this->enableDebugging(runtime_, runtimeName, breakOnNextLine, port, debugProtocolHandler_, debugService_);

    if (result == JsNoError) {
      debugPort_ = port;
      debugRuntimeName_ = runtimeName;
    }
  }

  if (runtimeArgs().memoryTracker) {
    size_t initialMemoryUsage = 0;
    JsGetRuntimeMemoryUsage(runtime_, &initialMemoryUsage);
    runtimeArgs().memoryTracker->Initialize(initialMemoryUsage);

    if (runtimeArgs().runtimeMemoryLimit > 0)
      JsSetRuntimeMemoryLimit(runtime_, runtimeArgs().runtimeMemoryLimit);

    JsSetRuntimeMemoryAllocationCallback(runtime_, runtimeArgs().memoryTracker.get(),
      [](void* callbackState, JsMemoryEventType allocationEvent, size_t allocationSize) -> bool
    {
      auto memoryTrackerPtr = static_cast<MemoryTracker*>(callbackState);
      switch (allocationEvent)
      {
      case JsMemoryAllocate:
        memoryTrackerPtr->OnAllocation(allocationSize);
        break;

      case JsMemoryFree:
        memoryTrackerPtr->OnDeallocation(allocationSize);
        break;

      case JsMemoryFailure:
      default:
        break;
      }

      return true;
    }
    );
  }

  if (breakOnNextLine && debugProtocolHandler_) {
    if(runtimeArgs().loggingCallback) runtimeArgs().loggingCallback("Waiting for debugger to connect...", facebook::react::chakra::LogLevel::Info);
    debugProtocolHandler_->WaitForDebugger();
    if (runtimeArgs().loggingCallback) runtimeArgs().loggingCallback("Debugger connected", facebook::react::chakra::LogLevel::Info);
  }
}

ChakraRuntimeWithDebugger::~ChakraRuntimeWithDebugger() {
  if (debugService_)
  {
    JsErrorCode result = debugService_->Close();

    if (result == JsNoError)
    {
      result = debugService_->UnregisterHandler(debugRuntimeName_);
    }
  }
  debugService_ = nullptr;
  debugProtocolHandler_ = nullptr;

  // TODO: Does order matter here? Should this be after release context and before disposeruntime?
  JsSetRuntimeMemoryAllocationCallback(runtime_, nullptr, nullptr);
}

JsErrorCode ChakraRuntimeWithDebugger::enableDebugging(JsRuntimeHandle runtime, std::string const& runtimeName, bool breakOnNextLine, uint16_t port,
  std::unique_ptr<DebugProtocolHandler>& debugProtocolHandler, std::unique_ptr<DebugService>& debugService) {
  JsErrorCode result = JsNoError;
  auto protocolHandler = std::make_unique<DebugProtocolHandler>(runtime);
  auto service = std::make_unique<DebugService>(runtime);

  result = service->RegisterHandler(runtimeName, *protocolHandler, breakOnNextLine);

  if (result == JsNoError) {
    if (protocolHandler)
    {
      result = protocolHandler->SetCommandQueueCallback(ProcessDebuggerCommandQueueCallback, this);
    }
  }

  if (result == JsNoError) {
    result = service->Listen(port);

    if (runtimeArgs().loggingCallback)runtimeArgs().loggingCallback((std::string("Listening on ws://127.0.0.1:") + std::to_string(port) + "/" + runtimeName).c_str(), facebook::react::chakra::LogLevel::Info);
  }

  if (result == JsNoError) {
    debugProtocolHandler = std::move(protocolHandler);
    debugService = std::move(service);
  }

  return result;
}

/* static */ void ChakraRuntimeWithDebugger::ProcessDebuggerCommandQueueCallback(void* callbackState){
  ChakraRuntimeWithDebugger* runtime= reinterpret_cast<ChakraRuntimeWithDebugger*>(callbackState);

  if (runtime) {
    runtime->ProcessDebuggerCommandQueue();
  }
}

void ChakraRuntimeWithDebugger::ProcessDebuggerCommandQueue() {
  if (runtimeArgs().jsQueue.get()) {
    runtimeArgs().jsQueue->runOnQueue([this]() {
      if (debugProtocolHandler_) {
        debugProtocolHandler_->ProcessCommandQueue();
      }
    });
  }
}

#else // JSI_CORE

class ChakraRuntimeWithNativeAsync : public ChakraRuntime {
public:
  ChakraRuntimeWithNativeAsync(IPromiseContinuation* promiseContinuation = nullptr);

  void promiseContinuation(JsValueRef value);

private:
  IPromiseContinuation* _promiseContinuation;
};

// ES6 Promise callback
void CALLBACK PromiseContinuationCallback(JsValueRef funcRef, void* callbackState) {
  ChakraRuntimeWithNativeAsync* runtime = static_cast<ChakraRuntimeWithNativeAsync*>(callbackState);
  runtime->promiseContinuation(funcRef);
}

void ChakraRuntimeWithNativeAsync::promiseContinuation(JsValueRef funcRef) {
  _promiseContinuation->registerPromise(createObject(funcRef).getFunction(*this));
}

ChakraRuntimeWithNativeAsync::ChakraRuntimeWithNativeAsync(IPromiseContinuation* promiseContinuation)
  : ChakraRuntime() {
  if (promiseContinuation) {
    _promiseContinuation = promiseContinuation;
    JsSetPromiseContinuationCallback(PromiseContinuationCallback, this);
  }
}
#endif // JSI_CORE

      
/*static*/ void initRuntimeVersion();
/*static*/ uint64_t ChakraRuntime::getRuntimeVersion() { return s_runtimeVersion; }


// This is very wierd. This should match with the definition of VS_VERSIONINFO as defined in https://docs.microsoft.com/en-us/windows/desktop/menurc/vs-versioninfo
// I can't find a way to include the actual definition of VS_VERSIONINFO
// TODO :: Re-evaluate this strategy.
struct FileVersionInfoResource {
  uint16_t len;
  uint16_t valLen;
  uint16_t type;
  wchar_t key[_countof(L"VS_VERSION_INFO")];
  uint16_t padding1;
  VS_FIXEDFILEINFO fixedFileInfo;
  uint32_t padding2;
};

// TODO :: This code is mostly copied from the old ChakraExecutor flow, and not verified for reliability yet.
// TODO :: Re-evaluate this strategy of finding the dll version for versioning the runtime.
/*static*/ void ChakraRuntime::initRuntimeVersion() {
  // This code is win32 only at the moment. We will need to change this
  // line if we want to support UWP.
  constexpr wchar_t chakraDllName[] = L"ChakraCore.dll";

  auto freeLibraryWrapper = [](void* p) { FreeLibrary((HMODULE)p); };
  HMODULE moduleHandle;
  if (!GetModuleHandleExW(0, chakraDllName, &moduleHandle))
    return;

  std::unique_ptr<void, decltype(freeLibraryWrapper)> moduleHandleWrapper(moduleHandle, std::move(freeLibraryWrapper));

  HRSRC versionResourceHandle = FindResourceW(moduleHandle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

  if (!versionResourceHandle || SizeofResource(static_cast<HMODULE>(moduleHandleWrapper.get()), versionResourceHandle) < sizeof(FileVersionInfoResource))
    return;

  HGLOBAL versionResourcePtrHandle = LoadResource(moduleHandle, versionResourceHandle);
  if (!versionResourcePtrHandle)
    return;

  FileVersionInfoResource* chakraVersionInfo = static_cast<FileVersionInfoResource*>(LockResource(versionResourcePtrHandle));
  if (!chakraVersionInfo)
    return;

  s_runtimeVersion = chakraVersionInfo->fixedFileInfo.dwFileVersionMS;
  s_runtimeVersion <<= 32;
  s_runtimeVersion |= chakraVersionInfo->fixedFileInfo.dwFileVersionLS;
}

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

std::unique_ptr<const jsi::Buffer> ChakraRuntime::generatePreparedScript(const std::string& sourceURL, std::shared_ptr<const jsi::Buffer> sourceBuffer) noexcept {
  const std::wstring scriptUTF16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(sourceBuffer->data()), sourceBuffer->size());

  unsigned int bytecodeSize = 0;
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

void ChakraRuntime::evaluateJavaScriptSimple(std::shared_ptr<const jsi::Buffer> buffer, const std::string& sourceURL) {
  JsValueRef sourceRef;
  JsCreateString(reinterpret_cast<const char*>(buffer->data()), buffer->size(), &sourceRef);

  JsValueRef sourceURLRef = nullptr;
  if (!sourceURL.empty()) {
    JsCreateString(reinterpret_cast<const char*>(sourceURL.c_str()), sourceURL.size(), &sourceURLRef);
  }

  JsValueRef result;
  checkException(JsRun(sourceRef, 0, sourceURLRef, JsParseScriptAttributes::JsParseScriptAttributeNone, &result), sourceURL.c_str());
}
      
void ChakraRuntime::evaluateJavaScript(
  std::unique_ptr<const jsi::Buffer> buffer,
  const std::string& sourceURL) {

  // Simple evaluate if scriptStore not available as it's risky to utilize the byte codes without checking the script version.
  if (!runtimeArgs().scriptStore) {
    if (!buffer) std::terminate();
    evaluateJavaScriptSimple(std::shared_ptr<const jsi::Buffer>(std::move(buffer)), sourceURL);
    return;
  }

  uint64_t scriptVersion = 0;
  std::unique_ptr<const jsi::Buffer> scriptBuffer;

  if (buffer) {
    scriptBuffer = std::move(buffer);
    scriptVersion = runtimeArgs().scriptStore->getScriptVersion(sourceURL);
  }
  else {
    auto versionedScript = runtimeArgs().scriptStore->getVersionedScript(sourceURL);
    scriptBuffer = std::move(versionedScript.buffer);
    scriptVersion = versionedScript.version;
  }

  if (!scriptBuffer) {
    std::terminate();
  }

  // Simple evaluate if script version can't be computed.
  if (scriptVersion == 0) {
    evaluateJavaScriptSimple(std::shared_ptr<const jsi::Buffer>(std::move(scriptBuffer)), sourceURL);
    return;
  }

  auto sharedScriptBuffer = std::shared_ptr<const jsi::Buffer>(std::move(scriptBuffer));

  jsi::ScriptSignature scriptSignature = { sourceURL, scriptVersion };
  jsi::JSRuntimeSignature runtimeSignature = { s_runtimeType, getRuntimeVersion() };

  auto preparedScript = runtimeArgs().preparedScriptStore->tryGetPreparedScript(scriptSignature, runtimeSignature, nullptr);

  std::shared_ptr<const jsi::Buffer> sharedPreparedScript;
  if (preparedScript) {
    sharedPreparedScript = std::shared_ptr<const jsi::Buffer>(std::move(preparedScript));
  }
  else {
    auto genPreparedScript = generatePreparedScript(sourceURL, sharedScriptBuffer);
    if (!genPreparedScript) std::terminate(); // Cache generation can't fail unless something really wrong. but we should get rid of this abort before shipping.

    sharedPreparedScript = std::shared_ptr<const jsi::Buffer>(std::move(genPreparedScript));
    runtimeArgs().preparedScriptStore->persistPreparedScript(sharedPreparedScript, scriptSignature, runtimeSignature, nullptr);
  }

  JsValueRef sourceURLRef = nullptr;
  if (!sourceURL.empty()) {
    JsCreateString(reinterpret_cast<const char*>(sourceURL.c_str()), sourceURL.size(), &sourceURLRef);
  }

  // We are pinning the buffers which are backing the external array buffers to the duration of this.
  // This is not good if the external array buffers have a reduced liftime compared to the runtime itself.
  // But, it's ok for the script and prepared script buffer as their lifetime is expected to be same as the JSI runtime.
  pinnedPreparedScripts_.push_back(sharedPreparedScript);
  pinnedScripts_.push_back(sharedScriptBuffer);

  // Execute prepared script
  JsValueRef bytecodeArrayBuffer = nullptr;
  if (JsCreateExternalArrayBuffer(const_cast<uint8_t *>(sharedPreparedScript->data()), sharedPreparedScript->size(), nullptr, nullptr, &bytecodeArrayBuffer) == JsNoError) {

    JsValueRef value = nullptr;
    checkException(JsRunSerialized(bytecodeArrayBuffer, [](JsSourceContext sourceContext, JsValueRef *value, JsParseScriptAttributes *parseAttributes)
    {
      const jsi::Buffer* scriptSource = reinterpret_cast<const jsi::Buffer*>(sourceContext);
      if (JsCreateExternalArrayBuffer(const_cast<uint8_t *>(scriptSource->data()), scriptSource->size(), nullptr, nullptr, value) != JsNoError)
        std::terminate();

      *parseAttributes = JsParseScriptAttributeNone;
      return true;
    }, reinterpret_cast<JsSourceContext>(pinnedScripts_.back().get()), sourceURLRef, &value));
  
    return;
  }

  // If we reach here, fall back to simple evaluation.
  evaluateJavaScriptSimple(sharedScriptBuffer, sourceURL);
}

jsi::Object ChakraRuntime::global() {
  JsValueRef value;
  JsGetGlobalObject(&value);
  return createObject(value);
}

std::string ChakraRuntime::description() {
  if (desc_.empty()) {
    desc_ = std::string("<ChakraRuntime>");
  }
  return desc_;
}

bool ChakraRuntime::isInspectable() {
  return false;
}

ChakraRuntime::ChakraPropertyIdValue::~ChakraPropertyIdValue() {
  JsRelease(propId_, nullptr);
}

void ChakraRuntime::ChakraPropertyIdValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraPropertyIdValue::ChakraPropertyIdValue(JsPropertyIdRef propIdRef)
  : propId_(propIdRef) {
  JsAddRef(propIdRef, nullptr);
}

ChakraRuntime::ChakraStringValue::ChakraStringValue(JsValueRef str)
  : str_(str) {
  JsAddRef(str, nullptr);
}

void ChakraRuntime::ChakraStringValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraStringValue::~ChakraStringValue() {
  JsRelease(str_, nullptr);
}

ChakraRuntime::ChakraObjectValue::ChakraObjectValue(
  JsValueRef obj
) : obj_(obj)
{
  JsAddRef(obj_, nullptr);
}

void ChakraRuntime::ChakraObjectValue::invalidate() {
  delete this;
}

ChakraRuntime::ChakraObjectValue::~ChakraObjectValue() {
  JsRelease(obj_, nullptr);
}

jsi::Runtime::PointerValue* ChakraRuntime::cloneString(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }
  const ChakraStringValue* string = static_cast<const ChakraStringValue*>(pv);
  return makeStringValue(string->str_);
}

jsi::Runtime::PointerValue* ChakraRuntime::cloneObject(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }

  const ChakraObjectValue* object = static_cast<const ChakraObjectValue*>(pv);
  return makeObjectValue(object->obj_);
}

jsi::Runtime::PointerValue* ChakraRuntime::clonePropNameID(
  const jsi::Runtime::PointerValue* pv) {

  if (!pv) {
    return nullptr;
  }

  const ChakraPropertyIdValue* propId = static_cast<const ChakraPropertyIdValue*>(pv);
  return makePropertyIdValue(propId->propId_);
}

jsi::PropNameID ChakraRuntime::createPropNameIDFromAscii(
  const char* str,
  size_t length) {

  JsValueRef propIdRef;
  checkException(JsCreatePropertyId(str, length, &propIdRef));

  auto res = createPropNameID(propIdRef);
  return res;
}

jsi::PropNameID ChakraRuntime::createPropNameIDFromUtf8(
  const uint8_t* utf8,
  size_t length) {

  JsValueRef prpoIdRef;
  checkException(JsCreatePropertyId(reinterpret_cast<const char*>(utf8), length, &prpoIdRef));
  auto res = createPropNameID(prpoIdRef);
  return res;
}

jsi::PropNameID ChakraRuntime::createPropNameIDFromString(const jsi::String& str) {
  std::string propNameString = JSStringToSTLString(stringRef(str));
  return createPropNameIDFromUtf8(reinterpret_cast<const uint8_t*>(propNameString.c_str()), propNameString.length());
}

std::string ChakraRuntime::utf8(const jsi::PropNameID& sym) {
  const wchar_t* name;
  checkException(JsGetPropertyNameFromId(propIdRef(sym), &name));
  return facebook::react::UnicodeConversion::Utf16ToUtf8(name, wcslen(name));
}

bool ChakraRuntime::compare(const jsi::PropNameID& a, const jsi::PropNameID& b) {
  bool result;
  JsEquals(propIdRef(a), propIdRef(b), &result);
  return result;
}

jsi::String ChakraRuntime::createStringFromAscii(const char* str, size_t length) {
  // Yes we end up double casting for semantic reasons (UTF8 contains ASCII,
  // not the other way around)
  return this->createStringFromUtf8(
    reinterpret_cast<const uint8_t*>(str), length);
}

jsi::String ChakraRuntime::createStringFromUtf8(
  const uint8_t* str,
  size_t length) {

  JsValueRef stringRef;
  checkException(JsCreateString(reinterpret_cast<const char*>(str), length, &stringRef));
  return createString(stringRef);
}

std::string ChakraRuntime::utf8(const jsi::String& str) {
  return JSStringToSTLString(stringRef(str));
}

jsi::Object ChakraRuntime::createObject() {
  return createObject(static_cast<JsValueRef>(nullptr));
}

jsi::Object ChakraRuntime::createObject(std::shared_ptr<jsi::HostObject> hostObject) {
  throw std::runtime_error("ChakraRuntime::createObject from HostObject is not implemented.");
}

std::shared_ptr<jsi::HostObject> ChakraRuntime::getHostObject(
  const jsi::Object& obj) {
  throw std::runtime_error("ChakraRuntime::createObject is not implemented.");
}

jsi::Value ChakraRuntime::getProperty(
  const jsi::Object& obj,
  const jsi::String& name) {
  JsValueRef objRef = objectRef(obj);

  std::wstring propName = JSStringToSTLWString(stringRef(name));
  JsPropertyIdRef propIdRef;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propIdRef));

  JsValueRef value;
  checkException(JsGetProperty(objectRef(obj), propIdRef, &value));
  return createValue(value);
}

jsi::Value ChakraRuntime::getProperty(
  const jsi::Object& obj,
  const jsi::PropNameID& name) {
  JsValueRef objRef = objectRef(obj);
  JsValueRef exc = nullptr;
  JsValueRef res;
  checkException(JsGetProperty(objRef, propIdRef(name), &res));
  return createValue(res);
}

bool ChakraRuntime::hasProperty(const jsi::Object& obj, const jsi::String& name) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propId, &hasProperty));
  return hasProperty;
}

bool ChakraRuntime::hasProperty(
  const jsi::Object& obj,
  const jsi::PropNameID& name) {
  bool hasProperty;
  checkException(JsHasProperty(objectRef(obj), propIdRef(name), &hasProperty));
  return hasProperty;
}

void ChakraRuntime::setPropertyValue(
  jsi::Object& object,
  const jsi::PropNameID& name,
  const jsi::Value& value) {
  checkException(JsSetProperty(objectRef(object), propIdRef(name), valueRef(value), true));
}

void ChakraRuntime::setPropertyValue(
  jsi::Object& object,
  const jsi::String& name,
  const jsi::Value& value) {
  std::wstring propName = JSStringToSTLWString(stringRef(name));

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsGetPropertyIdFromName(propName.c_str(), &propId));

  checkException(JsSetProperty(objectRef(object), propId, valueRef(value), true));
}

bool ChakraRuntime::isArray(const jsi::Object& obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsArray;
}

bool ChakraRuntime::isArrayBuffer(const jsi::Object& /*obj*/) const {
  throw std::runtime_error("Unsupported");
}

uint8_t* ChakraRuntime::data(const jsi::ArrayBuffer& /*obj*/) {
  throw std::runtime_error("Unsupported");
}

size_t ChakraRuntime::size(const jsi::ArrayBuffer& /*obj*/) {
  throw std::runtime_error("Unsupported");
}

bool ChakraRuntime::isFunction(const jsi::Object& obj) const {
  JsValueType type;
  JsGetValueType(objectRef(obj), &type);
  return type == JsValueType::JsFunction;
}

bool ChakraRuntime::isHostObject(const jsi::Object& obj) const {
  throw std::runtime_error("Unsupported");
}

jsi::Array ChakraRuntime::getPropertyNames(const jsi::Object& obj) {

  JsValueRef propertyNamesArrayRef;
  checkException(JsGetOwnPropertyNames(objectRef(obj), &propertyNamesArrayRef));

  JsPropertyIdRef propertyId;
  checkException(JsGetPropertyIdFromName(L"length", &propertyId));
  JsValueRef countRef;
  checkException(JsGetProperty(propertyNamesArrayRef, propertyId, &countRef));
  int count;
  checkException(JsNumberToInt(countRef, &count));

  auto result = createArray(count);
  for (size_t i = 0; i < count; i++) {
    JsValueRef index;
    checkException(JsIntToNumber(i, &index));
    JsValueRef propertyName;
    checkException(JsGetIndexedProperty(propertyNamesArrayRef, index, &propertyName));
    result.setValueAtIndex(*this, i, createString(propertyName));
  }

  return result;
}

jsi::WeakObject ChakraRuntime::createWeakObject(const jsi::Object&) {
  throw std::logic_error("Not implemented");
}

jsi::Value ChakraRuntime::lockWeakObject(const jsi::WeakObject&) {
  throw std::logic_error("Not implemented");
}

jsi::Array ChakraRuntime::createArray(size_t length) {
  JsValueRef result;
  checkException(JsCreateArray(length, &result));
  return createObject(result).getArray(*this);
}

size_t ChakraRuntime::size(const jsi::Array& arr) {
  std::string lengthStr = "length";

  JsPropertyIdRef propId = JS_INVALID_REFERENCE;
  checkException(JsCreatePropertyId(lengthStr.c_str(), lengthStr.length(), &propId));

  JsValueRef valueObject;
  checkException(JsGetProperty(objectRef(arr), propId, &valueObject));

  JsValueRef numberValue;
  checkException(JsConvertValueToNumber(valueObject, &numberValue));

  int intValue;
  checkException(JsNumberToInt(numberValue, &intValue));

  return intValue;
}

jsi::Value ChakraRuntime::getValueAtIndex(const jsi::Array& arr, size_t i) {
  JsValueRef index;
  JsIntToNumber(i, &index);
  JsValueRef property;
  checkException(JsGetIndexedProperty(objectRef(arr), index, &property));
  return createValue(property);
}

void ChakraRuntime::setValueAtIndexImpl(
  jsi::Array& arr,
  size_t i,
  const jsi::Value& value) {
  JsValueRef index;
  JsIntToNumber(i, &index);

  checkException(JsSetIndexedProperty(objectRef(arr), index, valueRef(value)));
}

class HostFunctionProxy {
public:
  HostFunctionProxy(jsi::HostFunctionType hostFunction, ChakraRuntime& runtime)
    : hostFunction_(hostFunction), runtime_(runtime) {}

  inline const jsi::HostFunctionType& getHostFunction() const{
    return hostFunction_;
  }

  inline ChakraRuntime& getRuntime() const{
    return runtime_;
  }

private:
  const jsi::HostFunctionType hostFunction_;
  ChakraRuntime& runtime_;
};

JsValueRef CALLBACK ChakraRuntime::HostFunctionCall(JsValueRef callee, bool isConstructCall, JsValueRef *argumentsIncThis, unsigned short argumentCountIncThis, void *callbackState)
{
  const HostFunctionProxy& hostFuncProxy = *reinterpret_cast<HostFunctionProxy*>(callbackState);

  const unsigned maxStackArgCount = 8;
  jsi::Value stackArgs[maxStackArgCount];
  std::unique_ptr<jsi::Value[]> heapArgs;
  jsi::Value* args;

  // Accounting for 'this' object at 0
  unsigned short argumentCount = argumentCountIncThis - 1;

  if (argumentCount > maxStackArgCount) {
    heapArgs = std::make_unique<jsi::Value[]>(argumentCount);
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      heapArgs[i - 1] = hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = heapArgs.get();
  }
  else {
    for (size_t i = 1; i < argumentCountIncThis; i++) {
      stackArgs[i - 1] = hostFuncProxy.getRuntime().createValue(argumentsIncThis[i]);
    }
    args = stackArgs;
  }
  JsValueRef res { JS_INVALID_REFERENCE };
  jsi::Value thisVal(hostFuncProxy.getRuntime().createObject(argumentsIncThis[0]));
  try {
    jsi::Value retVal = hostFuncProxy.getHostFunction()(hostFuncProxy.getRuntime(), thisVal, args, argumentCount);
    res = hostFuncProxy.getRuntime().valueRef(retVal);
  }
  catch (const jsi::JSError& error) {
    JsSetException(hostFuncProxy.getRuntime().valueRef(error.value()));
  }
  catch (const std::exception& ex) {
    std::string exwhat(ex.what());
    JsValueRef exn;
    JsCreateString(exwhat.c_str(), exwhat.size(), &exn);
    JsSetException(exn);
  }
  catch (...) {
    std::string exceptionString("Exception in HostFunction: <unknown>");
    JsValueRef exn;
    JsCreateString(exceptionString.c_str(), exceptionString.size(), &exn);
    JsSetException(exn);
  }

  return res;
}

jsi::Function ChakraRuntime::createFunctionFromHostFunction(
  const jsi::PropNameID& name,
  unsigned int paramCount,
  jsi::HostFunctionType func) {

  // Currently, we are allocating this proxy object in heap .. and deleting it whenever the JS object is garbage collected.
  HostFunctionProxy* hostFuncProxy = new HostFunctionProxy(func, *this);

  JsValueRef funcRef;
  checkException(JsCreateFunction(ChakraRuntime::HostFunctionCall, hostFuncProxy, &funcRef));
  checkException(JsSetObjectBeforeCollectCallback(funcRef, hostFuncProxy, [](JsRef ref, void *hostFuncProxy) { delete hostFuncProxy; }));

  return createObject(funcRef).getFunction(*this);
}

namespace detail {

  class ArgsConverterForCall {
  public:
    ArgsConverterForCall(ChakraRuntime& rt, JsValueRef thisObj, const jsi::Value* args, size_t count) {
      JsValueRef* destination = inline_;
      if (count + 1 > maxStackArgs) {
        outOfLine_ = std::make_unique<JsValueRef[]>(count + 1);
        destination = outOfLine_.get();
      }

      if (thisObj == nullptr)
      {
        JsValueRef undefinedValue;
        JsGetUndefinedValue(&undefinedValue);
        destination[0] = undefinedValue;
      }
      else
      {
        destination[0] = thisObj;
      }

      for (size_t i = 0; i < count; ++i) {
        destination[i+1] = rt.valueRef(args[i]);
      }
    }

    operator JsValueRef*() {
      return outOfLine_ ? outOfLine_.get() : inline_;
    }

  private:
    constexpr static unsigned maxStackArgs = 8;
    JsValueRef inline_[maxStackArgs];
    std::unique_ptr<JsValueRef[]> outOfLine_;
  };

} // namespace detail

bool ChakraRuntime::isHostFunction(const jsi::Function& obj) const {
  throw std::runtime_error("ChakraRuntime::isHostFunction is not implemented.");
}

jsi::HostFunctionType& ChakraRuntime::getHostFunction(const jsi::Function& obj) {
  throw std::runtime_error("ChakraRuntime::getHostFunction is not implemented.");
}

jsi::Value ChakraRuntime::call(
  const jsi::Function& f,
  const jsi::Value& jsThis,
  const jsi::Value* args,
  size_t count) {
  JsValueRef result = nullptr;
  checkException(JsCallFunction(
    objectRef(f),
    detail::ArgsConverterForCall(*this, jsThis.isUndefined() ? nullptr : objectRef(jsThis.getObject(*this)), args, count),
    count+1,
    &result));
  return createValue(result);
}

jsi::Value ChakraRuntime::callAsConstructor(
  const jsi::Function& f,
  const jsi::Value* args,
  size_t count) {
  JsValueRef result = nullptr;
  checkException(JsConstructObject(
    objectRef(f),
    detail::ArgsConverterForCall(*this, nullptr, args, count),
    count,
    &result));
  return createValue(result);
}

bool ChakraRuntime::strictEquals(const jsi::String& a, const jsi::String& b)
  const {
  bool result;
  JsStrictEquals(stringRef(a), stringRef(b), &result);
  return result;
}

bool ChakraRuntime::strictEquals(const jsi::Object& a, const jsi::Object& b)
  const {
  bool result;
  JsStrictEquals(objectRef(a), objectRef(b), &result);
  return result;
}

bool ChakraRuntime::instanceOf(const jsi::Object& o, const jsi::Function& f) {
  bool res;
  checkException(JsInstanceOf(objectRef(o), objectRef(f), &res));
  return res;
}

jsi::Runtime::PointerValue* ChakraRuntime::makeStringValue(
  JsValueRef stringRef) const {
  if (!stringRef) {
    JsValueRef emptyJsValue;
    JsCreateString("", 0, &emptyJsValue);
    stringRef = emptyJsValue;
  }
  return new ChakraStringValue(stringRef);
}

jsi::String ChakraRuntime::createString(JsValueRef str) const {
  return make<jsi::String>(makeStringValue(str));
}

jsi::PropNameID ChakraRuntime::createPropNameID(JsValueRef str) {
  return make<jsi::PropNameID>(makePropertyIdValue(str));
}

jsi::Runtime::PointerValue* ChakraRuntime::makePropertyIdValue(
  JsPropertyIdRef propIdRef) const {
  if (!propIdRef) {
    std::terminate();
  }
  return new ChakraPropertyIdValue(propIdRef);
}

jsi::Runtime::PointerValue* ChakraRuntime::makeObjectValue(
  JsValueRef objectRef) const {
  if (!objectRef) {
    JsCreateObject(&objectRef);
  }
  ChakraObjectValue* chakraObjValue = new ChakraObjectValue(objectRef);
  return chakraObjValue;
}

jsi::Object ChakraRuntime::createObject(JsValueRef obj) const {
  return make<jsi::Object>(makeObjectValue(obj));
}

jsi::Value ChakraRuntime::createValue(JsValueRef value) const {
  JsValueType type;
  JsGetValueType(value, &type);

  switch (type)
  {
  case JsUndefined:
    return jsi::Value();

  case JsNull:
    return jsi::Value(nullptr);

  case JsNumber:
  {
    double doubleValue;
    JsNumberToDouble(value, &doubleValue);
    jsi::Value val(doubleValue);
    return val;
  }

  case JsString:
  {
    std::string utf8str = JSStringToSTLString(value);
    return jsi::String::createFromUtf8(*const_cast<jsi::Runtime*>(reinterpret_cast<const jsi::Runtime*>(this)), reinterpret_cast<const uint8_t*>(utf8str.c_str()), utf8str.size());
    break;
  }

  case JsBoolean:
  {
    bool boolValue;
    JsBooleanToBool(value, &boolValue);
    jsi::Value val(boolValue);
    return val;
  }

  case JsObject:
  case JsFunction:
  case JsArray:
  {
    return jsi::Value(createObject(value));
    break;
  }

  case JsError:
  case JsSymbol:
  case JsArrayBuffer:
  case JsTypedArray:
  case JsDataView:
  default:
    std::terminate();
    break;
  }
}

JsValueRef ChakraRuntime::valueRef(const jsi::Value& valueIn){
  if (valueIn.isUndefined()) {
    JsValueRef value;
    JsGetUndefinedValue(&value);
    return value;
  }
  else if (valueIn.isNull()) {
    JsValueRef value;
    JsGetNullValue(&value);
    return value;
  }
  else if (valueIn.isBool()) {
    JsValueRef value;
    JsBoolToBoolean(valueIn.getBool(), &value);
    return value;
  }
  else if (valueIn.isNumber()) {
    JsValueRef value;
    JsDoubleToNumber(valueIn.getNumber(), &value);
    return value;
  }
  else if (valueIn.isString()) {
    return stringRef(valueIn.getString(*this));
  }
  else if (valueIn.isObject()) {
    return objectRef(valueIn.getObject(*this));
  }
  else {
    // What are you?
    abort();
  }
}

JsValueRef ChakraRuntime::stringRef(const jsi::String& str) {
  return static_cast<const ChakraStringValue*>(getPointerValue(str))->str_;
}

JsPropertyIdRef ChakraRuntime::propIdRef(const jsi::PropNameID& sym) {
  return static_cast<const ChakraPropertyIdValue*>(getPointerValue(sym))->propId_;
}

JsValueRef ChakraRuntime::objectRef(const jsi::Object& obj) {
  return static_cast<const ChakraObjectValue*>(getPointerValue(obj))->obj_;
}

void ChakraRuntime::checkException(JsErrorCode result)  {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  checkException(result, nullptr);
}

void ChakraRuntime::checkException(JsErrorCode result, const char* message) {
  bool hasException = false;
  if (result == JsNoError && (JsHasException(&hasException), !hasException))
    return;

  std::ostringstream errorStream;

  if (message != nullptr)
    errorStream << message << ". ";

  if (result != JsNoError) {
    errorStream << "ChakraCore API Error :" << std::hex << result << ". ";
  }

  if (hasException || result == JsErrorScriptException)
  {
    errorStream << "JS exception found: ";

    JsValueRef exn;
    JsGetAndClearException(&exn);

    JsPropertyIdRef messageName;
    JsGetPropertyIdFromName(L"stack", &messageName);

    JsValueRef messageValue;
    if (JsGetProperty(exn, messageName, &messageValue) == JsNoError)
    {
      if (JsConvertValueToString(exn, &messageValue) != JsNoError) {
        errorStream << "Unable to retrieve JS exception stack";
      }
      else {
        errorStream << JSStringToSTLString(messageValue);
      }
    }
    else
    {
      JsValueRef exnJStr;
      if (JsConvertValueToString(exn, &exnJStr) != JsNoError) {
        errorStream << "Unable to describe JS exception";
      }
      else {
        errorStream << JSStringToSTLString(exnJStr);
      }
    }
  }

  std::string errorString = errorStream.str();
  throw jsi::JSError(*this, createStringFromAscii(errorString.c_str(), errorString.length()));
}

std::wstring ChakraRuntime::JSStringToSTLWString(JsValueRef str) {
  std::wstring result;
  size_t length;
  JsCopyStringUtf16(str, 0, 256 /*TODO*/, nullptr, &length);
  result.resize(length);
  JsCopyStringUtf16(str, 0, length, reinterpret_cast<uint16_t*>(&result[0]), nullptr);
  return result;
}

std::string ChakraRuntime::JSStringToSTLString(JsValueRef str) {
  std::string result;
  size_t length;

  JsCopyString(str, nullptr, 0, &length);
  result.resize(length);
  JsCopyString(str, &result[0], length, nullptr);
  return result;
}

#ifndef JSI_CORE

std::unique_ptr<jsi::Runtime> makeChakraRuntime(ChakraRuntimeArgs&& args) {
  return std::make_unique<ChakraRuntimeWithDebugger>(std::move(args));
}

#else // JSI_CORE

std::unique_ptr<jsi::Runtime> makeChakraRuntime() {
  return std::make_unique<chakra::ChakraRuntime>();
}

std::unique_ptr<jsi::Runtime> makeChakraRuntime(IPromiseContinuation& promiseContinuation) {
  return std::make_unique<chakra::ChakraRuntimeWithNativeAsync>(&promiseContinuation);
}

#endif // JSI_CORE

}}} // facebook::react::chakra
