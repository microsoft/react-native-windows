// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollViewManager.h"
#include "Impl/ScrollViewUWPImplementation.h"

namespace react { namespace uwp {

enum class ScrollViewCommands
{
  ScrollTo = 1,
  ScrollToEnd,
};

ScrollViewManager::ScrollViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* ScrollViewManager::GetName() const
{
  return "RCTScrollView";
}

folly::dynamic ScrollViewManager::GetCommands() const
{
  auto commands = Super::GetCommands();
  commands.update(folly::dynamic::object
    ("scrollTo", static_cast<std::underlying_type_t<ScrollViewCommands>>(ScrollViewCommands::ScrollTo))
    ("scrollToEnd", static_cast<std::underlying_type_t<ScrollViewCommands>>(ScrollViewCommands::ScrollToEnd))
  );
  return commands;
}

folly::dynamic ScrollViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("horizontal", "boolean")
    ("showsHorizontalScrollIndicator", "boolean")
    ("showsVerticalScrollIndicator", "boolean")
    ("minimumZoomScale", "float")
    ("maximumZoomScale", "float")
    ("zoomScale", "float")
    ("snapToInterval", "float")
    ("snapToOffsets", "array")
    ("snapToAlignment", "number")
  );

  return props;
}

folly::dynamic ScrollViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topScrollBeginMomentum"] = folly::dynamic::object("registrationName", "onMomentumScrollBegin");
  directEvents["topScrollEndMomentum"] = folly::dynamic::object("registrationName", "onMomentumScrollEnd");
  directEvents["topScrollBeginDrag"] = folly::dynamic::object("registrationName", "onScrollBeginDrag");
  directEvents["topScrollEndDrag"] = folly::dynamic::object("registrationName", "onScrollEndDrag");
  directEvents["topScroll"] = folly::dynamic::object("registrationName", "onScroll");

  return directEvents;
}

XamlView ScrollViewManager::CreateViewCore(int64_t tag)
{
  auto scrollViewer = [this]()
  {
    auto scrollViewer = winrt::ScrollViewer{};

    scrollViewer.HorizontalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
    scrollViewer.VerticalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
    scrollViewer.VerticalSnapPointsAlignment(winrt::SnapPointsAlignment::Near);
    scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::Mandatory);
    scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::Mandatory);

    return scrollViewer;
  }();
  auto scrollViewUWPImplementation = [scrollViewer]()
  {
    ScrollViewUWPImplementation::ConvertScrollViewer(scrollViewer);
    return new ScrollViewUWPImplementation(scrollViewer);
  }();

  m_zoomFactors[tag] = scrollViewer.ZoomFactor();
  
  AddHandlers(scrollViewer, tag);

  m_scrollViewerSizeChangedRevokers[tag] = scrollViewer.SizeChanged(winrt::auto_revoke,
    [scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation->UpdateScrollableSize();
    });
  m_scrollViewerViewChangedRevokers[tag] = scrollViewer.ViewChanged(winrt::auto_revoke,
    [this, tag, scrollViewUWPImplementation](const auto & sender, const auto & args)
    {
      auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();
      auto zoomFactor = scrollViewerNotNull.ZoomFactor();
      if (m_zoomFactors[tag] != zoomFactor)
      {
        m_zoomFactors[tag] = zoomFactor;
        scrollViewUWPImplementation->UpdateScrollableSize();
      }
    });
  m_contentSizeChangedRevokers[tag] = scrollViewUWPImplementation->ScrollViewerSnapPointManager()->SizeChanged(winrt::auto_revoke,
    [scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation->UpdateScrollableSize();
    });
    
  return scrollViewer;
}

void ScrollViewManager::AddHandlers(const winrt::ScrollViewer& scrollViewer, int64_t tag)
{
  m_scrollViewerViewChangingRevokers[tag] = scrollViewer.ViewChanging(winrt::auto_revoke,
    [this, tag](const auto& sender, const auto& args)
    {
      auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();


      //If we are transitioning to inertial scrolling.
      if (m_isScrolling && !m_isScrollingFromInertia && args.IsInertial())
      {
        m_isScrollingFromInertia = true;

        EmitScrollEvent(
          scrollViewerNotNull,
          tag,
          "topScrollEndDrag",
          args.NextView().HorizontalOffset(),
          args.NextView().VerticalOffset(),
          args.NextView().ZoomFactor()
        );

        EmitScrollEvent(
          scrollViewerNotNull,
          tag,
          "topScrollBeginMomentum",
          args.NextView().HorizontalOffset(),
          args.NextView().VerticalOffset(),
          args.NextView().ZoomFactor()
        );
      }

      EmitScrollEvent(
        scrollViewerNotNull,
        tag,
        "topScroll",
        args.NextView().HorizontalOffset(),
        args.NextView().VerticalOffset(),
        args.NextView().ZoomFactor()
      );
    });

  m_scrollViewerDirectManipulationStartedRevokers[tag] = scrollViewer.DirectManipulationStarted(winrt::auto_revoke,
    [this, tag](const auto& sender, const auto&)
    {
      m_isScrolling = true;
      auto scrollViewer = sender.as<winrt::ScrollViewer>();
      EmitScrollEvent(
        scrollViewer,
        tag,
        "topScrollBeginDrag",
        scrollViewer.HorizontalOffset(),
        scrollViewer.VerticalOffset(),
        scrollViewer.ZoomFactor()
      );
    });

  m_scrollViewerDirectManipulationCompletedRevokers[tag] = scrollViewer.DirectManipulationCompleted(winrt::auto_revoke,
    [this, tag](const auto& sender, const auto&)
    {
      auto scrollViewer = sender.as<winrt::ScrollViewer>();
      if (m_isScrollingFromInertia)
      {
        EmitScrollEvent(
          scrollViewer,
          tag,
          "topScrollEndMomentum",
          scrollViewer.HorizontalOffset(),
          scrollViewer.VerticalOffset(),
          scrollViewer.ZoomFactor()
        );
      }
      else
      {
        EmitScrollEvent(
          scrollViewer,
          tag,
          "topScrollEndDrag",
          scrollViewer.HorizontalOffset(),
          scrollViewer.VerticalOffset(),
          scrollViewer.ZoomFactor()
        );
      }

      m_isScrolling = false;
      m_isScrollingFromInertia = false;
    });
}

void ScrollViewManager::EmitScrollEvent(
  const winrt::ScrollViewer& scrollViewer,
  int64_t tag,
  const char* eventName,
  double x, double y, double zoom)
{
  auto instance = m_wkReactInstance.lock();
  if (instance == nullptr)
    return;

  auto scrollViewerNotNull = scrollViewer;

  folly::dynamic offset = folly::dynamic::object
    ("x", x)
    ("y", y);

  folly::dynamic contentInset = folly::dynamic::object
    ("left", 0)
    ("top", 0)
    ("right", 0)
    ("bottom", 0);

  folly::dynamic contentSize = folly::dynamic::object
    ("width", scrollViewerNotNull.ExtentWidth())
    ("height", scrollViewerNotNull.ExtentHeight());

  folly::dynamic layoutSize = folly::dynamic::object
    ("width", scrollViewerNotNull.ActualWidth())
    ("height", scrollViewerNotNull.ActualHeight());

  folly::dynamic eventJson = folly::dynamic::object
    ("target", tag)
    ("responderIgnoreScroll", true)
    ("contentOffset", offset)
    ("contentInset", contentInset)
    ("contentSize", contentSize)
    ("layoutMeasurement", layoutSize)
    ("zoomScale", zoom);

  folly::dynamic params = folly::dynamic::array(tag, eventName, eventJson);
  instance->CallJsFunction("RCTEventEmitter", "receiveEvent", std::move(params));
}

void ScrollViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  if (parent && child)
  {
    if (auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).AddView(child, index);
    }
  }
}

void ScrollViewManager::RemoveAllChildren(XamlView parent)
{
  if (parent)
  {
    if (auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).RemoveAllChildren();
    }
  }
}

void ScrollViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  if (parent)
  {
    if (auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).RemoveChildAt(index);
    }
  }
}

void ScrollViewManager::SnapToInterval(XamlView parent, float interval)
{
  if (parent)
  {
    if (auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).SnapToInterval(interval);
    }
  }
}

void ScrollViewManager::SnapToOffsets(XamlView parent, const winrt::IVectorView<float>& offsets)
{
  if (parent)
  {
    if (auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(offsets);
    }
  }
}

void ScrollViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto scrollViewer = nodeToUpdate->GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  auto control = scrollViewer.as<winrt::Control>();
  for (auto& pair : reactDiffMap.items())
  {
    if (pair.first == "horizontal")
    {
      if (pair.second.isBool())
      {
        bool horiz = pair.second.getBool();

        scrollViewer.HorizontalScrollMode(horiz ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
        scrollViewer.VerticalScrollMode(horiz ? winrt::ScrollMode::Disabled : winrt::ScrollMode::Auto);
        ScrollViewUWPImplementation(scrollViewer).SetHorizontal(horiz);
      }
    }
    else if (pair.first == "showsHorizontalScrollIndicator")
    {
      if (pair.second.isBool())
      {
        scrollViewer.HorizontalScrollBarVisibility(pair.second.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    }
    else if (pair.first == "showsVerticalScrollIndicator")
    {
      if (pair.second.isBool())
      {
        scrollViewer.VerticalScrollBarVisibility(pair.second.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    }
    else if (pair.first == "minimumZoomScale")
    {
      if (pair.second.isNumber())
      {
        scrollViewer.MinZoomFactor(static_cast<float>(pair.second.asDouble()));
      }
    }
    else if (pair.first == "maximumZoomScale")
    {
      if (pair.second.isNumber())
      {
        scrollViewer.MaxZoomFactor(static_cast<float>(pair.second.asDouble()));
      }
    }
    else if (pair.first == "zoomScale")
    {
      if (pair.second.isNumber())
      {
        scrollViewer.ChangeView(nullptr, nullptr, static_cast<float>(pair.second.asDouble()));
      }
    }
    else if (pair.first == "snapToInterval")
    {
      if (pair.second.isNumber())
      {
        ScrollViewUWPImplementation(scrollViewer).SnapToInterval(pair.second.asDouble());
      }
    }
    else if (pair.first == "snapToOffsets")
    {
      if (pair.second.isArray())
      {
        auto vector = [offsets = pair.second]()
        {
          auto vector = winrt::single_threaded_vector<float>();
          for (auto val : offsets)
          {
            if (val.isNumber())
              vector.Append(val.asDouble());
          }
          return vector;
        }();
        ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(vector.GetView());
      }
    }
    else if (pair.first == "snapToAlignment")
    {
      if (pair.second.isString())
      {
        auto snapPointAlignment = [snapToAlignment = pair.second.asString()]()
        {
          if (snapToAlignment == "end")
          {
            return winrt::SnapPointsAlignment::Far;
          }
          else if (snapToAlignment == "center")
          {
            return winrt::SnapPointsAlignment::Center;
          }
          else
          {
            return winrt::SnapPointsAlignment::Near;
          }
        }();

        scrollViewer.HorizontalSnapPointsAlignment(snapPointAlignment);
        scrollViewer.VerticalSnapPointsAlignment(snapPointAlignment);
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

void ScrollViewManager::DispatchCommand(XamlView viewToUpdate, int64_t commandId, const folly::dynamic& commandArgs)
{
  auto scrollViewer = viewToUpdate.as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  switch (commandId)
  {
    case static_cast<int64_t>(ScrollViewCommands::ScrollTo):
    {
      double x = commandArgs[0].asDouble();
      double y = commandArgs[1].asDouble();
      bool animated = commandArgs[2].asBool();
      scrollViewer.ChangeView(x, y, nullptr, !animated/*disableAnimation*/);
      break;
    }
    case static_cast<int64_t>(ScrollViewCommands::ScrollToEnd):
    {
      bool animated = commandArgs[0].asBool();
      bool horiz = scrollViewer.HorizontalScrollMode() == winrt::ScrollMode::Auto;
      if (horiz)
        scrollViewer.ChangeView(scrollViewer.ScrollableWidth(), nullptr, nullptr, !animated/*disableAnimation*/);
      else
        scrollViewer.ChangeView(nullptr, scrollViewer.ScrollableHeight(), nullptr, !animated/*disableAnimation*/);
      break;
    }
  }
}

} }
