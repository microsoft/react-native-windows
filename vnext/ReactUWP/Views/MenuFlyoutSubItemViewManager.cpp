// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"
#include "MenuFlyoutSubItemViewManager.h"
#include <Views/ShadowNodeBase.h>
#include "TouchEventHandler.h"
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

    class  MenuFlyoutSubItemShadowNode : public ShadowNodeBase
    {
      using Super = ShadowNodeBase;
    public:
      MenuFlyoutSubItemShadowNode() = default;
      virtual ~MenuFlyoutSubItemShadowNode();

      void AddView(ShadowNode& child, int64_t index) override;
      void createView() override;

      void removeAllChildren() override;
      void updateProperties(const folly::dynamic&& props) override;

    };

    MenuFlyoutSubItemShadowNode::~MenuFlyoutSubItemShadowNode()
    {

    }

    void  MenuFlyoutSubItemShadowNode::AddView(ShadowNode& child, int64_t index)
    {
      // child must be a MenuFlyoutItemBase derivative
      auto childView = static_cast<ShadowNodeBase&>(child).GetView();
      auto  MenuFlyoutSubItem = GetView().as<winrt::MenuFlyoutSubItem>();
      MenuFlyoutSubItem.Items().Append(childView.as<winrt::MenuFlyoutItemBase>());
    }

    void  MenuFlyoutSubItemShadowNode::createView()
    {
      Super::createView();
      auto wkinstance = GetViewManager()->GetReactInstance();

    }

    void  MenuFlyoutSubItemShadowNode::removeAllChildren()
    {
      GetView().as<winrt::MenuFlyoutSubItem>().Items().Clear();
    }

    void  MenuFlyoutSubItemShadowNode::updateProperties(const folly::dynamic&& props)
    {
      auto  MenuFlyoutSubItem = GetView().as<winrt::MenuFlyoutSubItem>();

      if (MenuFlyoutSubItem == nullptr)
        return;

      for (auto& pair : props.items())
      {
        const std::string& propertyName = pair.first.getString();
        const folly::dynamic& propertyValue = pair.second;

        if (propertyName == "text")
        {
          if (propertyValue.isString())
          {
            MenuFlyoutSubItem.Text(asHstring(propertyValue.asString()));
          }
        }
      }

      Super::updateProperties(std::move(props));

    }


    MenuFlyoutSubItemViewManager::MenuFlyoutSubItemViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
      : Super(reactInstance)
    {
    }

    const char*  MenuFlyoutSubItemViewManager::GetName() const
    {
      return "RCTMenuFlyoutSubItem";
    }

    XamlView  MenuFlyoutSubItemViewManager::CreateViewCore(int64_t tag)
    {
      auto MenuFlyoutSubItem = winrt::MenuFlyoutSubItem();
      return MenuFlyoutSubItem;
    }

    facebook::react::ShadowNode*  MenuFlyoutSubItemViewManager::createShadow() const
    {
      return new  MenuFlyoutSubItemShadowNode();
    }

    folly::dynamic  MenuFlyoutSubItemViewManager::GetNativeProps() const
    {
      auto props = Super::GetNativeProps();

      props.update(folly::dynamic::object
      ("text", "string")
      );

      return props;
    }

    folly::dynamic  MenuFlyoutSubItemViewManager::GetExportedCustomDirectEventTypeConstants() const
    {
      auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
      return directEvents;
    }

    void  MenuFlyoutSubItemViewManager::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
    {
    }

  }
}
