// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollViewManager.h"

#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Text.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
}

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

  // snaps to the snapToInterval based on the snapToAlignment (start, center, end) within the visable area as defined by the viewableSize
  int snap(double original, winrt::Primitives::SnapPointsAlignment snapToAlignment, double snapToInterval, double viewableSize);

protected:
  double m_snapToInterval;
  bool m_isSnapToAlignmentSet;
  winrt::Primitives::SnapPointsAlignment m_snapToAlignment;

private:
   void topScroll(winrt::ScrollViewer& scrollViewer, int64_t tag, const char* event, double x, double y, double zoom);
};

ScrollViewShadowNode::ScrollViewShadowNode()
{
  m_snapToInterval = 0;
  m_isSnapToAlignmentSet = false;
  m_snapToAlignment = winrt::Primitives::SnapPointsAlignment::Near;
}

void ScrollViewShadowNode::dispatchCommand(int64_t commandId, const folly::dynamic& commandArgs)
{
  auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  switch (commandId)
  {
    case static_cast<int64_t>(ScrollViewCommands::ScrollTo) :
    {
      double x = commandArgs[0].asDouble();
      double y = commandArgs[1].asDouble();
      bool animated = commandArgs[2].asBool();
      bool horiz = scrollViewer.HorizontalScrollMode() == winrt::ScrollMode::Auto;
      if (horiz)
      {
        // snap the x coordinate
        if (m_isSnapToAlignmentSet && m_snapToInterval > 0.0)
        {
          x = snap(x, m_snapToAlignment, m_snapToInterval, scrollViewer.ViewportWidth());
        }
        // ChangeView parameters: x, y, zoom, animated
        scrollViewer.ChangeView(x, nullptr, nullptr, !animated/*disableAnimation*/);
      }
      else
      {
        // snap the y coordinate
        if (m_isSnapToAlignmentSet && m_snapToInterval > 0.0)
        {
          y = snap(y, m_snapToAlignment, m_snapToInterval, scrollViewer.ViewportHeight());
        }
        // ChangeView parameters: x, y, zoom, animated
        scrollViewer.ChangeView(nullptr, y, nullptr, !animated/*disableAnimation*/);
      }
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

  auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  ScrollViewManager* viewManager = static_cast<ScrollViewManager*>(GetViewManager());
  int64_t tag = m_tag;

  scrollViewer.ViewChanging([this, tag](const winrt::IInspectable& sender, const winrt::ScrollViewerViewChangingEventArgs& args)
  {
    winrt::ScrollViewer scrollViewer = sender.as<winrt::ScrollViewer>();
    topScroll(scrollViewer, tag,
      "topScroll",
      args.NextView().HorizontalOffset(),
      args.NextView().VerticalOffset(),
      args.NextView().ZoomFactor());

  });
}
 

void ScrollViewShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;

  auto scrollViewer = GetView().as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  for (auto& pair : props.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName.asString() == "horizontal")
    {
      if (propertyValue.isBool())
      {
        bool horiz = propertyValue.getBool();

        scrollViewer.HorizontalScrollMode(horiz ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
        scrollViewer.VerticalScrollMode(horiz ? winrt::ScrollMode::Disabled : winrt::ScrollMode::Auto);
      }
    }
    else if (propertyName.asString() == "showsHorizontalScrollIndicator")
    {
      if (propertyValue.isBool())
        scrollViewer.HorizontalScrollBarVisibility(propertyValue.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
    }
    else if (propertyName.asString() == "showsVerticalScrollIndicator")
    {
      if (propertyValue.isBool())
        scrollViewer.VerticalScrollBarVisibility(propertyValue.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
    }
    else if (propertyName.asString() == "minimumZoomScale")
    {
      if (propertyValue.isNumber())
        scrollViewer.MinZoomFactor(static_cast<float>(propertyValue.asDouble()));
    }
    else if (propertyName.asString() == "maximumZoomScale")
    {
      if (propertyValue.isNumber())
        scrollViewer.MaxZoomFactor(static_cast<float>(propertyValue.asDouble()));
    }
    else if (propertyName.asString() == "zoomScale")
    {
      if (propertyValue.isNumber())
        scrollViewer.ChangeView(nullptr, nullptr, static_cast<float>(propertyValue.asDouble()));
    }
    // the "snap" options don't match precisely the ScrollViewer options, so will perform the snap logic in the ShadowNode
    else if (propertyName.asString() == "snapToAlignment")
    {
      if (propertyValue.isString())
      {
        if (propertyValue.asString() == "start")
        {
          m_isSnapToAlignmentSet = true;
          m_snapToAlignment = winrt::Primitives::SnapPointsAlignment::Near;
        }
        else if (propertyValue.asString() == "center")
        {
          m_isSnapToAlignmentSet = true;
          m_snapToAlignment = winrt::Primitives::SnapPointsAlignment::Center;
        }
        else if (propertyValue.asString() == "end")
        {
          m_isSnapToAlignmentSet = true;
          m_snapToAlignment = winrt::Primitives::SnapPointsAlignment::Far;
        }     
        else
        {
          m_isSnapToAlignmentSet = false;
        }
        // TODO: need to implement the snap points themselves for this part to work.
        if (m_isSnapToAlignmentSet)
        {
          scrollViewer.HorizontalSnapPointsAlignment(m_snapToAlignment);
          scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::Mandatory);
          scrollViewer.VerticalSnapPointsAlignment(m_snapToAlignment);
          scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::Mandatory);
        }
        else
        {
          scrollViewer.HorizontalSnapPointsType(winrt::SnapPointsType::None);
          scrollViewer.VerticalSnapPointsType(winrt::SnapPointsType::None);
        }
      }
      else
      {
        m_isSnapToAlignmentSet = false;
      }
    }
    else if (propertyName.asString() == "snapToInterval")
    {
      if (propertyValue.isNumber())
      {
        m_snapToInterval = propertyValue.asDouble();
      }
      else
      {
        m_snapToInterval = 0.0;
      }
    }
  }

  Super::updateProperties(std::move(props));
  m_updating = false;

}

/***
   starting with original, snap to the snapToInterval, with relation to the snapToAlignment and the viewableSize
   original: The value to snap
   snapToAlignment: enum of either Near, Center, or Far, representing where in the viewableSize the value should snap to.
   snapToInterval: the output should be an even multiple of snapToInterval, offset into the viewableSize
   viewableSize: represents the size of the viewport, used with the snapToAlignment to offset the output to the beginning, middle, or end of the viewableSize

   return: the snapped value.
**/
int ScrollViewShadowNode::snap(double original, winrt::Primitives::SnapPointsAlignment snapToAlignment, double snapToInterval, double viewableSize)
{
  double snapAlignmentRatio = 0.0;
  switch (snapToAlignment)
  {
    case winrt::Primitives::SnapPointsAlignment::Near:
      snapAlignmentRatio = 0.0;
      break;

    case winrt::Primitives::SnapPointsAlignment::Center:
      snapAlignmentRatio = .5;
      break;

    case winrt::Primitives::SnapPointsAlignment::Far:
      snapAlignmentRatio = 1.0;
      break;

    default:
      snapAlignmentRatio = 0.0;
      break;

  }

  double snapDelta = fmod((original - snapAlignmentRatio * viewableSize), snapToInterval);
  // snap to the other side if it is closer
  if (snapDelta > snapToInterval / 2.0)
  {
    snapDelta -= snapToInterval;
  }
  double snappedPosition = (original - snapAlignmentRatio * viewableSize) - snapDelta;

  return snappedPosition;
}

void ScrollViewShadowNode::topScroll(winrt::ScrollViewer& scrollViewer, int64_t tag, const char* event, double x, double y, double zoom)
{
  ScrollViewManager* viewManager = static_cast<ScrollViewManager*>(GetViewManager());
  bool horiz = scrollViewer.HorizontalScrollMode() == winrt::ScrollMode::Auto;
  if (horiz)
  {
    // snap the x coordinate
    if (m_isSnapToAlignmentSet && m_snapToInterval > 0.0)
    {
      x = snap(x, m_snapToAlignment, m_snapToInterval, scrollViewer.ViewportWidth());
    }
  }
  else
  {
    // snap the y coordinate
    if (m_isSnapToAlignmentSet && m_snapToInterval > 0.0)
    {
      y = snap(y, m_snapToAlignment, m_snapToInterval, scrollViewer.ViewportHeight());
    }
  }
  viewManager->EmitScrollEvent(
    scrollViewer,
    m_tag,
    event,
    x,
    y,
    zoom);
}

/*
  ScrollViewManager class
*/
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
    ("snapToAlignment", "string")
    ("snapToInterval", "int")
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
  directEvents["topScrollBeginDrag"] = folly::dynamic::object("registrationName", "onScrollBeginDrag");
  directEvents["topScrollEndDrag"] = folly::dynamic::object("registrationName", "onScrollEndDrag");
  directEvents["topScroll"] = folly::dynamic::object("registrationName", "onScroll");

  return directEvents;
}

XamlView ScrollViewManager::CreateViewCore(int64_t tag)
{
  winrt::ScrollViewer scrollViewer;

  AddHandlers(scrollViewer, tag);

  return scrollViewer;
}

void ScrollViewManager::AddHandlers(winrt::ScrollViewer& scrollViewer, int64_t tag)
{
  scrollViewer.DirectManipulationStarted([this, tag](const winrt::IInspectable& sender, const winrt::IInspectable)
  {
    winrt::ScrollViewer scrollViewer = sender.as<winrt::ScrollViewer>();
    EmitScrollEvent(
      scrollViewer,
      tag,
      "topScrollBeginDrag",
      scrollViewer.HorizontalOffset(),
      scrollViewer.VerticalOffset(),
      scrollViewer.ZoomFactor()
    );
  });

  scrollViewer.DirectManipulationCompleted([this, tag](const winrt::IInspectable& sender, const winrt::IInspectable)
  {
    winrt::ScrollViewer scrollViewer = sender.as<winrt::ScrollViewer>();
    EmitScrollEvent(
      scrollViewer,
      tag,
      "topScrollEndDrag",
      scrollViewer.HorizontalOffset(),
      scrollViewer.VerticalOffset(),
      scrollViewer.ZoomFactor()
    );
  });
}

void ScrollViewManager::EmitScrollEvent(
  winrt::ScrollViewer& scrollViewer,
  int64_t tag,
  const char* eventName,
  double x, double y, double zoom)
{
  auto instance = m_wkReactInstance.lock();
  if (instance == nullptr)
    return;

  folly::dynamic offset = folly::dynamic::object
    ("x", x)
    ("y", y);

  folly::dynamic contentInset = folly::dynamic::object
    ("left", 0)
    ("top", 0)
    ("right", 0)
    ("bottom", 0);

  folly::dynamic contentSize = folly::dynamic::object
    ("width", scrollViewer.ExtentWidth())
    ("height", scrollViewer.ExtentHeight());

  folly::dynamic layoutSize = folly::dynamic::object
    ("width", scrollViewer.ActualWidth())
    ("height", scrollViewer.ActualHeight());

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
  assert(index == 0);
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  if (scrollViewer != nullptr)
    scrollViewer.Content(child);
}

void ScrollViewManager::RemoveAllChildren(XamlView parent)
{
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  if (scrollViewer != nullptr)
    scrollViewer.Content() = nullptr;
}

void ScrollViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  assert(index == 0);
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  if (scrollViewer != nullptr)
    scrollViewer.Content() = nullptr;
}

} }
