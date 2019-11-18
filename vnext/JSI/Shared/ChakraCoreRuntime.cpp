// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#ifdef CHAKRACORE

#include "ChakraCoreRuntime.h"

#include "ByteArrayBuffer.h"
#include "ChakraPreparedJavaScript.h"
#include "Unicode.h"
#include "Utilities.h"

// TODO (yicyao): MessageQueueThread is a defined in facebook::react. We need to
// see how we can get rid of this dependency.
#include "cxxreact/MessageQueueThread.h"

#include <windows.h>

#include <sstream>

namespace Microsoft::JSI {

namespace {

struct FileVersionInfoResource {
  uint16_t len;
  uint16_t valLen;
  uint16_t type;
  wchar_t key[_countof(L"VS_VERSION_INFO")];
  uint16_t padding1;
  VS_FIXEDFILEINFO fixedFileInfo;
  uint32_t padding2;
};

uint64_t GetDllFileVersion(const wchar_t *const dllName) noexcept {
  auto freeLibraryWrapper = [](void *p) { FreeLibrary((HMODULE)p); };

  HMODULE moduleHandle;
  if (!GetModuleHandleExW(0, dllName, &moduleHandle)) {
    std::terminate();
  }

  std::unique_ptr<void, decltype(freeLibraryWrapper)> moduleHandleWrapper(moduleHandle, std::move(freeLibraryWrapper));

  HRSRC versionResourceHandle = FindResourceW(moduleHandle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

  if (!versionResourceHandle ||
      SizeofResource(static_cast<HMODULE>(moduleHandleWrapper.get()), versionResourceHandle) <
          sizeof(FileVersionInfoResource)) {
    std::terminate();
  }

  HGLOBAL versionResourcePtrHandle = LoadResource(moduleHandle, versionResourceHandle);

  if (!versionResourcePtrHandle) {
    std::terminate();
  }

  FileVersionInfoResource *versionInfo = static_cast<FileVersionInfoResource *>(LockResource(versionResourcePtrHandle));

  if (!versionInfo) {
    std::terminate();
  }

  uint64_t dllFileVersion = versionInfo->fixedFileInfo.dwFileVersionMS;
  dllFileVersion <<= 32;
  dllFileVersion |= versionInfo->fixedFileInfo.dwFileVersionLS;

  return dllFileVersion;
}

class ChakraCoreByteCodeBuffer : public facebook::jsi::Buffer {
 public:
  ChakraCoreByteCodeBuffer(ChakraObjectRef &&bytecodeArrayBuffer) : m_bytecodeArrayBuffer(bytecodeArrayBuffer) {}

  inline size_t size() const override {
    return GetArrayBufferLength(m_bytecodeArrayBuffer);
  }

  inline const uint8_t *data() const override {
    return GetArrayBufferData(m_bytecodeArrayBuffer);
  }

 private:
  ChakraObjectRef m_bytecodeArrayBuffer;
};

struct DebuggerCallbackCookie {
  std::shared_ptr<facebook::react::MessageQueueThread> m_queue{nullptr};
  std::shared_ptr<DebugProtocolHandler> m_debugProtocolHandler{nullptr};
};

} // namespace

ChakraCoreRuntime::ChakraCoreRuntime(std::shared_ptr<ChakraCoreRuntimeArgs> &&args) noexcept
    : ChakraRuntime(std::static_pointer_cast<ChakraRuntimeArgs>(args)),
      m_args(std::move(args)),
      m_runtimeIsAlive{std::make_shared<bool>(true)} {}

ChakraCoreRuntime::~ChakraCoreRuntime() noexcept {
  if (m_args->enableDebugging) {
    VerifyChakraErrorElseCrash(m_debugService->Close());
    VerifyChakraErrorElseCrash(m_debugService->UnregisterHandler(m_args->debuggerRuntimeName));
  }

  if (m_args->enableNativePromiseSupport) {
    VerifyChakraErrorElseCrash(JsSetPromiseContinuationCallback(nullptr, nullptr));
    VerifyChakraErrorElseCrash(JsSetHostPromiseRejectionTracker(nullptr, nullptr));
  }
}

facebook::jsi::Value ChakraCoreRuntime::evaluatePreparedJavaScript(
    const std::shared_ptr<const facebook::jsi::PreparedJavaScript> &preparedJs) {
  assert(preparedJs);

  std::shared_ptr<const ChakraPreparedJavaScript> chakraPreparedJs =
      std::static_pointer_cast<const ChakraPreparedJavaScript>(preparedJs);

  ChakraObjectRef sourceCode = ToJsArrayBuffer(chakraPreparedJs->SourceCode());
  ChakraObjectRef sourceUrl = ToJsString(std::string_view{chakraPreparedJs->SourceUrl()});
  ChakraObjectRef bytecode = ToJsArrayBuffer(chakraPreparedJs->Bytecode());

  JsValueRef result = nullptr;
  JsErrorCode error = JsRunSerialized(
      bytecode,
      [](JsSourceContext sourceContext, JsValueRef *value, JsParseScriptAttributes *parseAttributes) {
        *value = reinterpret_cast<JsValueRef>(sourceContext);
        *parseAttributes = JsParseScriptAttributeNone;
        return true;
      },
      reinterpret_cast<JsSourceContext>(JsRef(sourceCode)),
      sourceUrl,
      &result);

  // Fall back to evaluating source code if bytecode evaluation fails.
  if (error == JsErrorBadSerializedScript) {
    JsValueRef exception = nullptr;
    VerifyJsErrorElseThrow(JsGetAndClearException(&exception));
    return EvaluateJavaScriptImpl(sourceCode, sourceUrl);
  }

  VerifyJsErrorElseThrow(error);
  return ToJsiValue(ChakraObjectRef(result));
}

std::string ChakraCoreRuntime::description() {
  return "ChakraCoreRuntime";
}

facebook::jsi::WeakObject ChakraCoreRuntime::createWeakObject(const facebook::jsi::Object &obj) {
  JsWeakRef weakRef = nullptr;
  VerifyJsErrorElseThrow(JsCreateWeakReference(GetChakraObjectRef(obj), &weakRef));
  return make<facebook::jsi::WeakObject>(new WeakChakraPointerValue(weakRef));
}

facebook::jsi::Value ChakraCoreRuntime::lockWeakObject(const facebook::jsi::WeakObject &weakObj) {
  JsValueRef ref = nullptr;

  VerifyJsErrorElseThrow(
      JsGetWeakReferenceValue(static_cast<const WeakChakraPointerValue *>(getPointerValue(weakObj))->GetRef(), &ref));

  if (ref == JS_INVALID_REFERENCE) {
    return facebook::jsi::Value::undefined();
  }

  return ToJsiValue(ChakraObjectRef(ref));
}

facebook::jsi::JSRuntimeVersion_t ChakraCoreRuntime::Version() {
  return GetDllFileVersion(L"ChakraCore.dll");
}

facebook::jsi::Value ChakraCoreRuntime::EvaluateJavaScriptSimple(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer,
    const std::string &sourceUrl) {
  assert(buffer);
  ChakraObjectRef jsBuffer = ToJsArrayBuffer(buffer);
  ChakraObjectRef jsSourceUrl = ToJsString(std::string_view{sourceUrl});
  return EvaluateJavaScriptImpl(jsBuffer, jsSourceUrl);
}

std::shared_ptr<const facebook::jsi::Buffer> ChakraCoreRuntime::SerializeScript(
    const std::shared_ptr<const facebook::jsi::Buffer> &buffer) {
  assert(buffer);
  JsValueRef bytecode = JS_INVALID_REFERENCE;
  VerifyJsErrorElseThrow(JsSerialize(ToJsArrayBuffer(buffer), &bytecode, JsParseScriptAttributeNone));
  return std::make_shared<const ChakraCoreByteCodeBuffer>(ChakraObjectRef(bytecode));
}

void ChakraCoreRuntime::StartDebugging() {
  if (!m_args->enableDebugging) {
    return;
  }

  uint16_t port = m_args->debuggerPort;
  const std::string &runtimeName = m_args->debuggerRuntimeName;

  m_debugProtocolHandler = std::make_shared<DebugProtocolHandler>(GetRuntime());
  m_debugService = std::make_unique<DebugService>(GetRuntime());

  VerifyChakraErrorElseThrow(
      m_debugService->RegisterHandler(runtimeName, *m_debugProtocolHandler, m_args->debuggerBreakOnNextLine));

  // This ChakraCoreRuntime is guaranteed to outlive protocolHandler, so using
  // this pointer as a callback cookie is safe here.
  VerifyChakraErrorElseThrow(
      m_debugProtocolHandler->SetCommandQueueCallback(ProcessDebuggerCommandQueueCallback, this));

  VerifyChakraErrorElseThrow(m_debugService->Listen(port));

  m_args->debuggerLoggingCallback(
      (std::string("Listening on ws://127.0.0.1:") + std::to_string(port) + "/" + runtimeName).c_str(), LogLevel::Info);

  if (m_args->debuggerBreakOnNextLine) {
    m_args->debuggerLoggingCallback("Waiting for debugger to connect...", LogLevel::Info);

    m_debugProtocolHandler->WaitForDebugger();

    m_args->debuggerLoggingCallback("Debugger connected", LogLevel::Info);
  }
}

void ChakraCoreRuntime::InitializeAdditionalFeatures() {
  if (m_args->enableNativePromiseSupport) {
    // These callbacks are deregistered when this ChakraCoreRuntime get
    // destroyed, so using the this pointer as a callback cookie is OK.
    VerifyChakraErrorElseThrow(JsSetPromiseContinuationCallback(PromiseContinuationCallback, this));
    VerifyChakraErrorElseThrow(JsSetHostPromiseRejectionTracker(PromiseRejectionTrackerCallback, this));
  }
}

facebook::jsi::Value ChakraCoreRuntime::EvaluateJavaScriptImpl(
    const ChakraObjectRef &source,
    const ChakraObjectRef &sourceUrl) {
  JsValueRef result = nullptr;

  VerifyJsErrorElseThrow(JsRun(
      source,
      JS_SOURCE_CONTEXT_NONE /*sourceContext*/,
      sourceUrl,
      JsParseScriptAttributes::JsParseScriptAttributeNone,
      &result));

  return ToJsiValue(ChakraObjectRef(result));
}

void ChakraCoreRuntime::PromiseContinuation(ChakraObjectRef &&funcRef) noexcept {
  if (m_args->jsQueue) {
    std::weak_ptr<bool> runtimeIsAliveWeakRef{m_runtimeIsAlive};

    m_args->jsQueue->runOnQueue(
        [this, runtimeIsAliveWeakRef = std::move(runtimeIsAliveWeakRef), funcRef = std::move(funcRef)]() {
          std::shared_ptr<bool> runtimeIsAlive = runtimeIsAliveWeakRef.lock();
          if (runtimeIsAlive && *runtimeIsAlive) {
            JsValueRef undefinedValue;
            JsGetUndefinedValue(&undefinedValue);
            VerifyJsErrorElseThrow(JsCallFunction(funcRef, &undefinedValue, 1, nullptr));
          }
        });
  }
}

void ChakraCoreRuntime::PromiseRejectionTracker(ChakraObjectRef &&promise, ChakraObjectRef &&reason, bool handled) {
  if (!handled) {
    std::ostringstream errorStream;
    errorStream << "ChakraCore uncaught promise rejection: ";

    try {
      std::string stack = ToStdString(ToJsString(GetProperty(reason, GetPropertyId("stack"))));
      errorStream << stack;
    } catch (facebook::jsi::JSINativeException &error) {
      // When we aren't able to print stack, we just convert reason to a string.
      std::string reasonAsString = ToStdString(ToJsString(reason));
      errorStream << reasonAsString;
    }

    throw facebook::jsi::JSError(*this, MakePointer<facebook::jsi::String>(ToJsString(errorStream.str())));
  }
}

void CALLBACK ChakraCoreRuntime::PromiseContinuationCallback(JsValueRef funcRef, void *callbackState) {
  ChakraCoreRuntime *runtime = static_cast<ChakraCoreRuntime *>(callbackState);
  runtime->PromiseContinuation(ChakraObjectRef(funcRef));
}

void CALLBACK ChakraCoreRuntime::PromiseRejectionTrackerCallback(
    JsValueRef promise,
    JsValueRef reason,
    bool handled,
    void *callbackState) {
  ChakraCoreRuntime *runtime = static_cast<ChakraCoreRuntime *>(callbackState);
  runtime->PromiseRejectionTracker(ChakraObjectRef(promise), ChakraObjectRef(reason), handled);
}

void ChakraCoreRuntime::ProcessDebuggerCommandQueue() {
  if (m_args->jsQueue && m_debugProtocolHandler) {
    std::weak_ptr<DebugProtocolHandler> handlerWeakRef{m_debugProtocolHandler};

    m_args->jsQueue->runOnQueue([handlerWeakRef = std::move(handlerWeakRef)]() {
      std::shared_ptr<DebugProtocolHandler> handler = handlerWeakRef.lock();
      if (handler) {
        VerifyChakraErrorElseThrow(handler->ProcessCommandQueue());
      }
    });
  }
}

void CALLBACK ChakraCoreRuntime::ProcessDebuggerCommandQueueCallback(void *callbackState) {
  ChakraCoreRuntime *runtime = reinterpret_cast<ChakraCoreRuntime *>(callbackState);
  if (runtime) {
    runtime->ProcessDebuggerCommandQueue();
  }
}

} // namespace Microsoft::JSI

#endif
