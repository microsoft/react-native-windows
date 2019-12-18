// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactImageBrush.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.Imaging.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.UI.Xaml.h>

#include <folly/dynamic.h>

namespace react {
namespace uwp {

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

struct ReactImage : winrt::Windows::UI::Xaml::Controls::GridT<ReactImage> {
  using Super = winrt::Windows::UI::Xaml::Controls::GridT<ReactImage>;

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

 private:
  winrt::Windows::UI::Xaml::Media::Stretch ResizeModeToStretch(react::uwp::ResizeMode value);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
  GetImageMemoryStreamAsync(ReactImageSource source);
  winrt::fire_and_forget SetBackground(bool fireLoadEndEvent);

  bool m_useCompositionBrush{false};
  ReactImageSource m_imageSource;
  react::uwp::ResizeMode m_resizeMode{ResizeMode::Contain};

  winrt::event<winrt::Windows::Foundation::EventHandler<bool>> m_onLoadEndEvent;
  winrt::Windows::UI::Xaml::FrameworkElement::SizeChanged_revoker m_sizeChangedRevoker;
  winrt::Windows::UI::Xaml::Media::LoadedImageSurface::LoadCompleted_revoker m_surfaceLoadedRevoker;
  winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::ImageOpened_revoker m_bitmapImageOpened;
  winrt::Windows::UI::Xaml::Media::ImageBrush::ImageOpened_revoker m_imageBrushOpenedRevoker;
  winrt::Windows::UI::Xaml::Media::ImageBrush::ImageFailed_revoker m_imageBrushFailedRevoker;
  winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::Opened_revoker m_svgImageSourceOpenedRevoker;
  winrt::Windows::UI::Xaml::Media::Imaging::SvgImageSource::OpenFailed_revoker m_svgImageSourceOpenFailedRevoker;
};

// Helper functions
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageStreamAsync(ReactImageSource source);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(ReactImageSource source);
} // namespace uwp
} // namespace react
