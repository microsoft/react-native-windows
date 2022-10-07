// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "../../codegen/NativeAppearanceSpec.g.h"
#include <React.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace Microsoft::ReactNative {

REACT_MODULE(Appearance)
struct Appearance : std::enable_shared_from_this<Appearance> {
  using ApplicationTheme = xaml::ApplicationTheme;
  using UISettings = winrt::Windows::UI::ViewManagement::UISettings;
  using ModuleSpec = ReactNativeSpecs::AppearanceSpec;

  REACT_INIT(Initialize)
  void Initialize(winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept;

  REACT_SYNC_METHOD(getColorScheme)
  std::optional<std::string> getColorScheme() noexcept;

  REACT_METHOD(addListener)
  void addListener(std::string eventName) noexcept;

  REACT_METHOD(removeListeners)
  void removeListeners(double count) noexcept;

  REACT_EVENT(appearanceChanged);
  std::function<void(std::string const &)> appearanceChanged;

  // This function allows the module to get the current theme on the UI thread before it is requested by any JS thread
  static void InitOnUIThread(const Mso::React::IReactContext &context) noexcept;

 private:
  static const char *ToString(ApplicationTheme theme) noexcept;
  ApplicationTheme GetCurrentTheme() noexcept;
  void RequeryTheme() noexcept;

  winrt::Microsoft::ReactNative::ReactContext m_context;
  UISettings m_uiSettings;
  UISettings::ColorValuesChanged_revoker m_revoker;
};

} // namespace Microsoft::ReactNative
