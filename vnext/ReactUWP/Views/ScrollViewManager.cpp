// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollViewManager.h"
#include <Views/ShadowNodeBase.h>
#include "Impl/ScrollViewUWPImplementation.h"

namespace react { namespace uwp {

enum class ScrollViewCommands
{
  ScrollTo = 1,
  ScrollToEnd,
};

class ScrollViewShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  ScrollViewShadowNode();
  void dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs) override;
  void createView() override;
  void updateProperties(const folly::dynamic&& props) override;

private:
  template <typename T> std::tuple<bool, T> getPropertyAndValidity(folly::dynamic propertyValue, T defaultValue);

  float m_zoomFactor{ 1.0f };

  winrt::FrameworkElement::SizeChanged_revoker m_scrollViewerSizeChangedRevoker{};
  winrt::FrameworkElement::SizeChanged_revoker m_contentSizeChangedRevoker{};
  winrt::ScrollViewer::ViewChanged_revoker m_scrollViewerViewChangedRevoker{};
};

ScrollViewShadowNode::ScrollViewShadowNode()
{
}

void ScrollViewShadowNode::dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs)
{
  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  switch (commandId)
  {
    case static_cast<int64_t>(ScrollViewCommands::ScrollTo) :
    {
      double x = commandArgs[0].asDouble();
      double y = commandArgs[1].asDouble();
      bool animated = commandArgs[2].asBool();
      scrollViewer.ChangeView(x, y, nullptr, !animated/*disableAnimation*/);
      break;
    }
    case static_cast<int64_t>(ScrollViewCommands::ScrollToEnd) :
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

void ScrollViewShadowNode::createView()
{
  Super::createView();

  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  const auto scrollViewUWPImplementation = new ScrollViewUWPImplementation(scrollViewer);

  m_scrollViewerSizeChangedRevoker = scrollViewer.SizeChanged(winrt::auto_revoke,
    [scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation->UpdateScrollableSize();
    });

  m_scrollViewerViewChangedRevoker = scrollViewer.ViewChanged(winrt::auto_revoke,
    [this, scrollViewUWPImplementation](const auto & sender, const auto & args)
    {
      const auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();
      const auto zoomFactor = scrollViewerNotNull.ZoomFactor();
      if (m_zoomFactor != zoomFactor)
      {
        m_zoomFactor = zoomFactor;
        scrollViewUWPImplementation->UpdateScrollableSize();
      }
    });

  m_contentSizeChangedRevoker = scrollViewUWPImplementation->ScrollViewerSnapPointManager()->SizeChanged(winrt::auto_revoke,
    [this, scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation->UpdateScrollableSize();
    });
}

void ScrollViewShadowNode::updateProperties(const folly::dynamic&& reactDiffMap)
{
  m_updating = true;

  const auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  for (const auto& pair : reactDiffMap.items())
  {
    const auto propertyName = pair.first;
    const auto propertyValue = pair.second;
    if (propertyName == "horizontal")
    {
      const auto [valid, horizontal] = getPropertyAndValidity(propertyValue, false);
      if (valid)
      {
        scrollViewer.HorizontalScrollMode(horizontal ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
        scrollViewer.VerticalScrollMode(horizontal ? winrt::ScrollMode::Disabled : winrt::ScrollMode::Auto);
        ScrollViewUWPImplementation(scrollViewer).SetHorizontal(horizontal);
      }
    }
    else if (propertyName == "showsHorizontalScrollIndicator")
    {
      const auto [valid, showsHorizontalScrollIndicator] = getPropertyAndValidity(propertyValue, true);
      if (valid)
      {
        scrollViewer.HorizontalScrollBarVisibility(showsHorizontalScrollIndicator ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    }
    else if (propertyName == "showsVerticalScrollIndicator")
    {
      const auto [valid, showsVerticalScrollIndicator] = getPropertyAndValidity(propertyValue, true);
      if (valid)
      {
        scrollViewer.VerticalScrollBarVisibility(showsVerticalScrollIndicator ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
      }
    }
    else if (propertyName == "minimumZoomScale")
    {
      const auto [valid, minimumZoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid)
      {
        scrollViewer.MinZoomFactor(static_cast<float>(minimumZoomScale));
      }
    }
    else if (propertyName == "maximumZoomScale")
    {
      const auto [valid, maximumZoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid)
      {
        scrollViewer.MaxZoomFactor(static_cast<float>(maximumZoomScale));
      }
    }
    else if (propertyName == "zoomScale")
    {
      const auto [valid, zoomScale] = getPropertyAndValidity(propertyValue, 1.0);
      if (valid)
      {
        scrollViewer.ChangeView(nullptr, nullptr, static_cast<float>(zoomScale));
      }
    }
    else if (propertyName == "snapToInterval")
    {
      const auto [valid, snapToInterval] = getPropertyAndValidity(propertyValue, 0.0);
      if (valid)
      {
        ScrollViewUWPImplementation(scrollViewer).SnapToInterval(static_cast<float>(snapToInterval));
      }
    }
    else if (propertyName == "snapToOffsets")
    {
      const auto [valid, snapToOffsets] = getPropertyAndValidity(propertyValue, winrt::single_threaded_vector<float>());
      if (valid)
      {
        ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(snapToOffsets.GetView());
      }
    }
    else if (propertyName == "snapToAlignment")
    {
      const auto [valid, snapToAlignment] = getPropertyAndValidity(propertyValue, winrt::SnapPointsAlignment::Near);
      if (valid)
      {
        scrollViewer.HorizontalSnapPointsAlignment(snapToAlignment);
        scrollViewer.VerticalSnapPointsAlignment(snapToAlignment);
      }
    }
  }

  Super::updateProperties(std::move(reactDiffMap));
  m_updating = false;
}

template <typename T>
std::tuple<bool, T> ScrollViewShadowNode::getPropertyAndValidity(folly::dynamic propertyValue, T defaultValue)
{
  if (propertyValue.isNull())
  {
    return std::make_tuple(true, defaultValue);
  }

  if constexpr (std::is_same<T, bool>())
  {
    if (propertyValue.isBool())
    {
      return std::make_tuple(true, propertyValue.asBool());
    }
  }

  if constexpr (std::is_same<T, double>())
  {
    if (propertyValue.isDouble())
    {
      return std::make_tuple(true, static_cast<T>(propertyValue.asDouble()));
    }
    if (propertyValue.isInt())
    {
      return std::make_tuple(true, static_cast<T>(propertyValue.asInt()));
    }
  }

  if constexpr (std::is_same<T, winrt::IVector<float>>())
  {
    if (propertyValue.isArray())
    {
      return std::make_tuple(
        true,
        [propertyValue]()
        {
          const auto vector = winrt::single_threaded_vector<float>();
          for (const auto val : propertyValue)
          {
            if (val.isNumber())
              vector.Append(static_cast<float>(val.asDouble()));
          }
          return vector;
        }());
    }
  }

  if constexpr (std::is_same<T, winrt::SnapPointsAlignment>())
  {
    if (propertyValue.isString())
    {
      return std::make_tuple(
        true,
        static_cast<T>([snapToAlignment = propertyValue.asString()]()
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
      }()));
    }
  }

  return std::make_tuple(false, defaultValue);
}












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

facebook::react::ShadowNode* ScrollViewManager::createShadow() const
{
  return new ScrollViewShadowNode();
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
  const auto scrollViewer = [this]()
  {
    const auto scrollViewer = winrt::ScrollViewer{};

    scrollViewer.HorizontalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
    scrollViewer.VerticalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
    scrollViewer.VerticalSnapPointsAlignment(winrt::SnapPointsAlignment::Near);
    scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::Mandatory);
    scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::Mandatory);

    return scrollViewer;
  }();

  const auto scrollViewUWPImplementation = [scrollViewer]()
  {
    ScrollViewUWPImplementation::ConvertScrollViewer(scrollViewer);
    return new ScrollViewUWPImplementation(scrollViewer);
  }();
  m_isScrollingFromInertia[tag] = false;
  m_isScrolling[tag] = false;
  AddHandlers(scrollViewer, tag);
    
  return scrollViewer;
}

void ScrollViewManager::AddHandlers(const winrt::ScrollViewer& scrollViewer, int64_t tag)
{
  m_scrollViewerViewChangingRevokers[tag] = scrollViewer.ViewChanging(winrt::auto_revoke,
    [this, tag](const auto & sender, const auto & args)
    {
      const auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();


      //If we are transitioning to inertial scrolling.
      if (m_isScrolling[tag] && !m_isScrollingFromInertia[tag] && args.IsInertial())
      {
        m_isScrollingFromInertia[tag] = true;

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
    [this, tag](const auto & sender, const auto&)
    {
      m_isScrolling[tag] = true;
      const auto scrollViewer = sender.as<winrt::ScrollViewer>();
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
    [this, tag](const auto & sender, const auto&)
    {
      const auto scrollViewer = sender.as<winrt::ScrollViewer>();
      if (m_isScrollingFromInertia[tag])
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

      m_isScrolling[tag] = false;
      m_isScrollingFromInertia[tag] = false;
    });
}

void ScrollViewManager::EmitScrollEvent(
  const winrt::ScrollViewer& scrollViewer,
  int64_t tag,
  const char* eventName,
  double x, double y, double zoom)
{
  const auto instance = m_wkReactInstance.lock();
  if (instance == nullptr)
    return;

  const auto scrollViewerNotNull = scrollViewer;

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
  if (parent && child && index >= 0)
  {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).AddView(child, static_cast<uint32_t>(index));
    }
  }
}

void ScrollViewManager::RemoveAllChildren(XamlView parent)
{
  if (parent)
  {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).RemoveAllChildren();
    }
  }
}

void ScrollViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  if (parent && index >= 0)
  {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).RemoveChildAt(static_cast<uint32_t>(index));
    }
  }
}

void ScrollViewManager::SnapToInterval(XamlView parent, float interval)
{
  if (parent)
  {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).SnapToInterval(interval);
    }
  }
}

void ScrollViewManager::SnapToOffsets(XamlView parent, const winrt::IVectorView<float>& offsets)
{
  if (parent)
  {
    if (const auto scrollViewer = parent.as<winrt::ScrollViewer>())
    {
      ScrollViewUWPImplementation(scrollViewer).SnapToOffsets(offsets);
    }
  }
}

} }
