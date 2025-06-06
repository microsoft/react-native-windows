/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <memory>
#include <string>

#include <cxxreact/NativeModule.h>
#include <folly/dynamic.h>
#include <jsinspector-modern/InspectorInterfaces.h>
#include <jsinspector-modern/FallbackRuntimeAgentDelegate.h>
#include <jsinspector-modern/ReactCdp.h>
#include <jsinspector-modern/ConsoleMessage.h>
#include <jsinspector-modern/StackTrace.h>

#ifndef RN_EXPORT
#define RN_EXPORT __attribute__((visibility("default")))
#endif

namespace facebook::react {

class JSBigString;
class JSExecutor;
class JSModulesUnbundle;
class MessageQueueThread;
class ModuleRegistry;
class RAMBundleRegistry;

// This interface describes the delegate interface required by
// Executor implementations to call from JS into native code.
class ExecutorDelegate {
 public:
  virtual ~ExecutorDelegate() {}

  virtual std::shared_ptr<ModuleRegistry> getModuleRegistry() = 0;

  virtual void callNativeModules(
      JSExecutor& executor,
      folly::dynamic&& calls,
      bool isEndOfBatch) = 0;
  virtual MethodCallResult callSerializableNativeHook(
      JSExecutor& executor,
      unsigned int moduleId,
      unsigned int methodId,
      folly::dynamic&& args) = 0;
};

class JSExecutorFactory {
 public:
  virtual std::unique_ptr<JSExecutor> createJSExecutor(
      std::shared_ptr<ExecutorDelegate> delegate,
      std::shared_ptr<MessageQueueThread> jsQueue) = 0;
  virtual ~JSExecutorFactory() {}
};

class RN_EXPORT JSExecutor : public jsinspector_modern::RuntimeTargetDelegate {
 public:
  /**
   * Prepares the JS runtime for React Native by installing global variables.
   * Called once before any JS is evaluated.
   */
  virtual void initializeRuntime() = 0;
  /**
   * Execute an application script bundle in the JS context.
   */
  virtual void loadBundle(
      std::unique_ptr<const JSBigString> script,
      std::string sourceURL) = 0;

  /**
   * Add an application "RAM" bundle registry
   */
  virtual void setBundleRegistry(
      std::unique_ptr<RAMBundleRegistry> bundleRegistry) = 0;

  /**
   * Register a file path for an additional "RAM" bundle
   */
  virtual void registerBundle(
      uint32_t bundleId,
      const std::string& bundlePath) = 0;

  /**
   * Executes BatchedBridge.callFunctionReturnFlushedQueue with the module ID,
   * method ID and optional additional arguments in JS. The executor is
   * responsible for using Bridge->callNativeModules to invoke any necessary
   * native modules methods.
   */
  virtual void callFunction(
      const std::string& moduleId,
      const std::string& methodId,
      const folly::dynamic& arguments) = 0;

  /**
   * Executes BatchedBridge.invokeCallbackAndReturnFlushedQueue with the cbID,
   * and optional additional arguments in JS and returns the next queue. The
   * executor is responsible for using Bridge->callNativeModules to invoke any
   * necessary native modules methods.
   */
  virtual void invokeCallback(
      const double callbackId,
      const folly::dynamic& arguments) = 0;

  virtual void setGlobalVariable(
      std::string propName,
      std::unique_ptr<const JSBigString> jsonValue) = 0;

  virtual void* getJavaScriptContext() {
    return nullptr;
  }

  /**
   * Returns whether or not the underlying executor supports debugging via the
   * Chrome remote debugging protocol. If true, the executor should also
   * override the \c createAgentDelegate method.
   */
  virtual bool isInspectable() {
    return false;
  }

  /**
   * The description is displayed in the dev menu, if there is one in
   * this build.  There is a default, but if this method returns a
   * non-empty string, it will be used instead.
   */
  virtual std::string getDescription() = 0;

  virtual void handleMemoryPressure([[maybe_unused]] int pressureLevel) {}

  virtual void destroy() {}
  virtual ~JSExecutor() override {}

  virtual void flush() {}

  static std::string getSyntheticBundlePath(
      uint32_t bundleId,
      const std::string& bundlePath);

  static double performanceNow();

  virtual void addConsoleMessage(jsi::Runtime& runtime, jsinspector_modern::ConsoleMessage message);

  virtual bool supportsConsole() const;

  /**
   * \returns an opaque representation of a stack trace. This may be passed back
   * to the `RuntimeTargetDelegate` as part of `addConsoleMessage` or other APIs
   * that report stack traces.
   * \param framesToSkip The number of call frames to skip. The first call frame
   * is the topmost (current) frame on the Runtime's call stack, which will
   * typically be the (native) JSI HostFunction that called this method.
   * \note The method is called on the JS thread, and receives a valid reference
   * to the current \c jsi::Runtime. The callee MAY use its own intrinsic
   * Runtime reference, if it has one, without checking it for equivalence with
   * the one provided here.
   */
  std::unique_ptr<jsinspector_modern::StackTrace> captureStackTrace(
      jsi::Runtime& runtime,
      size_t framesToSkip = 0) override;


  /**
   * Start sampling profiler.
   */
  virtual void enableSamplingProfiler() override;

  /**
   * Stop sampling profiler.
   */
  virtual void disableSamplingProfiler() override;

  /**
   * Return recorded sampling profile for the previous sampling session.
   */
  virtual facebook::react::jsinspector_modern::tracing::RuntimeSamplingProfile collectSamplingProfile() override;

  /**
   * Create a RuntimeAgentDelegate that can be used to debug the JS VM instance.
   */
  virtual std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate>
  createAgentDelegate(
      jsinspector_modern::FrontendChannel frontendChannel,
      jsinspector_modern::SessionState& sessionState,
      std::unique_ptr<jsinspector_modern::RuntimeAgentDelegate::ExportedState>
          previouslyExportedState,
      const jsinspector_modern::ExecutionContextDescription&
          executionContextDescription,
      RuntimeExecutor runtimeExecutor) override;
};

} // namespace facebook::react