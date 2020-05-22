// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

// Since DeviceInfo provides constants that require the UI thread, we need to initialize it before the module creation
// (module creation can happen on background thread)
// DeviceInfoHolder is created on the UI thread and stored in the PropertyBag so that the DeviceInfo module can return
// the constants synchronously.
struct DeviceInfoHolder {
  DeviceInfoHolder();

  static void SetCallback(
      const React::ReactPropertyBag &propertyBag,
      Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept;
  static void InitDeviceInfoHolder(const React::ReactPropertyBag &propertyBag) noexcept;
  static React::JSValueObject GetDimensions(const React::ReactPropertyBag &propertyBag) noexcept;

 private:
  React::JSValueObject getDimensions() noexcept;
  void updateDeviceInfo() noexcept;
  void notifyChanged() noexcept;

  float m_windowWidth{0};
  float m_windowHeight{0};
  float m_scale{0};
  double m_textScaleFactor{0};
  float m_dpi{0};
  uint32_t m_screenWidth{0};
  uint32_t m_screenHeight{0};

  winrt::Windows::UI::Core::CoreWindow::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker m_dpiChangedRevoker{};
  Mso::Functor<void(React::JSValueObject &&)> m_notifyCallback;
};

REACT_MODULE(DeviceInfo)
struct DeviceInfo : public std::enable_shared_from_this<DeviceInfo> {
  REACT_INIT(Initialize)
  void Initialize(React::ReactContext const &reactContext) noexcept;

  REACT_CONSTANT_PROVIDER(GetConstants)
  void GetConstants(React::ReactConstantProvider &provider) noexcept;

 private:
  winrt::Microsoft::ReactNative::ReactContext m_context;
};

} // namespace Microsoft::ReactNative
