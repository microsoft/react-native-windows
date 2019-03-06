// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>

#include <Utils/PropertyUtils.h>

#include <winrt/Windows.UI.Xaml.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace winrt {
using namespace Windows::UI::Xaml;
using namespace Windows::UI::Xaml::Controls;
}

namespace react { namespace uwp {

ControlViewManager::ControlViewManager(const std::shared_ptr<IReactInstance>& reactInstance)
  : Super(reactInstance)
{
}

void ControlViewManager::UpdateProperties(ShadowNodeBase* nodeToUpdate, folly::dynamic reactDiffMap)
{
  auto control(nodeToUpdate->GetView().as<winrt::Control>());

  if (control != nullptr)
  {
    for (auto& pair : reactDiffMap.items())
    {
      const folly::dynamic& propertyName = pair.first;
      const folly::dynamic& propertyValue = pair.second;

      if (TryUpdateBackgroundBrush(control, propertyName, propertyValue))
      {
        continue;
      }
      else if (TryUpdateBorderProperties(nodeToUpdate, control, propertyName, propertyValue))
      {
        continue;
      }
      else if (TryUpdateForeground(control, propertyName, propertyValue))
      {
        continue;
      }
      else if (TryUpdatePadding(nodeToUpdate, control, propertyName, propertyValue))
      {
        continue;
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} }
