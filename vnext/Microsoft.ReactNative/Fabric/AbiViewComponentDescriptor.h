// Copyright (c) Microsoft Corporation.
// Licensed under the MIT License.

#pragma once

#include <Fabric/Composition/ReactCompositionViewComponentBuilder.h>
#include <Fabric/WindowsComponentDescriptorRegistry.h>
#include <ReactContext.h>
#include <react/renderer/components/view/ConcreteViewShadowNode.h>
#include <react/renderer/core/ComponentDescriptor.h>
#include "AbiPortalShadowNode.h"
#include "AbiViewProps.h"
#include "AbiViewShadowNode.h"
#include "DynamicReader.h"
#include "winrt/Microsoft.ReactNative.h"

namespace Microsoft::ReactNative {

template <typename ShadowNodeT>
class ConcreteAbiViewComponentDescriptor : public facebook::react::ComponentDescriptor {
 protected:
  using SharedShadowNodeT = std::shared_ptr<const ShadowNodeT>;

 public:
  using ConcreteShadowNode = ShadowNodeT;
  using ConcreteProps = typename ShadowNodeT::ConcreteProps;
  using SharedConcreteProps = typename ShadowNodeT::SharedConcreteProps;
  using ConcreteEventEmitter = typename ShadowNodeT::ConcreteEventEmitter;
  using SharedConcreteEventEmitter = typename ShadowNodeT::SharedConcreteEventEmitter;
  using ConcreteState = typename ShadowNodeT::ConcreteState;
  using ConcreteStateData = typename ShadowNodeT::ConcreteState::Data;

  ConcreteAbiViewComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters)
      : ComponentDescriptor(parameters) {
    auto flavor = std::static_pointer_cast<std::string const>(this->flavor_);
    m_builder = WindowsComponentDescriptorRegistry::FromProperties(
                    parameters.contextContainer->at<winrt::Microsoft::ReactNative::ReactContext>("MSRN.ReactContext")
                        .Properties())
                    ->GetDescriptor(flavor);

    rawPropsParser_.prepare<ConcreteProps>();
  }
  facebook::react::ComponentHandle getComponentHandle() const override {
    return reinterpret_cast<facebook::react::ComponentHandle>(getComponentName());
  }

  facebook::react::ComponentName getComponentName() const override {
    return std::static_pointer_cast<std::string const>(this->flavor_)->c_str();
  }

  facebook::react::ShadowNodeTraits getTraits() const override {
    auto traits = ShadowNodeT::BaseTraits();
    if (winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
            ->MeasureContentHandler()) {
      traits.set(facebook::react::ShadowNodeTraits::LeafYogaNode);
      traits.set(facebook::react::ShadowNodeTraits::MeasurableYogaNode);
    }
    return traits;
  }

  std::shared_ptr<facebook::react::ShadowNode> createShadowNode(
      const facebook::react::ShadowNodeFragment &fragment,
      facebook::react::ShadowNodeFamily::Shared const &family) const override {
    auto shadowNode = std::make_shared<ShadowNodeT>(fragment, family, getTraits());
    shadowNode->Proxy(winrt::make<winrt::Microsoft::ReactNative::implementation::YogaLayoutableShadowNode>(shadowNode));
    winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
        ->CreateShadowNode(shadowNode->Proxy());

    adopt(*shadowNode);
    return shadowNode;
  }

  std::shared_ptr<facebook::react::ShadowNode> cloneShadowNode(
      const facebook::react::ShadowNode &sourceShadowNode,
      const facebook::react::ShadowNodeFragment &fragment) const override {
    auto shadowNode = std::make_shared<ShadowNodeT>(sourceShadowNode, fragment);
    shadowNode->Proxy(winrt::make<winrt::Microsoft::ReactNative::implementation::YogaLayoutableShadowNode>(shadowNode));
    winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
        ->CloneShadowNode(shadowNode->Proxy(), static_cast<const ShadowNodeT &>(sourceShadowNode).Proxy());

    adopt(*shadowNode);
    return shadowNode;
  }

  void appendChild(
      const facebook::react::ShadowNode::Shared &parentShadowNode,
      const facebook::react::ShadowNode::Shared &childShadowNode) const override {
    auto concreteParentShadowNode = std::static_pointer_cast<const ShadowNodeT>(parentShadowNode);
    auto concreteNonConstParentShadowNode = std::const_pointer_cast<ShadowNodeT>(concreteParentShadowNode);
    concreteNonConstParentShadowNode->appendChild(childShadowNode);
  }

  virtual facebook::react::Props::Shared cloneProps(
      const facebook::react::PropsParserContext &context,
      const facebook::react::Props::Shared &props,
      facebook::react::RawProps rawProps) const override {
    // Optimization:
    // Quite often nodes are constructed with default/empty props: the base
    // `props` object is `null` (there no base because it's not cloning) and the
    // `rawProps` is empty. In this case, we can return the default props object
    // of a concrete type entirely bypassing parsing.
    if (!props && rawProps.isEmpty()) {
      return ShadowNodeT::defaultSharedProps();
    }

    if constexpr (facebook::react::RawPropsFilterable<ShadowNodeT>) {
      ShadowNodeT::filterRawProps(rawProps);
    }

    rawProps.parse(rawPropsParser_);

    // Call old-style constructor
    // auto shadowNodeProps = std::make_shared<ShadowNodeT::Props>(context, rawProps, props);
    auto shadowNodeProps = std::make_shared<AbiViewProps>(
        context, props ? static_cast<AbiViewProps const &>(*props) : *ShadowNodeT::defaultSharedProps(), rawProps);
    auto viewProps =
        winrt::make<winrt::Microsoft::ReactNative::implementation::ViewProps>(shadowNodeProps, false /*holdRef*/);
    auto userProps =
        winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
            ->CreateProps(viewProps, props ? static_cast<AbiViewProps const &>(*props).UserProps() : nullptr);
    shadowNodeProps->SetUserProps(userProps, viewProps);

    const auto &dynamic = static_cast<folly::dynamic>(rawProps);
    for (const auto &pair : dynamic.items()) {
      const auto &propName = pair.first.getString();
      auto hash = RAW_PROPS_KEY_HASH(propName);
      shadowNodeProps.get()->setProp(context, hash, propName.c_str(), facebook::react::RawValue(pair.second));
      userProps.SetProp(
          hash, winrt::to_hstring(propName), winrt::make<winrt::Microsoft::ReactNative::DynamicReader>(pair.second));
    }
    return shadowNodeProps;
  }

  virtual facebook::react::State::Shared createInitialState(
      facebook::react::Props::Shared const &props,
      facebook::react::ShadowNodeFamily::Shared const &family) const override {
    if (std::is_same<ConcreteStateData, facebook::react::StateData>::value) {
      // Default case: Returning `null` for nodes that don't use `State`.
      return nullptr;
    }

    return std::make_shared<ConcreteState>(
        std::make_shared<ConcreteStateData const>(
            ConcreteAbiViewComponentDescriptor<ShadowNodeT>::initialStateData(props, family, *this)),
        family);
  }

  virtual facebook::react::State::Shared createState(
      facebook::react::ShadowNodeFamily const &family,
      facebook::react::StateData::Shared const &data) const override {
    if (std::is_same<ConcreteStateData, facebook::react::StateData>::value) {
      // Default case: Returning `null` for nodes that don't use `State`.
      return nullptr;
    }

    react_native_assert(data && "Provided `data` is nullptr.");

    return std::make_shared<ConcreteState const>(
        std::static_pointer_cast<ConcreteStateData const>(data), *family.getMostRecentState());
  }

  facebook::react::ShadowNodeFamily::Shared createFamily(
      facebook::react::ShadowNodeFamilyFragment const &fragment) const override {
    auto eventEmitter = std::make_shared<const ConcreteEventEmitter>(
        std::make_shared<facebook::react::EventTarget>(fragment.instanceHandle, fragment.surfaceId), eventDispatcher_);
    return std::make_shared<facebook::react::ShadowNodeFamily>(
        fragment, std::move(eventEmitter), eventDispatcher_, *this);
  }

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
  virtual void adopt(facebook::react::ShadowNode &shadowNode) const {
    react_native_assert(shadowNode.getComponentHandle() == getComponentHandle());

    auto &abiViewShadowNode = static_cast<AbiViewShadowNode &>(shadowNode);

    abiViewShadowNode.Builder(m_builder);

    if (winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(m_builder)
            ->MeasureContentHandler()) {
      abiViewShadowNode.dirtyLayout();
      abiViewShadowNode.enableMeasurement();
    }
  }

 private:
  static ConcreteStateData initialStateData(
      const facebook::react::Props::Shared &props,
      const facebook::react::ShadowNodeFamily::Shared & /*family*/,
      const facebook::react::ComponentDescriptor &componentDescriptor) noexcept {
    return {winrt::get_self<winrt::Microsoft::ReactNative::Composition::ReactCompositionViewComponentBuilder>(
                static_cast<const ConcreteAbiViewComponentDescriptor<ShadowNodeT> &>(componentDescriptor).m_builder)
                ->InitialStateData(std::static_pointer_cast<AbiViewProps const>(props)->UserProps())};
    return {};
  }

  winrt::Microsoft::ReactNative::IReactViewComponentBuilder m_builder{nullptr};
};

class AbiViewComponentDescriptor : public ConcreteAbiViewComponentDescriptor<AbiViewShadowNode> {
 public:
  AbiViewComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters)
      : ConcreteAbiViewComponentDescriptor<AbiViewShadowNode>(parameters) {}
};

class AbiPortalComponentDescriptor : public ConcreteAbiViewComponentDescriptor<AbiPortalShadowNode> {
 public:
  AbiPortalComponentDescriptor(facebook::react::ComponentDescriptorParameters const &parameters)
      : ConcreteAbiViewComponentDescriptor<AbiPortalShadowNode>(parameters) {}
};

} // namespace Microsoft::ReactNative
