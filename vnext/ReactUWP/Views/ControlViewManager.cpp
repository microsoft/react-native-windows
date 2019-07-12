// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>
#include <Utils/XamlDirect.h>

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

  bool implementsPadding = nodeToUpdate->ImplementsPadding();

  if (control != nullptr) {
    const auto controlXD =
        GetXamlDirect().GetXamlDirectObject(control);

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
            GetXamlDirect().SetInt32Property(
                controlXD,
                XDPropertyIndex::Control_TabIndex,
                static_cast<int32_t>(tabIndex));
        } else if (propertyValue.isNull()) {
            GetXamlDirect().ClearProperty(
                controlXD,
                XDPropertyIndex::Control_TabIndex);
        }
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);
}

} // namespace uwp
} // namespace react
