
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <winrt/Microsoft.ReactNative.Composition.h>
#include <Microsoft.ReactNative.Cxx/ReactContext.h>
#include <react/renderer/graphics/Color.h>

namespace Microsoft::ReactNative::Composition {

struct Theme {

  Theme(const winrt::Microsoft::ReactNative::ReactContext& reactContext) noexcept;
  winrt::Microsoft::ReactNative::Composition::IBrush Brush(const facebook::react::Color& color) noexcept;
  winrt::Microsoft::ReactNative::Composition::IBrush PlatformBrush(const std::string& platformColor) noexcept;
  winrt::Windows::UI::Color Color(const facebook::react::Color& color) noexcept;
  winrt::Windows::UI::Color PlatformColor(const std::string& platformColor) noexcept;

  static std::shared_ptr<Theme> FromContext(const winrt::Microsoft::ReactNative::ReactContext &context) noexcept;

private:
  std::pair<bool, winrt::Windows::UI::Color> TryGetPlatformColor(const std::string& platformColor) noexcept;

  std::unordered_map<std::string, std::pair<bool, winrt::Windows::UI::Color>> m_colorCache;
  std::unordered_map<std::string, winrt::Microsoft::ReactNative::Composition::IBrush> m_platformColorBrushCache;
  std::unordered_map<DWORD, winrt::Microsoft::ReactNative::Composition::IBrush> m_colorBrushCache;
  winrt::Microsoft::ReactNative::Composition::ICompositionContext m_compositionContext;
};

} // namespace Microsoft::ReactNative::Composition
