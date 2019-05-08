// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraJsiRuntime.h"
#include "UnicodeConversion.h"

#include <cxxreact/MessageQueueThread.h>

// This file contains non-edge-mode (or win32) implementations.
#if !defined(USE_EDGEMODE_JSRT)
#include <ChakraCore.h>

#if !defined(JSI_CORE)
#define CHAKRART_HAS_DEBUGGER
#endif

#if defined(CHAKRART_HAS_DEBUGGER)
#include "ChakraCoreDebugger.h"
#endif

namespace facebook { 
namespace jsi {
namespace chakraruntime {

void ChakraJsiRuntime::evaluateJavaScriptSimple(const jsi::Buffer& buffer, const std::string& sourceURL) {
  JsValueRef sourceRef;
  JsCreateString(reinterpret_cast<const char*>(buffer.data()), buffer.size(), &sourceRef);

  JsValueRef sourceURLRef = nullptr;
  if (!sourceURL.empty()) {
    JsCreateString(reinterpret_cast<const char*>(sourceURL.c_str()), sourceURL.size(), &sourceURLRef);
  }

  JsValueRef result;
  checkException(JsRun(sourceRef, 0, sourceURLRef, JsParseScriptAttributes::JsParseScriptAttributeNone, &result), sourceURL.c_str());
}

bool ChakraJsiRuntime::evaluateSerializedScript(const jsi::Buffer& scriptBuffer, const jsi::Buffer& serializedScriptBuffer, const std::string& sourceURL) {
  JsValueRef bytecodeArrayBuffer = nullptr;
  if (JsCreateExternalArrayBuffer(const_cast<uint8_t *>(serializedScriptBuffer.data()), static_cast<unsigned int>(serializedScriptBuffer.size()), nullptr, nullptr, &bytecodeArrayBuffer) == JsNoError) {

    JsValueRef sourceURLRef = nullptr;
    if (!sourceURL.empty()) {
      sourceURLRef = createJSString(reinterpret_cast<const char*>(sourceURL.c_str()), sourceURL.size());
    }

    JsValueRef value = nullptr;
    JsErrorCode result = JsRunSerialized(bytecodeArrayBuffer, [](JsSourceContext sourceContext, JsValueRef *value, JsParseScriptAttributes *parseAttributes)
    {
      const jsi::Buffer* scriptSource = reinterpret_cast<const jsi::Buffer*>(sourceContext);
      if (JsCreateExternalArrayBuffer(const_cast<uint8_t *>(scriptSource->data()), static_cast<unsigned int>(scriptSource->size()), nullptr, nullptr, value) != JsNoError)
        std::terminate();

      *parseAttributes = JsParseScriptAttributeNone;
      return true;
    }, reinterpret_cast<JsSourceContext>(m_pinnedScripts.back().get()), sourceURLRef, &value);

    if (result == JsNoError) {
      return true;
    }
    else if (result == JsErrorBadSerializedScript) {
      return false;
    }
    else {
      checkException(result);
    }
  }

  return false;
}

std::unique_ptr<const jsi::Buffer> ChakraJsiRuntime::generatePreparedScript(const std::string& sourceURL, const jsi::Buffer& sourceBuffer) noexcept {
  const std::wstring scriptUTF16 = facebook::react::UnicodeConversion::Utf8ToUtf16(reinterpret_cast<const char*>(sourceBuffer.data()), sourceBuffer.size());

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

// Note :: ChakraCore header provides an API which takes 8-bit string .. which is not available in edge mode.
JsValueRef ChakraJsiRuntime::createJSString(const char*data, size_t length) {
  JsValueRef value;
  JsCreateString(reinterpret_cast<const char*>(data), length, &value);
  return value;
}

// Note :: ChakraCore header provides an API which takes 8-bit string .. which is not available in edge mode.
JsValueRef ChakraJsiRuntime::createJSPropertyId(const char*data, size_t length) {
  JsValueRef propIdRef;
  if (JsNoError != JsCreatePropertyId(data, length, &propIdRef)) std::terminate();
  return propIdRef;
}

// ES6 Promise callback
void CALLBACK ChakraJsiRuntime::PromiseContinuationCallback(JsValueRef funcRef, void* callbackState)  noexcept {
  ChakraJsiRuntime* runtime = static_cast<ChakraJsiRuntime*>(callbackState);
  runtime->promiseContinuation(funcRef);
}

void ChakraJsiRuntime::promiseContinuation(JsValueRef funcRef)  noexcept {
  runtimeArgs().promiseContinuation->registerPromise(createObject(funcRef).getFunction(*this));
}

void ChakraJsiRuntime::setupNativePromiseContinuation() noexcept{
  if (m_args.promiseContinuation) {
    JsSetPromiseContinuationCallback(PromiseContinuationCallback, this);
  }
}

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
/*static*/ void ChakraJsiRuntime::initRuntimeVersion()  noexcept {
  // This code is win32 only at the moment. We will need to change this
  // line if we want to support UWP.
  constexpr wchar_t chakraDllName[] = L"ChakraCore.dll";

  auto freeLibraryWrapper = [](void* p) { FreeLibrary((HMODULE)p); };
  HMODULE moduleHandle;
  if (!GetModuleHandleExW(0, chakraDllName, &moduleHandle))
    return;

  std::unique_ptr<void, decltype(freeLibraryWrapper)> moduleHandleWrapper{ moduleHandle, std::move(freeLibraryWrapper) };

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

#ifdef CHAKRART_HAS_DEBUGGER
class ChakraJsiRuntimeWithDebugger : public ChakraJsiRuntime {
public:
  ChakraJsiRuntimeWithDebugger(ChakraJsiRuntimeArgs&& args);
  ~ChakraJsiRuntimeWithDebugger();

private:
  JsErrorCode enableDebugging(JsRuntimeHandle runtime, std::string const& runtimeName, bool breakOnNextLine, uint16_t port, std::unique_ptr<DebugProtocolHandler>& debugProtocolHandler, std::unique_ptr<DebugService>& debugService);
  void ProcessDebuggerCommandQueue();

  static void CHAKRA_CALLBACK ProcessDebuggerCommandQueueCallback(void* callbackState);

  constexpr static char DebuggerDefaultRuntimeName[] = "runtime1";
  constexpr static int DebuggerDefaultPort = 9229;

  std::string debugRuntimeName_;
  int debugPort_ { 0 };
  std::unique_ptr<DebugProtocolHandler> debugProtocolHandler_;
  std::unique_ptr<DebugService> debugService_;
};

ChakraJsiRuntimeWithDebugger::ChakraJsiRuntimeWithDebugger(ChakraJsiRuntimeArgs&& args)
  : ChakraJsiRuntime{ std::move(args) } {
  bool enableDebugging = runtimeArgs().enableDebugging;
  bool breakOnNextLine = runtimeArgs().debuggerBreakOnNextLine;

  if (enableDebugging)
  {
    std::string runtimeName = runtimeArgs().debuggerRuntimeName;
    int port = runtimeArgs().debuggerPort;

    if (runtimeName.empty()) {
      runtimeName = DebuggerDefaultRuntimeName;
    }

    if (port == 0) {
      port = DebuggerDefaultPort;
    }

    JsErrorCode result = this->enableDebugging(m_runtime, runtimeName, breakOnNextLine, static_cast<uint16_t>(port), debugProtocolHandler_, debugService_);

    if (result == JsNoError) {
      debugPort_ = port;
      debugRuntimeName_ = runtimeName;
    }
  }

  if (breakOnNextLine && debugProtocolHandler_) {
    if(runtimeArgs().loggingCallback) 
      runtimeArgs().loggingCallback("Waiting for debugger to connect...", facebook::jsi::chakraruntime::LogLevel::Info);
    
    debugProtocolHandler_->WaitForDebugger();
    
    if (runtimeArgs().loggingCallback) 
      runtimeArgs().loggingCallback("Debugger connected", facebook::jsi::chakraruntime::LogLevel::Info);
  }
}

ChakraJsiRuntimeWithDebugger::~ChakraJsiRuntimeWithDebugger() {
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
  JsSetRuntimeMemoryAllocationCallback(m_runtime, nullptr, nullptr);
}

JsErrorCode ChakraJsiRuntimeWithDebugger::enableDebugging(JsRuntimeHandle runtime, std::string const& runtimeName, bool breakOnNextLine, uint16_t port,
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

    if (runtimeArgs().loggingCallback)
      runtimeArgs().loggingCallback((std::string("Listening on ws://127.0.0.1:") + std::to_string(port) + "/" + runtimeName).c_str(), facebook::jsi::chakraruntime::LogLevel::Info);
  }

  if (result == JsNoError) {
    debugProtocolHandler = std::move(protocolHandler);
    debugService = std::move(service);
  }

  return result;
}

/* static */ void ChakraJsiRuntimeWithDebugger::ProcessDebuggerCommandQueueCallback(void* callbackState) {
  ChakraJsiRuntimeWithDebugger* runtime= reinterpret_cast<ChakraJsiRuntimeWithDebugger*>(callbackState);

  if (runtime) {
    runtime->ProcessDebuggerCommandQueue();
  }
}

void ChakraJsiRuntimeWithDebugger::ProcessDebuggerCommandQueue() {
  if (runtimeArgs().jsQueue) {
    runtimeArgs().jsQueue->runOnQueue([this]() {
      if (debugProtocolHandler_) {
        debugProtocolHandler_->ProcessCommandQueue();
      }
    });
  }
}

#endif

std::unique_ptr<jsi::Runtime> makeChakraRuntime(ChakraJsiRuntimeArgs&& args) noexcept {
#ifdef CHAKRART_HAS_DEBUGGER
  return std::make_unique<ChakraJsiRuntimeWithDebugger>(std::move(args));
#else
  return std::make_unique<ChakraJsiRuntime>(std::move(args));
#endif
}

}}} // namespace facebook::jsi::chakraruntime

#endif
