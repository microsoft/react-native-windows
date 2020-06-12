// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "IReactInstanceInternal.h"
#include "ReactNativeHeaders.h"
#include "React_win.h"
#include "activeObject/activeObject.h"

#include <Modules/AppStateModuleUwp.h>
#include <Modules/AppThemeModuleUwp.h>
#include <Modules/AppearanceModule.h>
#include <Modules/DeviceInfoModule.h>
#include <ReactUWP/Modules/I18nModule.h>
#include "UwpReactInstanceProxy.h"

#include <tuple>

namespace Mso::React {

static_assert(
    static_cast<int32_t>(facebook::react::RCTLogLevel::Trace) == static_cast<int32_t>(LogLevel::Trace),
    "LogLevel::Trace value must match");
static_assert(
    static_cast<int32_t>(facebook::react::RCTLogLevel::Info) == static_cast<int32_t>(LogLevel::Info),
    "LogLevel::Info value must match");
static_assert(
    static_cast<int32_t>(facebook::react::RCTLogLevel::Warning) == static_cast<int32_t>(LogLevel::Warning),
    "LogLevel::Warning value must match");
static_assert(
    static_cast<int32_t>(facebook::react::RCTLogLevel::Error) == static_cast<int32_t>(LogLevel::Error),
    "LogLevel::Error value must match");
static_assert(
    static_cast<int32_t>(facebook::react::RCTLogLevel::Fatal) == static_cast<int32_t>(LogLevel::Fatal),
    "LogLevel::Fatal value must match");

class ReactInstanceWin;

class ReactContext final : public Mso::UnknownObject<IReactContext> {
 public:
  ReactContext(
      Mso::WeakPtr<ReactInstanceWin> &&reactInstance,
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties) noexcept;

 public: // IReactContext
  winrt::Microsoft::ReactNative::IReactPropertyBag Properties() noexcept override;
  void CallJSFunction(std::string &&module, std::string &&method, folly::dynamic &&params) noexcept override;
  void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept override;

 private:
  Mso::WeakPtr<ReactInstanceWin> m_reactInstance;
  winrt::Microsoft::ReactNative::IReactPropertyBag m_properties;
};

//! ReactInstance implementation for Windows that is managed by ReactHost.
class ReactInstanceWin final : public Mso::ActiveObject<IReactInstanceInternal, ILegacyReactInstance> {
  using Super = ActiveObjectType;

 public: // IReactInstance
  const ReactOptions &Options() const noexcept override;
  ReactInstanceState State() const noexcept override;

 public: // IReactInstanceInternal
  Mso::Future<void> Destroy() noexcept override;

 public: // ILegacyReactInstance
  void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept override;
  void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept override;
  facebook::react::INativeUIManager *NativeUIManager() noexcept override;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() noexcept override;
  std::string GetBundleRootPath() noexcept override;
  std::shared_ptr<react::uwp::IReactInstance> UwpReactInstance() noexcept override;
  void AttachMeasuredRootView(
      facebook::react::IReactRootView *rootView,
      folly::dynamic &&initialProps) noexcept override;
  void DetachRootView(facebook::react::IReactRootView *rootView) noexcept override;

 private:
  friend MakePolicy;
  // UwpReactInstance(
  //    const std::shared_ptr<facebook::react::NativeModuleProvider> &moduleProvider,
  //    const std::shared_ptr<ViewManagerProvider> &viewManagerProvider = nullptr);
  ReactInstanceWin(
      IReactHost &reactHost,
      ReactOptions const &options,
      Mso::Promise<void> &&whenCreated,
      Mso::Promise<void> &&whenLoaded,
      Mso::VoidFunctor &&updateUI) noexcept;
  void Initialize() noexcept override;
  ~ReactInstanceWin() override;

 private:
  void LoadJSBundles() noexcept;
  void InitJSMessageThread() noexcept;
  void InitNativeMessageThread() noexcept;
  void InitUIMessageThread() noexcept;
  void InitUIManager() noexcept;
  std::string GetBytecodeFileName() noexcept;
  std::string GetDebugHost() noexcept;
  std::string GetSourceBundleHost() noexcept;
  std::string GetSourceBundlePort() noexcept;
  std::function<void()> GetLiveReloadCallback() noexcept;
  std::function<void(std::string)> GetErrorCallback() noexcept;
  facebook::react::NativeLoggingHook GetLoggingCallback() noexcept;
  std::shared_ptr<Mso::React::IRedBoxHandler> GetRedBoxHandler() noexcept;
  std::function<void()> GetWaitingForDebuggerCallback() noexcept;
  std::function<void()> GetDebuggerAttachCallback() noexcept;

  void OnError(const Mso::ErrorCode &errorcode) noexcept;
  void OnErrorWithMessage(const std::string &errorMessage) noexcept;
  void OnLiveReload() noexcept;
  void OnWaitingForDebugger() noexcept;
  void OnDebuggerAttach() noexcept;

  friend struct LoadedCallbackGuard;
  void OnReactInstanceLoaded(const Mso::ErrorCode &errorCode) noexcept;

  void DrainJSCallQueue() noexcept;
  void AbandonJSCallQueue() noexcept;

  struct JSCallEntry {
    std::string ModuleName;
    std::string MethodName;
    folly::dynamic Args;
  };

#if defined(USE_V8)
  static std::string getApplicationLocalFolder();
#endif

 private: // immutable fields
  const Mso::WeakPtr<IReactHost> m_weakReactHost;
  const ReactOptions m_options;
  const Mso::Promise<void> m_whenCreated;
  const Mso::Promise<void> m_whenLoaded;
  const Mso::Promise<void> m_whenDestroyed;
  const std::shared_ptr<react::uwp::UwpReactInstanceProxy> m_legacyInstance;
  const Mso::VoidFunctor m_updateUI;

  const Mso::CntPtr<ReactContext> m_reactContext;

  std::atomic<bool> m_isLoaded{false};
  std::atomic<bool> m_isDestroyed{false};
  std::atomic<bool> m_isRekaInitialized{false};

 private: // fields controlled by mutex
  mutable std::mutex m_mutex;

  const Mso::ActiveReadableField<Mso::DispatchQueue> m_jsDispatchQueue{Queue(), m_mutex};

  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::MessageQueueThread>> m_jsMessageThread{Queue(),
                                                                                                         m_mutex};
  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::MessageQueueThread>> m_nativeMessageThread{Queue(),
                                                                                                             m_mutex};
  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::MessageQueueThread>> m_uiMessageThread{Queue(),
                                                                                                         m_mutex};
  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::IUIManager>> m_uiManager{Queue(), m_mutex};

  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::InstanceWrapper>> m_instanceWrapper{Queue(), m_mutex};
  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::Instance>> m_instance{Queue(), m_mutex};
  std::atomic<ReactInstanceState> m_state{ReactInstanceState::Loading};

  std::shared_ptr<facebook::react::MessageQueueThread> m_batchingUIThread;

  std::shared_ptr<react::uwp::IReactInstance> m_legacyReactInstance;
  std::shared_ptr<react::uwp::DeviceInfo> m_deviceInfo;
  std::shared_ptr<facebook::react::AppState> m_appState;
  std::shared_ptr<IRedBoxHandler> m_redboxHandler;
  std::shared_ptr<react::uwp::AppTheme> m_appTheme;
  Mso::CntPtr<react::uwp::AppearanceChangeListener> m_appearanceListener;
  std::string m_bundleRootPath;
  Mso::DispatchQueue m_uiQueue;
  std::deque<JSCallEntry> m_jsCallQueue;
};

} // namespace Mso::React
