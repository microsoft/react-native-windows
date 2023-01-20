
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "SwitchComponentView.h"

//#include <UI.Xaml.Controls.h>
//#include <Utils/ValueUtils.h>

//#pragma warning(push)
//#pragma warning(disable : 4305)
//#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
//#pragma warning(pop)
//
//#include <windows.ui.composition.interop.h>
//
//#include <unicode.h>

namespace Microsoft::ReactNative {

SwitchComponentView::SwitchComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {}

std::vector<facebook::react::ComponentDescriptorProvider>
SwitchComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void SwitchComponentView::mountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  // ensureVisual();

  // m_children.insert(std::next(m_children.begin(), index), &childComponentView);
  // const_cast<IComponentView &>(childComponentView).parent(this);

  // m_visual.InsertAt(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual(), index);
}

void SwitchComponentView::unmountChildComponentView(const IComponentView &childComponentView, uint32_t index) noexcept {
  // m_children.erase(std::next(m_children.begin(), index));

  // m_visual.Remove(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual());
  // const_cast<IComponentView &>(childComponentView).parent(nullptr);
}

void SwitchComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  // const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);
  // const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(oldProps);

  //  ensureVisual();
  //
  //  if (!oldProps || oldViewProps.backgroundColor != newViewProps.backgroundColor) {
  //    if (newViewProps.backgroundColor) {
  //      m_visual.Brush(m_compContext.CreateColorBrush((*newViewProps.backgroundColor).m_color));
  //    } else {
  //      m_visual.Brush(m_compContext.CreateColorBrush({0, 0, 0, 0}));
  //    }
  //  }
  //
  //  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
  }

  void SwitchComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}

  void SwitchComponentView::updateState(
      facebook::react::State::Shared const &state, facebook::react::State::Shared const &oldState) noexcept {
    // const auto &newState = *std::static_pointer_cast<facebook::react::ScrollViewShadowNode::ConcreteState
    // const>(state);

    // m_contentSize = newState.getData().getContentSize();
    // updateContentVisualSize();
  }

  void SwitchComponentView::updateLayoutMetrics(
      facebook::react::LayoutMetrics const &layoutMetrics,
      facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
    // Set Position & Size Properties
    // ensureVisual();

    // if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    //   m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
    // }

    //// m_needsBorderUpdate = true;
    // m_layoutMetrics = layoutMetrics;

    // UpdateCenterPropertySet();
    // m_visual.Size(
    //     {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
    //      layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
    // m_visual.Offset({
    //     layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
    //     layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
    //     0.0f,
    // });
    // updateContentVisualSize();
  }

  void SwitchComponentView::updateContentVisualSize() noexcept {
    // m_visual.ContentSize(
    //     {std::max(m_contentSize.width, m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor,
    //      std::max(m_contentSize.height, m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor});
  }

  void SwitchComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
    // m_element.FinalizeProperties();
  }

  void SwitchComponentView::prepareForRecycle() noexcept {}
  facebook::react::Props::Shared SwitchComponentView::props() noexcept {
    assert(false);
    return {};
  }

  void SwitchComponentView::ensureVisual() noexcept {
    // if (!m_visual) {
    //   m_visual = m_compContext.CreateScrollerVisual();
    //   m_scrollPositionChangedRevoker = m_visual.ScrollPositionChanged(
    //       winrt::auto_revoke,
    //       [this](
    //           winrt::IInspectable const & /*sender*/,
    //           winrt::Microsoft::ReactNative::Composition::ScrollPositionChangedArgs const &args) {
    //         auto eventEmitter = GetEventEmitter();
    //         if (eventEmitter) {
    //           facebook::react::ScrollViewMetrics scrollMetrics;
    //           scrollMetrics.containerSize.height = m_layoutMetrics.frame.size.height;
    //           scrollMetrics.containerSize.width = m_layoutMetrics.frame.size.width;
    //           scrollMetrics.contentOffset.x = args.Position().x / m_layoutMetrics.pointScaleFactor;
    //           scrollMetrics.contentOffset.y = args.Position().y / m_layoutMetrics.pointScaleFactor;
    //           scrollMetrics.zoomScale = 1.0;
    //           scrollMetrics.contentSize.height = std::max(m_contentSize.height, m_layoutMetrics.frame.size.height);
    //           scrollMetrics.contentSize.width = std::max(m_contentSize.width, m_layoutMetrics.frame.size.width);
    //           std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)
    //               ->onScroll(scrollMetrics);
    //         }
    //       });
    // }
  }

  facebook::react::Tag SwitchComponentView::hitTest(facebook::react::Point pt, facebook::react::Point & localPt)
      const noexcept {
    // facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

    // facebook::react::Point ptContent{
    //     ptViewport.x + m_visual.ScrollPosition().x / m_layoutMetrics.pointScaleFactor,
    //     ptViewport.y + m_visual.ScrollPosition().y / m_layoutMetrics.pointScaleFactor};

    // facebook::react::Tag targetTag;
    // if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
    //      m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
    //     std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptContent, &localPt](auto child) {
    //       targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptContent, localPt);
    //       return targetTag != -1;
    //     }))
    //   return targetTag;

    // if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
    //      m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
    //     ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
    //     ptViewport.y <= m_layoutMetrics.frame.size.height) {
    //   localPt = ptViewport;
    //   return this->tag();
    // }

    return -1;
  }

   facebook::react::SharedTouchEventEmitter SwitchComponentView::touchEventEmitter() noexcept {
    return {};
   }
  
   winrt::Microsoft::ReactNative::Composition::IVisual SwitchComponentView::Visual() const noexcept {
     return m_visual;
   }

} // namespace Microsoft::ReactNative
