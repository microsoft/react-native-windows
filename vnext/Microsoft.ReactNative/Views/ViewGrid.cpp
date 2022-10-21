// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "ViewGrid.h"

#include <UI.Xaml.Media.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ResourceBrushUtils.h>
#include <cxxreact/SystraceSection.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

// Needed for latest versions of C++/WinRT
#if __has_include("ViewGrid.g.cpp")
#include "ViewGrid.g.cpp"
#endif

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace xaml::Automation::Peers;
using namespace xaml::Controls;
using namespace xaml::Interop;
using namespace winrt::Windows::UI::Xaml::Interop;
using namespace xaml::Media;
using namespace Windows::Foundation;
} // namespace winrt

using namespace facebook::react;

namespace winrt::Microsoft::ReactNative::implementation {

ViewGrid::ViewGrid() : Super() {}

winrt::AutomationPeer ViewGrid::OnCreateAutomationPeer() {
  // The parent ViewControl handles the peer needs
  return nullptr;
}


winrt::Size ViewGrid::MeasureOverride(winrt::Size /*availableSize*/) {
  // All children are given as much size as they'd like
  winrt::Size childConstraint(INFINITY, INFINITY);

  for (xaml::UIElement child : Children())
    child.Measure(childConstraint);

  // ViewPanels never choose their size, that is completely up to the parent -
  // so return no size
  return winrt::Size(0, 0);
}

winrt::Size ViewGrid::ArrangeOverride(winrt::Size finalSize) {
  for (xaml::UIElement child : Children()) {
    double childHeight = 0.0;
    double childWidth = 0.0;

    // We expect elements to have been arranged by yoga which means their
    // Width & Height are set
    xaml::FrameworkElement fe = child.try_as<xaml::FrameworkElement>();
    if (fe != nullptr) {
      childWidth = fe.Width();
      childHeight = fe.Height();
    }
    // But we fall back to the measured size otherwise
    else {
      childWidth = child.DesiredSize().Width;
      childHeight = child.DesiredSize().Height;
    }

    // Guard against negative values
    childWidth = std::max<double>(0.0f, childWidth);
    childHeight = std::max<double>(0.0f, childHeight);

    float left = static_cast<float>(ViewPanel::GetLeft(child));
    float top = static_cast<float>(ViewPanel::GetTop(child));

    child.Arrange(
        winrt::Rect(left, top, static_cast<float>(childWidth), static_cast<float>(childHeight)));
  }

  return finalSize;
}

} // namespace winrt::Microsoft::ReactNative::implementation
