/*
 * Copyright (c) Meta Platforms, Inc. and affiliates.
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this source tree.
 */

#pragma once

#include <react/renderer/components/rncore/EventEmitters.h>
#include <react/renderer/components/rncore/Props.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/LayoutContext.h>
#include <react/renderer/imagemanager/primitives.h>
#include "XamlLayoutableComponentState.h"

namespace facebook {
namespace react {

template <
    const char *concreteComponentName,
    typename ViewPropsT = ViewProps,
    typename ViewEventEmitterT = ViewEventEmitter,
    typename... Ts>
class ConcreteXamlLayoutableShadowNode
    : public ConcreteViewShadowNode<concreteComponentName, ViewPropsT, ViewEventEmitterT, Ts...> {
 public:
  using BaseShadowNode = ConcreteViewShadowNode<concreteComponentName, ViewPropsT, ViewEventEmitterT, Ts...>;

  using BaseShadowNode::BaseShadowNode;

  ConcreteXamlLayoutableShadowNode(
      ShadowNodeFragment const &fragment,
      ShadowNodeFamily::Shared const &family,
      ShadowNodeTraits traits)
      : BaseShadowNode(fragment, family, traits) {}

  ConcreteXamlLayoutableShadowNode(ShadowNode const &sourceShadowNode, ShadowNodeFragment const &fragment)
      : BaseShadowNode(sourceShadowNode, fragment) {}

  static XamlLayoutableComponentState initialStateData(
      ShadowNodeFragment const &fragment,
      ShadowNodeFamilyFragment const &familyFragment,
      ComponentDescriptor const &componentDescriptor) {
    return {{}, {}};
  }

  Size measureContent(LayoutContext const &layoutContext, LayoutConstraints const &layoutConstraints) const override {
    constraints_ = layoutConstraints;

    auto const &currentState = BaseShadowNode::getStateData();
    return currentState.getSize();
  }

  void layout(LayoutContext layoutContext) override {
    auto const &currentState = this->getStateData();

    auto constraints = currentState.getLayoutConstraints();
    if (constraints == constraints_) {
      return;
    }

    // Now we are about to mutate the Shadow Node.
    this->ensureUnsealed();
    this->setStateData({constraints_, currentState.getSize()});

    BaseShadowNode::layout(layoutContext);
  }

 private:
  mutable LayoutConstraints constraints_;
  Size size_;
};

} // namespace react
} // namespace facebook
