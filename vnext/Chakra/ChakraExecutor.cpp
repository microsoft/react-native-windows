// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ChakraExecutor.h"

#include <fcntl.h>
#include <folly/Conv.h>
#include <folly/Exception.h>
#include <folly/Memory.h>
#include <folly/String.h>
#include <folly/json.h>
#include <glog/logging.h>
#include <algorithm>
#include <condition_variable>
#include <mutex>
#include <sstream>
#include <string>

#include <cxxreact/JSModulesUnbundle.h>
#include <cxxreact/RAMBundleRegistry.h>
#include <cxxreact/SystraceSection.h>

#include <cxxreact/ModuleRegistry.h>
#if !defined(OSS_RN)
#include <cxxreact/Platform.h>
#endif

#include "ChakraNativeModules.h"
#include "ChakraPlatform.h"
#include "ChakraTracing.h"
#include "ChakraUtils.h"

#include <MemoryTracker.h>

namespace facebook {
namespace react {

struct JSContextHolder {
  JSContextHolder(JsContextRef context) {
    if (JsNoError != JsSetCurrentContext(context))
      std::terminate();
  }

  ~JSContextHolder() {
    if (JsNoError != JsSetCurrentContext(JS_INVALID_REFERENCE))
      std::terminate();
  }
};

template <JsValueRef (ChakraExecutor::*method)(size_t, const JsValueRef[])>
JsNativeFunction exceptionWrapMethod() {
  struct funcWrapper {
    static JsValueRef __stdcall call(
        JsValueRef function,
        bool isConstructCall,
        JsValueRef arguments[],
        unsigned short argumentCount,
        void *callbackState) {
      JsContextRef ctx;
      JsGetCurrentContext(&ctx);
      try {
        const JsValueRef *pArguments;
        JsValueRef thisObj;
        if (argumentCount > 0) {
          thisObj = arguments[0];
          pArguments = arguments + 1;
          argumentCount--;
        } else {
          JsGetNullValue(&thisObj);
          pArguments = arguments;
        }
        ChakraExecutor *executor = nullptr;
        JsGetContextData(ctx, (void **)&executor);
        return (executor->*method)(argumentCount, pArguments);
      } catch (...) {
        return translatePendingCppExceptionToJSError(function);
      }
    }
  };

  return &funcWrapper::call;
}

namespace {

template <JsValueRef (
    ChakraExecutor::*method)(JsValueRef object, JsValueRef propertyName)>
JsNativeFunction exceptionWrapMethod() {
  struct funcWrapper {
    static JsValueRef call(
        JsValueRef function,
        bool isConstructCall,
        JsValueRef arguments[],
        unsigned short argumentCount,
        void *callbackState) {
      JsContextRef ctx;
      JsGetCurrentContext(&ctx);
      try {
        JsValueRef thisObj = argumentCount > 0
            ? arguments[0]
            : (JsGetNullValue(&thisObj), thisObj);
        JsValueRef thisPropertyName = argumentCount > 1
            ? arguments[1]
            : (JsGetNullValue(&thisPropertyName), thisPropertyName);
        ChakraExecutor *executor = nullptr;
        JsGetContextData(ctx, (void **)&executor);
        return (executor->*method)(thisObj, thisPropertyName);
      } catch (...) {
        return translatePendingCppExceptionToJSError(function);
      }
    }
  };

  return &funcWrapper::call;
}

} // anonymous namespace

std::unique_ptr<JSExecutor> ChakraExecutorFactory::createJSExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> jsQueue) {
#if _DEBUG
  // as we're moving m_instanceArgs, we can't call this more than once
  assert(++m_executorCreationCount == 1);
#endif
  return std::unique_ptr<JSExecutor>(
      new ChakraExecutor(delegate, jsQueue, std::move(m_instanceArgs)));
}

ChakraExecutor::ChakraExecutor(
    std::shared_ptr<ExecutorDelegate> delegate,
    std::shared_ptr<MessageQueueThread> messageQueueThread,
    ChakraInstanceArgs &&instanceArgs)
    : m_delegate(delegate),
      m_messageQueueThread(messageQueueThread),
      m_nativeModules(delegate ? delegate->getModuleRegistry() : nullptr),
      m_instanceArgs(std::move(instanceArgs)) {
  initOnJSVMThread();

  {
    SystraceSection s("nativeModuleProxy object");

    // For Windows, we don't use nativeModuleProxy. Instead, we register the
    // native module configs.
    // installGlobalProxy(m_context, "nativeModuleProxy",
    //                   exceptionWrapMethod<&ChakraExecutor::getNativeModule>());
  }
}

ChakraExecutor::~ChakraExecutor() {
  CHECK(*m_isDestroyed)
      << "ChakraExecutor::destroy() must be called before its destructor!";
}

void ChakraExecutor::destroy() {
  *m_isDestroyed = true;
  if (m_messageQueueThread.get()) {
    m_messageQueueThread->runOnQueueSync([this]() { terminateOnJSVMThread(); });
  } else {
    terminateOnJSVMThread();
  }
}

void ChakraExecutor::setContextName(const std::string &name) {
  ChakraString jsName = ChakraString(name.c_str());
  // Chakra_GlobalContextSetName( jsName);
}

// This class tracks the created runtimes on a given thread, through the
// threadLocal tls_runtimeTracker variable
struct JsRuntimeTracker {
  // Handle to the runtime
  JsRuntimeHandle RuntimeHandle{nullptr};

  // Number of "usage counts", e.g. number of Chakra Contexts for this runtime
  uint32_t RefCount{0};

  std::shared_ptr<MemoryTracker> MemoryTracker{nullptr};

#if !defined(USE_EDGEMODE_JSRT)
  std::string DebugRuntimeName;
  int DebugPort{0};
  std::unique_ptr<DebugProtocolHandler> DebugProtocolHandler;
  std::unique_ptr<DebugService> DebugService;
#endif
};

#if !defined(USE_EDGEMODE_JSRT)

#define IfFailRet(v)          \
  {                           \
    JsErrorCode error = (v);  \
    if (error != JsNoError) { \
      return error;           \
    }                         \
  }

JsErrorCode ChakraExecutor::RedirectConsoleToDebugger(
    JsValueRef debuggerConsoleObject) {
  JsValueRef globalObject = JS_INVALID_REFERENCE;
  IfFailRet(JsGetGlobalObject(&globalObject));

  JsPropertyIdRef consolePropertyId = JS_INVALID_REFERENCE;
  IfFailRet(JsGetPropertyIdFromName(L"console", &consolePropertyId));

  JsValueRef consoleObject = JS_INVALID_REFERENCE;
  IfFailRet(JsGetProperty(globalObject, consolePropertyId, &consoleObject));

  JsValueRef undefinedValue = JS_INVALID_REFERENCE;
  IfFailRet(JsGetUndefinedValue(&undefinedValue));

  if (consoleObject == JS_INVALID_REFERENCE ||
      consoleObject == undefinedValue) {
    return JsErrorNotImplemented;
  }

  const char *script =
      "function patchConsoleObject$$1(global, console, debugConsole) {\n"
      "var obj = {};\n"
      "for (var fn in console) {\n"
      "if (typeof console[fn] === \"function\") {\n"
      "(function(name) {\n"
      "obj[name] = function(...args) {\n"
      "console[name](...args);\n"
      "if (name in debugConsole && typeof debugConsole[name] === \"function\") {\n"
      "debugConsole[name](...args);\n"
      "}\n"
      "}\n"
      "})(fn);\n"
      "}\n"
      "}\n"
      "global.console = obj;\n"
      "}\n"
      "patchConsoleObject$$1;\n";

  JsValueRef patchFunction = JS_INVALID_REFERENCE;

  JsValueRef scriptUrl = JS_INVALID_REFERENCE;
  IfFailRet(JsCreateString("", 0, &scriptUrl));

  JsValueRef scriptContentValue = JS_INVALID_REFERENCE;
  IfFailRet(JsCreateString(script, strlen(script), &scriptContentValue));
  IfFailRet(JsRun(
      scriptContentValue,
      JS_SOURCE_CONTEXT_NONE,
      scriptUrl,
      JsParseScriptAttributeLibraryCode,
      &patchFunction));

  JsValueRef args[4] = {
      undefinedValue, globalObject, consoleObject, debuggerConsoleObject};
  IfFailRet(JsCallFunction(
      patchFunction, args, _countof(args), nullptr /*no return value*/));

  return JsNoError;
}

#endif

static thread_local JsRuntimeTracker tls_runtimeTracker;

void ChakraExecutor::initOnJSVMThread() {
  SystraceSection s("ChakraExecutor.initOnJSVMThread");

  bool enableDebugging = m_instanceArgs.EnableDebugging;
  bool breakOnNextLine = m_instanceArgs.DebuggerBreakOnNextLine;
  bool enableNativePerformanceNow = m_instanceArgs.EnableNativePerformanceNow;

  // if there's not already a runtime
  if (tls_runtimeTracker.RuntimeHandle == nullptr) {
    // create a runtime
    CHAKRA_ASSERTDO(JsCreateRuntime(
        m_instanceArgs.RuntimeAttributes,
        nullptr,
        &tls_runtimeTracker.RuntimeHandle));

#if !defined(USE_EDGEMODE_JSRT)
    // if desired, enable debugging for the runtime
    if (enableDebugging) {
      std::string runtimeName = m_instanceArgs.DebuggerRuntimeName;
      auto port = m_instanceArgs.DebuggerPort;

      if (runtimeName.empty()) {
        runtimeName = "runtime1";
      }

      if (port == 0) {
        port = 9229;
      }

      JsErrorCode result = this->enableDebugging(
          tls_runtimeTracker.RuntimeHandle,
          runtimeName,
          breakOnNextLine,
          port,
          tls_runtimeTracker.DebugProtocolHandler,
          tls_runtimeTracker.DebugService);

      if (result == JsNoError) {
        tls_runtimeTracker.DebugPort = port;
        tls_runtimeTracker.DebugRuntimeName = runtimeName;
      }
    }
#endif
  }

  if (!tls_runtimeTracker.RuntimeHandle)
    throw std::runtime_error("ChakraCore init failed.");

  tls_runtimeTracker.MemoryTracker = m_instanceArgs.MemoryTracker;

  // Although the lambda has access to tls_runtimeTracker, we have to call
  // reportMemory(De)Allocation through a pointer. This is because ChakraCore
  // may invoke this callback on a different thread, in which case
  // tls_runtimeTracker will refer to a different thread local storage.
  JsSetRuntimeMemoryAllocationCallback(
      tls_runtimeTracker.RuntimeHandle,
      tls_runtimeTracker.MemoryTracker.get(),
      [](void *callbackState,
         JsMemoryEventType allocationEvent,
         size_t allocationSize) -> bool {
        auto memoryTrackerPtr = static_cast<MemoryTracker *>(callbackState);
        switch (allocationEvent) {
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
      });

  size_t initialMemoryUsage = 0;
  JsGetRuntimeMemoryUsage(
      tls_runtimeTracker.RuntimeHandle, &initialMemoryUsage);
  tls_runtimeTracker.MemoryTracker->Initialize(initialMemoryUsage);

  if (m_instanceArgs.RuntimeMemoryLimit > 0)
    CHAKRA_ASSERTDO(JsSetRuntimeMemoryLimit(
        tls_runtimeTracker.RuntimeHandle, m_instanceArgs.RuntimeMemoryLimit));

  m_context = nullptr;
  // Create an execution context
  JsCreateContext(tls_runtimeTracker.RuntimeHandle, &m_context);
  // Increment the usage count on the runtime
  ++tls_runtimeTracker.RefCount;
  // Now set the current execution context
  if (JsNoError != JsSetCurrentContext(m_context))
    std::terminate();
  // Keep this context alive until terminate is called
  JsAddRef(m_context, nullptr);

  // Add a pointer to ourselves so we can retrieve it later in our hooks
  JsSetContextData(m_context, this);

#if defined(USE_EDGEMODE_JSRT)
  if (enableDebugging)
    JsStartDebugging();
#else
  if (enableDebugging && m_instanceArgs.DebuggerConsoleRedirection) {
    JsValueRef debuggerConsoleObject;
    tls_runtimeTracker.DebugProtocolHandler->GetConsoleObject(
        &debuggerConsoleObject);

    JsValueRef undefinedValue = JS_INVALID_REFERENCE;
    JsGetUndefinedValue(&undefinedValue);

    if (debuggerConsoleObject != JS_INVALID_REFERENCE &&
        debuggerConsoleObject != undefinedValue) {
      needToRedirectConsoleToDebugger = true;
    }
  }
#endif

  installNativeHook<&ChakraExecutor::nativeFlushQueueImmediate>(
      "nativeFlushQueueImmediate");
  installNativeHook<&ChakraExecutor::nativeCallSyncHook>("nativeCallSyncHook");
  installNativeHook<&ChakraExecutor::nativeLoggingHook>("nativeLoggingHook");

  if (enableNativePerformanceNow) {
    installGlobalFunction("nativePerformanceNow", JSNativeHooks::nowHookJNF);
  }

// JS Tracing enabled only in verbose mode. 
#ifdef ENABLE_JS_SYSTRACE
  addNativeTracingHooks();
  setGlobalVariable(
      "__RCTProfileIsProfiling", std::make_unique<JSBigStdString>("true"));
#endif

#if !defined(USE_EDGEMODE_JSRT)
  if (breakOnNextLine && tls_runtimeTracker.DebugProtocolHandler) {
    std::cout << "Waiting for debugger to connect..." << std::endl;
    tls_runtimeTracker.DebugProtocolHandler->WaitForDebugger();
    std::cout << "Debugger connected" << std::endl;
  }
#endif
}

void ChakraExecutor::terminateOnJSVMThread() {
  m_nativeModules.reset();

  // Get rid of the usage/runtime for the thread
  if (!tls_runtimeTracker.RuntimeHandle)
    throw std::runtime_error("ChakraCore runtime handle is empty.");
  if (tls_runtimeTracker.RefCount == 0)
    throw std::runtime_error("ChakraCore doesn't have any references to it.");
  --tls_runtimeTracker.RefCount;

  if (tls_runtimeTracker.RefCount == 0) {
#if !defined(USE_EDGEMODE_JSRT)
    if (tls_runtimeTracker.DebugService) {
      JsErrorCode result = tls_runtimeTracker.DebugService->Close();

      if (result == JsNoError) {
        result = tls_runtimeTracker.DebugService->UnregisterHandler(
            tls_runtimeTracker.DebugRuntimeName);
      }
    }
    tls_runtimeTracker.DebugService = nullptr;
    tls_runtimeTracker.DebugProtocolHandler = nullptr;
#endif
  }

  JsSetContextData(m_context, nullptr);
  if (JsNoError != JsSetCurrentContext(JS_INVALID_REFERENCE))
    std::terminate();

  // Release the ref we added on context to keep it alive
  JsRelease(m_context, nullptr);

  // Reset runtime callback.
  JsSetRuntimeMemoryAllocationCallback(
      tls_runtimeTracker.RuntimeHandle, nullptr, nullptr);

  // dispose runtime only if there are no more contexts for it
  if (tls_runtimeTracker.RefCount == 0) {
    JsDisposeRuntime(tls_runtimeTracker.RuntimeHandle);
    tls_runtimeTracker.RuntimeHandle = nullptr;
    tls_runtimeTracker.MemoryTracker = nullptr;
  }

  m_context = nullptr;
}

#if !defined(USE_EDGEMODE_JSRT)
JsErrorCode ChakraExecutor::enableDebugging(
    JsRuntimeHandle runtime,
    std::string const &runtimeName,
    bool breakOnNextLine,
    uint16_t port,
    std::unique_ptr<DebugProtocolHandler> &debugProtocolHandler,
    std::unique_ptr<DebugService> &debugService) {
  SystraceSection s("ChakraExecutor.enableDebugging");

  JsErrorCode result = JsNoError;
  auto protocolHandler = std::make_unique<DebugProtocolHandler>(runtime);
  auto service = std::make_unique<DebugService>(runtime);

  result =
      service->RegisterHandler(runtimeName, *protocolHandler, breakOnNextLine);

  if (result == JsNoError) {
    if (protocolHandler) {
      result = protocolHandler->SetCommandQueueCallback(
          ProcessDebuggerCommandQueueCallback, this);
    }
  }

  if (result == JsNoError) {
    result = service->Listen(port);

    std::cout << "Listening on ws://127.0.0.1:" << port << "/" << runtimeName
              << std::endl;
  }

  if (result == JsNoError) {
    debugProtocolHandler = std::move(protocolHandler);
    debugService = std::move(service);
  }

  return result;
}
#endif

#if !defined(USE_EDGEMODE_JSRT)
/* static */ void ChakraExecutor::ProcessDebuggerCommandQueueCallback(
    void *callbackState) {
  ChakraExecutor *executor = reinterpret_cast<ChakraExecutor *>(callbackState);

  if (executor) {
    executor->ProcessDebuggerCommandQueue();
  }
}
#endif

#if !defined(USE_EDGEMODE_JSRT)
void ChakraExecutor::ProcessDebuggerCommandQueue() {
  if (m_messageQueueThread.get()) {
    m_messageQueueThread->runOnQueue([this]() {
      if (tls_runtimeTracker.DebugProtocolHandler) {
        tls_runtimeTracker.DebugProtocolHandler->ProcessCommandQueue();
      }
    });
  }
}
#endif

// basename_r isn't in all iOS SDKs, so use this simple version instead.
static std::string simpleBasename(const std::string &path) {
  size_t pos = path.rfind("/");
  return (pos != std::string::npos) ? path.substr(pos) : path;
}

void ChakraExecutor::loadApplicationScript(
    std::unique_ptr<const JSBigString> script,
#if !defined(OSS_RN)
    uint64_t scriptVersion,
#endif
    std::string sourceURL
#if !defined(OSS_RN)
    ,
    std::string &&bytecodeFileName
#endif
) {
  SystraceSection s(
      "ChakraExecutor::loadApplicationScript", "sourceURL", sourceURL);

  JSContextHolder ctx(m_context);

  std::string scriptName = simpleBasename(sourceURL);
#if !defined(OSS_RN)
  ReactMarker::logTaggedMarker(
      ReactMarker::RUN_JS_BUNDLE_START, scriptName.c_str());
#endif

  ChakraString jsSourceURL(sourceURL.c_str());

  // when debugging is enabled, don't use bytecode caching because ChakraCore
  // doesn't support it.
#if !defined(OSS_RN)
  if (bytecodeFileName.empty() || m_instanceArgs.EnableDebugging)
#endif
  {
    evaluateScript(std::move(script), jsSourceURL);
  }
#if !defined(OSS_RN)
  else {
    evaluateScriptWithBytecode(
        std::move(script),
        scriptVersion,
        jsSourceURL,
        std::move(bytecodeFileName));
  }
#endif

#if !defined(USE_EDGEMODE_JSRT)
  if (needToRedirectConsoleToDebugger) {
    JsValueRef debuggerConsoleObject;
    tls_runtimeTracker.DebugProtocolHandler->GetConsoleObject(
        &debuggerConsoleObject);

    JsErrorCode result = RedirectConsoleToDebugger(debuggerConsoleObject);
    if (result == JsNoError) {
      needToRedirectConsoleToDebugger = false;
    }
  }
#endif

  // TODO(luk): t13903306 Remove this check once we make native modules working
  // for java2js
  if (m_delegate) {
    bindBridge();
    flush();
  }

#if !defined(OSS_RN)
  ReactMarker::logMarker(ReactMarker::CREATE_REACT_CONTEXT_STOP);
  ReactMarker::logTaggedMarker(
      ReactMarker::RUN_JS_BUNDLE_STOP, scriptName.c_str());
#endif
}

void ChakraExecutor::setBundleRegistry(
    std::unique_ptr<RAMBundleRegistry> bundleRegistry) {
  if (!m_bundleRegistry) {
    installNativeHook<&ChakraExecutor::nativeRequire>("nativeRequire");
  }
  m_bundleRegistry = std::move(bundleRegistry);
}

void ChakraExecutor::registerBundle(
    uint32_t bundleId,
    const std::string &bundlePath) {
  // NYI
  std::terminate();
}

void ChakraExecutor::bindBridge() noexcept {
  SystraceSection s("ChakraExecutor::bindBridge");
  if (m_bridgeEstablished) {
    return;
  }
  auto global = ChakraObject::getGlobalObject();
  auto batchedBridgeValue = global.getProperty("__fbBatchedBridge");
  if (batchedBridgeValue.isUndefined()) {
    return;
  }

  auto batchedBridge = batchedBridgeValue.asObject();
  m_callFunctionReturnFlushedQueueJS =
      batchedBridge.getProperty("callFunctionReturnFlushedQueue").asObject();
  m_invokeCallbackAndReturnFlushedQueueJS =
      batchedBridge.getProperty("invokeCallbackAndReturnFlushedQueue")
          .asObject();
  m_flushedQueueJS = batchedBridge.getProperty("flushedQueue").asObject();
  m_callFunctionReturnResultAndFlushedQueueJS =
      batchedBridge.getProperty("callFunctionReturnResultAndFlushedQueue")
          .asObject();
  m_bridgeEstablished = true;
}

void ChakraExecutor::callNativeModules(ChakraValue &&value) {
  SystraceSection s("ChakraExecutor::callNativeModules");
  try {
    auto calls = value.toJSONString();
    m_delegate->callNativeModules(*this, folly::parseJson(calls), true);
  } catch (...) {
    std::string message = "Error in callNativeModules()";
    try {
      message += ":" + value.toString().str();
    } catch (...) {
      // ignored
    }
    std::throw_with_nested(std::runtime_error(message));
  }
}

void ChakraExecutor::flush() {
  SystraceSection s("ChakraExecutor::flush");
  if (m_flushedQueueJS.hasValue()) {
    callNativeModules(m_flushedQueueJS->callAsFunction({}));
  }
}

void ChakraExecutor::callFunction(
    const std::string &moduleId,
    const std::string &methodId,
    const folly::dynamic &arguments) {
  SystraceSection s("ChakraExecutor::callFunction");
  // Set the current context, since we are potentially running on the same
  // thread as other contexts
  JSContextHolder ctx(m_context);

  // This weird pattern is because Value is not default constructible.
  // The lambda is inlined, so there's no overhead.

  auto result = [&] {
    try {
      return m_callFunctionReturnFlushedQueueJS->callAsFunction(
          {ChakraValue(ChakraString::createExpectingAscii(moduleId)),
           ChakraValue(ChakraString::createExpectingAscii(methodId)),
           ChakraValue::fromDynamic(std::move(arguments))});
    } catch (...) {
      std::throw_with_nested(std::runtime_error(
          "Error calling function: " + moduleId + ":" + methodId));
    }
  }();

  callNativeModules(std::move(result));
}

void ChakraExecutor::invokeCallback(
    const double callbackId,
    const folly::dynamic &arguments) {
  SystraceSection s("ChakraExecutor::invokeCallback");
  JSContextHolder ctx(m_context);

  auto result = [&] {
    try {
      return m_invokeCallbackAndReturnFlushedQueueJS->callAsFunction(
          {ChakraValue::makeNumber(callbackId),
           ChakraValue::fromDynamic(std::move(arguments))});
    } catch (...) {
      std::throw_with_nested(std::runtime_error(
          folly::to<std::string>("Error invoking callback.", callbackId)));
    }
  }();

  callNativeModules(std::move(result));
}

void ChakraExecutor::setGlobalVariable(
    std::string propName,
    std::unique_ptr<const JSBigString> jsonValue) {
  try {
    SystraceSection s("ChakraExecutor.setGlobalVariable", "propName", propName);

    JSContextHolder ctx(m_context);

    auto valueToInject =
        ChakraValue::fromJSON(jsStringFromBigString(*jsonValue));
    ChakraObject::getGlobalObject().setProperty(
        propName.c_str(), valueToInject);
  } catch (...) {
    std::throw_with_nested(
        std::runtime_error("Error setting global variable: " + propName));
  }
}

std::string ChakraExecutor::getDescription() {
  return "Chakra";
}

void *ChakraExecutor::getJavaScriptContext() {
  return m_context;
}

#if !defined(OSS_RN)
int64_t ChakraExecutor::getPeakJsMemoryUsage() const noexcept {
  return tls_runtimeTracker.MemoryTracker->GetPeakMemoryUsage();
}
#endif

void ChakraExecutor::flushQueueImmediate(ChakraValue &&queue) {
  auto queueStr = queue.toJSONString();
  m_delegate->callNativeModules(*this, folly::parseJson(queueStr), false);
}

void ChakraExecutor::loadModule(uint32_t bundleId, uint32_t moduleId) {
#ifdef DEBUG
  JsContextRef ctx;
  if (JsNoError != JsGetCurrentContext(&ctx))
    std::terminate();
  // If this isn't the case, then we'll need to set the context here
  if (m_context != ctx)
    std::terminate();
#endif

  auto module = m_bundleRegistry->getModule(bundleId, moduleId);
  auto sourceUrl = ChakraString::createExpectingAscii(module.name);
  auto source = ChakraString(module.code.c_str());
  evaluateScript(source, sourceUrl);
}

// Native JS hooks
template <JsValueRef (ChakraExecutor::*method)(size_t, const JsValueRef[])>
void ChakraExecutor::installNativeHook(const char *name) {
#ifdef DEBUG
  JsContextRef ctx;
  if (JsNoError != JsGetCurrentContext(&ctx))
    std::terminate();
  // If this isn't the case, then we'll need to set the context here
  if (m_context != ctx)
    std::terminate();
#endif

  installGlobalFunction(name, exceptionWrapMethod<method>());
}

JsValueRef ChakraExecutor::getNativeModule(
    JsValueRef object,
    JsValueRef propertyName) {
  std::string string;

  /*auto result = */ JsStringToStdStringUtf8(propertyName, string);

  if (string.compare("name") == 0) {
    return ChakraValue(ChakraString("NativeModules"));
  }

#ifdef DEBUG
  JsContextRef ctx;
  if (JsNoError != JsGetCurrentContext(&ctx))
    std::terminate();
  // If this isn't the case, then we'll need to set the context here
  if (m_context != ctx)
    std::terminate();
#endif

  return m_nativeModules.getModule(propertyName);
}

JsValueRef ChakraExecutor::nativeRequire(
    size_t argumentCount,
    const JsValueRef arguments[]) {
  double moduleId = 0, bundleId = 0;

  if (argumentCount == 1) {
    moduleId = ChakraValue(arguments[0]).asNumber();
  } else if (argumentCount == 2) {
    moduleId = ChakraValue(arguments[0]).asNumber();
    bundleId = ChakraValue(arguments[1]).asNumber();
  } else {
    throw std::invalid_argument("Got wrong number of args");
  }

  if (moduleId < 0) {
    throw std::invalid_argument(folly::to<std::string>(
        "Received invalid module ID: ",
        ChakraValue(arguments[0]).toString().str()));
  }

  if (bundleId < 0) {
    throw std::invalid_argument(folly::to<std::string>(
        "Received invalid bundle ID: ",
        ChakraValue(arguments[1]).toString().str()));
  }

  loadModule(static_cast<uint32_t>(bundleId), static_cast<uint32_t>(moduleId));
  return ChakraValue::makeUndefined();
}

JsValueRef ChakraExecutor::nativeFlushQueueImmediate(
    size_t argumentCount,
    const JsValueRef arguments[]) {
  if (argumentCount != 1) {
    throw std::invalid_argument("Got wrong number of args");
  }

  flushQueueImmediate(ChakraValue(arguments[0]));
  return ChakraValue::makeUndefined();
}

JsValueRef ChakraExecutor::nativeCallSyncHook(
    size_t argumentCount,
    const JsValueRef arguments[]) {
  if (argumentCount != 3) {
    throw std::invalid_argument("Got wrong number of args");
  }

  unsigned int moduleId = ChakraValue(arguments[0]).asUnsignedInteger();
  unsigned int methodId = ChakraValue(arguments[1]).asUnsignedInteger();
  std::string argsJson = ChakraValue(arguments[2]).toJSONString();

  MethodCallResult result = m_delegate->callSerializableNativeHook(
      *this, moduleId, methodId, argsJson);
  if (!result.hasValue()) {
    return ChakraValue::makeUndefined();
  }

#ifdef DEBUG
  JsContextRef ctx;
  if (JsNoError != JsGetCurrentContext(&ctx))
    std::terminate();
  // If this isn't the case, then we'll need to setthe context here
  if (m_context != ctx)
    std::terminate();
#endif

  return ChakraValue::fromDynamic(result.value());
}

JsValueRef ChakraExecutor::nativeLoggingHook(
    size_t argumentCount,
    const JsValueRef arguments[]) {
  // arguments[0]: text message
  // arguments[1]: log level, optional

  assert(argumentCount >= 1);

  std::string message = ChakraValue(arguments[0]).toString().str();

  RCTLogLevel logLevel = RCTLogLevel::Trace;
  if (argumentCount > 1) {
    unsigned int logLevelAsInt = ChakraValue(arguments[1]).asUnsignedInteger();
    logLevel = static_cast<RCTLogLevel>(logLevelAsInt);
  }

  if (m_instanceArgs.LoggingCallback) {
    m_instanceArgs.LoggingCallback(logLevel, message.c_str());
  } else {
    // If there is no executor-specific logging callback, redirect to the global
    // logging hook
    if (JSNativeHooks::loggingHook != nullptr) {
      JSNativeHooks::loggingHook(logLevel, message.c_str());
    }
  }

  return ChakraValue::makeUndefined();
}

} // namespace react
} // namespace facebook
