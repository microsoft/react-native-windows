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

  bool ShouldUpdateView(bool isControl, bool hasOuterBorder)
  {
    return (IsControl() != isControl) || (HasOuterBorder() != hasOuterBorder);
  }

  bool IsControl() { return m_isControl; }
  void IsControl(bool isControl) { m_isControl = isControl; }

  bool HasOuterBorder() { return m_hasOuterBorder; }
  void HasOuterBorder(bool hasOuterBorder) { m_hasOuterBorder = hasOuterBorder; }

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

  winrt::com_ptr<ViewPanel> GetViewPanel()
  {
    XamlView current = m_view;

    if (IsControl())
    {
      auto control = m_view.as<winrt::ContentControl>();
      current = control.Content().as<XamlView>();
    }

    if (HasOuterBorder())
    {
      auto border = current.try_as<winrt::Border>();
      current = border.Child().try_as<XamlView>();
    }

    auto panel = current.try_as<ViewPanel>();
    assert(panel != nullptr);

    return std::move(panel);
  }

  winrt::impl::com_ref<react::uwp::ViewControl> GetControl()
  {
    return IsControl() ? m_view.as<ViewControl>() : nullptr;
  }

private:
  bool m_isControl = false;
  bool m_hasOuterBorder = false;

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

XamlView ViewViewManager::CreateViewControl(int64_t tag)
{
  // Create the ViewControl as the outer/containing element, nest the ViewPanel, set default properties
  auto contentControlPtr = ViewControl::Create();

  contentControlPtr->GotFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topFocus", std::move(folly::dynamic::object("target", tag)));
  });

  contentControlPtr->LostFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topBlur", std::move(folly::dynamic::object("target", tag)));
  });

  contentControlPtr->KeyDown([=](auto &&, winrt::KeyRoutedEventArgs const& e)
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

  return contentControlPtr->try_as<XamlView>();
}

XamlView ViewViewManager::CreateViewCore(int64_t tag)
{
  XamlView newViewPanelXamlView(ViewPanel::Create().as<XamlView>());
  auto panel = newViewPanelXamlView.as<ViewPanel>();
  panel->VerticalAlignment(winrt::VerticalAlignment::Top);

  return panel.as<XamlView>();
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

  auto pPanel = pViewShadowNode->GetViewPanel();
  if (pPanel != nullptr)
  {
    for (auto& pair : reactDiffMap.items())
    {
      const folly::dynamic& propertyName = pair.first;
      const folly::dynamic& propertyValue = pair.second;

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
          bool clipChildren = propertyValue.getString() == "hidden";
          pPanel->ClipChildren(clipChildren);
        }
      }
      else if (propertyName == "pointerEvents")
      {
        if (propertyValue.isString())
        {
          bool hitTestable = propertyValue.getString() != "none";
          pPanel->IsHitTestVisible(hitTestable);
        }
      }
      else if (propertyName == "acceptsKeyboardFocus")
      {
        if (propertyValue.isBool())
          shouldBeControl = propertyValue.getBool();
      }
      else if (propertyName == "accessibilityRole")
      {
        // FUTURE having an accessibilityRole shouldBeControl to support
        // non-Touchable scenarios
        if (propertyValue.isString())
        {
          const std::string& role = propertyValue.getString();
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
          pViewShadowNode->EnableFocusRing(propertyValue.getBool());
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

  TryUpdateView(pViewShadowNode, pPanel.get(), shouldBeControl);
}

void ViewViewManager::TryUpdateView(ViewShadowNode* pViewShadowNode, ViewPanel* pPanel, bool useControl)
{
  auto instance = m_wkReactInstance.lock();
  if (instance == nullptr)
    return;

  bool hasOuterBorder = pPanel->GetOuterBorder() != nullptr;

  // This short-circuits all of the update code when we have the same hierarchy
  if (!pViewShadowNode->ShouldUpdateView(useControl, hasOuterBorder))
    return;

  //
  // 1. Ensure we have the new 'root' and do the child replacement
  //      This is first to ensure that we can re-parent the Border or ViewPanel we already have
  // 2. Transfer properties
  //      There are likely some complexities to handle here
  // 3. Do any sub=parenting
  //      This means Panel under Border and/or Border under Control
  //

  int64_t tag = pViewShadowNode->m_tag;

  XamlView oldXamlView(pViewShadowNode->GetView());
  XamlView newXamlView(nullptr);

  // 1. Either create the new Control if needed or cleanup the old one if no longer needed
  if (useControl)
  {
    newXamlView = pViewShadowNode->GetControl().try_as<XamlView>();
    if (newXamlView == nullptr)
      newXamlView = CreateViewControl(tag);
  }
  else if (pViewShadowNode->IsControl())
  {
    pViewShadowNode->GetControl()->Content(nullptr);
  }

  // 2. If need outer border decide if it's our new root, else clean up old outer border
  if (hasOuterBorder)
  {
    if (!useControl)
      newXamlView = pPanel->GetOuterBorder().try_as<XamlView>();
  }
  else if (pViewShadowNode->HasOuterBorder())
  {
    winrt::Border outerBorder = pPanel->GetOuterBorder();
    if (outerBorder.Child() != nullptr)
      outerBorder.Child(pPanel->try_as<winrt::UIElement>());
  }

  // 3. Determine if the ViewPanel itself should be our root
  if (!useControl && !hasOuterBorder)
    newXamlView = pPanel->try_as<XamlView>();

  // ASSERT: One of the three scenarios should be true and we should have a root to use
  assert(newXamlView != nullptr);

  // Transfer properties from old XamlView to the new one
  TransferProperties(oldXamlView, newXamlView);

  // Since we transferred properties to the new view we need to make the call to finalize
  pPanel->FinalizeProperties();

  // Update the meta-data in the shadow node
  pViewShadowNode->IsControl(useControl);
  pViewShadowNode->HasOuterBorder(hasOuterBorder);

  // If we need to change the root of our view, do it now
  if (oldXamlView != newXamlView)
  {
    // Inform the parent ShadowNode of this change so the hierarchy can be updated
    int64_t parentTag = pViewShadowNode->GetParent();
    auto host = static_cast<facebook::react::INativeUIManager*>(instance->NativeUIManager())->getHost();
    auto *pParentNode = static_cast<ShadowNodeBase*>(host->FindShadowNodeForTag(parentTag));
    if (pParentNode != nullptr)
      pParentNode->ReplaceChild(oldXamlView, newXamlView);

    // Update the ShadowNode with the new XamlView
    pViewShadowNode->ReplaceView(newXamlView);
    pViewShadowNode->RefreshProperties();

    // Inform the NativeUIManager of this change so the yoga layout can be updated
    static_cast<facebook::react::INativeUIManager*>(instance->NativeUIManager())->ReplaceView(*pViewShadowNode);
  }

  // Ensure parenting is setup properly
  auto visualRoot = pPanel->try_as<winrt::UIElement>();

  if (hasOuterBorder)
  {
    winrt::Border outerBorder = pPanel->GetOuterBorder();
    if (outerBorder.Child() == nullptr)
      outerBorder.Child(pPanel->try_as<winrt::UIElement>());

    visualRoot = outerBorder;
  }

  if (useControl)
    pViewShadowNode->GetControl()->Content(visualRoot);
}

void ViewViewManager::SetLayoutProps(ShadowNodeBase& nodeToUpdate, XamlView viewToUpdate, float left, float top, float width, float height)
{
  // When the View has a ContentControl the ViewPanel must also have the Width & Height set
  // Do this first so that it is setup properly before any events are fired by the Super implementation
  auto* pViewShadowNode = static_cast<ViewShadowNode*>(&nodeToUpdate);
  if (pViewShadowNode->IsControl())
  {
    auto pPanel = pViewShadowNode->GetViewPanel();
    pPanel->Width(width);
    pPanel->Height(height);
  }

  Super::SetLayoutProps(nodeToUpdate, viewToUpdate, left, top, width, height);
}


} }
