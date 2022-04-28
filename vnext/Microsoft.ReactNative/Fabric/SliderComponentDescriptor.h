// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/core/ConcreteComponentDescriptor.h>
#include "SliderShadowNode.h"

namespace facebook::react {

/*
 * Descriptor for <Slider> component.
 */
class SliderComponentDescriptor final : public ConcreteComponentDescriptor<SliderShadowNode> {
 public:
  SliderComponentDescriptor(ComponentDescriptorParameters const &parameters)
      : ConcreteComponentDescriptor(parameters) {}

  void adopt(facebook::react::ShadowNode::Unshared const &shadowNode) const override {
    ConcreteComponentDescriptor::adopt(shadowNode);

    auto sliderShadowNode = std::static_pointer_cast<SliderShadowNode>(shadowNode);

    sliderShadowNode->dirtyLayout();
    sliderShadowNode->enableMeasurement();
  }

 private:
};

} // namespace facebook::react
