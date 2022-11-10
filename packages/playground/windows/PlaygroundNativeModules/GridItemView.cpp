// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "GridItemView.h"
#include "GridItemView.g.cpp"

namespace winrt {
using namespace Microsoft::ReactNative;
using namespace Windows::Foundation;
using namespace xaml;
using namespace xaml::Controls;
} // namespace winrt

namespace winrt::PlaygroundNativeModules::implementation {

winrt::Size GridItemView::ArrangeOverride(winrt::Size availableSize) {
  const auto desiredSize = Super::ArrangeOverride(availableSize);
  if (Children().Size() > 0) {
    if (const auto child = Children().GetAt(0).try_as<xaml::FrameworkElement>()) {
      const auto reactTag = React::XamlHelper::GetReactTag(child);
      if (reactTag != -1) {
        React::LayoutService::FromContext(m_reactContext).ApplyLayout(reactTag, desiredSize.Width, desiredSize.Height);
      }
    }
  }
  return desiredSize;
}

} // namespace winrt::PlaygroundNativeModules::implementation
