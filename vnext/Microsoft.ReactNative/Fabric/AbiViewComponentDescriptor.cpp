// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AbiViewComponentDescriptor.h"

#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <ReactContext.h>
#include <react/renderer/components/view/ViewPropsInterpolation.h>
#include "DynamicReader.h"

namespace Microsoft::ReactNative {

extern const char AbiViewComponentName[] = "AbiView";

AbiViewComponentDescriptor::AbiViewComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters)
    : ComponentDescriptor(parameters) {
  auto flavor = std::static_pointer_cast<std::string const>(this->flavor_);
  m_builder = WindowsComponentDescriptorRegistry::FromProperties(
                  parameters.contextContainer->at<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext")
                      .Properties())
                  ->GetDescriptor(flavor);

  rawPropsParser_.prepare<ConcreteProps>();
}

facebook::react::ComponentHandle AbiViewComponentDescriptor::getComponentHandle() const {
  return reinterpret_cast<facebook::react::ComponentHandle>(getComponentName());
}

facebook::react::ComponentName AbiViewComponentDescriptor::getComponentName() const {
  return std::static_pointer_cast<std::string const>(this->flavor_)->c_str();
}

facebook::react::ShadowNodeTraits AbiViewComponentDescriptor::getTraits() const {
  return ShadowNodeT::BaseTraits();
}

facebook::react::ShadowNode::Shared AbiViewComponentDescriptor::createShadowNode(
    const facebook::react::ShadowNodeFragment &fragment,
    facebook::react::ShadowNodeFamily::Shared const &family) const {
  auto shadowNode = std::make_shared<ShadowNodeT>(fragment, family, getTraits());

  adopt(*shadowNode);

  return shadowNode;
}

facebook::react::ShadowNode::Unshared AbiViewComponentDescriptor::cloneShadowNode(
    const facebook::react::ShadowNode &sourceShadowNode,
    const facebook::react::ShadowNodeFragment &fragment) const {
  auto shadowNode = std::make_shared<ShadowNodeT>(sourceShadowNode, fragment);

  adopt(*shadowNode);
  return shadowNode;
}

void AbiViewComponentDescriptor::appendChild(
    const facebook::react::ShadowNode::Shared &parentShadowNode,
    const facebook::react::ShadowNode::Shared &childShadowNode) const {
  auto concreteParentShadowNode = std::static_pointer_cast<const ShadowNodeT>(parentShadowNode);
  auto concreteNonConstParentShadowNode = std::const_pointer_cast<ShadowNodeT>(concreteParentShadowNode);
  concreteNonConstParentShadowNode->appendChild(childShadowNode);
}

facebook::react::Props::Shared AbiViewComponentDescriptor::cloneProps(
    const facebook::react::PropsParserContext &context,
    const facebook::react::Props::Shared &props,
    const facebook::react::RawProps &rawProps) const {
  // Optimization:
  // Quite often nodes are constructed with default/empty props: the base
  // `props` object is `null` (there no base because it's not cloning) and the
  // `rawProps` is empty. In this case, we can return the default props object
  // of a concrete type entirely bypassing parsing.
  if (!props && rawProps.isEmpty()) {
    return ShadowNodeT::defaultSharedProps();
  }

  rawProps.parse(rawPropsParser_, context);

  // Call old-style constructor
  // auto shadowNodeProps = std::make_shared<ShadowNodeT::Props>(context, rawProps, props);
  auto shadowNodeProps = std::make_shared<AbiViewProps>(
      context, props ? static_cast<AbiViewProps const &>(*props) : AbiViewProps(), rawProps);
  auto viewProps = winrt::make<winrt::Microsoft::ReactNative::implementation::UserViewProps>(shadowNodeProps);
  auto userProps =
      winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
          ->CreateProps(viewProps);
  shadowNodeProps->SetUserProps(userProps, viewProps);

  rawProps.iterateOverValues(
      [&](facebook::react::RawPropsPropNameHash hash, const char *propName, facebook::react::RawValue const &fn) {
        shadowNodeProps.get()->setProp(context, hash, propName, fn);
        userProps.SetProp(
            hash,
            winrt::to_hstring(propName),
            winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(folly::dynamic(fn)));
      });

  return shadowNodeProps;
};

facebook::react::State::Shared AbiViewComponentDescriptor::createInitialState(
    facebook::react::Props::Shared const &props,
    facebook::react::ShadowNodeFamily::Shared const &family) const {
  if (std::is_same<ConcreteStateData, facebook::react::StateData>::value) {
    // Default case: Returning `null` for nodes that don't use `State`.
    return nullptr;
  }

  return std::make_shared<ConcreteState>(
      std::make_shared<ConcreteStateData const>(ConcreteShadowNode::initialStateData(props, family, *this)), family);
}

facebook::react::State::Shared AbiViewComponentDescriptor::createState(
    facebook::react::ShadowNodeFamily const &family,
    facebook::react::StateData::Shared const &data) const {
  if (std::is_same<ConcreteStateData, facebook::react::StateData>::value) {
    // Default case: Returning `null` for nodes that don't use `State`.
    return nullptr;
  }

  react_native_assert(data && "Provided `data` is nullptr.");

  return std::make_shared<ConcreteState const>(
      std::static_pointer_cast<ConcreteStateData const>(data), *family.getMostRecentState());
}

facebook::react::ShadowNodeFamily::Shared AbiViewComponentDescriptor::createFamily(
    facebook::react::ShadowNodeFamilyFragment const &fragment) const {
  return std::make_shared<facebook::react::ShadowNodeFamily>(
      facebook::react::ShadowNodeFamilyFragment{fragment.tag, fragment.surfaceId, fragment.instanceHandle},
      eventDispatcher_,
      *this);
}

facebook::react::SharedEventEmitter AbiViewComponentDescriptor::createEventEmitter(
    facebook::react::InstanceHandle::Shared const &instanceHandle) const {
  return std::make_shared<ConcreteEventEmitter const>(
      std::make_shared<facebook::react::EventTarget>(instanceHandle), eventDispatcher_);
}

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
void AbiViewComponentDescriptor::adopt(facebook::react::ShadowNode &shadowNode) const {
  // Default implementation does nothing.
  react_native_assert(shadowNode.getComponentHandle() == getComponentHandle());
}

} // namespace Microsoft::ReactNative
