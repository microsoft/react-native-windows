// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <UI.Xaml.Media.h>
#include <winrt/Windows.Foundation.h>
#include "CppWinRTIncludes.h"

namespace react {
namespace uwp {

enum class ResizeMode { Cover = 0, Contain = 1, Stretch = 2, Repeat = 3, Center = 4 };

struct ReactImageBrush : xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush> {
  using Super = xaml::Media::XamlCompositionBrushBaseT<ReactImageBrush>;

  ReactImageBrush() = default;

 public:
  static winrt::com_ptr<ReactImageBrush> Create();

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

  void Source(xaml::Media::LoadedImageSurface const &value);

 private:
  void UpdateCompositionBrush();
  bool IsImageSmallerThanView();
  comp::CompositionStretch ResizeModeToStretch();
  comp::CompositionSurfaceBrush GetOrCreateSurfaceBrush();
  comp::CompositionEffectBrush GetOrCreateEffectBrush(comp::CompositionSurfaceBrush const &surfaceBrush);

  react::uwp::ResizeMode m_resizeMode{ResizeMode::Contain};
  winrt::Windows::Foundation::Size m_availableSize{};
  xaml::Media::LoadedImageSurface m_loadedImageSurface{nullptr};
  comp::CompositionEffectBrush m_effectBrush{nullptr};
};
} // namespace uwp
} // namespace react
