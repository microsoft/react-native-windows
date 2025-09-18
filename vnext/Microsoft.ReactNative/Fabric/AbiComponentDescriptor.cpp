// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#include "pch.h"

#include "AbiComponentDescriptor.h"

#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <ReactContext.h>
#include <type_traits>
#include "DynamicReader.h"

#include <react/featureflags/ReactNativeFeatureFlags.h>
namespace Microsoft::ReactNative {

AbiComponentDescriptor::AbiComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters)
    : ComponentDescriptor(parameters) {
  auto flavor = std::static_pointer_cast<std::string const>(this->flavor_);
  m_builder = WindowsComponentDescriptorRegistry::FromProperties(
                  parameters.contextContainer->at<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext")
                      .Properties())
                  ->GetDescriptor(flavor);

  rawPropsParser_.prepare<ConcreteProps>();
}

facebook::react::ComponentHandle AbiComponentDescriptor::getComponentHandle() const {
  return reinterpret_cast<facebook::react::ComponentHandle>(getComponentName());
}

facebook::react::ComponentName AbiComponentDescriptor::getComponentName() const {
  return std::static_pointer_cast<std::string const>(this->flavor_)->c_str();
}

facebook::react::ShadowNodeTraits AbiComponentDescriptor::getTraits() const {
  auto traits = ShadowNodeT::BaseTraits();
  if (winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
          ->MeasureContentHandler()) {
    traits.set(facebook::react::ShadowNodeTraits::LeafYogaNode);
    traits.set(facebook::react::ShadowNodeTraits::MeasurableYogaNode);
  }
  return traits;
}

std::shared_ptr<facebook::react::ShadowNode> AbiComponentDescriptor::createShadowNode(
    const facebook::react::ShadowNodeFragment &fragment,
    facebook::react::ShadowNodeFamily::Shared const &family) const {
  auto shadowNode = std::make_shared<ShadowNodeT>(fragment, family, getTraits());

  shadowNode->Proxy(winrt::make<winrt::Microsoft::ReactNative::implementation::ShadowNode>(shadowNode));
  winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
      ->CreateShadowNode(shadowNode->Proxy());

  adopt(*shadowNode);
  return shadowNode;
}

std::shared_ptr<facebook::react::ShadowNode> AbiComponentDescriptor::cloneShadowNode(
    const facebook::react::ShadowNode &sourceShadowNode,
    const facebook::react::ShadowNodeFragment &fragment) const {
  auto shadowNode = std::make_shared<ShadowNodeT>(sourceShadowNode, fragment);

  shadowNode->Proxy(winrt::make<winrt::Microsoft::ReactNative::implementation::ShadowNode>(shadowNode));
  winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
      ->CloneShadowNode(shadowNode->Proxy(), static_cast<const ShadowNodeT &>(sourceShadowNode).Proxy());

  adopt(*shadowNode);
  return shadowNode;
}

void AbiComponentDescriptor::appendChild(
    const facebook::react::ShadowNode::Shared &parentShadowNode,
    const facebook::react::ShadowNode::Shared &childShadowNode) const {
  auto concreteParentShadowNode = std::static_pointer_cast<const ShadowNodeT>(parentShadowNode);
  auto concreteNonConstParentShadowNode = std::const_pointer_cast<ShadowNodeT>(concreteParentShadowNode);
  concreteNonConstParentShadowNode->appendChild(childShadowNode);
}

facebook::react::Props::Shared AbiComponentDescriptor::cloneProps(
    const facebook::react::PropsParserContext &context,
    const facebook::react::Props::Shared &props,
    facebook::react::RawProps rawProps) const {
  // Optimization:
  // Quite often nodes are constructed with default/empty props: the base
  // `props` object is `null` (there no base because it's not cloning) and the
  // `rawProps` is empty. In this case, we can return the default props object
  // of a concrete type entirely bypassing parsing.
  if (!props && rawProps.isEmpty()) {
    return ShadowNodeT::defaultSharedProps();
  }

  rawProps.parse(rawPropsParser_);

  // Call old-style constructor
  // auto shadowNodeProps = std::make_shared<ShadowNodeT::Props>(context, rawProps, props);
  auto shadowNodeProps = std::make_shared<winrt::Microsoft::ReactNative::implementation::AbiProps>(
      context,
      props ? static_cast<winrt::Microsoft::ReactNative::implementation::AbiProps const &>(*props)
            : *ShadowNodeT::defaultSharedProps(),
      rawProps);
  auto userProps =
      winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
          ->CreateProps(
              nullptr,
              props ? static_cast<winrt::Microsoft::ReactNative::implementation::AbiProps const &>(*props).UserProps()
                    : nullptr);
  shadowNodeProps->SetUserProps(userProps);

  const auto &dynamic = static_cast<folly::dynamic>(rawProps);
  for (const auto &pair : dynamic.items()) {
    const auto &propName = pair.first.getString();
    auto hash = RAW_PROPS_KEY_HASH(propName);
    shadowNodeProps.get()->setProp(context, hash, propName.c_str(), facebook::react::RawValue(pair.second));
    userProps.SetProp(
        hash, winrt::to_hstring(propName), winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(pair.second));
  }

  return shadowNodeProps;
};

AbiComponentDescriptor::ConcreteStateData AbiComponentDescriptor::initialStateData(
    const facebook::react::Props::Shared &props,
    const facebook::react::ShadowNodeFamily::Shared & /*family*/,
    const facebook::react::ComponentDescriptor &componentDescriptor) noexcept {
  return {winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(
              static_cast<const AbiComponentDescriptor &>(componentDescriptor).m_builder)
              ->InitialStateData(
                  std::static_pointer_cast<winrt::Microsoft::ReactNative::implementation::AbiProps const>(props)
                      ->UserProps())};
}

facebook::react::State::Shared AbiComponentDescriptor::createInitialState(
    facebook::react::Props::Shared const &props,
    facebook::react::ShadowNodeFamily::Shared const &family) const {
  if (std::is_same<ConcreteStateData, facebook::react::StateData>::value) {
    // Default case: Returning `null` for nodes that don't use `State`.
    return nullptr;
  }

  return std::make_shared<ConcreteState>(
      std::make_shared<ConcreteStateData const>(AbiComponentDescriptor::initialStateData(props, family, *this)),
      family);
}

facebook::react::State::Shared AbiComponentDescriptor::createState(
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

facebook::react::ShadowNodeFamily::Shared AbiComponentDescriptor::createFamily(
    facebook::react::ShadowNodeFamilyFragment const &fragment) const {
  auto eventEmitter = std::make_shared<const ConcreteEventEmitter>(
      std::make_shared<facebook::react::EventTarget>(fragment.instanceHandle, fragment.surfaceId), eventDispatcher_);
  return std::make_shared<facebook::react::ShadowNodeFamily>(
      fragment, std::move(eventEmitter), eventDispatcher_, *this);
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
void AbiComponentDescriptor::adopt(facebook::react::ShadowNode &shadowNode) const {
  react_native_assert(shadowNode.getComponentHandle() == getComponentHandle());

  auto &abiShadowNode = static_cast<AbiShadowNode &>(shadowNode);
  abiShadowNode.Builder(m_builder);
}

} // namespace Microsoft::ReactNative
