
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompHelpers.h"

namespace Microsoft::ReactNative {

// TODO this obviously sucks
// TODO where should we store focus
static CompBaseComponentView *g_focusedComponent = nullptr;

CompBaseComponentView *GetFocusedComponent() noexcept {
  return g_focusedComponent;
}
void SetFocusedComponent(CompBaseComponentView *value) noexcept {
  g_focusedComponent = value;
}

facebook::react::Tag CompBaseComponentView::Tag() const noexcept {
  return m_tag;
}

void CompBaseComponentView::Tag(facebook::react::Tag tag) noexcept {
  m_tag = tag;
}

void CompBaseComponentView::parent(IComponentView *parent) noexcept {
  m_parent = parent;
}

IComponentView *CompBaseComponentView::parent() const noexcept {
  return m_parent;
}

void CompBaseComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
}

void CompBaseComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  assert(false); // Unhandled command
}

int64_t CompBaseComponentView::SendMessage(uint32_t msg, uint64_t wParam, int64_t lParam) noexcept {
  return 0;
}

RECT CompBaseComponentView::getClientRect() const noexcept {
  RECT rc{0};
  if (m_parent) {
    rc = m_parent->getClientRect();
  }

  rc.left += static_cast<LONG>(m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor);
  rc.top += static_cast<LONG>(m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor);
  rc.right = rc.left + static_cast<LONG>(m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor);
  rc.bottom = rc.left + static_cast<LONG>(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor);
  return rc;
}

const facebook::react::SharedViewEventEmitter &CompBaseComponentView::GetEventEmitter() const noexcept {
  return m_eventEmitter;
}

void CompBaseComponentView::CompContext(
    const std::shared_ptr<Microsoft::ReactNative::CompContext> &compContext) noexcept {
  m_compContext = compContext;
}

winrt::Windows::UI::Composition::Compositor CompBaseComponentView::Compositor() noexcept {
  return m_compContext->Compositor();
}

winrt::Windows::UI::Composition::CompositionGraphicsDevice CompBaseComponentView::CompositionGraphicsDevice() noexcept {
  return m_compContext->CompositionGraphicsDevice();
}

bool CompBaseComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  return false;
}

void CompBaseComponentView::ensureBorderVisual() noexcept {
  if (!m_borderVisual) {
    m_borderVisual = Compositor().CreateShapeVisual();
    m_borderGeometry = Compositor().CreateRoundedRectangleGeometry();
    m_borderShape = Compositor().CreateSpriteShape(m_borderGeometry);
    m_borderVisual.Shapes().Append(m_borderShape);

    Visual().Clip(Compositor().CreateGeometricClip(m_borderGeometry));

    auto containerChildren = Visual().as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    containerChildren.InsertAtBottom(m_borderVisual);
  }
}

void CompBaseComponentView::updateBorderProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (oldViewProps.borderColors != newViewProps.borderColors) {
    if (newViewProps.borderColors.all) {
      ensureBorderVisual();
      m_borderShape.StrokeBrush(Compositor().CreateColorBrush((*newViewProps.borderColors.all).AsWindowsColor()));
    } else {
      // TODO handle clearing border
    }
  }

  if (oldViewProps.borderRadii != newViewProps.borderRadii) {
    if (newViewProps.borderRadii.all) {
      ensureBorderVisual();
    }
  }
}

void CompBaseComponentView::updateBorderLayoutMetrics(const facebook::react::ViewProps &viewProps) noexcept {
  if (m_borderVisual) {
    m_borderGeometry.Size(
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor});
    m_borderVisual.Size(m_borderGeometry.Size());
    m_borderShape.StrokeThickness(
        m_layoutMetrics.borderWidth.left *
        m_layoutMetrics.pointScaleFactor); // Assume all border widths are the same for now

    if (viewProps.borderRadii.all) {
      // We have to update the corner radius on layout metric, because we need to clamp the radius to the width/2
      float radiusX, radiusY;
      radiusX = radiusY = *viewProps.borderRadii.all;
      if ((m_layoutMetrics.frame.size.width / 2) < radiusX) {
        radiusX = (m_layoutMetrics.frame.size.width / 2);
      }
      if ((m_layoutMetrics.frame.size.height / 2) < radiusY) {
        radiusY = (m_layoutMetrics.frame.size.height / 2);
      }

      m_borderGeometry.CornerRadius(
          {radiusX * m_layoutMetrics.pointScaleFactor, radiusY * m_layoutMetrics.pointScaleFactor});
    }
  }
}

void CompBaseComponentView::indexOffsetForBorder(uint32_t &index) const noexcept {
  if (m_borderVisual)
    index++;
}

CompViewComponentView::CompViewComponentView() {
  static auto const defaultProps = std::make_shared<facebook::react::ViewProps const>();
  m_props = defaultProps;
}

std::vector<facebook::react::ComponentDescriptorProvider>
CompViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompViewComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  ensureVisual();

  m_children.insert(std::next(m_children.begin(), index), &childComponentView);

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(this);

  auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
  if (index == 0 || containerChildren.Count() == 0) {
    containerChildren.InsertAtTop(static_cast<const CompBaseComponentView &>(childComponentView).Visual());
    return;
  }
  auto insertAfter = containerChildren.First();
  for (uint32_t i = 1; i < index; i++)
    insertAfter.MoveNext();
  containerChildren.InsertAbove(
      static_cast<const CompBaseComponentView &>(childComponentView).Visual(), insertAfter.Current());
}

void CompViewComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(nullptr);

  auto containerChildren = m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
  containerChildren.Remove(static_cast<const CompBaseComponentView &>(childComponentView).Visual());
}

void CompViewComponentView::ensureVisual() noexcept {
  if (!m_visual)
    m_visual = Compositor().CreateSpriteVisual();
}

void CompViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  ensureVisual();

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      auto brush = Compositor().CreateColorBrush((*newViewProps.backgroundColor).m_color);
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(brush);
    } else {
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(nullptr);
    }
  }

  if (oldViewProps.opacity != newViewProps.opacity) {
    m_visual.Opacity(newViewProps.opacity);
  }

  updateBorderProps(oldViewProps, newViewProps);

  // Shadow
  if (oldViewProps.shadowOffset != newViewProps.shadowOffset || oldViewProps.shadowColor != newViewProps.shadowColor ||
      oldViewProps.shadowOpacity != newViewProps.shadowOpacity ||
      oldViewProps.shadowRadius != newViewProps.shadowRadius) {
    winrt::Windows::UI::Composition::DropShadow shadow = Compositor().CreateDropShadow();
    shadow.Offset({newViewProps.shadowOffset.width, newViewProps.shadowOffset.height, 0});
    shadow.Opacity(newViewProps.shadowOpacity);
    shadow.BlurRadius(newViewProps.shadowRadius);
    if (newViewProps.shadowColor)
      shadow.Color(newViewProps.shadowColor.AsWindowsColor());
    m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Shadow(shadow);
  }

  // Transform - TODO doesn't handle multiple of the same kind of transform -- Doesn't handle hittesting updates
  if (oldViewProps.transform != newViewProps.transform) {
    for (const auto &operation : newViewProps.transform.operations) {
      if (operation.type == facebook::react::TransformOperationType::Scale)
        m_visual.Scale({operation.x, operation.y, operation.z});
      else if (operation.type == facebook::react::TransformOperationType::Rotate)
        m_visual.RotationAngle(operation.z);
    }
  }

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

facebook::react::Tag CompViewComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag tag;
  if (std::any_of(m_children.rbegin(), m_children.rend(), [&tag, &ptLocal, &localPt](auto child) {
        tag = static_cast<const CompBaseComponentView *>(child)->hitTest(ptLocal, localPt);
        return tag != -1;
      }))
    return tag;

  if (ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

bool CompViewComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag tag;
  if (std::any_of(m_children.rbegin(), m_children.rend(), [ptLocal, delta](auto child) {
        return const_cast<CompBaseComponentView *>(static_cast<const CompBaseComponentView *>(child))
            ->ScrollWheel(ptLocal, delta);
      }))
    return true;

  return false;
}

bool CompViewComponentView::shouldBeControl() const noexcept {
  // Fabric does not appear to have the focusable prop right now...
  return false; // m_props.focusable || HasDynamicAutomationProperties(view);
}

void CompViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void CompViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  updateBorderLayoutMetrics(*m_props);

  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
}

void CompViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  /*
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
  */
}

void CompViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps CompViewComponentView::props() noexcept {
  assert(false);
  return {};
}

// View is implemented with up to three elements, which get nested
// 1) ViewControl  - if focusable
// 2) Outer Border - if rounded corners or other clipping
// 3) ViewPanel
/*
const xaml::FrameworkElement ViewComponentView::Element() const noexcept {
  if (m_control)
    return m_control;
  if (m_outerBorder)
    return m_outerBorder;
  return m_panel;
}
*/

const winrt::Windows::UI::Composition::Visual CompViewComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
