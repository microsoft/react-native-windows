// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "PopupViewManager.h"
#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"

#include <Modules/NativeUIManager.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
  using namespace Windows::Foundation;
  using namespace Windows::UI::Core;
  using namespace Windows::UI::Xaml;
  using namespace Windows::UI::Xaml::Controls;
  using namespace Windows::UI::Xaml::Controls::Primitives;
}

namespace react { namespace uwp {

class PopupShadowNode : public ShadowNodeBase
{
  using Super = ShadowNodeBase;
public:
  PopupShadowNode() = default;
  virtual ~PopupShadowNode();
  void createView() override;
  void AddView(ShadowNode& child, int64_t index) override;
  void updateProperties(const folly::dynamic&& props) override;
  static void OnPopupClosed(IReactInstance& instance, int64_t tag, bool newValue);
  void SetAnchorPosition(const winrt::Windows::UI::Xaml::Controls::Primitives::Popup& popup);
  winrt::Windows::Foundation::Size GetAppWindowSize();

private:
  std::shared_ptr<TouchEventHandler> m_touchEventHanadler;
  int64_t m_targetTag;
};

PopupShadowNode::~PopupShadowNode()
{
  m_touchEventHanadler->RemoveTouchHandlers();
}

void PopupShadowNode::createView()
{
  Super::createView();

  auto popup = GetView().as<winrt::Popup>();
  auto wkinstance = GetViewManager()->GetReactInstance();
  m_touchEventHanadler = std::make_shared<TouchEventHandler>(wkinstance);

  popup.Closed([=](auto&&, auto&&)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnPopupClosed(*instance, m_tag, false);
  });

  popup.Opened([=](auto&&, auto&&)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      SetAnchorPosition(popup);
  });
}

void PopupShadowNode::AddView(ShadowNode& child, int64_t index)
{
  Super::AddView(child, index);

  auto childView = static_cast<ShadowNodeBase&>(child).GetView();
  m_touchEventHanadler->AddTouchHandlers(childView);
}

void PopupShadowNode::updateProperties(const folly::dynamic&& props)
{
  m_updating = true;
  
  auto popup = GetView().as<winrt::Popup>();
  if (popup == nullptr)
    return;

  for (auto& pair : props.items())
  {
    const std::string& propertyName = pair.first.getString();
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName == "target")
    {
      if (propertyValue.isNumber())
        m_targetTag = static_cast<int64_t>(propertyValue.asDouble());
      else
        m_targetTag = -1;
    }
    else if (propertyName == "isOpen")
    {
      if (propertyValue.isBool())
        popup.IsOpen(propertyValue.getBool());
      else if (propertyValue.isNull())
        popup.ClearValue(winrt::Popup::IsOpenProperty());
    }
    else if (propertyName == "isLightDismissEnabled")
    {
      if (propertyValue.isBool())
        popup.IsLightDismissEnabled(propertyValue.getBool());
      else if (propertyValue.isNull())
        popup.ClearValue(winrt::Popup::IsLightDismissEnabledProperty());
    }
    else if (propertyName == "horizontalOffset")
    {
      if (propertyValue.isNumber())
        popup.HorizontalOffset(propertyValue.asDouble());
      else if (propertyValue.isNull())
        popup.ClearValue(winrt::Popup::HorizontalOffsetProperty());
    }
    else if (propertyName == "verticalOffset")
    {
      if (propertyValue.isNumber())
        popup.VerticalOffset(propertyValue.asDouble());
      else if (propertyValue.isNull())
        popup.ClearValue(winrt::Popup::VerticalOffsetProperty());
    }
  }

  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void PopupShadowNode::OnPopupClosed(IReactInstance& instance, int64_t tag, bool newValue)
{
  folly::dynamic eventData = folly::dynamic::object("target", tag)("isOpen", newValue);
  instance.DispatchEvent(tag, "topDismiss", std::move(eventData));
}


/**
 * This is a quick fix for positioning with limited functionality. This will 
 * be followed by a more permanent fix soon.
 */
void PopupShadowNode::SetAnchorPosition(const winrt::Popup& popup)
{
  auto wkinstance = static_cast<PopupViewManager*>(GetViewManager())->m_wkReactInstance;
  auto instance = wkinstance.lock();

  if (instance == nullptr)
    return;

  // center relative to anchor
  if (m_targetTag > 0)
  {
    auto pNativeUIManagerHost = static_cast<NativeUIManager*>(instance->NativeUIManager())->getHost();
    ShadowNodeBase* pShadowNodeChild = static_cast<ShadowNodeBase*>(pNativeUIManagerHost->FindShadowNodeForTag(m_targetTag));

    if (pShadowNodeChild != nullptr)
    {
      auto targetView = pShadowNodeChild->GetView();
      auto targetElement = targetView.as<winrt::FrameworkElement>();

      auto popupTransform = targetElement.TransformToVisual(popup);
      winrt::Point bottomRightPoint(static_cast<float>(targetElement.Width()), static_cast<float>(targetElement.Height()));
      auto point = popupTransform.TransformPoint(bottomRightPoint);
      popup.HorizontalOffset(point.X + popup.HorizontalOffset());
      popup.VerticalOffset(point.Y + popup.VerticalOffset());
    }
  }
  else // Center relative to app window
  {
    auto appWindow = winrt::Window::Current().Content();
    auto popupToWindow = appWindow.TransformToVisual(popup);
    auto appWindowSize = GetAppWindowSize();
    winrt::Point centerPoint;
    centerPoint.X = static_cast<float>((appWindowSize.Width - popup.ActualWidth()) / 2);
    centerPoint.Y = static_cast<float>((appWindowSize.Height - popup.ActualHeight()) / 2);
    auto point = popupToWindow.TransformPoint(centerPoint);
    popup.HorizontalOffset(point.X);
    popup.VerticalOffset(point.Y);
  }
}

winrt::Size PopupShadowNode::GetAppWindowSize()
{
  winrt::Size windowSize = winrt::SizeHelper::Empty();

  if (auto current = winrt::Window::Current())
  {
    if (auto coreWindow = current.CoreWindow())
    {
      windowSize.Width = coreWindow.Bounds().Width;
      windowSize.Height = coreWindow.Bounds().Height;
    }
  }

  return windowSize;
}


PopupViewManager::PopupViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* PopupViewManager::GetName() const
{
  return "RCTPopup";
}

folly::dynamic PopupViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("isOpen", "boolean")
    ("isLightDismissEnabled", "boolean")
    ("horizontalOffset", "number")
    ("verticalOffset", "number")
    ("target", "number")
  );

  return props;
}

facebook::react::ShadowNode* PopupViewManager::createShadow() const
{
  return new PopupShadowNode();
}

XamlView PopupViewManager::CreateViewCore(int64_t tag)
{
  auto popup = winrt::Popup();
  return popup;
}

void PopupViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  assert(index == 0);

  auto popup = parent.as<winrt::Popup>();
  if (popup != nullptr)
    popup.Child(child.as<winrt::UIElement>());
}

folly::dynamic PopupViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topDismiss"] = folly::dynamic::object("registrationName", "onDismiss");

  return directEvents;
}

}}
