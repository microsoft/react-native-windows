// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewPanel.h"

#include <winrt/Windows.UI.Xaml.Interop.h>
#include <winrt/Windows.Foundation.h>

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Interop;
using namespace Windows::UI::Xaml::Media;
using namespace Windows::Foundation;
}

namespace react { namespace uwp {

const winrt::TypeName viewPanelTypeName
{
  winrt::hstring { L"ViewPanel" },
  winrt::TypeKind::Metadata
};

ViewPanel::ViewPanel()
{
  LayoutUpdated([=](auto&&, auto &&args) { UpdateClip(); });
}

winrt::DependencyProperty ViewPanel::BackgroundProperty()
{
  static winrt::DependencyProperty s_backgroundProperty =
    winrt::DependencyProperty::Register(
      L"Background",
      winrt::xaml_typename<winrt::Brush>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(
        winrt::SolidColorBrush(),
        ViewPanel::VisualPropertyChanged)
  );

  return s_backgroundProperty;
}

void ViewPanel::VisualPropertyChanged(winrt::DependencyObject sender, winrt::DependencyPropertyChangedEventArgs e)
{
  auto panel { sender.as<ViewPanel>() };
  if (panel.get() != nullptr)
    panel->m_propertiesChanged = true;
}

winrt::DependencyProperty ViewPanel::BorderThicknessProperty()
{
  static winrt::DependencyProperty s_borderThicknessProperty =
    winrt::DependencyProperty::Register(
      L"BorderThickness",
      winrt::xaml_typename<winrt::Thickness>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(
        winrt::box_value(winrt::Thickness()),
        ViewPanel::VisualPropertyChanged)
  );

  return s_borderThicknessProperty;
}

winrt::DependencyProperty ViewPanel::BorderBrushProperty()
{
  static winrt::DependencyProperty s_borderBrushProperty =
    winrt::DependencyProperty::Register(
      L"BorderBrush",
      winrt::xaml_typename<winrt::Brush>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(
        winrt::SolidColorBrush(),
        ViewPanel::VisualPropertyChanged)
  );

  return s_borderBrushProperty;
}

winrt::DependencyProperty ViewPanel::CornerRadiusProperty()
{
  static winrt::DependencyProperty s_cornerRadiusProperty =
    winrt::DependencyProperty::Register(
      L"CornerRadius",
      winrt::xaml_typename<winrt::CornerRadius>(),
      viewPanelTypeName,
      winrt::PropertyMetadata(
        winrt::box_value(winrt::CornerRadius()),
        ViewPanel::VisualPropertyChanged)
  );

  return s_cornerRadiusProperty;
}

winrt::Size ViewPanel::MeasureOverride(winrt::Size availableSize)
{
  for (winrt::UIElement child : Children())
  {
    child.Measure(winrt::Size(INFINITY, INFINITY));

    auto width = child.DesiredSize().Width;
    auto height = child.DesiredSize().Height;
  }

  return winrt::Size(0.0f, 0.0f);
}

winrt::Size ViewPanel::ArrangeOverride(winrt::Size finalSize)
{
  for (winrt::UIElement child : Children())
  {
    float left = (float)winrt::Canvas::GetLeft(child);
    float top =  (float)winrt::Canvas::GetTop(child);

    auto width = child.DesiredSize().Width;
    auto height = child.DesiredSize().Height;

    if ((child != m_border) && (child != m_innerElement))
      child.Arrange(winrt::Rect(left, top, child.DesiredSize().Width, child.DesiredSize().Height));
    else
      child.Arrange(winrt::Rect(0.0f, 0.f, finalSize.Width, finalSize.Height));
  }

  return finalSize;
}

void ViewPanel::InsertAt(uint32_t const index, winrt::UIElement const& value) const
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

void ViewPanel::ClearValue(winrt::DependencyProperty dp) const
{
  // Catch any clearing of the Background property so we can clear it wherever we might have
  //  visualized it. All other properties receive the base implementation.
  if (dp == BackgroundProperty())
  {
    auto self = const_cast<ViewPanel*>(this);
    self->m_optBackgroundBrush.reset();
    self->m_propertiesChanged = true;
  }
  else
  {
    Super::ClearValue(dp);
  }
}

void ViewPanel::Background(winrt::Brush const& value)
{
  m_optBackgroundBrush = value;
  m_propertiesChanged = true;
}

void ViewPanel::BorderThickness(winrt::Thickness const& value)
{
  SetValue(BorderThicknessProperty(), winrt::box_value(value));
}

void ViewPanel::BorderBrush(winrt::Brush const& value)
{
  SetValue(BorderBrushProperty(), value);
}

void ViewPanel::CornerRadius(winrt::CornerRadius const& value)
{
  SetValue(CornerRadiusProperty(), winrt::box_value(value));
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
    ViewPanel* pInnerPanel = new ViewPanel();
    m_innerElement = pInnerPanel->try_as<winrt::UIElement>();

    // If there are any children other than our own border then we need to transfer them to the inner panel
    for (uint32_t i = Children().Size(); i > 0; --i)
    {
      auto child = Children().GetAt(i - 1);
      if (child != m_border)
      {
        Children().RemoveAt(i - 1);
        pInnerPanel->InsertAt(0, child);
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

  auto borderBrush = BorderBrush();
  auto borderThickness = BorderThickness();
  auto cornerRadius = CornerRadius();

  bool hasBackground = m_optBackgroundBrush.has_value();
  bool hasBorderThickness = borderThickness != winrt::Thickness();
  bool hasBorderBrush = borderBrush != winrt::SolidColorBrush();
  bool hasCornerRadius = cornerRadius != winrt::CornerRadius();

  bool needInnerPanel = hasCornerRadius && m_clipChildren;
  bool needBorder = needInnerPanel || hasCornerRadius || (hasBorderThickness && hasBorderBrush);

  // Border element
  if (needBorder)
  {
    // Ensure Border is created and set properties
    EnsureBorder();

    if (hasBorderBrush)
      m_border.BorderBrush(borderBrush);
    else
      m_border.ClearValue(winrt::Border::BorderBrushProperty());

    if (hasBorderThickness)
      m_border.BorderThickness(borderThickness);
    else
      m_border.ClearValue(winrt::Border::BorderThicknessProperty());

    if (hasCornerRadius)
      m_border.CornerRadius(cornerRadius);
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
      Super::Background(Background());
    else
      Super::ClearValue(winrt::Panel::BackgroundProperty());
  }

  m_propertiesChanged = false;
}

void ViewPanel::UpdateClip()
{
  // Clipping is only applied to the outer panel. If the inner Panel is used it implies that
  //  clipping will happen and be handled by the Border element.

  if (m_clipChildren)
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

void ViewPanel::ClipChildren(bool value)
{
  m_clipChildren = value;
  UpdateClip();
}

ViewPanel* ViewPanel::GetInnerPanel() const
{
  return (m_innerElement != nullptr)
    ? m_innerElement.try_as<ViewPanel>().get()
    : nullptr;
}

} }
