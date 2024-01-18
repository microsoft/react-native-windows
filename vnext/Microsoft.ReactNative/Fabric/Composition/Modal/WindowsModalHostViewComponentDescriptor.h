/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <glog/logging.h>
#include <react/renderer/core/ConcreteComponentDescriptor.h>
// [Windows
#include "WindowsModalHostViewShadowNode.h"
// ]

namespace facebook::react {

/*
 * Descriptor for <WindowsModalHostView> component.
 */

class WindowsModalHostViewComponentDescriptor final
    : public ConcreteComponentDescriptor<WindowsModalHostViewShadowNode> {
 public:
  using ConcreteComponentDescriptor::ConcreteComponentDescriptor;

  void adopt(ShadowNode &shadowNode) const override {
    auto &layoutableShadowNode = static_cast<YogaLayoutableShadowNode &>(shadowNode);
    auto &stateData =
        static_cast<const WindowsModalHostViewShadowNode::ConcreteState &>(*shadowNode.getState()).getData();

    layoutableShadowNode.setSize(Size{stateData.screenSize.width, stateData.screenSize.height});
    layoutableShadowNode.setPositionType(YGPositionTypeAbsolute);

    ConcreteComponentDescriptor::adopt(shadowNode);
  }
};

} // namespace facebook::react
