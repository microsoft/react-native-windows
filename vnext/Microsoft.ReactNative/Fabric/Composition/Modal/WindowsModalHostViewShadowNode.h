/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/modal/ModalHostViewState.h>
// [Windows
#include <react/components/rnwcore/EventEmitters.h>
#include <react/components/rnwcore/Props.h>
// ]
#include <react/renderer/components/view/ConcreteViewShadowNode.h>

namespace facebook::react {

extern const char WindowsModalHostViewComponentName[];

/*
 * `ShadowNode` for <WindowsModalHostView> component.
 */
class WindowsModalHostViewShadowNode final : public ConcreteViewShadowNode<
                                                 WindowsModalHostViewComponentName,
                                                 ModalHostViewProps,
                                                 ModalHostViewEventEmitter,
                                                 ModalHostViewState> {
 public:
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  static ShadowNodeTraits BaseTraits() {
    auto traits = ConcreteViewShadowNode::BaseTraits();
    traits.set(ShadowNodeTraits::Trait::RootNodeKind);
    return traits;
  }
};

} // namespace facebook::react
