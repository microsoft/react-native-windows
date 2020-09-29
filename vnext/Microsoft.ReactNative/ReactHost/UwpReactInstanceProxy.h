// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>

#include <Modules/DeviceInfoModule.h>
#include <ReactHost/React.h>
#include <Views/ExpressionAnimationStore.h>

#include <memory>
#include <string>

namespace facebook::react {
struct NativeModuleProvider;
} // namespace facebook::react

namespace react::uwp {

struct ViewManagerProvider;

struct UwpReactInstanceProxy : IReactInstance, std::enable_shared_from_this<UwpReactInstanceProxy> {
  UwpReactInstanceProxy(Mso::CntPtr<Mso::React::IReactContext> &&context) noexcept;

 public: // IReactInstance
  void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept override;
  void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept override;
  facebook::react::INativeUIManager *NativeUIManager() const noexcept override;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;
  bool IsInError() const noexcept override;
  bool IsWaitingForDebugger() const noexcept override;
  ExpressionAnimationStore &GetExpressionAnimationStore() noexcept override;
  std::string GetBundleRootPath() const noexcept override;
  bool IsLoaded() const noexcept override;

  // Test hooks
  void SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> &&testHook) noexcept override;
  void CallXamlViewCreatedTestHook(react::uwp::XamlView const &view) noexcept override;

 private:
  Mso::CntPtr<Mso::React::IReactContext> m_context;
  ExpressionAnimationStore m_expressionAnimationStore;
  std::function<void(XamlView)> m_xamlViewCreatedTestHook;
};

} // namespace react::uwp
