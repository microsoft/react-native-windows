
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <Views/FrameworkElementTransferProperties.h>

namespace Microsoft::ReactNative {

void BaseComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
}

const facebook::react::SharedViewEventEmitter &BaseComponentView::GetEventEmitter() const noexcept {
  return m_eventEmitter;
}

ViewComponentView::ViewComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::ViewProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
ViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ViewComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_panel.Children().InsertAt(index, static_cast<const BaseComponentView &>(childComponentView).Element());
}

void ViewComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_panel.Children().RemoveAt(index);
}

void ViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    auto color = *newViewProps.backgroundColor;

    if (newViewProps.backgroundColor) {
      m_panel.ViewBackground(newViewProps.backgroundColor.AsWindowsBrush());
    } else {
      m_panel.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::ViewBackgroundProperty());
    }
  }

  if (oldViewProps.borderColors != newViewProps.borderColors) {
    if (newViewProps.borderColors.all) {
      m_panel.BorderBrush(newViewProps.borderColors.all->AsWindowsBrush());
    } else {
      m_panel.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::BorderBrushProperty());
    }
  }

  if (oldViewProps.opacity != newViewProps.opacity) {
    m_panel.Opacity(newViewProps.opacity);
  }

  if (oldViewProps.borderStyles != newViewProps.borderStyles || oldViewProps.borderRadii != newViewProps.borderRadii) {
    m_needsBorderUpdate = true;
  }

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

bool ViewComponentView::shouldBeControl() const noexcept {
  // Fabric does not appear to have the focusable prop right now...
  return false; // m_props.focusable || HasDynamicAutomationProperties(view);
}

void ViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}
void ViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties

  m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  winrt::Microsoft::ReactNative::ViewPanel::SetLeft(m_panel, layoutMetrics.frame.origin.x);
  winrt::Microsoft::ReactNative::ViewPanel::SetTop(m_panel, layoutMetrics.frame.origin.y);

  m_panel.Width(layoutMetrics.frame.size.width);
  m_panel.Height(layoutMetrics.frame.size.height);
}

void ViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  if (m_needsBorderUpdate) {
    auto const borderMetrics = m_props->resolveBorderMetrics(m_layoutMetrics);
    m_panel.BorderThickness(xaml::ThicknessHelper::FromLengths(
        borderMetrics.borderWidths.left,
        borderMetrics.borderWidths.top,
        borderMetrics.borderWidths.right,
        borderMetrics.borderWidths.bottom));

    xaml::CornerRadius cornerRadius;
    cornerRadius.BottomLeft = borderMetrics.borderRadii.bottomLeft;
    cornerRadius.BottomRight = borderMetrics.borderRadii.bottomRight;
    cornerRadius.TopLeft = borderMetrics.borderRadii.topLeft;
    cornerRadius.TopRight = borderMetrics.borderRadii.topRight;
    m_panel.CornerRadius(cornerRadius);

    m_needsBorderUpdate = false;
  }

  auto oldControl = m_control;
  auto oldOuterBorder = m_outerBorder;

  auto oldElement = Element();
  auto parent = oldElement.Parent();

  m_panel.FinalizeProperties();

  bool needsControl = shouldBeControl();
  if ((bool)m_control != needsControl || m_outerBorder != m_panel.GetOuterBorder()) {
    // -- Remove old children that are no longer needed
    if (needsControl && !m_control) {
      assert(false); // NYI
      // m_control = CreateViewControl();
    } else if (!needsControl && m_control) {
      m_control.Content(nullptr);
    }
    if (!needsControl) {
      m_control = nullptr;
    }

    if (m_panel.GetOuterBorder() && !m_outerBorder) {
      m_panel.GetOuterBorder().Child(nullptr);
    }

    m_outerBorder = m_panel.GetOuterBorder();

    auto newElement = Element();

    // -- Transfer properties to new element
    TransferFrameworkElementProperties(oldElement, newElement);
    m_panel.FinalizeProperties();

    // -- if the root element changes, we need to modify our parents child
    if (parent && oldElement != newElement) {
      // RefreshProperties(); - Transfer EnableFocusRing,TabIndex,IsFocusable
      auto parentPanel = parent.try_as<xaml::Controls::Panel>();
      if (parentPanel) {
        uint32_t index;
        auto found = parentPanel.Children().IndexOf(oldElement, index);
        assert(found);
        parentPanel.Children().SetAt(index, newElement);
      } else {
        assert(false); // TODO handle border/control parent
      }
    }

    // -- Ensure new heirarchy is setup properly
    if (m_outerBorder) {
      m_outerBorder.Child(m_panel);
    }

    if (m_control) {
      if (m_outerBorder)
        m_control.Content(m_outerBorder);
      else
        m_control.Content(m_panel);
    }
  }
}

void ViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps ViewComponentView::props() noexcept {
  assert(false);
  return {};
}

// View is implemented with up to three elements, which get nested
// 1) ViewControl  - if focusable
// 2) Outer Border - if rounded corners or other clipping
// 3) ViewPanel
const xaml::FrameworkElement ViewComponentView::Element() const noexcept {
  if (m_control)
    return m_control;
  if (m_outerBorder)
    return m_outerBorder;
  return m_panel;
}

} // namespace Microsoft::ReactNative
