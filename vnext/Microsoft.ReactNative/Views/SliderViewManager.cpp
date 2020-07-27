// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include <Views/ShadowNodeBase.h>
#include "SliderViewManager.h"

#include <Utils/ValueUtils.h>

#include <IReactInstance.h>
#include <UI.Composition.h>
#include <UI.Xaml.Controls.Primitives.h>
#include <UI.Xaml.Controls.h>

namespace winrt {
using ToggleButton = xaml::Controls::Primitives::ToggleButton;
}

namespace react::uwp {

class SliderShadowNode : public ShadowNodeBase {
  using Super = ShadowNodeBase;

 public:
  SliderShadowNode() = default;
  void createView() override;
  void updateProperties(const folly::dynamic &&props) override;
};

void SliderShadowNode::createView() {
  Super::createView();
}

void SliderShadowNode::updateProperties(const folly::dynamic &&props) {
  m_updating = true;
  Super::updateProperties(std::move(props));
  m_updating = false;
}

SliderViewManager::SliderViewManager(const std::shared_ptr<IReactInstance> &reactInstance) : Super(reactInstance) {}

const char *SliderViewManager::GetName() const {
  return "RCTSlider";
}

folly::dynamic SliderViewManager::GetNativeProps() const {
  auto props = Super::GetNativeProps();

  props.update(folly::dynamic::object("value", "integer")("disabled", "boolean"));

  return props;
}

facebook::react::ShadowNode *SliderViewManager::createShadow() const {
  return new SliderShadowNode();
}

XamlView SliderViewManager::CreateViewCore(int64_t /*tag*/) {
  auto slider = xaml::Controls::Slider();
  return slider;
}

bool SliderViewManager::UpdateProperty(
    ShadowNodeBase *nodeToUpdate,
    const std::string &propertyName,
    const folly::dynamic &propertyValue) {
  auto slider = nodeToUpdate->GetView().as<xaml::Controls::Slider>();
  if (slider == nullptr)
    return true;

  if (propertyName == "disabled") {
    if (propertyValue.isBool())
      slider.IsEnabled(!propertyValue.asBool());
    else if (propertyValue.isNull())
      slider.ClearValue(xaml::Controls::Control::IsEnabledProperty());
  } else if (propertyName == "value") {
    if (propertyValue.isNumber())
      slider.Value(propertyValue.asDouble());
    else if (propertyValue.isNull())
      slider.Value(0);
  } else {
    return Super::UpdateProperty(nodeToUpdate, propertyName, propertyValue);
  }
  return true;
}

} // namespace react::uwp
