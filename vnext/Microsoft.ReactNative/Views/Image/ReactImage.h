// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"
#include "ReactImageBrush.h"

#include <UI.Xaml.Controls.h>

#include <folly/dynamic.h>

namespace react::uwp {

enum class ImageSourceType { Uri = 0, Download = 1, InlineData = 2, Svg = 3 };

struct ReactImageSource {
  std::string uri;
  std::string method;
  std::string bundleRootPath;
  folly::dynamic headers;
  double width = 0;
  double height = 0;
  double scale = 1.0;
  bool packagerAsset = false;
  ImageSourceType sourceType = ImageSourceType::Uri;
};

struct ReactImage : xaml::Controls::GridT<ReactImage> {
  using Super = xaml::Controls::GridT<ReactImage>;

  ReactImage() = default;

 public:
  static winrt::com_ptr<ReactImage> Create();

  // Overrides
  winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

  // Events
  winrt::event_token OnLoadEnd(winrt::Windows::Foundation::EventHandler<bool> const &handler);
  void OnLoadEnd(winrt::event_token const &token) noexcept;

  // Public Properties
  ReactImageSource Source() {
    return m_imageSource;
  }
  void Source(ReactImageSource source);

  react::uwp::ResizeMode ResizeMode() {
    return m_resizeMode;
  }
  void ResizeMode(react::uwp::ResizeMode value);

  float BlurRadius() {
    return m_blurRadius;
  }
  void BlurRadius(float value);

  winrt::Windows::UI::Color TintColor() {
    return m_tintColor;
  }
  void TintColor(winrt::Windows::UI::Color value);

 private:
  xaml::Media::Stretch ResizeModeToStretch(react::uwp::ResizeMode value);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
  GetImageMemoryStreamAsync(ReactImageSource source);
  winrt::fire_and_forget SetBackground(bool fireLoadEndEvent);

  bool m_useCompositionBrush{false};
  float m_blurRadius{0};
  ReactImageSource m_imageSource;
  react::uwp::ResizeMode m_resizeMode{ResizeMode::Contain};
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

// Helper functions
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageStreamAsync(ReactImageSource source);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(ReactImageSource source);
} // namespace react::uwp
