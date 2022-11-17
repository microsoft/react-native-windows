
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "ScrollViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <windows.ui.composition.interop.h>

#include <unicode.h>

namespace Microsoft::ReactNative {

ScrollViewComponentView::ScrollViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  // static auto const defaultProps = std::make_shared<facebook::react::TextProps const>();
  // m_props = defaultProps;

  // m_element.Content(m_contentPanel);

  /*
    m_scrollViewerViewChangingRevoker =
        m_element.ViewChanging(winrt::auto_revoke, [this](const auto &sender, const auto &args) {
          const auto scrollViewerNotNull = sender.as<xaml::Controls::ScrollViewer>();

          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(args.NextView().HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(args.NextView().VerticalOffset());
          scrollMetrics.zoomScale = args.NextView().ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          // If we are transitioning to inertial scrolling.
          if (m_isScrolling && !m_isScrollingFromInertia && args.IsInertial()) {
            m_isScrollingFromInertia = true;

            if (m_eventEmitter) {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onScrollEndDrag(scrollMetrics);
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onMomentumScrollBegin(scrollMetrics);
            }
          }

          if (m_eventEmitter) {
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                ->onScroll(scrollMetrics);
          }
        });

    m_scrollViewerDirectManipulationStartedRevoker =
        m_element.DirectManipulationStarted(winrt::auto_revoke, [this](const auto &sender, const auto &) {
          m_isScrolling = true;


          //if (m_dismissKeyboardOnDrag && m_SIPEventHandler) {
          //  m_SIPEventHandler->TryHide();
          //}


          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(m_element.HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(m_element.VerticalOffset());
          scrollMetrics.zoomScale = m_element.ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          const auto scrollViewer = sender.as<xaml::Controls::ScrollViewer>();
          if (m_eventEmitter) {
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                ->onScrollBeginDrag(scrollMetrics);
          }
        });

    m_scrollViewerDirectManipulationCompletedRevoker =
        m_element.DirectManipulationCompleted(winrt::auto_revoke, [this](const auto &sender, const auto &args) {
          const auto scrollViewer = sender.as<xaml::Controls::ScrollViewer>();

          facebook::react::ScrollViewMetrics scrollMetrics;
          scrollMetrics.containerSize.height = static_cast<facebook::react::Float>(m_element.ActualHeight());
          scrollMetrics.containerSize.width = static_cast<facebook::react::Float>(m_element.ActualWidth());
          scrollMetrics.contentOffset.x = static_cast<facebook::react::Float>(m_element.HorizontalOffset());
          scrollMetrics.contentOffset.y = static_cast<facebook::react::Float>(m_element.VerticalOffset());
          scrollMetrics.zoomScale = m_element.ZoomFactor();
          scrollMetrics.contentSize.height = static_cast<facebook::react::Float>(m_contentPanel.ActualHeight());
          scrollMetrics.contentSize.width = static_cast<facebook::react::Float>(m_contentPanel.ActualWidth());

          if (m_eventEmitter) {
            if (m_isScrollingFromInertia) {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onMomentumScrollEnd(scrollMetrics);
            } else {
              std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(m_eventEmitter)
                  ->onScrollEndDrag(scrollMetrics);
            }
          }

          m_isScrolling = false;
          m_isScrollingFromInertia = false;
        });
        */
}

std::vector<facebook::react::ComponentDescriptorProvider>
ScrollViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void ScrollViewComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  ensureVisual();

  m_children.insert(std::next(m_children.begin(), index), &childComponentView);
  const_cast<IComponentView &>(childComponentView).parent(this);

  m_visual.InsertAt(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual(), index);
}

void ScrollViewComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  m_visual.Remove(static_cast<const CompositionBaseComponentView &>(childComponentView).Visual());
  const_cast<IComponentView &>(childComponentView).parent(nullptr);
}

void ScrollViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(oldProps);

  ensureVisual();

  if (!oldProps || oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      m_visual.Brush(m_compContext.CreateColorBrush((*newViewProps.backgroundColor).m_color));
    } else {
      m_visual.Brush(m_compContext.CreateColorBrush({0, 0, 0, 0}));
    }
  }

  /*
  if (oldViewProps.borderColors != newViewProps.borderColors) {
    if (newViewProps.borderColors.all) {
      m_element.BorderBrush(SolidColorBrushFrom(*newViewProps.borderColors.all));
    } else {
      m_element.ClearValue(winrt::Microsoft::ReactNative::ViewPanel::BorderBrushProperty());
    }
  }

  if (oldViewProps.borderStyles != newViewProps.borderStyles) {
    m_needsBorderUpdate = true;
  }
  */
  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

void ScrollViewComponentView::updateEventEmitter(facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}

void ScrollViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ScrollViewShadowNode::ConcreteState const>(state);

  m_contentSize = newState.getData().getContentSize();
  updateContentVisualSize();
}

void ScrollViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
  updateContentVisualSize();
}

void ScrollViewComponentView::updateContentVisualSize() noexcept {
  m_visual.ContentSize(
      {std::max(m_contentSize.width, m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor,
       std::max(m_contentSize.height, m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor});
}

void ScrollViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void ScrollViewComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ScrollViewComponentView::props() noexcept {
  assert(false);
  return {};
}

/*
ScrollViewComponentView::ScrollInteractionTrackerOwner::ScrollInteractionTrackerOwner(
    ScrollViewComponentView *outer)
    : m_outer(outer) {}

void ScrollViewComponentView::ScrollInteractionTrackerOwner::CustomAnimationStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::IdleStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::InertiaStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::InteractingStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs args) noexcept {}
void ScrollViewComponentView::ScrollInteractionTrackerOwner::RequestIgnored(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs args) noexcept {}

void ScrollViewComponentView::ScrollInteractionTrackerOwner::ValuesChanged(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs args) noexcept {

  auto eventEmitter = m_outer->GetEventEmitter();
  if (eventEmitter) {
    facebook::react::ScrollViewMetrics scrollMetrics;
    scrollMetrics.containerSize.height = m_outer->Visual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.containerSize.width = m_outer->Visual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentOffset.x = args.Position().x / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentOffset.y = args.Position().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.zoomScale = m_outer->ContentVisual().Scale().x;
    scrollMetrics.contentSize.height = m_outer->ContentVisual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
    scrollMetrics.contentSize.width = m_outer->ContentVisual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;
    std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)->onScroll(scrollMetrics);
  }
}
*/
/*
void ScrollViewComponentView::OnPointerDown(const winrt::Windows::UI::Input::PointerPoint &pp) noexcept {
  m_visualInteractionSource.TryRedirectForManipulation(pp);
}
*/

bool ScrollViewComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_visual.ScrollPosition().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_visual.ScrollPosition().y / m_layoutMetrics.pointScaleFactor};

  if (std::any_of(m_children.rbegin(), m_children.rend(), [ptContent, delta](auto child) {
        return const_cast<CompositionBaseComponentView *>(static_cast<const CompositionBaseComponentView *>(child))
            ->ScrollWheel(ptContent, delta);
      }))
    return true;

  if (ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    m_visual.ScrollBy({0, static_cast<float>(-delta), 0});
    return true;
  }

  return false;
}

void ScrollViewComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateScrollerVisual();
    m_scrollPositionChangedRevoker = m_visual.ScrollPositionChanged(
        winrt::auto_revoke,
        [this](
            winrt::IInspectable const & /*sender*/,
            winrt::Microsoft::ReactNative::Composition::ScrollPositionChangedArgs const &args) {
          auto eventEmitter = GetEventEmitter();
          if (eventEmitter) {
            facebook::react::ScrollViewMetrics scrollMetrics;
            scrollMetrics.containerSize.height = m_layoutMetrics.frame.size.height;
            scrollMetrics.containerSize.width = m_layoutMetrics.frame.size.width;
            scrollMetrics.contentOffset.x = args.Position().x / m_layoutMetrics.pointScaleFactor;
            scrollMetrics.contentOffset.y = args.Position().y / m_layoutMetrics.pointScaleFactor;
            scrollMetrics.zoomScale = 1.0;
            scrollMetrics.contentSize.height = std::max(m_contentSize.height, m_layoutMetrics.frame.size.height);
            scrollMetrics.contentSize.width = std::max(m_contentSize.width, m_layoutMetrics.frame.size.width);
            std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)
                ->onScroll(scrollMetrics);
          }
        });
  }
}

facebook::react::Tag ScrollViewComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_visual.ScrollPosition().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_visual.ScrollPosition().y / m_layoutMetrics.pointScaleFactor};

  facebook::react::Tag targetTag;
  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptContent, &localPt](auto child) {
        targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptContent, localPt);
        return targetTag != -1;
      }))
    return targetTag;

  if ((m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptViewport;
    return this->tag();
  }

  return -1;
}

facebook::react::SharedTouchEventEmitter ScrollViewComponentView::touchEventEmitter() noexcept {
  return m_eventEmitter;
}

winrt::Microsoft::ReactNative::Composition::IVisual ScrollViewComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
