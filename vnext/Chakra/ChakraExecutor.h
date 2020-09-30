// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <cstdint>
#include <memory>
#include <unordered_map>

#include <folly/Optional.h>
#include <folly/json.h>

#include <cxxreact\JSExecutor.h>

#include "ChakraHelpers.h"
#include "ChakraInstanceArgs.h"
#include "ChakraNativeModules.h"
#include "ChakraValue.h"

#if !defined(USE_EDGEMODE_JSRT)
#include "ChakraCoreDebugger.h"
#endif

namespace facebook {
namespace react {

class MessageQueueThread;
class RAMBundleRegistry;

class ChakraExecutorFactory : public JSExecutorFactory {
 public:
  ChakraExecutorFactory(ChakraInstanceArgs &&instanceArgs) : m_instanceArgs(std::move(instanceArgs)) {}
  virtual std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) override;

 private:
  ChakraInstanceArgs m_instanceArgs;
#if _DEBUG
  uint8_t m_executorCreationCount = 0;
#endif
};

class ChakraExecutor;

class WorkerRegistration {
 public:
  WorkerRegistration(const WorkerRegistration &) = delete;
  WorkerRegistration &operator=(const WorkerRegistration &) = delete;
  WorkerRegistration() = default;

  explicit WorkerRegistration(ChakraExecutor *executor_, ChakraObject jsObj_)
      : executor(executor_), jsObj(std::move(jsObj_)) {}

  ChakraExecutor *executor;
  ChakraObject jsObj;
};

class ChakraExecutor final : public JSExecutor {
 public:
  /**
   * Must be invoked from thread this Executor will run on.
   */
  explicit ChakraExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> messageQueueThread,
      ChakraInstanceArgs &&instanceArgs);
  ~ChakraExecutor() override;

  virtual void initializeRuntime() override;

  virtual void loadBundle(std::unique_ptr<const JSBigString> script, std::string sourceURL) override;

  virtual void setBundleRegistry(std::unique_ptr<RAMBundleRegistry> bundleRegistry) override;

  virtual void registerBundle(uint32_t bundleId, const std::string &bundlePath) override;

  virtual void callFunction(const std::string &moduleId, const std::string &methodId, const folly::dynamic &arguments)
      override;

  virtual void invokeCallback(const double callbackId, const folly::dynamic &arguments) override;

  virtual void setGlobalVariable(std::string propName, std::unique_ptr<const JSBigString> jsonValue) override;

  virtual std::string getDescription() override;

  virtual void *getJavaScriptContext() override;

  virtual void destroy() override;

  void setContextName(const std::string &name);

 private:
  JsContextRef m_context;
  std::shared_ptr<ExecutorDelegate> m_delegate;
  int m_workerId = 0; // if this is a worker executor, this is non-zero
  ChakraExecutor *m_owner = nullptr; // if this is a worker executor, this is non-null
  std::shared_ptr<bool> m_isDestroyed = std::shared_ptr<bool>(new bool(false));
  std::unordered_map<int, WorkerRegistration> m_ownedWorkers;
  std::shared_ptr<MessageQueueThread> m_messageQueueThread;
  std::unique_ptr<RAMBundleRegistry> m_bundleRegistry;
  ChakraNativeModules m_nativeModules;
  bool m_bridgeEstablished = false;
  ChakraInstanceArgs m_instanceArgs;

  folly::Optional<ChakraObject> m_invokeCallbackAndReturnFlushedQueueJS;
  folly::Optional<ChakraObject> m_callFunctionReturnFlushedQueueJS;
  folly::Optional<ChakraObject> m_flushedQueueJS;

  /**
   * WebWorker constructor. Must be invoked from thread this Executor will run
   * on.
   */
  ChakraExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> messageQueueThread,
      int workerId,
      ChakraExecutor *owner,
      std::string scriptURL,
      std::unordered_map<std::string, std::string> globalObjAsJSON,
      const folly::dynamic &jscConfig);

  void initOnJSVMThread();
  bool isNetworkInspected(const std::string &owner, const std::string &app, const std::string &device);
  // This method is experimental, and may be modified or removed.
  ChakraValue callFunctionSyncWithValue(const std::string &module, const std::string &method, ChakraValue value);
  void terminateOnJSVMThread();
  void bindBridge() noexcept;
  void callNativeModules(ChakraValue &&);
#if !defined(USE_EDGEMODE_JSRT)
  JsErrorCode enableDebugging(
      JsRuntimeHandle runtime,
      std::string const &runtimeName,
      bool breakOnNextLine,
      uint16_t port,
      std::unique_ptr<DebugProtocolHandler> &debugProtocolHandler,
      std::unique_ptr<DebugService> &debugService);
  static void CHAKRA_CALLBACK ProcessDebuggerCommandQueueCallback(void *callbackState);
  void ProcessDebuggerCommandQueue();
  bool needToRedirectConsoleToDebugger = false;
  JsErrorCode RedirectConsoleToDebugger(JsValueRef debuggerConsoleObject);
#endif
  void flush();
  void flushQueueImmediate(ChakraValue &&);
  void loadModule(uint32_t bundleId, uint32_t moduleId);

  template <JsValueRef (ChakraExecutor::*method)(size_t, const JsValueRef[])>
  void installNativeHook(const char *name);
  JsValueRef getNativeModule(JsValueRef object, JsValueRef propertyName);

  JsValueRef nativePostMessageToWorker(size_t argumentCount, const JsValueRef arguments[]);
  JsValueRef nativeTerminateWorker(size_t argumentCount, const JsValueRef arguments[]);
  JsValueRef nativeRequire(size_t argumentCount, const JsValueRef arguments[]);
  JsValueRef nativeFlushQueueImmediate(size_t argumentCount, const JsValueRef arguments[]);
  JsValueRef nativeCallSyncHook(size_t argumentCount, const JsValueRef arguments[]);
  JsValueRef nativeLoggingHook(size_t argumentCount, const JsValueRef arguments[]);
};

template <JsValueRef (ChakraExecutor::*method)(size_t, const JsValueRef[])>
static JsNativeFunction exceptionWrapMethod();

} // namespace react
} // namespace facebook
