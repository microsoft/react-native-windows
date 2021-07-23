// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <UI.Xaml.Media.h>
#include <react/renderer/imagemanager/primitives.h>
#include <winrt/Windows.Foundation.h>
#include "CppWinRTIncludes.h"

namespace Microsoft::ReactNative {

struct ReactImageBrush : xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush> {
  using Super = xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush>;

  ReactImageBrush() = default;

 public:
  static winrt::com_ptr<ReactImageBrush> Create();

  // XamlCompositionBaseBrush Overrides
  void OnConnected();
  void OnDisconnected();

  // Public Properties
  facebook::react::ImageResizeMode ResizeMode() {
    return m_resizeMode;
  }
  void ResizeMode(facebook::react::ImageResizeMode value);

  float BlurRadius() {
    return m_blurRadius;
  }
  void BlurRadius(float value);

  winrt::Windows::UI::Color TintColor() {
    return m_tintColor;
  }
  void TintColor(winrt::Windows::UI::Color value);

  winrt::Windows::Foundation::Size AvailableSize() {
    return m_availableSize;
  }
  void AvailableSize(winrt::Windows::Foundation::Size const &value);

  void Source(xaml::Media::LoadedImageSurface const &value);

 private:
  static constexpr auto TintColorColor{L"TintColor.Color"};
  static constexpr auto BlurBlurAmount{L"Blur.BlurAmount"};

  void UpdateCompositionBrush(bool forceEffectBrush = false);
  bool IsImageSmallerThanView();
  comp::CompositionStretch ResizeModeToStretch();
  comp::CompositionSurfaceBrush GetOrCreateSurfaceBrush();
  comp::CompositionEffectBrush GetOrCreateEffectBrush(
      comp::CompositionSurfaceBrush const &surfaceBrush,
      bool forceEffectBrush = false);

  float m_blurRadius{0};
  facebook::react::ImageResizeMode m_resizeMode{facebook::react::ImageResizeMode::Contain};
  winrt::Windows::UI::Color m_tintColor{winrt::Colors::Transparent()};
  winrt::Windows::Foundation::Size m_availableSize{};
  xaml::Media::LoadedImageSurface m_loadedImageSurface{nullptr};
  comp::CompositionEffectBrush m_effectBrush{nullptr};
};
} // namespace Microsoft::ReactNative
