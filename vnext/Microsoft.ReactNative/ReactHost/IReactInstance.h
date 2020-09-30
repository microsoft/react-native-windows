// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <InstanceManager.h>

#include <DevSettings.h>
#include <Folly/dynamic.h>
#include "XamlView.h"

#include <functional>
#include <string>

namespace facebook {
namespace react {
struct INativeUIManager;
}
} // namespace facebook

namespace react::uwp {

struct IXamlRootView;
class ExpressionAnimationStore;

typedef unsigned int LiveReloadCallbackCookie;
typedef unsigned int ErrorCallbackCookie;
typedef unsigned int DebuggerAttachCallbackCookie;

enum class JSIEngine : int32_t {
  Chakra = 0, // Use the JSIExecutorFactory with ChakraRuntime
  Hermes = 1, // Use the JSIExecutorFactory with Hermes
  V8 = 2, // Use the JSIExecutorFactory with V8
};

struct IReactInstance {
  virtual ~IReactInstance() = default;

  virtual void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept = 0;
  virtual void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept = 0;

  virtual facebook::react::INativeUIManager *NativeUIManager() const noexcept = 0;

  virtual std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept = 0;

  virtual bool IsInError() const noexcept = 0;
  virtual bool IsWaitingForDebugger() const noexcept = 0;

  // Returns the root path of the JS bundle. This is needed for
  // classes that do not have access to the settings object.
  virtual std::string GetBundleRootPath() const noexcept = 0;

  // Test Hooks
  virtual void SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> &&testHook) noexcept = 0;
  virtual void CallXamlViewCreatedTestHook(react::uwp::XamlView const &view) noexcept = 0;

  virtual ExpressionAnimationStore &GetExpressionAnimationStore() noexcept = 0;

  virtual bool IsLoaded() const noexcept = 0;
};

} // namespace react::uwp
