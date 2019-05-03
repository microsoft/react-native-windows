// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ButtonViewManager.h"

#include <Utils/ValueUtils.h>
#include <Views/ShadowNodeBase.h>

#include <IReactInstance.h>

#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

namespace winrt {
using namespace Windows::Foundation;
using namespace Windows::UI;
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
using namespace Windows::UI::Xaml::Controls::Primitives;
using namespace Windows::UI::Xaml::Media;
}

namespace react { namespace uwp { namespace polyester {

ButtonViewManager::ButtonViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : ContentControlViewManager(reactInstance)
{
}

const char* ButtonViewManager::GetName() const
{
  // TODO: Is this right? Or should it be RCTButton?
  return "PLYButton";
}

folly::dynamic ButtonViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("accessibilityLabel", "string")
    ("disabled", "boolean")
    ("buttonType", "string")
  );

  return props;
}

folly::dynamic ButtonViewManager::GetExportedCustomDirectEventTypeConstants() const
{
  auto directEvents = Super::GetExportedCustomDirectEventTypeConstants();
  directEvents["topClick"] = folly::dynamic::object("registrationName", "onClick");

  return directEvents;
}

XamlView ButtonViewManager::CreateViewCore(int64_t tag)
{
  winrt::Button button = winrt::Button();
  button.Click([=](auto &&, auto &&)
  {
    auto instance = m_wkReactInstance.lock();
    folly::dynamic eventData = folly::dynamic::object("target", tag);
    if (instance != nullptr)
      instance->DispatchEvent(tag, "topClick", std::move(eventData));
  });

  return button;
}

void ButtonViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto button = nodeToUpdate->GetView().as<winrt::Button>();
  if (button == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

    if (propertyName.asString() == "disabled")
    {
      if (propertyValue.isBool())
        button.IsEnabled(!propertyValue.asBool());
    }

    continue;
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}
}}}
