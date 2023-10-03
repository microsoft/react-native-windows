// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/ComponentDescriptor.h>
#include "AbiViewProps.h"
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

extern const char AbiViewComponentName[];

class AbiViewState {
 public:
  AbiViewState() = default;
};

// Need to replace ConcreteShadowNode template usage with a specific impl that can have a different name per type
using AbiViewShadowNode = facebook::react::
    ConcreteViewShadowNode<AbiViewComponentName, AbiViewProps, facebook::react::ViewEventEmitter, AbiViewState>;

class AbiViewComponentDescriptor : public facebook::react::ComponentDescriptor {
  using ShadowNodeT = AbiViewShadowNode;
  using SharedShadowNodeT = std::shared_ptr<const ShadowNodeT>;

 public:
  using ConcreteShadowNode = ShadowNodeT;
  using ConcreteProps = typename ShadowNodeT::ConcreteProps;
  using SharedConcreteProps = typename ShadowNodeT::SharedConcreteProps;
  using ConcreteEventEmitter = typename ShadowNodeT::ConcreteEventEmitter;
  using SharedConcreteEventEmitter = typename ShadowNodeT::SharedConcreteEventEmitter;
  using ConcreteState = typename ShadowNodeT::ConcreteState;
  using ConcreteStateData = typename ShadowNodeT::ConcreteState::Data;

  AbiViewComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters);

  facebook::react::ComponentHandle getComponentHandle() const override;
  facebook::react::ComponentName getComponentName() const override;
  facebook::react::ShadowNodeTraits getTraits() const override;
  facebook::react::ShadowNode::Shared createShadowNode(
      const facebook::react::ShadowNodeFragment &fragment,
      facebook::react::ShadowNodeFamily::Shared const &family) const override;
  facebook::react::ShadowNode::Unshared cloneShadowNode(
      const facebook::react::ShadowNode &sourceShadowNode,
      const facebook::react::ShadowNodeFragment &fragment) const override;

  void appendChild(
      const facebook::react::ShadowNode::Shared &parentShadowNode,
      const facebook::react::ShadowNode::Shared &childShadowNode) const override;
  virtual facebook::react::Props::Shared cloneProps(
      const facebook::react::PropsParserContext &context,
      const facebook::react::Props::Shared &props,
      const facebook::react::RawProps &rawProps) const override;
  virtual facebook::react::State::Shared createInitialState(
      facebook::react::Props::Shared const &props,
      facebook::react::ShadowNodeFamily::Shared const &family) const override;
  virtual facebook::react::State::Shared createState(
      facebook::react::ShadowNodeFamily const &family,
      facebook::react::StateData::Shared const &data) const override;

  facebook::react::ShadowNodeFamily::Shared createFamily(
      facebook::react::ShadowNodeFamilyFragment const &fragment) const override;

  facebook::react::SharedEventEmitter createEventEmitter(
      facebook::react::InstanceHandle::Shared const &instanceHandle) const override;

 protected:
  /*
   * Called immediately after `ShadowNode` is created or cloned.
   *
   * Override this method to pass information from custom `ComponentDescriptor`
   * to new instance of `ShadowNode`.
   *
   * Example usages:
   *   - Inject image manager to `ImageShadowNode` in
   * `ImageComponentDescriptor`.
   *   - Set `ShadowNode`'s size from state in
   * `ModalHostViewComponentDescriptor`.
   */
  virtual void adopt(facebook::react::ShadowNode &shadowNode) const;

 private:
  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder;
};

} // namespace Microsoft::ReactNative
