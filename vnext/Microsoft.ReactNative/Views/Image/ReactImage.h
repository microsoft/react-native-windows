// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CppWinRTIncludes.h"
#include "ReactImageBrush.h"

#include <UI.Xaml.Controls.h>

#include <JSValue.h>
#include <folly/dynamic.h>

namespace Microsoft::ReactNative {

enum class ImageSourceType { Uri = 0, Download = 1, InlineData = 2 };
enum class ImageSourceFormat { Bitmap = 0, Svg = 1 };

struct ReactImageSource {
  std::string uri;
  std::string method;
  std::string bundleRootPath;
  std::vector<std::pair<std::string, std::string>> headers;
  double width = 0;
  double height = 0;
  double scale = 1.0;
  bool packagerAsset = false;
  ImageSourceType sourceType = ImageSourceType::Uri;
  ImageSourceFormat sourceFormat = ImageSourceFormat::Bitmap;
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
  xaml::Media::Stretch ResizeModeToStretch(facebook::react::ImageResizeMode value);
  winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
  GetImageMemoryStreamAsync(ReactImageSource source);
  winrt::fire_and_forget SetBackground(bool fireLoadEndEvent);

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

// Helper functions
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageStreamAsync(ReactImageSource source);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(ReactImageSource source);
} // namespace Microsoft::ReactNative
