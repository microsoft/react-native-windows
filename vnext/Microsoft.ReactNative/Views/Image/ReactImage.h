// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"
#include "ReactImageBrush.h"

#include <UI.Xaml.Controls.h>

#include <JSValue.h>
#include <Utils/ImageUtils.h>
#include <folly/dynamic.h>

namespace Microsoft::ReactNative {

struct ReactImage : xaml::Controls::GridT<ReactImage> {
  using Super = xaml::Controls::GridT<ReactImage>;

  ReactImage() = default;

 public:
  static winrt::com_ptr<ReactImage> Create();

  // Overrides
  winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);
  xaml::Automation::Peers::AutomationPeer OnCreateAutomationPeer();

  // Events
  winrt::event_token OnLoadEnd(winrt::Windows::Foundation::EventHandler<bool> const &handler);
  void OnLoadEnd(winrt::event_token const &token) noexcept;

  // Public Properties
  const ReactImageSource &Source() {
    return m_imageSource;
  }
  void Source(ReactImageSource source);

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

 private:
  xaml::Media::Stretch ResizeModeToStretch();
  xaml::Media::Stretch ResizeModeToStretch(winrt::Windows::Foundation::Size size);
  winrt::fire_and_forget SetBackground(bool fireLoadEndEvent);
  double GetWidth();
  double GetHeight();

  bool m_useCompositionBrush{false};
  float m_blurRadius{0};
  int m_imageSourceId{0};
  ReactImageSource m_imageSource;
  facebook::react::ImageResizeMode m_resizeMode{facebook::react::ImageResizeMode::Contain};
  winrt::Windows::UI::Color m_tintColor{winrt::Colors::Transparent()};

  winrt::event<winrt::Windows::Foundation::EventHandler<bool>> m_onLoadEndEvent;
  xaml::FrameworkElement::SizeChanged_revoker m_sizeChangedRevoker;
  xaml::Media::LoadedImageSurface::LoadCompleted_revoker m_surfaceLoadedRevoker;
  xaml::Media::Imaging::BitmapImage::ImageOpened_revoker m_bitmapImageOpened;
  xaml::Media::Imaging::BitmapImage::ImageFailed_revoker m_bitmapImageFailed;
  xaml::Media::ImageBrush::ImageOpened_revoker m_imageBrushOpenedRevoker;
  xaml::Media::Imaging::SvgImageSource::Opened_revoker m_svgImageSourceOpenedRevoker;
  xaml::Media::Imaging::SvgImageSource::OpenFailed_revoker m_svgImageSourceOpenFailedRevoker;
};

} // namespace Microsoft::ReactNative
