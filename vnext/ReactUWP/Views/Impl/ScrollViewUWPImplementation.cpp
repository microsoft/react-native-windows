#include "pch.h"

#include "ScrollViewUWPImplementation.h"

namespace react {
namespace uwp {

ScrollViewUWPImplementation::ScrollViewUWPImplementation(const winrt::ScrollViewer &scrollViewer) {
  assert(scrollViewer);
  const auto scrollViewContent = scrollViewer.Content();
  assert(scrollViewContent);
  const auto snapPointManager = scrollViewContent.as<SnapPointManagingContentControl>();
  assert(snapPointManager);

  m_scrollViewer = winrt::make_weak(scrollViewer);
}

void ScrollViewUWPImplementation::SetHorizontal(bool horizontal) {
  ScrollViewerSnapPointManager()->SetHorizontal(horizontal);
}

void ScrollViewUWPImplementation::SnapToInterval(float interval) {
  ScrollViewerSnapPointManager()->SnapToInterval(interval);
}

void ScrollViewUWPImplementation::PagingEnabled(bool pagingEnabled) {
  ScrollViewerSnapPointManager()->PagingEnabled(pagingEnabled);
}

void ScrollViewUWPImplementation::SnapToStart(bool snapToStart) {
  ScrollViewerSnapPointManager()->SnapToStart(snapToStart);
}

void ScrollViewUWPImplementation::SnapToEnd(bool snapToEnd) {
  ScrollViewerSnapPointManager()->SnapToEnd(snapToEnd);
}

void ScrollViewUWPImplementation::SnapToOffsets(const winrt::IVectorView<float> &offsets) {
  ScrollViewerSnapPointManager()->SnapToOffsets(offsets);
}

void ScrollViewUWPImplementation::SnapPointAlignment(const winrt::SnapPointsAlignment alignment) {
  if (const auto scrollViewer = m_scrollViewer.get()) {
    scrollViewer.HorizontalSnapPointsAlignment(alignment);
    scrollViewer.VerticalSnapPointsAlignment(alignment);
    UpdateScrollableSize();
    ScrollViewerSnapPointManager()->NotifySnapPointsUpdated();
  }
}

void ScrollViewUWPImplementation::UpdateScrollableSize() const {
  if (const auto scrollViewer = m_scrollViewer.get()) {
    const auto zoomFactor = scrollViewer.ZoomFactor();
    const auto scaledViewportWidth = static_cast<float>(scrollViewer.ViewportWidth() / zoomFactor);
    const auto scaledviewportHeight = static_cast<float>(scrollViewer.ViewportHeight() / zoomFactor);
    const auto scrollViewContent = scrollViewer.Content().as<winrt::ContentControl>();
    const auto contentWidth = scrollViewContent.ActualWidth();
    const auto contentHeight = scrollViewContent.ActualHeight();

    ScrollViewerSnapPointManager()->SetViewportSize(scaledViewportWidth, scaledviewportHeight);

    switch (scrollViewer.HorizontalSnapPointsAlignment()) {
      case winrt::SnapPointsAlignment::Near:
        ScrollViewerSnapPointManager()->SetWidthBounds(0.0f, static_cast<float>(contentWidth) - scaledViewportWidth);
        break;
      case winrt::SnapPointsAlignment::Center:
        ScrollViewerSnapPointManager()->SetWidthBounds(
            scaledViewportWidth / 2.0f, static_cast<float>(contentWidth) - scaledViewportWidth / 2.0f);
        break;
      case winrt::SnapPointsAlignment::Far:
        ScrollViewerSnapPointManager()->SetWidthBounds(scaledViewportWidth, static_cast<float>(contentWidth));
        break;
    }

    switch (scrollViewer.VerticalSnapPointsAlignment()) {
      case winrt::SnapPointsAlignment::Near:
        ScrollViewerSnapPointManager()->SetHeightBounds(0.0f, static_cast<float>(contentHeight) - scaledviewportHeight);
        break;
      case winrt::SnapPointsAlignment::Center:
        ScrollViewerSnapPointManager()->SetHeightBounds(
            scaledviewportHeight / 2.0f, static_cast<float>(contentHeight) - scaledviewportHeight / 2.0f);
        break;
      case winrt::SnapPointsAlignment::Far:
        ScrollViewerSnapPointManager()->SetHeightBounds(
            static_cast<float>(scaledViewportWidth), static_cast<float>(contentHeight));
        break;
    }
  }
}

winrt::ScrollViewer ScrollViewUWPImplementation::ScrollViewer() const {
  return m_scrollViewer.get();
}

// privates //

winrt::com_ptr<SnapPointManagingContentControl> ScrollViewUWPImplementation::ScrollViewerSnapPointManager() const {
  if (const auto scrollViewer = ScrollViewer()) {
    if (const auto content = scrollViewer.Content()) {
      return content.as<SnapPointManagingContentControl>();
    }
  }
  return nullptr;
}

} // namespace uwp
} // namespace react
