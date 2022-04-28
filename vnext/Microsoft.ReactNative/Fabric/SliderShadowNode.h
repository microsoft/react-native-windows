// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/rncore/EventEmitters.h>
#include <react/renderer/components/rncore/Props.h>
#include "XamlLayoutableComponentState.h"
#include <react/renderer/components/view/ConcreteViewShadowNode.h>

#include "ConcreteXamlLayoutableShadowNode.h"

namespace facebook::react {

extern const char SliderComponentName[];

/*
 * `ShadowNode` for <Slider> component.
 */
class SliderShadowNode final : public ConcreteXamlLayoutableShadowNode<
                                   SliderComponentName,
                                   SliderProps,
                                   SliderEventEmitter,
                                   XamlLayoutableComponentState> {

 public:
  using ConcreteXamlLayoutableShadowNode::ConcreteXamlLayoutableShadowNode;

};

} // namespace facebook::react
