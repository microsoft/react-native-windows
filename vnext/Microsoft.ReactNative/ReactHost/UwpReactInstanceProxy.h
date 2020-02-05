// Copyright (c) Microsoft Corporation. All rights reserved.
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
  UwpReactInstanceProxy(
      Mso::WeakPtr<Mso::React::IReactInstance> &&weakReactInstance,
      ReactInstanceSettings &&instanceSettings) noexcept;

 public: // IReactInstance
  void Start(const std::shared_ptr<IReactInstance> &spThis, const ReactInstanceSettings &settings) override;

  void AttachMeasuredRootView(IXamlRootView *pRootView, folly::dynamic &&initProps) override;
  void DetachRootView(IXamlRootView *pRootView) override;
  LiveReloadCallbackCookie RegisterLiveReloadCallback(std::function<void()> callback) override;
  void UnregisterLiveReloadCallback(LiveReloadCallbackCookie &cookie) override;
  ErrorCallbackCookie RegisterErrorCallback(std::function<void()> callback) override;
  void UnregisterErrorCallback(ErrorCallbackCookie &cookie) override;
  DebuggerAttachCallbackCookie RegisterDebuggerAttachCallback(std::function<void()> callback) override;
  void UnRegisterDebuggerAttachCallback(DebuggerAttachCallbackCookie &cookie) override;
  void DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic &&eventData) override;
  void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept override;
  const std::shared_ptr<facebook::react::MessageQueueThread> &JSMessageQueueThread() const noexcept override;
  const std::shared_ptr<facebook::react::MessageQueueThread> &DefaultNativeMessageQueueThread() const noexcept override;
  facebook::react::INativeUIManager *NativeUIManager() const noexcept override;
  bool NeedsReload() const noexcept override;
  void SetAsNeedsReload() noexcept override;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;
  bool IsInError() const noexcept override;
  bool IsWaitingForDebugger() const noexcept override;
  const std::string &LastErrorMessage() const noexcept override;
  void loadBundle(std::string &&jsBundleRelativePath) override;
  ExpressionAnimationStore &GetExpressionAnimationStore() override;
  const ReactInstanceSettings &GetReactInstanceSettings() const override;
  std::string GetBundleRootPath() const noexcept override;

  // Test hooks
  void SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> testHook) override;
  void CallXamlViewCreatedTestHook(react::uwp::XamlView view) override;

 private:
  Mso::WeakPtr<Mso::React::IReactInstance> m_weakReactInstance;
  ReactInstanceSettings m_instanceSettings;
  ExpressionAnimationStore m_expressionAnimationStore;
  std::function<void(XamlView)> m_xamlViewCreatedTestHook;
};

} // namespace react::uwp
