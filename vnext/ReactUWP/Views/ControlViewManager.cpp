// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/XamlDirectInstance.h>

namespace react {
namespace uwp {

ControlViewManager::ControlViewManager(
    const std::shared_ptr<IReactInstance> &reactInstance)
    : Super(reactInstance) {}

folly::dynamic ControlViewManager::GetNativeProps() const {
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object("tabIndex", "number"));
  return props;
}

void ControlViewManager::UpdateProperties(
    ShadowNodeBase *nodeToUpdate,
    const folly::dynamic &reactDiffMap) {
  auto control(nodeToUpdate->GetView().as<winrt::Control>());
  const auto controlXD =
      XamlDirectInstance::GetXamlDirect().GetXamlDirectObject(control);

  bool implementsPadding = nodeToUpdate->ImplementsPadding();

  if (controlXD != nullptr) {
    for (const auto &pair : reactDiffMap.items()) {
      const std::string &propertyName = pair.first.getString();
      const folly::dynamic &propertyValue = pair.second;

      if (TryUpdateBackgroundBrush(control, propertyName, propertyValue)) {
        continue;
      } else if (TryUpdateBorderProperties(
                     nodeToUpdate, control, propertyName, propertyValue)) {
        continue;
      } else if (TryUpdateForeground(control, propertyName, propertyValue)) {
        continue;
      } else if (
          implementsPadding &&
          TryUpdatePadding(
              nodeToUpdate, control, propertyName, propertyValue)) {
        continue;
      } else if (propertyName == "tabIndex") {
        if (propertyValue.isNumber()) {
          auto tabIndex = propertyValue.asDouble();
          if (tabIndex == static_cast<int32_t>(tabIndex))
            XamlDirectInstance::GetXamlDirect().SetInt32Property(
                controlXD,
                XD::XamlPropertyIndex::Control_TabIndex,
                static_cast<int32_t>(tabIndex));
        } else if (propertyValue.isNull()) {
            XamlDirectInstance::GetXamlDirect().ClearProperty(
                controlXD,
                XD::XamlPropertyIndex::Control_TabIndex);
        }
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace uwp
} // namespace react
