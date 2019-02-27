// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "PopupViewManager.h"
#include "ShadowNodeBase.h"
#include "TouchEventHandler.h"

#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
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

private:
  bool m_updating = false;
  std::shared_ptr<TouchEventHandler> m_touchEventHanadler;
};

PopupShadowNode::~PopupShadowNode()
{
  m_touchEventHanadler->RemoveTouchHandlers();
}

void PopupShadowNode::createView()
{
  Super::createView();

  auto popup = GetView().as<winrt::Popup>();
  auto wkinstance = static_cast<PopupViewManager*>(GetViewManager())->m_wkReactInstance;
  m_touchEventHanadler = std::make_shared<TouchEventHandler>(wkinstance);

  popup.Closed([=](auto&&, auto&&)
  {
    auto instance = wkinstance.lock();
    if (!m_updating && instance != nullptr)
      OnPopupClosed(*instance, m_tag, false);
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
  Super::updateProperties(std::move(props));
  m_updating = false;
}

/*static*/ void PopupShadowNode::OnPopupClosed(IReactInstance& instance, int64_t tag, bool newValue)
{
  folly::dynamic eventData = folly::dynamic::object("target", tag)("isOpen", newValue);
  instance.DispatchEvent(tag, "topDismiss", std::move(eventData));
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
    ("horizontalOffset", "int")
    ("verticalOffset", "int")
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

void PopupViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto popup = nodeToUpdate->GetView().as<winrt::Popup>();
  if (popup == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

   if (propertyName.asString() == "isOpen")
   {
      if (propertyValue.isBool())
        popup.IsOpen(propertyValue.asBool());
     else if (propertyValue.isNull())
       popup.ClearValue(winrt::Popup::IsOpenProperty());
   }
   else if (propertyName.asString() == "isLightDismissEnabled")
   {
     if (propertyValue.isBool())
       popup.IsLightDismissEnabled(propertyValue.asBool());
     else if (propertyValue.isNull())
       popup.ClearValue(winrt::Popup::IsLightDismissEnabledProperty());
   }
   else if (propertyName.asString() == "horizontalOffset")
   {
     if (propertyValue.isInt())
       popup.HorizontalOffset(propertyValue.asInt());
     else if (propertyValue.isNull())
       popup.ClearValue(winrt::Popup::HorizontalOffsetProperty());
   }
   else if (propertyName.asString() == "verticalOffset")
   {
     if (propertyValue.isInt())
       popup.VerticalOffset(propertyValue.asInt());
     else if (propertyValue.isNull())
       popup.ClearValue(winrt::Popup::VerticalOffsetProperty());
   }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

folly::dynamic PopupViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topDismiss"] = folly::dynamic::object("registrationName", "onDismiss");

  return directEvents;
}

}}
