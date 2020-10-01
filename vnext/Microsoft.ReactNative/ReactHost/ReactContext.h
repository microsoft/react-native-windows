// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "React.h"

namespace Mso::React {

class ReactInstanceWin;

class ReactContext final : public Mso::UnknownObject<IReactContext> {
 public:
  ReactContext(
      Mso::WeakPtr<ReactInstanceWin> &&reactInstance,
      winrt::Microsoft::ReactNative::IReactPropertyBag const &properties,
      winrt::Microsoft::ReactNative::IReactNotificationService const &notifications) noexcept;

  // ReactContext may have longer lifespan than ReactInstance.
  // The ReactInstance uses the Destroy method to enforce the ReactContext cleanup
  // when the ReactInstance is destroyed.
  void Destroy() noexcept;

 public: // IReactContext
  winrt::Microsoft::ReactNative::IReactPropertyBag Properties() noexcept override;
  winrt::Microsoft::ReactNative::IReactNotificationService Notifications() noexcept override;
  void CallJSFunction(std::string &&module, std::string &&method, folly::dynamic &&params) noexcept override;
  void DispatchEvent(int64_t viewTag, std::string &&eventName, folly::dynamic &&eventData) noexcept override;
#ifndef CORE_ABI
  ReactInstanceState State() const noexcept override;
  bool IsLoaded() const noexcept override;
  facebook::react::INativeUIManager *NativeUIManager() const noexcept override;
  std::shared_ptr<facebook::react::Instance> GetInnerInstance() const noexcept override;

  bool UseWebDebugger() const noexcept override;
  bool UseFastRefresh() const noexcept override;
  bool UseDirectDebugger() const noexcept override;
  bool DebuggerBreakOnNextLine() const noexcept override;
  uint16_t DebuggerPort() const noexcept override;
  std::string DebugBundlePath() const noexcept override;
  std::string BundleRootPath() const noexcept override;
  std::string SourceBundleHost() const noexcept override;
  uint16_t SourceBundlePort() const noexcept override;
  std::string JavaScriptBundleFile() const noexcept override;

#endif

 private:
  Mso::WeakPtr<ReactInstanceWin> m_reactInstance;
  winrt::Microsoft::ReactNative::IReactPropertyBag m_properties;
  winrt::Microsoft::ReactNative::IReactNotificationService m_notifications;
};
} // namespace Mso::React
