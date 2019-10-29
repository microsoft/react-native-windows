// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "ColorPickerViewManager.h"
#include <Utils/ValueUtils.h>
#include "Unicode.h"

#include <IReactInstance.h>

#include <winrt/Windows.Foundation.Metadata.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>

#include "WinUI2/WinUI2.h"

namespace react {
namespace uwp {

class ColorPickerShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  ColorPickerShadowNode();
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;
};

ColorPickerShadowNode::ColorPickerShadowNode() : Super() {

}

void ColorPickerShadowNode::createView() {
  Super::createView();
}

void ColorPickerShadowNode::updateProperties(const folly::dynamic &&props) {
  for (auto &pair : props.items()) {
  }

  Super::updateProperties(std::move(props));
}


ColorPickerViewManager::ColorPickerViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *ColorPickerViewManager::GetName() const {
  return "RCTColorPicker";
}

folly::dynamic ColorPickerViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();
  return props;
}

facebook::react::ShadowNode *ColorPickerViewManager::createShadow() const {
  return new ColorPickerShadowNode();
}

XamlView ColorPickerViewManager::CreateViewCore(int64_t tag) {
  return WinUI2::CreateColorPicker();
}

} // namespace uwp
} // namespace react
