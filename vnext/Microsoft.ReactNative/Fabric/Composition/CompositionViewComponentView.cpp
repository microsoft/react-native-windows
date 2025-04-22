
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompositionViewComponentView.h"

#include <AutoDraw.h>
#include <Fabric/AbiState.h>
#include <Fabric/AbiViewProps.h>
#include <Fabric/Composition/ReactNativeIsland.h>
#include <Fabric/FabricUIManagerModule.h>
#include <UI.Xaml.Controls.h>
#include <Utils/KeyboardUtils.h>
#include <Utils/ValueUtils.h>
#include <Views/FrameworkElementTransferProperties.h>
#include <atlcomcli.h>
#include <winrt/Microsoft.ReactNative.Composition.Experimental.h>
#include <winrt/Microsoft.UI.Input.h>
#include <winrt/Windows.UI.Composition.h>
#include "CompositionContextHelper.h"
#include "CompositionDynamicAutomationProvider.h"
#include "CompositionHelpers.h"
#include "RootComponentView.h"
#include "Theme.h"
#include "TooltipService.h"
#include "UiaHelpers.h"
#include "d2d1helper.h"

#include "Composition.ComponentView.g.cpp"
#include "Composition.ViewComponentView.g.cpp"

namespace winrt::Microsoft::ReactNative::Composition::implementation {

constexpr float FOCUS_VISUAL_WIDTH = 2.0f;
constexpr float FOCUS_VISUAL_RADIUS = 3.0f;

// m_outerVisual
//   |
//   |
//   ----- m_visual <-- Background / clip - Can be a custom visual depending on Component type
//            |
//            ----- Border Visuals x N (BorderPrimitive attached to m_visual)
//   ------Focus Visual Container (created when hosting focus visuals)
//           |
//           |------Inner Focus Visual
//                     |
//                     ------ Border Visuals x N (BorderPrimitive)
//           |------Outer Focus Visual
//                     |
//                     ------ Border Visuals x N (BorderPrimitive)

ComponentView::ComponentView(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ComponentViewFeatures flags,
    winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder)
    : base_type(tag, reactContext, builder), m_compContext(compContext), m_flags(flags) {
  m_outerVisual = compContext.CreateSpriteVisual(); // TODO could be a raw ContainerVisual if we had a
                                                    // CreateContainerVisual in ICompositionContext
}

ComponentView::~ComponentView() {
  if (m_tooltipTracked) {
    TooltipService::GetCurrent(m_reactContext.Properties())->StopTracking(*this);
    m_tooltipTracked = false;
  }
}

facebook::react::Tag ComponentView::Tag() const noexcept {
  return m_tag;
}

facebook::react::Props::Shared ComponentView::props() noexcept {
  return viewProps();
}

void ComponentView::onThemeChanged() noexcept {
  if ((m_flags & ComponentViewFeatures::Background) == ComponentViewFeatures::Background) {
    if (viewProps()->backgroundColor) {
      Visual().as<Experimental::ISpriteVisual>().Brush(theme()->Brush(*viewProps()->backgroundColor));
    } else {
      Visual().as<Experimental::ISpriteVisual>().Brush(nullptr);
    }
  }

  if (m_borderPrimitive) {
    m_borderPrimitive->onThemeChanged(
        m_layoutMetrics, BorderPrimitive::resolveAndAlignBorderMetrics(m_layoutMetrics, *viewProps()));
  }
  if (m_componentHostingFocusVisual) {
    if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive) {
      auto innerFocusMetrics = focusLayoutMetrics(true /*inner*/);
      m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive->onThemeChanged(
          innerFocusMetrics, focusBorderMetrics(true /*inner*/, innerFocusMetrics));
    }
    if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive) {
      auto outerFocusMetrics = focusLayoutMetrics(true /*inner*/);
      m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive->onThemeChanged(
          outerFocusMetrics, focusBorderMetrics(false /*inner*/, outerFocusMetrics));
    }
  }

  if ((m_flags & ComponentViewFeatures::ShadowProps) == ComponentViewFeatures::ShadowProps) {
    applyShadowProps(*viewProps());
  }

  base_type::onThemeChanged();

  if (m_themeChangedEvent) {
    m_themeChangedEvent(*this, *this);
  }
}

winrt::event_token ComponentView::ThemeChanged(
    winrt::Windows::Foundation::EventHandler<winrt::IInspectable> const &handler) noexcept {
  return m_themeChangedEvent.add(handler);
}
void ComponentView::ThemeChanged(winrt::event_token const &token) noexcept {
  m_themeChangedEvent.remove(token);
}

void ComponentView::Theme(const winrt::Microsoft::ReactNative::Composition::Theme &value) noexcept {
  theme(winrt::get_self<winrt::Microsoft::ReactNative::Composition::implementation::Theme>(value));
}

winrt::Microsoft::ReactNative::Composition::Theme ComponentView::Theme() const noexcept {
  return theme()->get_strong().as<winrt::Microsoft::ReactNative::Composition::Theme>();
}

winrt::Microsoft::ReactNative::Composition::RootComponentView ComponentView::Root() noexcept {
  return *rootComponentView();
}

winrt::Microsoft::UI::Composition::Compositor ComponentView::Compositor() const noexcept {
  return winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::InnerCompositor(
      m_compContext);
}

winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext ComponentView::CompositionContext()
    const noexcept {
  return m_compContext;
}

void ComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps =
      oldProps ? (*std::static_pointer_cast<const facebook::react::ViewProps>(oldProps)) : (*viewProps());
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  if ((m_flags & ComponentViewFeatures::Background) == ComponentViewFeatures::Background) {
    if (oldViewProps.backgroundColor != newViewProps.backgroundColor) {
      if (newViewProps.backgroundColor) {
        Visual().as<Experimental::ISpriteVisual>().Brush(theme()->Brush(*newViewProps.backgroundColor));
      } else {
        Visual().as<Experimental::ISpriteVisual>().Brush(nullptr);
      }
    }
  }

  if (m_borderPrimitive) {
    m_borderPrimitive->updateProps(oldViewProps, newViewProps);
  }

  if (m_componentHostingFocusVisual) {
    if (!newViewProps.enableFocusRing) {
      m_componentHostingFocusVisual->hostFocusVisual(false, get_strong());
    }

    // We have to check m_componentHostingFocusVisual again, as it can be set to null by above hostFocusVisual call
    if (m_componentHostingFocusVisual) {
      if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive) {
        m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive->updateProps(oldViewProps, newViewProps);
      }
      if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive) {
        m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive->updateProps(oldViewProps, newViewProps);
      }
    }
  }
  if ((m_flags & ComponentViewFeatures::ShadowProps) == ComponentViewFeatures::ShadowProps) {
    updateShadowProps(oldViewProps, newViewProps);
  }
  if (oldViewProps.tooltip != newViewProps.tooltip) {
    if (!m_tooltipTracked && newViewProps.tooltip && !newViewProps.tooltip->empty()) {
      TooltipService::GetCurrent(m_reactContext.Properties())->StartTracking(*this);
      m_tooltipTracked = true;
    } else if (m_tooltipTracked && !newViewProps.tooltip) {
      TooltipService::GetCurrent(m_reactContext.Properties())->StopTracking(*this);
      m_tooltipTracked = false;
    }
  }

  base_type::updateProps(props, oldProps);
}

void ComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    updateClippingPath(layoutMetrics, *viewProps());
    OuterVisual().Size(
        {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
         layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
    OuterVisual().Offset({
        layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
        layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
        0.0f,
    });
  }

  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);

  if (layoutMetrics != oldLayoutMetrics) {
    if (m_borderPrimitive) {
      m_borderPrimitive->markNeedsUpdate();
    }

    if (m_componentHostingFocusVisual) {
      m_componentHostingFocusVisual->updateFocusLayoutMetrics();
    }
  }

  UpdateCenterPropertySet();
}

void ComponentView::updateFocusLayoutMetrics() noexcept {
  facebook::react::RectangleEdges<bool> nudgeEdges;
  auto scaleFactor = m_focusPrimitive->m_focusVisualComponent->m_layoutMetrics.pointScaleFactor;
  if (m_focusPrimitive) {
    auto nudgeEdges = m_focusPrimitive->m_focusVisualComponent->focusNudges();
    if (m_focusPrimitive->m_focusOuterPrimitive) {
      auto outerFocusMetrics = m_focusPrimitive->m_focusVisualComponent->focusLayoutMetrics(false /*inner*/);
      m_focusPrimitive->m_focusOuterPrimitive->RootVisual().Size(
          {outerFocusMetrics.frame.size.width * scaleFactor, outerFocusMetrics.frame.size.height * scaleFactor});
      m_focusPrimitive->m_focusOuterPrimitive->RootVisual().Offset(
          {nudgeEdges.left ? 0 : -(FOCUS_VISUAL_WIDTH * 2 * scaleFactor),
           nudgeEdges.top ? 0 : -(FOCUS_VISUAL_WIDTH * 2 * scaleFactor),
           0.0f});
      m_focusPrimitive->m_focusOuterPrimitive->markNeedsUpdate();
    }

    if (m_focusPrimitive->m_focusInnerPrimitive) {
      auto innerFocusMetrics = m_focusPrimitive->m_focusVisualComponent->focusLayoutMetrics(true /*inner*/);
      m_focusPrimitive->m_focusInnerPrimitive->RootVisual().Size(
          {innerFocusMetrics.frame.size.width * scaleFactor, innerFocusMetrics.frame.size.height * scaleFactor});
      m_focusPrimitive->m_focusInnerPrimitive->RootVisual().Offset(
          {nudgeEdges.left ? (FOCUS_VISUAL_WIDTH * scaleFactor) : (-FOCUS_VISUAL_WIDTH * scaleFactor),
           nudgeEdges.top ? (FOCUS_VISUAL_WIDTH * scaleFactor) : (-FOCUS_VISUAL_WIDTH * scaleFactor),
           0.0f});
      m_focusPrimitive->m_focusInnerPrimitive->markNeedsUpdate();
    }
  }
}

const facebook::react::LayoutMetrics &ComponentView::layoutMetrics() const noexcept {
  return m_layoutMetrics;
}

void ComponentView::FinalizeTransform(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  const auto resolveTransformMatrix = viewProps.resolveTransform(layoutMetrics);
  winrt::Windows::Foundation::Numerics::float4x4 transformMatrix;
  transformMatrix.m11 = resolveTransformMatrix.matrix[0];
  transformMatrix.m12 = resolveTransformMatrix.matrix[1];
  transformMatrix.m13 = resolveTransformMatrix.matrix[2];
  transformMatrix.m14 = resolveTransformMatrix.matrix[3];
  transformMatrix.m21 = resolveTransformMatrix.matrix[4];
  transformMatrix.m22 = resolveTransformMatrix.matrix[5];
  transformMatrix.m23 = resolveTransformMatrix.matrix[6];
  transformMatrix.m24 = resolveTransformMatrix.matrix[7];
  transformMatrix.m31 = resolveTransformMatrix.matrix[8];
  transformMatrix.m32 = resolveTransformMatrix.matrix[9];
  transformMatrix.m33 = resolveTransformMatrix.matrix[10];
  transformMatrix.m34 = resolveTransformMatrix.matrix[11];
  transformMatrix.m41 = resolveTransformMatrix.matrix[12];
  transformMatrix.m42 = resolveTransformMatrix.matrix[13];
  transformMatrix.m43 = resolveTransformMatrix.matrix[14];
  transformMatrix.m44 = resolveTransformMatrix.matrix[15];

  auto centerPointPropSet = EnsureCenterPointPropertySet();
  if (centerPointPropSet) {
    centerPointPropSet.InsertMatrix4x4(L"transform", transformMatrix);
  }

  EnsureTransformMatrixFacade();
  m_FinalizeTransform = false;
}

void ComponentView::FinalizeUpdates(winrt::Microsoft::ReactNative::ComponentViewUpdateMask updateMask) noexcept {
  if ((m_flags & ComponentViewFeatures::NativeBorder) == ComponentViewFeatures::NativeBorder) {
    auto borderMetrics = BorderPrimitive::resolveAndAlignBorderMetrics(m_layoutMetrics, *viewProps());
    if (!m_borderPrimitive && BorderPrimitive::requiresBorder(borderMetrics, theme())) {
      m_borderPrimitive = std::make_shared<BorderPrimitive>(*this, Visual());
    }

    if (m_borderPrimitive) {
      m_borderPrimitive->finalize(m_layoutMetrics, borderMetrics);
    }
  }

  if (m_componentHostingFocusVisual) {
    if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive) {
      auto innerFocusMetrics = focusLayoutMetrics(true /*inner*/);
      m_componentHostingFocusVisual->m_focusPrimitive->m_focusInnerPrimitive->finalize(
          innerFocusMetrics, focusBorderMetrics(true /*inner*/, innerFocusMetrics));
    }
    if (m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive) {
      auto outerFocusMetrics = focusLayoutMetrics(false /*inner*/);
      m_componentHostingFocusVisual->m_focusPrimitive->m_focusOuterPrimitive->finalize(
          outerFocusMetrics, focusBorderMetrics(false /*inner*/, outerFocusMetrics));
    }
  }

  if (m_FinalizeTransform) {
    FinalizeTransform(m_layoutMetrics, *viewProps());
  }

  base_type::FinalizeUpdates(updateMask);
}

void ComponentView::onLostFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  if (args.OriginalSource() == Tag()) {
    m_eventEmitter->onBlur();

    if (m_componentHostingFocusVisual) {
      auto s = get_strong();

      m_componentHostingFocusVisual->hostFocusVisual(false, get_strong());
    }
    if (UiaClientsAreListening()) {
      winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
          EnsureUiaProvider(), UIA_HasKeyboardFocusPropertyId, true, false);
    }
  }
  base_type::onLostFocus(args);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ComponentView::visualToHostFocus() noexcept {
  return OuterVisual();
}

// We want to host focus visuals as close to the focused component as possible.  However since the focus visuals extend
// past the bounds of the component, in cases where additional components are positioned directly next to this one, we'd
// get zorder issues causing most of the focus rect to be obscured. So we go up the tree until we find a component who's
// bounds will fix the entire focus rect.
winrt::com_ptr<ComponentView> ComponentView::focusVisualRoot(const facebook::react::Rect &focusRect) noexcept {
  auto compVisual =
      winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(OuterVisual());
  if (!compVisual) {
    return get_strong();
    // When not using lifted composition, force the focus visual to host within its own component, as we do not support
    // ParentForTransform
  }

  if (facebook::react::Rect::intersect(focusRect, m_layoutMetrics.frame) == focusRect) {
    return get_strong();
  }

  if (!m_parent) {
    return get_strong();
  }

  return m_parent.as<ComponentView>()->focusVisualRoot(
      {{focusRect.origin.x + m_layoutMetrics.frame.origin.x, focusRect.origin.y + m_layoutMetrics.frame.origin.y},
       focusRect.size});
}

void ComponentView::onGotFocus(
    const winrt::Microsoft::ReactNative::Composition::Input::RoutedEventArgs &args) noexcept {
  if (args.OriginalSource() == Tag()) {
    m_eventEmitter->onFocus();
    if (viewProps()->enableFocusRing) {
      facebook::react::Rect focusRect = m_layoutMetrics.frame;
      focusRect.origin.x -= (FOCUS_VISUAL_WIDTH * 2);
      focusRect.origin.y -= (FOCUS_VISUAL_WIDTH * 2);
      focusRect.size.width += (FOCUS_VISUAL_WIDTH * 2);
      focusRect.size.height += (FOCUS_VISUAL_WIDTH * 2);
      focusVisualRoot(focusRect)->hostFocusVisual(true, get_strong());
    }
    if (UiaClientsAreListening()) {
      auto spProviderSimple = EnsureUiaProvider().try_as<IRawElementProviderSimple>();
      if (spProviderSimple != nullptr) {
        winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
            m_uiaProvider, UIA_HasKeyboardFocusPropertyId, false, true);
        UiaRaiseAutomationEvent(spProviderSimple.get(), UIA_AutomationFocusChangedEventId);
      }
    }

    StartBringIntoView({});
  }
  base_type::onGotFocus(args);
}

void ComponentView::StartBringIntoView(
    winrt::Microsoft::ReactNative::implementation::BringIntoViewOptions &&options) noexcept {
  if (!options.TargetRect) {
    // Default to bring the entire of this component into view
    options.TargetRect = {
        {0, 0},
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor}};
  }

  if (m_parent) {
    options.TargetRect->origin.y += m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor;
    options.TargetRect->origin.x += m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor;
    winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->StartBringIntoView(
        std::move(options));
  }
}

void ComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {
  m_eventEmitter = std::static_pointer_cast<facebook::react::ViewEventEmitter const>(eventEmitter);
  base_type::updateEventEmitter(eventEmitter);
}

void ComponentView::HandleCommand(const winrt::Microsoft::ReactNative::HandleCommandArgs &args) noexcept {
  base_type::HandleCommand(args);
  if (args.Handled())
    return;

  auto commandName = args.CommandName();
  if (commandName == L"focus") {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(*get_strong(), winrt::Microsoft::ReactNative::FocusNavigationDirection::None);
    }
    return;
  }
  if (commandName == L"blur") {
    if (auto root = rootComponentView()) {
      root->TrySetFocusedComponent(
          nullptr, winrt::Microsoft::ReactNative::FocusNavigationDirection::None); // Todo store this component as
                                                                                   // previously focused element
    }
    return;
  }
}

bool ComponentView::CapturePointer(const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept {
  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return false;

  auto root = rootComponentView();
  if (!root)
    return false;

  auto rootView{root->ReactNativeIsland()};
  if (!rootView) {
    return false;
  }

  return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)->CapturePointer(
      pointer, static_cast<facebook::react::Tag>(Tag()));
}

void ComponentView::ReleasePointerCapture(
    const winrt::Microsoft::ReactNative::Composition::Input::Pointer &pointer) noexcept {
  auto uiManager = ::Microsoft::ReactNative::FabricUIManager::FromProperties(m_reactContext.Properties());
  if (uiManager == nullptr)
    return;

  auto root = rootComponentView();
  if (!root)
    return;

  auto rootView{root->ReactNativeIsland()};
  if (!rootView) {
    return;
  }

  return winrt::get_self<winrt::Microsoft::ReactNative::implementation::ReactNativeIsland>(rootView)
      ->ReleasePointerCapture(pointer, static_cast<facebook::react::Tag>(Tag()));
}

void ComponentView::SetViewFeatures(ComponentViewFeatures viewFeatures) noexcept {
  m_flags = viewFeatures;
}

RECT ComponentView::getClientRect() const noexcept {
  RECT rc{0};
  facebook::react::Point parentOffset{0};
  if (m_parent) {
    parentOffset =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientOffset();
  }

  rc.left = static_cast<LONG>((m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor) + parentOffset.x);
  rc.top += static_cast<LONG>((m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor) + parentOffset.y);
  rc.right = rc.left + static_cast<LONG>(m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor);
  rc.bottom = rc.top + static_cast<LONG>(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor);
  return rc;
}

facebook::react::Point ComponentView::getClientOffset() const noexcept {
  facebook::react::Point parentOffset{0};
  if (m_parent) {
    parentOffset =
        winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientOffset();
  }

  return {
      (m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor) + parentOffset.x,
      (m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor) + parentOffset.y};
}

const facebook::react::SharedViewEventEmitter &ComponentView::GetEventEmitter() const noexcept {
  return m_eventEmitter;
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ComponentView::OuterVisual() const noexcept {
  return m_outerVisual ? m_outerVisual : Visual();
}

// If the focus visual would extend past the bounds of the hosting visual,
// then we will nudge the focus visual back inside the hosting visuals bounds.
facebook::react::RectangleEdges<bool> ComponentView::focusNudges() const noexcept {
  facebook::react::RectangleEdges<bool> nudgeEdges;

  // Always use outer focus metrics to determine if we need to nudge the focus rect over to fit
  facebook::react::LayoutMetrics layoutMetrics = focusLayoutMetricsNoNudge(false /*inner*/);

  Assert(m_componentHostingFocusVisual);

  if (layoutMetrics.frame.origin.x < 0) {
    nudgeEdges.left = true;
  }
  if (layoutMetrics.frame.origin.y < 0) {
    nudgeEdges.top = true;
  }
  if (layoutMetrics.frame.getMaxX() > m_componentHostingFocusVisual->m_layoutMetrics.frame.getMaxX()) {
    nudgeEdges.right = true;
  }
  if (layoutMetrics.frame.getMaxY() > m_componentHostingFocusVisual->m_layoutMetrics.frame.getMaxY()) {
    nudgeEdges.bottom = true;
  }

  return nudgeEdges;
}

facebook::react::LayoutMetrics ComponentView::focusLayoutMetricsNoNudge(bool inner) const noexcept {
  facebook::react::LayoutMetrics layoutMetrics = m_layoutMetrics;
  layoutMetrics.frame.origin.x -= FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  layoutMetrics.frame.origin.y -= FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  layoutMetrics.frame.size.height += FOCUS_VISUAL_WIDTH * (inner ? 2 : 4);
  layoutMetrics.frame.size.width += FOCUS_VISUAL_WIDTH * (inner ? 2 : 4);
  return layoutMetrics;
}

facebook::react::LayoutMetrics ComponentView::focusLayoutMetrics(bool inner) const noexcept {
  auto nudgeEdges = focusNudges();
  auto layoutMetrics = focusLayoutMetricsNoNudge(inner);

  if (nudgeEdges.left) {
    layoutMetrics.frame.origin.x += FOCUS_VISUAL_WIDTH * 2;
    layoutMetrics.frame.size.width -= FOCUS_VISUAL_WIDTH * 2;
  }
  if (nudgeEdges.top) {
    layoutMetrics.frame.origin.y += FOCUS_VISUAL_WIDTH * 2;
    layoutMetrics.frame.size.height -= FOCUS_VISUAL_WIDTH * 2;
  }
  if (nudgeEdges.right) {
    layoutMetrics.frame.size.width -= FOCUS_VISUAL_WIDTH * 2;
  }
  if (nudgeEdges.bottom) {
    layoutMetrics.frame.size.height -= FOCUS_VISUAL_WIDTH * 2;
  }

  return layoutMetrics;
}

facebook::react::BorderMetrics ComponentView::focusBorderMetrics(
    bool inner,
    const facebook::react::LayoutMetrics &layoutMetrics) const noexcept {
  facebook::react::BorderMetrics metrics = BorderPrimitive::resolveAndAlignBorderMetrics(layoutMetrics, *viewProps());
  facebook::react::Color innerColor;
  innerColor.m_color = {1, 0, 0, 0};
  innerColor.m_platformColor.push_back(inner ? "FocusVisualSecondary" : "FocusVisualPrimary");
  metrics.borderColors.bottom = metrics.borderColors.left = metrics.borderColors.right = metrics.borderColors.top =
      innerColor;

  metrics.borderRadii.bottomLeft.horizontal =
      (metrics.borderRadii.bottomLeft.horizontal ? metrics.borderRadii.bottomLeft.horizontal : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.bottomLeft.vertical =
      (metrics.borderRadii.bottomLeft.vertical ? metrics.borderRadii.bottomLeft.vertical : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.bottomRight.horizontal =
      (metrics.borderRadii.bottomRight.horizontal ? metrics.borderRadii.bottomRight.horizontal : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.bottomRight.vertical =
      (metrics.borderRadii.bottomRight.vertical ? metrics.borderRadii.bottomRight.vertical : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.topLeft.horizontal =
      (metrics.borderRadii.topLeft.horizontal ? metrics.borderRadii.topLeft.horizontal : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.topLeft.vertical =
      (metrics.borderRadii.topLeft.vertical ? metrics.borderRadii.topLeft.vertical : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.topRight.horizontal =
      (metrics.borderRadii.topRight.horizontal ? metrics.borderRadii.topRight.horizontal : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);
  metrics.borderRadii.topRight.vertical =
      (metrics.borderRadii.topRight.vertical ? metrics.borderRadii.topRight.vertical : FOCUS_VISUAL_RADIUS) +
      FOCUS_VISUAL_WIDTH * (inner ? 1 : 2);

  metrics.borderStyles.bottom = metrics.borderStyles.left = metrics.borderStyles.right = metrics.borderStyles.top =
      facebook::react::BorderStyle::Solid;
  metrics.borderWidths.bottom = metrics.borderWidths.left = metrics.borderWidths.right = metrics.borderWidths.top =
      FOCUS_VISUAL_WIDTH * layoutMetrics.pointScaleFactor;
  return metrics;
}

void ComponentView::hostFocusVisual(bool show, winrt::com_ptr<ComponentView> view) noexcept {
  if ((view->m_flags & ComponentViewFeatures::FocusVisual) == ComponentViewFeatures::FocusVisual) {
    // Any previous view showing focus visuals should have removed them before another shows it
    assert(
        !m_focusPrimitive || !m_focusPrimitive->m_focusVisualComponent ||
        m_focusPrimitive->m_focusVisualComponent == view);
    assert(
        !m_focusPrimitive || !m_focusPrimitive->m_focusVisualComponent ||
        view->m_componentHostingFocusVisual.get() == this);
    if (show && !view->m_componentHostingFocusVisual) {
      view->m_componentHostingFocusVisual = get_strong();

      if (!m_focusPrimitive) {
        m_focusPrimitive = std::make_unique<FocusPrimitive>();
      }
      m_focusPrimitive->m_focusVisualComponent = view;

      if (!m_focusPrimitive->m_focusVisual) {
        m_focusPrimitive->m_focusVisual = m_compContext.CreateSpriteVisual();
        auto hostingVisual =
            winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
                visualToHostFocus())
                .as<winrt::Microsoft::UI::Composition::ContainerVisual>();
        if (hostingVisual) {
          hostingVisual.Children().InsertAtTop(
              winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
                  m_focusPrimitive->m_focusVisual));
        } else {
          assert(
              view.get() ==
              this); // When not using lifted comp, focus visuals should always host within their own component
          OuterVisual().InsertAt(m_focusPrimitive->m_focusVisual, 1);
        }
      }

      m_focusPrimitive->m_focusVisual.IsVisible(true);
      assert(view->viewProps()->enableFocusRing);
      if (!m_focusPrimitive->m_focusInnerPrimitive) {
        m_focusPrimitive->m_focusInnerPrimitive = std::make_shared<BorderPrimitive>(*this);
        m_focusPrimitive->m_focusVisual.InsertAt(m_focusPrimitive->m_focusInnerPrimitive->RootVisual(), 0);
      }
      if (!m_focusPrimitive->m_focusOuterPrimitive) {
        m_focusPrimitive->m_focusOuterPrimitive = std::make_shared<BorderPrimitive>(*this);
        m_focusPrimitive->m_focusVisual.InsertAt(m_focusPrimitive->m_focusOuterPrimitive->RootVisual(), 0);
      }
      if (auto focusVisual =
              winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
                  m_focusPrimitive->m_focusVisual)) {
        auto outerVisual =
            winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(
                view->OuterVisual());
        focusVisual.ParentForTransform(outerVisual);
      }
      updateFocusLayoutMetrics();
      auto innerFocusMetrics = view->focusLayoutMetrics(true /*inner*/);
      m_focusPrimitive->m_focusInnerPrimitive->finalize(
          innerFocusMetrics, view->focusBorderMetrics(true /*inner*/, innerFocusMetrics));
      auto outerFocusMetrics = view->focusLayoutMetrics(false /*inner*/);
      m_focusPrimitive->m_focusOuterPrimitive->finalize(
          outerFocusMetrics, view->focusBorderMetrics(false /*inner*/, outerFocusMetrics));
    } else if (!show && view->m_componentHostingFocusVisual && m_focusPrimitive) {
      m_focusPrimitive->m_focusVisualComponent = nullptr;
      m_focusPrimitive->m_focusVisual.IsVisible(false);
      view->m_componentHostingFocusVisual = nullptr;
    }
  }
}

void ComponentView::updateShadowProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  // Shadow Properties
  if (oldViewProps.shadowOffset != newViewProps.shadowOffset || oldViewProps.shadowColor != newViewProps.shadowColor ||
      oldViewProps.shadowOpacity != newViewProps.shadowOpacity ||
      oldViewProps.shadowRadius != newViewProps.shadowRadius || oldViewProps.boxShadow != newViewProps.boxShadow) {
    applyShadowProps(newViewProps);
  }
}

void ComponentView::applyShadowProps(const facebook::react::ViewProps &viewProps) noexcept {
  auto shadow = m_compContext.CreateDropShadow();
  if (!viewProps.boxShadow.empty()) {
    shadow.Offset({viewProps.boxShadow[0].offsetX, viewProps.boxShadow[0].offsetY, 0});
    shadow.Opacity(1);
    shadow.BlurRadius(viewProps.boxShadow[0].blurRadius);
    shadow.Color(theme()->Color(*viewProps.boxShadow[0].color));
  } else {
    shadow.Offset({viewProps.shadowOffset.width, viewProps.shadowOffset.height, 0});
    shadow.Opacity(viewProps.shadowOpacity);
    shadow.BlurRadius(viewProps.shadowRadius);
    if (viewProps.shadowColor)
      shadow.Color(theme()->Color(*viewProps.shadowColor));
  }

  Visual().as<winrt::Microsoft::ReactNative::Composition::Experimental::ISpriteVisual>().Shadow(shadow);
}

void ComponentView::updateTransformProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps,
    winrt::Microsoft::ReactNative::Composition::Experimental::IVisual visual) noexcept {
  // check for backfaceVisibility prop
  if (oldViewProps.backfaceVisibility != newViewProps.backfaceVisibility) {
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Inherit) ==
        facebook::react::BackfaceVisibility::Auto);
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Visible) ==
        facebook::react::BackfaceVisibility::Visible);
    static_assert(
        static_cast<facebook::react::BackfaceVisibility>(
            winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility::Hidden) ==
        facebook::react::BackfaceVisibility::Hidden);
    visual.BackfaceVisibility(static_cast<winrt::Microsoft::ReactNative::Composition::Experimental::BackfaceVisibility>(
        newViewProps.backfaceVisibility));
  }

  // Transform - TODO doesn't handle multiple of the same kind of transform -- Doesn't handle hittesting updates
  if (oldViewProps.transform != newViewProps.transform) {
    m_FinalizeTransform = true;
  }
}

void ComponentView::updateAccessibilityProps(
    const facebook::react::ViewProps &oldViewProps,
    const facebook::react::ViewProps &newViewProps) noexcept {
  if (!UiaClientsAreListening())
    return;

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(), UIA_IsKeyboardFocusablePropertyId, oldViewProps.focusable, newViewProps.focusable);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_NamePropertyId,
      oldViewProps.accessibilityLabel,
      newViewProps.accessibilityLabel.empty() ? DefaultAccessibleName() : newViewProps.accessibilityLabel);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_IsContentElementPropertyId,
      (oldViewProps.accessible && oldViewProps.accessibilityRole != "none"),
      (newViewProps.accessible && newViewProps.accessibilityRole != "none"));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_IsControlElementPropertyId,
      (oldViewProps.accessible && oldViewProps.accessibilityRole != "none"),
      (newViewProps.accessible && newViewProps.accessibilityRole != "none"));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_IsEnabledPropertyId,
      !(oldViewProps.accessibilityState && oldViewProps.accessibilityState->disabled),
      !(newViewProps.accessibilityState && newViewProps.accessibilityState->disabled));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_IsEnabledPropertyId,
      !(oldViewProps.accessibilityState && oldViewProps.accessibilityState->busy),
      !(newViewProps.accessibilityState && newViewProps.accessibilityState->busy));

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(), UIA_ControlTypePropertyId, oldViewProps.accessibilityRole, newViewProps.accessibilityRole);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(), UIA_HelpTextPropertyId, oldViewProps.accessibilityHint, newViewProps.accessibilityHint);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_PositionInSetPropertyId,
      oldViewProps.accessibilityPosInSet,
      newViewProps.accessibilityPosInSet);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_SizeOfSetPropertyId,
      oldViewProps.accessibilitySetSize,
      newViewProps.accessibilitySetSize);

  winrt::Microsoft::ReactNative::implementation::UpdateUiaProperty(
      EnsureUiaProvider(),
      UIA_LiveSettingPropertyId,
      oldViewProps.accessibilityLiveRegion,
      newViewProps.accessibilityLiveRegion);

  if ((oldViewProps.accessibilityState.has_value() && oldViewProps.accessibilityState->selected.has_value()) !=
      ((newViewProps.accessibilityState.has_value() && newViewProps.accessibilityState->selected.has_value()))) {
    auto compProvider =
        EnsureUiaProvider()
            .try_as<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>();
    if (compProvider) {
      if ((newViewProps.accessibilityState.has_value() && newViewProps.accessibilityState->selected.has_value())) {
        winrt::Microsoft::ReactNative::implementation::AddSelectionItemsToContainer(compProvider.get());
      } else {
        winrt::Microsoft::ReactNative::implementation::RemoveSelectionItemsFromContainer(compProvider.get());
      }
    }
  }
}

std::optional<std::string> ComponentView::getAccessiblityValue() noexcept {
  return std::static_pointer_cast<const facebook::react::ViewProps>(props())->accessibilityValue.text.value();
}

void ComponentView::setAcccessiblityValue(std::string &&value) noexcept {
  // no-op
}

bool ComponentView::getAcccessiblityIsReadOnly() noexcept {
  return true;
}

ToggleState ComponentView::getToggleState() noexcept {
  return ToggleState::ToggleState_Off;
}

void ComponentView::Toggle() noexcept {
  // no-op
}

void ComponentView::updateClippingPath(
    facebook::react::LayoutMetrics const &layoutMetrics,
    const facebook::react::ViewProps &viewProps) noexcept {
  auto borderMetrics = BorderPrimitive::resolveAndAlignBorderMetrics(layoutMetrics, viewProps);

  if (borderMetrics.borderRadii.topLeft.horizontal == 0 && borderMetrics.borderRadii.topRight.horizontal == 0 &&
      borderMetrics.borderRadii.bottomLeft.horizontal == 0 && borderMetrics.borderRadii.bottomRight.horizontal == 0 &&
      borderMetrics.borderRadii.topLeft.vertical == 0 && borderMetrics.borderRadii.topRight.vertical == 0 &&
      borderMetrics.borderRadii.bottomLeft.vertical == 0 && borderMetrics.borderRadii.bottomRight.vertical == 0) {
    Visual().as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(nullptr);
  } else {
    winrt::com_ptr<ID2D1PathGeometry> pathGeometry = BorderPrimitive::GenerateRoundedRectPathGeometry(
        m_compContext,
        borderMetrics.borderRadii,
        {0, 0, 0, 0},
        {0,
         0,
         layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
         layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});

    Visual().as<::Microsoft::ReactNative::Composition::Experimental::IVisualInterop>()->SetClippingPath(
        pathGeometry.get());
  }
}

std::pair<facebook::react::Cursor, HCURSOR> ComponentView::cursor() const noexcept {
  return {viewProps()->cursor, nullptr};
}

void ComponentView::indexOffsetForBorder(uint32_t &index) const noexcept {
  if (m_borderPrimitive) {
    index += m_borderPrimitive->numberOfVisuals();
  }
}

void ComponentView::OnRenderingDeviceLost() noexcept {}

std::vector<facebook::react::ComponentDescriptorProvider>
ComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

comp::CompositionPropertySet ComponentView::EnsureCenterPointPropertySet() noexcept {
  if (m_centerPropSet == nullptr) {
    if (auto compositor =
            winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerCompositor(
                m_compContext)) {
      m_centerPropSet = compositor.CreatePropertySet();
      UpdateCenterPropertySet();
      m_centerPropSet.InsertMatrix4x4(L"transform", winrt::Windows::Foundation::Numerics::float4x4::identity());
      m_centerPropSet.InsertVector3(L"translation", {0, 0, 0});
    }
  }

  return m_centerPropSet;
}

// Create a PropertySet that will hold properties used for animation facades:
//   "center":      This is the center of the Visual, which is used by animations and transforms to rotate around the
//                  correct center
//   "dpiScale3":   This is a vector3 containing the scale factor for this view, used in expressions to convert from
//                  dips to px
//   "dpiScale3":   This is a vector3 containing the inverse scale factor for this view, used in expressions to
//                  convert from px to dips "translation": An additional translation to apply to the to the visual in
//                  addition to Offset.  This is used to apply any translation properties set, or animated
//   "transform":   A transform to apply to the visual.  This transform is in dips.
void ComponentView::UpdateCenterPropertySet() noexcept {
  if (m_centerPropSet != nullptr) {
    m_centerPropSet.InsertVector3(
        L"center",
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor / 2,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor / 2,
         0});
    m_centerPropSet.InsertVector3(
        L"dpiScale3",
        {m_layoutMetrics.pointScaleFactor, m_layoutMetrics.pointScaleFactor, m_layoutMetrics.pointScaleFactor});
    m_centerPropSet.InsertVector3(
        L"dpiScale3Inv",
        {1 / m_layoutMetrics.pointScaleFactor,
         1 / m_layoutMetrics.pointScaleFactor,
         1 / m_layoutMetrics.pointScaleFactor});
  }
}

void ComponentView::EnsureTransformMatrixFacade() noexcept {
  if (m_hasTransformMatrixFacade)
    return;
  m_hasTransformMatrixFacade = true;

  auto centerPointPropSet = EnsureCenterPointPropertySet();
  if (auto compositor =
          winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerCompositor(
              m_compContext)) {
    // TODO cache expression instead of creating new ones all the time
    auto expression = compositor.CreateExpressionAnimation(
        L"Matrix4x4.CreateFromScale(PS.dpiScale3Inv) * Matrix4x4.CreateFromTranslation(PS.translation) * PS.transform * Matrix4x4.CreateFromScale(PS.dpiScale3)");
    expression.SetReferenceParameter(L"PS", centerPointPropSet);
    winrt::Microsoft::ReactNative::Composition::Experimental::CompositionContextHelper::InnerVisual(OuterVisual())
        .StartAnimation(L"TransformMatrix", expression);
  }
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitter() noexcept {
  return m_eventEmitter;
}

facebook::react::SharedViewEventEmitter ComponentView::eventEmitterAtPoint(facebook::react::Point /*pt*/) noexcept {
  return m_eventEmitter;
}

bool ComponentView::anyHitTestHelper(
    facebook::react::Tag &targetTag,
    facebook::react::Point &ptContent,
    facebook::react::Point &localPt) const noexcept {
  if (auto index = m_children.Size()) {
    do {
      index--;
      targetTag = winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_children.GetAt(index))
                      ->hitTest(ptContent, localPt);
      if (targetTag != -1) {
        return true;
      }
    } while (index != 0);
  }

  return false;
}

std::string ComponentView::DefaultControlType() const noexcept {
  return "group";
}

std::string ComponentView::DefaultAccessibleName() const noexcept {
  return "";
}

std::string ComponentView::DefaultHelpText() const noexcept {
  return "";
}

facebook::react::SharedViewProps ViewComponentView::defaultProps() noexcept {
  static auto const defaultViewProps = std::make_shared<facebook::react::ViewProps const>();
  return defaultViewProps;
}

ViewComponentView::ViewComponentView(
    const facebook::react::SharedViewProps &defaultProps,
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext,
    ComponentViewFeatures flags,
    winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder *builder)
    : base_type(compContext, tag, reactContext, flags, builder),
      m_props(defaultProps ? defaultProps : ViewComponentView::defaultProps()) {}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ViewComponentView::createVisual() noexcept {
  return m_compContext.CreateSpriteVisual();
}

void ViewComponentView::CreateInternalVisualHandler(
    const winrt::Microsoft::ReactNative::Composition::Experimental::CreateInternalVisualDelegate &handler) {
  m_createInternalVisualHandler = handler;
}

winrt::Microsoft::ReactNative::Composition::Experimental::CreateInternalVisualDelegate
ViewComponentView::CreateInternalVisualHandler() const noexcept {
  return m_createInternalVisualHandler;
}

void ViewComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    if (m_createInternalVisualHandler) {
      m_visual = m_createInternalVisualHandler(*this);
    } else if (m_builder && m_builder->CreateVisualHandler()) {
      m_visual =
          winrt::Microsoft::ReactNative::Composition::Experimental::MicrosoftCompositionContextHelper::CreateVisual(
              m_builder->CreateVisualHandler()(*this));
    } else {
      m_visual = createVisual();
    }
    OuterVisual().InsertAt(m_visual, 0);
  }
}

winrt::Microsoft::ReactNative::ComponentView ViewComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::Experimental::ICompositionContext &compContext,
    facebook::react::Tag tag,
    winrt::Microsoft::ReactNative::ReactContext const &reactContext) noexcept {
  return winrt::make<ViewComponentView>(
      ViewComponentView::defaultProps(), compContext, tag, reactContext, ComponentViewFeatures::Default);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual
ViewComponentView::VisualToMountChildrenInto() noexcept {
  if (m_builder && m_builder->VisualToMountChildrenIntoHandler())
    return m_builder->VisualToMountChildrenIntoHandler()(*this);
  return Visual();
}

void ViewComponentView::MountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::MountChildComponentView(childComponentView, index);

  indexOffsetForBorder(index);
  ensureVisual();

  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    auto visualIndex = index;
    // Most of the time child index will align with visual index.
    // But if we have non-visual children, we need to account for that.
    if (m_hasNonVisualChildren) {
      for (uint32_t i = 0; i <= index; i++) {
        if (!m_children.GetAt(i).try_as<ComponentView>()) {
          visualIndex--;
        }
      }
    }
    VisualToMountChildrenInto().InsertAt(compositionChild->OuterVisual(), visualIndex);
  } else {
    m_hasNonVisualChildren = true;
  }
}

void ViewComponentView::UnmountChildComponentView(
    const winrt::Microsoft::ReactNative::ComponentView &childComponentView,
    uint32_t index) noexcept {
  base_type::UnmountChildComponentView(childComponentView, index);

  indexOffsetForBorder(index);
  if (auto compositionChild = childComponentView.try_as<ComponentView>()) {
    VisualToMountChildrenInto().Remove(compositionChild->OuterVisual());
  }
}

void ViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(oldProps ? oldProps : m_props);
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ViewProps>(props);

  ensureVisual();
  if (oldViewProps.opacity != newViewProps.opacity) {
    Visual().Opacity(newViewProps.opacity);
  }
  if (oldViewProps.testId != newViewProps.testId) {
    Visual().Comment(winrt::to_hstring(newViewProps.testId));
  }

  // update BaseComponentView props
  updateAccessibilityProps(oldViewProps, newViewProps);
  updateTransformProps(oldViewProps, newViewProps, Visual());
  base_type::updateProps(props, oldProps);

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

const winrt::Microsoft::ReactNative::IComponentProps ViewComponentView::userProps(
    facebook::react::Props::Shared const &props) noexcept {
  const auto &abiViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(props);
  return abiViewProps.UserProps();
}

facebook::react::Tag ViewComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptLocal{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Tag targetTag = -1;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      anyHitTestHelper(targetTag, ptLocal, localPt))
    return targetTag;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptLocal.x >= 0 && ptLocal.x <= m_layoutMetrics.frame.size.width && ptLocal.y >= 0 &&
      ptLocal.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptLocal;
    return Tag();
  }

  return -1;
}

inline winrt::Windows::System::VirtualKey GetLeftOrRightModifiedKey(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    winrt::Windows::System::VirtualKey leftKey,
    winrt::Windows::System::VirtualKey rightKey) {
  return ((source.GetKeyState(leftKey) & winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
          winrt::Microsoft::UI::Input::VirtualKeyStates::Down)
      ? leftKey
      : rightKey;
}

std::string CodeFromVirtualKey(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    winrt::Windows::System::VirtualKey virtualKey) {
  int key = static_cast<int>(virtualKey);

  if (isdigit(key)) {
    return "Digit" + std::string(1, static_cast<char>(key));
  } else if (isupper(key)) {
    return "Key" + std::string(1, static_cast<char>(key));
  } else {
    // Override the virtual key if it's modified key of Control, Shift or Menu
    if (virtualKey == winrt::Windows::System::VirtualKey::Control) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftControl, winrt::Windows::System::VirtualKey::RightControl);
    } else if (virtualKey == winrt::Windows::System::VirtualKey::Shift) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftShift, winrt::Windows::System::VirtualKey::RightShift);
    } else if (virtualKey == winrt::Windows::System::VirtualKey::Menu) {
      virtualKey = GetLeftOrRightModifiedKey(
          source, winrt::Windows::System::VirtualKey::LeftMenu, winrt::Windows::System::VirtualKey::RightMenu);
    }
  }

  return ::Microsoft::ReactNative::GetOrUnidentifiedCode(virtualKey);
}

void ViewComponentView::OnKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  auto eventCode = CodeFromVirtualKey(args.KeyboardSource(), args.Key());
  bool fShift =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Shift) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fAlt =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Menu) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fCtrl =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fMeta =
      ((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::LeftWindows) &
        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ||
      ((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::RightWindows) &
        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down);

  if (args.OriginalSource() == Tag() && !args.Handled()) {
    facebook::react::KeyEvent event;
    event.shiftKey = fShift;
    event.ctrlKey = fCtrl;
    event.altKey = fAlt;
    event.metaKey = fMeta;

    event.key = ::Microsoft::ReactNative::FromVirtualKey(
        args.Key(),
        event.shiftKey,
        !!((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::CapitalLock) &
            winrt::Microsoft::UI::Input::VirtualKeyStates::Locked) ==
           winrt::Microsoft::UI::Input::VirtualKeyStates::Locked));
    event.code = eventCode;
    m_eventEmitter->onKeyDown(event);
  }

  for (const auto &handledKey : m_props->keyDownEvents) {
    if (handledKey.code == eventCode && handledKey.altKey == fAlt && handledKey.ctrlKey == fCtrl &&
        handledKey.metaKey == fMeta && handledKey.shiftKey == fShift &&
        handledKey.handledEventPhase == facebook::react::HandledEventPhase::Bubbling) {
      // Stop bubbling event if marked as handled from JS
      args.Handled(true);
    }
  }

  base_type::OnKeyDown(args);
}

void ViewComponentView::OnKeyUp(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  auto eventCode = CodeFromVirtualKey(args.KeyboardSource(), args.Key());
  bool fShift =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Shift) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fAlt =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Menu) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fCtrl =
      (args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::Control) &
       winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down;
  bool fMeta =
      ((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::LeftWindows) &
        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ||
      ((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::RightWindows) &
        winrt::Microsoft::UI::Input::VirtualKeyStates::Down) == winrt::Microsoft::UI::Input::VirtualKeyStates::Down);

  if (args.OriginalSource() == Tag()) {
    facebook::react::KeyEvent event;
    event.shiftKey = fShift;
    event.ctrlKey = fCtrl;
    event.altKey = fAlt;
    event.metaKey = fMeta;

    event.key = ::Microsoft::ReactNative::FromVirtualKey(
        args.Key(),
        event.shiftKey,
        !!((args.KeyboardSource().GetKeyState(winrt::Windows::System::VirtualKey::CapitalLock) &
            winrt::Microsoft::UI::Input::VirtualKeyStates::Down) ==
           winrt::Microsoft::UI::Input::VirtualKeyStates::Down));
    event.code = eventCode;
    m_eventEmitter->onKeyUp(event);
  }

  for (const auto &handledKey : m_props->keyUpEvents) {
    if (handledKey.code == eventCode && handledKey.altKey == fAlt && handledKey.ctrlKey == fCtrl &&
        handledKey.metaKey == fMeta && handledKey.shiftKey == fShift &&
        handledKey.handledEventPhase == facebook::react::HandledEventPhase::Bubbling) {
      // Stop bubbling event if marked as handled from JS
      args.Handled(true);
    }
  }

  base_type::OnKeyUp(args);
}

void ViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }
  ensureVisual();
  base_type::updateLayoutMetrics(layoutMetrics, oldLayoutMetrics);
  Visual().Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
}

void ViewComponentView::prepareForRecycle() noexcept {}

const facebook::react::SharedViewProps &ViewComponentView::viewProps() const noexcept {
  return m_props;
}

winrt::Microsoft::ReactNative::ViewProps ViewComponentView::ViewProps() noexcept {
  return ViewPropsInner();
}

winrt::Microsoft::ReactNative::ViewProps ViewComponentView::ViewPropsInner() noexcept {
  // If we have AbiViewProps, then we dont need to new up a props wrapper
  if (m_builder) {
    const auto &abiViewProps = *std::static_pointer_cast<const ::Microsoft::ReactNative::AbiViewProps>(m_props);
    return abiViewProps.ViewProps();
  }

  return winrt::make<winrt::Microsoft::ReactNative::implementation::ViewProps>(m_props);
}

winrt::Microsoft::ReactNative::Composition::Experimental::IVisual ViewComponentView::Visual() const noexcept {
  assert(m_visual);
  return m_visual;
}

bool ViewComponentView::focusable() const noexcept {
  return m_props->focusable;
}

std::string ViewComponentView::DefaultControlType() const noexcept {
  return "group";
}

winrt::IInspectable ComponentView::EnsureUiaProvider() noexcept {
  if (m_uiaProvider == nullptr) {
    m_uiaProvider =
        winrt::make<winrt::Microsoft::ReactNative::implementation::CompositionDynamicAutomationProvider>(*get_strong());
  }
  return m_uiaProvider;
}

bool IntersectRect(RECT *prcDst, const RECT &prcSrc1, const RECT &prcSrc2) {
  prcDst->left = std::max(prcSrc1.left, prcSrc2.left);
  prcDst->right = std::min(prcSrc1.right, prcSrc2.right);

  if (prcDst->left < prcDst->right) {
    prcDst->top = std::max(prcSrc1.top, prcSrc2.top);
    prcDst->bottom = std::min(prcSrc1.bottom, prcSrc2.bottom);

    if (prcDst->top < prcDst->bottom) {
      return true;
    }
  }

  prcDst->left = prcDst->top = prcDst->right = prcDst->bottom = 0;
  return false;
}

winrt::Microsoft::ReactNative::implementation::ClipState ComponentView::getClipState() noexcept {
  if (!m_parent) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  RECT intersection;
  const auto parentRect =
      winrt::get_self<winrt::Microsoft::ReactNative::implementation::ComponentView>(m_parent)->getClientRect();
  const auto clientRect = getClientRect();

  IntersectRect(&intersection, parentRect, clientRect);

  if (intersection == clientRect) {
    return m_parent.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->getClipState();
  }

  if (((intersection.right - intersection.left) == 0) && ((intersection.bottom - intersection.top) == 0)) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  if (m_parent.as<winrt::Microsoft::ReactNative::Composition::implementation::ComponentView>()->getClipState() ==
      winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped) {
    return winrt::Microsoft::ReactNative::implementation::ClipState::FullyClipped;
  }

  return winrt::Microsoft::ReactNative::implementation::ClipState::PartialClip;
}

} // namespace winrt::Microsoft::ReactNative::Composition::implementation

namespace winrt::Microsoft::ReactNative::implementation {

winrt::Microsoft::ReactNative::ComponentView lastDeepChild(
    const winrt::Microsoft::ReactNative::ComponentView &view) noexcept {
  auto current = view;
  while (current) {
    auto children = current.Children();
    if (children.Size() == 0)
      break;
    current = children.GetAt(children.Size() - 1);
  }
  return current;
}

// Walks the tree calling the function fn on each node.
// If fn returns true, then walkTree stops iterating over the tree, and returns true.
// If the tree walk completes without fn returning true, then walkTree returns false.
bool walkTree(
    const winrt::Microsoft::ReactNative::ComponentView &view,
    bool forward,
    Mso::Functor<bool(const winrt::Microsoft::ReactNative::ComponentView &)> &fn) noexcept {
  if (forward) {
    if (fn(view)) {
      return true;
    }

    auto children = view.Children();
    for (auto it = children.begin(); it != children.end(); ++it) {
      return walkTree(*it, forward, fn);
    }

    auto current = view;
    auto parent = current.Parent();
    while (parent) {
      auto parentsChildren = parent.Children();
      auto itNextView = std::find(parentsChildren.begin(), parentsChildren.end(), current);
      assert(itNextView != parentsChildren.end());
      ++itNextView;
      if (itNextView != parentsChildren.end()) {
        return walkTree(*itNextView, true, fn);
      }
      current = parent;
      parent = current.Parent();
    }

  } else {
    auto current = view;
    auto parent = current.Parent();
    while (parent) {
      auto parentsChildren = parent.Children();
      uint32_t index;
      bool success = parent.Children().IndexOf(current, index);
      assert(success);
      if (index > 0) {
        auto lastChild = lastDeepChild(parent.Children().GetAt(index - 1));
        if (fn(lastChild))
          return true;
        return walkTree(lastChild, false, fn);
      }

      if (fn(parent)) {
        return true;
      }
      current = parent;
      parent = current.Parent();
    }
  }
  return false;
}

} // namespace winrt::Microsoft::ReactNative::implementation
