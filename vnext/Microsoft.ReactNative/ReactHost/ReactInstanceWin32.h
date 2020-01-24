// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "IReactInstanceInternal.h"
#include "ReactNativeHeaders.h"
#include "React_win.h"
#include "activeObject/activeObject.h"

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

//! ReactInstance implementation for Win32 that is managed by ReactHost.
class ReactInstanceWin32 final
    : public Mso::ActiveObject<IReactInstanceInternal, IReactInstanceWin32, IFBReactInstance> {
  using Super = ActiveObjectType;

 public: // IReactInstance
  const std::string &JsBundleName() const noexcept override;
  std::string SDXBasePath() const noexcept override;
  bool NeedsReload() const noexcept override {
    VerifyElseCrashSzTag(false, "Not implemented", 0x0285e28e /* tag_c74ko */);
  }
  void SetAsNeedsReload() noexcept override {
    VerifyElseCrashSzTag(false, "Not implemented", 0x0285e28f /* tag_c74kp */);
  }
  bool GetQuirk(SDXQuirk /*quirk*/) const noexcept override {
    VerifyElseCrashSzTag(false, "Not implemented", 0x0285e290 /* tag_c74kq */);
  }
  const ReactOptions &Options() const noexcept override;

 public: // IReactInstanceInternal
  Mso::Future<void> Destroy() noexcept override;

 public: // IFBReactInstance
  void CallJsFunction(std::string &&moduleName, std::string &&method, folly::dynamic &&params) noexcept override;
  std::shared_ptr<facebook::react::MessageQueueThread> DefaultNativeMessageQueueThread() const noexcept override;
  std::shared_ptr<facebook::react::MessageQueueThread> JSMessageQueueThread() const noexcept override;

 public: // IReactInstanceWin32 members
  std::shared_ptr<facebook::react::InstanceWrapper> InstanceObject() const noexcept override;
  std::shared_ptr<facebook::react::IUIManager> UIManager() const noexcept override;

 private:
  friend MakePolicy;
  ReactInstanceWin32(IReactHost &reactHost, ReactOptions &&options, Mso::Promise<void> &&whenLoaded) noexcept;
  void Initialize() noexcept override;
  ~ReactInstanceWin32() override;

 private:
  void InitJSMessageThread() noexcept;
  void InitNativeMessageThread() noexcept;
  void InitUIManager() noexcept;
  CxxModuleProviders GetCxxModuleProviders() noexcept;
  std::string GetBytecodeFileName() noexcept;
  std::string GetDebugHost() noexcept;
  std::function<void()> GetLiveReloadCallback() noexcept;
  std::function<void(std::string)> GetErrorCallback() noexcept;
  facebook::react::NativeLoggingHook GetLoggingCallback() noexcept;
  std::function<void(facebook::react::JSExceptionInfo &&)> GetJSExceptionCallback() noexcept;

  void OnError(const Mso::ErrorCode &errorcode) noexcept;
  void OnErrorWithMessage(const std::string &errorMessage) noexcept;
  void OnLiveReload() noexcept;

  void LoadJSBundles() noexcept;
  friend struct LoadedCallbackGuard;
  void OnReactInstanceLoaded(const Mso::ErrorCode &errorCode) noexcept;

 private: // immutable fields
  const Mso::WeakPtr<IReactHost> m_weakReactHost;
  const ReactOptions m_options;
  const Mso::Promise<void> m_whenLoaded;
  const Mso::Promise<void> m_whenDestroyed;

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

  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::IUIManager>> m_uiManager{Queue(), m_mutex};

  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::InstanceWrapper>> m_instanceWrapper{Queue(), m_mutex};
  const Mso::ActiveReadableField<std::shared_ptr<facebook::react::Instance>> m_instance{Queue(), m_mutex};
};

} // namespace Mso::React
