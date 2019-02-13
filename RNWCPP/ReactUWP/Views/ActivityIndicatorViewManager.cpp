// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ActivityIndicatorViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>

#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
  using namespace Windows::UI::Xaml::Controls;
}

namespace react { namespace uwp {

ActivityIndicatorViewManager::ActivityIndicatorViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : FrameworkElementViewManager(reactInstance)
{
}

const char* ActivityIndicatorViewManager::GetName() const
{
  return "RCTActivityIndicatorView";
}

folly::dynamic ActivityIndicatorViewManager::GetNativeProps() const
{
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object
    ("animating", "boolean")
  );

  return props;
}

XamlView ActivityIndicatorViewManager::CreateViewCore(int64_t tag)
{
  auto progressRing = winrt::ProgressRing();
  return progressRing;
}

void ActivityIndicatorViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, XamlView viewToUpdate, folly::dynamic reactDiffMap)
{
  auto progressRing = viewToUpdate.as<winrt::ProgressRing>();
  if (progressRing == nullptr)
    return;

  for (auto& pair : reactDiffMap.items())
  {
    const folly::dynamic& propertyName = pair.first;
    const folly::dynamic& propertyValue = pair.second;

   if (propertyName.asString() == "animating")
   {
      if (propertyValue.isBool())
        progressRing.IsActive(propertyValue.asBool());
   }
   // FUTURE: In the future cppwinrt will generate code where static methods on base types can
   // be called.  For now we specify the base type explicitly
   else if (TryUpdateForeground<winrt::Control>(progressRing, pair.first, pair.second))
   {
     continue;
   }
  }

  Super::UpdateProperties(nodeToUpdate, viewToUpdate, reactDiffMap);
}

}}
