// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ViewViewManager.h"

#include "ViewPanel.h"

#include <Utils/PropertyUtils.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#if defined(_DEBUG)
// Currently only used for tagging controls in debug
#include <winrt/Windows.Foundation.h>
#endif

namespace winrt {
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Input;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp {

ViewViewManager::ViewViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
{
}

const char* ViewViewManager::GetName() const
{
  return "RCTView";
}

XamlView ViewViewManager::CreateViewCore(int64_t tag)
{
  ViewPanel *pPanel = new ViewPanel();

  pPanel->VerticalAlignment(winrt::VerticalAlignment::Top);


#if defined(_DEBUG)
  pPanel->Tag(winrt::Windows::Foundation::PropertyValue::CreateInt64(tag));
#endif

  // TODO: Remove once existing clients stop using TouchableNativeFeedback.uwp
  pPanel->Tapped([=](auto &&, auto &&args)
  {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag);
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topPress", std::move(eventData));
    args.Handled(true);
  });

  return pPanel->try_as<XamlView>();
}

folly::dynamic ViewViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("pointerEvents", "string")
    ("onMouseEnter", "function")
    ("onMouseLeave", "function")
//  ("onMouseMove", "function")
  );

  return props;
}

void ViewViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto pPanel = viewToUpdate.as<ViewPanel>().get();
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
    }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);

  pPanel->FinalizeProperties();
}

void ViewViewManager::AddView(XamlView parent, XamlView child, int64_t index)
{
  auto pPanel = parent.as<ViewPanel>().get();
  if (pPanel != nullptr)
    pPanel->InsertAt(static_cast<uint32_t>(index), child.as<winrt::UIElement>());
}

XamlView ViewViewManager::GetChildAt(XamlView parent, int64_t index)
{
  auto pPanel = parent.as<ViewPanel>().get();
  if (pPanel != nullptr)
    return pPanel->GetAt(static_cast<uint32_t>(index));
  else
    return nullptr;
}

int64_t ViewViewManager::GetChildCount(XamlView parent)
{
  auto pPanel = parent.as<ViewPanel>().get();
  return (pPanel != nullptr) ? pPanel->Size() : 0;
}

void ViewViewManager::RemoveAllChildren(XamlView parent)
{
  auto pPanel = parent.as<ViewPanel>().get();
  if (pPanel != nullptr)
    pPanel->Clear();
}

void ViewViewManager::RemoveChildAt(XamlView parent, int64_t index)
{
  auto pPanel = parent.as<ViewPanel>().get();
  if (pPanel != nullptr)
    pPanel->RemoveAt(static_cast<uint32_t>(index));
}

} }
