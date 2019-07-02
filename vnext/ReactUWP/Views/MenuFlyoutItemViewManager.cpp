// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "MenuFlyoutItemViewManager.h"
#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"
#include "ViewPanel.h"
#include <Utils/ValueUtils.h>

#include <Modules/NativeUIManager.h>
#include <Utils/PropertyHandlerUtils.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
  using namespace Windows::UI::Xaml::Controls::Primitives;
  using namespace Windows::UI::Xaml::Interop;
}


namespace react {
  namespace uwp {

    class MenuFlyoutItemShadowNode : public ShadowNodeBase
    {
      using Super = ShadowNodeBase;
    public:
      MenuFlyoutItemShadowNode() = default;
      virtual ~MenuFlyoutItemShadowNode();

      void AddView(ShadowNode& child, int64_t index) override;
      void createView() override;

      void removeAllChildren() override;
      void updateProperties(const folly::dynamic&& props) override;

    private:
      std::shared_ptr<TouchEventHandler> m_touchEventHandler;
    };

    MenuFlyoutItemShadowNode::~MenuFlyoutItemShadowNode()
    {
      m_touchEventHandler->RemoveTouchHandlers();
    }

    void MenuFlyoutItemShadowNode::AddView(ShadowNode& child, int64_t index)
    {
      // MenuItems should have no children
    }

    void MenuFlyoutItemShadowNode::createView()
    {
      Super::createView();
      auto wkinstance = GetViewManager()->GetReactInstance();
      m_touchEventHandler = std::make_shared<TouchEventHandler>(wkinstance);
    }

    void MenuFlyoutItemShadowNode::removeAllChildren()
    {
      // MenuItems should have no children
    }

    void MenuFlyoutItemShadowNode::updateProperties(const folly::dynamic&& props)
    {
      auto MenuFlyoutItem = GetView().as<winrt::MenuFlyoutItem>();

      if (MenuFlyoutItem == nullptr)
        return;

      for (auto& pair : props.items())
      {
        const std::string& propertyName = pair.first.getString();
        const folly::dynamic& propertyValue = pair.second;
       
        if (propertyName == "text")
        {
          if (propertyValue.isString())
          {
            MenuFlyoutItem.Text(asHstring(propertyValue.asString()));
          }
        }
      }

      Super::updateProperties(std::move(props));
    }


    MenuFlyoutItemViewManager::MenuFlyoutItemViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
      : Super(reactInstance)
    {
    }

    const char* MenuFlyoutItemViewManager::GetName() const
    {
      return "RCTMenuFlyoutItem";
    }

    XamlView MenuFlyoutItemViewManager::CreateViewCore(int64_t tag)
    {
      auto MenuFlyoutItem = winrt::MenuFlyoutItem();

      MenuFlyoutItem.Click([=](auto &&, auto &&)
      {
        auto instance = m_wkReactInstance.lock();
        folly::dynamic eventData = folly::dynamic::object("target", tag);
        if (instance != nullptr)
          instance->DispatchEvent(tag, "topClick", std::move(eventData));
      });
      return MenuFlyoutItem;
    }

    facebook::react::ShadowNode* MenuFlyoutItemViewManager::createShadow() const
    {
      return new MenuFlyoutItemShadowNode();
    }

    folly::dynamic MenuFlyoutItemViewManager::GetNativeProps() const
    {
      auto props = Super::GetNativeProps();

      props.update(folly::dynamic::object
      ("text", "string")
      );

      return props;
    }

    folly::dynamic MenuFlyoutItemViewManager::GetExportedCustomDirectEventTypeConstants() const
    {
      auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
      directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");

      return directEvents;
    }

    void MenuFlyoutItemViewManager::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
    {
    }

  }
}
