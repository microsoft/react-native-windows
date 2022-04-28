// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "YogaXamlPanel.h"

#include <Utils/PropertyUtils.h>
#include <winrt/Windows.Foundation.h>

// Needed for latest versions of C++/WinRT
#if __has_include("YogaXamlPanel.g.cpp")
#include "YogaXamlPanel.g.cpp"
#endif

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace Windows::Foundation;
} // namespace winrt

namespace winrt::Microsoft::ReactNative::implementation {

const winrt::TypeName yogaXamlPanelTypeName{winrt::hstring{L"YogaXamlPanel"}, winrt::TypeKind::Metadata};

YogaXamlPanel::YogaXamlPanel(
    std::function<void(facebook::react::Size size)> &&onMeasured,
    std::function<const facebook::react::LayoutConstraints &()> &&getConstraints)
    : Super(), m_onMeasured(std::move(onMeasured)), m_getConstraints(std::move(getConstraints)) {}

winrt::Size YogaXamlPanel::MeasureOverride(winrt::Size /*availableSize*/) {
  auto &constraints = m_getConstraints();
  winrt::Size availableSize{{constraints.maximumSize.width, constraints.maximumSize.height}};

  assert(Children().Size() == 1);
  auto child = Children().GetAt(0);

  auto widthProp = xaml::FrameworkElement::WidthProperty();
  auto heightProp = xaml::FrameworkElement::HeightProperty();
  auto origWidth = child.GetValue(widthProp);
  auto origHeight = child.GetValue(heightProp);
  child.ClearValue(widthProp);
  child.ClearValue(heightProp);

  child.Measure(availableSize);
  auto desiredSize = child.DesiredSize();

  child.SetValue(widthProp, origWidth);
  child.SetValue(heightProp, origHeight);

  m_onMeasured({desiredSize.Width, desiredSize.Height});

  // YogaXamlPanel never choose their size, that is completely up to yoga -
  // so return no size
  return winrt::Size(0, 0);
}

winrt::Size YogaXamlPanel::ArrangeOverride(winrt::Size finalSize) {
  assert(Children().Size() == 1);
  auto child = Children().GetAt(0);
  child.Arrange(winrt::Windows::Foundation::Rect{0, 0, finalSize.Width, finalSize.Height});
  return finalSize;
}

} // namespace winrt::Microsoft::ReactNative::implementation
