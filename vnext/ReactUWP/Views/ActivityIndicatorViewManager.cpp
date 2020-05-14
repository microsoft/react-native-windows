// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include "ActivityIndicatorViewManager.h"

#include <Utils/PropertyUtils.h>
#include <Utils/ValueUtils.h>
#include <winrt/Windows.UI.Xaml.Controls.h>

namespace react {
namespace uwp {

ActivityIndicatorViewManager::ActivityIndicatorViewManager(const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

const char *ActivityIndicatorViewManager::GetName() const {
  return "RCTActivityIndicatorView";
}

folly::dynamic ActivityIndicatorViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("animating", "boolean")("color", "Color"));

  return props;
}

XamlView ActivityIndicatorViewManager::CreateViewCore(int64_t /*tag*/) {
  auto progressRing = winrt::ProgressRing();
  return progressRing;
}

bool ActivityIndicatorViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto progressRing = nodeToUpdate->GetView().as<winrt::ProgressRing>();
  if (progressRing == nullptr)
    return true;

  if (propertyName == "animating") {
    if (propertyValue.isBool())
      progressRing.IsActive(propertyValue.asBool());
    else if (propertyValue.isNull())
      progressRing.ClearValue(winrt::ProgressRing::IsActiveProperty());
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace uwp
} // namespace react
