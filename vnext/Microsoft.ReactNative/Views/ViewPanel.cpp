// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "ViewPanel.h"

#include <UI.Xaml.Media.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ResourceBrushUtils.h>
#include <cxxreact/SystraceSection.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Xaml.Interop.h>

// Needed for latest versions of C++/WinRT
#if __has_include("ViewPanel.g.cpp")
#include "ViewPanel.g.cpp"
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

const winrt::TypeName viewPanelTypeName{winrt::hstring{L"ViewPanel"}, winrt::TypeKind::Metadata};

ViewPanel::ViewPanel() : Super() {}

winrt::AutomationPeer ViewPanel::OnCreateAutomationPeer() {
  // The parent ViewControl handles the peer needs
  return nullptr;
}

/*static*/ void ViewPanel::VisualPropertyChanged(
    xaml::DependencyObject sender,
    xaml::DependencyPropertyChangedEventArgs e) {
  auto panel{sender.as<ViewPanel>()};
  if (panel.get() != nullptr)
    panel->m_propertiesChanged = true;
}

/*static*/ xaml::DependencyProperty ViewPanel::ViewBackgroundProperty() {
  return xaml::Controls::Panel::BackgroundProperty();
}

/*static*/ xaml::DependencyProperty ViewPanel::BorderThicknessProperty() {
  return xaml::Controls::Grid::BorderThicknessProperty();
}

/*static*/ xaml::DependencyProperty ViewPanel::BorderBrushProperty() {
  return xaml::Controls::Grid::BorderBrushProperty();
}

/*static*/ xaml::DependencyProperty ViewPanel::CornerRadiusProperty() {
  return xaml::Controls::Grid::CornerRadiusProperty();
}

/*static*/ xaml::DependencyProperty ViewPanel::TopProperty() {
  return xaml::Controls::Canvas::TopProperty();
}

/*static*/ xaml::DependencyProperty ViewPanel::LeftProperty() {
  return xaml::Controls::Canvas::LeftProperty();
}

/*static*/ void ViewPanel::SetTop(xaml::UIElement const &element, double value) {
  element.SetValue(TopProperty(), winrt::box_value<double>(value));
  InvalidateForArrange(element);
}

/*static*/ void ViewPanel::SetLeft(xaml::UIElement const &element, double value) {
  element.SetValue(LeftProperty(), winrt::box_value<double>(value));
  InvalidateForArrange(element);
}

void ViewPanel::InvalidateForArrange(const xaml::DependencyObject &element) {
  // If the element's position has changed, we must invalidate the parent for arrange,
  // as it's the parent's responsibility to arrange its children.
  if (auto parent = VisualTreeHelper::GetParent(element)) {
    if (auto parentUIE = parent.try_as<xaml::UIElement>()) {
      parentUIE.InvalidateArrange();
    }
  }
}

winrt::Size ViewPanel::MeasureOverride(winrt::Size /*availableSize*/) {
  // All children are given as much size as they'd like
  winrt::Size childConstraint(INFINITY, INFINITY);

  for (xaml::UIElement child : Children())
    child.Measure(childConstraint);

  // ViewPanels never choose their size, that is completely up to the parent -
  // so return no size
  return winrt::Size(0, 0);
}

winrt::Size ViewPanel::ArrangeOverride(winrt::Size finalSize) {
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

    child.Arrange(winrt::Rect(
        static_cast<float>(ViewPanel::GetLeft(child)),
        static_cast<float>(ViewPanel::GetTop(child)),
        static_cast<float>(childWidth),
        static_cast<float>(childHeight)));
  }

  return finalSize;
}

void ViewPanel::InsertAt(uint32_t const index, xaml::UIElement const &value) const {
  Children().InsertAt(index, value);
}

void ViewPanel::RemoveAt(uint32_t const index) const {
  Children().RemoveAt(index);
}

void ViewPanel::Remove(xaml::UIElement element) const {
  uint32_t index;

  if (Children().IndexOf(element, index))
    Children().RemoveAt(index);
}

void ViewPanel::Clear() const {
  Children().Clear();
}

void ViewPanel::ViewBackground(winrt::Brush const &value) {
  SetValue(ViewBackgroundProperty(), winrt::box_value(value));
}

} // namespace winrt::Microsoft::ReactNative::implementation
