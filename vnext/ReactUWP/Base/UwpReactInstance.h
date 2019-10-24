// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <IReactInstance.h>

#include <Modules/DeviceInfoModule.h>
#include <Threading/WorkerMessageQueueThread.h>
#include <Views/ExpressionAnimationStore.h>

#include <winrt/Windows.UI.Core.h>

#include <map>
#include <memory>
#include <string>

namespace facebook {
namespace react {
struct NativeModuleProvider;
}
} // namespace facebook
namespace react {
namespace uwp {
struct ViewManagerProvider;
}
} // namespace react

namespace react {
namespace uwp {

class UwpReactInstance : public IReactInstance, public ::std::enable_shared_from_this<UwpReactInstance> {
 public:
  // Creation
  UwpReactInstance(
      const std::shared_ptr<facebook::react::NativeModuleProvider> &moduleProvider,
      const std::shared_ptr<ViewManagerProvider> &viewManagerProvider = nullptr);

  void Start(const std::shared_ptr<IReactInstance> &spThis, const ReactInstanceSettings &settings) override;

  // IReactInstance implementation
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
  bool NeedsReload() const noexcept override {
    return m_needsReload;
  }
  void SetAsNeedsReload() noexcept override {
    m_needsReload = true;
  }
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override {
    return m_instanceWrapper->GetInstance();
  }
  bool IsInError() const noexcept override {
    return m_isInError;
  }
  bool IsWaitingForDebugger() const noexcept override {
    return m_isWaitingForDebugger;
  }
  const std::string &LastErrorMessage() const noexcept override {
    return m_errorMessage;
  }
  void loadBundle(std::string &&jsBundleRelativePath) override {
    if (!m_isInError)
      m_instanceWrapper->loadBundle(std::move(jsBundleRelativePath));
  };
  ExpressionAnimationStore &GetExpressionAnimationStore() override {
    return m_expressionAnimationStore;
  }
  const ReactInstanceSettings &GetReactInstanceSettings() const override {
    return m_reactInstanceSettings;
  }
  std::string GetBundleRootPath() const noexcept override {
    return m_bundleRootPath;
  }

  // Test hooks
  void SetXamlViewCreatedTestHook(std::function<void(react::uwp::XamlView)> testHook) override;
  void CallXamlViewCreatedTestHook(react::uwp::XamlView view) override;

  // Public functions
  std::shared_ptr<facebook::react::MessageQueueThread> GetNewUIMessageQueue() const;

 private:
  void OnHitError(const std::string &error) noexcept;
  void OnWaitingForDebugger() noexcept;
  void OnDebuggerAttach() noexcept;

 private:
#if defined(USE_V8)
  static std::string getApplicationLocalFolder();
#endif
  std::shared_ptr<WorkerMessageQueueThread> m_initThread;
  std::shared_ptr<facebook::react::MessageQueueThread> m_jsThread;
  std::shared_ptr<facebook::react::MessageQueueThread> m_defaultNativeThread;
  std::shared_ptr<facebook::react::MessageQueueThread> m_batchingNativeThread;
  std::shared_ptr<facebook::react::IUIManager> m_uiManager;
  std::shared_ptr<facebook::react::InstanceWrapper> m_instanceWrapper;
  winrt::Windows::UI::Core::CoreDispatcher m_uiDispatcher{nullptr};
  std::shared_ptr<facebook::react::NativeModuleProvider> m_moduleProvider;
  std::shared_ptr<ViewManagerProvider> m_viewManagerProvider;
  std::map<LiveReloadCallbackCookie, std::function<void()>> m_liveReloadCallbacks;
  std::map<ErrorCallbackCookie, std::function<void()>> m_errorCallbacks;
  std::map<DebuggerAttachCallbackCookie, std::function<void()>> m_debuggerAttachCallbacks;
  bool m_needsReload{false};
  bool m_started{false};
  std::atomic_bool m_isInError{false};
  std::atomic_bool m_isWaitingForDebugger{false};
  std::string m_errorMessage;
  ExpressionAnimationStore m_expressionAnimationStore;

  std::function<void(XamlView)> m_xamlViewCreatedTestHook;

  std::string m_bundleRootPath;
  ReactInstanceSettings m_reactInstanceSettings;
  std::shared_ptr<DeviceInfo> m_deviceInfo;
};

} // namespace uwp
} // namespace react
