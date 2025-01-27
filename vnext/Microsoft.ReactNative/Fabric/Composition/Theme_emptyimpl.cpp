
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "Theme.h"

#include "Composition.Theme.g.cpp"
#include "winrt/Microsoft.ReactNative.Composition.h"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

Theme::Theme(
    const winrt::Microsoft::ReactNative::ReactContext &,
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &) noexcept {}

void Theme::ClearCacheAndRaiseChangedEvent() noexcept {}

Theme::Theme() noexcept {}

bool Theme::TryGetPlatformColor(winrt::hstring, winrt::Windows::UI::Color &) noexcept {
  return false;
}

bool Theme::TryGetPlatformColor(const std::string &, winrt::Windows::UI::Color &) noexcept {
  return false;
}

winrt::Windows::UI::Color Theme::Color(const facebook::react::Color &) noexcept {
  return {};
}

winrt::Windows::UI::Color Theme::PlatformColor(const std::string &) noexcept {
  return {};
}

winrt::Microsoft::UI::Composition::CompositionBrush Theme::PlatformBrush(winrt::hstring) noexcept {
  return nullptr;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Theme::PlatformBrush(const std::string &) noexcept {
  return nullptr;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IBrush Theme::Brush(const facebook::react::Color &) noexcept {
  return nullptr;
}

D2D1::ColorF Theme::D2DColor(const facebook::react::Color &) noexcept {
  return D2D1::ColorF::Blue;
}

D2D1::ColorF Theme::D2DPlatformColor(const std::string &) noexcept {
  return D2D1::ColorF::Blue;
}

bool Theme::IsEmpty() const noexcept {
  return false;
}

winrt::event_token Theme::ThemeChanged(
    winrt::Windows::Foundation::EventHandler<winrt::Windows::Foundation::IInspectable> const &) noexcept {
  return {};
}
void Theme::ThemeChanged(winrt::event_token const &) noexcept {}

winrt::Microsoft::ReactNative::Composition::Theme Theme::EmptyTheme() noexcept {
  return nullptr;
}

/*static*/ winrt::Microsoft::ReactNative::Composition::Theme Theme::GetDefaultTheme(
    const winrt::Microsoft::ReactNative::IReactContext &) noexcept {
  return nullptr;
}

/*static*/ void Theme::SetDefaultResources(
    const winrt::Microsoft::ReactNative::ReactInstanceSettings &,
    const winrt::Microsoft::ReactNative::Composition::ICustomResourceLoader &) noexcept {}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation
