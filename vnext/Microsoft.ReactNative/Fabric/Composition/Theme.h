
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Composition.Theme.g.h"
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <react/renderer/graphics/Color.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.ReactNative.Composition.h>
#include <winrt/Windows.UI.ViewManagement.h>

namespace winrt::Microsoft::ReactNative::Composition::implementation {

struct Theme : ThemeT<Theme, Experimental::IInternalTheme> {
  Theme(
      const winrt::Microsoft::ReactNative::ReactContext &reactContext,
      const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &customResourceLoader) noexcept;

// Public APIs
  winrt::Microsoft::UI::Composition::CompositionBrush PlatformBrush(winrt::hstring platformColor) noexcept;
  bool TryGetPlatformColor(winrt::hstring platformColor, winrt::Windows::UI::Color &color) noexcept;
  bool IsEmpty() const noexcept;

  winrt::event_token ThemeChanged(
      winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const &handler) noexcept;
  void ThemeChanged(winrt::event_token const &token) noexcept;

  // Internal APIs
  Theme() noexcept;

  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush InternalPlatformBrush(
      winrt::hstring platformColor) noexcept;

  winrt::Windows::UI::Color PlatformColor(const std::string &platformColor) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush PlatformBrush(
      const std::string &platformColor) noexcept;
  winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Brush(const facebook::react::Color &color) noexcept;
  winrt::Windows::UI::Color Color(const facebook::react::Color &color) noexcept;

  D2D1::ColorF D2DColor(const facebook::react::Color &color) noexcept;
  D2D1::ColorF D2DPlatformColor(const std::string &platformColor) noexcept;

  static winrt::Microsoft::ReactNative::Composition::Theme FromContext(
      const winrt::Microsoft::ReactNative::ReactContext &context) noexcept;
  static winrt::Microsoft::ReactNative::Composition::Theme EmptyTheme() noexcept;

  static winrt::Microsoft::ReactNative::Composition::Theme GetDefaultTheme(
      const winrt::Microsoft::ReactNative::IReactContext &context) noexcept;
  static void SetDefaultResources(
      const winrt::Microsoft::ReactNative::ReactInstanceSettings &settings,
      const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept;

 private:
  void UpdateCustomResources(
      const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &resources) noexcept;
  bool TryGetPlatformColor(const std::string &platformColor, winrt::Windows::UI::Color &color) noexcept;
  void ClearCacheAndRaiseChangedEvent() noexcept;

  winrt::event<winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable>> m_themeChangedEvent;
  bool m_emptyTheme{false};
  bool m_darkTheme{false};
  bool m_highContrast{false};
  std::unordered_map<std::string, std::pair<bool, winrt::Windows::UI::Color>> m_colorCache;
  winrt::Windows::UI::ViewManagement::UISettings m_uisettings;
  winrt::Windows::UI::ViewManagement::UISettings::ColorValuesChanged_revoker m_colorValuesChangedRevoker;
  std::unordered_map<std::string, winrt::Microsoft::ReactNative::Composition::Experimental::IBrush>
      m_platformColorBrushCache;
  std::unordered_map<DWORD, winrt::Microsoft::ReactNative::Composition::Experimental::IBrush> m_colorBrushCache;
  winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext m_compositionContext;
  winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader m_customResourceLoader;
  winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader::ResourcesChanged_revoker m_resourceChangedRevoker;
};

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::Composition::factory_implementation {
struct Theme : ThemeT<Theme, implementation::Theme> {};
} // namespace winrt::Microsoft::ReactNative::Composition::factory_implementation
