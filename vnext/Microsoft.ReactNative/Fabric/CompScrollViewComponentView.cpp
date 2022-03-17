
// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include "CompScrollViewComponentView.h"

#include <UI.Xaml.Controls.h>
#include <Utils/ValueUtils.h>

#pragma warning(push)
#pragma warning(disable : 4305)
#include <react/renderer/components/scrollview/ScrollViewShadowNode.h>
#pragma warning(pop)

#include <windows.ui.composition.interop.h>

#include <unicode.h>

namespace Microsoft::ReactNative {

CompScrollViewComponentView::CompScrollViewComponentView() {
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
CompScrollViewComponentView::supplementalComponentDescriptorProviders() noexcept {
  return {};
}

void CompScrollViewComponentView::mountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  ensureVisual();

  m_children.insert(std::next(m_children.begin(), index), &childComponentView);
  const_cast<IComponentView &>(childComponentView).parent(this);

  auto containerChildren = m_contentVisual.Children();
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

void CompScrollViewComponentView::unmountChildComponentView(
    const IComponentView &childComponentView,
    uint32_t index) noexcept {
  m_children.erase(std::next(m_children.begin(), index));

  auto containerChildren = m_contentVisual.Children();
  containerChildren.Remove(static_cast<const CompBaseComponentView &>(childComponentView).Visual());
  const_cast<IComponentView &>(childComponentView).parent(nullptr);
}

void CompScrollViewComponentView::updateProps(
    facebook::react::Props::Shared const &props,
    facebook::react::Props::Shared const &oldProps) noexcept {
  const auto &newViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(props);
  const auto &oldViewProps = *std::static_pointer_cast<const facebook::react::ScrollViewProps>(oldProps);

  if (!oldProps || oldViewProps.backgroundColor != newViewProps.backgroundColor) {
    if (newViewProps.backgroundColor) {
      auto brush = m_compositor.CreateColorBrush((*newViewProps.backgroundColor).m_color);
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(brush);
    } else {
      auto brush = m_compositor.CreateColorBrush({0, 0, 0, 0});
      m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(nullptr);
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
  // m_props = std::static_pointer_cast<facebook::react::TextProps const>(props);
}

void CompScrollViewComponentView::updateEventEmitter(
    facebook::react::EventEmitter::Shared const &eventEmitter) noexcept {}
void CompScrollViewComponentView::updateState(
    facebook::react::State::Shared const &state,
    facebook::react::State::Shared const &oldState) noexcept {
  const auto &newState = *std::static_pointer_cast<facebook::react::ScrollViewShadowNode::ConcreteState const>(state);

  auto contentSize = newState.getData().getContentSize();
  m_contentVisual.Size(
      {std::max(contentSize.width, m_layoutMetrics.frame.size.width) * m_layoutMetrics.pointScaleFactor,
       std::max(contentSize.height, m_layoutMetrics.frame.size.height) * m_layoutMetrics.pointScaleFactor});

  updateInteractionMaxPosition();
}

void CompScrollViewComponentView::updateLayoutMetrics(
    facebook::react::LayoutMetrics const &layoutMetrics,
    facebook::react::LayoutMetrics const &oldLayoutMetrics) noexcept {
  // Set Position & Size Properties
  ensureVisual();

  if ((layoutMetrics.displayType != m_layoutMetrics.displayType)) {
    m_visual.IsVisible(layoutMetrics.displayType != facebook::react::DisplayType::None);
  }

  // m_needsBorderUpdate = true;
  m_layoutMetrics = layoutMetrics;

  m_visual.Size(
      {layoutMetrics.frame.size.width * layoutMetrics.pointScaleFactor,
       layoutMetrics.frame.size.height * layoutMetrics.pointScaleFactor});
  m_visual.Offset({
      layoutMetrics.frame.origin.x * layoutMetrics.pointScaleFactor,
      layoutMetrics.frame.origin.y * layoutMetrics.pointScaleFactor,
      0.0f,
  });
  updateInteractionMaxPosition();
}

void CompScrollViewComponentView::updateInteractionMaxPosition() noexcept {
  m_interactionTracker.MaxPosition(
      {m_contentVisual.Size().x - (m_layoutMetrics.frame.size.width * m_layoutMetrics.pointScaleFactor),
       m_contentVisual.Size().y - (m_layoutMetrics.frame.size.height * m_layoutMetrics.pointScaleFactor),
       0.0});
}

void CompScrollViewComponentView::finalizeUpdates(RNComponentViewUpdateMask updateMask) noexcept {
  // m_element.FinalizeProperties();
}
void CompScrollViewComponentView::prepareForRecycle() noexcept {}
facebook::react::SharedProps CompScrollViewComponentView::props() noexcept {
  assert(false);
  return {};
}

const winrt::Windows::UI::Composition::ContainerVisual CompScrollViewComponentView::ContentVisual() const noexcept {
  return m_contentVisual;
}

CompScrollViewComponentView::ScrollInteractionTrackerOwner::ScrollInteractionTrackerOwner(
    CompScrollViewComponentView *outer)
    : m_outer(outer) {}

void CompScrollViewComponentView::ScrollInteractionTrackerOwner::CustomAnimationStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerCustomAnimationStateEnteredArgs args) noexcept {}
void CompScrollViewComponentView::ScrollInteractionTrackerOwner::IdleStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerIdleStateEnteredArgs args) noexcept {}
void CompScrollViewComponentView::ScrollInteractionTrackerOwner::InertiaStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInertiaStateEnteredArgs args) noexcept {}
void CompScrollViewComponentView::ScrollInteractionTrackerOwner::InteractingStateEntered(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerInteractingStateEnteredArgs args) noexcept {}
void CompScrollViewComponentView::ScrollInteractionTrackerOwner::RequestIgnored(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerRequestIgnoredArgs args) noexcept {}

void CompScrollViewComponentView::ScrollInteractionTrackerOwner::ValuesChanged(
    winrt::Windows::UI::Composition::Interactions::InteractionTracker sender,
    winrt::Windows::UI::Composition::Interactions::InteractionTrackerValuesChangedArgs args) noexcept {
  facebook::react::ScrollViewMetrics scrollMetrics;
  scrollMetrics.containerSize.height = m_outer->Visual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
  scrollMetrics.containerSize.width = m_outer->Visual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;
  scrollMetrics.contentOffset.x = args.Position().x / m_outer->m_layoutMetrics.pointScaleFactor;
  scrollMetrics.contentOffset.y = args.Position().y / m_outer->m_layoutMetrics.pointScaleFactor;
  scrollMetrics.zoomScale = m_outer->ContentVisual().Scale().x;
  scrollMetrics.contentSize.height = m_outer->ContentVisual().Size().y / m_outer->m_layoutMetrics.pointScaleFactor;
  scrollMetrics.contentSize.width = m_outer->ContentVisual().Size().x / m_outer->m_layoutMetrics.pointScaleFactor;

  /*
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
  */

  auto eventEmitter = m_outer->GetEventEmitter();
  if (eventEmitter) {
    std::static_pointer_cast<facebook::react::ScrollViewEventEmitter const>(eventEmitter)->onScroll(scrollMetrics);
  }
}

/*
void CompScrollViewComponentView::OnPointerDown(const winrt::Windows::UI::Input::PointerPoint &pp) noexcept {
  m_visualInteractionSource.TryRedirectForManipulation(pp);
}
*/

bool CompScrollViewComponentView::ScrollWheel(facebook::react::Point pt, int32_t delta) noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_interactionTracker.Position().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_interactionTracker.Position().y / m_layoutMetrics.pointScaleFactor};

  if (std::any_of(m_children.rbegin(), m_children.rend(), [ptContent, delta](auto child) {
        return const_cast<CompBaseComponentView *>(static_cast<const CompBaseComponentView *>(child))
            ->ScrollWheel(ptContent, delta);
      }))
    return true;

  if (ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    m_interactionTracker.TryUpdatePositionBy({0, static_cast<float>(-delta), 0});
    return true;
  }

  return false;
}

void CompScrollViewComponentView::ensureVisual() noexcept {
  assert(m_compositor);
  if (!m_visual) {
    m_visual = m_compositor.CreateSpriteVisual();

    m_contentVisual = m_compositor.CreateSpriteVisual();

    auto brush = m_compositor.CreateColorBrush({0, 0, 0, 0}); // transparent brush so that hit testing works
    m_visual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(brush);
    m_contentVisual.as<winrt::Windows::UI::Composition::SpriteVisual>().Brush(brush);

    m_visual.as<winrt::Windows::UI::Composition::ContainerVisual>().Children().InsertAtTop(m_contentVisual);

    m_visual.Clip(m_compositor.CreateInsetClip(0, 0, 0, 0));

    m_interactionTracker = winrt::Windows::UI::Composition::Interactions::InteractionTracker::CreateWithOwner(
        m_compositor, winrt::make<ScrollInteractionTrackerOwner>(this));

    m_interactionTracker.MinScale(1.0);
    m_interactionTracker.MaxScale(1.0);

    m_visualInteractionSource =
        winrt::Windows::UI::Composition::Interactions::VisualInteractionSource::Create(m_visual);

    m_visualInteractionSource.PositionXSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.PositionYSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::EnabledWithInertia);
    m_visualInteractionSource.ScaleSourceMode(
        winrt::Windows::UI::Composition::Interactions::InteractionSourceMode::Disabled);

    m_visualInteractionSource.ManipulationRedirectionMode(
        winrt::Windows::UI::Composition::Interactions::VisualInteractionSourceRedirectionMode::
            CapableTouchpadAndPointerWheel);
    m_interactionTracker.InteractionSources().Add(m_visualInteractionSource);

    auto positionExpression = m_compositor.CreateExpressionAnimation(L"-tracker.Position");
    positionExpression.SetReferenceParameter(L"tracker", m_interactionTracker);
    m_contentVisual.StartAnimation(L"Offset", positionExpression);
  }
}

facebook::react::Tag CompScrollViewComponentView::hitTest(facebook::react::Point pt, facebook::react::Point &localPt)
    const noexcept {
  facebook::react::Point ptViewport{pt.x - m_layoutMetrics.frame.origin.x, pt.y - m_layoutMetrics.frame.origin.y};

  facebook::react::Point ptContent{
      ptViewport.x + m_interactionTracker.Position().x / m_layoutMetrics.pointScaleFactor,
      ptViewport.y + m_interactionTracker.Position().y / m_layoutMetrics.pointScaleFactor};

  facebook::react::Tag tag;
  if (std::any_of(m_children.rbegin(), m_children.rend(), [&tag, &ptContent, &localPt](auto child) {
        tag = static_cast<const CompBaseComponentView *>(child)->hitTest(ptContent, localPt);
        return tag != -1;
      }))
    return tag;

  if (ptViewport.x >= 0 && ptViewport.x <= m_layoutMetrics.frame.size.width && ptViewport.y >= 0 &&
      ptViewport.y <= m_layoutMetrics.frame.size.height) {
    localPt = ptViewport;
    return Tag();
  }

  return -1;
}

const winrt::Windows::UI::Composition::Visual CompScrollViewComponentView::Visual() const noexcept {
  return m_visual;
}

} // namespace Microsoft::ReactNative
