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

struct ImageSource {
  std::string uri;
  std::string method;
  std::string bundleRootPath;
  folly::dynamic headers;
  double width = 0;
  double height = 0;
  double scale = 1.0;
  bool packagerAsset = false;
};

struct ReactImage : winrt::Windows::UI::Xaml::Controls::CanvasT<ReactImage> {
  using Super = winrt::Windows::UI::Xaml::Controls::CanvasT<ReactImage>;

  ReactImage();

 public:
  static winrt::com_ptr<ReactImage> Create();

  // Overrides
  winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

  // Events
  winrt::event_token OnLoadEnd(winrt::Windows::Foundation::EventHandler<bool> const &handler);
  void OnLoadEnd(winrt::event_token const &token) noexcept;

  // Public Properties
  ImageSource Source() {
    return m_imageSource;
  }
  winrt::fire_and_forget Source(ImageSource source);

  react::uwp::ResizeMode ResizeMode() {
    return m_resizeMode;
  }
  void ResizeMode(react::uwp::ResizeMode value);

 private:
  bool ShouldUseCompositionBrush();

  bool m_useCompositionBrush = false;
  ImageSource m_imageSource;
  winrt::Windows::UI::Xaml::Media::ImageBrush m_bitmapBrush{};
  winrt::com_ptr<ReactImageBrush> m_brush;
  react::uwp::ResizeMode m_resizeMode{ResizeMode::Contain};
  winrt::event<winrt::Windows::Foundation::EventHandler<bool>> m_onLoadEndEvent;
  winrt::Windows::UI::Xaml::Media::LoadedImageSurface::LoadCompleted_revoker m_surfaceLoadedRevoker;
  winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::ImageOpened_revoker m_imageOpenedRevoker;
  winrt::Windows::UI::Xaml::Media::Imaging::BitmapImage::ImageFailed_revoker m_imageFailedRevoker;
};

// Helper functions
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageStreamAsync(ImageSource source);
winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream>
GetImageInlineDataAsync(ImageSource source);
} // namespace uwp
} // namespace react
