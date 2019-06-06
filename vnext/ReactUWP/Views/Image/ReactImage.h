// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#pragma once

#include "ReactImageBrush.h"

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Media.h>

namespace react {
  namespace uwp {

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
      virtual winrt::Windows::Foundation::Size ArrangeOverride(winrt::Windows::Foundation::Size finalSize);

      // Public Properties
      winrt::Windows::Foundation::Uri SourceUri() { return m_brush->SourceUri(); }
      void SourceUri(winrt::Windows::Foundation::Uri const& value) { m_brush->SourceUri(value); }

      react::uwp::ResizeMode ResizeMode() { return m_brush->ResizeMode(); }
      void ResizeMode(react::uwp::ResizeMode value) { m_brush->ResizeMode(value); }

    private:
      winrt::com_ptr<ReactImageBrush> m_brush;
    };

  }
} // namespace react::uwp
