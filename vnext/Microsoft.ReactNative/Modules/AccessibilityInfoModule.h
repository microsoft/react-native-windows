// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include "codegen/NativeAccessibilityInfoSpec.g.h"
#include <NativeModules.h>

namespace Microsoft::ReactNative {

REACT_MODULE(AccessibilityInfo)
struct AccessibilityInfo : public std::enable_shared_from_this<AccessibilityInfo> {
  using ModuleSpec = ReactNativeSpecs::AccessibilityInfoSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_METHOD(isReduceMotionEnabled)
  void isReduceMotionEnabled(std::function<void(bool)> const &onSuccess) noexcept;

  REACT_METHOD(isInvertColorsEnabled)
  void isInvertColorsEnabled(std::function<void(bool)> const &onSuccess) noexcept;

  REACT_METHOD(isHighTextContrastEnabled)
  void isHighTextContrastEnabled(std::function<void(bool)> const &onSuccess) noexcept;

  REACT_METHOD(isTouchExplorationEnabled)
  void isTouchExplorationEnabled(std::function<void(bool)> const &onSuccess) noexcept;

  REACT_METHOD(setAccessibilityFocus)
  void setAccessibilityFocus(double reactTag) noexcept;

  REACT_METHOD(announceForAccessibility)
  void announceForAccessibility(std::wstring announcement) noexcept;

  REACT_METHOD(getRecommendedTimeoutMillis)
  void getRecommendedTimeoutMillis(double mSec, std::function<void(double)> const &onSuccess) noexcept;

  REACT_METHOD(isGrayscaleEnabled)
  void isGrayscaleEnabled(std::function<void(bool)> const &onSuccess) noexcept;

  REACT_METHOD(isAccessibilityServiceEnabled)
  void isAccessibilityServiceEnabled(std::function<void(bool)> const &onSuccess) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
