// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

// Since I18nModule provides constants that require the UI thread, we need to initialize those constants before module
// creation (module creation can happen on background thread) InitI18nInfo will store the required information in the
// PropertyBag so that the I18nModule can return the constants synchronously.
REACT_MODULE(I18nManager)
struct I18nManager {
  static void InitI18nInfo(const React::ReactPropertyBag &propertyBag) noexcept;
  static bool IsRTL(const React::ReactPropertyBag &propertyBag) noexcept;

  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_CONSTANT_PROVIDER(GetConstants)
  void GetConstants(React::ReactConstantProvider &provider) noexcept;

  REACT_METHOD(AllowRTL, L"allowRTL")
  void AllowRTL(bool allowRTL) noexcept;

  REACT_METHOD(ForceRTL, L"forceRTL")
  void ForceRTL(bool forceRTL) noexcept;

  REACT_METHOD(SwapLeftAndRightInRTL, L"swapLeftAndRightInRTL")
  void SwapLeftAndRightInRTL(bool flipStyles) noexcept;

 private:
  bool IsRTL() noexcept;

  React::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
