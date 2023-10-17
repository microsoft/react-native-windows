
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
#include "CompositionDynamicAutomationProvider.h"

namespace Microsoft::ReactNative {

constexpr float c_scrollerLineDelta = 16.0f;

std::shared_ptr<ScrollViewComponentView> ScrollViewComponentView::Create(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag) noexcept {
  return std::shared_ptr<ScrollViewComponentView>(new ScrollViewComponentView(compContext, tag));
}

ScrollViewComponentView::ScrollViewComponentView(
    const winrt::Microsoft::ReactNative::Composition::ICompositionContext &compContext,
    facebook::react::Tag tag)
    : Super(compContext, tag) {
  static auto const defaultProps = std::make_shared<facebook::react::ScrollViewProps const>();
  m_props = defaultProps;

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

void ScrollViewComponentView::mountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  ensureVisual();

  m_children.insert(std::next(m_children.begin(), index), &childComponentView);
  childComponentView.parent(this);

  m_scrollVisual.InsertAt(static_cast<CompositionBaseComponentView &>(childComponentView).OuterVisual(), index);
}

void ScrollViewComponentView::unmountChildComponentView(IComponentView &childComponentView, uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  m_scrollVisual.Remove(static_cast<CompositionBaseComponentView &>(childComponentView).OuterVisual());
  childComponentView.parent(nullptr);
}

void ScrollViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(m_props);

  ensureVisual();

  if (!oldProps || oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      m_scrollVisual.Brush(m_compContext.CreateColorBrush(newViewProps.backgroundColor.AsWindowsColor()));
    } else {
      m_scrollVisual.Brush(m_compContext.CreateColorBrush({0, 0, 0, 0}));
    }
  }

  // update BaseComponentView props
  updateShadowProps(oldViewProps, newViewProps, m_visual);
  updateTransformProps(oldViewProps, newViewProps, m_visual);
  updateBorderProps(oldViewProps, newViewProps);

  m_props = std::static_pointer_cast<facebook::react::ViewProps const>(props);
}

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
    OuterVisual().IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // m_needsBorderUpdate = true;
  updateBorderLayoutMetrics(layoutMetrics, *m_props);
  m_layoutMetrics = layoutMetrics;

  UpdateCenterPropertySet();
  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_scrollVisual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  updateContentVisualSize();
}

void ScrollViewComponentView::updateContentVisualSize() noexcept {
  m_scrollVisual.ContentSize(
      {std::max(m_contentSize.width, m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor,
       std::max(m_contentSize.height, m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor});
}

void ScrollViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void ScrollViewComponentView::prepareForRecycle() noexcept {}
facebook::react::Props::Shared ScrollViewComponentView::props() noexcept {
  return static_cast<facebook::react::Props::Shared>(m_props);
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

void ScrollViewComponentView::onPointerWheelChanged(
    const winrt::Microsoft::ReactNative::Composition::Input::PointerRoutedEventArgs &args) noexcept {
  auto ppp = args.GetCurrentPoint(-1).Properties();
  auto delta = static_cast<float>(ppp.MouseWheelDelta());
  if (ppp.IsHorizontalMouseWheel()) {
    if (delta > 0) {
      if (scrollLeft(delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    } else if (delta < 0) {
      if (scrollRight(-delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    }
  } else {
    if (delta > 0) {
      if (scrollUp(delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    } else if (delta < 0) {
      if (scrollDown(-delta * m_layoutMetrics.pointScaleFactor, true)) {
        args.Handled(true);
      }
    }
  }
}

void ScrollViewComponentView::onKeyDown(
    const winrt::Microsoft::ReactNative::Composition::Input::KeyboardSource &source,
    const winrt::Microsoft::ReactNative::Composition::Input::KeyRoutedEventArgs &args) noexcept {
  switch (args.Key()) {
    case winrt::Windows::System::VirtualKey::End:
      args.Handled(scrollToEnd(true));
      break;
    case winrt::Windows::System::VirtualKey::Home:
      args.Handled(scrollToStart(true));
      break;
    case winrt::Windows::System::VirtualKey::PageDown:
      args.Handled(pageDown(true));
      break;
    case winrt::Windows::System::VirtualKey::PageUp:
      args.Handled(pageUp(true));
      break;
    case winrt::Windows::System::VirtualKey::Up:
      args.Handled(lineUp(true));
      break;
    case winrt::Windows::System::VirtualKey::Down:
      args.Handled(lineDown(true));
      break;
    case winrt::Windows::System::VirtualKey::Left:
      args.Handled(lineLeft(true));
      break;
    case winrt::Windows::System::VirtualKey::Right:
      args.Handled(lineRight(true));
      break;
  }
}

bool ScrollViewComponentView::scrollToEnd(bool animate) noexcept {
  if ((((m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor) -
       m_scrollVisual.ScrollPosition().y) < 1.0f) {
    return false;
  }

  auto x = (m_contentSize.width - m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor;
  auto y = (m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor;
  m_scrollVisual.TryUpdatePosition({static_cast<float>(x), static_cast<float>(y), 0.0f}, animate);
  return true;
}

bool ScrollViewComponentView::scrollToStart(bool animate) noexcept {
  m_scrollVisual.TryUpdatePosition({0.0f, 0.0f, 0.0f}, animate);
  return true;
}

bool ScrollViewComponentView::pageUp(bool animate) noexcept {
  return scrollUp(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::pageDown(bool animate) noexcept {
  return scrollDown(m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineUp(bool animate) noexcept {
  return scrollUp(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineDown(bool animate) noexcept {
  return scrollDown(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineLeft(bool animate) noexcept {
  return scrollLeft(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::lineRight(bool animate) noexcept {
  return scrollRight(c_scrollerLineDelta * m_layoutMetrics.pointScaleFactor, animate);
}

bool ScrollViewComponentView::scrollDown(float delta, bool animate) noexcept {
  if (((m_contentSize.height - m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor) -
          m_scrollVisual.ScrollPosition().y <
      1.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({0, delta, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollUp(float delta, bool animate) noexcept {
  if (m_scrollVisual.ScrollPosition().y <= 0.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({0, -delta, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollLeft(float delta, bool animate) noexcept {
  if (m_scrollVisual.ScrollPosition().x <= 0.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({delta, 0, 0}, animate);
  return true;
}

bool ScrollViewComponentView::scrollRight(float delta, bool animate) noexcept {
  if (((m_contentSize.width - m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor) -
          m_scrollVisual.ScrollPosition().x <
      1.0f) {
    return false;
  }

  m_scrollVisual.ScrollBy({delta, 0, 0}, animate);
  return true;
}

void ScrollViewComponentView::handleCommand(std::string const &commandName, folly::dynamic const &arg) noexcept {
  if (commandName == "scrollTo") {
    auto x = arg[0].asDouble() * m_layoutMetrics.pointScaleFactor;
    auto y = arg[1].asDouble() * m_layoutMetrics.pointScaleFactor;
    auto animate = arg[2].asBool();
    m_scrollVisual.TryUpdatePosition({static_cast<float>(x), static_cast<float>(y), 0.0f}, animate);
  } else if (commandName == "flashScrollIndicators") {
    // No-op for now
  } else if (commandName == "scrollToEnd") {
    auto animate = arg[0].asBool();
    scrollToEnd(animate);
  } else if (commandName == "zoomToRect") {
    // No-op for now
  } else {
    Super::handleCommand(commandName, arg);
  }
}

void ScrollViewComponentView::StartBringIntoView(BringIntoViewOptions &&options) noexcept {
  RECT rc{getClientRect()};

  if (!options.TargetRect) {
    Super::StartBringIntoView(std::move(options));
    return;
  }

  bool needsScroll = false;
  float scrollToVertical = m_scrollVisual.ScrollPosition().y;
  float scrollToHorizontal = m_scrollVisual.ScrollPosition().x;
  float viewerHeight = m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor;
  float viewerWidth = m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor;
  float targetBottom = options.TargetRect->origin.y + options.TargetRect->size.height;
  float targetRight = options.TargetRect->origin.x + options.TargetRect->size.width;

  if (isnan(options.VerticalAlignmentRatio)) {
    // Scroll Down
    if (targetBottom > (m_scrollVisual.ScrollPosition().y + viewerHeight) &&
        options.TargetRect->origin.y > m_scrollVisual.ScrollPosition().y) {
      needsScroll = true;
      if (options.TargetRect->size.height > viewerHeight) {
        scrollToVertical = options.TargetRect->origin.y + options.VerticalOffset;
      } else {
        scrollToVertical = (targetBottom - viewerHeight) + options.VerticalOffset;
      }
      // Scroll Up
    } else if (
        options.TargetRect->origin.y < m_scrollVisual.ScrollPosition().y &&
        targetBottom < (m_scrollVisual.ScrollPosition().y + viewerHeight)) {
      needsScroll = true;
      if (options.TargetRect->size.height > viewerHeight) {
        scrollToVertical = targetBottom - viewerHeight - options.VerticalOffset;
      } else {
        scrollToVertical = options.TargetRect->origin.y - options.VerticalOffset;
      }
    }
  } else {
    needsScroll = true;
    scrollToVertical = options.TargetRect->getMidY() - (viewerHeight * options.VerticalAlignmentRatio);
  }

  if (isnan(options.HorizontalAlignmentRatio)) {
    // Scroll Right
    if (targetRight > (m_scrollVisual.ScrollPosition().x + viewerWidth) &&
        options.TargetRect->origin.x > m_scrollVisual.ScrollPosition().x) {
      needsScroll = true;
      if (options.TargetRect->size.width > viewerWidth) {
        scrollToHorizontal = options.TargetRect->origin.x + options.HorizontalOffset;
      } else {
        scrollToHorizontal = (targetRight - viewerWidth) + options.HorizontalOffset;
      }
      // Scroll Left
    } else if (
        options.TargetRect->origin.x < m_scrollVisual.ScrollPosition().x &&
        targetRight < (m_scrollVisual.ScrollPosition().x + viewerWidth)) {
      needsScroll = true;
      if (options.TargetRect->size.width > viewerWidth) {
        scrollToHorizontal = targetRight - viewerWidth - options.HorizontalOffset;
      } else {
        scrollToHorizontal = options.TargetRect->origin.x - options.HorizontalOffset;
      }
    }
  } else {
    needsScroll = true;
    scrollToHorizontal = options.TargetRect->getMidX() - (viewerWidth * options.HorizontalAlignmentRatio);
  }

  if (needsScroll) {
    m_scrollVisual.TryUpdatePosition(
        {static_cast<float>(scrollToHorizontal), static_cast<float>(scrollToVertical), 0.0f}, options.AnimationDesired);
  }

  if (m_parent) {
    options.TargetRect->origin.y += m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor;
    options.TargetRect->origin.x += m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor;

    options.TargetRect->origin.y -= m_scrollVisual.ScrollPosition().y;
    options.TargetRect->origin.x -= m_scrollVisual.ScrollPosition().x;

    facebook::react::Rect viewerRect = {
        {m_layoutMetrics.frame.origin.x * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.origin.y * m_layoutMetrics.pointScaleFactor},
        {m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor,
         m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor}};

    options.TargetRect = facebook::react::Rect::intersect(viewerRect, options.TargetRect.value());

    m_parent->StartBringIntoView(std::move(options));
  }
}

void ScrollViewComponentView::ensureVisual() noexcept {
  if (!m_visual) {
    m_visual = m_compContext.CreateSpriteVisual();
    m_scrollVisual = m_compContext.CreateScrollerVisual();
    m_visual.InsertAt(m_scrollVisual, 0);
    m_scrollPositionChangedRevoker = m_scrollVisual.ScrollPositionChanged(
        winrt::auto_revoke,
        [this](
            winrt::IInspectable const & /*sender*/,
            winrt::Microsoft::ReactNative::Composition::IScrollPositionChangedArgs const &args) {
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
    OuterVisual().InsertAt(m_visual, 0);
  }
}

facebook::react::Tag ScrollViewComponentView::hitTest(
    facebook::react::Point pt,
    facebook::react::Point &localPt,
    bool ignorePointerEvents) const noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_scrollVisual.ScrollPosition().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_scrollVisual.ScrollPosition().y / m_layoutMetrics.pointScaleFactor};

  facebook::react::Tag targetTag;
  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxNone) &&
      std::any_of(m_children.rbegin(), m_children.rend(), [&targetTag, &ptContent, &localPt](auto child) {
        targetTag = static_cast<const CompositionBaseComponentView *>(child)->hitTest(ptContent, localPt);
        return targetTag != -1;
      }))
    return targetTag;

  if ((ignorePointerEvents || m_props->pointerEvents == facebook::react::PointerEventsMode::Auto ||
       m_props->pointerEvents == facebook::react::PointerEventsMode::BoxOnly) &&
      ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptViewport;
    return this->tag();
  }

  return -1;
}

winrt::Microsoft::ReactNative::Composition::IVisual ScrollViewComponentView::Visual() const noexcept {
  return m_visual;
}

std::string ScrollViewComponentView::DefaultControlType() const noexcept {
  return "scrollbar";
}

} // namespace Microsoft::ReactNative
