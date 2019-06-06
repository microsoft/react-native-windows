// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ReactImage.h"

namespace winrt {
  using namespace Windows::UI;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Media;
  using namespace Windows::Foundation;
}

namespace react {
  namespace uwp {

    ReactImage::ReactImage()
    {
      m_brush = ReactImageBrush::Create();
      this->Background(m_brush.as<winrt::XamlCompositionBrushBase>());
    }

    /*static*/ winrt::com_ptr<ReactImage> ReactImage::Create()
    {
      return winrt::make_self<ReactImage>();
    }

    winrt::Size ReactImage::ArrangeOverride(winrt::Size finalSize)
    {
      auto brush{ Background().as<ReactImageBrush>() };
      brush->AvailableSize(finalSize);

      return finalSize;
    }
  }
} // namespace react::uwp
