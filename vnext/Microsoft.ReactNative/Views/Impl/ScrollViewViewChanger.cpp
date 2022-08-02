// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <UI.Xaml.Controls.h>
#include <Views/ViewViewManager.h>
#include "ScrollViewUWPImplementation.h"
#include "ScrollViewViewChanger.h"
#include "SnapPointManagingContentControl.h"

namespace Microsoft::ReactNative {

constexpr const double SCROLL_EPSILON = 1.0;

void ScrollViewViewChanger::Horizontal(bool horizontal) {
  m_horizontal = horizontal;
}

void ScrollViewViewChanger::Inverted(bool inverted) {
  m_inverted = inverted;
}

void ScrollViewViewChanger::ScrollToEnd(const xaml::Controls::ScrollViewer &scrollViewer, bool animated) {
  if (m_inverted) {
    UpdateScrollAnchoringEnabled(scrollViewer, false);
  }

  if (m_horizontal) {
    m_targetScrollToEndOffset = scrollViewer.ScrollableWidth();
    scrollViewer.ChangeView(m_targetScrollToEndOffset.value(), nullptr, nullptr, !animated);
  } else {
    m_targetScrollToEndOffset = scrollViewer.ScrollableHeight();
    scrollViewer.ChangeView(nullptr, m_targetScrollToEndOffset.value(), nullptr, !animated);
  }
}

void ScrollViewViewChanger::OnViewChanging(
    const xaml::Controls::ScrollViewer &scrollViewer,
    const xaml::Controls::ScrollViewerViewChangingEventArgs &args) {
  // For non-inverted views, a ScrollViewer.ViewChanging event always emits an `onScroll` event
  if (m_inverted) {
    // Do not update scroll anchoring during ScrollToEnd
    if (!IsScrollToEndActive()) {
      // For inverted views, we need to detect if we're scrolling to or away from the bottom edge to enable or disable
      // view anchoring
      const auto scrollingToEnd =
          IsScrollingToEnd(scrollViewer, args.NextView().HorizontalOffset(), args.NextView().VerticalOffset());
      UpdateScrollAnchoringEnabled(scrollViewer, !scrollingToEnd);
    } else if (!IsScrollingToEnd(scrollViewer, m_targetScrollToEndOffset.value(), m_targetScrollToEndOffset.value())) {
      // If we were previously in an active ScrollToEnd command, we may need to
      // restart the operation if the content size has changed
      ScrollToEnd(scrollViewer, true);
    }
  }
}

void ScrollViewViewChanger::OnViewChanged(
    const xaml::Controls::ScrollViewer &scrollViewer,
    const xaml::Controls::ScrollViewerViewChangedEventArgs &args) {
  // Stop tracking scroll-to-end once the ScrollView comes to rest
  if (!args.IsIntermediate()) {
    m_targetScrollToEndOffset = std::nullopt;
    if (m_inverted) {
      const auto scrolledToEnd =
          IsScrollingToEnd(scrollViewer, scrollViewer.HorizontalOffset(), scrollViewer.VerticalOffset());
      UpdateScrollAnchoringEnabled(scrollViewer, !scrolledToEnd);
    }
  }
}

void ScrollViewViewChanger::UpdateScrollAnchoringEnabled(
    const xaml::Controls::ScrollViewer &scrollViewer,
    bool enabled) {
  if (m_wasScrollAnchoringEnabled != enabled) {
    m_wasScrollAnchoringEnabled = enabled;
    ScrollViewUWPImplementation(scrollViewer).ContentAnchoringEnabled(enabled);
    const auto snapPointManager = scrollViewer.Content().as<SnapPointManagingContentControl>();
    auto panel = snapPointManager->Content().as<xaml::Controls::Panel>();
    for (auto child : panel.Children()) {
      const auto childElement = child.as<xaml::UIElement>();
      if (winrt::unbox_value<bool>(childElement.GetValue(ViewViewManager::CanBeScrollAnchorProperty()))) {
        if (enabled) {
          childElement.CanBeScrollAnchor(true);
        } else {
          childElement.ClearValue(xaml::UIElement::CanBeScrollAnchorProperty());
        }
      }
    }
  }
}

bool ScrollViewViewChanger::IsScrollingToEnd(
    const xaml::Controls::ScrollViewer &scrollViewer,
    double horizontalOffset,
    double verticalOffset) {
  return m_horizontal ? horizontalOffset > (scrollViewer.ScrollableWidth() - SCROLL_EPSILON)
                      : verticalOffset > (scrollViewer.ScrollableHeight() - SCROLL_EPSILON);
}

} // namespace Microsoft::ReactNative
