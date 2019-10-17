// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

namespace react {
namespace uwp {

enum class ResizeMode {
  Cover = 0,
  Contain = 1,
  Stretch = 2,
  Repeat = 3,
  Center = 4
};

struct ReactImageBrush
    : winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBaseT<
          ReactImageBrush> {
  using Super = winrt::Windows::UI::Xaml::Media::XamlCompositionBrushBaseT<
      ReactImageBrush>;

 private:
  // Constructors
  ReactImageBrush() = default;

 public:
  static winrt::com_ptr<ReactImageBrush> Create();
  template <typename D, typename... Args>
  friend winrt::com_ptr<D> winrt::make_self(Args &&... args);

  // XamlCompositionBaseBrush Overrides
  void OnConnected();
  void OnDisconnected();

  // Public Properties
  react::uwp::ResizeMode ResizeMode() {
    return m_resizeMode;
  }
  void ResizeMode(react::uwp::ResizeMode value);

  winrt::Windows::Foundation::Size AvailableSize() {
    return m_availableSize;
  }
  void AvailableSize(winrt::Windows::Foundation::Size const &value);

  void Source(winrt::Windows::UI::Xaml::Media::LoadedImageSurface const &value);

 private:
  void UpdateCompositionBrush();
  bool IsImageSmallerThanView();
  winrt::Windows::UI::Composition::CompositionStretch ResizeModeToStretch();
  winrt::Windows::UI::Composition::CompositionSurfaceBrush
  GetOrCreateSurfaceBrush();
  winrt::Windows::UI::Composition::CompositionEffectBrush
  GetOrCreateEffectBrush(
      winrt::Windows::UI::Composition::CompositionSurfaceBrush const
          &surfaceBrush);

  react::uwp::ResizeMode m_resizeMode{ResizeMode::Contain};
  winrt::Windows::Foundation::Size m_availableSize{};
  winrt::Windows::UI::Xaml::Media::LoadedImageSurface m_loadedImageSurface{
      nullptr};
  winrt::Windows::UI::Composition::CompositionEffectBrush m_effectBrush{
      nullptr};
};
} // namespace uwp
} // namespace react
