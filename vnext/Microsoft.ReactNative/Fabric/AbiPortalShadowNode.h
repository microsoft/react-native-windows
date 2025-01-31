// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/components/rnwcore/EventEmitters.h>
#include <unordered_map>
#include "AbiShadowNode.h"
#include "AbiState.h"
#include "AbiViewProps.h"

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/LayoutContext.h>

namespace Microsoft::ReactNative {

extern const char AbiPortalComponentName[];

class AbiPortalShadowNode final : public facebook::react::ConcreteViewShadowNode<
                                      AbiPortalComponentName,
                                      AbiViewProps,
                                      facebook::react::ViewEventEmitter,
                                      Microsoft::ReactNative::AbiStateData> {
 public:
  using ConcreteViewShadowNode::ConcreteViewShadowNode;

  static facebook::react::ShadowNodeTraits BaseTraits() {
    auto traits = facebook::react::ShadowNode::BaseTraits();
    traits.set(facebook::react::ShadowNodeTraits::Trait::FormsStackingContext);
    traits.set(facebook::react::ShadowNodeTraits::Trait::FormsView);
    traits.set(facebook::react::ShadowNodeTraits::Trait::RootNodeKind);
    traits.set(facebook::react::ShadowNodeTraits::Trait::LeafYogaNode);
    traits.set(facebook::react::ShadowNodeTraits::Trait::MeasurableYogaNode);
    return traits;
  }

  facebook::react::Size measureContent(
      const facebook::react::LayoutContext &layoutContext,
      const facebook::react::LayoutConstraints &layoutConstraints) const override;
  void layout(facebook::react::LayoutContext layoutContext) override;

  void OnClone(const facebook::react::ShadowNode &sourceShadowNode) noexcept;
  void Builder(winrt::Microsoft::ReactNative::IReactViewComponentBuilder builder) noexcept;
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder Builder() const noexcept;
  void Proxy(winrt::Microsoft::ReactNative::ShadowNode handle) noexcept;
  winrt::Microsoft::ReactNative::ShadowNode Proxy() const noexcept;

 private:
  winrt::Microsoft::ReactNative::ShadowNode m_proxy{nullptr};
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder{nullptr};
};

} // namespace Microsoft::ReactNative
