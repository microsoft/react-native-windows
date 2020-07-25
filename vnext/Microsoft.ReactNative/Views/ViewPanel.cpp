// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "DynamicAutomationPeer.h"
#include "ViewPanel.h"

#include <UI.Xaml.Media.h>
#include <Utils/PropertyUtils.h>
#include <Utils/ResourceBrushUtils.h>
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

namespace winrt::PROJECT_ROOT_NAMESPACE::implementation {

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

/*static*/ void ViewPanel::PositionPropertyChanged(
    xaml::DependencyObject sender,
    xaml::DependencyPropertyChangedEventArgs e) {
  auto element{sender.as<xaml::UIElement>()};
  if (element != nullptr)
    element.InvalidateArrange();
}

/*static*/ xaml::DependencyProperty ViewPanel::ViewBackgroundProperty() {
  static xaml::DependencyProperty s_viewBackgroundProperty = xaml::DependencyProperty::Register(
      L"ViewBackground",
      winrt::xaml_typename<winrt::Brush>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(xaml::Media::SolidColorBrush(), ViewPanel::VisualPropertyChanged));

  return s_viewBackgroundProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::BorderThicknessProperty() {
  static xaml::DependencyProperty s_borderThicknessProperty = xaml::DependencyProperty::Register(
      L"BorderThickness",
      winrt::xaml_typename<winrt::Thickness>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(winrt::box_value(winrt::Thickness()), ViewPanel::VisualPropertyChanged));

  return s_borderThicknessProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::BorderBrushProperty() {
  static xaml::DependencyProperty s_borderBrushProperty = xaml::DependencyProperty::Register(
      L"BorderBrush",
      winrt::xaml_typename<winrt::Brush>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(xaml::Media::SolidColorBrush(), ViewPanel::VisualPropertyChanged));

  return s_borderBrushProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::CornerRadiusProperty() {
  static xaml::DependencyProperty s_cornerRadiusProperty = xaml::DependencyProperty::Register(
      L"CornerRadius",
      winrt::xaml_typename<xaml::CornerRadius>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(winrt::box_value(xaml::CornerRadius()), ViewPanel::VisualPropertyChanged));

  return s_cornerRadiusProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::TopProperty() {
  static xaml::DependencyProperty s_topProperty = xaml::DependencyProperty::RegisterAttached(
      L"Top",
      winrt::xaml_typename<double>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(winrt::box_value((double)0), ViewPanel::PositionPropertyChanged));

  return s_topProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::LeftProperty() {
  static xaml::DependencyProperty s_LeftProperty = xaml::DependencyProperty::RegisterAttached(
      L"Left",
      winrt::xaml_typename<double>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(winrt::box_value((double)0), ViewPanel::PositionPropertyChanged));

  return s_LeftProperty;
}

/*static*/ xaml::DependencyProperty ViewPanel::ClipChildrenProperty() {
  static xaml::DependencyProperty s_clipChildrenProperty = xaml::DependencyProperty::Register(
      L"ClipChildren",
      winrt::xaml_typename<bool>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(winrt::box_value(false), ViewPanel::VisualPropertyChanged));

  return s_clipChildrenProperty;
}

/*static*/ void ViewPanel::SetTop(xaml::UIElement const &element, double value) {
  element.SetValue(TopProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
}

/*static*/ void ViewPanel::SetLeft(xaml::UIElement const &element, double value) {
  element.SetValue(LeftProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
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
  // Sometimes we create outerBorder(i.e. when CornerRadius is true) instead of innerBorder,
  // Yoga has no notion of outerBorder when calculating the child's position, so we
  // need to make adujustment in arrange for outerborder's thickness.
  float outerBorderLeft = 0;
  float outerBorderTop = 0;
  if (auto outerBorder = GetOuterBorder()) {
    auto borderThickness = outerBorder.BorderThickness();
    outerBorderLeft = static_cast<float>(borderThickness.Left);
    outerBorderTop = static_cast<float>(borderThickness.Top);
  }
  for (xaml::UIElement child : Children()) {
    double childHeight = 0.0;
    double childWidth = 0.0;

    // A Border or inner ViewPanel should take up the same space as this panel
    if (child == m_border) {
      childWidth = finalSize.Width;
      childHeight = finalSize.Height;
    } else {
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
    }

    // Guard against negative values
    childWidth = std::max<double>(0.0f, childWidth);
    childHeight = std::max<double>(0.0f, childHeight);

    float adjustedLeft = static_cast<float>(ViewPanel::GetLeft(child)) - outerBorderLeft;
    float adjustedTop = static_cast<float>(ViewPanel::GetTop(child)) - outerBorderTop;

    child.Arrange(
        winrt::Rect(adjustedLeft, adjustedTop, static_cast<float>(childWidth), static_cast<float>(childHeight)));
  }

  UpdateClip(finalSize);

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

void ViewPanel::BorderThickness(winrt::Thickness const &value) {
  SetValue(BorderThicknessProperty(), winrt::box_value(value));
}

void ViewPanel::BorderBrush(winrt::Brush const &value) {
  SetValue(BorderBrushProperty(), winrt::box_value(value));
}

void ViewPanel::CornerRadius(xaml::CornerRadius const &value) {
  SetValue(CornerRadiusProperty(), winrt::box_value(value));
}

void ViewPanel::ClipChildren(bool value) {
  SetValue(ClipChildrenProperty(), winrt::box_value(value));
}

void ViewPanel::FinalizeProperties() {
  if (!m_propertiesChanged)
    return;

  // There are 3 different solutions
  //
  //  A) No Border
  //      -- No need for any border properties so just contain real children
  //      >> Background applied to ViewPanel
  //
  //  B) Inner Border (child of this panel along with real children)
  //      -- Border created and made a child of this panel, alongside standard
  //      children
  //      >> Border* properties applied to Border, Background applied to
  //      ViewPanel
  //
  //  C) Outer Border
  //      -- Border created but handed out to view manager to make parent of
  //      this ViewPanel
  //      >> Border* properties and Background applied to Border

  const auto unsetValue = xaml::DependencyProperty::UnsetValue();
  const Thickness emptyThickness = ThicknessHelper::FromUniformLength(0.0);

  bool hasBackground = ReadLocalValue(ViewBackgroundProperty()) != unsetValue;
  bool hasBorderBrush = ReadLocalValue(BorderBrushProperty()) != unsetValue;
  bool hasBorderThickness = BorderThickness() != emptyThickness;
  bool hasCornerRadius = ReadLocalValue(CornerRadiusProperty()) != unsetValue;
  bool displayBorder = hasBorderBrush && hasBorderThickness;

  // Determine which scenario our current properties have put us into
  enum class Scenario { OuterBorder, InnerBorder, NoBorder } scenario;
  if (hasCornerRadius) {
    scenario = Scenario::OuterBorder;
    m_hasOuterBorder = true;
  } else if (!displayBorder) {
    scenario = Scenario::NoBorder;
    m_hasOuterBorder = false;
  } else if (ClipChildren()) {
    scenario = Scenario::OuterBorder;
    m_hasOuterBorder = true;
  } else {
    scenario = Scenario::InnerBorder;
    m_hasOuterBorder = false;
  }

  // Detach this Panel from the outer Border if needed
  if (scenario != Scenario::OuterBorder && m_border != nullptr) {
    m_border.Child(nullptr);
  }

  // Border element
  if (scenario != Scenario::NoBorder) {
    // Ensure Border is created
    if (m_border == nullptr) {
      m_border = xaml::Controls::Border();

      // Add border as the top child if using as inner border
      if (scenario == Scenario::InnerBorder)
        Children().Append(m_border);
    }

    // TODO: Can Binding be used here?
    if (hasBorderBrush)
      m_border.BorderBrush(BorderBrush());
    else if (!hasBorderThickness) {
      m_border.ClearValue(xaml::Controls::Border::BorderBrushProperty());
    }

    if (hasBorderThickness) {
      m_border.BorderThickness(BorderThickness());
      if (!hasBorderBrush) {
        // Borders with no brush draw something other than transparent on other platforms.
        // To match, we'll use a default border brush if one isn't already set.
        // Note:  Keep this in sync with code in TryUpdateBorderProperties().
        m_border.BorderBrush(::react::uwp::DefaultBrushStore::Instance().GetDefaultBorderBrush());
      }
    } else
      m_border.ClearValue(xaml::Controls::Border::BorderThicknessProperty());

    if (hasCornerRadius)
      m_border.CornerRadius(CornerRadius());
    else
      m_border.ClearValue(xaml::Controls::Border::CornerRadiusProperty());
  } else if (m_border != nullptr) {
    // Clean up and remove the Border element
    m_border.Child(nullptr);
    Remove(m_border);
    m_border = nullptr;
  }

  if (scenario == Scenario::OuterBorder) {
    if (hasBackground)
      m_border.Background(ViewBackground());
    else
      m_border.ClearValue(xaml::Controls::Border::BackgroundProperty());

    ClearValue(winrt::Panel::BackgroundProperty());

    // Ensure the Border is not parented to this Panel
    Remove(m_border);
  } else {
    // Set any background on this Panel
    if (hasBackground)
      SetValue(winrt::Panel::BackgroundProperty(), ViewBackground());
    else
      ClearValue(winrt::Panel::BackgroundProperty());
  }

  m_propertiesChanged = false;
}

xaml::Controls::Border ViewPanel::GetOuterBorder() {
  if (m_hasOuterBorder && (m_border != nullptr))
    return m_border;
  else
    return xaml::Controls::Border(nullptr);
}

void ViewPanel::UpdateClip(winrt::Size &finalSize) {
  if (ClipChildren()) {
    winrt::RectangleGeometry clipGeometry;
    clipGeometry.Rect(winrt::Rect(0, 0, static_cast<float>(finalSize.Width), static_cast<float>(finalSize.Height)));

    Clip(clipGeometry);
  } else {
    Clip(nullptr);
  }
}

} // namespace winrt::PROJECT_ROOT_NAMESPACE::implementation
