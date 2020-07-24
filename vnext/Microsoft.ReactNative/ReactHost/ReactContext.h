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

 private:
  Mso::WeakPtr<ReactInstanceWin> m_reactInstance;
  winrt::Microsoft::ReactNative::IReactPropertyBag m_properties;
  winrt::Microsoft::ReactNative::IReactNotificationService m_notifications;
};
} // namespace Mso::React
