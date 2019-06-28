// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactImageBrush.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Storage.Streams.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <folly/dynamic.h>

namespace react {
  namespace uwp {

    struct ImageSource
    {
      std::string uri;
      std::string method;
      folly::dynamic headers;
      double width = 0;
      double height = 0;
      double scale = 1.0;
      bool packagerAsset = false;
    };

    struct ReactImage : winrt::Windows::UI::Xaml::Controls::CanvasT<ReactImage>
    {
      using Super = winrt::Windows::UI::Xaml::Controls::CanvasT<ReactImage>;
    private:
      // Constructors
      ReactImage();

    public:
      static winrt::com_ptr<ReactImage> Create();
      template <typename D, typename... Args> friend auto winrt::make_self(Args&& ... args);

      // Overrides
      virtual winrt::Windows::Foundation::Size ArrangeOverride(
        winrt::Windows::Foundation::Size finalSize);

      // Events
      winrt::event_token OnLoadEnd(winrt::Windows::Foundation::EventHandler<bool> const& handler);
      void OnLoadEnd(winrt::event_token const& token) noexcept;

      // Public Properties
      ImageSource Source() { return m_imageSource; }
      winrt::fire_and_forget Source(ImageSource source);

      react::uwp::ResizeMode ResizeMode() { return m_brush->ResizeMode(); }
      void ResizeMode(react::uwp::ResizeMode value) { m_brush->ResizeMode(value); }

    private:
      ImageSource m_imageSource;
      winrt::com_ptr<ReactImageBrush> m_brush;
      winrt::event<winrt::Windows::Foundation::EventHandler<bool>> m_onLoadEndEvent;
      winrt::Windows::UI::Xaml::Media::LoadedImageSurface::LoadCompleted_revoker m_surfaceLoadedRevoker;
    };

    // Helper functions
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream> GetImageStreamAsync(ImageSource source);
    winrt::Windows::Foundation::IAsyncOperation<winrt::Windows::Storage::Streams::InMemoryRandomAccessStream> GetImageInlineDataAsync(ImageSource source);
  }
} // namespace react::uwp
