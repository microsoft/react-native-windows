// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewViewManager.h"

#include "ViewControl.h"
#include "ViewPanel.h"

#include <Modules/NativeUIManager.h>
#include <Utils/PropertyUtils.h>

#include <INativeUIManager.h>
#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>
#include <winrt/Windows.System.h>

#if defined(_DEBUG)
// Currently only used for tagging controls in debug
#include <winrt/Windows.Foundation.h>
#endif

namespace winrt {
using namespace Windows::System;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp {

class ViewShadowNode : public ShadowNodeBase
{
public:
  ViewShadowNode() = default;

  bool IsControl() { return m_isControl; }
  void IsControl(bool isControl) { m_isControl = isControl; }

  bool EnableFocusRing() { return m_enableFocusRing; }
  void EnableFocusRing(bool enable)
  {
    m_enableFocusRing = enable;

    if (IsControl())
      GetControl()->UseSystemFocusVisuals(m_enableFocusRing);
  }

  int32_t TabIndex() { return m_tabIndex; }
  void TabIndex(int32_t tabIndex)
  {
    m_tabIndex = tabIndex;

    if (IsControl())
      GetControl()->TabIndex(m_tabIndex);
  }

  bool OnClick() { return m_onClick; }
  void OnClick(bool isSet) { m_onClick = isSet; }

  AccessibilityRoles AccessibilityRole() { return m_accessibilityRole; }
  void AccessibilityRole(AccessibilityRoles role)
  {
    m_accessibilityRole = role;
    if (IsControl())
      GetControl()->AccessibilityRole(role);
  }

  bool AccessibilityState(AccessibilityStates state) { return m_accessibilityStates[state]; }
  void AccessibilityState(AccessibilityStates state, bool value)
  {
    m_accessibilityStates[state] = value;
  }

  void AddView(ShadowNode& child, int64_t index) override
  {
    GetViewPanel()->InsertAt(static_cast<uint32_t>(index), static_cast<ShadowNodeBase&>(child).GetView().as<winrt::UIElement>());
  }

  void RemoveChildAt(int64_t indexToRemove) override
  {
    if (indexToRemove == static_cast<uint32_t>(indexToRemove))
      GetViewPanel()->RemoveAt(static_cast<uint32_t>(indexToRemove));
  }

  void removeAllChildren() override
  {
    GetViewPanel()->Clear();
  }

  void ReplaceChild(XamlView oldChildView, XamlView newChildView) override
  {
    auto pPanel = GetViewPanel();
    if (pPanel != nullptr)
    {
      uint32_t index;
      if (pPanel->Children().IndexOf(oldChildView.as<winrt::UIElement>(), index))
      {
        pPanel->RemoveAt(index);
        pPanel->InsertAt(index, newChildView.as<winrt::UIElement>());
      }
      else
      {
        assert(false);
      }
    }
  }

  void RefreshProperties()
  {
    // The view may have been replaced, so transfer properties stored on the shadow node to the view
    AccessibilityRole(AccessibilityRole());
    AccessibilityState(AccessibilityStates::Disabled, AccessibilityState(AccessibilityStates::Disabled));
    AccessibilityState(AccessibilityStates::Selected, AccessibilityState(AccessibilityStates::Selected));
    EnableFocusRing(EnableFocusRing());
    TabIndex(TabIndex());
  }

  ViewPanel* GetViewPanel()
  {
    if (IsControl())
    {
      auto contentControl = m_view.as<winrt::ContentControl>();
      return contentControl.Content().as<ViewPanel>().get();
    }
    else
    {
      return m_view.as<ViewPanel>().get();
    }
  }

  winrt::impl::com_ref<react::uwp::ViewControl> GetControl()
  {
    return IsControl() ? m_view.as<react::uwp::ViewControl>() : nullptr;
  }

private:
  bool m_isControl = false;
  bool m_enableFocusRing = true;
  bool m_onClick = false;
  int32_t m_tabIndex = std::numeric_limits<std::int32_t>::max();
  AccessibilityRoles m_accessibilityRole = AccessibilityRoles::None;
  bool m_accessibilityStates[AccessibilityStates::CountStates] = { false, false };
};


ViewViewManager::ViewViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

const char* ViewViewManager::GetName() const
{
  return "RCTView";
}

folly::dynamic ViewViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");
  directEvents["topAccessibilityTap"] = folly::dynamic::object("registrationName", "onAccessibilityTap");

  return directEvents;
}

facebook::react::ShadowNode* ViewViewManager::createShadow() const
{
  return new ViewShadowNode();
}

void ViewViewManager::DispatchEvent(int64_t viewTag, std::string eventName, folly::dynamic&& eventData)
{
    auto instance = m_wkReactInstance.lock();
    if (instance != nullptr)
      instance->DispatchEvent(viewTag, eventName, std::move(eventData));
}

XamlView ViewViewManager::CreateViewPanel(int64_t tag)
{
  XamlView newViewPanelXamlView(ViewPanel::Create().as<XamlView>());
  auto panel = newViewPanelXamlView.as<ViewPanel>();
  panel->VerticalAlignment(winrt::VerticalAlignment::Top);

  // TODO: Remove once existing clients stop using TouchableNativeFeedback.uwp
  panel->Tapped([=](auto &&, auto &&args)
  {
    DispatchEvent(tag, "topPress", std::move(folly::dynamic::object("target", tag)));
    args.Handled(true);
  });

  return panel.as<XamlView>();
}

XamlView ViewViewManager::CreateViewControl(int64_t tag)
{
  // Create the ViewPanel which will be nested under the ContentControl
  XamlView newViewPanelXamlView(ViewPanel::Create().as<XamlView>());
  auto panel = newViewPanelXamlView.as<ViewPanel>();
  panel->VerticalAlignment(winrt::VerticalAlignment::Top);

  // Create the ContentControl as the outer/containing element, nest the ViewPanel, set default properties
  auto contentControlPtr = ViewControl::Create();
  auto contentControl = contentControlPtr.as<winrt::ContentControl>();
  contentControl.Content(newViewPanelXamlView);

  // TODO: Remove once existing clients stop using TouchableNativeFeedback.uwp
  contentControl.Tapped([=](auto &&, auto &&args)
  {
    DispatchEvent(tag, "topPress", std::move(folly::dynamic::object("target", tag)));
    args.Handled(true);
  });

  contentControl.GotFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topFocus", std::move(folly::dynamic::object("target", tag)));
  });

  contentControl.LostFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topBlur", std::move(folly::dynamic::object("target", tag)));
  });

  contentControl.KeyDown([=](auto &&, winrt::KeyRoutedEventArgs const& e)
  {
    if (e.Key() == winrt::VirtualKey::Enter || e.Key() == winrt::VirtualKey::Space)
    {
      auto instance = m_wkReactInstance.lock();
      if (instance != nullptr)
      {
        auto pNativeUiManager = static_cast<NativeUIManager*>(instance->NativeUIManager());
        facebook::react::INativeUIManagerHost* pUIManagerHost = pNativeUiManager->getHost();

        auto pViewShadowNode = static_cast<ViewShadowNode*>(pUIManagerHost->FindShadowNodeForTag(tag));
        if (pViewShadowNode != nullptr && pViewShadowNode->OnClick())
        {
          DispatchEvent(tag, "topClick", std::move(folly::dynamic::object("target", tag)));
          e.Handled(true);
        }
      }
    }
  });

  contentControlPtr->AccessibilityInvoke([=]()
  {
    auto instance = m_wkReactInstance.lock();
    if (instance != nullptr)
    {
      auto pNativeUiManager = static_cast<NativeUIManager*>(instance->NativeUIManager());
      facebook::react::INativeUIManagerHost* pUIManagerHost = pNativeUiManager->getHost();

      auto pViewShadowNode = static_cast<ViewShadowNode*>(pUIManagerHost->FindShadowNodeForTag(tag));
      if (pViewShadowNode != nullptr && pViewShadowNode->OnClick())
        DispatchEvent(tag, "topClick", std::move(folly::dynamic::object("target", tag)));
      else
        DispatchEvent(tag, "topAccessibilityTap", std::move(folly::dynamic::object("target", tag)));
    }
  });

  return contentControl.as<XamlView>();
}

XamlView ViewViewManager::CreateViewCore(int64_t tag)
{
  return CreateViewPanel(tag);
}

folly::dynamic ViewViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("accessible", "boolean")
    ("accessibilityRole", "string")
    ("accessibilityStates", "array")
    ("pointerEvents", "string")
    ("onClick", "function")
    ("onMouseEnter", "function")
    ("onMouseLeave", "function")
//  ("onMouseMove", "function")
    ("acceptsKeyboardFocus", "boolean")
    ("enableFocusRing", "boolean")
    ("tabIndex", "number")
  );

  return props;
}

void ViewViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto* pViewShadowNode = static_cast<ViewShadowNode*>(nodeToUpdate);
  bool shouldBeControl = pViewShadowNode->IsControl();

  auto* pPanel = pViewShadowNode->GetViewPanel();
  if (pPanel != nullptr)
  {
    for (auto& pair : reactDiffMap.items())
    {
      const folly::dynamic& propertyName = pair.first;
      const folly::dynamic& propertyValue = pair.second;

      // FUTURE: In the future cppwinrt will generate code where static methods on base types can
      // be called.  For now we specify the base type explicitly
      if (TryUpdateBackgroundBrush(*pPanel, propertyName, propertyValue))
      {
        continue;
      }
      else
      if (TryUpdateBorderProperties(nodeToUpdate, *pPanel, propertyName, propertyValue))
      {
        continue;
      }
      else if (TryUpdateCornerRadius(nodeToUpdate, *pPanel, propertyName, propertyValue))
      {
        continue;
      }
      else if (TryUpdateMouseEvents(nodeToUpdate, propertyName, propertyValue))
      {
        continue;
      }
      else if (propertyName == "onClick")
      {
        pViewShadowNode->OnClick(!propertyValue.isNull() && propertyValue.asBool());
      }
      else if (propertyName == "overflow")
      {
        if (propertyValue.isString())
        {
          bool clipChildren = propertyValue.asString() == "hidden";
          pPanel->ClipChildren(clipChildren);
        }
      }
      else if (propertyName == "pointerEvents")
      {
        if (propertyValue.isString())
        {
          bool hitTestable = propertyValue.asString() != "none";
          pPanel->IsHitTestVisible(hitTestable);
        }
      }
      else if (propertyName == "acceptsKeyboardFocus")
      {
        if (propertyValue.isBool())
          shouldBeControl = propertyValue.asBool();
      }
      else if (propertyName == "accessibilityRole")
      {
        // FUTURE having an accessibilityRole shouldBeControl to support
        // non-Touchable scenarios
        if (propertyValue.isString())
        {
          auto role = propertyValue.asString();
          if (role == "none")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::None);
          else if (role == "button")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Button);
          else if (role == "link")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Link);
          else if (role == "search")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Search);
          else if (role == "image")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Image);
          else if (role == "keyboardkey")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::KeyboardKey);
          else if (role == "text")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Text);
          else if (role == "adjustable")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Adjustable);
          else if (role == "imagebutton")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::ImageButton);
          else if (role == "header")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Header);
          else if (role == "summary")
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Summary);
          else
            pViewShadowNode->AccessibilityRole(AccessibilityRoles::Unknown);
        }
        else if (propertyValue.isNull())
        {
          pViewShadowNode->AccessibilityRole(AccessibilityRoles::None);
        }
      }
      else if (propertyName == "accessibilityStates")
      {
        // FUTURE having an accessibilityStates shouldBeControl to support
        // non-Touchable scenarios
        bool disabled = false;
        bool selected = false;
        if (propertyValue.isArray())
        {
          for (const auto& state : propertyValue)
          {
            if (!state.isString())
              continue;
            if (state.getString() == "disabled")
              disabled = true;
            else if (state.getString() == "selected")
              selected = true;
          }
        }

        pViewShadowNode->AccessibilityState(AccessibilityStates::Disabled, disabled);
        pViewShadowNode->AccessibilityState(AccessibilityStates::Selected, selected);
      }
      else if (propertyName == "enableFocusRing")
      {
        if (propertyValue.isBool())
          pViewShadowNode->EnableFocusRing(propertyValue.asBool());
        else if (propertyValue.isNull())
          pViewShadowNode->EnableFocusRing(false);
      }
      else if (propertyName == "tabIndex")
      {
        if (propertyValue.isNumber())
        {
          auto tabIndex = propertyValue.asInt();
          if (tabIndex == static_cast<int32_t>(tabIndex))
          {
            pViewShadowNode->TabIndex(static_cast<int32_t>(tabIndex));
          }
        }
        else if (propertyValue.isNull())
          pViewShadowNode->TabIndex(-1);
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);

  pPanel->FinalizeProperties();

  if (shouldBeControl != pViewShadowNode->IsControl())
    ReplaceView(pViewShadowNode, shouldBeControl);
}

XamlView GetPanel(XamlView view)
{
  // If the view is a ContentControl then its content is the ViewPanel else it is a ViewPanel
  winrt::ContentControl contentControl(nullptr);
  if (view.try_as<winrt::ContentControl>(contentControl))
    return contentControl.Content().as<winrt::FrameworkElement>();
  else
    return view;
}

void ViewViewManager::TransferProperties(XamlView oldView, XamlView newView)
{
  Super::TransferProperties(oldView, newView);

  // Retrieve the ViewPanels for use in these transfers since they are the elements that take all the properties
  XamlView oldPanel = GetPanel(oldView);
  XamlView newPanel = GetPanel(newView);

  TransferProperty(oldPanel, newPanel, ViewPanel::BackgroundProperty());
  TransferProperty(oldPanel, newPanel, ViewPanel::BorderBrushProperty());
  TransferProperty(oldPanel, newPanel, ViewPanel::BorderThicknessProperty());
  TransferProperty(oldPanel, newPanel, ViewPanel::ClipChildrenProperty());
  TransferProperty(oldPanel, newPanel, ViewPanel::CornerRadiusProperty());
}

void ViewViewManager::ReplaceView(ViewShadowNode* pViewShadowNode, bool useControl)
{
    auto instance = m_wkReactInstance.lock();
    if (instance != nullptr)
    {
      int64_t tag = pViewShadowNode->m_tag;

      // Create the ViewPanel we know we'll need either way
      XamlView newXamlView(nullptr);

      // If we need a Control then create a ContentControl and nest the ViewPanel inside it
      if (useControl)
        newXamlView = CreateViewControl(tag);
      // Else just use the ViewPanel (already created)
      else
        newXamlView = CreateViewPanel(tag);

      XamlView oldXamlView(pViewShadowNode->GetView());

      // Transfer properties from old XamlView to the new one
      TransferProperties(oldXamlView, newXamlView);

      // Update the ShadowNode with the new XamlView
      pViewShadowNode->ReplaceView(newXamlView);
      pViewShadowNode->IsControl(useControl);
      pViewShadowNode->RefreshProperties();

      // Inform the parent ShadowNode of this change so the hierarchy can be updated
      int64_t parentTag = pViewShadowNode->GetParent();
      auto host = static_cast<facebook::react::INativeUIManager*>(instance->NativeUIManager())->getHost();
      auto *pParentNode = static_cast<ShadowNodeBase*>(host->FindShadowNodeForTag(parentTag));
      if (pParentNode != nullptr)
        pParentNode->ReplaceChild(oldXamlView, newXamlView);

      // Since we transferred properties to the new view we need to make the call to finalize
      pViewShadowNode->GetViewPanel()->FinalizeProperties();

      // Inform the NativeUIManager of this change so the yoga layout can be updated
      static_cast<facebook::react::INativeUIManager*>(instance->NativeUIManager())->ReplaceView(*pViewShadowNode);
    }
}

void ViewViewManager::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
{
  // When the View has a ContentControl the ViewPanel must also have the Width & Height set
  // Do this first so that it is setup properly before any events are fired by the Super implementation
  auto* pViewShadowNode = static_cast<ViewShadowNode*>(&nodeToUpdate);
  if (pViewShadowNode->IsControl())
  {
    auto* pPanel = pViewShadowNode->GetViewPanel();
    pPanel->Width(width);
    pPanel->Height(height);
  }

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
}


} }
