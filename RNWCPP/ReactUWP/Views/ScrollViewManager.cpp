// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ScrollViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

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

ScrollViewManager::ScrollViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
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
  );

  return props;
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
  // Enable as tabstop so clicks inside don't move focus to the first element.
  // FUTURE: expose prop to control this
  scrollViewer.IsTabStop(true);

  return scrollViewer;
}

void ScrollViewManager::AddHandlers(winrt::ScrollViewer& scrollViewer, int64_t tag)
{
  scrollViewer.ViewChanging([this, tag](const winrt::IInspectable& sender, const winrt::ScrollViewerViewChangingEventArgs& args)
  {
    winrt::ScrollViewer scrollViewer = sender.as<winrt::ScrollViewer>();
    EmitScrollEvent(
      scrollViewer,
      tag,
      "topScroll",
      args.NextView().HorizontalOffset(),
      args.NextView().VerticalOffset(),
      args.NextView().ZoomFactor()
    );
  });

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

XamlView ScrollViewManager::GetChildAt(XamlView parent, int64_t index)
{
  assert(index == 0);
  auto scrollViewer = parent.as<winrt::ScrollViewer>();
  if (scrollViewer != nullptr)
  {
    auto c = scrollViewer.Content();
    return c.as<XamlView>();
  }

  return nullptr;
}

int64_t ScrollViewManager::GetChildCount(XamlView parent)
{
  auto scrollViewer = parent.as<winrt::ScrollViewer>();

  return (scrollViewer != nullptr && scrollViewer.Content() != nullptr) ? 1 : 0;
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

void ScrollViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto scrollViewer = viewToUpdate.as<winrt::ScrollViewer>();
  if (scrollViewer == nullptr)
    return;

  auto control = scrollViewer.as<winrt::Control>();
  for (auto& pair : reactDiffMap.items())
  {
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    if (TryUpdateForeground<winrt::Control>(scrollViewer, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateFontProperties(scrollViewer, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdatePadding(nodeToUpdate, scrollViewer, pair.first, pair.second))
    {
      continue;
    }
    else if (TryUpdateCharacterSpacing(scrollViewer, pair.first, pair.second))
    {
      continue;
    }
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    else if (TryUpdateBackgroundBrush(control, pair.first, pair.second))
    {
      continue;
    }
    // FUTURE: In the future cppwinrt will generate code where static methods on base types can
    // be called.  For now we specify the base type explicitly
    else if (TryUpdateBorderProperties(nodeToUpdate, control, pair.first, pair.second))
    {
      continue;
    }
    else if (pair.first == "horizontal")
    {
      if (pair.second.isBool())
      {
        bool horiz = pair.second.getBool();

        scrollViewer.HorizontalScrollMode(horiz ? winrt::ScrollMode::Auto : winrt::ScrollMode::Disabled);
        scrollViewer.VerticalScrollMode(horiz ? winrt::ScrollMode::Disabled : winrt::ScrollMode::Auto);
      }
    }
    else if (pair.first == "showsHorizontalScrollIndicator")
    {
      if (pair.second.isBool())
        scrollViewer.HorizontalScrollBarVisibility(pair.second.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
    }
    else if (pair.first == "showsVerticalScrollIndicator")
    {
      if (pair.second.isBool())
        scrollViewer.VerticalScrollBarVisibility(pair.second.getBool() ? winrt::ScrollBarVisibility::Visible : winrt::ScrollBarVisibility::Hidden);
    }
    else if (pair.first == "minimumZoomScale")
    {
      if (pair.second.isNumber())
        scrollViewer.MinZoomFactor(static_cast<float>(pair.second.asDouble()));
    }
    else if (pair.first == "maximumZoomScale")
    {
      if (pair.second.isNumber())
        scrollViewer.MaxZoomFactor(static_cast<float>(pair.second.asDouble()));
    }
    else if (pair.first == "zoomScale")
    {
      if (pair.second.isNumber())
        scrollViewer.ChangeView(nullptr, nullptr, static_cast<float>(pair.second.asDouble()));
    }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);
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
