// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ControlViewManager.h>
#include <Views/ShadowNodeBase.h>

#include <Utils/PropertyUtils.h>

namespace react {
namespace uwp {

ControlViewManager::ControlViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

folly::dynamic ControlViewManager::GetNativeProps() const {
  folly::dynamic props = Super::GetNativeProps();
  props.update(folly::dynamic::object("tabIndex", "number"));
  return props;
}
void ControlViewManager::TransferProperties(XamlView oldView, XamlView newView) {
  TransferProperty(oldView, newView, winrt::Control::FontSizeProperty());
  TransferProperty(oldView, newView, winrt::Control::FontFamilyProperty());
  TransferProperty(oldView, newView, winrt::Control::FontWeightProperty());
  TransferProperty(oldView, newView, winrt::Control::FontStyleProperty());
  TransferProperty(oldView, newView, winrt::Control::CharacterSpacingProperty());
  TransferProperty(oldView, newView, winrt::Control::IsTextScaleFactorEnabledProperty());
  TransferProperty(oldView, newView, winrt::Control::BackgroundProperty());
  TransferProperty(oldView, newView, winrt::Control::BorderBrushProperty());
  TransferProperty(oldView, newView, winrt::Control::BorderThicknessProperty());
  TransferProperty(oldView, newView, winrt::Control::PaddingProperty());
  TransferProperty(oldView, newView, winrt::Control::ForegroundProperty());
  TransferProperty(oldView, newView, winrt::Control::TabIndexProperty());
  // Control.CornerRadius is only supported on >= RS5
  if (oldView.try_as<winrt::Windows::UI::Xaml::Controls::IControl7>() &&
      newView.try_as<winrt::Windows::UI::Xaml::Controls::IControl7>()) {
    TransferProperty(oldView, newView, winrt::Control::CornerRadiusProperty());
  }
  Super::TransferProperties(oldView, newView);
}

void ControlViewManager::UpdateProperties(ShadowNodeBase *nodeToUpdate, const folly::dynamic &reactDiffMap) {
  auto control(nodeToUpdate->GetView().as<winrt::Control>());

  bool implementsPadding = nodeToUpdate->ImplementsPadding();
  bool finalizeBorderRadius{false};

  if (control != nullptr) {
    for (const auto &pair : reactDiffMap.items()) {
      const std::string &propertyName = pair.first.getString();
      const folly::dynamic &propertyValue = pair.second;

      if (TryUpdateBackgroundBrush(control, propertyName, propertyValue)) {
        continue;
      } else if (TryUpdateBorderProperties(nodeToUpdate, control, propertyName, propertyValue)) {
        continue;
      } else if (TryUpdateForeground(control, propertyName, propertyValue)) {
        continue;
      } else if (TryUpdateCornerRadiusOnNode(nodeToUpdate, control, propertyName, propertyValue)) {
        finalizeBorderRadius = true;
        continue;
      } else if (implementsPadding && TryUpdatePadding(nodeToUpdate, control, propertyName, propertyValue)) {
        continue;
      } else if (propertyName == "tabIndex") {
        if (propertyValue.isNumber()) {
          auto tabIndex = propertyValue.asDouble();
          if (tabIndex == static_cast<int32_t>(tabIndex)) {
            if (tabIndex < 0) {
              control.IsTabStop(false);
              control.ClearValue(winrt::Control::TabIndexProperty());
            } else {
              control.IsTabStop(true);
              control.TabIndex(static_cast<int32_t>(tabIndex));
            }
          }
        } else if (propertyValue.isNull()) {
          control.ClearValue(winrt::Control::TabIndexProperty());
        }
      }
    }
  }

  Super::UpdateProperties(nodeToUpdate, reactDiffMap);

  if (finalizeBorderRadius && control.try_as<winrt::Windows::UI::Xaml::Controls::IControl7>()) {
    // Control.CornerRadius is only supported on >= RS5, setting borderRadius on Controls have no effect < RS5
    UpdateCornerRadiusOnElement(nodeToUpdate, control);
  }
}

void ControlViewManager::OnViewCreated(XamlView view) {
  // Set the default cornerRadius to 0 for Control: WinUI usually default cornerRadius to 2
  // Only works on >= RS5 becuase Control.CornerRadius is only supported >= RS5
  if (auto control = view.try_as<winrt::Windows::UI::Xaml::Controls::IControl7>()) {
    control.CornerRadius({0});
  }
}

} // namespace uwp
} // namespace react
