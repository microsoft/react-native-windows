// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <atomic>
#include <functional>
#include <map>
#include <vector>

#if !defined(NOJSC)
#include <cxxreact/JSCExecutor.h>
#endif

#include <cxxreact/JSExecutor.h>

namespace folly {
struct dynamic;
}

namespace facebook {
namespace react {

#if !defined(OSS_RN)

struct InstanceCallback;
class JSModulesUnbundle;
class JsToNativeBridge;
class MessageQueueThread;
class ModuleRegistry;

class SandboxHostNativeBridge : public react::ExecutorDelegate
{
public:
  SandboxHostNativeBridge(std::shared_ptr<ModuleRegistry> registry,
    std::shared_ptr<InstanceCallback> callback);

  // ExecutorDelegate methods
  std::shared_ptr<ModuleRegistry> getModuleRegistry() override;
  void callNativeModules(
    JSExecutor& executor, folly::dynamic&& calls, bool isEndOfBatch) override;
  MethodCallResult callSerializableNativeHook(
    JSExecutor& executor, unsigned int moduleId, unsigned int methodId,
    folly::dynamic&& args) override;

private:

  // These methods are always invoked from an Executor.  The NativeToJsBridge
  // keeps a reference to the executor, and when destroy() is called, the
  // executor is destroyed synchronously on its queue.
  std::shared_ptr<ModuleRegistry> m_registry;
  std::shared_ptr<InstanceCallback> m_callback;
  bool m_batchHadNativeModuleCalls = false;
};


// This class manages calls from JS to native code for sandbox.
class SandboxJsToNativeBridge : public react::ExecutorDelegate
{
public:
  SandboxJsToNativeBridge(std::shared_ptr<ModuleRegistry> registry,
      std::shared_ptr<InstanceCallback> callback,
      std::function<void(std::string&& message)> remoteCallFunc);

  // ExecutorDelegate methods
  std::shared_ptr<ModuleRegistry> getModuleRegistry() override;
  void callNativeModules(
      JSExecutor& executor, folly::dynamic&& calls, bool isEndOfBatch) override;
  MethodCallResult callSerializableNativeHook(
      JSExecutor& executor, unsigned int moduleId, unsigned int methodId,
      folly::dynamic&& args) override;

private:

  // These methods are always invoked from an Executor.  The NativeToJsBridge
  // keeps a reference to the executor, and when destroy() is called, the
  // executor is destroyed synchronously on its queue.
  std::shared_ptr<ModuleRegistry> m_registry;
  std::shared_ptr<InstanceCallback> m_callback;
  std::function<void(std::string&& message)> m_remoteCallFunc;
  bool m_batchHadNativeModuleCalls = false;
};

#endif // OSS_RN

} // namespace react
} // namespace facebook
