
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

CompBaseComponentView::CompBaseComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : m_tag(tag), m_compContext(compContext) {}

facebook::react::Tag CompBaseComponentView::Tag() const noexcept {
  return m_tag;
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

bool CompBaseComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  return false;
}

void CompBaseComponentView::ensureBorderVisual() noexcept {
  // TODO borders using Switcher (and more complete implementation in general)
  /*
  if (!m_borderVisual) {
    m_borderVisual = Compositor().CreateShapeVisual();
    m_borderGeometry = Compositor().CreateRoundedRectangleGeometry();
    m_borderShape = Compositor().CreateSpriteShape(m_borderGeometry);
    m_borderVisual.Shapes().Append(m_borderShape);

    Visual().Clip(Compositor().CreateGeometricClip(m_borderGeometry));

    auto containerChildren = Visual().as<winrt::Windows::UI::Composition::ContainerVisual>().Children();
    containerChildren.InsertAtBottom(m_borderVisual);
  }
  */
}

void CompBaseComponentView::updateBorderProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  // TODO borders using Switcher (and more complete implementation in general)
  /*
if (oldViewProps.borderColors != newViewProps.borderColors) {
if (newViewProps.borderColors.all) {
  ensureBorderVisual();
  m_borderShape.StrokeBrush(Compositor().CreateColorBrush((*newViewProps.borderColors.all).AsWindowsColor()));
} else {
  // TODO handle clearing border
}
}
*/

  if (oldViewProps.borderRadii != newViewProps.borderRadii) {
    if (newViewProps.borderRadii.all) {
      ensureBorderVisual();
    }
  }
}

void CompBaseComponentView::updateBorderLayoutMetrics(const facebook::react::ViewProps &viewProps) noexcept {
  // TODO borders using Switcher (and more complete implementation in general)
  /*
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
  */
}

void CompBaseComponentView::indexOffsetForBorder(uint32_t &index) const noexcept {
  // TODO borders using Switcher (and more complete implementation in general)
  /*
if (m_borderVisual)
index++;
*/
}

void CompBaseComponentView::OnRenderingDeviceLost() noexcept {}

CompViewComponentView::CompViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ViewProps const>();
  m_props = defaultProps;
  m_visual = m_compContext.CreateSpriteVisual();
}

std::vector<facebook::react::ComponentDescriptorProvider>
CompViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompViewComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  m_children.insert(std::next(m_children.begin(), index), &childComponentView);

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(this);

  m_visual.InsertAt(static_cast<const CompBaseComponentView &>(childComponentView).Visual(), index);
}

void CompViewComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  indexOffsetForBorder(index);

  const_cast<IComponentView &>(childComponentView).parent(nullptr);
  m_visual.Remove(static_cast<const CompBaseComponentView &>(childComponentView).Visual());
}

void CompViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      m_visual.Brush(m_compContext.CreateColorBrush((*newViewProps.backgroundColor).m_color));
    } else {
      m_visual.Brush(nullptr);
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
    auto shadow = m_compContext.CreateDropShadow();
    shadow.Offset({newViewProps.shadowOffset.width, newViewProps.shadowOffset.height, 0});
    shadow.Opacity(newViewProps.shadowOpacity);
    shadow.BlurRadius(newViewProps.shadowRadius);
    if (newViewProps.shadowColor)
      shadow.Color(newViewProps.shadowColor.AsWindowsColor());
    m_visual.Shadow(shadow);
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

void CompViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {}

void CompViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

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

void CompViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {}

void CompViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps CompViewComponentView::props() noexcept {
  assert(false);
  return {};
}

winrt::Microsoft::ReactNative::Composition::IVisual CompViewComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
