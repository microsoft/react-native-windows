// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <React.h>
#include <ReactNotificationService.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

// Since DeviceInfo provides constants that require the UI thread, we need to initialize it before the module creation
// (module creation can happen on background thread)
// DeviceInfoHolder is created on the UI thread and stored in the PropertyBag so that the DeviceInfo module can return
// the constants synchronously.
struct DeviceInfoHolder {
  DeviceInfoHolder(const Mso::React::IReactContext &context);

  static void SetCallback(
      const React::ReactPropertyBag &propertyBag,
      Mso::Functor<void(React::JSValueObject &&)> &&callback) noexcept;
  static void InitDeviceInfoHolder(const Mso::React::IReactContext &context) noexcept;
  static React::JSValueObject GetDimensions(const React::ReactPropertyBag &propertyBag) noexcept;

 private:
  React::JSValueObject getDimensions() noexcept;
  void updateDeviceInfo() noexcept;
  void notifyChanged() noexcept;

  float m_windowWidth{0};
  float m_windowHeight{0};
  float m_scale{1.0f};
  double m_textScaleFactor{0};
  float m_dpi{96};
  uint32_t m_screenWidth{0};
  uint32_t m_screenHeight{0};

  winrt::Windows::UI::Core::CoreWindow::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker m_dpiChangedRevoker{};
  Mso::Functor<void(React::JSValueObject &&)> m_notifyCallback;
  winrt::Microsoft::ReactNative::ReactNotificationSubscription m_wmSubscription{};
  Mso::CntPtr<const Mso::React::IReactContext> m_context{};
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
