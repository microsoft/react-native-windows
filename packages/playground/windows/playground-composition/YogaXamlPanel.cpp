// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "YogaXamlPanel.h"

#include <winrt/Windows.Foundation.h>

// Needed for latest versions of C++/WinRT
#if __has_include("App.YogaXamlPanel.g.cpp")
#include "App.YogaXamlPanel.g.cpp"
#endif

namespace winrt {
using namespace Windows::UI;
using namespace xaml;
using namespace Windows::Foundation;
} // namespace winrt

namespace winrt::PlaygroundApp::implementation {

const winrt::Windows::UI::Xaml::Interop::TypeName yogaXamlPanelTypeName{winrt::hstring{L"YogaXamlPanel"}, winrt::Windows::UI::Xaml::Interop::TypeKind::Metadata};

YogaXamlPanel::YogaXamlPanel(std::function<void(winrt::Windows::Foundation::Size size)>&& onMeasured, std::function<const winrt::Microsoft::ReactNative::LayoutConstraints&()>&& getConstraints) : Super(), m_onMeasured(std::move(onMeasured)), m_getConstraints(std::move(getConstraints)) {}

winrt::Size YogaXamlPanel::MeasureOverride(winrt::Windows::Foundation::Size availableSize) {

  auto& constraints = m_getConstraints();
  //winrt::Size availableSize{{constraints.MaximumSize.Width, constraints.MaximumSize.Height}};

  assert(Children().Size() == 1);
  auto child = Children().GetAt(0);

  //child.Measure(availableSize);
  child.Measure({std::numeric_limits<float>::max(), std::numeric_limits<float>::max() });

  auto desiredSize = child.DesiredSize();
  m_onMeasured({desiredSize.Width, desiredSize.Height});

  child.Measure(availableSize);
  auto desiredSize2 = child.DesiredSize();
  return desiredSize2;

  // YogaXamlPanel never choose their size, that is completely up to yoga -
  // so return no size
  // return winrt::Size(0, 0);
  // return desiredSize;
}

winrt::Size YogaXamlPanel::ArrangeOverride(winrt::Windows::Foundation::Size finalSize) {
  assert(Children().Size() == 1);
  auto child = Children().GetAt(0);
  child.Arrange(winrt::Windows::Foundation::Rect{0, 0, finalSize.Width, finalSize.Height});
  return finalSize;
}

} // namespace winrt::PlaygroundApp::implementation
