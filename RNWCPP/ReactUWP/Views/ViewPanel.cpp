// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewPanel.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>

namespace winrt
{
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
} // namespace winrt

namespace react
{
namespace uwp
{

const winrt::TypeName viewPanelTypeName{
    winrt::hstring{L"ViewPanel"},
    winrt::TypeKind::Metadata};

ViewPanel::ViewPanel()
{
  LayoutUpdated([=](auto &&, auto &&args) { UpdateClip(); });
}

/*static*/ winrt::com_ptr<ViewPanel> ViewPanel::Create()
{
  return winrt::make_self<ViewPanel>();
}

/*static*/ winrt::DependencyProperty ViewPanel::BackgroundProperty()
{
  static winrt::DependencyProperty s_backgroundProperty =
      winrt::DependencyProperty::Register(
          L"Background",
          winrt::xaml_typename<winrt::Brush>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::SolidColorBrush(),
              ViewPanel::VisualPropertyChanged));

  return s_backgroundProperty;
}

/*static*/ void ViewPanel::VisualPropertyChanged(winrt::DependencyObject sender, winrt::DependencyPropertyChangedEventArgs e)
{
  auto panel{sender.as<ViewPanel>()};
  if (panel.get() != nullptr)
    panel->m_propertiesChanged = true;
}

/*static*/ void ViewPanel::PositionPropertyChanged(winrt::DependencyObject sender, winrt::DependencyPropertyChangedEventArgs e)
{
  auto element{sender.as<winrt::UIElement>()};
  if (element != nullptr)
    element.InvalidateArrange();
}

/*static*/ winrt::DependencyProperty ViewPanel::BorderThicknessProperty()
{
  static winrt::DependencyProperty s_borderThicknessProperty =
      winrt::DependencyProperty::Register(
          L"BorderThickness",
          winrt::xaml_typename<winrt::Thickness>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(winrt::Thickness()),
              ViewPanel::VisualPropertyChanged));

  return s_borderThicknessProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::BorderBrushProperty()
{
  static winrt::DependencyProperty s_borderBrushProperty =
      winrt::DependencyProperty::Register(
          L"BorderBrush",
          winrt::xaml_typename<winrt::Brush>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::SolidColorBrush(),
              ViewPanel::VisualPropertyChanged));

  return s_borderBrushProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::CornerRadiusProperty()
{
  static winrt::DependencyProperty s_cornerRadiusProperty =
      winrt::DependencyProperty::Register(
          L"CornerRadius",
          winrt::xaml_typename<winrt::CornerRadius>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(winrt::CornerRadius()),
              ViewPanel::VisualPropertyChanged));

  return s_cornerRadiusProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::TopProperty()
{
  static winrt::DependencyProperty s_topProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"Top",
          winrt::xaml_typename<double>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value((double)0),
              ViewPanel::PositionPropertyChanged));

  return s_topProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::LeftProperty()
{
  static winrt::DependencyProperty s_topProperty =
      winrt::DependencyProperty::RegisterAttached(
          L"Left",
          winrt::xaml_typename<double>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value((double)0),
              ViewPanel::PositionPropertyChanged));

  return s_topProperty;
}

/*static*/ winrt::DependencyProperty ViewPanel::ClipChildrenProperty()
{
  static winrt::DependencyProperty s_clipChildrenProperty =
      winrt::DependencyProperty::Register(
          L"ClipChildren",
          winrt::xaml_typename<bool>(),
          viewPanelTypeName,
          winrt::PropertyMetadata(
              winrt::box_value(false),
              ViewPanel::VisualPropertyChanged));

  return s_clipChildrenProperty;
}

/*static*/ void ViewPanel::SetTop(winrt::Windows::UI::Xaml::UIElement &element, double value)
{
  element.SetValue(TopProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
}

/*static*/ void ViewPanel::SetLeft(winrt::Windows::UI::Xaml::UIElement &element, double value)
{
  element.SetValue(LeftProperty(), winrt::box_value<double>(value));
  element.InvalidateArrange();
}

winrt::Size ViewPanel::MeasureOverride(winrt::Size availableSize)
{
  // All children are given as much size as they'd like
  winrt::Size childConstraint(INFINITY, INFINITY);

  for (winrt::UIElement child : Children())
    child.Measure(childConstraint);

  // ViewPanels never choose their size, that is completely up to the parent - so return no size
  return winrt::Size(0, 0);
}

winrt::Size ViewPanel::ArrangeOverride(winrt::Size finalSize)
{
  for (winrt::UIElement child : Children())
  {
    double childHeight = 0.0;
    double childWidth = 0.0;

    // A Border or inner ViewPanel should take up the same space as this panel
    if ((child == m_border) || (child == m_innerElement))
    {
      childWidth = finalSize.Width;
      childHeight = finalSize.Height;
    }
    else
    {
      // We expect elements to have been arranged by yoga which means their Width & Height are set
      winrt::FrameworkElement fe = child.try_as<winrt::FrameworkElement>();
      if (fe != nullptr)
      {
        childWidth = fe.Width();
        childHeight = fe.Height();
      }
      // But we fall back to the measured size otherwise
      else
      {
        childWidth = child.DesiredSize().Width;
        childHeight = child.DesiredSize().Height;
      }
    }

    // Guard against negative values
    childWidth = std::max<double>(0.0f, childWidth);
    childHeight = std::max<double>(0.0f, childHeight);

    child.Arrange(winrt::Rect((float)ViewPanel::GetLeft(child), (float)ViewPanel::GetTop(child), (float)childWidth, (float)childHeight));
  }

  return finalSize;
}

void ViewPanel::InsertAt(uint32_t const index, winrt::UIElement const &value) const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
    innerPanel->InsertAt(index, value);
  else
    Children().InsertAt(index, value);
}

void ViewPanel::RemoveAt(uint32_t const index) const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
    innerPanel->RemoveAt(index);
  else
    Children().RemoveAt(index);
}

void ViewPanel::Remove(winrt::UIElement element) const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
  {
    innerPanel->Remove(element);
  }
  else
  {
    uint32_t index;

    if (Children().IndexOf(element, index))
      Children().RemoveAt(index);
  }
}

void ViewPanel::Clear() const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
    innerPanel->Clear();
  else
    Children().Clear();
}

winrt::UIElement ViewPanel::GetAt(uint32_t const index) const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
    return innerPanel->GetAt(index);
  else
    return Children().GetAt(index);
}

uint32_t ViewPanel::Size() const
{
  auto innerPanel = GetInnerPanel();
  if (innerPanel != nullptr)
  {
    return innerPanel->Size();
  }
  else
  {
    auto size = Children().Size();

    if (m_border != nullptr)
      size -= 1;

    return size;
  }
}

void ViewPanel::Background(winrt::Brush const &value)
{
  SetValue(BackgroundProperty(), winrt::box_value(value));
}

void ViewPanel::BorderThickness(winrt::Thickness const &value)
{
  SetValue(BorderThicknessProperty(), winrt::box_value(value));
}

void ViewPanel::BorderBrush(winrt::Brush const &value)
{
  SetValue(BorderBrushProperty(), value);
}

void ViewPanel::CornerRadius(winrt::CornerRadius const &value)
{
  SetValue(CornerRadiusProperty(), winrt::box_value(value));
}

void ViewPanel::ClipChildren(bool value)
{
  SetValue(ClipChildrenProperty(), winrt::box_value(value));
}

void ViewPanel::EnsureBorder()
{
  if (m_border == nullptr)
  {
    m_border = winrt::Border();
    Children().Append(m_border);
  }
}

void ViewPanel::EnsureInnerPanel()
{
  // ASSERT: Need to have a Border to put the ViewPanel into
  assert(m_border != nullptr);

  if (m_innerElement == nullptr)
  {
    auto innerPanel = ViewPanel::Create();
    m_innerElement = innerPanel.try_as<winrt::UIElement>();

    // If there are any children other than our own border then we need to transfer them to the inner panel
    for (uint32_t i = Children().Size(); i > 0; --i)
    {
      auto child = Children().GetAt(i - 1);
      if (child != m_border)
      {
        Children().RemoveAt(i - 1);
        innerPanel->InsertAt(0, child);
      }
    }

    m_border.Child(m_innerElement);
  }
}

void ViewPanel::FinalizeProperties()
{
  if (!m_propertiesChanged)
    return;

  // There are 3 different solutions
  //
  //  A) Have Border with Inner Panel child that contains real children
  //      >> border and background properties go on the Border
  //      ** This solution only used when there is a corner radius
  //  B) Have Border with no child and this Panel contains the Border and the real children
  //      >> border properties go on Border, background goes on this Panel
  //      ** This solution used when there are border properties but no corner radius
  //  C) No Border and real children contained by this Panel
  //      >> background goes on this Panel
  //      ** This solution used when there are no border properties or corner radius
  //
  // What we need to know:
  //  1. Do we need an inner panel? Only if we have a corner radius and need clipping.
  //  2. Do we need a Border? NOTE: Includes any time we need an inner panel!
  //  3. Where do we apply the background? (implied by #1)

  const auto unsetValue = winrt::DependencyProperty::UnsetValue();

  bool hasBackground = ReadLocalValue(BackgroundProperty()) != unsetValue;
  bool hasBorderBrush = ReadLocalValue(BorderBrushProperty()) != unsetValue;
  bool hasBorderThickness = ReadLocalValue(BorderThicknessProperty()) != unsetValue;
  bool hasCornerRadius = ReadLocalValue(CornerRadiusProperty()) != unsetValue;
  bool displayBorder = hasBorderBrush && hasBorderThickness;

  bool needInnerPanel = hasCornerRadius && (ClipChildren() || !displayBorder);
  bool needBorder = needInnerPanel || hasCornerRadius || (hasBorderThickness && hasBorderBrush);

  // Border element
  if (needBorder)
  {
    // Ensure Border is created and set properties
    EnsureBorder();

    // TODO: Can Binding be used here?
    if (hasBorderBrush)
      m_border.BorderBrush(BorderBrush());
    else
      m_border.ClearValue(winrt::Border::BorderBrushProperty());

    if (hasBorderThickness)
      m_border.BorderThickness(BorderThickness());
    else
      m_border.ClearValue(winrt::Border::BorderThicknessProperty());

    if (hasCornerRadius)
      m_border.CornerRadius(CornerRadius());
    else
      m_border.ClearValue(winrt::Border::CornerRadiusProperty());
  }
  else if (m_border != nullptr)
  {
    // Remove the Border element
    Remove(m_border);
    m_border = nullptr;
  }

  // Inner ViewPanel element
  if (needInnerPanel)
  {
    EnsureInnerPanel();
  }
  else if (m_innerElement != nullptr)
  {
    Remove(m_innerElement);
    m_innerElement = nullptr;
  }

  // Background property: On Border if there's an inner Panel, on this Panel otherwise
  if (needInnerPanel)
  {
    // Remove any background set on this Panel
    Super::ClearValue(winrt::Panel::BackgroundProperty());

    // Set any background on the Border
    if (hasBackground)
      m_border.Background(Background());
    else
      m_border.ClearValue(winrt::Border::BackgroundProperty());
  }
  else
  {
    // Remove any background set on the Border and set it on this Panel
    if (m_border != nullptr)
      m_border.ClearValue(winrt::Border::BackgroundProperty());

    // Set any background on this Panel
    if (hasBackground)
      SetValue(winrt::Panel::BackgroundProperty(), Background());
    else
      ClearValue(winrt::Panel::BackgroundProperty());
  }

  UpdateClip();

  m_propertiesChanged = false;
}

void ViewPanel::UpdateClip()
{
  // Clipping is only applied to the outer panel. If the inner Panel is used it implies that
  //  clipping will happen and be handled by the Border element.

  if (ClipChildren())
  {
    winrt::RectangleGeometry clipGeometry;
    clipGeometry.Rect(winrt::Rect(0, 0, static_cast<float>(ActualWidth()), static_cast<float>(ActualHeight())));

    Clip(clipGeometry);
  }
  else
  {
    Clip(nullptr);
  }
}

ViewPanel *ViewPanel::GetInnerPanel() const
{
  return (m_innerElement != nullptr)
             ? m_innerElement.try_as<ViewPanel>().get()
             : nullptr;
}

} // namespace uwp
} // namespace react
