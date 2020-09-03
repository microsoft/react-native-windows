// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(AccessibilityInfo)
struct AccessibilityInfo : public std::enable_shared_from_this<AccessibilityInfo> {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(isReduceMotionEnabled)
  void isReduceMotionEnabled(std::function<void(React::JSValue const &)> const &onSuccess) noexcept;

  REACT_METHOD(isTouchExplorationEnabled)
  void isTouchExplorationEnabled(std::function<void(React::JSValue const &)> const &onSuccess) noexcept;

  REACT_METHOD(setAccessibilityFocus)
  void setAccessibilityFocus(double reactTag) noexcept;

  REACT_METHOD(announceForAccessibility)
  void announceForAccessibility(std::string announcement) noexcept;

 private:
  React::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
