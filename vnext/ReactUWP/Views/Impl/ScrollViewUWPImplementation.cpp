#include "pch.h"

#include "ScrollViewUWPImplementation.h"

namespace react {
namespace uwp {

ScrollViewUWPImplementation::ScrollViewUWPImplementation(const winrt::ScrollViewer& scrollViewer)
{
  assert(scrollViewer);
  auto scrollViewContent = scrollViewer.Content();
  assert(scrollViewContent);
  auto snapPointManager = scrollViewContent.as<SnapPointManagingContentControl>();
  assert(snapPointManager);
  assert(snapPointManager->Content().as<winrt::StackPanel>());

  m_scrollViewer = winrt::make_weak(scrollViewer);
}

void ScrollViewUWPImplementation::ConvertScrollViewer(const winrt::ScrollViewer& scrollViewer)
{
  if (scrollViewer)
  {
    auto snapPointManager = new SnapPointManagingContentControl();
    snapPointManager->Content(winrt::StackPanel{});

    scrollViewer.Content(*snapPointManager);
  }
}

void ScrollViewUWPImplementation::AddView(const XamlView& child, int64_t index)
{
  auto contentCollection = ScrollViewerContent().Children();
  assert(index <= contentCollection.Size());
  contentCollection.InsertAt(index, child.as<winrt::UIElement>());
}

void ScrollViewUWPImplementation::RemoveAllChildren()
{
  ScrollViewerContent().Children().Clear();
}

void ScrollViewUWPImplementation::RemoveChildAt(int64_t index)
{
  auto contentCollection = ScrollViewerContent().Children();
  assert(index < contentCollection.Size());
  contentCollection.RemoveAt(index);
}

void ScrollViewUWPImplementation::SetHorizontal(bool horizontal)
{
  ScrollViewerSnapPointManager()->SetHorizontal(horizontal);
}

void ScrollViewUWPImplementation::SnapToInterval(float interval)
{
  ScrollViewerSnapPointManager()->SnapToInterval(interval);
}

void ScrollViewUWPImplementation::SnapToOffsets(const winrt::IVectorView<float>& offsets)
{
  ScrollViewerSnapPointManager()->SnapToOffsets(offsets);
}

void ScrollViewUWPImplementation::UpdateScrollableSize()
{
  if (auto scrollViewer = m_scrollViewer.get())
  {
    switch (scrollViewer.HorizontalSnapPointsAlignment())
    {
    case winrt::SnapPointsAlignment::Near:
      ScrollViewerSnapPointManager()->SetWidthBounds(0, scrollViewer.ScrollableWidth());
      break;
    case winrt::SnapPointsAlignment::Center:
      ScrollViewerSnapPointManager()->SetWidthBounds((scrollViewer.ActualWidth() / 2), scrollViewer.ScrollableWidth() + (scrollViewer.ActualWidth() / 2));
      break;
    case winrt::SnapPointsAlignment::Far:
      ScrollViewerSnapPointManager()->SetWidthBounds(scrollViewer.ActualWidth(), scrollViewer.ScrollableWidth() + scrollViewer.ActualWidth());
      break;
    }

    switch (scrollViewer.VerticalSnapPointsAlignment())
    {
    case winrt::SnapPointsAlignment::Near:
      ScrollViewerSnapPointManager()->SetHeightBounds(0, scrollViewer.ScrollableWidth());
      break;
    case winrt::SnapPointsAlignment::Center:
      ScrollViewerSnapPointManager()->SetHeightBounds((scrollViewer.ActualHeight() / 2) + 1, scrollViewer.ScrollableWidth() + (scrollViewer.ActualHeight() / 2));
      break;
    case winrt::SnapPointsAlignment::Far:
      ScrollViewerSnapPointManager()->SetHeightBounds(scrollViewer.ActualHeight(), scrollViewer.ScrollableWidth() + scrollViewer.ActualHeight());
      break;
    }
  }
}

winrt::ScrollViewer ScrollViewUWPImplementation::ScrollViewer()
{
  return m_scrollViewer.get();
}

// privates //

winrt::com_ptr<SnapPointManagingContentControl>ScrollViewUWPImplementation::ScrollViewerSnapPointManager()
{
  if (auto scrollViewer = m_scrollViewer.get())
  {
    if (auto content = scrollViewer.Content())
    {
      return content.as<SnapPointManagingContentControl>();
    }
  }
  return nullptr;
}

winrt::StackPanel ScrollViewUWPImplementation::ScrollViewerContent()
{
  if (auto snapPointManagingContentControl = ScrollViewerSnapPointManager())
  {
    if (auto content = snapPointManagingContentControl->Content())
    {
      return content.as<winrt::StackPanel>();
    }
  }
  return nullptr;
}

} }
