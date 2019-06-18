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
  void AddHandlers(const winrt::ScrollViewer& scrollViewer);
  void EmitScrollEvent(
    const winrt::ScrollViewer& scrollViewer,
    int64_t tag,
    const char* eventName,
    double x, double y, double zoom);
  template <typename T> std::tuple<bool, T> getPropertyAndValidity(folly::dynamic propertyValue, T defaultValue);
  void SetScrollMode(const winrt::ScrollViewer& scrollViewer);

  float m_zoomFactor{ 1.0f };
  bool m_isScrollingFromInertia = false;
  bool m_isScrolling = false;
  bool m_isHorizontal = false;
  bool m_isScrollingEnabled = true;
  
  winrt::FrameworkElement::SizeChanged_revoker m_scrollViewerSizeChangedRevoker{};
  winrt::FrameworkElement::SizeChanged_revoker m_contentSizeChangedRevoker{};
  winrt::ScrollViewer::ViewChanged_revoker m_scrollViewerViewChangedRevoker{};
  winrt::ScrollViewer::ViewChanging_revoker m_scrollViewerViewChangingRevoker{};
  winrt::ScrollViewer::DirectManipulationCompleted_revoker m_scrollViewerDirectManipulationCompletedRevoker{};
  winrt::ScrollViewer::DirectManipulationStarted_revoker m_scrollViewerDirectManipulationStartedRevoker{};
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
  const auto scrollViewUWPImplementation = ScrollViewUWPImplementation(scrollViewer);
  scrollViewUWPImplementation.ScrollViewerSnapPointManager();

  AddHandlers(scrollViewer);

  m_scrollViewerSizeChangedRevoker = scrollViewer.SizeChanged(winrt::auto_revoke,
    [scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation.UpdateScrollableSize();
    });

  m_scrollViewerViewChangedRevoker = scrollViewer.ViewChanged(winrt::auto_revoke,
    [this, scrollViewUWPImplementation](const auto & sender, const auto & args)
    {
      const auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();
      const auto zoomFactor = scrollViewerNotNull.ZoomFactor();
      if (m_zoomFactor != zoomFactor)
      {
        m_zoomFactor = zoomFactor;
        scrollViewUWPImplementation.UpdateScrollableSize();
      }
    });

  m_contentSizeChangedRevoker = scrollViewUWPImplementation.ScrollViewerSnapPointManager()->SizeChanged(winrt::auto_revoke,
    [this, scrollViewUWPImplementation](const auto&, const auto&)
    {
      scrollViewUWPImplementation.UpdateScrollableSize();
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
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;
  
    if (propertyName == "horizontal")
    {
      const auto [valid, horizontal] = getPropertyAndValidity(propertyValue, false);
      if (valid)
      {
        m_isHorizontal = horizontal;
        ScrollViewUWPImplementation(scrollViewer).SetHorizontal(horizontal);
        SetScrollMode(scrollViewer);
      }
    }
    if (propertyName == "scrollEnabled")
    {
      const auto [valid, scrollEnabled] = getPropertyAndValidity(propertyValue, true);
      if (valid)
      {
        m_isScrollingEnabled = scrollEnabled;
        SetScrollMode(scrollViewer);
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

void ScrollViewShadowNode::AddHandlers(const winrt::ScrollViewer& scrollViewer)
{
  m_scrollViewerViewChangingRevoker = scrollViewer.ViewChanging(winrt::auto_revoke,
    [this](const auto& sender, const auto& args)
    {
      const auto scrollViewerNotNull = sender.as<winrt::ScrollViewer>();


      //If we are transitioning to inertial scrolling.
      if (m_isScrolling && !m_isScrollingFromInertia && args.IsInertial())
      {
        m_isScrollingFromInertia = true;

        EmitScrollEvent(
          scrollViewerNotNull,
          m_tag,
          "topScrollEndDrag",
          args.NextView().HorizontalOffset(),
          args.NextView().VerticalOffset(),
          args.NextView().ZoomFactor()
        );

        EmitScrollEvent(
          scrollViewerNotNull,
          m_tag,
          "topScrollBeginMomentum",
          args.NextView().HorizontalOffset(),
          args.NextView().VerticalOffset(),
          args.NextView().ZoomFactor()
        );
      }

      EmitScrollEvent(
        scrollViewerNotNull,
        m_tag,
        "topScroll",
        args.NextView().HorizontalOffset(),
        args.NextView().VerticalOffset(),
        args.NextView().ZoomFactor()
      );
    });

  m_scrollViewerDirectManipulationStartedRevoker = scrollViewer.DirectManipulationStarted(winrt::auto_revoke,
    [this](const auto& sender, const auto&)
    {
      m_isScrolling = true;
      const auto scrollViewer = sender.as<winrt::ScrollViewer>();
      EmitScrollEvent(
        scrollViewer,
        m_tag,
        "topScrollBeginDrag",
        scrollViewer.HorizontalOffset(),
        scrollViewer.VerticalOffset(),
        scrollViewer.ZoomFactor()
      );
    });

  m_scrollViewerDirectManipulationCompletedRevoker = scrollViewer.DirectManipulationCompleted(winrt::auto_revoke,
    [this](const auto& sender, const auto&)
    {
      const auto scrollViewer = sender.as<winrt::ScrollViewer>();
      if (m_isScrollingFromInertia)
      {
        EmitScrollEvent(
          scrollViewer,
          m_tag,
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
          m_tag,
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

void ScrollViewShadowNode::EmitScrollEvent(
  const winrt::ScrollViewer& scrollViewer,
  int64_t tag,
  const char* eventName,
  double x, double y, double zoom)
{
  const auto instance = GetViewManager()->GetReactInstance().lock();
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
      return std::make_tuple(true, propertyValue.getBool());
    }
  }

  if constexpr (std::is_same<T, double>())
  {
    if (propertyValue.isDouble())
    {
      return std::make_tuple(true, propertyValue.getDouble());
    }
    if (propertyValue.isInt())
    {
      return std::make_tuple(true, static_cast<double>(propertyValue.getInt()));
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
            if (val.isDouble())
              vector.Append(static_cast<float>(val.getDouble()));
            else if (val.isInt())
              vector.Append(static_cast<float>(val.getInt()));
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
        static_cast<T>([snapToAlignment = propertyValue.getString()]()
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

void ScrollViewShadowNode::SetScrollMode(const winrt::ScrollViewer& scrollViewer)
{
  const auto horizontalScrollingEnabled = m_isScrollingEnabled && m_isHorizontal;
  const auto verticalScrollingEnabled = m_isScrollingEnabled && !m_isHorizontal;
  scrollViewer.HorizontalScrollMode(horizontalScrollingEnabled ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
  scrollViewer.VerticalScrollMode(verticalScrollingEnabled ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
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
    ("scrollEnabled", "boolean")
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
  const auto scrollViewer = winrt::ScrollViewer{};

  scrollViewer.HorizontalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
  scrollViewer.VerticalScrollBarVisibility(winrt::ScrollBarVisibility::Visible);
  scrollViewer.VerticalSnapPointsAlignment(winrt::SnapPointsAlignment::Near);
  scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::Mandatory);
  scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::Mandatory);

  const auto snapPointManager = SnapPointManagingContentControl::Create();
  scrollViewer.Content(*snapPointManager);

  return scrollViewer;
}

void ScrollViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  assert(index == 0);

  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  auto snapPointManager = scrollViewer.Content().as<SnapPointManagingContentControl>();
  snapPointManager->Content(child);
}

void ScrollViewManager::RemoveAllChildren(XamlView parent)
{
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  auto snapPointManager = scrollViewer.Content().as<SnapPointManagingContentControl>();
  snapPointManager->Content(nullptr);
}

void ScrollViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  assert(index == 0);
  RemoveAllChildren(parent);
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
