// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewViewManager.h"

#include "ViewPanel.h"

#include <Modules/NativeUIManager.h>
#include <Utils/PropertyUtils.h>

#include <INativeUIManager.h>

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
      GetControl().UseSystemFocusVisuals(m_enableFocusRing);
  }

  bool OnClick() { return m_onClick; }
  void OnClick(bool isSet) { m_onClick = isSet; }

  void AddView(ShadowNode& child, int64_t index) override
  {
    GetViewPanel()->InsertAt(static_cast<uint32_t>(index), static_cast<ShadowNodeBase&>(child).GetView().as<winrt::UIElement>());
  }

  void RemoveChildAt(int64_t indexToRemove) override
  {
    GetViewPanel()->RemoveAt(indexToRemove);
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
    EnableFocusRing(EnableFocusRing());
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

  winrt::ContentControl GetControl()
  {
    return IsControl() ? m_view.as<winrt::ContentControl>() : winrt::ContentControl(nullptr);
  }

private:
  bool m_isControl = false;
  bool m_enableFocusRing = true;
  bool m_onClick = false;
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
  directEvents["topTextInputFocus"] = folly::dynamic::object("registrationName", "onFocus");
  directEvents["topTextInputBlur"] = folly::dynamic::object("registrationName", "onBlur");
  directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");

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
  winrt::ContentControl contentControl;
  contentControl.Content(newViewPanelXamlView);

  // TODO: Remove once existing clients stop using TouchableNativeFeedback.uwp
  contentControl.Tapped([=](auto &&, auto &&args)
  {
    DispatchEvent(tag, "topPress", std::move(folly::dynamic::object("target", tag)));
    args.Handled(true);
  });

  contentControl.GotFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topTextInputFocus", std::move(folly::dynamic::object("target", tag)));
  });

  contentControl.LostFocus([=](auto &&, auto &&)
  {
    DispatchEvent(tag, "topTextInputBlur", std::move(folly::dynamic::object("target", tag)));
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
    ("pointerEvents", "string")
    ("onClick", "function")
    ("onMouseEnter", "function")
    ("onMouseLeave", "function")
//  ("onMouseMove", "function")
    ("acceptsKeyboardFocus", "boolean")
    ("enableFocusRing", "boolean")
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
      else if (pair.first == "acceptsKeyboardFocus")
      {
        if (pair.second.isBool())
          shouldBeControl = pair.second.asBool();
      }
      else if (pair.first == "enableFocusRing")
      {
        if (propertyValue.isBool())
          pViewShadowNode->EnableFocusRing(propertyValue.asBool());
        else if (propertyValue.isNull())
          pViewShadowNode->EnableFocusRing(false);
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

} }
