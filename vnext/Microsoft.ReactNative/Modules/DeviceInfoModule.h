// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.
#pragma once

#include <NativeModules.h>
#include <winrt/Windows.ApplicationModel.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Graphics.Display.h>

namespace Microsoft::ReactNative {

REACT_MODULE(DeviceInfo)
struct DeviceInfo: public std::enable_shared_from_this<DeviceInfo> {
  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_CONSTANT_PROVIDER(getConstants)
  void getConstants(React::ReactConstantProvider &provider) noexcept;

private:
  void emitUpdate() noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  winrt::Windows::UI::Core::CoreWindow::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Windows::Graphics::Display::DisplayInformation::DpiChanged_revoker m_dpiChangedRevoker{};
};

} // namespace Microsoft::ReactNative
