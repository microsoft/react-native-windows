// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include <JSI/ByteArrayBuffer.h>
#include <JSI/ChakraCoreRuntime.h>
#include <JSI/ChakraRuntime.h>
#include <JSI/ChakraRuntimeFactory.h>

#include <cxxreact/MessageQueueThread.h>
#include "Unicode.h"

// This file contains non-edge-mode (or win32) implementations.
#if !defined(USE_EDGEMODE_JSRT)
#include <ChakraCore.h>
#include "ChakraCoreDebugger.h"

namespace Microsoft::JSI {

namespace {

#if !defined(CHAKRACORE_UWP)
// This is very wierd. This should match with the definition of VS_VERSIONINFO
// as defined in
// https://docs.microsoft.com/en-us/windows/desktop/menurc/vs-versioninfo I
// can't find a way to include the actual definition of VS_VERSIONINFO
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
#endif

} // namespace

ChakraCoreRuntime::ChakraCoreRuntime(ChakraRuntimeArgs &&args) noexcept : ChakraRuntime(std::move(args)) {
  this->Init();
}

ChakraCoreRuntime::~ChakraCoreRuntime() noexcept {
  stopDebuggingIfNeeded();
}

// TODO: We temporarily removed weak reference semantics from
// ChakraCore based jsi::Runtime.

void CALLBACK ChakraRuntime::PromiseRejectionTrackerCallback(
    JsValueRef promise,
    JsValueRef reason,
    bool handled,
    void *callbackState) {
  ChakraRuntime *runtime = static_cast<ChakraRuntime *>(callbackState);
  runtime->PromiseRejectionTracker(promise, reason, handled);
}

void ChakraRuntime::PromiseRejectionTracker(JsValueRef /*promise*/, JsValueRef reason, bool handled) {
  if (!handled) {
    std::ostringstream errorStream;
    errorStream << "ChakraCore uncaught promise rejection: ";

    JsPropertyIdRef stackPropertyID;
    JsErrorCode error = JsGetPropertyIdFromName(L"stack", &stackPropertyID);
    if (error == JsNoError) {
      JsValueRef stack;
      error = JsGetProperty(reason, stackPropertyID, &stack);
      if (error == JsNoError) {
        JsValueRef stackStrValue;
        error = JsConvertValueToString(stack, &stackStrValue);
        if (error == JsNoError) {
          errorStream << StringToStdString(stackStrValue);
        }
      }
    }

    if (error != JsNoError) {
      // weren't able to print stack, so just convert reason to a string
      JsValueRef strValue;
      error = JsConvertValueToString(reason, &strValue);
      if (error == JsNoError) {
        errorStream << StringToStdString(strValue);
      }
    }

    std::string errorString = errorStream.str();
    throw facebook::jsi::JSError(*this, createStringFromAscii(errorString.c_str(), errorString.length()));
  }
}

void ChakraCoreRuntime::setupNativePromiseContinuation() noexcept {
  if (runtimeArgs().enableNativePromiseSupport) {
    JsSetPromiseContinuationCallback(PromiseContinuationCallback, this);
    JsSetHostPromiseRejectionTracker(PromiseRejectionTrackerCallback, this);
  }
}

void ChakraCoreRuntime::startDebuggingIfNeeded() {
  auto &args = runtimeArgs();
  if (args.enableDebugging) {
    auto port = args.debuggerPort == 0 ? DebuggerDefaultPort : args.debuggerPort;
    auto runtimeName = args.debuggerRuntimeName.empty() ? DebuggerDefaultRuntimeName : args.debuggerRuntimeName;

    JsErrorCode result = enableDebugging(
        m_runtime,
        runtimeName,
        args.debuggerBreakOnNextLine,
        static_cast<uint16_t>(port),
        m_debugProtocolHandler,
        m_debugService);

    if (result == JsNoError) {
      m_debugPort = port;
      m_debugRuntimeName = runtimeName;
    }
  }

  if (args.debuggerBreakOnNextLine && m_debugProtocolHandler) {
    if (args.loggingCallback)
      args.loggingCallback("Waiting for debugger to connect...", LogLevel::Info);

    m_debugProtocolHandler->WaitForDebugger();

    if (args.loggingCallback)
      args.loggingCallback("Debugger connected", LogLevel::Info);
  }
}

void ChakraCoreRuntime::stopDebuggingIfNeeded() {
  if (m_debugService) {
    JsErrorCode result = m_debugService->Close();

    if (result == JsNoError) {
      result = m_debugService->UnregisterHandler(m_debugRuntimeName);
    }
  }
  m_debugService = nullptr;
  m_debugProtocolHandler = nullptr;
}

JsErrorCode ChakraCoreRuntime::enableDebugging(
    JsRuntimeHandle runtime,
    std::string const &runtimeName,
    bool breakOnNextLine,
    uint16_t port,
    std::unique_ptr<DebugProtocolHandler> &debugProtocolHandler,
    std::unique_ptr<DebugService> &debugService) {
  JsErrorCode result = JsNoError;
  auto protocolHandler = std::make_unique<DebugProtocolHandler>(runtime);
  auto service = std::make_unique<DebugService>(runtime);

  result = service->RegisterHandler(runtimeName, *protocolHandler, breakOnNextLine);

  if (result == JsNoError) {
    if (protocolHandler) {
      result = protocolHandler->SetCommandQueueCallback(ProcessDebuggerCommandQueueCallback, this);
    }
  }

  if (result == JsNoError) {
    result = service->Listen(port);

    if (runtimeArgs().loggingCallback)
      runtimeArgs().loggingCallback(
          (std::string("Listening on ws://127.0.0.1:") + std::to_string(port) + "/" + runtimeName).c_str(),
          LogLevel::Info);
  }

  if (result == JsNoError) {
    debugProtocolHandler = std::move(protocolHandler);
    debugService = std::move(service);
  }

  return result;
}

void ChakraCoreRuntime::ProcessDebuggerCommandQueue() {
  if (runtimeArgs().jsQueue) {
    runtimeArgs().jsQueue->runOnQueue([this]() {
      if (m_debugProtocolHandler) {
        m_debugProtocolHandler->ProcessCommandQueue();
      }
    });
  }
}

/* static */ void ChakraCoreRuntime::ProcessDebuggerCommandQueueCallback(void *callbackState) {
  ChakraCoreRuntime *runtime = reinterpret_cast<ChakraCoreRuntime *>(callbackState);

  if (runtime) {
    runtime->ProcessDebuggerCommandQueue();
  }
}

// TODO :: This code is mostly copied from the old ChakraExecutor flow, and not
// verified for reliability yet.
// TODO :: Re-evaluate this strategy of finding the dll version for versioning
// the runtime.
/*static*/ void ChakraRuntime::initRuntimeVersion() noexcept {
  // This code is win32 only at the moment. We will need to change this
  // line if we want to support UWP.
#if !defined(CHAKRACORE_UWP)
  constexpr wchar_t chakraDllName[] = L"ChakraCore.dll";

  auto freeLibraryWrapper = [](void *p) { FreeLibrary((HMODULE)p); };
  HMODULE moduleHandle;
  if (!GetModuleHandleExW(0, chakraDllName, &moduleHandle))
    return;

  std::unique_ptr<void, decltype(freeLibraryWrapper)> moduleHandleWrapper{moduleHandle, std::move(freeLibraryWrapper)};

  HRSRC versionResourceHandle = FindResourceW(moduleHandle, MAKEINTRESOURCE(VS_VERSION_INFO), RT_VERSION);

  if (!versionResourceHandle ||
      SizeofResource(static_cast<HMODULE>(moduleHandleWrapper.get()), versionResourceHandle) <
          sizeof(FileVersionInfoResource))
    return;

  HGLOBAL versionResourcePtrHandle = LoadResource(moduleHandle, versionResourceHandle);
  if (!versionResourcePtrHandle)
    return;

  FileVersionInfoResource *chakraVersionInfo =
      static_cast<FileVersionInfoResource *>(LockResource(versionResourcePtrHandle));
  if (!chakraVersionInfo)
    return;

  s_runtimeVersion = chakraVersionInfo->fixedFileInfo.dwFileVersionMS;
  s_runtimeVersion <<= 32;
  s_runtimeVersion |= chakraVersionInfo->fixedFileInfo.dwFileVersionLS;
#endif
}

std::unique_ptr<const facebook::jsi::Buffer> ChakraCoreRuntime::generatePreparedScript(
    const std::string &sourceURL,
    const facebook::jsi::Buffer &sourceBuffer) noexcept {
  const std::wstring scriptUTF16 =
      Microsoft::Common::Unicode::Utf8ToUtf16(reinterpret_cast<const char *>(sourceBuffer.data()), sourceBuffer.size());

  unsigned int bytecodeSize = 0;
  if (JsSerializeScript(scriptUTF16.c_str(), nullptr, &bytecodeSize) == JsNoError) {
    std::unique_ptr<ByteArrayBuffer> bytecodeBuffer(std::make_unique<ByteArrayBuffer>(bytecodeSize));
    if (JsSerializeScript(scriptUTF16.c_str(), bytecodeBuffer->data(), &bytecodeSize) == JsNoError) {
      return bytecodeBuffer;
    }
  }

  return nullptr;
}

facebook::jsi::Value ChakraCoreRuntime::evaluateJavaScriptSimple(
    const facebook::jsi::Buffer &buffer,
    const std::string &sourceURL) {
  JsValueRef sourceRef;
  JsCreateString(reinterpret_cast<const char *>(buffer.data()), buffer.size(), &sourceRef);

  JsValueRef sourceURLRef = nullptr;
  JsCreateString(reinterpret_cast<const char *>(sourceURL.c_str()), sourceURL.size(), &sourceURLRef);

  JsValueRef result;
  ChakraVerifyJsErrorElseThrow(
      JsRun(sourceRef, 0, sourceURLRef, JsParseScriptAttributes::JsParseScriptAttributeNone, &result));

  return ToJsiValue(result);
}

bool ChakraCoreRuntime::evaluateSerializedScript(
    const facebook::jsi::Buffer &scriptBuffer,
    const facebook::jsi::Buffer &serializedScriptBuffer,
    const std::string &sourceURL,
    JsValueRef *result) {
  JsValueRef bytecodeArrayBuffer = nullptr;
  if (JsCreateExternalArrayBuffer(
          const_cast<uint8_t *>(serializedScriptBuffer.data()),
          static_cast<unsigned int>(serializedScriptBuffer.size()),
          nullptr,
          nullptr,
          &bytecodeArrayBuffer) == JsNoError) {
    JsValueRef sourceURLRef = nullptr;
    if (!sourceURL.empty()) {
      sourceURLRef = PointerToString(sourceURL);
    }

    JsErrorCode errorCode = JsRunSerialized(
        bytecodeArrayBuffer,
        [](JsSourceContext sourceContext, JsValueRef *value, JsParseScriptAttributes *parseAttributes) {
          const facebook::jsi::Buffer *scriptSource = reinterpret_cast<const facebook::jsi::Buffer *>(sourceContext);
          if (JsCreateExternalArrayBuffer(
                  const_cast<uint8_t *>(scriptSource->data()),
                  static_cast<unsigned int>(scriptSource->size()),
                  nullptr,
                  nullptr,
                  value) != JsNoError)
            std::terminate();

          *parseAttributes = JsParseScriptAttributeNone;
          return true;
        },
        reinterpret_cast<JsSourceContext>(m_pinnedScripts.back().get()),
        sourceURLRef,
        result);

    if (errorCode == JsNoError) {
      return true;
    } else if (errorCode == JsErrorBadSerializedScript) {
      return false;
    } else {
      ChakraVerifyJsErrorElseThrow(errorCode);
    }
  }

  return false;
}

std::unique_ptr<facebook::jsi::Runtime> MakeChakraCoreRuntime(ChakraRuntimeArgs &&args) noexcept {
  return std::make_unique<ChakraCoreRuntime>(std::move(args));
}

} // namespace Microsoft::JSI

#endif
